// Litres per second: a little while idling plus a part that grows with speed. Tuned so a 55 l tank lasts
// about 45 minutes at city speed; other tanks burn in proportion, so every vehicle lasts about as long.
#define FUEL_IDLE_PER_S         (0.002)
#define FUEL_PER_KMH_S          (0.00023)
#define FUEL_REFERENCE_TANK     (55.0)
#define FUEL_RESERVE_PERCENT    (15)
// Velocity units to km/h, the factor most speedometers use.
#define VEH_SPEED_TO_KMH        (180.0)

// 0 means the vehicle does not use fuel (bicycles, trailers, trains, RC toys).
stock Fuel_TankSize(model)
{
    switch (model)
    {
        case 481, 509, 510, 435, 450, 584, 591, 606, 607, 608, 610, 611, 449, 537, 538, 569, 570, 590, 441, 464, 465, 501, 564, 594:
        {
            return 0;
        }
        case 448, 461, 462, 463, 468, 471, 521, 522, 523, 581, 586:
        {
            return 15;
        }
        case 403, 406, 407, 408, 413, 414, 416, 418, 423, 427, 428, 431, 433, 437, 440, 443, 444, 455, 456, 459, 482, 498, 499, 508,
            514, 515, 524, 525, 528, 532, 544, 556, 557, 573, 578, 582, 588, 601, 609,
            417, 425, 447, 460, 469, 476, 487, 488, 497, 511, 512, 513, 519, 520, 548, 553, 563, 577, 592, 593,
            430, 446, 452, 453, 454, 472, 473, 484, 493, 595:
        {
            return 100;
        }
    }
    return 55;
}

stock bool:Fuel_IsUsed(vehicleid)
{
    return !g_IsAdminVehicle[vehicleid] && Fuel_TankSize(GetVehicleModel(vehicleid)) > 0;
}

stock Fuel_Set(vehicleid, Float:litres)
{
    g_VehFuel[vehicleid] = litres;
    g_VehFuelReserveWarned[vehicleid] = false;
}

stock Fuel_Percent(vehicleid)
{
    new tank = Fuel_TankSize(GetVehicleModel(vehicleid));
    return tank == 0 ? 100 : floatround(g_VehFuel[vehicleid] * 100.0 / float(tank), floatround_floor);
}

stock Float:Veh_SpeedKmh(vehicleid)
{
    new Float:vx, Float:vy, Float:vz;
    GetVehicleVelocity(vehicleid, vx, vy, vz);
    return VectorSize(vx, vy, vz) * VEH_SPEED_TO_KMH;
}

// Runs every second for every vehicle with its engine running, with or without a driver.
stock Fuel_Tick()
{
    for (new vehicleid = 1; vehicleid < MAX_VEHICLES; vehicleid++)
    {
        if (!IsValidVehicle(vehicleid) || !Fuel_IsUsed(vehicleid) || !Veh_IsEngineOn(vehicleid))
        {
            continue;
        }

        new tank = Fuel_TankSize(GetVehicleModel(vehicleid));
        new Float:used = (FUEL_IDLE_PER_S + Veh_SpeedKmh(vehicleid) * FUEL_PER_KMH_S) * float(tank) / FUEL_REFERENCE_TANK;
        g_VehFuel[vehicleid] -= used;

        new driver = GetVehicleDriver(vehicleid);
        if (g_VehFuel[vehicleid] <= 0.0)
        {
            g_VehFuel[vehicleid] = 0.0;
            Veh_SetParam(vehicleid, VEH_PARAM_ENGINE, false);
            if (driver != INVALID_PLAYER_ID)
            {
                Msg_Error(driver, "Vozilo je ostalo bez goriva. Benzinska pumpa: /gps, a gorivo se toci sa /gorivo.");
            }
        }
        else if (!g_VehFuelReserveWarned[vehicleid] && Fuel_Percent(vehicleid) < FUEL_RESERVE_PERCENT)
        {
            g_VehFuelReserveWarned[vehicleid] = true;
            if (driver != INVALID_PLAYER_ID)
            {
                Msg_Info(driver, "Upalila se lampica za rezervu: ostalo je %.0f l goriva.", g_VehFuel[vehicleid]);
            }
        }
    }
}

cmd:gorivo(playerid, params[])
{
    if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
    {
        Msg_Error(playerid, "Gorivo se toci dok sedite za volanom, kod benzinske pumpe.");
        return 1;
    }
    new vehicleid = GetPlayerVehicleID(playerid);
    if (!Fuel_IsUsed(vehicleid))
    {
        Msg_Error(playerid, "Ovo vozilo ne koristi gorivo.");
        return 1;
    }
    if (!Point_Require(playerid, POINT_FUEL))
    {
        return 1;
    }
    // Pouring fuel into a running engine is how cars catch fire.
    if (Veh_IsEngineOn(vehicleid))
    {
        Msg_Error(playerid, "Prvo ugasite motor (taster N ili /v motor).");
        return 1;
    }

    new tank = Fuel_TankSize(GetVehicleModel(vehicleid));
    new missing = floatround(float(tank) - g_VehFuel[vehicleid], floatround_floor);
    if (missing < 1)
    {
        Msg_Info(playerid, "Rezervoar je pun.");
        return 1;
    }

    new litres = missing;
    if (!isnull(params) && (sscanf(params, "d", litres) || litres < 1))
    {
        Msg_Usage(playerid, "/gorivo [litara] (bez broja toci do punog, $%d po litru)", FUEL_PRICE_PER_LITRE);
        return 1;
    }
    litres = min(litres, missing);

    new reason[32];
    format(reason, sizeof(reason), "gorivo: %d l", litres);
    if (!Money_Take(playerid, litres * FUEL_PRICE_PER_LITRE, reason))
    {
        return 1;
    }
    Fuel_Set(vehicleid, g_VehFuel[vehicleid] + float(litres));
    Chat_Action(playerid, "toci gorivo u vozilo.");
    Msg_Success(playerid, "Natocili ste %d l za $%d. U rezervoaru: %.0f/%d l.", litres, litres * FUEL_PRICE_PER_LITRE, g_VehFuel[vehicleid], tank);
    Veh_SaveIfPersonal(vehicleid);
    return 1;
}
alias:gorivo("fuel")
