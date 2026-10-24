new g_JailTimer;

stock Punish_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `bans` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`account_id` INT UNSIGNED NULL," \
        "`name` VARCHAR(24) NOT NULL DEFAULT ''," \
        "`ip` VARCHAR(16) NOT NULL DEFAULT ''," \
        "`serial` VARCHAR(64) NOT NULL DEFAULT ''," \
        "`reason` VARCHAR(128) NOT NULL," \
        "`admin_name` VARCHAR(24) NOT NULL," \
        "`created_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP," \
        "`expires_at` DATETIME NULL," \
        "`active` TINYINT(1) NOT NULL DEFAULT 1," \
        "INDEX `ban_name` (`name`), INDEX `ban_ip` (`ip`), INDEX `ban_serial` (`serial`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");

    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `warnings` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`account_id` INT UNSIGNED NOT NULL," \
        "`admin_name` VARCHAR(24) NOT NULL," \
        "`reason` VARCHAR(128) NOT NULL," \
        "`created_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP," \
        "`active` TINYINT(1) NOT NULL DEFAULT 1," \
        "INDEX `warn_account` (`account_id`, `active`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");

    g_JailTimer = SetTimer("Punish_JailTick", 1000, true);
}

stock Punish_Exit()
{
    KillTimer(g_JailTimer);
}

// Every punishment is announced to the whole server in one consistent line.
stock Punish_Announce(const message[])
{
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (Account_IsLoggedIn(i))
        {
            Chat_Send(i, COLOR_RED, message);
        }
    }
}

// Loopback is shared by every client on the test machine, so it is never stored or matched in bans.
stock bool:Punish_IsLoopback(const ip[])
{
    return !strcmp(ip, "127.0.0.1");
}

stock bool:Punish_IsMuted(playerid)
{
    return g_Player[playerid][pMuteUntil] > gettime();
}

stock Punish_MinutesLeft(until)
{
    return (until - gettime() + 59) / 60;
}

stock Staff_CheckBan(playerid)
{
    if (Account_IsOwnerName(g_Player[playerid][pName]))
    {
        Account_CheckName(playerid);
        return;
    }

    new ip[16], serial[64];
    GetPlayerIp(playerid, ip, sizeof(ip));
    gpci(playerid, serial, sizeof(serial));

    new query[512];
    mysql_format(g_SQL, query, sizeof(query),
        "SELECT `reason`, `admin_name`, IFNULL(DATE_FORMAT(`expires_at`, '%%d.%%m.%%Y. u %%H:%%i'), '') AS `expires_text` " \
        "FROM `bans` WHERE `active` = 1 AND (`expires_at` IS NULL OR `expires_at` > NOW()) " \
        "AND (`name` = '%e' OR (`ip` <> '' AND `ip` = '%e') OR (`serial` <> '' AND `serial` = '%e')) " \
        "ORDER BY `expires_at` IS NULL DESC, `expires_at` DESC LIMIT 1",
        g_Player[playerid][pName], ip, serial);
    mysql_tquery(g_SQL, query, "OnPunishBanCheck", "dd", playerid, g_AccountRace[playerid]);
}

forward OnPunishBanCheck(playerid, race);
public OnPunishBanCheck(playerid, race)
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }

    if (cache_num_rows() > 0)
    {
        new reason[128], admin_name[MAX_PLAYER_NAME + 1], expires_text[32];
        cache_get_value_name(0, "reason", reason);
        cache_get_value_name(0, "admin_name", admin_name);
        cache_get_value_name(0, "expires_text", expires_text);

        Msg_Error(playerid, "Banovani ste sa ovog servera.");
        Msg_Error(playerid, "Razlog: %s    Admin: %s", reason, admin_name);
        if (expires_text[0])
        {
            Msg_Error(playerid, "Ban istice: %s", expires_text);
        }
        else
        {
            Msg_Error(playerid, "Ban je trajan.");
        }
        KickWithDelay(playerid);
        return 1;
    }

    Punish_CheckSharedIp(playerid);
    Account_CheckName(playerid);
    return 1;
}

