// Letters used on Serbian plates (no Q, W, X, Y), so plates read as local.
static const g_PlateLetters[] = "ABCDEFGHIJKLMNOPRSTUVZ";

#define VEH_SELECT "SELECT v.*, IFNULL(v.`plate`, '') AS `plate_text`, IFNULL(v.`pending_plate`, '') AS `pending_text`, IFNULL(o.`name`, '') AS `owner_name`, IFNULL(k.`name`, '') AS `key_name` " \
    "FROM `vehicles` v LEFT JOIN `accounts` o ON o.`id` = v.`owner_id` LEFT JOIN `accounts` k ON k.`id` = v.`key_holder` "

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
        "`plate` VARCHAR(12) NULL," \
        "`park_x` FLOAT NOT NULL, `park_y` FLOAT NOT NULL, `park_z` FLOAT NOT NULL, `park_a` FLOAT NOT NULL," \
        "`fuel` FLOAT NOT NULL, `health` FLOAT NOT NULL DEFAULT 1000," \
        "`panels` INT UNSIGNED NOT NULL DEFAULT 0, `doors` INT UNSIGNED NOT NULL DEFAULT 0," \
        "`lights` INT UNSIGNED NOT NULL DEFAULT 0, `tyres` INT UNSIGNED NOT NULL DEFAULT 0," \
        "`locked` TINYINT UNSIGNED NOT NULL DEFAULT 1," \
        "`key_holder` INT UNSIGNED NOT NULL DEFAULT 0," \
        "`created_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP," \
        "UNIQUE KEY `vehicle_plate` (`plate`), INDEX `vehicle_owner` (`owner_id`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
    // An unregistered vehicle has no plate; NULL keeps the unique key from clashing between them.
    mysql_tquery(g_SQL, "ALTER TABLE `vehicles` MODIFY COLUMN `plate` VARCHAR(12) NULL");
    mysql_tquery(g_SQL, "ALTER TABLE `vehicles` ADD COLUMN IF NOT EXISTS `registered_until` INT NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `vehicles` ADD COLUMN IF NOT EXISTS `mods` VARCHAR(128) NOT NULL DEFAULT ''");
    mysql_tquery(g_SQL, "ALTER TABLE `vehicles` ADD COLUMN IF NOT EXISTS `paintjob` TINYINT UNSIGNED NOT NULL DEFAULT 3");
    mysql_tquery(g_SQL, "ALTER TABLE `vehicles` ADD COLUMN IF NOT EXISTS `pending_plate` VARCHAR(12) NULL");
    mysql_tquery(g_SQL, "ALTER TABLE `vehicles` ADD COLUMN IF NOT EXISTS `parked` TINYINT UNSIGNED NOT NULL DEFAULT 1");
    mysql_tquery(g_SQL, "ALTER TABLE `vehicles` ADD UNIQUE INDEX IF NOT EXISTS `vehicle_pending_plate` (`pending_plate`)");
    // Vehicles that got a plate before registration existed count as freshly registered.
    new query[512];
    mysql_format(g_SQL, query, sizeof(query),
        "UPDATE `vehicles` SET `registered_until` = UNIX_TIMESTAMP() + %d WHERE `plate` IS NOT NULL AND `registered_until` = 0",
        VEH_REGISTRATION_DAYS * 86400);
    mysql_tquery(g_SQL, query);

    mysql_format(g_SQL, query, sizeof(query), VEH_SELECT "WHERE o.`last_login` >= NOW() - INTERVAL %d DAY ORDER BY v.`id`", VEH_INACTIVE_DAYS);
    mysql_tquery(g_SQL, query, "OnVehiclesLoaded", "ddd", INVALID_PLAYER_ID, 0, false);

    g_VehTimer = SetTimer("Veh_Tick", 1000, true);
}

stock Veh_Exit()
{
    KillTimer(g_VehTimer);
    if (g_VehResetTimer != 0)
    {
        KillTimer(g_VehResetTimer);
    }
    for (new slot = 0; slot < MAX_PERSONAL_VEHICLES; slot++)
    {
        if (g_Veh[slot][vUsed])
        {
            Veh_SaveState(slot);
        }
    }
}

