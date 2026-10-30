stock bool:Setting_IsOn(playerid, E_SETTING:setting)
{
    // A set bit means the player switched the setting off; everything starts on.
    return (g_Player[playerid][pSettingBits] & (1 << _:setting)) == 0;
}

stock Setting_Toggle(playerid, E_SETTING:setting)
{
    g_Player[playerid][pSettingBits] ^= (1 << _:setting);
    Hud_Refresh(playerid);
    Speedo_Refresh(playerid);
}

stock Settings_Show(playerid)
{
    new body[512], line[96];
    strcat(body, C_SERVER "Podesavanje\t" C_SERVER "Stanje\n");

    new bool:hud = Setting_IsOn(playerid, SETTING_HUD);
    for (new E_SETTING:setting; setting < E_SETTING; setting++)
    {
        if (setting != SETTING_HUD && !hud)
        {
            format(line, sizeof(line), C_GREY "%s\t" C_GREY "(HUD je iskljucen)\n", g_SettingNames[setting]);
        }
        else
        {
            format(line, sizeof(line), C_WHITE "%s\t%s\n", g_SettingNames[setting], Setting_IsOn(playerid, setting) ? (C_GREEN "ukljuceno") : (C_RED "iskljuceno"));
        }
        strcat(body, line);
    }

    strcat(body, C_WHITE "Kanali (chatovi i obavestenja)\t" C_SERVER ">>\n");
    ShowPlayerDialog(playerid, DIALOG_SETTINGS, DIALOG_STYLE_TABLIST_HEADERS, SERVER_NAME " - Podesavanja (klik menja)", body, "Izaberi", "Zatvori");
}

stock bool:Settings_OnDialogResponse(playerid, dialogid, response, listitem)
{
    if (dialogid != DIALOG_SETTINGS)
    {
        return false;
    }
    if (!response)
    {
        return true;
    }

    if (listitem == SETTINGS_ROW_CHANNELS)
    {
        Channel_ShowDialog(playerid);
    }
    else if (listitem >= 0 && listitem < _:E_SETTING)
    {
        new E_SETTING:setting = E_SETTING:listitem;
        if (setting != SETTING_HUD && !Setting_IsOn(playerid, SETTING_HUD))
        {
            Msg_Error(playerid, "Prvo ukljucite HUD (prvi red).");
        }
        else
        {
            Setting_Toggle(playerid, setting);
        }
        Settings_Show(playerid);
    }
    return true;
}

cmd:podesavanja(playerid, params[])
{
    Settings_Show(playerid);
    return 1;
}

alias:podesavanja("settings")
flags:podesavanja(CMD_FLAG_JAIL_OK)
