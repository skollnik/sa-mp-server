// Bumped on every connect so a delayed kick never hits a different player who got the same ID meanwhile.
new g_ConnectSerial[MAX_PLAYERS];

stock Utils_OnPlayerConnect(playerid)
{
    g_ConnectSerial[playerid]++;
}

// Kick after a short delay so the last message/dialog actually reaches the client.
stock KickWithDelay(playerid, delay_ms = 500)
{
    SetTimerEx("Utils_DoKick", delay_ms, false, "dd", playerid, g_ConnectSerial[playerid]);
}

forward Utils_DoKick(playerid, serial);
public Utils_DoKick(playerid, serial)
{
    if (serial == g_ConnectSerial[playerid])
    {
        Kick(playerid);
    }
    return 1;
}
