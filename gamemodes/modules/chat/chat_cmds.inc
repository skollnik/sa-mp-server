cmd:s(playerid, params[])
{
    if (isnull(params))
    {
        Msg_Usage(playerid, "/s [tekst] (vika)");
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1], message[192];
    Account_GetDisplayName(playerid, name);
    format(message, sizeof(message), "%s vice: %s!", name, params);
    Chat_SendNearby(playerid, CHAT_RADIUS_SHOUT, COLOR_SHOUT, message, true);
    return 1;
}

cmd:l(playerid, params[])
{
    if (isnull(params))
    {
        Msg_Usage(playerid, "/l [tekst] (tihi govor)");
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1], message[192];
    Account_GetDisplayName(playerid, name);
    format(message, sizeof(message), "%s kaze (tiho): %s", name, params);
    Chat_SendNearby(playerid, CHAT_RADIUS_LOW, COLOR_WHITE, message, true);
    return 1;
}

cmd:w(playerid, params[])
{
    new targetid, text[128];
    if (sscanf(params, "us[128]", targetid, text))
    {
        Msg_Usage(playerid, "/w [id ili ime] [tekst] (sapat)");
        return 1;
    }
    if (targetid == INVALID_PLAYER_ID || !Account_IsLoggedIn(targetid))
    {
        Msg_Error(playerid, "Taj igrac nije pronadjen.");
        return 1;
    }
    if (targetid == playerid)
    {
        Msg_Error(playerid, "Ne mozete sapnuti samom sebi.");
        return 1;
    }
    if (!Chat_IsInRange(playerid, targetid, CHAT_RADIUS_WHISPER))
    {
        Msg_Error(playerid, "Morate biti blizu igraca da biste mu sapnuli.");
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1], target_name[MAX_PLAYER_NAME + 1], message[192];
    Account_GetDisplayName(playerid, name);
    Account_GetDisplayName(targetid, target_name);

    format(message, sizeof(message), "%s vam sapuce: %s", name, text);
    Chat_Send(targetid, COLOR_WHISPER, message);
    format(message, sizeof(message), "Sapucete %s: %s", target_name, text);
    Chat_Send(playerid, COLOR_WHISPER, message);
    return 1;
}

cmd:me(playerid, params[])
{
    if (isnull(params))
    {
        Msg_Usage(playerid, "/me [radnja]");
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1], message[192];
    Account_GetDisplayName(playerid, name);
    format(message, sizeof(message), "* %s %s", name, params);
    Chat_SendNearby(playerid, CHAT_RADIUS_ACTION, COLOR_PURPLE, message);
    return 1;
}

cmd:do(playerid, params[])
{
    if (isnull(params))
    {
        Msg_Usage(playerid, "/do [opis okoline ili stanja]");
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1], message[192];
    Account_GetDisplayName(playerid, name);
    format(message, sizeof(message), "* %s (( %s ))", params, name);
    Chat_SendNearby(playerid, CHAT_RADIUS_ACTION, COLOR_PURPLE, message);
    return 1;
}

cmd:ame(playerid, params[])
{
    if (isnull(params))
    {
        Msg_Usage(playerid, "/ame [radnja] (tekst iznad glave)");
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1], message[192];
    Account_GetDisplayName(playerid, name);
    format(message, sizeof(message), "* %s %s", name, params);
    Chat_ShowBubble(playerid, message);

    format(message, sizeof(message), "> %s", params);
    Chat_Send(playerid, COLOR_PURPLE, message);
    return 1;
}

cmd:b(playerid, params[])
{
    if (isnull(params))
    {
        Msg_Usage(playerid, "/b [tekst] (lokalni OOC)");
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1], message[192];
    Account_GetDisplayName(playerid, name);
    format(message, sizeof(message), "(( [%d] %s: %s ))", playerid, name, params);
    Chat_SendNearby(playerid, CHAT_RADIUS_OOC, COLOR_OOC_LOCAL, message);
    return 1;
}

