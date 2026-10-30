stock bool:Veh_IsRegistered(slot)
{
    return g_Veh[slot][vRegisteredUntil] > gettime();
}

static stock Veh_RegistrationDaysLeft(slot)
{
    return (g_Veh[slot][vRegisteredUntil] - gettime()) / 86400;
}

stock Veh_RegistrationText(slot, dest[], size = sizeof(dest))
{
    if (Veh_IsBicycle(g_Veh[slot][vModel]))
    {
        strcopy(dest, "-", size);
    }
    else if (g_Veh[slot][vRegisteredUntil] == 0)
    {
        strcopy(dest, "nije registrovano", size);
    }
    else if (!Veh_IsRegistered(slot))
    {
        strcopy(dest, "istekla", size);
    }
    else
    {
        format(dest, size, "jos %d dana", Veh_RegistrationDaysLeft(slot));
    }
}

static stock Veh_RegistrationPrice(model)
{
    switch (Fuel_TankSize(model))
    {
        case 15: return REG_PRICE_MOTORBIKE;
        case 100: return REG_PRICE_LARGE;
    }
    return REG_PRICE_CAR;
}

// Once per login, so an owner learns about an unregistered or expiring vehicle without having to look.
stock Veh_RemindRegistration(playerid)
{
    new dbid = g_Player[playerid][pDbId];
    for (new slot = 0; slot < MAX_PERSONAL_VEHICLES; slot++)
    {
        if (!g_Veh[slot][vUsed] || g_Veh[slot][vOwnerDbId] != dbid || Veh_IsBicycle(g_Veh[slot][vModel]))
        {
            continue;
        }
        if (g_Veh[slot][vRegisteredUntil] == 0)
        {
            Msg_Info(playerid, "Vozilo %s nije registrovano. Tablice dobijate na tehnickom pregledu (/registruj, mesto na /gps).", Veh_Name(slot));
        }
        else if (!Veh_IsRegistered(slot))
        {
            Msg_Info(playerid, "Registracija vozila %s je istekla. Produzava se na tehnickom pregledu (/registruj).", Veh_Name(slot));
        }
        else if (Veh_RegistrationDaysLeft(slot) < VEH_REGISTRATION_WARN_DAYS)
        {
            Msg_Info(playerid, "Registracija vozila %s istice za %d dana (/registruj na tehnickom pregledu).", Veh_Name(slot), Veh_RegistrationDaysLeft(slot));
        }
    }
}

// Checked when starting and again when the inspection ends, since the player may drive off or switch the engine on meanwhile.
static stock Veh_CheckInspection(playerid)
{
    if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
    {
        Msg_Error(playerid, "Na tehnicki pregled se dolazi vozilom: sedite za volan svog vozila.");
        return -1;
    }
    new vehicleid = GetPlayerVehicleID(playerid), slot = g_VehSlotOf[vehicleid];
    if (slot == -1 || !Veh_IsOwner(playerid, vehicleid))
    {
        Msg_Error(playerid, "Registruje se samo vase licno vozilo.");
        return -1;
    }
    if (Veh_IsBicycle(g_Veh[slot][vModel]))
    {
        Msg_Error(playerid, "Bicikli se ne registruju.");
        return -1;
    }
    if (!Point_Require(playerid, POINT_INSPECTION))
    {
        return -1;
    }
    if (Veh_IsEngineOn(vehicleid))
    {
        Msg_Error(playerid, "Ugasite motor za tehnicki pregled (taster N).");
        return -1;
    }
    new Float:health;
    GetVehicleHealth(vehicleid, health);
    if (health < VEH_INSPECTION_MIN_HEALTH)
    {
        Msg_Error(playerid, "Vozilo je previse osteceno i ne prolazi tehnicki pregled. Prvo ga popravite.");
        return -1;
    }
    return slot;
}

