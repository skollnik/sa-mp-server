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
#include "core/messages.inc"
#include "core/dialogs.inc"
#include "core/utils.inc"
#include "core/database.inc"

#include "modules/account/account_data.inc"
#include "modules/account/account_impl.inc"
#include "modules/account/account_cmds.inc"

#include "modules/chat/chat_impl.inc"
#include "modules/chat/chat_cmds.inc"

#include "modules/help/help_cmds.inc"

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

    Account_Init();

    printf("[SERVER] %s gamemode loaded.", SERVER_NAME);
    return 1;
}

public OnGameModeExit()
{
    Account_Exit();
    Database_Disconnect();
    return 1;
}

public OnPlayerConnect(playerid)
{
    Account_OnPlayerConnect(playerid);
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    Account_OnPlayerDisconnect(playerid);
    return 1;
}

public OnPlayerRequestClass(playerid, classid)
{
    Account_OnPlayerRequestClass(playerid);
    return 1;
}

public OnPlayerRequestSpawn(playerid)
{
    return Account_IsLoggedIn(playerid) ? 1 : 0;
}

public OnPlayerSpawn(playerid)
{
    Account_OnPlayerSpawn(playerid);
    return 1;
}

public OnPlayerText(playerid, text[])
{
    if (Account_IsLoggedIn(playerid))
    {
        Chat_OnPlayerText(playerid, text);
    }
    return 0;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    if (Account_OnDialogResponse(playerid, dialogid, response, listitem, inputtext))
    {
        return 1;
    }
    return 0;
}

public OnPlayerCommandReceived(playerid, cmd[], params[], flags)
{
    if (!Account_IsLoggedIn(playerid))
    {
        return 0;
    }
    return 1;
}

public OnPlayerCommandPerformed(playerid, cmd[], params[], result, flags)
{
    if (result == -1)
    {
        Msg_Error(playerid, "Nepoznata komanda. Spisak komandi: /pomoc");
        return 0;
    }
    return 1;
}
