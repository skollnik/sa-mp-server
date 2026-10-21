cmd:s(playerid, params[])
{
    if (isnull(params))
    {
        SendClientMessage(playerid, COLOR_GREY, "KORISCENJE: /s [tekst] (vika)");
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
        SendClientMessage(playerid, COLOR_GREY, "KORISCENJE: /l [tekst] (tihi govor)");
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
        SendClientMessage(playerid, COLOR_GREY, "KORISCENJE: /w [id ili ime] [tekst] (sapat)");
        return 1;
    }
    if (targetid == INVALID_PLAYER_ID || !Account_IsLoggedIn(targetid) || targetid == playerid)
    {
        SendClientMessage(playerid, COLOR_GREY, "Taj igrac nije pronadjen.");
        return 1;
    }
    if (!Chat_IsInRange(playerid, targetid, CHAT_RADIUS_WHISPER))
    {
        SendClientMessage(playerid, COLOR_GREY, "Morate biti blizu igraca da biste mu sapnuli.");
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
        SendClientMessage(playerid, COLOR_GREY, "KORISCENJE: /me [radnja]");
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
        SendClientMessage(playerid, COLOR_GREY, "KORISCENJE: /do [opis okoline ili stanja]");
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
        SendClientMessage(playerid, COLOR_GREY, "KORISCENJE: /ame [radnja] (tekst iznad glave)");
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1], message[192];
    Account_GetDisplayName(playerid, name);
    format(message, sizeof(message), "* %s %s", name, params);
    SetPlayerChatBubble(playerid, message, COLOR_PURPLE, CHAT_RADIUS_ACTION, 8000);

    format(message, sizeof(message), "> %s", params);
    Chat_Send(playerid, COLOR_PURPLE, message);
    return 1;
}

cmd:b(playerid, params[])
{
    if (isnull(params))
    {
        SendClientMessage(playerid, COLOR_GREY, "KORISCENJE: /b [tekst] (lokalni OOC)");
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
        SendClientMessage(playerid, COLOR_GREY, "KORISCENJE: /o [tekst] (globalni OOC)");
        return 1;
    }
    if (!g_GlobalOocEnabled)
    {
        SendClientMessage(playerid, COLOR_GREY, "Globalni OOC chat je trenutno iskljucen.");
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1], message[192];
    Account_GetDisplayName(playerid, name);
    format(message, sizeof(message), "(( [OOC] %s: %s ))", name, params);
    Chat_SendToAll(COLOR_OOC_GLOBAL, message);
    return 1;
}

cmd:pm(playerid, params[])
{
    new targetid, text[128];
    if (sscanf(params, "us[128]", targetid, text))
    {
        SendClientMessage(playerid, COLOR_GREY, "KORISCENJE: /pm [id ili ime] [tekst]");
        return 1;
    }
    if (targetid == INVALID_PLAYER_ID || !Account_IsLoggedIn(targetid))
    {
        SendClientMessage(playerid, COLOR_GREY, "Taj igrac nije pronadjen.");
        return 1;
    }
    if (targetid == playerid)
    {
        SendClientMessage(playerid, COLOR_GREY, "Ne mozete poslati poruku sebi.");
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1], target_name[MAX_PLAYER_NAME + 1], message[192];
    Account_GetDisplayName(playerid, name);
    Account_GetDisplayName(targetid, target_name);

    format(message, sizeof(message), "(( PM od %s [%d]: %s ))", name, playerid, text);
    Chat_Send(targetid, COLOR_PM, message);
    format(message, sizeof(message), "(( PM za %s [%d]: %s ))", target_name, targetid, text);
    Chat_Send(playerid, COLOR_PM, message);
    return 1;
}