stock Veh_OnPlayerConnect(playerid)
{
    g_VehStartTimer[playerid] = 0;
    g_VehInspectTimer[playerid] = 0;
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

// An online owner's vehicles are all loaded (an inactive owner's come back on login), so counting slots counts them all.
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
    if (g_Veh[slot][vPlate][0])
    {
        format(name, sizeof(name), "%s (%s)", Veh_ModelName(g_Veh[slot][vModel]), g_Veh[slot][vPlate]);
    }
    else
    {
        format(name, sizeof(name), "%s (bez tablica)", Veh_ModelName(g_Veh[slot][vModel]));
    }
    return name;
}

static stock Veh_ApplyLock(slot)
{
    Veh_SetParam(g_Veh[slot][vVehicleId], VEH_PARAM_DOORS, g_Veh[slot][vLocked]);
}

static stock Veh_ApplyTuning(slot)
{
    new vehicleid = g_Veh[slot][vVehicleId];
    for (new i = 0; i < VEH_TUNE_SLOTS; i++)
    {
        if (g_Veh[slot][vMods][i] != 0)
        {
            AddVehicleComponent(vehicleid, g_Veh[slot][vMods][i]);
        }
    }
    if (g_Veh[slot][vPaintjob] != VEH_PAINTJOB_NONE)
    {
        ChangeVehiclePaintjob(vehicleid, g_Veh[slot][vPaintjob]);
    }
}

static stock bool:Veh_SpawnAt(slot, Float:x, Float:y, Float:z, Float:angle, interior = 0, world = 0)
{
    new vehicleid = CreateVehicle(g_Veh[slot][vModel], x, y, z, angle, g_Veh[slot][vColor1], g_Veh[slot][vColor2], -1);
    if (vehicleid == INVALID_VEHICLE_ID)
    {
        printf("[VEHICLES] Vehicle %d could not be created (vehicle limit reached?).", g_Veh[slot][vDbId]);
        g_Veh[slot][vUsed] = false;
        return false;
    }

    g_Veh[slot][vVehicleId] = vehicleid;
    g_VehSlotOf[vehicleid] = slot;
    LinkVehicleToInterior(vehicleid, interior);
    SetVehicleVirtualWorld(vehicleid, world);
    // A blank plate instead of the game's default text, which would look like a real registration.
    if (g_Veh[slot][vPlate][0])
    {
        SetVehicleNumberPlate(vehicleid, g_Veh[slot][vPlate]);
    }
    else
    {
        SetVehicleNumberPlate(vehicleid, " ");
    }
    SetVehicleHealth(vehicleid, g_Veh[slot][vHealth]);
    UpdateVehicleDamageStatus(vehicleid, g_Veh[slot][vPanels], g_Veh[slot][vDoors], g_Veh[slot][vLights], g_Veh[slot][vTyres]);
    Veh_ApplyTuning(slot);
    Fuel_Set(vehicleid, g_Veh[slot][vFuel]);
    SetVehicleParamsEx(vehicleid, VEHICLE_PARAMS_OFF, VEHICLE_PARAMS_OFF, VEHICLE_PARAMS_OFF,
        g_Veh[slot][vLocked] ? VEHICLE_PARAMS_ON : VEHICLE_PARAMS_OFF, VEHICLE_PARAMS_OFF, VEHICLE_PARAMS_OFF, VEHICLE_PARAMS_OFF);
    return true;
}