stock Punish_CheckSharedIp(playerid)
{
    new ip[16], other_ip[16], message[144];
    GetPlayerIp(playerid, ip, sizeof(ip));

    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (i == playerid || !IsPlayerConnected(i))
        {
            continue;
        }

        GetPlayerIp(i, other_ip, sizeof(other_ip));
        if (!strcmp(ip, other_ip))
        {
            format(message, sizeof(message), "[A] Upozorenje: %s [%d] je usao sa iste IP adrese kao %s [%d].",
                g_Player[playerid][pName], playerid, g_Player[i][pName], i);
            Staff_SendToAdmins(message);
        }
    }
}

// days == 0 means permanent.
stock Punish_InsertBan(adminid, account_id, const name[], const ip[], const serial[], days, const reason[])
{
    new store_ip[16];
    if (!Punish_IsLoopback(ip))
    {
        strcopy(store_ip, ip);
    }

    new query[640];
    if (days > 0)
    {
        mysql_format(g_SQL, query, sizeof(query),
            "INSERT INTO `bans` (`account_id`, `name`, `ip`, `serial`, `reason`, `admin_name`, `expires_at`) " \
            "VALUES (NULLIF(%d, 0), '%e', '%e', '%e', '%e', '%e', NOW() + INTERVAL %d DAY)",
            account_id, name, store_ip, serial, reason, g_Player[adminid][pName], days);
    }
    else
    {
        mysql_format(g_SQL, query, sizeof(query),
            "INSERT INTO `bans` (`account_id`, `name`, `ip`, `serial`, `reason`, `admin_name`) " \
            "VALUES (NULLIF(%d, 0), '%e', '%e', '%e', '%e', '%e')",
            account_id, name, store_ip, serial, reason, g_Player[adminid][pName]);
    }
    mysql_tquery(g_SQL, query);
}

stock Punish_BanOnline(adminid, targetid, days, const reason[])
{
    new ip[16], serial[64];
    GetPlayerIp(targetid, ip, sizeof(ip));
    gpci(targetid, serial, sizeof(serial));
    Punish_InsertBan(adminid, g_Player[targetid][pDbId], g_Player[targetid][pName], ip, serial, days, reason);

    new admin_name[MAX_PLAYER_NAME + 1], target_name[MAX_PLAYER_NAME + 1], message[192], length[24];
    Account_GetDisplayName(adminid, admin_name);
    Account_GetDisplayName(targetid, target_name);

    if (days > 0)
    {
        format(length, sizeof(length), "%d dana", days);
    }
    else
    {
        strcopy(length, "trajno");
    }

    format(message, sizeof(message), "[ADMIN] %s je banovao igraca %s (%s). Razlog: %s", admin_name, target_name, length, reason);
    Punish_Announce(message);

    format(message, sizeof(message), "%s, %s", length, reason);
    Staff_Log(adminid, "ban", targetid, message);
    KickWithDelay(targetid);
}

stock Punish_PutInJail(playerid)
{
    SetPlayerInterior(playerid, JAIL_INTERIOR);
    SetPlayerVirtualWorld(playerid, JAIL_WORLD);
    SetPlayerPos(playerid, JAIL_X, JAIL_Y, JAIL_Z);
    ResetPlayerWeapons(playerid);
}

stock Punish_Release(playerid)
{
    g_Player[playerid][pJailSeconds] = 0;
    g_Player[playerid][pJailReason][0] = '\0';

    SetPlayerInterior(playerid, 0);
    SetPlayerVirtualWorld(playerid, 0);
    SetPlayerPos(playerid, NEW_SPAWN_X, NEW_SPAWN_Y, NEW_SPAWN_Z);
    SetPlayerFacingAngle(playerid, NEW_SPAWN_A);
    Account_Save(playerid);
}

stock Punish_OnPlayerSpawn(playerid)
{
    if (g_Player[playerid][pJailSeconds] > 0)
    {
        Punish_PutInJail(playerid);
        Msg_Admin(playerid, "Nalazite se u admin zatvoru jos %d min. Razlog: %s", (g_Player[playerid][pJailSeconds] + 59) / 60, g_Player[playerid][pJailReason]);
    }
}

