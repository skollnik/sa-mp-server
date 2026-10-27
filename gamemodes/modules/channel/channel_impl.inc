stock bool:Channel_IsAvailable(playerid, E_CHANNEL:channel)
{
    switch (channel)
    {
        case CHANNEL_OOC, CHANNEL_PM, CHANNEL_NEWBIE:
        {
            return true;
        }
        case CHANNEL_VIP:
        {
            return Vip_IsVip(playerid) || Staff_IsAdmin(playerid);
        }
        case CHANNEL_HELPER, CHANNEL_QUESTIONS:
        {
            return Staff_IsStaff(playerid);
        }
        case CHANNEL_ADMIN, CHANNEL_REPORTS, CHANNEL_ANTICHEAT, CHANNEL_SHARED_IP:
        {
            return Staff_IsAdmin(playerid);
        }
        case CHANNEL_PM_SPY:
        {
            return g_Player[playerid][pAdminLevel] >= ADMIN_LEVEL_HEAD;
        }
    }
    return false;
}

// What staff on duty are there to handle cannot be switched off while on duty.
stock bool:Channel_IsLocked(playerid, E_CHANNEL:channel)
{
    switch (channel)
    {
        case CHANNEL_REPORTS, CHANNEL_ANTICHEAT:
        {
            return g_Staff[playerid][sAdminDuty];
        }
        case CHANNEL_QUESTIONS:
        {
            return g_Staff[playerid][sHelperDuty] || g_Staff[playerid][sAdminDuty];
        }
    }
    return false;
}

stock bool:Channel_IsOn(playerid, E_CHANNEL:channel)
{
    if (!Channel_IsAvailable(playerid, channel))
    {
        return false;
    }
    if (Channel_IsLocked(playerid, channel))
    {
        return true;
    }

    // A set bit means the player changed the channel away from its default.
    new bool:changed = (g_Player[playerid][pChannelBits] & (1 << _:channel)) != 0;
    return g_ChannelDefaultOn[channel] != changed;
}

stock Channel_Toggle(playerid, E_CHANNEL:channel)
{
    if (!Channel_IsAvailable(playerid, channel))
    {
        Msg_Error(playerid, "Nemate dozvolu za ovu komandu.");
        return;
    }
    if (Channel_IsLocked(playerid, channel))
    {
        Msg_Error(playerid, "Kanal '%s' ne moze da se iskljuci dok ste na duznosti.", g_ChannelNames[channel]);
        return;
    }

    g_Player[playerid][pChannelBits] ^= (1 << _:channel);
    Msg_Info(playerid, "Kanal '%s' je sada %s.", g_ChannelNames[channel], Channel_IsOn(playerid, channel) ? ("ukljucen") : ("iskljucen"));
}

stock Channel_ShowDialog(playerid)
{
    new body[1024], line[96], count;
    strcat(body, C_SERVER "Kanal\t" C_SERVER "Stanje\n");

    for (new E_CHANNEL:channel; channel < E_CHANNEL; channel++)
    {
        if (!Channel_IsAvailable(playerid, channel))
        {
            continue;
        }

        if (Channel_IsLocked(playerid, channel))
        {
            format(line, sizeof(line), C_WHITE "%s\t" C_GREEN "ukljucen " C_GREY "(na duznosti)\n", g_ChannelNames[channel]);
        }
        else if (Channel_IsOn(playerid, channel))
        {
            format(line, sizeof(line), C_WHITE "%s\t" C_GREEN "ukljucen\n", g_ChannelNames[channel]);
        }
        else
        {
            format(line, sizeof(line), C_WHITE "%s\t" C_RED "iskljucen\n", g_ChannelNames[channel]);
        }
        strcat(body, line);
        g_ChannelDialogRows[playerid][E_CHANNEL:count++] = channel;
    }

    g_ChannelDialogRowCount[playerid] = count;
    ShowPlayerDialog(playerid, DIALOG_CHANNELS, DIALOG_STYLE_TABLIST_HEADERS, SERVER_NAME " - Kanali (klik menja stanje)", body, "Promeni", "Zatvori");
}

stock bool:Channel_OnDialogResponse(playerid, dialogid, response, listitem)
{
    if (dialogid != DIALOG_CHANNELS)
    {
        return false;
    }
    if (response && listitem >= 0 && listitem < g_ChannelDialogRowCount[playerid])
    {
        Channel_Toggle(playerid, g_ChannelDialogRows[playerid][E_CHANNEL:listitem]);
        Channel_ShowDialog(playerid);
    }
    return true;
}

// Goes to every logged-in player who has the channel on (the channel itself decides who may have it).
stock Channel_Send(E_CHANNEL:channel, color, const message[], bool:keep_colors = false)
{
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (Account_IsLoggedIn(i) && Channel_IsOn(i, channel))
        {
            Chat_Send(i, color, message, keep_colors);
        }
    }
}
