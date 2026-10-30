#define PIN_LENGTH              (4)
#define PIN_MAX_FAILS           (3)
#define PIN_BLOCK_MINUTES       (30)

// Why a new PIN is being hashed; the fee is charged only once the hash is ready, so a player who leaves meanwhile pays nothing.
enum E_PIN_ISSUE
{
    PIN_ISSUE_OPEN,
    PIN_ISSUE_RESET
}

new E_PIN_ISSUE:g_PinIssue[MAX_PLAYERS];
new g_PinIssued[MAX_PLAYERS][PIN_LENGTH + 1];

static stock bool:BankPin_IsValid(const input[])
{
    if (strlen(input) != PIN_LENGTH)
    {
        return false;
    }
    for (new i = 0; i < PIN_LENGTH; i++)
    {
        if (input[i] < '0' || input[i] > '9')
        {
            return false;
        }
    }
    return true;
}

static stock BankPin_ShowAtmInput(playerid, const error[] = "")
{
    new body[160];
    format(body, sizeof(body), C_WHITE "Unesite PIN kartice (%d cifre).%s%s", PIN_LENGTH, error[0] ? ("\n\n" C_RED) : (""), error);
    ShowPlayerDialog(playerid, DIALOG_ATM_PIN, DIALOG_STYLE_PASSWORD, SERVER_NAME " - Bankomat", body, "Potvrdi", "Odustani");
}

// The bank knows the player by the ID card, so both opening an account and a new PIN need it.
static stock bool:BankPin_RequireIdCard(playerid)
{
    if (g_Player[playerid][pHasIdCard])
    {
        return true;
    }
    Msg_Error(playerid, "Treba vam licna karta. Vadi se u opstini (/opstina).");
    return false;
}

// The PIN is random and shown once; the player cannot pick it.
static stock BankPin_Issue(playerid, E_PIN_ISSUE:issue)
{
    if (g_Player[playerid][pPinPending])
    {
        return;
    }
    format(g_PinIssued[playerid], sizeof(g_PinIssued[]), "%04d", random(10000));
    g_PinIssue[playerid] = issue;
    g_Player[playerid][pPinPending] = true;
    bcrypt_hash(playerid, "OnBankPinHashed", g_PinIssued[playerid], BCRYPT_COST);
}

stock BankPin_ShowReset(playerid)
{
    new body[256];
    format(body, sizeof(body), C_WHITE "Banka izdaje novi PIN kartice za " C_GREEN "$%d" C_WHITE " (uz licnu kartu).\n\n" C_GREY "Stari PIN prestaje da vazi, a blokirana kartica se odmah odblokira.",
        BANK_PIN_RESET_PRICE);
    ShowPlayerDialog(playerid, DIALOG_BANK_PIN, DIALOG_STYLE_MSGBOX, SERVER_NAME " - Novi PIN", body, "Potvrdi", "Nazad");
}

// The counter needs no PIN: there the player shows the ID card. Only the ATM asks for it.
stock BankPin_OpenAtm(playerid)
{
    if (!Money_RequireAccount(playerid))
    {
        return;
    }
    if (isnull(g_Player[playerid][pBankPin]))
    {
        Msg_Error(playerid, "Vasa kartica nema PIN. Novi PIN se izdaje na salteru banke (/banka).");
        return;
    }
    new left = g_Player[playerid][pPinBlockedUntil] - gettime();
    if (left > 0)
    {
        Msg_Error(playerid, "Kartica je blokirana jos %d min zbog pogresnog PIN-a. Na salteru banke se odmah odblokira novim PIN-om.", left / 60 + 1);
        return;
    }
    BankPin_ShowAtmInput(playerid);
}

forward OnAtmPinVerify(playerid, bool:success);
public OnAtmPinVerify(playerid, bool:success)
{
    // pPinPending is wiped on connect, so a late answer for a player who left is dropped.
    if (!g_Player[playerid][pPinPending])
    {
        return 1;
    }
    g_Player[playerid][pPinPending] = false;

    if (success)
    {
        g_Player[playerid][pPinFails] = 0;
        // The player may have walked away while the PIN was being checked.
        if (Point_Require(playerid, POINT_ATM))
        {
            Money_ShowBank(playerid, true);
        }
        return 1;
    }

    // Fails are saved right away, so leaving the game does not reset the count.
    if (++g_Player[playerid][pPinFails] >= PIN_MAX_FAILS)
    {
        g_Player[playerid][pPinFails] = 0;
        g_Player[playerid][pPinBlockedUntil] = gettime() + PIN_BLOCK_MINUTES * 60;
        Account_Save(playerid);
        Msg_Error(playerid, "Tri puta pogresan PIN: kartica je blokirana %d min. Na salteru banke se odmah odblokira novim PIN-om.", PIN_BLOCK_MINUTES);
        return 1;
    }
    Account_Save(playerid);

    new error[64];
    format(error, sizeof(error), "Pogresan PIN. Preostalo pokusaja: %d.", PIN_MAX_FAILS - g_Player[playerid][pPinFails]);
    BankPin_ShowAtmInput(playerid, error);
    return 1;
}

