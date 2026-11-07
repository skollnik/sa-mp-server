#define MAX_RENT_VEHICLES       (60)
#define RENT_MAX_HOURS          (3)
#define RENT_WARN_S             (300)
// How long someone may sit in a free rental before renting it.
#define RENT_DECIDE_S           (15)
// A free rental pushed away from its spot goes back after standing alone this long.
#define RENT_IDLE_RETURN_S      (600)
#define RENT_SPOT_RANGE         (5.0)
#define RENT_DEATH_RESPAWN_S    (10)

enum E_RENT
{
    bool:rnUsed,
    rnDbId,
    rnModel,
    Float:rnX,
    Float:rnY,
    Float:rnZ,
    Float:rnA,
    rnPrice,
    rnVehicle,
    rnRenter,
    rnRenterDbId,
    rnUntil,
    bool:rnWarned,
    rnAwaySince,
    rnDiedAt
}

new g_Rent[MAX_RENT_VEHICLES][E_RENT];
new g_RentOf[MAX_VEHICLES] = { -1, ... };
new g_RentDecideTimer[MAX_PLAYERS];

// The places reference gamemodes use most: cars and scooters in front of the airport (the spawn), bicycles at Unity Station.
static const Float:g_RentSeed[][5] =
{
    { 540.0, 1519.02, -2360.46, 13.53, 0.0 },
    { 458.0, 1512.52, -2360.63, 13.52, 0.0 },
    { 546.0, 1505.62, -2361.03, 13.40, 0.0 },
    { 426.0, 1499.27, -2361.04, 13.37, 0.0 },
    { 540.0, 1492.61, -2360.71, 13.53, 0.0 },
    { 462.0, 1560.11, -2338.39, 13.22, 90.0 },
    { 462.0, 1560.09, -2335.06, 13.22, 90.0 },
    { 462.0, 1560.07, -2331.86, 13.22, 90.0 },
    { 462.0, 1560.05, -2328.59, 13.22, 90.0 },
    { 462.0, 1560.03, -2325.44, 13.22, 90.0 },
    { 462.0, 1560.02, -2322.24, 13.22, 90.0 },
    { 509.0, 1790.00, -1885.50, 13.10, 180.0 },
    { 481.0, 1793.00, -1885.50, 13.10, 180.0 },
    { 509.0, 1796.00, -1885.50, 13.10, 180.0 },
    { 481.0, 1799.00, -1885.50, 13.10, 180.0 },
    { 510.0, 1802.00, -1885.50, 13.10, 180.0 }
};

stock Rent_DefaultPrice(model)
{
    switch (Veh_Class(model))
    {
        case VEH_CLASS_BICYCLE: return RENT_PRICE_BIKE;
        case VEH_CLASS_MOTO: return RENT_PRICE_MOTO;
    }
    return RENT_PRICE_CAR;
}

stock Rent_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `rent_vehicles` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`model` SMALLINT UNSIGNED NOT NULL," \
        "`x` FLOAT NOT NULL, `y` FLOAT NOT NULL, `z` FLOAT NOT NULL, `a` FLOAT NOT NULL," \
        "`price` INT NOT NULL" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
    // The flag keeps deleted starting rentals from coming back on the next restart.
    mysql_tquery(g_SQL, "SELECT `value` FROM `server_vars` WHERE `name` = 'rent_seeded'", "OnRentSeedChecked");
}

forward OnRentSeedChecked();
public OnRentSeedChecked()
{
    if (cache_num_rows() == 0)
    {
        new query[192];
        for (new i = 0; i < sizeof(g_RentSeed); i++)
        {
            new model = floatround(g_RentSeed[i][0]);
            mysql_format(g_SQL, query, sizeof(query), "INSERT INTO `rent_vehicles` (`model`, `x`, `y`, `z`, `a`, `price`) VALUES (%d, %f, %f, %f, %f, %d)",
                model, g_RentSeed[i][1], g_RentSeed[i][2], g_RentSeed[i][3], g_RentSeed[i][4], Rent_DefaultPrice(model));
            mysql_tquery(g_SQL, query);
        }
        mysql_tquery(g_SQL, "INSERT INTO `server_vars` (`name`, `value`) VALUES ('rent_seeded', 1)");
    }
    mysql_tquery(g_SQL, "SELECT * FROM `rent_vehicles` ORDER BY `id`", "OnRentLoaded");
    return 1;
}

