new g_BankTransferName[MAX_PLAYERS][MAX_PLAYER_NAME + 1];

stock Money_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `money_logs` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`account_id` INT UNSIGNED NOT NULL," \
        "`kind` ENUM('cash','bank') NOT NULL," \
        "`amount` INT NOT NULL," \
        "`balance` INT NOT NULL," \
        "`reason` VARCHAR(64) NOT NULL," \
        "`other_id` INT UNSIGNED NULL," \
        "`created_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP," \
        "INDEX `money_account_time` (`account_id`, `created_at`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
}

stock Money_Log(playerid, const kind[], amount, balance, const reason[], other_dbid)
{
    new query[320];
    if (other_dbid > 0)
    {
        mysql_format(g_SQL, query, sizeof(query),
            "INSERT INTO `money_logs` (`account_id`, `kind`, `amount`, `balance`, `reason`, `other_id`) VALUES (%d, '%e', %d, %d, '%e', %d)",
            g_Player[playerid][pDbId], kind, amount, balance, reason, other_dbid);
    }
    else
    {
        mysql_format(g_SQL, query, sizeof(query),
            "INSERT INTO `money_logs` (`account_id`, `kind`, `amount`, `balance`, `reason`) VALUES (%d, '%e', %d, %d, '%e')",
            g_Player[playerid][pDbId], kind, amount, balance, reason);
    }
    mysql_tquery(g_SQL, query);
}

// The only way cash changes. The client's money display is just a mirror of the server value.
stock Money_GiveCash(playerid, amount, const reason[], other_dbid = 0)
{
    g_Player[playerid][pCash] += amount;
    ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, g_Player[playerid][pCash]);
    Money_Log(playerid, "cash", amount, g_Player[playerid][pCash], reason, other_dbid);
}

stock Money_GiveBank(playerid, amount, const reason[], other_dbid = 0)
{
    g_Player[playerid][pBank] += amount;
    Money_Log(playerid, "bank", amount, g_Player[playerid][pBank], reason, other_dbid);
}

stock bool:Money_RequireAccount(playerid)
{
    if (g_Player[playerid][pHasBankAccount])
    {
        return true;
    }
    Msg_Error(playerid, "Nemate racun u banci. Otvara se na salteru banke (/banka), uz licnu kartu.");
    return false;
}

// Pay and prizes go into the account; without one they wait at the bank counter. Returns true when it went into the account.
stock bool:Money_PayToBank(playerid, amount, const reason[])
{
    if (g_Player[playerid][pHasBankAccount])
    {
        Money_GiveBank(playerid, amount, reason);
        return true;
    }
    g_Player[playerid][pHeldPay] += amount;
    new held_reason[64];
    format(held_reason, sizeof(held_reason), "%s (ceka na salteru)", reason);
    Money_Log(playerid, "bank", amount, g_Player[playerid][pHeldPay], held_reason, 0);
    return false;
}

// Takes cash for a purchase; prints the error itself and returns false when the player cannot afford it.
stock bool:Money_Take(playerid, amount, const reason[])
{
    if (g_Player[playerid][pCash] < amount)
    {
        Msg_Error(playerid, "Nemate dovoljno novca (potrebno je $%d, imate $%d).", amount, g_Player[playerid][pCash]);
        return false;
    }
    Money_GiveCash(playerid, -amount, reason);
    return true;
}

// Reads a positive whole amount from dialog input; anything else (letters, zero, negative, overflow) is refused.
stock Money_ParseAmount(const input[])
{
    new amount;
    if (sscanf(input, "d", amount) || amount <= 0)
    {
        return 0;
    }
    return amount;
}

stock Money_ShowBank(playerid, bool:atm)
{
    new title[48];
    format(title, sizeof(title), SERVER_NAME " - %s", atm ? ("Bankomat") : ("Banka"));
    if (atm)
    {
        ShowPlayerDialog(playerid, DIALOG_ATM_MENU, DIALOG_STYLE_LIST, title, "Stanje racuna\nIsplata", "Izaberi", "Zatvori");
    }
    else if (!g_Player[playerid][pHasBankAccount])
    {
        new body[192];
        format(body, sizeof(body), "Otvori racun i karticu ($%d, uz licnu kartu)\nPodigni novac koji vas ceka: " C_GREEN "$%d", BANK_ACCOUNT_PRICE, g_Player[playerid][pHeldPay]);
        ShowPlayerDialog(playerid, DIALOG_BANK_OPEN, DIALOG_STYLE_LIST, title, body, "Izaberi", "Zatvori");
    }
    else
    {
        new body[192];
        format(body, sizeof(body), "Stanje racuna\nUplata\nIsplata\nTransfer na drugi racun\nNovi PIN kartice ($%d, uz licnu kartu; odblokira karticu)", BANK_PIN_RESET_PRICE);
        ShowPlayerDialog(playerid, DIALOG_BANK_MENU, DIALOG_STYLE_LIST, title, body, "Izaberi", "Zatvori");
    }
}

stock Money_ShowAmountInput(playerid, dialogid, const title[], const text[], const error[] = "")
{
    new body[192];
    format(body, sizeof(body), C_WHITE "%s\n\nNa racunu: " C_GREEN "$%d" C_WHITE "  U dzepu: " C_GREEN "$%d%s%s",
        text, g_Player[playerid][pBank], g_Player[playerid][pCash], error[0] ? ("\n\n" C_RED) : (""), error);
    ShowPlayerDialog(playerid, dialogid, DIALOG_STYLE_INPUT, title, body, "Potvrdi", "Nazad");
}

stock Money_Fee(amount)
{
    return max(1, amount * TRANSFER_FEE_PERCENT / 100);
}

// A player who is connecting but not logged in yet may already have their bank balance loaded, so they are refused.
stock Money_FindOnlineByName(const name[], &bool:connecting)
{
    connecting = false;
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (IsPlayerConnected(i) && !strcmp(g_Player[i][pName], name, true))
        {
            if (Account_IsLoggedIn(i))
            {
                return i;
            }
            connecting = true;
        }
    }
    return INVALID_PLAYER_ID;
}

stock Money_Transfer(playerid, amount)
{
    new fee = Money_Fee(amount), total = amount + fee;
    if (amount > g_Player[playerid][pBank] || total > g_Player[playerid][pBank])
    {
        Money_ShowAmountInput(playerid, DIALOG_BANK_TRANSFER_AMOUNT, SERVER_NAME " - Transfer", "Koliko saljete?", "Nemate dovoljno na racunu (uz proviziju).");
        return;
    }

    new bool:connecting, targetid = Money_FindOnlineByName(g_BankTransferName[playerid], connecting);
    if (connecting)
    {
        Msg_Error(playerid, "Taj igrac se upravo prijavljuje, pokusajte za minut.");
        return;
    }

    if (targetid != INVALID_PLAYER_ID && !g_Player[targetid][pHasBankAccount])
    {
        Msg_Error(playerid, "%s nema racun u banci.", g_BankTransferName[playerid]);
        return;
    }

    Money_GiveBank(playerid, -total, "transfer poslat");
    if (targetid != INVALID_PLAYER_ID)
    {
        Money_GiveBank(targetid, amount, "transfer primljen", g_Player[playerid][pDbId]);
        Account_Save(targetid);
        Msg_Info(targetid, "Na vas racun je stigao transfer od $%d (%s).", amount, Account_Name(playerid));
        Money_TransferDone(playerid, amount, fee);
        return;
    }

    // The receiver is offline: the balance is raised in the database, and the money comes back if no such bank account exists.
    new query[192];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `accounts` SET `bank` = `bank` + %d WHERE `name` = '%e' AND `bank_account` = 1 LIMIT 1", amount, g_BankTransferName[playerid]);
    mysql_tquery(g_SQL, query, "OnBankTransferOffline", "dddd", playerid, g_AccountRace[playerid], amount, fee);
}

forward OnBankTransferOffline(playerid, race, amount, fee);
public OnBankTransferOffline(playerid, race, amount, fee)
{
    new bool:delivered = cache_affected_rows() > 0;
    if (race != g_AccountRace[playerid])
    {
        if (!delivered)
        {
            new query[128];
            mysql_format(g_SQL, query, sizeof(query), "UPDATE `accounts` SET `bank` = `bank` + %d WHERE `id` = %d", amount + fee, g_Player[playerid][pDbId]);
            mysql_tquery(g_SQL, query);
        }
        return 1;
    }

    if (!delivered)
    {
        Money_GiveBank(playerid, amount + fee, "transfer vracen");
        Msg_Error(playerid, "Racun sa imenom %s ne postoji. Novac je vracen.", g_BankTransferName[playerid]);
        return 1;
    }

    new query[160];
    mysql_format(g_SQL, query, sizeof(query),
        "INSERT INTO `money_logs` (`account_id`, `kind`, `amount`, `balance`, `reason`, `other_id`) " \
        "SELECT `id`, 'bank', %d, `bank`, 'transfer primljen', %d FROM `accounts` WHERE `name` = '%e' LIMIT 1",
        amount, g_Player[playerid][pDbId], g_BankTransferName[playerid]);
    mysql_tquery(g_SQL, query);
    Money_TransferDone(playerid, amount, fee);
    return 1;
}

stock Money_TransferDone(playerid, amount, fee)
{
    Account_Save(playerid);
    Msg_Success(playerid, "Poslali ste $%d na racun %s (provizija $%d). Na racunu: $%d.", amount, g_BankTransferName[playerid], fee, g_Player[playerid][pBank]);
}

stock bool:Money_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
    switch (dialogid)
    {
        case DIALOG_BANK_MENU, DIALOG_ATM_MENU:
        {
            new bool:atm = dialogid == DIALOG_ATM_MENU;
            if (!response || !Point_Require(playerid, atm ? POINT_ATM : POINT_BANK) || !Money_RequireAccount(playerid))
            {
                return true;
            }

            if (listitem == 0)
            {
                Msg_Info(playerid, "Stanje racuna: $%d. U dzepu: $%d.", g_Player[playerid][pBank], g_Player[playerid][pCash]);
                Money_ShowBank(playerid, atm);
            }
            else if (!atm && listitem == 1)
            {
                Money_ShowAmountInput(playerid, DIALOG_BANK_DEPOSIT, SERVER_NAME " - Uplata", "Koliko uplacujete na racun?");
            }
            else if (listitem == (atm ? 1 : 2))
            {
                new text[64];
                if (atm) format(text, sizeof(text), "Koliko podizete? (najvise $%d odjednom)", ATM_MAX_WITHDRAW);
                else strcopy(text, "Koliko podizete sa racuna?");
                Money_ShowAmountInput(playerid, atm ? DIALOG_ATM_WITHDRAW : DIALOG_BANK_WITHDRAW, SERVER_NAME " - Isplata", text);
            }
            else if (!atm && listitem == 3)
            {
                if (g_Player[playerid][pLevel] < TRANSFER_MIN_LEVEL)
                {
                    Msg_Error(playerid, "Transfer je dostupan od nivoa %d.", TRANSFER_MIN_LEVEL);
                    return true;
                }
                ShowPlayerDialog(playerid, DIALOG_BANK_TRANSFER_NAME, DIALOG_STYLE_INPUT, SERVER_NAME " - Transfer",
                    C_WHITE "Unesite ime vlasnika racuna (Ime_Prezime).", "Dalje", "Nazad");
            }
            else if (!atm && listitem == 4)
            {
                BankPin_ShowReset(playerid);
            }
        }
        case DIALOG_BANK_DEPOSIT:
        {
            if (!response || !Point_Require(playerid, POINT_BANK))
            {
                Money_ShowBank(playerid, false);
                return true;
            }
            new amount = Money_ParseAmount(inputtext);
            if (amount == 0 || amount > g_Player[playerid][pCash])
            {
                Money_ShowAmountInput(playerid, dialogid, SERVER_NAME " - Uplata", "Koliko uplacujete na racun?", "Unesite iznos koji imate u dzepu.");
                return true;
            }
            Money_GiveCash(playerid, -amount, "uplata u banku");
            Money_GiveBank(playerid, amount, "uplata u banku");
            Account_Save(playerid);
            Msg_Success(playerid, "Uplatili ste $%d. Na racunu: $%d.", amount, g_Player[playerid][pBank]);
        }
        case DIALOG_BANK_WITHDRAW, DIALOG_ATM_WITHDRAW:
        {
            new bool:atm = dialogid == DIALOG_ATM_WITHDRAW;
            if (!response || !Point_Require(playerid, atm ? POINT_ATM : POINT_BANK))
            {
                Money_ShowBank(playerid, atm);
                return true;
            }
            new amount = Money_ParseAmount(inputtext);
            if (amount == 0 || amount > g_Player[playerid][pBank] || (atm && amount > ATM_MAX_WITHDRAW))
            {
                Money_ShowAmountInput(playerid, dialogid, SERVER_NAME " - Isplata", "Koliko podizete?", "Unesite iznos koji imate na racunu (bankomat do $10000).");
                return true;
            }
            Money_GiveBank(playerid, -amount, atm ? ("isplata na bankomatu") : ("isplata u banci"));
            Money_GiveCash(playerid, amount, atm ? ("isplata na bankomatu") : ("isplata u banci"));
            Account_Save(playerid);
            Msg_Success(playerid, "Podigli ste $%d. Na racunu: $%d.", amount, g_Player[playerid][pBank]);
        }
        case DIALOG_BANK_TRANSFER_NAME:
        {
            if (!response)
            {
                Money_ShowBank(playerid, false);
                return true;
            }
            if (!Account_IsValidRpName(inputtext) || !strcmp(inputtext, g_Player[playerid][pName], true))
            {
                ShowPlayerDialog(playerid, DIALOG_BANK_TRANSFER_NAME, DIALOG_STYLE_INPUT, SERVER_NAME " - Transfer",
                    C_WHITE "Unesite ime vlasnika racuna (Ime_Prezime).\n\n" C_RED "Neispravno ime ili je to vas racun.", "Dalje", "Nazad");
                return true;
            }
            strcopy(g_BankTransferName[playerid], inputtext, MAX_PLAYER_NAME + 1);

            new text[96];
            format(text, sizeof(text), "Koliko saljete na racun %s? Provizija je %d%%.", inputtext, TRANSFER_FEE_PERCENT);
            Money_ShowAmountInput(playerid, DIALOG_BANK_TRANSFER_AMOUNT, SERVER_NAME " - Transfer", text);
        }
        case DIALOG_BANK_TRANSFER_AMOUNT:
        {
            if (!response || !Point_Require(playerid, POINT_BANK) || g_Player[playerid][pLevel] < TRANSFER_MIN_LEVEL)
            {
                return true;
            }
            new amount = Money_ParseAmount(inputtext);
            if (amount == 0)
            {
                Money_ShowAmountInput(playerid, dialogid, SERVER_NAME " - Transfer", "Koliko saljete?", "Unesite ispravan iznos.");
                return true;
            }
            Money_Transfer(playerid, amount);
        }
        default:
        {
            return false;
        }
    }
    return true;
}