static stock bool:Veh_Spawn(slot)
{
    return Veh_SpawnAt(slot, g_Veh[slot][vParkX], g_Veh[slot][vParkY], g_Veh[slot][vParkZ], g_Veh[slot][vParkA]);
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

// Only tuning done through /atune is kept; parts from GTA's own mod shops are not saved.
static stock Veh_FormatMods(slot, dest[], size = sizeof(dest))
{
    dest[0] = '\0';
    for (new i = 0; i < VEH_TUNE_SLOTS; i++)
    {
        format(dest, size, "%s%d,", dest, g_Veh[slot][vMods][i]);
    }
}

stock Veh_SaveState(slot)
{
    Veh_ReadState(slot);
    new mods[128], query[640];
    Veh_FormatMods(slot, mods);
    mysql_format(g_SQL, query, sizeof(query),
        "UPDATE `vehicles` SET `fuel` = %f, `health` = %f, `panels` = %d, `doors` = %d, `lights` = %d, `tyres` = %d, " \
        "`locked` = %d, `color1` = %d, `color2` = %d, `key_holder` = %d, `mods` = '%e', `paintjob` = %d, `registered_until` = %d WHERE `id` = %d",
        g_Veh[slot][vFuel], g_Veh[slot][vHealth], _:g_Veh[slot][vPanels], _:g_Veh[slot][vDoors], _:g_Veh[slot][vLights], _:g_Veh[slot][vTyres],
        g_Veh[slot][vLocked] ? 1 : 0, g_Veh[slot][vColor1], g_Veh[slot][vColor2], g_Veh[slot][vKeyDbId], mods, g_Veh[slot][vPaintjob],
        g_Veh[slot][vRegisteredUntil], g_Veh[slot][vDbId]);
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

// The plate only shows on a freshly created vehicle, so a new plate means building the vehicle again where it stands,
// with everyone inside put back in their seats.
stock Veh_Rebuild(slot)
{
    new vehicleid = g_Veh[slot][vVehicleId], Float:x, Float:y, Float:z, Float:angle;
    Veh_ReadState(slot);
    GetVehiclePos(vehicleid, x, y, z);
    GetVehicleZAngle(vehicleid, angle);
    new interior = GetVehicleInterior(vehicleid), world = GetVehicleVirtualWorld(vehicleid);

    new seats[MAX_PLAYERS];
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        seats[i] = IsPlayerInVehicle(i, vehicleid) ? GetPlayerVehicleSeat(i) : -1;
    }

    DestroyVehicle(vehicleid);
    g_VehSlotOf[vehicleid] = -1;
    if (!Veh_SpawnAt(slot, x, y, z, angle, interior, world))
    {
        return;
    }
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (seats[i] != -1)
        {
            PutPlayerInVehicle(i, g_Veh[slot][vVehicleId], seats[i]);
        }
    }
}

