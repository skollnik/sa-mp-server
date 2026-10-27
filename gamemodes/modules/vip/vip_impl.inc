stock Vip_Init()
{
    // Every grant is kept so paid months can be checked later.
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `vip_history` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`account_id` INT UNSIGNED NOT NULL," \
        "`name` VARCHAR(24) NOT NULL," \
        "`vip_level` TINYINT UNSIGNED NOT NULL," \
        "`days` INT NOT NULL," \
        "`expires_at` DATETIME NULL," \
        "`admin_name` VARCHAR(24) NOT NULL," \
        "`created_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP," \
        "INDEX `vip_account` (`account_id`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
}

// Expiry is checked on every read, so a status that runs out while the player is online stops working immediately.
stock Vip_GetLevel(playerid)
{
    if (g_Player[playerid][pVipLevel] > 0 && g_Player[playerid][pVipUntil] > gettime())
    {
        return g_Player[playerid][pVipLevel];
    }
    return 0;
}

stock bool:Vip_IsVip(playerid)
{
    return Vip_GetLevel(playerid) > 0;
}

stock Vip_DaysLeft(playerid)
{
    return (g_Player[playerid][pVipUntil] - gettime() + SECONDS_PER_DAY - 1) / SECONDS_PER_DAY;
}

stock Vip_Cooldown(playerid, base_seconds)
{
    return base_seconds * (100 - g_VipCooldownCut[Vip_GetLevel(playerid)]) / 100;
}

stock Vip_OnLogin(playerid)
{
    if (g_Player[playerid][pVipLevel] == 0)
    {
        return;
    }

    if (!Vip_IsVip(playerid))
    {
        Msg_Info(playerid, "Vas %s je istekao. Za produzenje se javite vlasniku servera.", g_VipNames[g_Player[playerid][pVipLevel]]);
        Vip_Store(playerid, 0, 0);
        return;
    }

    new days = Vip_DaysLeft(playerid);
    if (days <= VIP_EXPIRY_WARNING_DAYS)
    {
        Msg_Info(playerid, "Vas %s istice za %d dana.", g_VipNames[g_Player[playerid][pVipLevel]], days);
    }
    else
    {
        Msg_Info(playerid, "%s je aktivan jos %d dana. Pogodnosti: /vip", g_VipNames[g_Player[playerid][pVipLevel]], days);
    }
}

// The VIP module is the only writer of the VIP columns, so autosave can never overwrite a fresh grant.
stock Vip_Store(playerid, level, until)
{
    g_Player[playerid][pVipLevel] = level;
    g_Player[playerid][pVipUntil] = until;

    new query[128];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `accounts` SET `vip_level` = %d, `vip_until` = %d WHERE `id` = %d",
        level, until, g_Player[playerid][pDbId]);
    mysql_tquery(g_SQL, query);
}

// A grant of the same level extends the remaining time; a different level starts from today.
stock Vip_Grant(adminid, targetid, level, days)
{
    new now = gettime(), start = now;
    if (level > 0 && Vip_GetLevel(targetid) == level)
    {
        start = g_Player[targetid][pVipUntil];
    }

    new until = (level > 0) ? start + days * SECONDS_PER_DAY : 0;
    Vip_Store(targetid, level, until);

    new query[384];
    mysql_format(g_SQL, query, sizeof(query),
        "INSERT INTO `vip_history` (`account_id`, `name`, `vip_level`, `days`, `expires_at`, `admin_name`) " \
        "VALUES (%d, '%e', %d, %d, IF(%d > 0, FROM_UNIXTIME(%d), NULL), '%e')",
        g_Player[targetid][pDbId], g_Player[targetid][pName], level, days, until, until, g_Player[adminid][pName]);
    mysql_tquery(g_SQL, query);
}

stock Vip_SendToVips(const message[])
{
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (Account_IsLoggedIn(i) && (Vip_IsVip(i) || Staff_IsAdmin(i)))
        {
            Chat_Send(i, g_VipColors[VIP_GOLD], message);
        }
    }
}