static stock Rent_Spawn(slot)
{
    new vehicleid = CreateVehicle(g_Rent[slot][rnModel], g_Rent[slot][rnX], g_Rent[slot][rnY], g_Rent[slot][rnZ], g_Rent[slot][rnA], 1, 1, -1);
    g_Rent[slot][rnVehicle] = vehicleid;
    g_Rent[slot][rnRenter] = INVALID_PLAYER_ID;
    g_Rent[slot][rnAwaySince] = 0;
    g_Rent[slot][rnDiedAt] = 0;
    if (vehicleid == INVALID_VEHICLE_ID)
    {
        return;
    }
    g_RentOf[vehicleid] = slot;
    SetVehicleNumberPlate(vehicleid, "RENT");
    Rent_UpdateLabel(slot);
    Rent_Reset(vehicleid);
}

// A free rental shows its price under the label; a rented one only says what it is.
static stock Rent_UpdateLabel(slot)
{
    new text[40];
    if (g_Rent[slot][rnRenter] == INVALID_PLAYER_ID)
    {
        format(text, sizeof(text), "[RENT]\n" C_GREEN "$%d / sat", g_Rent[slot][rnPrice]);
    }
    else
    {
        strcopy(text, "[RENT]");
    }
    Veh_SetLabel(g_Rent[slot][rnVehicle], text, COLOR_WHITE);
}

// Back to the state a free rental waits in: full tank, engine off, doors open.
static stock Rent_Reset(vehicleid)
{
    Fuel_Set(vehicleid, float(Fuel_TankSize(GetVehicleModel(vehicleid))));
    SetVehicleParamsEx(vehicleid, VEHICLE_PARAMS_OFF, VEHICLE_PARAMS_OFF, VEHICLE_PARAMS_OFF, VEHICLE_PARAMS_OFF, VEHICLE_PARAMS_OFF, VEHICLE_PARAMS_OFF, VEHICLE_PARAMS_OFF);
}

forward OnRentLoaded();
public OnRentLoaded()
{
    new rows = cache_num_rows();
    for (new row = 0; row < rows && row < MAX_RENT_VEHICLES; row++)
    {
        g_Rent[row][rnUsed] = true;
        cache_get_value_name_int(row, "id", g_Rent[row][rnDbId]);
        cache_get_value_name_int(row, "model", g_Rent[row][rnModel]);
        cache_get_value_name_float(row, "x", g_Rent[row][rnX]);
        cache_get_value_name_float(row, "y", g_Rent[row][rnY]);
        cache_get_value_name_float(row, "z", g_Rent[row][rnZ]);
        cache_get_value_name_float(row, "a", g_Rent[row][rnA]);
        cache_get_value_name_int(row, "price", g_Rent[row][rnPrice]);
        Rent_Spawn(row);
    }
    printf("[RENT] %d rental vehicles loaded.", min(rows, MAX_RENT_VEHICLES));
    return 1;
}

stock bool:Rent_IsRentVehicle(vehicleid)
{
    return vehicleid > 0 && vehicleid < MAX_VEHICLES && g_RentOf[vehicleid] != -1;
}

stock bool:Rent_IsRenter(playerid, vehicleid)
{
    return Rent_IsRentVehicle(vehicleid) && g_Rent[g_RentOf[vehicleid]][rnRenter] == playerid;
}

// The account that pays a camera fine for a rental: whoever rented it, or 0 when it is free.
stock Rent_RenterDbId(vehicleid)
{
    if (!Rent_IsRentVehicle(vehicleid) || g_Rent[g_RentOf[vehicleid]][rnRenter] == INVALID_PLAYER_ID)
    {
        return 0;
    }
    return g_Rent[g_RentOf[vehicleid]][rnRenterDbId];
}

static stock Rent_FindByRenter(playerid)
{
    for (new i = 0; i < MAX_RENT_VEHICLES; i++)
    {
        if (g_Rent[i][rnUsed] && g_Rent[i][rnRenter] == playerid)
        {
            return i;
        }
    }
    return -1;
}

