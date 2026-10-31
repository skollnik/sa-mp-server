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
#include "core/economy.inc"
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
#include "modules/settings/settings_data.inc"
#include "modules/preview/preview_data.inc"
#include "modules/door/door_data.inc"
#include "modules/point/point_data.inc"
#include "modules/money/money_data.inc"
#include "modules/inventory/inventory_data.inc"
#include "modules/inventory/accessories_data.inc"
#include "modules/vehicle/vehicle_data.inc"
#include "modules/vehicle/vehicle_names.inc"
#include "modules/health/hunger_data.inc"
#include "modules/world/checkpoint.inc"
#include "modules/world/zones.inc"
#include "modules/world/gps.inc"

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

#include "modules/preview/preview_impl.inc"

#include "modules/door/door_impl.inc"
#include "modules/door/door_cmds.inc"

#include "modules/point/point_impl.inc"

#include "modules/money/money_impl.inc"
#include "modules/money/bank_account.inc"
#include "modules/money/begging.inc"
#include "modules/gamble/bets.inc"
#include "modules/gamble/lottery.inc"
#include "modules/money/money_cmds.inc"
#include "modules/money/payday.inc"

#include "modules/world/time.inc"
#include "modules/world/hud.inc"
#include "modules/health/hunger.inc"

#include "modules/cityhall/cityhall.inc"

#include "modules/inventory/inventory_impl.inc"
#include "modules/inventory/accessories.inc"
#include "modules/phone/phone.inc"
#include "modules/settings/settings.inc"

#include "modules/vehicle/vehicle_impl.inc"
#include "modules/vehicle/fuel.inc"
#include "modules/vehicle/registration.inc"
#include "modules/vehicle/tuning.inc"
#include "modules/vehicle/plates.inc"
#include "modules/vehicle/fines.inc"
#include "modules/vehicle/cameras.inc"
#include "modules/vehicle/vehicle_cmds.inc"
#include "modules/vehicle/speedometer.inc"

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
    Door_Init();
    Point_Init();
    Money_Init();
    Beg_Init();
    Lottery_Init();
    Payday_Init();
    Gps_Init();
    Time_Init();
    Hud_Init();
    Hunger_Init();
    Inv_Init();
    Phone_Init();
    Veh_Init();
    Fine_Init();
    Cam_Init();
    Speedo_Init();
    Maps_Init();

    printf("[SERVER] %s gamemode loaded.", SERVER_NAME);
    return 1;
}

public OnGameModeExit()
{
    Speedo_Exit();
    Veh_Exit();
    Phone_Exit();
    Inv_Exit();
    Hunger_Exit();
    Hud_Exit();
    Time_Exit();
    Lottery_Exit();
    Beg_Exit();
    Payday_Exit();
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
    Preview_OnPlayerConnect(playerid);
    Door_OnPlayerConnect(playerid);
    Checkpoint_OnPlayerConnect(playerid);
    Gps_OnPlayerConnect(playerid);
    Hud_OnPlayerConnect(playerid);
    Hunger_OnPlayerConnect(playerid);
    Inv_OnPlayerConnect(playerid);
    Acc_OnPlayerConnect(playerid);
    Phone_OnPlayerConnect(playerid);
    Speedo_OnPlayerConnect(playerid);
    Veh_OnPlayerConnect(playerid);
    Fine_OnPlayerConnect(playerid);
    Bet_OnPlayerConnect(playerid);
    Beg_OnPlayerConnect(playerid);
    Lottery_OnPlayerConnect(playerid);
    Staff_OnPlayerConnect(playerid);
    Maps_OnPlayerConnect(playerid);
    Account_OnPlayerConnect(playerid);
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    Gps_OnPlayerDisconnect(playerid);
    Phone_OnPlayerDisconnect(playerid);
    Beg_OnPlayerDisconnect(playerid);
    Staff_OnPlayerDisconnect(playerid);
    Veh_OnPlayerDisconnect(playerid);
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
    Utils_PreloadAnimations(playerid);
    Acc_OnPlayerSpawn(playerid);
    Time_Apply(playerid);
    Hud_Refresh(playerid);
    return 1;
}

public OnPlayerUpdate(playerid)
{
    Payday_OnPlayerUpdate(playerid);
    return 1;
}

public OnPlayerEnterCheckpoint(playerid)
{
    Checkpoint_OnPlayerEnter(playerid);
    return 1;
}

public OnPlayerTakeDamage(playerid, issuerid, Float:amount, WEAPON:weaponid, bodypart)
{
    Account_OnPlayerTakeDamage(playerid);
    return 1;
}

public OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA:areaid)
{
    Cam_OnPlayerEnterArea(playerid, areaid);
    return 1;
}

public OnVehicleDeath(vehicleid, killerid)
{
    Veh_OnVehicleDeath(vehicleid);
    return 1;
}

public OnPlayerDeath(playerid, killerid, WEAPON:reason)
{
    AC_OnPlayerDeath(playerid);
    Phone_OnPlayerDeath(playerid);
    Hunger_OnPlayerDeath(playerid);
    Beg_OnPlayerDeath(playerid);
    return 1;
}

public OnPlayerStateChange(playerid, PLAYER_STATE:newstate, PLAYER_STATE:oldstate)
{
    Staff_OnPlayerStateChange(playerid, newstate);
    Beg_OnPlayerStateChange(playerid, newstate);
    Veh_OnPlayerStateChange(playerid, newstate);
    Speedo_OnPlayerStateChange(playerid, newstate);
    return 1;
}

public OnPlayerInteriorChange(playerid, newinteriorid, oldinteriorid)
{
    Staff_OnPlayerInteriorChange(playerid, newinteriorid);
    return 1;
}

public OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys)
{
    if (Account_IsLoggedIn(playerid))
    {
        if (!Veh_OnPlayerKeyStateChange(playerid, newkeys, oldkeys) && !Inv_OnPlayerKeyStateChange(playerid, newkeys, oldkeys))
        {
            Door_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
        }
    }
    return 1;
}

public OnPlayerClickTextDraw(playerid, Text:clickedid)
{
    Preview_OnClickTextDraw(playerid, clickedid);
    return 0;
}

public OnPlayerClickPlayerTextDraw(playerid, PlayerText:playertextid)
{
    return Preview_OnClickPlayerTextDraw(playerid, playertextid) ? 1 : 0;
}

public OnPlayerText(playerid, text[])
{
    if (!Account_IsLoggedIn(playerid))
    {
        return 0;
    }
    if (!Punish_CanSpeak(playerid, text))
    {
        return 0;
    }

    Payday_MarkActive(playerid);
    if (!Phone_OnPlayerText(playerid, text))
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
    if (Staff_OnDialogResponse(playerid, dialogid, response, listitem))
    {
        return 1;
    }
    if (Channel_OnDialogResponse(playerid, dialogid, response, listitem))
    {
        return 1;
    }
    if (Money_OnDialogResponse(playerid, dialogid, response, listitem, inputtext))
    {
        return 1;
    }
    if (BankPin_OnDialogResponse(playerid, dialogid, response, listitem, inputtext))
    {
        return 1;
    }
    if (Bet_OnDialogResponse(playerid, dialogid, response))
    {
        return 1;
    }
    if (Gps_OnDialogResponse(playerid, dialogid, response, listitem))
    {
        return 1;
    }
    if (CityHall_OnDialogResponse(playerid, dialogid, response, listitem))
    {
        return 1;
    }
    if (Inv_OnDialogResponse(playerid, dialogid, response, listitem, inputtext))
    {
        return 1;
    }
    if (Shop_OnDialogResponse(playerid, dialogid, response, listitem))
    {
        return 1;
    }
    if (Acc_OnDialogResponse(playerid, dialogid, response, listitem))
    {
        return 1;
    }
    if (Phone_OnDialogResponse(playerid, dialogid, response, listitem, inputtext))
    {
        return 1;
    }
    if (Settings_OnDialogResponse(playerid, dialogid, response, listitem))
    {
        return 1;
    }
    if (Veh_OnDialogResponse(playerid, dialogid, response, listitem))
    {
        return 1;
    }
    if (Tune_OnDialogResponse(playerid, dialogid, response, listitem, inputtext))
    {
        return 1;
    }
    if (Point_OnDialogResponse(playerid, dialogid, response, listitem))
    {
        return 1;
    }
    if (Plate_OnDialogResponse(playerid, dialogid, response, listitem, inputtext))
    {
        return 1;
    }
    if (Fine_OnDialogResponse(playerid, dialogid, response))
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
    if ((flags & CMD_FLAG_CHAT) && !Punish_CanSpeak(playerid, params))
    {
        return 0;
    }
    return 1;
}

public OnPlayerCommandPerformed(playerid, cmd[], params[], result, flags)
{
    Payday_MarkActive(playerid);
    if (result == -1)
    {
        Msg_Error(playerid, "Nepoznata komanda. Spisak komandi: /pomoc");
        return 0;
    }
    return 1;
}
