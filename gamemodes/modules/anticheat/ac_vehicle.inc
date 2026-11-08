// Remembers the vehicle as the server just made it; the client is given a moment to show it before checks resume.
static stock AC_VehicleBaseline(vehicleid)
{
    new VEHICLE_PANEL_STATUS:panels, VEHICLE_DOOR_STATUS:doors, VEHICLE_LIGHT_STATUS:lights, VEHICLE_TYRE_STATUS:tyres;
    GetVehicleHealth(vehicleid, g_AcVehicle[vehicleid][avHealth]);
    GetVehicleDamageStatus(vehicleid, panels, doors, lights, tyres);
    g_AcVehicle[vehicleid][avPanels] = _:panels;
    g_AcVehicle[vehicleid][avDoors] = _:doors;
    g_AcVehicle[vehicleid][avLights] = _:lights;
    g_AcVehicle[vehicleid][avTyres] = _:tyres;
    g_AcVehicle[vehicleid][avSyncUntil] = gettime() + AC_VEHICLE_SYNC_S;
}

stock AC_CreateVehicle(modelid, Float:x, Float:y, Float:z, Float:angle, colour1, colour2, respawn_delay)
{
    new vehicleid = CreateVehicle(modelid, x, y, z, angle, colour1, colour2, respawn_delay);
    if (vehicleid != INVALID_VEHICLE_ID)
    {
        AC_VehicleBaseline(vehicleid);
    }
    return vehicleid;
}

stock AC_SetVehicleHealth(vehicleid, Float:health)
{
    SetVehicleHealth(vehicleid, health);
    AC_VehicleBaseline(vehicleid);
}

stock AC_RepairVehicle(vehicleid)
{
    RepairVehicle(vehicleid);
    AC_VehicleBaseline(vehicleid);
}

stock AC_UpdateVehicleDamageStatus(vehicleid, VEHICLE_PANEL_STATUS:panels, VEHICLE_DOOR_STATUS:doors, VEHICLE_LIGHT_STATUS:lights, VEHICLE_TYRE_STATUS:tyres)
{
    UpdateVehicleDamageStatus(vehicleid, panels, doors, lights, tyres);
    AC_VehicleBaseline(vehicleid);
}

// A respawned vehicle comes back new, as the game makes it.
stock AC_OnVehicleSpawn(vehicleid)
{
    AC_VehicleBaseline(vehicleid);
}

// Damage only ever gets worse on its own: broken lights and popped tyres stay broken, panels only dent further,
// and a door stays dented (2) or missing (4); its swinging bit (1) changes whenever it is shut, so it is left out.
static stock bool:AC_DamageReduced(vehicleid, panels, doors, lights, tyres)
{
    if ((g_AcVehicle[vehicleid][avLights] & ~lights) != 0 || (g_AcVehicle[vehicleid][avTyres] & ~tyres) != 0)
    {
        return true;
    }
    for (new shift = 0; shift < 28; shift += 4)
    {
        if (((panels >>> shift) & 0xF) < ((g_AcVehicle[vehicleid][avPanels] >>> shift) & 0xF))
        {
            return true;
        }
    }
    for (new shift = 0; shift < 32; shift += 8)
    {
        if (((g_AcVehicle[vehicleid][avDoors] >>> shift) & 0x6 & ~((doors >>> shift) & 0x6)) != 0)
        {
            return true;
        }
    }
    return false;
}

// Health never rises and damage never heals unless the server did it through the AC_* functions above,
// so a rise means a repair hack: the vehicle is put back as it was and admins are told who was driving.
static stock AC_CheckVehicle(vehicleid, now)
{
    if (now < g_AcVehicle[vehicleid][avSyncUntil])
    {
        return;
    }
    new Float:health, VEHICLE_PANEL_STATUS:panel_status, VEHICLE_DOOR_STATUS:door_status, VEHICLE_LIGHT_STATUS:light_status, VEHICLE_TYRE_STATUS:tyre_status;
    GetVehicleHealth(vehicleid, health);
    GetVehicleDamageStatus(vehicleid, panel_status, door_status, light_status, tyre_status);
    new panels = _:panel_status, doors = _:door_status, lights = _:light_status, tyres = _:tyre_status;
    if (health <= g_AcVehicle[vehicleid][avHealth] + 1.0 && !AC_DamageReduced(vehicleid, panels, doors, lights, tyres))
    {
        g_AcVehicle[vehicleid][avHealth] = health;
        g_AcVehicle[vehicleid][avPanels] = panels;
        g_AcVehicle[vehicleid][avDoors] = doors;
        g_AcVehicle[vehicleid][avLights] = lights;
        g_AcVehicle[vehicleid][avTyres] = tyres;
        return;
    }
    new Float:allowed = g_AcVehicle[vehicleid][avHealth];
    SetVehicleHealth(vehicleid, allowed);
    UpdateVehicleDamageStatus(vehicleid, VEHICLE_PANEL_STATUS:g_AcVehicle[vehicleid][avPanels], VEHICLE_DOOR_STATUS:g_AcVehicle[vehicleid][avDoors],
        VEHICLE_LIGHT_STATUS:g_AcVehicle[vehicleid][avLights], VEHICLE_TYRE_STATUS:g_AcVehicle[vehicleid][avTyres]);
    g_AcVehicle[vehicleid][avSyncUntil] = now + AC_VEHICLE_SYNC_S;
    new driver = GetVehicleDriver(vehicleid);
    if (driver != INVALID_PLAYER_ID && Account_IsLoggedIn(driver))
    {
        new details[64];
        format(details, sizeof(details), "%s, HP %.0f -> %.0f, vraceno", Veh_ModelName(GetVehicleModel(vehicleid)), allowed, health);
        AC_Alert(driver, AC_TYPE_VEHICLE_REPAIR, details);
    }
}

stock AC_CheckVehicles()
{
    new now = gettime();
    for (new vehicleid = 1; vehicleid < MAX_VEHICLES; vehicleid++)
    {
        if (IsValidVehicle(vehicleid))
        {
            AC_CheckVehicle(vehicleid, now);
        }
    }
}