static stock Veh_LoadRow(slot, row, ownerid)
{
    new value, mods[128];
    g_Veh[slot][vUsed] = true;
    g_Veh[slot][vOwnerId] = ownerid;
    g_Veh[slot][vVehicleId] = INVALID_VEHICLE_ID;
    g_Veh[slot][vDiedAt] = 0;
    g_Veh[slot][vCameraSlot] = -1;
    cache_get_value_name_int(row, "id", g_Veh[slot][vDbId]);
    cache_get_value_name_int(row, "owner_id", g_Veh[slot][vOwnerDbId]);
    cache_get_value_name(row, "owner_name", g_Veh[slot][vOwnerName], MAX_PLAYER_NAME + 1);
    cache_get_value_name_int(row, "model", g_Veh[slot][vModel]);
    cache_get_value_name_int(row, "color1", g_Veh[slot][vColor1]);
    cache_get_value_name_int(row, "color2", g_Veh[slot][vColor2]);
    cache_get_value_name(row, "plate_text", g_Veh[slot][vPlate], VEH_PLATE_LENGTH);
    cache_get_value_name(row, "pending_text", g_Veh[slot][vPendingPlate], VEH_PLATE_LENGTH);
    cache_get_value_name_int(row, "registered_until", g_Veh[slot][vRegisteredUntil]);
    cache_get_value_name_float(row, "park_x", g_Veh[slot][vParkX]);
    cache_get_value_name_float(row, "park_y", g_Veh[slot][vParkY]);
    cache_get_value_name_float(row, "park_z", g_Veh[slot][vParkZ]);
    cache_get_value_name_float(row, "park_a", g_Veh[slot][vParkA]);
    cache_get_value_name_int(row, "parked", value);
    g_Veh[slot][vParked] = value != 0;
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
    cache_get_value_name(row, "mods", mods);
    for (new i = 0; i < VEH_TUNE_SLOTS; i++)
    {
        g_Veh[slot][vMods][i] = 0;
    }
    sscanf(mods, "p<,>a<i>[" #VEH_TUNE_SLOTS "]", g_Veh[slot][vMods]);
    cache_get_value_name_int(row, "paintjob", g_Veh[slot][vPaintjob]);
}

static stock Veh_QueryOwned(playerid, const where[], id, bool:remind)
{
    new query[512];
    mysql_format(g_SQL, query, sizeof(query), VEH_SELECT "WHERE v.`%e` = %d ORDER BY v.`id`", where, id);
    mysql_tquery(g_SQL, query, "OnVehiclesLoaded", "ddd", playerid, g_AccountRace[playerid], remind);
}

stock Veh_OnLogin(playerid)
{
    Veh_QueryOwned(playerid, "owner_id", g_Player[playerid][pDbId], true);
}

// Called on startup for every active owner (playerid invalid) and on login for the owner's own vehicles.
forward OnVehiclesLoaded(playerid, race, bool:remind);
public OnVehiclesLoaded(playerid, race, bool:remind)
{
    if (playerid != INVALID_PLAYER_ID && race != g_AccountRace[playerid])
    {
        return 1;
    }

    new rows = cache_num_rows(), dbid, loaded;
    for (new row = 0; row < rows; row++)
    {
        cache_get_value_name_int(row, "id", dbid);
        new slot = Veh_FindByDbId(dbid);
        if (slot != -1)
        {
            // Already in the world (it stays there while the owner is away): only the owner is known again.
            g_Veh[slot][vOwnerId] = playerid;
            continue;
        }
        slot = Veh_FindFreeSlot();
        if (slot == -1)
        {
            print("[VEHICLES] No free slot for a personal vehicle; raise MAX_PERSONAL_VEHICLES.");
            break;
        }
        Veh_LoadRow(slot, row, playerid);
        if (Veh_Spawn(slot))
        {
            loaded++;
        }
    }
    if (playerid == INVALID_PLAYER_ID)
    {
        printf("[VEHICLES] %d personal vehicles loaded.", loaded);
    }
    if (remind)
    {
        Veh_RemindRegistration(playerid);
    }
    return 1;
}

// The vehicles stay in the world; a key copy keeps working while the owner is away.
stock Veh_OnPlayerDisconnect(playerid)
{
    for (new slot = 0; slot < MAX_PERSONAL_VEHICLES; slot++)
    {
        if (g_Veh[slot][vUsed] && g_Veh[slot][vOwnerId] == playerid)
        {
            g_Veh[slot][vOwnerId] = INVALID_PLAYER_ID;
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
        if (g_Veh[slot][vUsed] && g_Veh[slot][vDiedAt] != 0 && now - g_Veh[slot][vDiedAt] >= VEH_DEATH_RESPAWN_S)
        {
            Veh_RespawnAfterDeath(slot);
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

static stock bool:Veh_HasEngine(vehicleid)
{
    return Fuel_TankSize(GetVehicleModel(vehicleid)) > 0 || g_IsAdminVehicle[vehicleid];
}

// Switching off needs no key (it is in the ignition); starting does, and takes a moment like a real engine.
stock Veh_ToggleEngine(playerid)
{
    if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
    {
        Msg_Error(playerid, "Motor pali i gasi vozac.");
        return;
    }
    new vehicleid = GetPlayerVehicleID(playerid);
    if (!Veh_HasEngine(vehicleid))
    {
        Msg_Error(playerid, "Ovo vozilo nema motor.");
        return;
    }
    if (g_VehStartTimer[playerid] != 0)
    {
        return;
    }
    if (Veh_IsEngineOn(vehicleid))
    {
        Veh_SetParam(vehicleid, VEH_PARAM_ENGINE, false);
        Chat_Action(playerid, "gasi motor.");
        return;
    }
    if (!Veh_HasKey(playerid, vehicleid))
    {
        Msg_Error(playerid, "Nemate kljuc ovog vozila, pa ne mozete upaliti motor.");
        return;
    }
    if (Fuel_IsUsed(vehicleid) && g_VehFuel[vehicleid] <= 0.0)
    {
        Msg_Error(playerid, "Vozilo nema goriva. Benzinska pumpa se trazi na /gps, a gorivo se toci sa /gorivo.");
        return;
    }
    Chat_Action(playerid, "okrece kljuc u bravi.");
    GameTextForPlayer(playerid, "~w~paljenje...", VEH_ENGINE_START_MS, 6);
    g_VehStartTimer[playerid] = SetTimerEx("Veh_FinishStart", VEH_ENGINE_START_MS, false, "ddd", playerid, vehicleid, g_AccountRace[playerid]);
}

forward Veh_FinishStart(playerid, vehicleid, race);
public Veh_FinishStart(playerid, vehicleid, race)
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }
    g_VehStartTimer[playerid] = 0;
    if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER || GetPlayerVehicleID(playerid) != vehicleid || Veh_IsEngineOn(vehicleid))
    {
        return 1;
    }
    new Float:health;
    GetVehicleHealth(vehicleid, health);
    if (health < VEH_ENGINE_WEAK_HEALTH && random(2) == 0)
    {
        Chat_Action(playerid, "pokusava da upali motor, ali motor se gusi.");
        GameTextForPlayer(playerid, "~r~motor nije upalio", 1500, 6);
        return 1;
    }
    Veh_SetParam(vehicleid, VEH_PARAM_ENGINE, true);
    GameTextForPlayer(playerid, "~g~motor upaljen", 1500, 6);
    return 1;
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
        if (!Veh_HasEngine(GetPlayerVehicleID(playerid)))
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
    new bool:running = Veh_IsEngineOn(vehicleid);
    if (!Veh_HasKey(playerid, vehicleid))
    {
        // The key is in a running engine, so the vehicle can be driven; without a key it cannot be started again.
        if (running)
        {
            Msg_Info(playerid, "Motor radi, pa mozete voziti. Ovo vozilo nije vase: kad se motor ugasi, bez kljuca ga ne mozete ponovo upaliti.");
        }
        else
        {
            Msg_Info(playerid, "Ovo vozilo nije vase i nemate kljuc, pa ne mozete upaliti motor.");
        }
        return;
    }
    if (!running && Fuel_IsUsed(vehicleid))
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

// component 0 removes the part in that slot.
stock Veh_SetMod(slot, CARMODTYPE:type, component)
{
    g_Veh[slot][vMods][_:type] = component;
    Veh_SaveState(slot);
}

stock Veh_SetPaintjob(slot, paintjob)
{
    g_Veh[slot][vPaintjob] = paintjob;
    Veh_SaveState(slot);
}

stock Veh_RandomPlate(plate[], size = sizeof(plate))
{
    format(plate, size, "LS %03d-%c%c", 100 + random(900),
        g_PlateLetters[random(sizeof(g_PlateLetters) - 1)], g_PlateLetters[random(sizeof(g_PlateLetters) - 1)]);
}

// Silver and Gold VIP get one more vehicle. When VIP ends nothing is taken away: the vehicles stay,
// but no new one can be bought or received until the owner is back under the limit.
stock Veh_MaxFor(playerid)
{
    return VEH_MAX_PER_PLAYER + (Vip_GetLevel(playerid) >= VIP_SILVER ? VEH_VIP_EXTRA : 0);
}

stock bool:Veh_HasFreeSlot(playerid)
{
    return Veh_CountOwned(g_Player[playerid][pDbId]) < Veh_MaxFor(playerid);
}

// A new vehicle has no plates (they come with the first registration). adminid is INVALID_PLAYER_ID for a market purchase.
// parked = false: the spot is only temporary (the market exit) until the owner leaves the vehicle somewhere else.
stock Veh_Insert(adminid, targetid, model, Float:x, Float:y, Float:z, Float:angle, Float:fuel, bool:parked)
{
    new query[640];
    mysql_format(g_SQL, query, sizeof(query),
        "INSERT INTO `vehicles` (`id`, `owner_id`, `model`, `color1`, `color2`, `park_x`, `park_y`, `park_z`, `park_a`, `fuel`, `parked`) " \
        "SELECT " DB_FREE_ID("vehicles") ", %d, %d, %d, %d, %f, %f, %f, %f, %f, %d",
        g_Player[targetid][pDbId], model, random(127), random(127), x, y, z, angle, fuel, parked ? 1 : 0);
    mysql_tquery(g_SQL, query, "OnVehicleInserted", "ddd", adminid, targetid, g_AccountRace[targetid]);
}

stock Veh_CreateFor(adminid, targetid, model)
{
    new Float:x, Float:y, Float:z, Float:angle;
    GetPlayerPos(targetid, x, y, z);
    GetPlayerFacingAngle(targetid, angle);
    x += 4.0 * floatcos(angle, degrees);
    y += 4.0 * floatsin(angle, degrees);
    Veh_Insert(adminid, targetid, model, x, y, z, angle, float(Fuel_TankSize(model)), true);
}

forward OnVehicleInserted(adminid, targetid, race);
public OnVehicleInserted(adminid, targetid, race)
{
    if (race != g_AccountRace[targetid])
    {
        return 1;
    }
    new dbid = cache_insert_id();
    Veh_QueryOwned(targetid, "id", dbid, false);
    if (adminid == INVALID_PLAYER_ID)
    {
        return 1;
    }

    new details[32];
    format(details, sizeof(details), "vozilo %d", dbid);
    if (Account_IsLoggedIn(adminid))
    {
        Msg_Admin(adminid, "Igrac %s je dobio vozilo (ID u bazi %d).", Account_Name(targetid), dbid);
        Staff_Log(adminid, "giveveh", targetid, details);
    }
    Msg_Success(targetid, "Dobili ste vozilo, stoji pored vas. Parkirajte ga sa /v parkiraj; registruje se na tehnickom pregledu (/registruj), a tablice se vade u policiji (/tablice).");
    return 1;
}

// Called when a driver leaves a vehicle: a market vehicle that was never parked takes the first spot away from the market,
// so bought vehicles do not all come back to the market exit after a restart.
stock Veh_OnPlayerExitVehicle(playerid, vehicleid)
{
    new slot = g_VehSlotOf[vehicleid];
    if (slot == -1 || g_Veh[slot][vParked] || !Veh_IsOwner(playerid, vehicleid) || Market_IsNearExit(vehicleid))
    {
        return;
    }
    Veh_SetParkingHere(slot);
    Msg_Info(playerid, "Vozilo %s je parkirano ovde i ovde se vraca posle restarta. Drugo mesto: /v parkiraj.", Veh_Name(slot));
}

// The vehicle's current spot becomes its parking spot.
stock Veh_SetParkingHere(slot)
{
    new vehicleid = g_Veh[slot][vVehicleId];
    GetVehiclePos(vehicleid, g_Veh[slot][vParkX], g_Veh[slot][vParkY], g_Veh[slot][vParkZ]);
    GetVehicleZAngle(vehicleid, g_Veh[slot][vParkA]);
    g_Veh[slot][vParked] = true;

    new query[224];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `vehicles` SET `park_x` = %f, `park_y` = %f, `park_z` = %f, `park_a` = %f, `parked` = 1 WHERE `id` = %d",
        g_Veh[slot][vParkX], g_Veh[slot][vParkY], g_Veh[slot][vParkZ], g_Veh[slot][vParkA], g_Veh[slot][vDbId]);
    mysql_tquery(g_SQL, query);
}

// A sold vehicle stays where it stands, and that becomes the new owner's parking spot; the old key copy stops working.
stock Veh_TransferTo(slot, buyerid)
{
    g_Veh[slot][vOwnerDbId] = g_Player[buyerid][pDbId];
    g_Veh[slot][vOwnerId] = buyerid;
    strcopy(g_Veh[slot][vOwnerName], g_Player[buyerid][pName], MAX_PLAYER_NAME + 1);
    g_Veh[slot][vKeyDbId] = 0;
    g_Veh[slot][vKeyName][0] = '\0';
    new query[96];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `vehicles` SET `owner_id` = %d WHERE `id` = %d", g_Veh[slot][vOwnerDbId], g_Veh[slot][vDbId]);
    mysql_tquery(g_SQL, query);
    Veh_SetParkingHere(slot);
    Veh_SaveState(slot);
}

stock Veh_ShowList(playerid)
{
    new body[768], line[160], count, dbid = g_Player[playerid][pDbId];
    strcopy(body, "Vozilo\tGorivo\tGde je\tRegistracija\tKopija kljuca\n");
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
        new zone = Hud_FindZone(x, y), place[32], key[MAX_PLAYER_NAME + 1], registration[24];
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
        Veh_RegistrationText(slot, registration);
        format(line, sizeof(line), "%s\t%s\t%s\t%s\t%s\n", Veh_Name(slot), fuel, place, registration, key);
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
    new body[384], line[64];
    strcopy(body, "Oznaci na GPS-u\n");
    if (g_Veh[slot][vKeyDbId] != 0)
    {
        format(line, sizeof(line), "Oduzmi kopiju kljuca (%s)\n", g_Veh[slot][vKeyName]);
        strcat(body, line);
    }
    // The rest works on the vehicle the player sits in or stands next to, like the /v commands they run.
    strcat(body, "Zakljucaj / otkljucaj (pored vozila)\nParkiraj ovde (za volanom)\nPostavi izvadjene tablice (pored vozila)\nDaj kopiju kljuca igracu pored vas\nProdaj ili zameni (/ponudi)");
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
            else
            {
                switch (listitem - (g_Veh[slot][vKeyDbId] != 0 ? 2 : 1))
                {
                    case 0: PC_EmulateCommand(playerid, "/v zakljucaj");
                    case 1: PC_EmulateCommand(playerid, "/v parkiraj");
                    case 2: PC_EmulateCommand(playerid, "/v tablice");
                    case 3: Menu_AskInput(playerid, "v kljuc", "ID ili ime igraca pored vas:");
                    case 4: Menu_AskInput(playerid, "ponudi", "ID ili ime igraca pored vas:");
                }
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
    Msg_Info(targetid, "Dobili ste kopiju kljuca od vozila %s. Mozete ga otkljucati (Y) i voziti (N), i kad vlasnik nije tu.", Veh_Name(slot));
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

    if (Market_IsNearExit(vehicleid))
    {
        Msg_Error(playerid, "Na izlazu sa auto pijace se ne parkira; tu se pojavljuju kupljena vozila.");
        return;
    }

    new slot = g_VehSlotOf[vehicleid];
    Veh_SetParkingHere(slot);
    Msg_Success(playerid, "Vozilo %s je parkirano ovde. Ovde se vraca posle restarta servera, unistenja ili admin reseta.", Veh_Name(slot));
}

// Health, damage and fuel stay as they are, so this is never a free repair. Returns false for an occupied or burning vehicle.
stock bool:Veh_ReturnToParking(slot)
{
    new vehicleid = g_Veh[slot][vVehicleId];
    if (!g_Veh[slot][vUsed] || vehicleid == INVALID_VEHICLE_ID || g_Veh[slot][vDiedAt] != 0 || IsVehicleOccupied(vehicleid))
    {
        return false;
    }
    SetVehicleVelocity(vehicleid, 0.0, 0.0, 0.0);
    SetVehiclePos(vehicleid, g_Veh[slot][vParkX], g_Veh[slot][vParkY], g_Veh[slot][vParkZ]);
    SetVehicleZAngle(vehicleid, g_Veh[slot][vParkA]);
    LinkVehicleToInterior(vehicleid, 0);
    SetVehicleVirtualWorld(vehicleid, 0);
    return true;
}

stock Veh_ReturnAllToParking()
{
    new count;
    for (new slot = 0; slot < MAX_PERSONAL_VEHICLES; slot++)
    {
        if (Veh_ReturnToParking(slot))
        {
            count++;
        }
    }
    return count;
}

// Everyone gets a warning first, so whoever wants to keep a vehicle where it is can sit in it.
stock bool:Veh_StartReset(adminid, seconds)
{
    if (g_VehResetTimer != 0)
    {
        return false;
    }
    g_VehResetTimer = SetTimerEx("Veh_FinishReset", seconds * 1000, false, "dd", adminid, g_AccountRace[adminid]);
    return true;
}

forward Veh_FinishReset(adminid, race);
public Veh_FinishReset(adminid, race)
{
    g_VehResetTimer = 0;
    new count = Veh_ReturnAllToParking(), removed = Staff_DestroyAdminVehicles(true), message[144];
    format(message, sizeof(message), "[ADMIN] Prazna vozila su vracena na parking (licnih: %d, obrisanih admin vozila: %d).", count, removed);
    Chat_SendToAll(COLOR_ADMIN, message);
    if (race == g_AccountRace[adminid] && Account_IsLoggedIn(adminid))
    {
        new details[48];
        format(details, sizeof(details), "%d licnih, %d admin", count, removed);
        Staff_Log(adminid, "resetveh", INVALID_PLAYER_ID, details);
    }
    return 1;
}
