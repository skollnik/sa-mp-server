#define PIN_LENGTH              (4)
#define PIN_MAX_FAILS           (3)
#define PIN_BLOCK_MINUTES       (30)

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

stock BankPin_ShowSet(playerid, const error[] = "")
{
    new body[192];
    format(body, sizeof(body), C_WHITE "Unesite novi PIN kartice (%d cifre).\n" C_GREY "Bez PIN-a bankomat ne isplacuje novac. Novi PIN odblokira i karticu.%s%s",
        PIN_LENGTH, error[0] ? ("\n\n" C_RED) : (""), error);
    ShowPlayerDialog(playerid, DIALOG_BANK_PIN, DIALOG_STYLE_PASSWORD, SERVER_NAME " - PIN kartice", body, "Sacuvaj", "Nazad");
}

// The counter needs no PIN: there the player shows the ID card. Only the ATM asks for it.
stock BankPin_OpenAtm(playerid)
{
    if (isnull(g_Player[playerid][pBankPin]))
    {
        Msg_Error(playerid, "Vasa kartica nema PIN. Postavite ga na salteru banke (/banka).");
        return;
    }
    new left = g_Player[playerid][pPinBlockedUntil] - gettime();
    if (left > 0)
    {
        Msg_Error(playerid, "Kartica je blokirana jos %d min zbog pogresnog PIN-a. Odmah se odblokira na salteru banke (/banka).", left / 60 + 1);
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
        Msg_Error(playerid, "Tri puta pogresan PIN: kartica je blokirana %d min. Na salteru banke se odmah odblokira.", PIN_BLOCK_MINUTES);
        return 1;
    }
    Account_Save(playerid);

    new error[64];
    format(error, sizeof(error), "Pogresan PIN. Preostalo pokusaja: %d.", PIN_MAX_FAILS - g_Player[playerid][pPinFails]);
    BankPin_ShowAtmInput(playerid, error);
    return 1;
}

forward OnBankPinHashed(playerid, hashid);
public OnBankPinHashed(playerid, hashid)
{
    if (!g_Player[playerid][pPinPending])
    {
        return 1;
    }
    g_Player[playerid][pPinPending] = false;

    bcrypt_get_hash(g_Player[playerid][pBankPin], BCRYPT_HASH_LENGTH);
    g_Player[playerid][pPinFails] = 0;
    g_Player[playerid][pPinBlockedUntil] = 0;

    new query[160];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `accounts` SET `bank_pin` = '%e' WHERE `id` = %d", g_Player[playerid][pBankPin], g_Player[playerid][pDbId]);
    mysql_tquery(g_SQL, query);
    Account_Save(playerid);
    Msg_Success(playerid, "PIN kartice je sacuvan, a kartica je aktivna. Zapamtite PIN: bankomat ga trazi svaki put.");
    return 1;
}

stock bool:BankPin_OnDialogResponse(playerid, dialogid, response, const inputtext[])
{
    if (dialogid == DIALOG_ATM_PIN)
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
        return true;
    }

    if (dialogid == DIALOG_BANK_PIN)
    {
        if (!response)
        {
            Money_ShowBank(playerid, false);
            return true;
        }
        if (g_Player[playerid][pPinPending] || !Point_Require(playerid, POINT_BANK))
        {
            return true;
        }
        if (!BankPin_IsValid(inputtext))
        {
            BankPin_ShowSet(playerid, "PIN ima tacno 4 cifre (samo brojevi).");
            return true;
        }
        g_Player[playerid][pPinPending] = true;
        bcrypt_hash(playerid, "OnBankPinHashed", inputtext, BCRYPT_COST);
        return true;
    }
    return false;
}
