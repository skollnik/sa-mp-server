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