// Jail time only runs down while the jailed player is online and in the game.
forward Punish_JailTick();
public Punish_JailTick()
{
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (!Account_IsLoggedIn(i) || g_Player[i][pJailSeconds] <= 0)
        {
            continue;
        }

        new PLAYER_STATE:player_state = GetPlayerState(i);
        if (player_state == PLAYER_STATE_NONE || player_state == PLAYER_STATE_SPECTATING || player_state == PLAYER_STATE_WASTED)
        {
            continue;
        }

        if (--g_Player[i][pJailSeconds] <= 0)
        {
            Punish_Release(i);
            Msg_Admin(i, "Odsluzili ste kaznu u admin zatvoru. Slobodni ste.");
            continue;
        }

        if (GetPlayerVirtualWorld(i) != JAIL_WORLD || GetPlayerInterior(i) != JAIL_INTERIOR
            || GetPlayerDistanceFromPoint(i, JAIL_X, JAIL_Y, JAIL_Z) > JAIL_ESCAPE_RADIUS)
        {
            Punish_PutInJail(i);
        }

        if (g_Player[i][pJailSeconds] % 60 == 0)
        {
            Msg_Info(i, "Preostalo vreme u admin zatvoru: %d min.", g_Player[i][pJailSeconds] / 60);
        }
    }
    return 1;
}

cmd:kick(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }

    new targetid, reason[96];
    if (sscanf(params, "us[96]", targetid, reason))
    {
        Msg_Usage(playerid, "/kick [id ili ime] [razlog]");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid, "Ne mozete izbaciti samog sebe.") || !Staff_CheckOutranks(playerid, targetid))
    {
        return 1;
    }

    new admin_name[MAX_PLAYER_NAME + 1], target_name[MAX_PLAYER_NAME + 1], message[192];
    Account_GetDisplayName(playerid, admin_name);
    Account_GetDisplayName(targetid, target_name);
    format(message, sizeof(message), "[ADMIN] %s je izbacio igraca %s sa servera. Razlog: %s", admin_name, target_name, reason);
    Punish_Announce(message);

    Staff_Log(playerid, "kick", targetid, reason);
    KickWithDelay(targetid);
    return 1;
}

cmd:ban(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_ADMIN))
    {
        return 1;
    }

    new bool:can_permanent = g_Player[playerid][pAdminLevel] >= ADMIN_LEVEL_SENIOR;

    new targetid, days, reason[96];
    if (sscanf(params, "uds[96]", targetid, days, reason) || days < 0 || (days == 0 && !can_permanent) || (!can_permanent && days > ADMIN_MAX_BAN_DAYS))
    {
        if (can_permanent)
        {
            Msg_Usage(playerid, "/ban [id ili ime] [dana, 0 = trajno] [razlog]");
        }
        else
        {
            Msg_Usage(playerid, "/ban [id ili ime] [dana 1-%d] [razlog]", ADMIN_MAX_BAN_DAYS);
        }
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid, "Ne mozete banovati samog sebe.") || !Staff_CheckOutranks(playerid, targetid))
    {
        return 1;
    }

    Punish_BanOnline(playerid, targetid, days, reason);
    return 1;
}

cmd:oban(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_SENIOR))
    {
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1], days, reason[96];
    if (sscanf(params, "s[25]ds[96]", name, days, reason) || days < 0)
    {
        Msg_Usage(playerid, "/oban [tacno ime naloga] [dana, 0 = trajno] [razlog] (ban igraca koji nije online)");
        return 1;
    }

    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (IsPlayerConnected(i) && !strcmp(g_Player[i][pName], name, true))
        {
            Msg_Error(playerid, "Taj igrac je trenutno na serveru. Koristite /ban.");
            return 1;
        }
    }

    new query[160];
    mysql_format(g_SQL, query, sizeof(query), "SELECT `id`, `name`, `last_ip`, `admin_level` FROM `accounts` WHERE `name` = '%e' LIMIT 1", name);
    mysql_tquery(g_SQL, query, "OnPunishOfflineBan", "dds", playerid, days, reason);
    return 1;
}