// The game's own respawn puts it back on its spot with everyone thrown out; OnVehicleSpawn refills it.
static stock Rent_Return(slot)
{
    g_Rent[slot][rnRenter] = INVALID_PLAYER_ID;
    g_Rent[slot][rnRenterDbId] = 0;
    g_Rent[slot][rnUntil] = 0;
    g_Rent[slot][rnWarned] = false;
    g_Rent[slot][rnAwaySince] = 0;
    g_Rent[slot][rnDiedAt] = 0;
    Rent_UpdateLabel(slot);
    SetVehicleToRespawn(g_Rent[slot][rnVehicle]);
}

stock Rent_OnVehicleSpawn(vehicleid)
{
    if (Rent_IsRentVehicle(vehicleid))
    {
        Rent_Reset(vehicleid);
    }
}

// Rentals never respawn on their own, so a destroyed one is brought back here.
stock Rent_OnVehicleDeath(vehicleid)
{
    if (Rent_IsRentVehicle(vehicleid) && g_Rent[g_RentOf[vehicleid]][rnDiedAt] == 0)
    {
        g_Rent[g_RentOf[vehicleid]][rnDiedAt] = gettime();
    }
}

stock Rent_OnPlayerConnect(playerid)
{
    g_RentDecideTimer[playerid] = 0;
}

stock Rent_OnPlayerDisconnect(playerid)
{
    if (g_RentDecideTimer[playerid] != 0)
    {
        KillTimer(g_RentDecideTimer[playerid]);
    }
    new slot = Rent_FindByRenter(playerid);
    if (slot != -1)
    {
        Rent_Return(slot);
    }
}

stock Rent_OnPlayerStateChange(playerid, PLAYER_STATE:newstate)
{
    if (newstate != PLAYER_STATE_DRIVER)
    {
        return;
    }
    new vehicleid = GetPlayerVehicleID(playerid);
    if (!Rent_IsRentVehicle(vehicleid))
    {
        return;
    }
    new slot = g_RentOf[vehicleid], renter = g_Rent[slot][rnRenter];
    if (renter == playerid)
    {
        return;
    }
    if (renter != INVALID_PLAYER_ID)
    {
        RemovePlayerFromVehicle(playerid);
        Msg_Error(playerid, "Ovo vozilo je vec iznajmljeno.");
        return;
    }
    Msg_Info(playerid, "Rent-a-car: %s, $%d po satu (do %d sata). Iznajmljivanje: /rent [sati].", Veh_ModelName(g_Rent[slot][rnModel]), g_Rent[slot][rnPrice], RENT_MAX_HOURS);
    if (g_RentDecideTimer[playerid] != 0)
    {
        KillTimer(g_RentDecideTimer[playerid]);
    }
    g_RentDecideTimer[playerid] = SetTimerEx("Rent_DecideTimeout", RENT_DECIDE_S * 1000, false, "ddd", playerid, vehicleid, g_AccountRace[playerid]);
}

forward Rent_DecideTimeout(playerid, vehicleid, race);
public Rent_DecideTimeout(playerid, vehicleid, race)
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }
    g_RentDecideTimer[playerid] = 0;
    if (GetPlayerVehicleID(playerid) == vehicleid && GetPlayerState(playerid) == PLAYER_STATE_DRIVER && !Rent_IsRenter(playerid, vehicleid))
    {
        RemovePlayerFromVehicle(playerid);
        Msg_Info(playerid, "Niste iznajmili vozilo.");
    }
    return 1;
}

