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
        SendClientMessage(playerid, COLOR_GREY, "Nemate dozvolu za ovu komandu.");
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1];
    if (sscanf(params, "s[25]", name) || strlen(name) < 3)
    {
        SendClientMessage(playerid, COLOR_GREY, "KORISCENJE: /dozvoliime [ime]");
        return 1;
    }

    new query[160];
    mysql_format(g_SQL, query, sizeof(query), "INSERT IGNORE INTO `name_exemptions` (`name`, `granted_by`) VALUES ('%e', '%e')",
        name, g_Player[playerid][pName]);
    mysql_tquery(g_SQL, query);

    new message[96];
    format(message, sizeof(message), "Ime '%s' sada moze da se koristi na serveru.", name);
    SendClientMessage(playerid, COLOR_GREEN, message);
    return 1;
}