cmd:o(playerid, params[])
{
    if (isnull(params))
    {
        Msg_Usage(playerid, "/o [tekst] (globalni OOC)");
        return 1;
    }
    if (!g_GlobalOocEnabled && !Staff_IsAdminOnDuty(playerid))
    {
        Msg_Error(playerid, "Globalni OOC chat je trenutno iskljucen.");
        return 1;
    }
    if (g_Player[playerid][pTogOoc])
    {
        Msg_Error(playerid, "Iskljucili ste globalni OOC. Ukljucite ga sa /togooc.");
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1], message[192];
    Account_GetDisplayName(playerid, name);
    format(message, sizeof(message), "(( [OOC] %s: %s ))", name, params);
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (Account_IsLoggedIn(i) && !g_Player[i][pTogOoc])
        {
            Chat_Send(i, COLOR_OOC_GLOBAL, message);
        }
    }
    return 1;
}

cmd:togooc(playerid, params[])
{
    g_Player[playerid][pTogOoc] = !g_Player[playerid][pTogOoc];
    if (g_Player[playerid][pTogOoc])
    {
        Msg_Info(playerid, "Globalni OOC chat je iskljucen za vas. Ponovo ga ukljucite sa /togooc.");
    }
    else
    {
        Msg_Info(playerid, "Globalni OOC chat je ukljucen za vas.");
    }
    return 1;
}

cmd:togpm(playerid, params[])
{
    g_Player[playerid][pTogPm] = !g_Player[playerid][pTogPm];
    if (g_Player[playerid][pTogPm])
    {
        Msg_Info(playerid, "Privatne poruke su iskljucene. Admini i dalje mogu da vam pisu.");
    }
    else
    {
        Msg_Info(playerid, "Privatne poruke su ukljucene.");
    }
    return 1;
}

cmd:ado(playerid, params[])
{
    if (isnull(params))
    {
        Msg_Usage(playerid, "/ado [opis] (opis iznad glave)");
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1], message[192];
    Account_GetDisplayName(playerid, name);
    format(message, sizeof(message), "* %s (( %s ))", params, name);
    Chat_ShowBubble(playerid, message);

    format(message, sizeof(message), "> %s", params);
    Chat_Send(playerid, COLOR_PURPLE, message);
    return 1;
}

cmd:pm(playerid, params[])
{
    new targetid, text[128];
    if (sscanf(params, "us[128]", targetid, text))
    {
        Msg_Usage(playerid, "/pm [id ili ime] [tekst]");
        return 1;
    }
    if (targetid == INVALID_PLAYER_ID || !Account_IsLoggedIn(targetid))
    {
        Msg_Error(playerid, "Taj igrac nije pronadjen.");
        return 1;
    }
    if (targetid == playerid)
    {
        Msg_Error(playerid, "Ne mozete poslati poruku sebi.");
        return 1;
    }
    if (g_Player[targetid][pTogPm] && !Staff_IsAdmin(playerid))
    {
        Msg_Error(playerid, "Taj igrac je iskljucio privatne poruke.");
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1], target_name[MAX_PLAYER_NAME + 1], message[192];
    Account_GetDisplayName(playerid, name);
    Account_GetDisplayName(targetid, target_name);

    format(message, sizeof(message), "(( PM od %s [%d]: %s ))", name, playerid, text);
    Chat_Send(targetid, COLOR_PM, message);
    format(message, sizeof(message), "(( PM za %s [%d]: %s ))", target_name, targetid, text);
    Chat_Send(playerid, COLOR_PM, message);

    format(message, sizeof(message), "[PM SPY] %s [%d] -> %s [%d]: %s", name, playerid, target_name, targetid, text);
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (i != playerid && i != targetid && Account_IsLoggedIn(i) && g_Staff[i][sPmSpy])
        {
            Chat_Send(i, COLOR_GREY, message);
        }
    }
    return 1;
}

// Commands marked as chat are blocked while the player is muted (checked in OnPlayerCommandReceived).
flags:s(CMD_FLAG_CHAT)
flags:l(CMD_FLAG_CHAT)
flags:w(CMD_FLAG_CHAT)
flags:me(CMD_FLAG_CHAT)
flags:do(CMD_FLAG_CHAT)
flags:ame(CMD_FLAG_CHAT)
flags:ado(CMD_FLAG_CHAT)
flags:b(CMD_FLAG_CHAT)
flags:o(CMD_FLAG_CHAT)
flags:pm(CMD_FLAG_CHAT)