// Every second: rentals that ran out go back, and free ones left away from their spot return after a while.
stock Rent_Tick()
{
    new now = gettime();
    for (new slot = 0; slot < MAX_RENT_VEHICLES; slot++)
    {
        if (!g_Rent[slot][rnUsed] || g_Rent[slot][rnVehicle] == INVALID_VEHICLE_ID)
        {
            continue;
        }
        if (g_Rent[slot][rnDiedAt] != 0)
        {
            if (now - g_Rent[slot][rnDiedAt] >= RENT_DEATH_RESPAWN_S)
            {
                if (g_Rent[slot][rnRenter] != INVALID_PLAYER_ID)
                {
                    Msg_Info(g_Rent[slot][rnRenter], "Iznajmljeno vozilo je unisteno; najam je zavrsen.");
                }
                Rent_Return(slot);
            }
            continue;
        }
        new renter = g_Rent[slot][rnRenter];
        if (renter != INVALID_PLAYER_ID)
        {
            new left = g_Rent[slot][rnUntil] - now;
            if (left <= 0)
            {
                Msg_Info(renter, "Isteklo je vreme najma; vozilo je vraceno.");
                Rent_Return(slot);
            }
            else if (left <= RENT_WARN_S && !g_Rent[slot][rnWarned])
            {
                g_Rent[slot][rnWarned] = true;
                Msg_Info(renter, "Najam vozila istice za %d minuta. Produzenje: /rent [sati] u vozilu.", left / 60 + 1);
            }
            continue;
        }
        new vehicleid = g_Rent[slot][rnVehicle];
        if (IsVehicleOccupied(vehicleid) || GetVehicleDistanceFromPoint(vehicleid, g_Rent[slot][rnX], g_Rent[slot][rnY], g_Rent[slot][rnZ]) <= RENT_SPOT_RANGE)
        {
            g_Rent[slot][rnAwaySince] = 0;
        }
        else if (g_Rent[slot][rnAwaySince] == 0)
        {
            g_Rent[slot][rnAwaySince] = now;
        }
        else if (now - g_Rent[slot][rnAwaySince] >= RENT_IDLE_RETURN_S)
        {
            Rent_Return(slot);
        }
    }
}

cmd:rent(playerid, params[])
{
    new vehicleid = GetPlayerVehicleID(playerid), hours;
    if (!Rent_IsRentVehicle(vehicleid) || GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
    {
        Msg_Error(playerid, "Sedite za volan vozila za rent (natpis [RENT]).");
        return 1;
    }
    if (sscanf(params, "d", hours) || hours < 1 || hours > RENT_MAX_HOURS)
    {
        Msg_Usage(playerid, "/rent [1-%d sata]", RENT_MAX_HOURS);
        return 1;
    }
    new slot = g_RentOf[vehicleid], owned = Rent_FindByRenter(playerid);
    if (owned != -1 && owned != slot)
    {
        Msg_Error(playerid, "Vec imate iznajmljeno vozilo. Vracanje: /vratirent");
        return 1;
    }
    // Extending adds hours, but a rental never runs longer than the maximum from now.
    new now = gettime(), start = owned == slot ? g_Rent[slot][rnUntil] : now;
    if (start + hours * 3600 > now + RENT_MAX_HOURS * 3600)
    {
        Msg_Error(playerid, "Najam moze trajati najvise %d sata unapred.", RENT_MAX_HOURS);
        return 1;
    }
    new E_LICENSE:license = Lic_ForModel(g_Rent[slot][rnModel]);
    if (license != LIC_NONE && !Lic_Has(playerid, license))
    {
        Msg_Error(playerid, "Za ovo vozilo treba dozvola: %s (auto skola, /gps).", g_LicInfo[license][liName]);
        return 1;
    }
    new price = g_Rent[slot][rnPrice] * hours, reason[40];
    format(reason, sizeof(reason), "rent: %s, %d h", Veh_ModelName(g_Rent[slot][rnModel]), hours);
    if (!Money_Take(playerid, price, reason))
    {
        return 1;
    }
    g_Rent[slot][rnRenter] = playerid;
    g_Rent[slot][rnRenterDbId] = g_Player[playerid][pDbId];
    g_Rent[slot][rnUntil] = start + hours * 3600;
    g_Rent[slot][rnWarned] = false;
    Rent_UpdateLabel(slot);
    if (g_RentDecideTimer[playerid] != 0)
    {
        KillTimer(g_RentDecideTimer[playerid]);
        g_RentDecideTimer[playerid] = 0;
    }
    new minutes = (g_Rent[slot][rnUntil] - now) / 60;
    Msg_Success(playerid, "Iznajmili ste %s za $%d (%d h, vazi jos %d min). Motor: N, vracanje: /vratirent.", Veh_ModelName(g_Rent[slot][rnModel]), price, hours, minutes);
    return 1;
}

cmd:vratirent(playerid, params[])
{
    new slot = Rent_FindByRenter(playerid);
    if (slot == -1)
    {
        Msg_Error(playerid, "Nemate iznajmljeno vozilo.");
        return 1;
    }
    if (GetPlayerVehicleID(playerid) != g_Rent[slot][rnVehicle])
    {
        Msg_Error(playerid, "Vozilo se vraca iz vozila: sedite u iznajmljeno vozilo.");
        return 1;
    }
    Rent_Return(slot);
    Msg_Success(playerid, "Vratili ste iznajmljeno vozilo.");
    return 1;
}

cmd:napravirent(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }
    new price, vehicleid = GetPlayerVehicleID(playerid);
    if (sscanf(params, "d", price) || price < 1 || price > 100000)
    {
        Msg_Usage(playerid, "/napravirent [cena po satu] (sedite u admin vozilu koje postaje rent, na mestu gde ce stajati)");
        return 1;
    }
    if (vehicleid == 0 || !g_IsAdminVehicle[vehicleid])
    {
        Msg_Error(playerid, "Sedite u admin vozilo (/veh [model]) i parkirajte ga gde ce rent stajati.");
        return 1;
    }
    new slot = -1;
    for (new i = 0; i < MAX_RENT_VEHICLES; i++)
    {
        if (!g_Rent[i][rnUsed])
        {
            slot = i;
            break;
        }
    }
    if (slot == -1)
    {
        Msg_Error(playerid, "Dostignut je najveci broj rent vozila (%d).", MAX_RENT_VEHICLES);
        return 1;
    }
    g_Rent[slot][rnUsed] = true;
    g_Rent[slot][rnModel] = GetVehicleModel(vehicleid);
    g_Rent[slot][rnPrice] = price;
    GetVehiclePos(vehicleid, g_Rent[slot][rnX], g_Rent[slot][rnY], g_Rent[slot][rnZ]);
    GetVehicleZAngle(vehicleid, g_Rent[slot][rnA]);
    Staff_DestroyAdminVehicle(vehicleid);
    Rent_Spawn(slot);

    new query[192];
    mysql_format(g_SQL, query, sizeof(query), "INSERT INTO `rent_vehicles` (`model`, `x`, `y`, `z`, `a`, `price`) VALUES (%d, %f, %f, %f, %f, %d)",
        g_Rent[slot][rnModel], g_Rent[slot][rnX], g_Rent[slot][rnY], g_Rent[slot][rnZ], g_Rent[slot][rnA], price);
    mysql_tquery(g_SQL, query, "OnRentInserted", "d", slot);

    new details[48];
    format(details, sizeof(details), "%s, $%d po satu", Veh_ModelName(g_Rent[slot][rnModel]), price);
    Staff_Log(playerid, "rent_create", INVALID_PLAYER_ID, details);
    Msg_Admin(playerid, "Napravljeno je rent vozilo: %s.", details);
    return 1;
}

