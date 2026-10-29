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

// Moving into an interior: the objects around the destination are streamed first and the player is held
// still meanwhile, otherwise they fall through a floor that has not loaded yet.
#define UTILS_PRELOAD_MS    (1500)

stock Utils_MoveInto(playerid, Float:x, Float:y, Float:z, Float:angle, interior, world)
{
    SetPlayerInterior(playerid, interior);
    SetPlayerVirtualWorld(playerid, world);
    AC_ExpectTeleport(playerid, x, y, z);
    Streamer_UpdateEx(playerid, x, y, z, world, interior, STREAMER_TYPE_OBJECT, UTILS_PRELOAD_MS, 1);
    SetPlayerFacingAngle(playerid, angle);
    SetCameraBehindPlayer(playerid);
}

stock Float:Utils_FloatMin(Float:a, Float:b)
{
    return a < b ? a : b;
}
