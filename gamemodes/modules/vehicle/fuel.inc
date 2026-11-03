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

// A jerry can is filled at the pump at the pump price and gets a stranded vehicle to the next gas station.
// It keeps whatever litres did not fit into the tank.
#define CANISTER_POUR_MS        (5000)
#define CANISTER_RANGE          (3.0)

new g_CanisterTimer[MAX_PLAYERS] = { -1, ... };
new g_CanisterSlot[MAX_PLAYERS];

stock Fuel_OnPlayerConnect(playerid)
{
    g_CanisterTimer[playerid] = -1;
}

// How many more litres a slot can take: a part-full can up to the brim, an empty can a whole can, anything else none.
static stock Fuel_CanisterRoom(playerid, slot)
{
    switch (g_Inv[playerid][slot][isItem])
    {
        case ITEM_CANISTER_FULL: return CANISTER_LITRES - g_Inv[playerid][slot][isAmount];
        case ITEM_CANISTER: return CANISTER_LITRES;
    }
    return 0;
}

// /gorivo [litara] on foot. Part-full cans are topped up before empty ones are started, so the fuel ends up
// in as few cans as possible; without a number every can is filled.
static stock Fuel_FillCanisters(playerid, const params[])
{
    new wanted = cellmax, room;
    if (!isnull(params) && (sscanf(params, "d", wanted) || wanted < 1))
    {
        Msg_Usage(playerid, "/gorivo [litara] (peske pored pumpe puni kanistere; bez broja do punog, $%d po litru)", FUEL_PRICE_PER_LITRE);
        return;
    }
    for (new slot = 0; slot < INV_MAX_SLOTS; slot++)
    {
        room += Fuel_CanisterRoom(playerid, slot);
    }
    if (room == 0)
    {
        Msg_Error(playerid, "Gorivo se toci dok sedite za volanom; peske se puni kanister (prazan se kupuje u 24/7), a vasi su puni.");
        return;
    }
    if (!Point_Require(playerid, POINT_FUEL))
    {
        return;
    }
    new litres = min(wanted, room), price = litres * FUEL_PRICE_PER_LITRE;
    if (!Money_Take(playerid, price, "gorivo: kanister"))
    {
        return;
    }
    new left = litres;
    for (new pass = 0; pass < 2 && left > 0; pass++)
    {
        new E_ITEM:kind = pass == 0 ? ITEM_CANISTER_FULL : ITEM_CANISTER;
        for (new slot = 0; slot < INV_MAX_SLOTS && left > 0; slot++)
        {
            new space = Fuel_CanisterRoom(playerid, slot);
            if (g_Inv[playerid][slot][isItem] != kind || space == 0)
            {
                continue;
            }
            new put = min(left, space);
            Inv_SetSlot(playerid, slot, ITEM_CANISTER_FULL, CANISTER_LITRES - space + put);
            left -= put;
        }
    }
    Chat_Action(playerid, "puni kanister gorivom.");
    Msg_Success(playerid, "Natocili ste %d l u kanister za $%d. Sipa se pored vozila: /inv, pa kanister.", litres, price);
}

// Clearing animations in a vehicle throws the player out of it, and on F it would also stop them getting in.
static stock Fuel_StopCanister(playerid, bool:clear = true)
{
    KillTimer(g_CanisterTimer[playerid]);
    g_CanisterTimer[playerid] = -1;
    if (clear && GetPlayerState(playerid) == PLAYER_STATE_ONFOOT)
    {
        ClearAnimations(playerid);
    }
}

// Also on death and disconnect, so the timer never runs for a player who is gone.
stock Fuel_CancelCanister(playerid)
{
    if (g_CanisterTimer[playerid] != -1)
    {
        Fuel_StopCanister(playerid);
    }
}

// Any key (F to enter a vehicle, sprint, jump, fire) stops the pouring right away.
stock Fuel_OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys)
{
    if (g_CanisterTimer[playerid] != -1 && (newkeys & ~oldkeys))
    {
        Fuel_StopCanister(playerid, !(newkeys & KEY_SECONDARY_ATTACK));
        Msg_Info(playerid, "Sipanje goriva je prekinuto.");
    }
}