static stock bool:BankPin_ChargeIssue(playerid)
{
    if (g_PinIssue[playerid] == PIN_ISSUE_RESET)
    {
        return Money_Take(playerid, BANK_PIN_RESET_PRICE, "novi PIN kartice");
    }
    if (g_Player[playerid][pHasBankAccount])
    {
        return false;
    }
    g_Player[playerid][pHasBankAccount] = true;
    // Whatever waited at the counter goes straight into the new account.
    if (g_Player[playerid][pHeldPay] > 0)
    {
        Money_GiveBank(playerid, g_Player[playerid][pHeldPay], "novac koji je cekao u banci");
        g_Player[playerid][pHeldPay] = 0;
    }
    return true;
}

forward OnBankPinHashed(playerid, hashid);
public OnBankPinHashed(playerid, hashid)
{
    if (!g_Player[playerid][pPinPending])
    {
        return 1;
    }
    g_Player[playerid][pPinPending] = false;
    if (!BankPin_ChargeIssue(playerid))
    {
        return 1;
    }

    bcrypt_get_hash(g_Player[playerid][pBankPin], BCRYPT_HASH_LENGTH);
    g_Player[playerid][pPinFails] = 0;
    g_Player[playerid][pPinBlockedUntil] = 0;

    new query[160];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `accounts` SET `bank_pin` = '%e' WHERE `id` = %d", g_Player[playerid][pBankPin], g_Player[playerid][pDbId]);
    mysql_tquery(g_SQL, query);
    Account_Save(playerid);

    new body[256];
    format(body, sizeof(body), C_WHITE "%s\n\nPIN vase kartice je: " C_GREEN "%s\n\n" C_GREY "Zapamtite ga: bankomat ga trazi svaki put. Zaboravljen PIN se menja ovde za $%d.",
        g_PinIssue[playerid] == PIN_ISSUE_OPEN ? ("Otvorili ste racun i dobili karticu.") : ("Izdat je novi PIN, kartica je aktivna."),
        g_PinIssued[playerid], BANK_PIN_RESET_PRICE);
    ShowPlayerDialog(playerid, DIALOG_BANK_PIN_SHOWN, DIALOG_STYLE_MSGBOX, SERVER_NAME " - Kartica", body, "U redu", "");
    if (g_PinIssue[playerid] == PIN_ISSUE_OPEN)
    {
        Msg_Success(playerid, "Racun je otvoren. Na racunu: $%d. Plata od sada stize na racun.", g_Player[playerid][pBank]);
    }
    return 1;
}

static stock Bank_CollectHeld(playerid)
{
    new amount = g_Player[playerid][pHeldPay];
    if (amount <= 0)
    {
        Msg_Info(playerid, "U banci vas ne ceka nikakav novac.");
        return;
    }
    g_Player[playerid][pHeldPay] = 0;
    Money_GiveCash(playerid, amount, "podignut novac sa saltera");
    Account_Save(playerid);
    Msg_Success(playerid, "Podigli ste $%d. Sa racunom bi plata stizala sama (/banka).", amount);
}

stock bool:BankPin_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
    switch (dialogid)
    {
        case DIALOG_ATM_PIN:
        {
            if (!response || g_Player[playerid][pPinPending] || !Point_Require(playerid, POINT_ATM))
            {
                return true;
            }
            // A typo with letters is not counted as a wrong PIN.
            if (!BankPin_IsValid(inputtext))
            {
                BankPin_ShowAtmInput(playerid, "PIN ima tacno 4 cifre.");
                return true;
            }
            g_Player[playerid][pPinPending] = true;
            bcrypt_verify(playerid, "OnAtmPinVerify", inputtext, g_Player[playerid][pBankPin]);
        }
        case DIALOG_BANK_PIN:
        {
            if (!response)
            {
                Money_ShowBank(playerid, false);
                return true;
            }
            if (!Point_Require(playerid, POINT_BANK) || !Money_RequireAccount(playerid) || !BankPin_RequireIdCard(playerid))
            {
                return true;
            }
            if (g_Player[playerid][pCash] < BANK_PIN_RESET_PRICE)
            {
                Msg_Error(playerid, "Novi PIN kosta $%d.", BANK_PIN_RESET_PRICE);
                return true;
            }
            BankPin_Issue(playerid, PIN_ISSUE_RESET);
        }
        case DIALOG_BANK_OPEN:
        {
            if (!response || !Point_Require(playerid, POINT_BANK) || g_Player[playerid][pHasBankAccount] || !BankPin_RequireIdCard(playerid))
            {
                return true;
            }
            if (listitem == 1)
            {
                Bank_CollectHeld(playerid);
                return true;
            }
            BankPin_Issue(playerid, PIN_ISSUE_OPEN);
        }
        case DIALOG_BANK_PIN_SHOWN:
        {
        }
        default:
        {
            return false;
        }
    }
    return true;
}
