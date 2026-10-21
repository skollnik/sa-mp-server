// Kick after a short delay so the last message/dialog actually reaches the client.
stock KickWithDelay(playerid, delay_ms = 500)
{
    SetTimerEx("Utils_DoKick", delay_ms, false, "d", playerid);
}

forward Utils_DoKick(playerid);
public Utils_DoKick(playerid)
{
    Kick(playerid);
    return 1;
}

stock GetPlayerNameEx(playerid)
{
    new name[MAX_PLAYER_NAME + 1];
    GetPlayerName(playerid, name, sizeof(name));
    return name;
}