forward OnPunishOfflineBan(playerid, days, const reason[]);
public OnPunishOfflineBan(playerid, days, const reason[])
{
    if (!Account_IsLoggedIn(playerid))
    {
        return 1;
    }
    if (cache_num_rows() == 0)
    {
        Msg_Error(playerid, "Nalog sa tim imenom ne postoji.");
        return 1;
    }

    new account_id, name[MAX_PLAYER_NAME + 1], ip[16], admin_level;
    cache_get_value_name_int(0, "id", account_id);
    cache_get_value_name(0, "name", name);
    cache_get_value_name(0, "last_ip", ip);
    cache_get_value_name_int(0, "admin_level", admin_level);

    if (admin_level >= g_Player[playerid][pAdminLevel])
    {
        Msg_Error(playerid, "Nemate dozvolu za ovu komandu.");
        return 1;
    }

    Punish_InsertBan(playerid, account_id, name, ip, "", days, reason);

    new admin_name[MAX_PLAYER_NAME + 1], message[192];
    Account_GetDisplayName(playerid, admin_name);
    if (days > 0)
    {
        format(message, sizeof(message), "[ADMIN] %s je banovao igraca %s (%d dana, van servera). Razlog: %s", admin_name, name, days, reason);
    }
    else
    {
        format(message, sizeof(message), "[ADMIN] %s je trajno banovao igraca %s (van servera). Razlog: %s", admin_name, name, reason);
    }
    Punish_Announce(message);

    format(message, sizeof(message), "%s: %d dana, %s", name, days, reason);
    Staff_Log(playerid, "oban", INVALID_PLAYER_ID, message);
    return 1;
}

cmd:banip(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_SENIOR))
    {
        return 1;
    }

    new ip[16], days, reason[96];
    if (sscanf(params, "s[16]ds[96]", ip, days, reason) || days < 0 || strlen(ip) < 7)
    {
        Msg_Usage(playerid, "/banip [IP adresa] [dana, 0 = trajno] [razlog]");
        return 1;
    }
    if (Punish_IsLoopback(ip))
    {
        Msg_Error(playerid, "Lokalna adresa 127.0.0.1 se ne moze banovati.");
        return 1;
    }

    Punish_InsertBan(playerid, 0, "", ip, "", days, reason);
    Msg_Admin(playerid, "IP adresa %s je banovana.", ip);

    new details[160];
    format(details, sizeof(details), "%s: %d dana, %s", ip, days, reason);
    Staff_Log(playerid, "banip", INVALID_PLAYER_ID, details);
    return 1;
}

cmd:unban(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_SENIOR))
    {
        return 1;
    }

    new value[25];
    if (sscanf(params, "s[25]", value))
    {
        Msg_Usage(playerid, "/unban [ime naloga ili IP adresa]");
        return 1;
    }

    new query[192];
    mysql_format(g_SQL, query, sizeof(query),
        "UPDATE `bans` SET `active` = 0 WHERE `active` = 1 AND (`name` = '%e' OR `ip` = '%e')", value, value);
    mysql_tquery(g_SQL, query, "OnPunishUnban", "ds", playerid, value);
    return 1;
}

forward OnPunishUnban(playerid, const value[]);
public OnPunishUnban(playerid, const value[])
{
    if (!Account_IsLoggedIn(playerid))
    {
        return 1;
    }

    new count = cache_affected_rows();
    if (count == 0)
    {
        Msg_Error(playerid, "Nema aktivnog bana za '%s'.", value);
        return 1;
    }

    Msg_Admin(playerid, "Skinuto banova za '%s': %d.", value, count);
    Staff_Log(playerid, "unban", INVALID_PLAYER_ID, value);
    return 1;
}

cmd:warn(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_ADMIN))
    {
        return 1;
    }

    new targetid, reason[96];
    if (sscanf(params, "us[96]", targetid, reason))
    {
        Msg_Usage(playerid, "/warn [id ili ime] [razlog] (%d upozorenja = ban %d dana)", MAX_WARNINGS, WARNING_BAN_DAYS);
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid, "Ne mozete upozoriti samog sebe.") || !Staff_CheckOutranks(playerid, targetid))
    {
        return 1;
    }

    new query[384];
    mysql_format(g_SQL, query, sizeof(query),
        "INSERT INTO `warnings` (`account_id`, `admin_name`, `reason`) VALUES (%d, '%e', '%e')",
        g_Player[targetid][pDbId], g_Player[playerid][pName], reason);
    mysql_tquery(g_SQL, query);

    mysql_format(g_SQL, query, sizeof(query),
        "SELECT COUNT(*) AS `total` FROM `warnings` WHERE `account_id` = %d AND `active` = 1", g_Player[targetid][pDbId]);
    mysql_tquery(g_SQL, query, "OnPunishWarnCount", "ddds", playerid, targetid, g_AccountRace[targetid], reason);
    return 1;
}

