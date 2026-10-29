// A player has only one checkpoint, shared by every system. Whoever set it owns it, and only the owner
// reacts when the player enters it, so e.g. a GPS marker can never complete a job route.
enum E_CP_OWNER
{
    CP_OWNER_NONE,
    CP_OWNER_GPS
}

new E_CP_OWNER:g_CheckpointOwner[MAX_PLAYERS];

stock Checkpoint_OnPlayerConnect(playerid)
{
    g_CheckpointOwner[playerid] = CP_OWNER_NONE;
}

// GPS is the lowest priority: it never takes the checkpoint from another system, while any system may take it from GPS.
stock bool:Checkpoint_Set(playerid, E_CP_OWNER:owner, Float:x, Float:y, Float:z, Float:radius)
{
    new E_CP_OWNER:current = g_CheckpointOwner[playerid];
    if (current != CP_OWNER_NONE && current != owner && owner == CP_OWNER_GPS)
    {
        return false;
    }
    if (current == CP_OWNER_GPS && owner != CP_OWNER_GPS)
    {
        Msg_Info(playerid, "GPS je iskljucen jer je checkpoint potreban drugom sistemu.");
    }

    DisablePlayerCheckpoint(playerid);
    SetPlayerCheckpoint(playerid, x, y, z, radius);
    g_CheckpointOwner[playerid] = owner;
    return true;
}

stock Checkpoint_Clear(playerid, E_CP_OWNER:owner)
{
    if (g_CheckpointOwner[playerid] != owner)
    {
        return;
    }
    DisablePlayerCheckpoint(playerid);
    g_CheckpointOwner[playerid] = CP_OWNER_NONE;
}

stock E_CP_OWNER:Checkpoint_GetOwner(playerid)
{
    return g_CheckpointOwner[playerid];
}

stock Checkpoint_OnPlayerEnter(playerid)
{
    switch (g_CheckpointOwner[playerid])
    {
        case CP_OWNER_GPS: Gps_OnArrive(playerid);
        default: {}
    }
}
