// Letters used on Serbian plates (no Q, W, X, Y), so plates read as local.
static const g_PlateLetters[] = "ABCDEFGHIJKLMNOPRSTUVZ";

stock Veh_Init()
{
    for (new v = 0; v < MAX_VEHICLES; v++)
    {
        g_VehSlotOf[v] = -1;
    }

    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `vehicles` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`owner_id` INT UNSIGNED NOT NULL," \
        "`model` SMALLINT UNSIGNED NOT NULL," \
        "`color1` TINYINT UNSIGNED NOT NULL DEFAULT 1, `color2` TINYINT UNSIGNED NOT NULL DEFAULT 1," \
        "`plate` VARCHAR(12) NOT NULL," \
        "`park_x` FLOAT NOT NULL, `park_y` FLOAT NOT NULL, `park_z` FLOAT NOT NULL, `park_a` FLOAT NOT NULL," \
        "`fuel` FLOAT NOT NULL, `health` FLOAT NOT NULL DEFAULT 1000," \
        "`panels` INT UNSIGNED NOT NULL DEFAULT 0, `doors` INT UNSIGNED NOT NULL DEFAULT 0," \
        "`lights` INT UNSIGNED NOT NULL DEFAULT 0, `tyres` INT UNSIGNED NOT NULL DEFAULT 0," \
        "`locked` TINYINT UNSIGNED NOT NULL DEFAULT 1," \
        "`key_holder` INT UNSIGNED NOT NULL DEFAULT 0," \
        "`created_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP," \
        "UNIQUE KEY `vehicle_plate` (`plate`), INDEX `vehicle_owner` (`owner_id`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");

    g_VehTimer = SetTimer("Veh_Tick", 1000, true);
}

stock Veh_Exit()
{
    KillTimer(g_VehTimer);
    for (new slot = 0; slot < MAX_PERSONAL_VEHICLES; slot++)
    {
        if (g_Veh[slot][vUsed])
        {
            Veh_SaveState(slot);
        }
    }
}

stock bool:Veh_GetParam(vehicleid, E_VEH_PARAM:param)
{
    new values[E_VEH_PARAM], objective;
    GetVehicleParamsEx(vehicleid, values[VEH_PARAM_ENGINE], values[VEH_PARAM_LIGHTS], values[VEH_PARAM_ALARM],
        values[VEH_PARAM_DOORS], values[VEH_PARAM_BONNET], values[VEH_PARAM_BOOT], objective);
    return values[param] == VEHICLE_PARAMS_ON;
}

// Changes one part and writes the rest back as it was, so e.g. starting the engine never unlocks the doors.
stock Veh_SetParam(vehicleid, E_VEH_PARAM:param, bool:on)
{
    new values[E_VEH_PARAM], objective;
    GetVehicleParamsEx(vehicleid, values[VEH_PARAM_ENGINE], values[VEH_PARAM_LIGHTS], values[VEH_PARAM_ALARM],
        values[VEH_PARAM_DOORS], values[VEH_PARAM_BONNET], values[VEH_PARAM_BOOT], objective);
    values[param] = on ? VEHICLE_PARAMS_ON : VEHICLE_PARAMS_OFF;
    SetVehicleParamsEx(vehicleid, values[VEH_PARAM_ENGINE], values[VEH_PARAM_LIGHTS], values[VEH_PARAM_ALARM],
        values[VEH_PARAM_DOORS], values[VEH_PARAM_BONNET], values[VEH_PARAM_BOOT], objective);
}

stock bool:Veh_IsEngineOn(vehicleid)
{
    return Veh_GetParam(vehicleid, VEH_PARAM_ENGINE);
}

stock bool:Veh_IsBicycle(model)
{
    return model == 481 || model == 509 || model == 510;
}

// Trailers, trains and RC toys cannot be owned; everything else with an engine, and bicycles, can.
stock bool:Veh_IsOwnableModel(model)
{
    return model >= VEHICLE_MODEL_FIRST && model <= VEHICLE_MODEL_LAST && (Fuel_TankSize(model) > 0 || Veh_IsBicycle(model));
}

stock bool:Veh_HasKey(playerid, vehicleid)
{
    if (g_IsAdminVehicle[vehicleid])
    {
        return Staff_IsAdmin(playerid);
    }
    new slot = g_VehSlotOf[vehicleid];
    if (slot == -1)
    {
        return false;
    }
    new dbid = g_Player[playerid][pDbId];
    return dbid == g_Veh[slot][vOwnerDbId] || (g_Veh[slot][vKeyDbId] != 0 && dbid == g_Veh[slot][vKeyDbId]);
}

stock bool:Veh_IsOwner(playerid, vehicleid)
{
    new slot = g_VehSlotOf[vehicleid];
    return slot != -1 && g_Veh[slot][vOwnerDbId] == g_Player[playerid][pDbId];
}

stock Veh_FindByDbId(dbid)
{
    for (new slot = 0; slot < MAX_PERSONAL_VEHICLES; slot++)
    {
        if (g_Veh[slot][vUsed] && g_Veh[slot][vDbId] == dbid)
        {
            return slot;
        }
    }
    return -1;
}

static stock Veh_FindFreeSlot()
{
    for (new slot = 0; slot < MAX_PERSONAL_VEHICLES; slot++)
    {
        if (!g_Veh[slot][vUsed])
        {
            return slot;
        }
    }
    return -1;
}

// Every owned vehicle is in the world while its owner is online, so counting slots counts them all.
stock Veh_CountOwned(owner_dbid)
{
    new count;
    for (new slot = 0; slot < MAX_PERSONAL_VEHICLES; slot++)
    {
        if (g_Veh[slot][vUsed] && g_Veh[slot][vOwnerDbId] == owner_dbid)
        {
            count++;
        }
    }
    return count;
}

stock Veh_Name(slot)
{
    new name[48];
    format(name, sizeof(name), "%s (%s)", Veh_ModelName(g_Veh[slot][vModel]), g_Veh[slot][vPlate]);
    return name;
}

static stock Veh_ApplyLock(slot)
{
    Veh_SetParam(g_Veh[slot][vVehicleId], VEH_PARAM_DOORS, g_Veh[slot][vLocked]);
}

static stock bool:Veh_Spawn(slot)
{
    new vehicleid = CreateVehicle(g_Veh[slot][vModel], g_Veh[slot][vParkX], g_Veh[slot][vParkY], g_Veh[slot][vParkZ], g_Veh[slot][vParkA],
        g_Veh[slot][vColor1], g_Veh[slot][vColor2], -1);
    if (vehicleid == INVALID_VEHICLE_ID)
    {
        printf("[VEHICLES] Vehicle %d could not be created (vehicle limit reached?).", g_Veh[slot][vDbId]);
        g_Veh[slot][vUsed] = false;
        return false;
    }

    g_Veh[slot][vVehicleId] = vehicleid;
    g_VehSlotOf[vehicleid] = slot;
    SetVehicleNumberPlate(vehicleid, g_Veh[slot][vPlate]);
    SetVehicleHealth(vehicleid, g_Veh[slot][vHealth]);
    UpdateVehicleDamageStatus(vehicleid, g_Veh[slot][vPanels], g_Veh[slot][vDoors], g_Veh[slot][vLights], g_Veh[slot][vTyres]);
    Fuel_Set(vehicleid, g_Veh[slot][vFuel]);
    SetVehicleParamsEx(vehicleid, VEHICLE_PARAMS_OFF, VEHICLE_PARAMS_OFF, VEHICLE_PARAMS_OFF,
        g_Veh[slot][vLocked] ? VEHICLE_PARAMS_ON : VEHICLE_PARAMS_OFF, VEHICLE_PARAMS_OFF, VEHICLE_PARAMS_OFF, VEHICLE_PARAMS_OFF);
    return true;
}

static stock Veh_ReadState(slot)
{
    new vehicleid = g_Veh[slot][vVehicleId];
    if (vehicleid == INVALID_VEHICLE_ID)
    {
        return;
    }
    new Float:health;
    GetVehicleHealth(vehicleid, health);
    g_Veh[slot][vHealth] = health < VEH_MIN_SAVED_HEALTH ? VEH_MIN_SAVED_HEALTH : health;
    GetVehicleDamageStatus(vehicleid, g_Veh[slot][vPanels], g_Veh[slot][vDoors], g_Veh[slot][vLights], g_Veh[slot][vTyres]);
    g_Veh[slot][vFuel] = g_VehFuel[vehicleid];
}

stock Veh_SaveState(slot)
{
    Veh_ReadState(slot);
    new query[320];
    mysql_format(g_SQL, query, sizeof(query),
        "UPDATE `vehicles` SET `fuel` = %f, `health` = %f, `panels` = %d, `doors` = %d, `lights` = %d, `tyres` = %d, " \
        "`locked` = %d, `color1` = %d, `color2` = %d, `key_holder` = %d WHERE `id` = %d",
        g_Veh[slot][vFuel], g_Veh[slot][vHealth], _:g_Veh[slot][vPanels], _:g_Veh[slot][vDoors], _:g_Veh[slot][vLights], _:g_Veh[slot][vTyres],
        g_Veh[slot][vLocked] ? 1 : 0, g_Veh[slot][vColor1], g_Veh[slot][vColor2], g_Veh[slot][vKeyDbId], g_Veh[slot][vDbId]);
    mysql_tquery(g_SQL, query);
}

stock Veh_SaveIfPersonal(vehicleid)
{
    new slot = g_VehSlotOf[vehicleid];
    if (slot != -1)
    {
        Veh_SaveState(slot);
    }
}

stock Veh_Despawn(slot, bool:save)
{
    if (save)
    {
        Veh_SaveState(slot);
    }
    new vehicleid = g_Veh[slot][vVehicleId];
    if (vehicleid != INVALID_VEHICLE_ID)
    {
        DestroyVehicle(vehicleid);
        g_VehSlotOf[vehicleid] = -1;
    }
    static const empty[E_VEH];
    g_Veh[slot] = empty;
}

static stock Veh_LoadRow(slot, row, ownerid)
{
    new value;
    g_Veh[slot][vUsed] = true;
    g_Veh[slot][vOwnerId] = ownerid;
    g_Veh[slot][vVehicleId] = INVALID_VEHICLE_ID;
    g_Veh[slot][vDespawnAt] = 0;
    g_Veh[slot][vDiedAt] = 0;
    cache_get_value_name_int(row, "id", g_Veh[slot][vDbId]);
    cache_get_value_name_int(row, "owner_id", g_Veh[slot][vOwnerDbId]);
    cache_get_value_name_int(row, "model", g_Veh[slot][vModel]);
    cache_get_value_name_int(row, "color1", g_Veh[slot][vColor1]);
    cache_get_value_name_int(row, "color2", g_Veh[slot][vColor2]);
    cache_get_value_name(row, "plate", g_Veh[slot][vPlate], VEH_PLATE_LENGTH);
    cache_get_value_name_float(row, "park_x", g_Veh[slot][vParkX]);
    cache_get_value_name_float(row, "park_y", g_Veh[slot][vParkY]);
    cache_get_value_name_float(row, "park_z", g_Veh[slot][vParkZ]);
    cache_get_value_name_float(row, "park_a", g_Veh[slot][vParkA]);
    cache_get_value_name_float(row, "fuel", g_Veh[slot][vFuel]);
    cache_get_value_name_float(row, "health", g_Veh[slot][vHealth]);
    cache_get_value_name_int(row, "panels", value);
    g_Veh[slot][vPanels] = VEHICLE_PANEL_STATUS:value;
    cache_get_value_name_int(row, "doors", value);
    g_Veh[slot][vDoors] = VEHICLE_DOOR_STATUS:value;
    cache_get_value_name_int(row, "lights", value);
    g_Veh[slot][vLights] = VEHICLE_LIGHT_STATUS:value;
    cache_get_value_name_int(row, "tyres", value);
    g_Veh[slot][vTyres] = VEHICLE_TYRE_STATUS:value;
    cache_get_value_name_int(row, "locked", value);
    g_Veh[slot][vLocked] = value != 0;
    cache_get_value_name_int(row, "key_holder", g_Veh[slot][vKeyDbId]);
    cache_get_value_name(row, "key_name", g_Veh[slot][vKeyName], MAX_PLAYER_NAME + 1);
}

static stock Veh_QueryOwned(playerid, const where[], id)
{
    new query[256];
    mysql_format(g_SQL, query, sizeof(query),
        "SELECT v.*, IFNULL(a.`name`, '') AS `key_name` FROM `vehicles` v LEFT JOIN `accounts` a ON a.`id` = v.`key_holder` WHERE v.`%e` = %d ORDER BY v.`id`",
        where, id);
    mysql_tquery(g_SQL, query, "OnVehiclesLoaded", "dd", playerid, g_AccountRace[playerid]);
}

stock Veh_OnLogin(playerid)
{
    Veh_QueryOwned(playerid, "owner_id", g_Player[playerid][pDbId]);
}

forward OnVehiclesLoaded(playerid, race);
public OnVehiclesLoaded(playerid, race)
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }

    new rows = cache_num_rows(), dbid;
    for (new row = 0; row < rows; row++)
    {
        cache_get_value_name_int(row, "id", dbid);
        new slot = Veh_FindByDbId(dbid);
        if (slot != -1)
        {
            // Still in the world from before a short reconnect: it stays where it was left.
            g_Veh[slot][vOwnerId] = playerid;
            g_Veh[slot][vDespawnAt] = 0;
            continue;
        }
        slot = Veh_FindFreeSlot();
        if (slot == -1)
        {
            print("[VEHICLES] No free slot for a personal vehicle; raise MAX_PERSONAL_VEHICLES.");
            break;
        }
        Veh_LoadRow(slot, row, playerid);
        Veh_Spawn(slot);
    }
    return 1;
}

// The vehicles stay for a while (a passenger may still be inside, or the owner reconnects) and then leave the world.
stock Veh_OnPlayerDisconnect(playerid)
{
    for (new slot = 0; slot < MAX_PERSONAL_VEHICLES; slot++)
    {
        if (g_Veh[slot][vUsed] && g_Veh[slot][vOwnerId] == playerid)
        {
            g_Veh[slot][vOwnerId] = INVALID_PLAYER_ID;
            g_Veh[slot][vDespawnAt] = gettime() + VEH_DESPAWN_DELAY_S;
            Veh_SaveState(slot);
        }
    }
}

stock Veh_OnVehicleDeath(vehicleid)
{
    new slot = g_VehSlotOf[vehicleid];
    if (slot != -1 && g_Veh[slot][vDiedAt] == 0)
    {
        g_Veh[slot][vDiedAt] = gettime();
    }
}

// Rebuilt at the parking spot instead of using the game's own respawn, which would bring it back fully repaired.
static stock Veh_RespawnAfterDeath(slot)
{
    new vehicleid = g_Veh[slot][vVehicleId];
    g_Veh[slot][vFuel] = g_VehFuel[vehicleid];
    DestroyVehicle(vehicleid);
    g_VehSlotOf[vehicleid] = -1;

    g_Veh[slot][vDiedAt] = 0;
    g_Veh[slot][vHealth] = VEH_RESPAWN_HEALTH;
    g_Veh[slot][vPanels] = VEHICLE_PANEL_STATUS:0;
    g_Veh[slot][vDoors] = VEHICLE_DOOR_STATUS:0;
    g_Veh[slot][vLights] = VEHICLE_LIGHT_STATUS:0;
    g_Veh[slot][vTyres] = VEHICLE_TYRE_STATUS:0;
    new ownerid = g_Veh[slot][vOwnerId];
    if (!Veh_Spawn(slot))
    {
        return;
    }
    Veh_SaveState(slot);
    if (ownerid != INVALID_PLAYER_ID)
    {
        Msg_Info(ownerid, "Vase vozilo %s je unisteno i vraceno na parking, osteceno i sa dimom iz motora.", Veh_Name(slot));
    }
}

forward Veh_Tick();
public Veh_Tick()
{
    Fuel_Tick();

    new now = gettime();
    for (new slot = 0; slot < MAX_PERSONAL_VEHICLES; slot++)
    {
        if (!g_Veh[slot][vUsed])
        {
            continue;
        }
        if (g_Veh[slot][vDiedAt] != 0)
        {
            if (now - g_Veh[slot][vDiedAt] >= VEH_DEATH_RESPAWN_S)
            {
                Veh_RespawnAfterDeath(slot);
            }
        }
        else if (g_Veh[slot][vDespawnAt] != 0 && now >= g_Veh[slot][vDespawnAt] && !IsVehicleOccupied(g_Veh[slot][vVehicleId]))
        {
            Veh_Despawn(slot, true);
        }
    }
    return 1;
}

// The vehicle the player sits in, or else the nearest one within reach; owner_only leaves out vehicles held only by a key copy.
stock Veh_FindNearby(playerid, bool:owner_only)
{
    new vehicleid = GetPlayerVehicleID(playerid);
    if (vehicleid != 0)
    {
        return (owner_only ? Veh_IsOwner(playerid, vehicleid) : Veh_HasKey(playerid, vehicleid)) ? vehicleid : INVALID_VEHICLE_ID;
    }

    new best = INVALID_VEHICLE_ID, Float:best_distance = VEH_KEY_RANGE, world = GetPlayerVirtualWorld(playerid);
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    for (new slot = 0; slot < MAX_PERSONAL_VEHICLES; slot++)
    {
        new id = g_Veh[slot][vVehicleId];
        if (!g_Veh[slot][vUsed] || id == INVALID_VEHICLE_ID || GetVehicleVirtualWorld(id) != world)
        {
            continue;
        }
        if (owner_only ? !Veh_IsOwner(playerid, id) : !Veh_HasKey(playerid, id))
        {
            continue;
        }
        new Float:distance = GetVehicleDistanceFromPoint(id, x, y, z);
        if (distance < best_distance)
        {
            best_distance = distance;
            best = id;
        }
    }
    return best;
}

stock Veh_ToggleEngine(playerid)
{
    if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
    {
        Msg_Error(playerid, "Motor pali i gasi vozac.");
        return;
    }
    new vehicleid = GetPlayerVehicleID(playerid);
    if (Fuel_TankSize(GetVehicleModel(vehicleid)) == 0 && !g_IsAdminVehicle[vehicleid])
    {
        Msg_Error(playerid, "Ovo vozilo nema motor.");
        return;
    }
    if (!Veh_HasKey(playerid, vehicleid))
    {
        Msg_Error(playerid, "Nemate kljuc ovog vozila.");
        return;
    }
    if (Veh_IsEngineOn(vehicleid))
    {
        Veh_SetParam(vehicleid, VEH_PARAM_ENGINE, false);
        Chat_Action(playerid, "gasi motor.");
        return;
    }
    if (Fuel_IsUsed(vehicleid) && g_VehFuel[vehicleid] <= 0.0)
    {
        Msg_Error(playerid, "Vozilo nema goriva. Benzinska pumpa se trazi na /gps, a gorivo se toci sa /gorivo.");
        return;
    }
    Veh_SetParam(vehicleid, VEH_PARAM_ENGINE, true);
    Chat_Action(playerid, "okrece kljuc i pali motor.");
}

stock Veh_ToggleLock(playerid)
{
    new vehicleid = Veh_FindNearby(playerid, false);
    if (vehicleid == INVALID_VEHICLE_ID || g_VehSlotOf[vehicleid] == -1)
    {
        Msg_Error(playerid, "U blizini nema vaseg vozila ni vozila za koje imate kljuc.");
        return;
    }
    new slot = g_VehSlotOf[vehicleid];
    g_Veh[slot][vLocked] = !g_Veh[slot][vLocked];
    Veh_ApplyLock(slot);
    Veh_SaveState(slot);
    Chat_Action(playerid, g_Veh[slot][vLocked] ? ("zakljucava vozilo.") : ("otkljucava vozilo."));
    GameTextForPlayer(playerid, g_Veh[slot][vLocked] ? ("~r~zakljucano") : ("~g~otkljucano"), 1500, 6);
}

// Bonnet and boot: from the driver's seat or standing next to a vehicle the player has a key for.
stock Veh_TogglePart(playerid, E_VEH_PARAM:param, const open_action[], const close_action[])
{
    new vehicleid = Veh_FindNearby(playerid, false);
    if (vehicleid == INVALID_VEHICLE_ID)
    {
        Msg_Error(playerid, "U blizini nema vaseg vozila ni vozila za koje imate kljuc.");
        return;
    }
    new bool:open = !Veh_GetParam(vehicleid, param);
    Veh_SetParam(vehicleid, param, open);
    Chat_Action(playerid, open ? open_action : close_action);
}

stock Veh_ToggleLights(playerid)
{
    if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
    {
        Msg_Error(playerid, "Svetla pali vozac.");
        return;
    }
    new vehicleid = GetPlayerVehicleID(playerid), bool:on = !Veh_GetParam(vehicleid, VEH_PARAM_LIGHTS);
    Veh_SetParam(vehicleid, VEH_PARAM_LIGHTS, on);
    GameTextForPlayer(playerid, on ? ("~w~svetla ~g~upaljena") : ("~w~svetla ~r~ugasena"), 1500, 6);
}

// N starts and stops the engine as the driver; Y locks the nearest vehicle the player holds a key for.
stock bool:Veh_OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys)
{
    new PLAYER_STATE:player_state = GetPlayerState(playerid);
    if ((newkeys & KEY_NO) && !(oldkeys & KEY_NO) && player_state == PLAYER_STATE_DRIVER)
    {
        new vehicleid = GetPlayerVehicleID(playerid);
        if (Fuel_TankSize(GetVehicleModel(vehicleid)) == 0 && !g_IsAdminVehicle[vehicleid])
        {
            return false;
        }
        Veh_ToggleEngine(playerid);
        return true;
    }
    if ((newkeys & KEY_YES) && !(oldkeys & KEY_YES)
        && (player_state == PLAYER_STATE_ONFOOT || player_state == PLAYER_STATE_DRIVER || player_state == PLAYER_STATE_PASSENGER))
    {
        new vehicleid = Veh_FindNearby(playerid, false);
        if (vehicleid == INVALID_VEHICLE_ID || g_VehSlotOf[vehicleid] == -1)
        {
            return false;
        }
        Veh_ToggleLock(playerid);
        return true;
    }
    return false;
}

stock Veh_OnPlayerStateChange(playerid, PLAYER_STATE:newstate)
{
    if (newstate != PLAYER_STATE_DRIVER)
    {
        return;
    }
    new vehicleid = GetPlayerVehicleID(playerid);
    if (g_VehSlotOf[vehicleid] == -1)
    {
        return;
    }
    if (!Veh_HasKey(playerid, vehicleid))
    {
        // Nobody drives off in someone else's car without the key; hot-wiring comes with vehicle theft.
        if (Veh_IsEngineOn(vehicleid))
        {
            Veh_SetParam(vehicleid, VEH_PARAM_ENGINE, false);
        }
        Msg_Info(playerid, "Ovo vozilo nije vase i nemate kljuc, pa ne mozete upaliti motor.");
        return;
    }
    if (!Veh_IsEngineOn(vehicleid) && Fuel_IsUsed(vehicleid))
    {
        Msg_Info(playerid, "Motor: taster N (/v motor). Brava: taster Y (/v zakljucaj). Ostalo: /v");
    }
}

// Changing the colour of a personal vehicle is kept, so it survives the next login.
stock Veh_OnColoursChanged(vehicleid, color1, color2)
{
    new slot = g_VehSlotOf[vehicleid];
    if (slot == -1)
    {
        return;
    }
    g_Veh[slot][vColor1] = color1;
    g_Veh[slot][vColor2] = color2;
    Veh_SaveState(slot);
}

stock Veh_RandomPlate(plate[], size = sizeof(plate))
{
    format(plate, size, "LS %03d-%c%c", 100 + random(900),
        g_PlateLetters[random(sizeof(g_PlateLetters) - 1)], g_PlateLetters[random(sizeof(g_PlateLetters) - 1)]);
}

// A new vehicle stands next to the player; the plate is checked against the database first, since owners can be offline.
stock Veh_CreateFor(adminid, targetid, model)
{
    new Float:x, Float:y, Float:z, Float:angle;
    GetPlayerPos(targetid, x, y, z);
    GetPlayerFacingAngle(targetid, angle);
    x += 4.0 * floatcos(angle, degrees);
    y += 4.0 * floatsin(angle, degrees);

    new plate[VEH_PLATE_LENGTH];
    Veh_RandomPlate(plate);
    Veh_CheckPlate(adminid, targetid, g_AccountRace[targetid], model, plate, x, y, z, angle);
}

static stock Veh_CheckPlate(adminid, targetid, race, model, const plate[], Float:x, Float:y, Float:z, Float:angle)
{
    new query[96];
    mysql_format(g_SQL, query, sizeof(query), "SELECT `id` FROM `vehicles` WHERE `plate` = '%e'", plate);
    mysql_tquery(g_SQL, query, "OnVehiclePlateChecked", "ddddsffff", adminid, targetid, race, model, plate, x, y, z, angle);
}

forward OnVehiclePlateChecked(adminid, targetid, race, model, const plate[], Float:x, Float:y, Float:z, Float:angle);
public OnVehiclePlateChecked(adminid, targetid, race, model, const plate[], Float:x, Float:y, Float:z, Float:angle)
{
    if (race != g_AccountRace[targetid])
    {
        return 1;
    }
    if (cache_num_rows() > 0)
    {
        new other[VEH_PLATE_LENGTH];
        Veh_RandomPlate(other);
        Veh_CheckPlate(adminid, targetid, race, model, other, x, y, z, angle);
        return 1;
    }
    if (Veh_CountOwned(g_Player[targetid][pDbId]) >= VEH_MAX_PER_PLAYER)
    {
        return 1;
    }

    new query[384];
    mysql_format(g_SQL, query, sizeof(query),
        "INSERT INTO `vehicles` (`owner_id`, `model`, `color1`, `color2`, `plate`, `park_x`, `park_y`, `park_z`, `park_a`, `fuel`) " \
        "VALUES (%d, %d, %d, %d, '%e', %f, %f, %f, %f, %d)",
        g_Player[targetid][pDbId], model, random(127), random(127), plate, x, y, z, angle, Fuel_TankSize(model));
    mysql_tquery(g_SQL, query, "OnVehicleInserted", "ddds", adminid, targetid, race, plate);
    return 1;
}

forward OnVehicleInserted(adminid, targetid, race, const plate[]);
public OnVehicleInserted(adminid, targetid, race, const plate[])
{
    if (race != g_AccountRace[targetid])
    {
        return 1;
    }
    Veh_QueryOwned(targetid, "id", cache_insert_id());

    new details[48];
    format(details, sizeof(details), "tablica %s", plate);
    if (Account_IsLoggedIn(adminid))
    {
        Msg_Admin(adminid, "Igrac %s je dobio vozilo sa tablicom %s.", Account_Name(targetid), plate);
        Staff_Log(adminid, "giveveh", targetid, details);
    }
    Msg_Success(targetid, "Dobili ste vozilo (tablica %s), stoji pored vas. Parkirajte ga gde zelite sa /v parkiraj.", plate);
    return 1;
}

stock Veh_ShowList(playerid)
{
    new body[640], line[144], count, dbid = g_Player[playerid][pDbId];
    strcopy(body, "Vozilo\tGorivo\tGde je\tKopija kljuca\n");
    for (new slot = 0; slot < MAX_PERSONAL_VEHICLES && count < sizeof(g_VehListed[]); slot++)
    {
        if (!g_Veh[slot][vUsed] || g_Veh[slot][vOwnerDbId] != dbid)
        {
            continue;
        }
        new vehicleid = g_Veh[slot][vVehicleId], fuel[8], Float:x, Float:y, Float:z;
        if (Fuel_IsUsed(vehicleid))
        {
            format(fuel, sizeof(fuel), "%d%%", Fuel_Percent(vehicleid));
        }
        else
        {
            strcopy(fuel, "-");
        }
        GetVehiclePos(vehicleid, x, y, z);
        new zone = Hud_FindZone(x, y), place[32], key[MAX_PLAYER_NAME + 1];
        if (zone == -1)
        {
            strcopy(place, "van grada");
        }
        else
        {
            strcopy(place, g_Zones[zone][zName]);
        }
        if (g_Veh[slot][vKeyDbId] != 0)
        {
            strcopy(key, g_Veh[slot][vKeyName]);
        }
        else
        {
            strcopy(key, "nema");
        }
        format(line, sizeof(line), "%s\t%s\t%s\t%s\n", Veh_Name(slot), fuel, place, key);
        strcat(body, line);
        g_VehListed[playerid][count++] = slot;
    }
    if (count == 0)
    {
        Msg_Info(playerid, "Nemate vozilo. Vozila se kupuju na auto pijaci (kad je admin otvori) ili od drugog igraca.");
        return;
    }
    ShowPlayerDialog(playerid, DIALOG_VEH_LIST, DIALOG_STYLE_TABLIST_HEADERS, "Vasa vozila (klik za opcije)", body, "Izaberi", "Zatvori");
}

static stock bool:Veh_IsOwnSlot(playerid, slot)
{
    return slot >= 0 && slot < MAX_PERSONAL_VEHICLES && g_Veh[slot][vUsed] && g_Veh[slot][vOwnerDbId] == g_Player[playerid][pDbId];
}

static stock Veh_ShowActions(playerid, slot)
{
    new body[128], line[64];
    strcopy(body, "Oznaci na GPS-u\n");
    if (g_Veh[slot][vKeyDbId] != 0)
    {
        format(line, sizeof(line), "Oduzmi kopiju kljuca (%s)\n", g_Veh[slot][vKeyName]);
        strcat(body, line);
    }
    g_VehSelected[playerid] = slot;
    ShowPlayerDialog(playerid, DIALOG_VEH_ACTION, DIALOG_STYLE_LIST, Veh_Name(slot), body, "Izaberi", "Nazad");
}

static stock Veh_TakeKey(playerid, slot)
{
    new holder = Account_FindOnlineByDbId(g_Veh[slot][vKeyDbId]);
    if (holder != INVALID_PLAYER_ID)
    {
        Msg_Info(holder, "%s vam je oduzeo kopiju kljuca od vozila %s.", Account_Name(playerid), Veh_Name(slot));
    }
    Msg_Success(playerid, "Oduzeli ste kopiju kljuca od vozila %s (%s).", Veh_Name(slot), g_Veh[slot][vKeyName]);
    g_Veh[slot][vKeyDbId] = 0;
    g_Veh[slot][vKeyName][0] = '\0';
    Veh_SaveState(slot);
}

stock bool:Veh_OnDialogResponse(playerid, dialogid, response, listitem)
{
    switch (dialogid)
    {
        case DIALOG_VEH_LIST:
        {
            if (!response || listitem < 0 || listitem >= sizeof(g_VehListed[]))
            {
                return true;
            }
            new slot = g_VehListed[playerid][listitem];
            if (Veh_IsOwnSlot(playerid, slot))
            {
                Veh_ShowActions(playerid, slot);
            }
        }
        case DIALOG_VEH_ACTION:
        {
            new slot = g_VehSelected[playerid];
            if (!Veh_IsOwnSlot(playerid, slot))
            {
                return true;
            }
            if (!response)
            {
                Veh_ShowList(playerid);
                return true;
            }
            if (listitem == 0)
            {
                new Float:x, Float:y, Float:z;
                GetVehiclePos(g_Veh[slot][vVehicleId], x, y, z);
                Gps_StartAt(playerid, -1, Veh_Name(slot), x, y, z);
            }
            else if (listitem == 1 && g_Veh[slot][vKeyDbId] != 0)
            {
                Veh_TakeKey(playerid, slot);
            }
        }
        default:
        {
            return false;
        }
    }
    return true;
}

stock Veh_GiveKey(playerid, const params[])
{
    new targetid;
    if (sscanf(params, "u", targetid))
    {
        Msg_Usage(playerid, "/v kljuc [id ili ime] (dok ste u svom vozilu ili pored njega, a igrac je pored vas)");
        return;
    }
    if (targetid == INVALID_PLAYER_ID || !Account_IsLoggedIn(targetid))
    {
        Msg_Error(playerid, "Taj igrac nije pronadjen.");
        return;
    }
    if (targetid == playerid)
    {
        Msg_Error(playerid, "Kljuc od svog vozila vec imate.");
        return;
    }
    new vehicleid = Veh_FindNearby(playerid, true);
    if (vehicleid == INVALID_VEHICLE_ID)
    {
        Msg_Error(playerid, "Morate biti u svom vozilu ili pored njega.");
        return;
    }
    if (!Chat_CheckBeside(playerid, targetid, 3.0))
    {
        return;
    }
    new slot = g_VehSlotOf[vehicleid];
    if (g_Veh[slot][vKeyDbId] != 0)
    {
        Msg_Error(playerid, "Kopiju kljuca vec ima %s. Postoji samo jedna kopija; oduzima se u /v.", g_Veh[slot][vKeyName]);
        return;
    }

    g_Veh[slot][vKeyDbId] = g_Player[targetid][pDbId];
    strcopy(g_Veh[slot][vKeyName], g_Player[targetid][pName], MAX_PLAYER_NAME + 1);
    Veh_SaveState(slot);

    new action[96];
    format(action, sizeof(action), "daje kopiju kljuca od vozila igracu %s.", Account_Name(targetid));
    Chat_Action(playerid, action);
    Msg_Success(playerid, "Dali ste kopiju kljuca od vozila %s igracu %s. Oduzima se u /v.", Veh_Name(slot), Account_Name(targetid));
    Msg_Info(targetid, "Dobili ste kopiju kljuca od vozila %s. Mozete ga otkljucati (Y) i voziti (N).", Veh_Name(slot));
}

stock Veh_Park(playerid)
{
    new vehicleid = GetPlayerVehicleID(playerid);
    if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER || !Veh_IsOwner(playerid, vehicleid))
    {
        Msg_Error(playerid, "Morate sedeti za volanom svog vozila.");
        return;
    }
    if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
    {
        Msg_Error(playerid, "Vozilo se parkira samo napolju.");
        return;
    }
    if (Veh_SpeedKmh(vehicleid) > 5.0)
    {
        Msg_Error(playerid, "Prvo zaustavite vozilo.");
        return;
    }

    new slot = g_VehSlotOf[vehicleid];
    GetVehiclePos(vehicleid, g_Veh[slot][vParkX], g_Veh[slot][vParkY], g_Veh[slot][vParkZ]);
    GetVehicleZAngle(vehicleid, g_Veh[slot][vParkA]);

    new query[192];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `vehicles` SET `park_x` = %f, `park_y` = %f, `park_z` = %f, `park_a` = %f WHERE `id` = %d",
        g_Veh[slot][vParkX], g_Veh[slot][vParkY], g_Veh[slot][vParkZ], g_Veh[slot][vParkA], g_Veh[slot][vDbId]);
    mysql_tquery(g_SQL, query);
    Msg_Success(playerid, "Vozilo %s je parkirano ovde. Ovde vas ceka posle svake prijave.", Veh_Name(slot));
}

// Only empty vehicles move; health, damage and fuel stay as they are, so this is never a free repair.
stock Veh_ReturnAllToParking()
{
    new count;
    for (new slot = 0; slot < MAX_PERSONAL_VEHICLES; slot++)
    {
        new vehicleid = g_Veh[slot][vVehicleId];
        if (!g_Veh[slot][vUsed] || vehicleid == INVALID_VEHICLE_ID || g_Veh[slot][vDiedAt] != 0 || IsVehicleOccupied(vehicleid))
        {
            continue;
        }
        SetVehicleVelocity(vehicleid, 0.0, 0.0, 0.0);
        SetVehiclePos(vehicleid, g_Veh[slot][vParkX], g_Veh[slot][vParkY], g_Veh[slot][vParkZ]);
        SetVehicleZAngle(vehicleid, g_Veh[slot][vParkA]);
        LinkVehicleToInterior(vehicleid, 0);
        SetVehicleVirtualWorld(vehicleid, 0);
        count++;
    }
    return count;
}