forward OnPunishWarnCount(playerid, targetid, target_race, const reason[]);
public OnPunishWarnCount(playerid, targetid, target_race, const reason[])
{
    if (target_race != g_AccountRace[targetid] || !Account_IsLoggedIn(playerid))
    {
        return 1;
    }

    new total;
    cache_get_value_name_int(0, "total", total);

    new admin_name[MAX_PLAYER_NAME + 1], target_name[MAX_PLAYER_NAME + 1], message[192];
    Account_GetDisplayName(playerid, admin_name);
    Account_GetDisplayName(targetid, target_name);
    format(message, sizeof(message), "[ADMIN] %s je upozorio igraca %s (%d/%d). Razlog: %s", admin_name, target_name, total, MAX_WARNINGS, reason);
    Punish_Announce(message);
    Staff_Log(playerid, "warn", targetid, reason);

    if (total >= MAX_WARNINGS)
    {
        new query[128];
        mysql_format(g_SQL, query, sizeof(query), "UPDATE `warnings` SET `active` = 0 WHERE `account_id` = %d", g_Player[targetid][pDbId]);
        mysql_tquery(g_SQL, query);

        format(message, sizeof(message), "%d upozorenja", MAX_WARNINGS);
        Punish_BanOnline(playerid, targetid, WARNING_BAN_DAYS, message);
    }
    return 1;
}

cmd:unwarn(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_SENIOR))
    {
        return 1;
    }

    new targetid;
    if (sscanf(params, "u", targetid))
    {
        Msg_Usage(playerid, "/unwarn [id ili ime] (skida poslednje upozorenje)");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }

    new query[192];
    mysql_format(g_SQL, query, sizeof(query),
        "UPDATE `warnings` SET `active` = 0 WHERE `account_id` = %d AND `active` = 1 ORDER BY `id` DESC LIMIT 1", g_Player[targetid][pDbId]);
    mysql_tquery(g_SQL, query, "OnPunishUnwarn", "dd", playerid, targetid);
    return 1;
}

forward OnPunishUnwarn(playerid, targetid);
public OnPunishUnwarn(playerid, targetid)
{
    if (!Account_IsLoggedIn(playerid))
    {
        return 1;
    }
    if (cache_affected_rows() == 0)
    {
        Msg_Error(playerid, "Taj igrac nema aktivnih upozorenja.");
        return 1;
    }

    new target_name[MAX_PLAYER_NAME + 1];
    Account_GetDisplayName(targetid, target_name);
    Msg_Admin(playerid, "Skinuli ste poslednje upozorenje igracu %s.", target_name);
    Staff_NotifyTarget(playerid, targetid, "vam je skinuo jedno upozorenje.");
    Staff_Log(playerid, "unwarn", targetid, "");
    return 1;
}

cmd:warns(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }

    new targetid;
    if (sscanf(params, "u", targetid))
    {
        Msg_Usage(playerid, "/warns [id ili ime]");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }

    new query[256];
    mysql_format(g_SQL, query, sizeof(query),
        "SELECT `admin_name`, `reason`, DATE_FORMAT(`created_at`, '%%d.%%m.%%Y.') AS `date_text` " \
        "FROM `warnings` WHERE `account_id` = %d AND `active` = 1 ORDER BY `id`", g_Player[targetid][pDbId]);
    mysql_tquery(g_SQL, query, "OnPunishWarnList", "dd", playerid, targetid);
    return 1;
}

