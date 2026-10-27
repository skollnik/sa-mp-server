cmd:kanali(playerid, params[])
{
    Channel_ShowDialog(playerid);
    return 1;
}

cmd:togooc(playerid, params[])
{
    Channel_Toggle(playerid, CHANNEL_OOC);
    return 1;
}

cmd:togpm(playerid, params[])
{
    Channel_Toggle(playerid, CHANNEL_PM);
    return 1;
}

cmd:tognewbie(playerid, params[])
{
    Channel_Toggle(playerid, CHANNEL_NEWBIE);
    return 1;
}

cmd:pmspy(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }

    Channel_Toggle(playerid, CHANNEL_PM_SPY);
    Staff_Log(playerid, Channel_IsOn(playerid, CHANNEL_PM_SPY) ? ("pmspy_on") : ("pmspy_off"), INVALID_PLAYER_ID, "");
    return 1;
}

alias:kanali("channels")
flags:kanali(CMD_FLAG_JAIL_OK)
flags:togooc(CMD_FLAG_JAIL_OK)
flags:togpm(CMD_FLAG_JAIL_OK)
flags:tognewbie(CMD_FLAG_JAIL_OK)
