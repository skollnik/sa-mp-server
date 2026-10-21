// Roleplay gamemode entry point.
// Only includes and the main callbacks live here; each callback forwards to the modules.

#include <open.mp>

#undef MAX_PLAYERS
#define MAX_PLAYERS (50)

#include <a_mysql>
#include <sscanf2>
#include <streamer>
#include <samp_bcrypt>
#include <Pawn.CMD>

#include "core/config.inc"
#include "core/colors.inc"
#include "core/database.inc"

main() {}

public OnGameModeInit()
{
    SetGameModeText(SERVER_MODE_TEXT);

    if (!Database_Connect())
    {
        print("[SERVER] Shutting down: no database connection.");
        SendRconCommand("exit");
        return 1;
    }

    DisableInteriorEnterExits();
    EnableStuntBonusForAll(false);
    ShowPlayerMarkers(PLAYER_MARKERS_MODE_OFF);
    ManualVehicleEngineAndLights();

    printf("[SERVER] %s gamemode loaded.", SERVER_NAME);
    return 1;
}

public OnGameModeExit()
{
    Database_Disconnect();
    return 1;
}

public OnPlayerConnect(playerid)
{
    SendClientMessage(playerid, COLOR_SERVER, "Dobrodosli na " SERVER_NAME "!");
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    return 1;
}

public OnPlayerRequestClass(playerid, classid)
{
    // Temporary until the account system takes over spawning.
    SetSpawnInfo(playerid, NO_TEAM, DEFAULT_SKIN, DEFAULT_SPAWN_X, DEFAULT_SPAWN_Y, DEFAULT_SPAWN_Z, DEFAULT_SPAWN_A);
    SpawnPlayer(playerid);
    return 1;
}

public OnPlayerSpawn(playerid)
{
    return 1;
}

public OnPlayerCommandPerformed(playerid, cmd[], params[], result, flags)
{
    if (result == -1)
    {
        SendClientMessage(playerid, COLOR_GREY, "Nepoznata komanda.");
        return 0;
    }
    return 1;
}