forward OnPunishWarnList(playerid, targetid);
public OnPunishWarnList(playerid, targetid)
{
    if (!Account_IsLoggedIn(playerid))
    {
        return 1;
    }

    new rows = cache_num_rows();
    new body[1024], line[192], admin_name[MAX_PLAYER_NAME + 1], reason[128], date_text[16];

    strcat(body, C_SERVER "Datum\t" C_SERVER "Admin\t" C_SERVER "Razlog\n");
    for (new i = 0; i < rows; i++)
    {
        cache_get_value_name(i, "admin_name", admin_name);
        cache_get_value_name(i, "reason", reason);
        cache_get_value_name(i, "date_text", date_text);
        format(line, sizeof(line), C_WHITE "%s\t%s\t%s\n", date_text, admin_name, reason);
        strcat(body, line);
    }
    if (rows == 0)
    {
        strcat(body, C_WHITE "-\tNema aktivnih upozorenja\t-\n");
    }

    new title[64];
    format(title, sizeof(title), "Upozorenja: %s (%d/%d)", g_Player[targetid][pName], rows, MAX_WARNINGS);
    ShowPlayerDialog(playerid, DIALOG_ADMIN_INFO, DIALOG_STYLE_TABLIST_HEADERS, title, body, "Zatvori", "");
    return 1;
}

cmd:mute(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }

    new targetid, minutes, reason[96];
    if (sscanf(params, "uds[96]", targetid, minutes, reason) || minutes < 1 || minutes > MAX_PUNISH_MINUTES)
    {
        Msg_Usage(playerid, "/mute [id ili ime] [minuta 1-%d] [razlog]", MAX_PUNISH_MINUTES);
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid, "Ne mozete dati mute samom sebi.") || !Staff_CheckOutranks(playerid, targetid))
    {
        return 1;
    }

    g_Player[targetid][pMuteUntil] = gettime() + minutes * 60;
    Account_Save(targetid);

    new admin_name[MAX_PLAYER_NAME + 1], target_name[MAX_PLAYER_NAME + 1], message[192];
    Account_GetDisplayName(playerid, admin_name);
    Account_GetDisplayName(targetid, target_name);
    format(message, sizeof(message), "[ADMIN] %s je dao mute igracu %s na %d min. Razlog: %s", admin_name, target_name, minutes, reason);
    Punish_Announce(message);

    format(message, sizeof(message), "%d min, %s", minutes, reason);
    Staff_Log(playerid, "mute", targetid, message);
    return 1;
}

cmd:unmute(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }

    new targetid;
    if (sscanf(params, "u", targetid))
    {
        Msg_Usage(playerid, "/unmute [id ili ime]");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }
    if (!Punish_IsMuted(targetid))
    {
        Msg_Error(playerid, "Taj igrac nema mute.");
        return 1;
    }

    g_Player[targetid][pMuteUntil] = 0;
    Account_Save(targetid);

    new target_name[MAX_PLAYER_NAME + 1];
    Account_GetDisplayName(targetid, target_name);
    Msg_Admin(playerid, "Skinuli ste mute igracu %s.", target_name);
    Staff_NotifyTarget(playerid, targetid, "vam je skinuo mute.");
    Staff_Log(playerid, "unmute", targetid, "");
    return 1;
}

cmd:nmute(playerid, params[])
{
    if (!Staff_CheckHelper(playerid, HELPER_LEVEL_SENIOR))
    {
        return 1;
    }

    new targetid, minutes;
    if (sscanf(params, "ud", targetid, minutes) || minutes < 1 || minutes > MAX_PUNISH_MINUTES)
    {
        Msg_Usage(playerid, "/nmute [id ili ime] [minuta 1-%d] (zabrana pisanja u newbie chat)", MAX_PUNISH_MINUTES);
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid, "Ne mozete dati newbie mute samom sebi."))
    {
        return 1;
    }
    if (Staff_IsStaff(targetid))
    {
        Msg_Error(playerid, "Clanovima tima se ne moze dati newbie mute.");
        return 1;
    }

    g_Player[targetid][pNewbieMuteUntil] = gettime() + minutes * 60;
    Account_Save(targetid);

    new name[MAX_PLAYER_NAME + 1], target_name[MAX_PLAYER_NAME + 1], message[160];
    Account_GetDisplayName(playerid, name);
    Account_GetDisplayName(targetid, target_name);
    format(message, sizeof(message), "%s vam je zabranio pisanje u newbie chat na %d min.", name, minutes);
    Staff_MessageAs(playerid, targetid, message);
    format(message, sizeof(message), "[H] %s je dao newbie mute igracu %s na %d min.", name, target_name, minutes);
    Staff_SendToHelpers(message);
    return 1;
}

