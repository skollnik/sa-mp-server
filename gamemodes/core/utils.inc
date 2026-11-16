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

// freeze is false for a player an admin froze, so the loading does not unfreeze them.
stock Utils_MoveInto(playerid, Float:x, Float:y, Float:z, Float:angle, interior, world, bool:freeze = true)
{
    SetPlayerInterior(playerid, interior);
    SetPlayerVirtualWorld(playerid, world);
    AC_ExpectTeleport(playerid, x, y, z);
    Streamer_UpdateEx(playerid, x, y, z, world, interior, STREAMER_TYPE_OBJECT, UTILS_PRELOAD_MS, freeze ? 1 : 0);
    SetPlayerFacingAngle(playerid, angle);
    SetCameraBehindPlayer(playerid);
}

// The lowest DB id not in use, so a deleted entry's id goes to the next new one and the ids admins type stay short.
// finder is the module's public "slot by DB id" function (-1 when free). Call it before the new slot is marked used,
// since a reused slot still holds the deleted entry's id.
stock Utils_FreeDbId(const finder[])
{
    new dbid = 1;
    while (CallLocalFunction(finder, "d", dbid) != -1)
    {
        dbid++;
    }
    return dbid;
}

stock Float:Utils_FloatMin(Float:a, Float:b)
{
    return a < b ? a : b;
}

// The client plays an animation from a library it has not loaded only on the second try, so every library the
// gamemode uses is loaded once per spawn.
new const g_AnimLibraries[][] = { "FOOD", "BOMBER", "BEACH" };

stock Utils_PreloadAnimations(playerid)
{
    for (new i = 0; i < sizeof(g_AnimLibraries); i++)
    {
        ApplyAnimation(playerid, g_AnimLibraries[i], "null", 4.1, false, false, false, false, 0, SYNC_NONE);
    }
}
