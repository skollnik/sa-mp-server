// After death everyone wakes up in front of the hospital, hurt, as is usual on RP servers; injuries and treatment come
// with the hospital faction (phase 5).
#define RESPAWN_HEALTH          (50.0)

new bool:g_RespawnAfterDeath[MAX_PLAYERS];

stock Respawn_OnPlayerConnect(playerid)
{
    g_RespawnAfterDeath[playerid] = false;
}

stock Respawn_OnPlayerDeath(playerid)
{
    g_RespawnAfterDeath[playerid] = true;
    SetSpawnInfo(playerid, NO_TEAM, g_Player[playerid][pSkin], HOSPITAL_SPAWN_X, HOSPITAL_SPAWN_Y, HOSPITAL_SPAWN_Z, HOSPITAL_SPAWN_A);
}

// The fight that killed the player is over, so doors and houses open again right away.
stock Respawn_OnPlayerSpawn(playerid)
{
    if (!g_RespawnAfterDeath[playerid])
    {
        return;
    }
    g_RespawnAfterDeath[playerid] = false;
    g_Player[playerid][pLastDamage] = 0;
    SetPlayerInterior(playerid, 0);
    SetPlayerVirtualWorld(playerid, 0);
    SetPlayerHealth(playerid, RESPAWN_HEALTH);
    if (g_Player[playerid][pJailSeconds] == 0)
    {
        Msg_Info(playerid, "Probudili ste se ispred bolnice, povredjeni.");
    }
}