cmd:unnmute(playerid, params[])
{
    if (!Staff_CheckHelper(playerid, HELPER_LEVEL_SENIOR))
    {
        return 1;
    }

    new targetid;
    if (sscanf(params, "u", targetid))
    {
        Msg_Usage(playerid, "/unnmute [id ili ime]");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }

    g_Player[targetid][pNewbieMuteUntil] = 0;
    Account_Save(targetid);

    new name[MAX_PLAYER_NAME + 1], target_name[MAX_PLAYER_NAME + 1], message[144];
    Account_GetDisplayName(playerid, name);
    Account_GetDisplayName(targetid, target_name);

    format(message, sizeof(message), "Skinuli ste newbie mute igracu %s.", target_name);
    Staff_MessageAs(playerid, playerid, message);
    format(message, sizeof(message), "%s vam je skinuo newbie mute. Ponovo mozete pisati u newbie chat.", name);
    Staff_MessageAs(playerid, targetid, message);
    return 1;
}

cmd:ajail(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }

    new max_minutes = (g_Player[playerid][pAdminLevel] >= ADMIN_LEVEL_ADMIN) ? MAX_PUNISH_MINUTES : TRIAL_MAX_JAIL_MINUTES;

    new targetid, minutes, reason[96];
    if (sscanf(params, "uds[96]", targetid, minutes, reason) || minutes < 1 || minutes > max_minutes)
    {
        Msg_Usage(playerid, "/ajail [id ili ime] [minuta 1-%d] [razlog]", max_minutes);
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid, "Ne mozete zatvoriti samog sebe.") || !Staff_CheckOutranks(playerid, targetid))
    {
        return 1;
    }

    g_Player[targetid][pJailSeconds] = minutes * 60;
    strcopy(g_Player[targetid][pJailReason], reason, 96);
    Punish_PutInJail(targetid);
    Account_Save(targetid);

    new admin_name[MAX_PLAYER_NAME + 1], target_name[MAX_PLAYER_NAME + 1], message[192];
    Account_GetDisplayName(playerid, admin_name);
    Account_GetDisplayName(targetid, target_name);
    format(message, sizeof(message), "[ADMIN] %s je poslao igraca %s u admin zatvor na %d min. Razlog: %s", admin_name, target_name, minutes, reason);
    Punish_Announce(message);

    format(message, sizeof(message), "%d min, %s", minutes, reason);
    Staff_Log(playerid, "ajail", targetid, message);
    return 1;
}

cmd:unjail(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }

    new targetid;
    if (sscanf(params, "u", targetid))
    {
        Msg_Usage(playerid, "/unjail [id ili ime]");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }
    if (g_Player[targetid][pJailSeconds] <= 0)
    {
        Msg_Error(playerid, "Taj igrac nije u admin zatvoru.");
        return 1;
    }

    Punish_Release(targetid);

    new target_name[MAX_PLAYER_NAME + 1];
    Account_GetDisplayName(targetid, target_name);
    Msg_Admin(playerid, "Pustili ste igraca %s iz admin zatvora.", target_name);
    Staff_NotifyTarget(playerid, targetid, "vas je pustio iz admin zatvora.");
    Staff_Log(playerid, "unjail", targetid, "");
    return 1;
}

cmd:pmspy(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }

    g_Staff[playerid][sPmSpy] = !g_Staff[playerid][sPmSpy];
    if (g_Staff[playerid][sPmSpy])
    {
        Msg_Admin(playerid, "Pracenje privatnih poruka je ukljuceno.");
    }
    else
    {
        Msg_Admin(playerid, "Pracenje privatnih poruka je iskljuceno.");
    }
    Staff_Log(playerid, g_Staff[playerid][sPmSpy] ? ("pmspy_on") : ("pmspy_off"), INVALID_PLAYER_ID, "");
    return 1;
}