forward OnRentInserted(slot);
public OnRentInserted(slot)
{
    g_Rent[slot][rnDbId] = cache_insert_id();
    return 1;
}

cmd:obrisirent(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }
    new vehicleid = GetPlayerVehicleID(playerid);
    if (vehicleid == 0)
    {
        vehicleid = Veh_FindClosest(playerid, RENT_SPOT_RANGE);
    }
    if (!Rent_IsRentVehicle(vehicleid))
    {
        Msg_Error(playerid, "Sedite u rent vozilo ili stanite pored njega.");
        return 1;
    }
    new slot = g_RentOf[vehicleid], query[64], details[48];
    if (g_Rent[slot][rnRenter] != INVALID_PLAYER_ID)
    {
        Msg_Info(g_Rent[slot][rnRenter], "Admin je uklonio vase iznajmljeno vozilo.");
    }
    mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `rent_vehicles` WHERE `id` = %d", g_Rent[slot][rnDbId]);
    mysql_tquery(g_SQL, query);
    format(details, sizeof(details), "%s, $%d po satu", Veh_ModelName(g_Rent[slot][rnModel]), g_Rent[slot][rnPrice]);
    Staff_Log(playerid, "rent_delete", INVALID_PLAYER_ID, details);
    Veh_ClearLabel(vehicleid);
    DestroyVehicle(vehicleid);
    g_RentOf[vehicleid] = -1;
    g_Rent[slot][rnUsed] = false;
    Msg_Admin(playerid, "Obrisano je rent vozilo: %s.", details);
    return 1;
}
