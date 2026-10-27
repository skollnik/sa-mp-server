#include <open.mp>

#undef MAX_PLAYERS
#define MAX_PLAYERS (50)

// Dialog bodies are built in local arrays; the default stack (4096 cells) gets tight as systems grow.
#pragma dynamic 16384

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
#include "modules/staff/staff_data.inc"
#include "modules/anticheat/ac_data.inc"
#include "modules/vip/vip_data.inc"
#include "modules/channel/channel_data.inc"

#include "modules/account/account_impl.inc"
#include "modules/account/account_cmds.inc"

#include "modules/chat/chat_impl.inc"
#include "modules/chat/chat_cmds.inc"

#include "modules/staff/staff_impl.inc"
#include "modules/staff/staff_cmds_player.inc"
#include "modules/staff/staff_cmds_helper.inc"
#include "modules/staff/staff_cmds_admin.inc"
#include "modules/staff/staff_punish.inc"

#include "modules/anticheat/ac_impl.inc"
#include "modules/anticheat/ac_cmds.inc"

#include "modules/vip/vip_impl.inc"
#include "modules/vip/vip_cmds.inc"

#include "modules/channel/channel_impl.inc"
#include "modules/channel/channel_cmds.inc"

#include "modules/help/help_cmds.inc"

#include "maps/maps.inc"

main() {}

public OnGameModeInit()
{
    SendRconCommand("name " SERVER_HOSTNAME);
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
    Staff_Init();
    Punish_Init();
    AC_Init();
    Vip_Init();
    Maps_Init();

    printf("[SERVER] %s gamemode loaded.", SERVER_NAME);
    return 1;
}

public OnGameModeExit()
{
    Vip_Exit();
    AC_Exit();
    Punish_Exit();
    Account_Exit();
    Database_Disconnect();
    return 1;
}

public OnPlayerConnect(playerid)
{
    Utils_OnPlayerConnect(playerid);
    AC_OnPlayerConnect(playerid);
    Vip_OnPlayerConnect(playerid);
    Staff_OnPlayerConnect(playerid);
    Maps_OnPlayerConnect(playerid);
    Account_OnPlayerConnect(playerid);
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    Staff_OnPlayerDisconnect(playerid);
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
    AC_OnPlayerSpawn(playerid);
    Account_OnPlayerSpawn(playerid);
    Staff_OnPlayerSpawn(playerid);
    Punish_OnPlayerSpawn(playerid);
    return 1;
}

public OnPlayerTakeDamage(playerid, issuerid, Float:amount, WEAPON:weaponid, bodypart)
{
    Vip_OnPlayerTakeDamage(playerid);
    return 1;
}

public OnPlayerDeath(playerid, killerid, WEAPON:reason)
{
    AC_OnPlayerDeath(playerid);
    return 1;
}

public OnPlayerStateChange(playerid, PLAYER_STATE:newstate, PLAYER_STATE:oldstate)
{
    Staff_OnPlayerStateChange(playerid, newstate);
    return 1;
}

public OnPlayerInteriorChange(playerid, newinteriorid, oldinteriorid)
{
    Staff_OnPlayerInteriorChange(playerid, newinteriorid);
    return 1;
}

public OnPlayerText(playerid, text[])
{
    if (!Account_IsLoggedIn(playerid))
    {
        return 0;
    }
    if (Punish_IsMuted(playerid))
    {
        Msg_Error(playerid, "Imate mute jos %d min.", Punish_MinutesLeft(g_Player[playerid][pMuteUntil]));
        return 0;
    }
    if (!AC_CheckChat(playerid, text))
    {
        return 0;
    }

    Chat_OnPlayerText(playerid, text);
    return 0;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    if (Account_OnDialogResponse(playerid, dialogid, response, listitem, inputtext))
    {
        return 1;
    }
    if (Staff_OnDialogResponse(playerid, dialogid, response, listitem))
    {
        return 1;
    }
    if (Channel_OnDialogResponse(playerid, dialogid, response, listitem))
    {
        return 1;
    }
    if (Vip_OnDialogResponse(playerid, dialogid, response, listitem))
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
    if (!AC_CheckCommand(playerid))
    {
        return 0;
    }
    if (g_Player[playerid][pJailSeconds] > 0 && !(flags & (CMD_FLAG_CHAT | CMD_FLAG_JAIL_OK)))
    {
        Msg_Error(playerid, "Ova komanda se ne moze koristiti u admin zatvoru.");
        return 0;
    }
    if (flags & CMD_FLAG_CHAT)
    {
        if (Punish_IsMuted(playerid))
        {
            Msg_Error(playerid, "Imate mute jos %d min.", Punish_MinutesLeft(g_Player[playerid][pMuteUntil]));
            return 0;
        }
        if (!isnull(params) && !AC_CheckChat(playerid, params))
        {
            return 0;
        }
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
