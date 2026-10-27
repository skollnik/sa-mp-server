cmd:aclog(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }

    new targetid = Staff_ParseTarget(playerid, params, "/aclog [id ili ime] (poslednja anti-cheat upozorenja)");
    if (targetid == INVALID_PLAYER_ID)
    {
        return 1;
    }

    new query[256];
    mysql_format(g_SQL, query, sizeof(query),
        "SELECT `type`, `details`, `action`, DATE_FORMAT(`created_at`, '%%d.%%m. %%H:%%i') AS `time_text` " \
        "FROM `ac_logs` WHERE `account_id` = %d ORDER BY `id` DESC LIMIT 15", g_Player[targetid][pDbId]);
    mysql_tquery(g_SQL, query, "OnAcLogList", "dd", playerid, targetid);
    return 1;
}

forward OnAcLogList(playerid, targetid);
public OnAcLogList(playerid, targetid)
{
    if (!Account_IsLoggedIn(playerid))
    {
        return 1;
    }

    new rows = cache_num_rows();
    new body[2048], line[192], type[24], details[128], action[16], time_text[16];

    strcat(body, C_SERVER "Vreme\t" C_SERVER "Vrsta\t" C_SERVER "Detalji\t" C_SERVER "Akcija\n");
    for (new i = 0; i < rows; i++)
    {
        cache_get_value_name(i, "type", type);
        cache_get_value_name(i, "details", details);
        cache_get_value_name(i, "action", action);
        cache_get_value_name(i, "time_text", time_text);
        format(line, sizeof(line), C_WHITE "%s\t%s\t%s\t%s\n", time_text, type, details, strcmp(action, "kick") ? ("upozorenje") : (C_RED "kick"));
        strcat(body, line);
    }
    if (rows == 0)
    {
        strcat(body, C_WHITE "-\tNema zapisa\t-\t-\n");
    }

    new title[64];
    format(title, sizeof(title), "Anti-cheat: %s", g_Player[targetid][pName]);
    ShowPlayerDialog(playerid, DIALOG_ADMIN_INFO, DIALOG_STYLE_TABLIST_HEADERS, title, body, "Zatvori", "");
    return 1;
}
