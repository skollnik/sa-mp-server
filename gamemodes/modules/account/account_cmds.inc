cmd:promenilozinku(playerid, params[])
{
    if (g_Player[playerid][pAwaitingBcrypt])
    {
        return 1;
    }

    g_Player[playerid][pOldPasswordVerified] = false;
    Account_ShowChangePassOld(playerid, "");
    return 1;
}

// Owner only: allow a name that is not in the Firstname_Lastname format.
cmd:dozvoliime(playerid, params[])
{
    if (g_Player[playerid][pAdminLevel] < ADMIN_LEVEL_OWNER)
    {
        Msg_Error(playerid, "Nemate dozvolu za ovu komandu.");
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1];
    if (sscanf(params, "s[25]", name) || strlen(name) < 3)
    {
        Msg_Usage(playerid, "/dozvoliime [ime]");
        return 1;
    }

    new query[160];
    mysql_format(g_SQL, query, sizeof(query), "INSERT IGNORE INTO `name_exemptions` (`name`, `granted_by`) VALUES ('%e', '%e')",
        name, g_Player[playerid][pName]);
    mysql_tquery(g_SQL, query);

    Msg_Success(playerid, "Ime '%s' sada moze da se koristi na serveru.", name);
    return 1;
}

// Owner only: take a non-format name back off the allowed list.
cmd:zabraniime(playerid, params[])
{
    if (g_Player[playerid][pAdminLevel] < ADMIN_LEVEL_OWNER)
    {
        Msg_Error(playerid, "Nemate dozvolu za ovu komandu.");
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1];
    if (sscanf(params, "s[25]", name))
    {
        Msg_Usage(playerid, "/zabraniime [ime]");
        return 1;
    }

    new query[128];
    mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `name_exemptions` WHERE `name` = '%e'", name);
    mysql_tquery(g_SQL, query);

    Msg_Success(playerid, "Ime '%s' je uklonjeno sa liste dozvoljenih imena.", name);
    return 1;
}

flags:promenilozinku(CMD_FLAG_JAIL_OK)

cmd:stats(playerid, params[])
{
    new body[1024], vip[48];
    if (Vip_IsVip(playerid))
    {
        format(vip, sizeof(vip), "%s%s" C_WHITE " (jos %d dana)", g_VipColorTags[Vip_GetLevel(playerid)], g_VipNames[Vip_GetLevel(playerid)], Vip_DaysLeft(playerid));
    }
    else
    {
        strcopy(vip, "nema");
    }

    new played = g_Player[playerid][pPlayedSeconds] + gettime() - g_Player[playerid][pSessionStart];
    new phone[16];
    if (g_Player[playerid][pPhoneNumber] > 0) format(phone, sizeof(phone), "%d", g_Player[playerid][pPhoneNumber]);
    else strcopy(phone, "nema");

    format(body, sizeof(body),
        C_WHITE "Ime: " C_SERVER "%s\n" \
        C_WHITE "Pol: %s   Godine: %d\n\n" \
        "Nivo: " C_SERVER "%d" C_WHITE "   Respect: %d/%d do nivoa %d\n" \
        "Sati igre: %d   Do plate: %d/%d aktivnih minuta%s\n\n" \
        "Novac kod sebe: " C_GREEN "$%d\n" C_WHITE "Banka: " C_GREEN "$%d\n\n" \
        C_WHITE "Licna karta: %s   Telefon: %s\nVIP: %s",
        Account_Name(playerid), g_Player[playerid][pGender] == GENDER_FEMALE ? ("zensko") : ("musko"), g_Player[playerid][pAge],
        g_Player[playerid][pLevel], g_Player[playerid][pRespect], Payday_RespectNeeded(playerid), g_Player[playerid][pLevel] + 1,
        played / 3600, g_Player[playerid][pPaydayMinutes], PAYDAY_MIN_MINUTES, Payday_IsAfk(playerid) ? (C_RED " (AFK)" C_WHITE) : (""),
        g_Player[playerid][pCash], g_Player[playerid][pBank],
        g_Player[playerid][pHasIdCard] ? ("ima") : ("nema"), phone, vip);
    ShowPlayerDialog(playerid, DIALOG_ADMIN_INFO, DIALOG_STYLE_MSGBOX, SERVER_NAME " - Statistika", body, "Zatvori", "");
    return 1;
}
flags:stats(CMD_FLAG_JAIL_OK)