cmd:registruj(playerid, params[])
{
    if (g_VehInspectTimer[playerid] != 0)
    {
        Msg_Error(playerid, "Tehnicki pregled je u toku.");
        return 1;
    }
    new slot = Veh_CheckInspection(playerid);
    if (slot == -1)
    {
        return 1;
    }
    if (Veh_IsRegistered(slot) && Veh_RegistrationDaysLeft(slot) >= VEH_REGISTRATION_RENEW_DAYS)
    {
        Msg_Info(playerid, "Registracija vazi jos %d dana. Produzava se kad ostane manje od %d dana.", Veh_RegistrationDaysLeft(slot), VEH_REGISTRATION_RENEW_DAYS);
        return 1;
    }
    new price = Veh_RegistrationPrice(g_Veh[slot][vModel]);
    if (g_Player[playerid][pCash] < price)
    {
        Msg_Error(playerid, "Tehnicki pregled i registracija kostaju $%d (gotovinom).", price);
        return 1;
    }

    Chat_Action(playerid, "predaje vozilo na tehnicki pregled.");
    GameTextForPlayer(playerid, "~w~tehnicki pregled...", VEH_INSPECTION_MS, 6);
    g_VehInspectTimer[playerid] = SetTimerEx("Veh_FinishInspection", VEH_INSPECTION_MS, false, "ddd", playerid, g_Veh[slot][vDbId], g_AccountRace[playerid]);
    return 1;
}
alias:registruj("registracija", "tehnicki")

forward Veh_FinishInspection(playerid, dbid, race);
public Veh_FinishInspection(playerid, dbid, race)
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }
    g_VehInspectTimer[playerid] = 0;
    new slot = Veh_CheckInspection(playerid);
    if (slot == -1 || g_Veh[slot][vDbId] != dbid)
    {
        Msg_Error(playerid, "Tehnicki pregled je prekinut.");
        return 1;
    }

    new price = Veh_RegistrationPrice(g_Veh[slot][vModel]), reason[48];
    format(reason, sizeof(reason), "registracija vozila %d", dbid);
    if (!Money_Take(playerid, price, reason))
    {
        return 1;
    }
    if (g_Veh[slot][vPlate][0])
    {
        Veh_CompleteRegistration(playerid, slot, "");
        return 1;
    }
    new plate[VEH_PLATE_LENGTH];
    Veh_RandomPlate(plate);
    Veh_CheckRegistrationPlate(playerid, race, dbid, plate);
    return 1;
}

// Owners of other plates can be offline, so a new plate is checked against the database.
static stock Veh_CheckRegistrationPlate(playerid, race, dbid, const plate[])
{
    new query[96];
    mysql_format(g_SQL, query, sizeof(query), "SELECT `id` FROM `vehicles` WHERE `plate` = '%e'", plate);
    mysql_tquery(g_SQL, query, "OnRegistrationPlateChecked", "ddds", playerid, race, dbid, plate);
}

forward OnRegistrationPlateChecked(playerid, race, dbid, const plate[]);
public OnRegistrationPlateChecked(playerid, race, dbid, const plate[])
{
    if (cache_num_rows() > 0)
    {
        new other[VEH_PLATE_LENGTH];
        Veh_RandomPlate(other);
        Veh_CheckRegistrationPlate(playerid, race, dbid, other);
        return 1;
    }
    new slot = Veh_FindByDbId(dbid);
    if (slot == -1)
    {
        return 1;
    }
    Veh_CompleteRegistration(race == g_AccountRace[playerid] ? playerid : INVALID_PLAYER_ID, slot, plate);
    return 1;
}

// plate is empty for a renewal; a first registration also gets its plates mounted.
static stock Veh_CompleteRegistration(playerid, slot, const plate[])
{
    new now = gettime();
    g_Veh[slot][vRegisteredUntil] = (g_Veh[slot][vRegisteredUntil] > now ? g_Veh[slot][vRegisteredUntil] : now) + VEH_REGISTRATION_DAYS * 86400;
    if (plate[0])
    {
        strcopy(g_Veh[slot][vPlate], plate, VEH_PLATE_LENGTH);
        new query[96];
        mysql_format(g_SQL, query, sizeof(query), "UPDATE `vehicles` SET `plate` = '%e' WHERE `id` = %d", plate, g_Veh[slot][vDbId]);
        mysql_tquery(g_SQL, query);
        Veh_Rebuild(slot);
    }
    Veh_SaveState(slot);
    if (playerid != INVALID_PLAYER_ID)
    {
        Msg_Success(playerid, "Vozilo %s je proslo tehnicki pregled i registrovano je na %d dana.", Veh_Name(slot), VEH_REGISTRATION_DAYS);
    }
}
