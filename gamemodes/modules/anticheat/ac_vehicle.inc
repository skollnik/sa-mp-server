// Remembers the vehicle as the server just made it; the client is given a moment to show it before checks resume.
static stock AC_VehicleBaseline(vehicleid)
{
    new VEHICLE_PANEL_STATUS:panels, VEHICLE_DOOR_STATUS:doors, VEHICLE_LIGHT_STATUS:lights, VEHICLE_TYRE_STATUS:tyres;
    GetVehicleHealth(vehicleid, g_AcVehicle[vehicleid][avHealth]);
    GetVehicleDamageStatus(vehicleid, panels, doors, lights, tyres);
    g_AcVehicle[vehicleid][avTyres] = _:tyres;
    g_AcVehicle[vehicleid][avSetHealth] = g_AcVehicle[vehicleid][avHealth];
    g_AcVehicle[vehicleid][avSetTyres] = _:tyres;
    g_AcVehicle[vehicleid][avSetAt] = gettime();
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

static stock bool:AC_VehicleMended(Float:health, tyres, Float:was_health, was_tyres)
{
    return health > was_health + 1.0 || (was_tyres & ~tyres) != 0;
}

// Health never rises and popped tyres never mend unless the server did it through the AC_* functions above,
// so a rise means a repair hack: the vehicle is put back as it was and admins are told who was driving.
// Dents, doors and lights are left out: after a repair the client reports them its own way, and they are only looks.
static stock AC_CheckVehicle(vehicleid, now)
{
    if (now < g_AcVehicle[vehicleid][avSyncUntil])
    {
        return;
    }
    new Float:health, VEHICLE_PANEL_STATUS:panels, VEHICLE_DOOR_STATUS:doors, VEHICLE_LIGHT_STATUS:lights, VEHICLE_TYRE_STATUS:tyre_status;
    GetVehicleHealth(vehicleid, health);
    GetVehicleDamageStatus(vehicleid, panels, doors, lights, tyre_status);
    new tyres = _:tyre_status;
    if (!AC_VehicleMended(health, tyres, g_AcVehicle[vehicleid][avHealth], g_AcVehicle[vehicleid][avTyres])
        || (now - g_AcVehicle[vehicleid][avSetAt] < AC_VEHICLE_CONFIRM_S
            && !AC_VehicleMended(health, tyres, g_AcVehicle[vehicleid][avSetHealth], g_AcVehicle[vehicleid][avSetTyres])))
    {
        g_AcVehicle[vehicleid][avHealth] = health;
        g_AcVehicle[vehicleid][avTyres] = tyres;
        return;
    }
    new Float:allowed = g_AcVehicle[vehicleid][avHealth];
    SetVehicleHealth(vehicleid, allowed);
    UpdateVehicleDamageStatus(vehicleid, panels, doors, lights, VEHICLE_TYRE_STATUS:g_AcVehicle[vehicleid][avTyres]);
    g_AcVehicle[vehicleid][avSyncUntil] = now + AC_VEHICLE_SYNC_S;
    new driver = GetVehicleDriver(vehicleid);
    if (driver != INVALID_PLAYER_ID && Account_IsLoggedIn(driver))
    {
        new details[64];
        format(details, sizeof(details), "%s, HP %.0f -> %.0f%s, vraceno", Veh_ModelName(GetVehicleModel(vehicleid)), allowed, health,
            (g_AcVehicle[vehicleid][avTyres] & ~tyres) != 0 ? (", gume") : (""));
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