// Called from the inventory with the can's slot; litres leave the can only when the pouring is done.
stock Fuel_StartCanister(playerid, slot)
{
    if (g_CanisterTimer[playerid] != -1)
    {
        return;
    }
    new vehicleid = Veh_FindClosest(playerid, CANISTER_RANGE);
    if (vehicleid == INVALID_VEHICLE_ID || !Fuel_IsUsed(vehicleid))
    {
        Msg_Error(playerid, "Stanite pored vozila koje koristi gorivo.");
        return;
    }
    if (Veh_IsEngineOn(vehicleid))
    {
        Msg_Error(playerid, "Prvo treba ugasiti motor vozila.");
        return;
    }
    if (float(Fuel_TankSize(GetVehicleModel(vehicleid))) - g_VehFuel[vehicleid] < 0.5)
    {
        Msg_Info(playerid, "Rezervoar je pun.");
        return;
    }
    g_CanisterSlot[playerid] = slot;
    ApplyAnimation(playerid, "BOMBER", "BOM_Plant_Loop", 4.1, true, false, false, false, 0, SYNC_ALL);
    Chat_Action(playerid, "sipa gorivo iz kanistera u vozilo.");
    Msg_Info(playerid, "Sipanje traje %d s; bilo koji taster ga prekida.", CANISTER_POUR_MS / 1000);
    g_CanisterTimer[playerid] = SetTimerEx("Fuel_FinishCanister", CANISTER_POUR_MS, false, "dd", playerid, vehicleid);
}

forward Fuel_FinishCanister(playerid, vehicleid);
public Fuel_FinishCanister(playerid, vehicleid)
{
    g_CanisterTimer[playerid] = -1;
    new slot = g_CanisterSlot[playerid];
    if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT)
    {
        return 1;
    }
    ClearAnimations(playerid);
    // The vehicle may have been driven off or started meanwhile, or the can given away.
    if (!IsValidVehicle(vehicleid) || Veh_FindClosest(playerid, CANISTER_RANGE) != vehicleid || Veh_IsEngineOn(vehicleid)
        || g_Inv[playerid][slot][isItem] != ITEM_CANISTER_FULL)
    {
        Msg_Error(playerid, "Sipanje je prekinuto.");
        return 1;
    }
    new Float:tank = float(Fuel_TankSize(GetVehicleModel(vehicleid)));
    new litres = min(g_Inv[playerid][slot][isAmount], floatround(tank - g_VehFuel[vehicleid], floatround_ceil));
    Fuel_Set(vehicleid, Utils_FloatMin(g_VehFuel[vehicleid] + float(litres), tank));
    Veh_SaveIfPersonal(vehicleid);

    new left = g_Inv[playerid][slot][isAmount] - litres;
    if (left > 0)
    {
        Inv_SetSlot(playerid, slot, ITEM_CANISTER_FULL, left);
    }
    else
    {
        Inv_SetSlot(playerid, slot, ITEM_CANISTER, 1);
    }
    Msg_Success(playerid, "Sipali ste %d l. U rezervoaru: %.0f/%.0f l, u kanisteru je ostalo %d l.", litres, g_VehFuel[vehicleid], tank, left);
    return 1;
}

cmd:gorivo(playerid, params[])
{
    if (GetPlayerState(playerid) == PLAYER_STATE_ONFOOT)
    {
        Fuel_FillCanisters(playerid, params);
        return 1;
    }
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
    new Float:missing = float(tank) - g_VehFuel[vehicleid];
    if (missing < 0.5)
    {
        Msg_Info(playerid, "Rezervoar je pun.");
        return 1;
    }

    // Driving leaves fractions of a litre; a top-up fills to the brim and a started litre is paid as a whole one.
    new litres = floatround(missing, floatround_ceil);
    if (!isnull(params) && (sscanf(params, "d", litres) || litres < 1))
    {
        Msg_Usage(playerid, "/gorivo [litara] (bez broja toci do punog, $%d po litru)", FUEL_PRICE_PER_LITRE);
        return 1;
    }
    new Float:poured = float(litres) < missing ? float(litres) : missing;
    litres = floatround(poured, floatround_ceil);

    new reason[32];
    format(reason, sizeof(reason), "gorivo: %d l", litres);
    if (!Money_Take(playerid, litres * FUEL_PRICE_PER_LITRE, reason))
    {
        return 1;
    }
    Fuel_Set(vehicleid, g_VehFuel[vehicleid] + poured);
    Chat_Action(playerid, "toci gorivo u vozilo.");
    Msg_Success(playerid, "Natocili ste %d l za $%d. U rezervoaru: %.0f/%d l.", litres, litres * FUEL_PRICE_PER_LITRE, g_VehFuel[vehicleid], tank);
    Veh_SaveIfPersonal(vehicleid);
    return 1;
}
alias:gorivo("fuel")
