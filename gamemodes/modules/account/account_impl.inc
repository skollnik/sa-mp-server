forward bool:Account_IsOwnerName(const name[]);
forward bool:Account_IsValidRpName(const name[]);
forward bool:Account_CheckFreeSlot(playerid);

stock Account_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `accounts` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`name` VARCHAR(24) NOT NULL UNIQUE," \
        "`password` CHAR(60) NOT NULL," \
        "`admin_level` TINYINT UNSIGNED NOT NULL DEFAULT 0," \
        "`gender` TINYINT UNSIGNED NOT NULL DEFAULT 0," \
        "`age` TINYINT UNSIGNED NOT NULL DEFAULT 18," \
        "`skin` SMALLINT UNSIGNED NOT NULL DEFAULT 0," \
        "`level` INT UNSIGNED NOT NULL DEFAULT 1," \
        "`respect` INT UNSIGNED NOT NULL DEFAULT 0," \
        "`cash` INT NOT NULL DEFAULT 0," \
        "`bank` INT NOT NULL DEFAULT 0," \
        "`pos_x` FLOAT NOT NULL DEFAULT 0," \
        "`pos_y` FLOAT NOT NULL DEFAULT 0," \
        "`pos_z` FLOAT NOT NULL DEFAULT 0," \
        "`pos_a` FLOAT NOT NULL DEFAULT 0," \
        "`interior` INT NOT NULL DEFAULT 0," \
        "`virtual_world` INT NOT NULL DEFAULT 0," \
        "`health` FLOAT NOT NULL DEFAULT 100," \
        "`armour` FLOAT NOT NULL DEFAULT 0," \
        "`played_seconds` INT UNSIGNED NOT NULL DEFAULT 0," \
        "`register_ip` VARCHAR(16) NOT NULL DEFAULT ''," \
        "`last_ip` VARCHAR(16) NOT NULL DEFAULT ''," \
        "`registered_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP," \
        "`last_login` DATETIME NULL," \
        "`helper_level` TINYINT UNSIGNED NOT NULL DEFAULT 0," \
        "`tog_ooc` TINYINT(1) NOT NULL DEFAULT 0," \
        "`tog_pm` TINYINT(1) NOT NULL DEFAULT 0," \
        "`tog_newbie` TINYINT(1) NOT NULL DEFAULT 0," \
        "`mute_until` INT NOT NULL DEFAULT 0," \
        "`newbie_mute_until` INT NOT NULL DEFAULT 0," \
        "`jail_seconds` INT NOT NULL DEFAULT 0," \
        "`jail_reason` VARCHAR(96) NOT NULL DEFAULT ''," \
        "`vip_level` TINYINT UNSIGNED NOT NULL DEFAULT 0," \
        "`vip_until` INT NOT NULL DEFAULT 0" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");

    // Columns added after the table first shipped; existing databases get them here.
    mysql_tquery(g_SQL, "ALTER TABLE `accounts` ADD COLUMN IF NOT EXISTS `helper_level` TINYINT UNSIGNED NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `accounts` ADD COLUMN IF NOT EXISTS `tog_ooc` TINYINT(1) NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `accounts` ADD COLUMN IF NOT EXISTS `tog_pm` TINYINT(1) NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `accounts` ADD COLUMN IF NOT EXISTS `tog_newbie` TINYINT(1) NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `accounts` ADD COLUMN IF NOT EXISTS `mute_until` INT NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `accounts` ADD COLUMN IF NOT EXISTS `newbie_mute_until` INT NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `accounts` ADD COLUMN IF NOT EXISTS `jail_seconds` INT NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `accounts` ADD COLUMN IF NOT EXISTS `jail_reason` VARCHAR(96) NOT NULL DEFAULT ''");
    mysql_tquery(g_SQL, "ALTER TABLE `accounts` ADD COLUMN IF NOT EXISTS `vip_level` TINYINT UNSIGNED NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `accounts` ADD COLUMN IF NOT EXISTS `vip_until` INT NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `accounts` ADD COLUMN IF NOT EXISTS `channel_bits` INT NOT NULL DEFAULT 0");

    // The old per-chat toggle columns are folded into channel_bits (OOC = bit 0, PM = bit 1, newbie = bit 2) and cleared.
    mysql_tquery(g_SQL,
        "UPDATE `accounts` SET `channel_bits` = `channel_bits` | `tog_ooc` | (`tog_pm` << 1) | (`tog_newbie` << 2), " \
        "`tog_ooc` = 0, `tog_pm` = 0, `tog_newbie` = 0 WHERE `tog_ooc` = 1 OR `tog_pm` = 1 OR `tog_newbie` = 1");

    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `login_attempts` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`account_id` INT UNSIGNED NOT NULL," \
        "`ip` VARCHAR(16) NOT NULL," \
        "`success` TINYINT(1) NOT NULL," \
        "`attempted_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP," \
        "INDEX `account_time` (`account_id`, `attempted_at`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");

    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `name_exemptions` (" \
        "`name` VARCHAR(24) NOT NULL PRIMARY KEY," \
        "`granted_by` VARCHAR(24) NOT NULL," \
        "`granted_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");

    g_AutosaveTimer = SetTimer("Account_Autosave", AUTOSAVE_INTERVAL_MS, true);
}

stock Account_Exit()
{
    KillTimer(g_AutosaveTimer);

    // Saved synchronously because the DB connection is closed right after this.
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        Account_Save(i, true);
        g_Player[i][pState] = ACCOUNT_STATE_NONE;
    }
}

stock Account_OnPlayerConnect(playerid)
{
    static const empty[E_PLAYER_DATA];
    g_Player[playerid] = empty;
    g_PendingPassword[playerid][0] = '\0';
    g_AccountRace[playerid]++;

    GetPlayerName(playerid, g_Player[playerid][pName], MAX_PLAYER_NAME + 1);

    Chat_Clear(playerid, 20);

    // The ban check runs first; it calls Account_CheckName when the player is allowed in.
    Staff_CheckBan(playerid);
}

stock Account_CheckName(playerid)
{
    if (Account_IsOwnerName(g_Player[playerid][pName]) || Account_IsValidRpName(g_Player[playerid][pName]))
    {
        Account_Lookup(playerid);
        return;
    }

    new query[128];
    mysql_format(g_SQL, query, sizeof(query), "SELECT 1 FROM `name_exemptions` WHERE `name` = '%e' LIMIT 1", g_Player[playerid][pName]);
    mysql_tquery(g_SQL, query, "OnAccountExemptionCheck", "dd", playerid, g_AccountRace[playerid]);
}

stock Account_OnPlayerDisconnect(playerid)
{
    Account_Save(playerid);

    if (g_Player[playerid][pLoginTimer])
    {
        KillTimer(g_Player[playerid][pLoginTimer]);
    }

    g_Player[playerid][pState] = ACCOUNT_STATE_NONE;
    g_PendingPassword[playerid][0] = '\0';
    g_AccountRace[playerid]++;
}

stock Account_OnPlayerRequestClass(playerid)
{
    if (g_Player[playerid][pState] != ACCOUNT_STATE_LOGGED_IN)
    {
        TogglePlayerSpectating(playerid, true);
        InterpolateCameraPos(playerid,
            LOGIN_CAMERA_FROM_X, LOGIN_CAMERA_FROM_Y, LOGIN_CAMERA_Z,
            LOGIN_CAMERA_TO_X, LOGIN_CAMERA_TO_Y, LOGIN_CAMERA_Z,
            LOGIN_CAMERA_TIME_MS, CAMERA_MOVE);
        InterpolateCameraLookAt(playerid,
            LOGIN_LOOKAT_FROM_X, LOGIN_LOOKAT_Y, LOGIN_LOOKAT_Z,
            LOGIN_LOOKAT_TO_X, LOGIN_LOOKAT_Y, LOGIN_LOOKAT_Z,
            LOGIN_CAMERA_TIME_MS, CAMERA_MOVE);
    }
}

// Underscore replaced by a space, as names are shown in chat.
stock Account_GetDisplayName(playerid, dest[], size = sizeof(dest))
{
    strcopy(dest, g_Player[playerid][pName], size);
    for (new i = 0; dest[i] != '\0'; i++)
    {
        if (dest[i] == '_')
        {
            dest[i] = ' ';
        }
    }
}

// Same display name, returned by value so it can go straight into format arguments.
stock Account_Name(playerid)
{
    new name[MAX_PLAYER_NAME + 1];
    Account_GetDisplayName(playerid, name);
    return name;
}

stock Account_OnPlayerSpawn(playerid)
{
    if (g_Player[playerid][pState] != ACCOUNT_STATE_LOGGED_IN)
    {
        Kick(playerid);
        return;
    }

    SetPlayerSkin(playerid, g_Player[playerid][pSkin]);
    SetPlayerScore(playerid, g_Player[playerid][pLevel]);
    ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, g_Player[playerid][pCash]);

    if (g_Player[playerid][pFirstSpawn])
    {
        g_Player[playerid][pFirstSpawn] = false;
        SetPlayerInterior(playerid, g_Player[playerid][pInterior]);
        SetPlayerVirtualWorld(playerid, g_Player[playerid][pVirtualWorld]);
        SetPlayerHealth(playerid, g_Player[playerid][pHealth]);
        AC_SetPlayerArmour(playerid, g_Player[playerid][pArmour]);
    }
}

stock Account_OnPlayerTakeDamage(playerid)
{
    g_Player[playerid][pLastDamage] = gettime();
}

// Used to keep convenience features (VIP commands, doors) from serving as an escape from a fight.
stock Account_SecondsSinceDamage(playerid)
{
    return gettime() - g_Player[playerid][pLastDamage];
}

stock bool:Account_IsLoggedIn(playerid)
{
    return g_Player[playerid][pState] == ACCOUNT_STATE_LOGGED_IN;
}

// Cash lives on the server; the client's money display is only a mirror of it.
stock Account_GiveCash(playerid, amount)
{
    g_Player[playerid][pCash] += amount;
    ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, g_Player[playerid][pCash]);
}

stock bool:Account_IsOwnerName(const name[])
{
    return strcmp(name, OWNER_NAME, true) == 0;
}

// Firstname_Lastname: exactly one underscore, each part at least 2 letters,
// first letter of each part uppercase, the rest lowercase.
stock bool:Account_IsValidRpName(const name[])
{
    new len = strlen(name),
        underscore = -1;

    if (len < 5 || len > MAX_PLAYER_NAME)
    {
        return false;
    }

    for (new i = 0; i < len; i++)
    {
        new c = name[i];

        if (c == '_')
        {
            if (underscore != -1)
            {
                return false;
            }
            underscore = i;
            continue;
        }

        new bool:part_start = (i == 0 || i == underscore + 1);

        if (part_start)
        {
            if (c < 'A' || c > 'Z')
            {
                return false;
            }
        }
        else if (c < 'a' || c > 'z')
        {
            return false;
        }
    }

    return underscore >= 2 && (len - underscore - 1) >= 2;
}

forward OnAccountExemptionCheck(playerid, race);
public OnAccountExemptionCheck(playerid, race)
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }

    if (cache_num_rows() > 0)
    {
        Account_Lookup(playerid);
        return 1;
    }

    ShowPlayerDialog(playerid, DIALOG_INVALID_NAME, DIALOG_STYLE_MSGBOX, SERVER_NAME " - Neispravno ime",
        C_WHITE "Vase ime nije u ispravnom formatu.\n\n" \
        "Na ovom serveru ime mora biti u formatu " C_SERVER "Ime_Prezime" C_WHITE ", na primer " C_SERVER "Marko_Petrovic" C_WHITE ".\n" \
        "- samo slova i jedna donja crta izmedju imena i prezimena\n" \
        "- ime i prezime pocinju velikim slovom\n\n" \
        "Promenite ime u SA-MP klijentu i ponovo se povezite.",
        "U redu", "");
    Account_StartTimeout(playerid, LOGIN_TIMEOUT_MS);
    return 1;
}

stock Account_Lookup(playerid)
{
    // The lockout is per account AND per IP, so someone guessing a password cannot lock the real owner out.
    new ip[16], query[768];
    GetPlayerIp(playerid, ip, sizeof(ip));
    mysql_format(g_SQL, query, sizeof(query),
        "SELECT a.*, IFNULL(DATE_FORMAT(a.`last_login`, '%%d.%%m.%%Y. u %%H:%%i'), '') AS `last_login_text`, " \
        "IFNULL((SELECT CEIL(TIMESTAMPDIFF(SECOND, NOW(), MAX(l.`attempted_at`) + INTERVAL %d MINUTE) / 60) " \
        "FROM `login_attempts` l WHERE l.`account_id` = a.`id` AND l.`ip` = '%e' AND l.`success` = 0 " \
        "AND l.`attempted_at` > NOW() - INTERVAL %d MINUTE HAVING COUNT(*) >= %d), 0) AS `lock_minutes` " \
        "FROM `accounts` a WHERE a.`name` = '%e' LIMIT 1",
        LOCKOUT_MINUTES, ip, LOCKOUT_WINDOW_MINUTES, LOCKOUT_MAX_FAILURES, g_Player[playerid][pName]);
    mysql_tquery(g_SQL, query, "OnAccountLookup", "dd", playerid, g_AccountRace[playerid]);
}

forward OnAccountLookup(playerid, race);
public OnAccountLookup(playerid, race)
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }

    if (cache_num_rows() == 0)
    {
        if (!Account_CheckFreeSlot(playerid))
        {
            return 1;
        }
        g_Player[playerid][pState] = ACCOUNT_STATE_REGISTER;
        Account_ShowRegisterPassword(playerid, "");
        Account_StartTimeout(playerid, REGISTER_TIMEOUT_MS);
        return 1;
    }

    new lock_minutes;
    cache_get_value_name_int(0, "lock_minutes", lock_minutes);
    if (lock_minutes > 0)
    {
        Msg_Error(playerid, "Previse pogresnih lozinki sa vase adrese. Pokusajte ponovo za %d min.", lock_minutes);
        KickWithDelay(playerid);
        return 1;
    }

    cache_get_value_name(0, "last_login_text", g_Player[playerid][pLastLoginText], 24);
    cache_get_value_name(0, "last_ip", g_Player[playerid][pLastIp], 16);
    cache_get_value_name_int(0, "id", g_Player[playerid][pDbId]);
    cache_get_value_name(0, "password", g_Player[playerid][pPasswordHash], BCRYPT_HASH_LENGTH);
    cache_get_value_name_int(0, "admin_level", g_Player[playerid][pAdminLevel]);
    cache_get_value_name_int(0, "helper_level", g_Player[playerid][pHelperLevel]);
    cache_get_value_name_int(0, "channel_bits", g_Player[playerid][pChannelBits]);
    cache_get_value_name_int(0, "mute_until", g_Player[playerid][pMuteUntil]);
    cache_get_value_name_int(0, "newbie_mute_until", g_Player[playerid][pNewbieMuteUntil]);
    cache_get_value_name_int(0, "jail_seconds", g_Player[playerid][pJailSeconds]);
    cache_get_value_name(0, "jail_reason", g_Player[playerid][pJailReason], 96);
    cache_get_value_name_int(0, "vip_level", g_Player[playerid][pVipLevel]);
    cache_get_value_name_int(0, "vip_until", g_Player[playerid][pVipUntil]);
    cache_get_value_name_int(0, "gender", g_Player[playerid][pGender]);
    cache_get_value_name_int(0, "age", g_Player[playerid][pAge]);
    cache_get_value_name_int(0, "skin", g_Player[playerid][pSkin]);
    cache_get_value_name_int(0, "level", g_Player[playerid][pLevel]);
    cache_get_value_name_int(0, "respect", g_Player[playerid][pRespect]);
    cache_get_value_name_int(0, "cash", g_Player[playerid][pCash]);
    cache_get_value_name_int(0, "bank", g_Player[playerid][pBank]);
    cache_get_value_name_float(0, "pos_x", g_Player[playerid][pPosX]);
    cache_get_value_name_float(0, "pos_y", g_Player[playerid][pPosY]);
    cache_get_value_name_float(0, "pos_z", g_Player[playerid][pPosZ]);
    cache_get_value_name_float(0, "pos_a", g_Player[playerid][pPosA]);
    cache_get_value_name_int(0, "interior", g_Player[playerid][pInterior]);
    cache_get_value_name_int(0, "virtual_world", g_Player[playerid][pVirtualWorld]);
    cache_get_value_name_float(0, "health", g_Player[playerid][pHealth]);
    cache_get_value_name_float(0, "armour", g_Player[playerid][pArmour]);
    cache_get_value_name_int(0, "played_seconds", g_Player[playerid][pPlayedSeconds]);

    if (!Account_CheckFreeSlot(playerid))
    {
        return 1;
    }

    g_Player[playerid][pState] = ACCOUNT_STATE_LOGIN;
    Account_ShowLogin(playerid, "");
    Account_StartTimeout(playerid, LOGIN_TIMEOUT_MS);
    return 1;
}

stock Account_ShowLogin(playerid, const error[])
{
    new body[256];
    format(body, sizeof(body),
        C_WHITE "Dobro dosli nazad, " C_SERVER "%s" C_WHITE "!\n\n" \
        "Ovaj nalog je registrovan. Unesite lozinku da biste se prijavili.%s%s",
        g_Player[playerid][pName], error[0] ? ("\n\n" C_RED) : (""), error);
    ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, SERVER_NAME " - Prijava", body, "Prijavi se", "Izlaz");
}

stock Account_ShowRegisterPassword(playerid, const error[])
{
    new body[384];
    format(body, sizeof(body),
        C_WHITE "Dobro dosli na " C_SERVER SERVER_NAME C_WHITE ", " C_SERVER "%s" C_WHITE "!\n\n" \
        "Ovaj nalog nije registrovan. Izaberite lozinku za svoj nalog.\n" \
        "Lozinka mora imati od %d do %d karaktera.%s%s",
        g_Player[playerid][pName], PASSWORD_MIN_LENGTH, PASSWORD_MAX_LENGTH, error[0] ? ("\n\n" C_RED) : (""), error);
    ShowPlayerDialog(playerid, DIALOG_REGISTER_PASSWORD, DIALOG_STYLE_PASSWORD, SERVER_NAME " - Registracija (1/5)", body, "Dalje", "Izlaz");
}

stock Account_ShowRegisterConfirm(playerid)
{
    ShowPlayerDialog(playerid, DIALOG_REGISTER_CONFIRM, DIALOG_STYLE_PASSWORD, SERVER_NAME " - Registracija (2/5)",
        C_WHITE "Ponovo unesite istu lozinku kao potvrdu.", "Dalje", "Nazad");
}

stock Account_ShowRegisterGender(playerid)
{
    ShowPlayerDialog(playerid, DIALOG_REGISTER_GENDER, DIALOG_STYLE_LIST, SERVER_NAME " - Registracija (3/5): Pol",
        "Musko\nZensko", "Dalje", "Izlaz");
}

stock Account_ShowRegisterAge(playerid, const error[])
{
    new body[192];
    format(body, sizeof(body),
        C_WHITE "Koliko godina ima vas karakter?\n" \
        "Unesite broj od %d do %d.%s%s",
        CHARACTER_MIN_AGE, CHARACTER_MAX_AGE, error[0] ? ("\n\n" C_RED) : (""), error);
    ShowPlayerDialog(playerid, DIALOG_REGISTER_AGE, DIALOG_STYLE_INPUT, SERVER_NAME " - Registracija (4/5): Godine", body, "Dalje", "Nazad");
}

stock Account_ShowRegisterRules(playerid)
{
    ShowPlayerDialog(playerid, DIALOG_REGISTER_RULES, DIALOG_STYLE_MSGBOX, SERVER_NAME " - Registracija (5/5): Pravila",
        C_WHITE "1. Uvek ostanite u ulozi (IC). OOC razgovor samo preko /b.\n" \
        "2. Zabranjen je Metagaming (MG): koriscenje OOC informacija u igri.\n" \
        "3. Zabranjen je Powergaming (PG): nerealne ili nametnute akcije.\n" \
        "4. Zabranjen je Deathmatch (DM): napad ili ubistvo bez RP razloga.\n" \
        "5. Zabranjen je Revenge Kill (RK) i povratak na mesto smrti (NLR).\n" \
        "6. Zabranjeno je koriscenje cheatova, bugova i modova koji daju prednost.\n" \
        "7. Postujte sve igrace i administraciju.\n\n" \
        "Krsenje pravila se kaznjava kick-om, zatvorom ili banom.\n\n" \
        C_SERVER "Klikom na \"Prihvatam\" potvrdjujete da ste procitali i prihvatili pravila.",
        "Prihvatam", "Odbijam");
}

stock bool:Account_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
    switch (dialogid)
    {
        case DIALOG_INVALID_NAME:
        {
            Kick(playerid);
        }
        case DIALOG_LOGIN:
        {
            if (g_Player[playerid][pState] != ACCOUNT_STATE_LOGIN || g_Player[playerid][pAwaitingBcrypt])
            {
                return true;
            }
            if (!response)
            {
                Kick(playerid);
                return true;
            }
            if (!inputtext[0])
            {
                Account_ShowLogin(playerid, "Niste uneli lozinku.");
                return true;
            }

            g_Player[playerid][pAwaitingBcrypt] = true;
            bcrypt_verify(playerid, "OnAccountPasswordVerify", inputtext, g_Player[playerid][pPasswordHash]);
        }
        case DIALOG_REGISTER_PASSWORD:
        {
            if (!response)
            {
                Kick(playerid);
                return true;
            }

            new len = strlen(inputtext);
            if (len < PASSWORD_MIN_LENGTH || len > PASSWORD_MAX_LENGTH)
            {
                Account_ShowRegisterPassword(playerid, "Lozinka nije odgovarajuce duzine.");
                return true;
            }

            strcopy(g_PendingPassword[playerid], inputtext);
            Account_ShowRegisterConfirm(playerid);
        }
        case DIALOG_REGISTER_CONFIRM:
        {
            if (!response)
            {
                g_PendingPassword[playerid][0] = '\0';
                Account_ShowRegisterPassword(playerid, "");
                return true;
            }
            if (strcmp(inputtext, g_PendingPassword[playerid]) != 0 || !inputtext[0])
            {
                g_PendingPassword[playerid][0] = '\0';
                Account_ShowRegisterPassword(playerid, "Lozinke se ne poklapaju, pokusajte ponovo.");
                return true;
            }
            if (g_Player[playerid][pAwaitingBcrypt])
            {
                return true;
            }

            g_Player[playerid][pAwaitingBcrypt] = true;
            bcrypt_hash(playerid, "OnAccountPasswordHashed", g_PendingPassword[playerid], BCRYPT_COST);
            g_PendingPassword[playerid][0] = '\0';
        }
        case DIALOG_REGISTER_GENDER:
        {
            if (!response)
            {
                Kick(playerid);
                return true;
            }

            g_Player[playerid][pGender] = (listitem == 1) ? GENDER_FEMALE : GENDER_MALE;
            Account_ShowRegisterAge(playerid, "");
        }
        case DIALOG_REGISTER_AGE:
        {
            if (!response)
            {
                Account_ShowRegisterGender(playerid);
                return true;
            }

            new age;
            if (sscanf(inputtext, "d", age) || age < CHARACTER_MIN_AGE || age > CHARACTER_MAX_AGE)
            {
                Account_ShowRegisterAge(playerid, "Unesite ispravan broj godina.");
                return true;
            }

            g_Player[playerid][pAge] = age;
            Account_ShowRegisterRules(playerid);
        }
        case DIALOG_REGISTER_RULES:
        {
            if (!response)
            {
                Msg_Error(playerid, "Morate prihvatiti pravila da biste igrali na serveru.");
                KickWithDelay(playerid);
                return true;
            }

            Account_Create(playerid);
        }
        case DIALOG_CHANGEPASS_OLD:
        {
            if (!response || g_Player[playerid][pState] != ACCOUNT_STATE_LOGGED_IN || g_Player[playerid][pAwaitingBcrypt])
            {
                return true;
            }
            if (!inputtext[0])
            {
                Account_ShowChangePassOld(playerid, "Niste uneli lozinku.");
                return true;
            }

            g_Player[playerid][pAwaitingBcrypt] = true;
            bcrypt_verify(playerid, "OnAccountChangePassVerify", inputtext, g_Player[playerid][pPasswordHash]);
        }
        case DIALOG_CHANGEPASS_NEW:
        {
            if (!response || !g_Player[playerid][pOldPasswordVerified])
            {
                g_Player[playerid][pOldPasswordVerified] = false;
                return true;
            }

            new len = strlen(inputtext);
            if (len < PASSWORD_MIN_LENGTH || len > PASSWORD_MAX_LENGTH)
            {
                Account_ShowChangePassNew(playerid, "Lozinka nije odgovarajuce duzine.");
                return true;
            }

            strcopy(g_PendingPassword[playerid], inputtext);
            ShowPlayerDialog(playerid, DIALOG_CHANGEPASS_CONFIRM, DIALOG_STYLE_PASSWORD, SERVER_NAME " - Promena lozinke (3/3)",
                C_WHITE "Ponovo unesite novu lozinku kao potvrdu.", "Promeni", "Nazad");
        }
        case DIALOG_CHANGEPASS_CONFIRM:
        {
            if (!g_Player[playerid][pOldPasswordVerified] || g_Player[playerid][pAwaitingBcrypt])
            {
                return true;
            }
            if (!response)
            {
                g_PendingPassword[playerid][0] = '\0';
                Account_ShowChangePassNew(playerid, "");
                return true;
            }
            if (strcmp(inputtext, g_PendingPassword[playerid]) != 0 || !inputtext[0])
            {
                g_PendingPassword[playerid][0] = '\0';
                Account_ShowChangePassNew(playerid, "Lozinke se ne poklapaju, pokusajte ponovo.");
                return true;
            }

            g_Player[playerid][pAwaitingBcrypt] = true;
            bcrypt_hash(playerid, "OnAccountNewPasswordHashed", g_PendingPassword[playerid], BCRYPT_COST);
            g_PendingPassword[playerid][0] = '\0';
        }
        default:
        {
            return false;
        }
    }
    return true;
}

forward OnAccountPasswordVerify(playerid, bool:success);
public OnAccountPasswordVerify(playerid, bool:success)
{
    if (g_Player[playerid][pState] != ACCOUNT_STATE_LOGIN || !g_Player[playerid][pAwaitingBcrypt])
    {
        return 1;
    }
    g_Player[playerid][pAwaitingBcrypt] = false;

    Account_LogAttempt(playerid, success);

    if (success)
    {
        Account_CompleteLogin(playerid, false);
        return 1;
    }

    new ip[16], query[256];
    GetPlayerIp(playerid, ip, sizeof(ip));
    mysql_format(g_SQL, query, sizeof(query),
        "SELECT COUNT(*) AS `failures` FROM `login_attempts` " \
        "WHERE `account_id` = %d AND `ip` = '%e' AND `success` = 0 AND `attempted_at` > NOW() - INTERVAL %d MINUTE",
        g_Player[playerid][pDbId], ip, LOCKOUT_WINDOW_MINUTES);
    mysql_tquery(g_SQL, query, "OnAccountFailedLogin", "dd", playerid, g_AccountRace[playerid]);
    return 1;
}

stock Account_LogAttempt(playerid, bool:success)
{
    new ip[16], query[160];
    GetPlayerIp(playerid, ip, sizeof(ip));
    mysql_format(g_SQL, query, sizeof(query),
        "INSERT INTO `login_attempts` (`account_id`, `ip`, `success`) VALUES (%d, '%e', %d)",
        g_Player[playerid][pDbId], ip, success ? 1 : 0);
    mysql_tquery(g_SQL, query);
}

forward OnAccountFailedLogin(playerid, race);
public OnAccountFailedLogin(playerid, race)
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }

    new failures;
    cache_get_value_name_int(0, "failures", failures);

    if (failures >= LOCKOUT_MAX_FAILURES)
    {
        Msg_Error(playerid, "Previse pogresnih lozinki. Prijava sa vase adrese je zakljucana na %d minuta.", LOCKOUT_MINUTES);
        KickWithDelay(playerid);
        return 1;
    }

    if (++g_Player[playerid][pLoginAttempts] >= MAX_LOGIN_ATTEMPTS)
    {
        Msg_Error(playerid, "Previse pogresnih pokusaja. Izbaceni ste sa servera.");
        KickWithDelay(playerid);
        return 1;
    }

    new error[64];
    format(error, sizeof(error), "Pogresna lozinka! Preostalo pokusaja: %d", MAX_LOGIN_ATTEMPTS - g_Player[playerid][pLoginAttempts]);
    Account_ShowLogin(playerid, error);
    return 1;
}

forward OnAccountPasswordHashed(playerid, hashid);
public OnAccountPasswordHashed(playerid, hashid)
{
    if (g_Player[playerid][pState] != ACCOUNT_STATE_REGISTER || !g_Player[playerid][pAwaitingBcrypt])
    {
        return 1;
    }
    g_Player[playerid][pAwaitingBcrypt] = false;

    bcrypt_get_hash(g_Player[playerid][pPasswordHash], BCRYPT_HASH_LENGTH);
    Account_ShowRegisterGender(playerid);
    return 1;
}

stock Account_ShowChangePassOld(playerid, const error[])
{
    new body[192];
    format(body, sizeof(body), C_WHITE "Unesite svoju trenutnu lozinku.%s%s", error[0] ? ("\n\n" C_RED) : (""), error);
    ShowPlayerDialog(playerid, DIALOG_CHANGEPASS_OLD, DIALOG_STYLE_PASSWORD, SERVER_NAME " - Promena lozinke (1/3)", body, "Dalje", "Odustani");
}

stock Account_ShowChangePassNew(playerid, const error[])
{
    new body[192];
    format(body, sizeof(body),
        C_WHITE "Unesite novu lozinku.\nLozinka mora imati od %d do %d karaktera.%s%s",
        PASSWORD_MIN_LENGTH, PASSWORD_MAX_LENGTH, error[0] ? ("\n\n" C_RED) : (""), error);
    ShowPlayerDialog(playerid, DIALOG_CHANGEPASS_NEW, DIALOG_STYLE_PASSWORD, SERVER_NAME " - Promena lozinke (2/3)", body, "Dalje", "Odustani");
}

forward OnAccountChangePassVerify(playerid, bool:success);
public OnAccountChangePassVerify(playerid, bool:success)
{
    if (g_Player[playerid][pState] != ACCOUNT_STATE_LOGGED_IN || !g_Player[playerid][pAwaitingBcrypt])
    {
        return 1;
    }
    g_Player[playerid][pAwaitingBcrypt] = false;

    if (!success)
    {
        Account_ShowChangePassOld(playerid, "Pogresna lozinka.");
        return 1;
    }

    g_Player[playerid][pOldPasswordVerified] = true;
    Account_ShowChangePassNew(playerid, "");
    return 1;
}

forward OnAccountNewPasswordHashed(playerid, hashid);
public OnAccountNewPasswordHashed(playerid, hashid)
{
    if (g_Player[playerid][pState] != ACCOUNT_STATE_LOGGED_IN || !g_Player[playerid][pAwaitingBcrypt])
    {
        return 1;
    }
    g_Player[playerid][pAwaitingBcrypt] = false;
    g_Player[playerid][pOldPasswordVerified] = false;

    bcrypt_get_hash(g_Player[playerid][pPasswordHash], BCRYPT_HASH_LENGTH);

    new query[160];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `accounts` SET `password` = '%e' WHERE `id` = %d",
        g_Player[playerid][pPasswordHash], g_Player[playerid][pDbId]);
    mysql_tquery(g_SQL, query);

    Msg_Success(playerid, "Lozinka je uspesno promenjena.");
    return 1;
}

stock Account_Create(playerid)
{
    if (g_Player[playerid][pGender] == GENDER_FEMALE)
    {
        g_Player[playerid][pSkin] = g_StarterSkinsFemale[random(sizeof(g_StarterSkinsFemale))];
    }
    else
    {
        g_Player[playerid][pSkin] = g_StarterSkinsMale[random(sizeof(g_StarterSkinsMale))];
    }

    g_Player[playerid][pAdminLevel] = Account_IsOwnerName(g_Player[playerid][pName]) ? ADMIN_LEVEL_OWNER : 0;
    g_Player[playerid][pLevel] = 1;
    g_Player[playerid][pRespect] = 0;
    g_Player[playerid][pCash] = STARTER_CASH;
    g_Player[playerid][pBank] = STARTER_BANK;
    g_Player[playerid][pPosX] = NEW_SPAWN_X;
    g_Player[playerid][pPosY] = NEW_SPAWN_Y;
    g_Player[playerid][pPosZ] = NEW_SPAWN_Z;
    g_Player[playerid][pPosA] = NEW_SPAWN_A;
    g_Player[playerid][pInterior] = 0;
    g_Player[playerid][pVirtualWorld] = 0;
    g_Player[playerid][pHealth] = 100.0;
    g_Player[playerid][pArmour] = 0.0;

    new ip[16], query[640];
    GetPlayerIp(playerid, ip, sizeof(ip));
    mysql_format(g_SQL, query, sizeof(query),
        "INSERT INTO `accounts` (`name`, `password`, `admin_level`, `gender`, `age`, `skin`, `level`, `cash`, `bank`, " \
        "`pos_x`, `pos_y`, `pos_z`, `pos_a`, `register_ip`, `last_ip`, `last_login`) " \
        "VALUES ('%e', '%e', %d, %d, %d, %d, %d, %d, %d, %f, %f, %f, %f, '%e', '%e', NOW())",
        g_Player[playerid][pName], g_Player[playerid][pPasswordHash], g_Player[playerid][pAdminLevel],
        g_Player[playerid][pGender], g_Player[playerid][pAge], g_Player[playerid][pSkin], g_Player[playerid][pLevel],
        g_Player[playerid][pCash], g_Player[playerid][pBank],
        g_Player[playerid][pPosX], g_Player[playerid][pPosY], g_Player[playerid][pPosZ], g_Player[playerid][pPosA],
        ip, ip);
    mysql_tquery(g_SQL, query, "OnAccountCreated", "dd", playerid, g_AccountRace[playerid]);
}

forward OnAccountCreated(playerid, race);
public OnAccountCreated(playerid, race)
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }

    g_Player[playerid][pDbId] = cache_insert_id();
    Account_CompleteLogin(playerid, true);
    return 1;
}

stock Account_CompleteLogin(playerid, bool:is_new)
{
    if (g_Player[playerid][pLoginTimer])
    {
        KillTimer(g_Player[playerid][pLoginTimer]);
        g_Player[playerid][pLoginTimer] = 0;
    }

    g_Player[playerid][pState] = ACCOUNT_STATE_LOGGED_IN;
    g_Player[playerid][pFirstSpawn] = true;
    g_Player[playerid][pSessionStart] = gettime();

    if (Account_IsOwnerName(g_Player[playerid][pName]))
    {
        g_Player[playerid][pAdminLevel] = ADMIN_LEVEL_OWNER;
    }

    if (g_Player[playerid][pHealth] < 1.0)
    {
        g_Player[playerid][pHealth] = 100.0;
    }

    if (is_new)
    {
        Msg_Server(playerid, "Uspesno ste se registrovali! Dobro dosli u Los Santos.");
    }
    else
    {
        new ip[16], query[160];
        GetPlayerIp(playerid, ip, sizeof(ip));
        mysql_format(g_SQL, query, sizeof(query), "UPDATE `accounts` SET `last_login` = NOW(), `last_ip` = '%e' WHERE `id` = %d", ip, g_Player[playerid][pDbId]);
        mysql_tquery(g_SQL, query);

        Msg_Server(playerid, "Uspesno ste se prijavili. Dobro dosli nazad!");

        if (g_Player[playerid][pLastLoginText][0])
        {
            Msg_Info(playerid, "Poslednja prijava: %s sa IP %s", g_Player[playerid][pLastLoginText], g_Player[playerid][pLastIp]);
        }
        Vip_OnLogin(playerid);
    }

    SetSpawnInfo(playerid, NO_TEAM, g_Player[playerid][pSkin],
        g_Player[playerid][pPosX], g_Player[playerid][pPosY], g_Player[playerid][pPosZ], g_Player[playerid][pPosA]);
    TogglePlayerSpectating(playerid, false);
}

stock Account_StartTimeout(playerid, time_ms)
{
    if (g_Player[playerid][pLoginTimer])
    {
        KillTimer(g_Player[playerid][pLoginTimer]);
    }
    g_Player[playerid][pLoginTimer] = SetTimerEx("Account_OnTimeout", time_ms, false, "dd", playerid, g_AccountRace[playerid]);
}

forward Account_OnTimeout(playerid, race);
public Account_OnTimeout(playerid, race)
{
    if (race != g_AccountRace[playerid] || g_Player[playerid][pState] == ACCOUNT_STATE_LOGGED_IN)
    {
        return 1;
    }

    g_Player[playerid][pLoginTimer] = 0;
    Msg_Error(playerid, "Isteklo je vreme za prijavu. Izbaceni ste sa servera.");
    KickWithDelay(playerid);
    return 1;
}

stock Account_Save(playerid, bool:sync = false)
{
    if (g_Player[playerid][pState] != ACCOUNT_STATE_LOGGED_IN)
    {
        return;
    }

    new PLAYER_STATE:player_state = GetPlayerState(playerid);
    if (player_state != PLAYER_STATE_NONE && player_state != PLAYER_STATE_WASTED && player_state != PLAYER_STATE_SPECTATING)
    {
        GetPlayerPos(playerid, g_Player[playerid][pPosX], g_Player[playerid][pPosY], g_Player[playerid][pPosZ]);
        GetPlayerFacingAngle(playerid, g_Player[playerid][pPosA]);
        g_Player[playerid][pInterior] = GetPlayerInterior(playerid);
        g_Player[playerid][pVirtualWorld] = GetPlayerVirtualWorld(playerid);
        GetPlayerHealth(playerid, g_Player[playerid][pHealth]);
        GetPlayerArmour(playerid, g_Player[playerid][pArmour]);
    }

    new now = gettime();
    g_Player[playerid][pPlayedSeconds] += now - g_Player[playerid][pSessionStart];
    g_Player[playerid][pSessionStart] = now;

    new query[1024];
    mysql_format(g_SQL, query, sizeof(query),
        "UPDATE `accounts` SET `admin_level` = %d, `helper_level` = %d, `skin` = %d, `level` = %d, `respect` = %d, `cash` = %d, `bank` = %d, " \
        "`pos_x` = %f, `pos_y` = %f, `pos_z` = %f, `pos_a` = %f, `interior` = %d, `virtual_world` = %d, " \
        "`health` = %f, `armour` = %f, `played_seconds` = %d, `channel_bits` = %d, " \
        "`mute_until` = %d, `newbie_mute_until` = %d, `jail_seconds` = %d, `jail_reason` = '%e' WHERE `id` = %d",
        g_Player[playerid][pAdminLevel], g_Player[playerid][pHelperLevel], g_Player[playerid][pSkin],
        g_Player[playerid][pLevel], g_Player[playerid][pRespect],
        g_Player[playerid][pCash], g_Player[playerid][pBank],
        g_Player[playerid][pPosX], g_Player[playerid][pPosY], g_Player[playerid][pPosZ], g_Player[playerid][pPosA],
        g_Player[playerid][pInterior], g_Player[playerid][pVirtualWorld],
        g_Player[playerid][pHealth], g_Player[playerid][pArmour], g_Player[playerid][pPlayedSeconds],
        g_Player[playerid][pChannelBits],
        g_Player[playerid][pMuteUntil], g_Player[playerid][pNewbieMuteUntil],
        g_Player[playerid][pJailSeconds], g_Player[playerid][pJailReason],
        g_Player[playerid][pDbId]);

    if (sync)
    {
        mysql_query(g_SQL, query, false);
    }
    else
    {
        mysql_tquery(g_SQL, query);
    }
}

forward Account_Autosave();
public Account_Autosave()
{
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        Account_Save(i);
    }
    return 1;
}

// Runs after the account row is read, so VIP and staff status are known before the slot is refused.
stock bool:Account_CheckFreeSlot(playerid)
{
    if (Vip_HasFreeSlot(playerid))
    {
        return true;
    }
    Msg_Error(playerid, "Server je pun. Poslednja mesta su rezervisana za VIP igrace. Pokusajte malo kasnije.");
    KickWithDelay(playerid);
    return false;
}
