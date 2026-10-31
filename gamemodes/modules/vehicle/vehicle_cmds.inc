cmd:v(playerid, params[])
{
    new action[16], rest[32];
    if (sscanf(params, "s[16]S()[32]", action, rest))
    {
        Veh_ShowList(playerid);
        return 1;
    }

    if (!strcmp(action, "motor", true))
    {
        Veh_ToggleEngine(playerid);
    }
    else if (!strcmp(action, "zakljucaj", true))
    {
        Veh_ToggleLock(playerid);
    }
    else if (!strcmp(action, "svetla", true))
    {
        Veh_ToggleLights(playerid);
    }
    else if (!strcmp(action, "hauba", true))
    {
        Veh_TogglePart(playerid, VEH_PARAM_BONNET, "otvara haubu.", "zatvara haubu.");
    }
    else if (!strcmp(action, "gepek", true))
    {
        Veh_TogglePart(playerid, VEH_PARAM_BOOT, "otvara gepek.", "zatvara gepek.");
    }
    else if (!strcmp(action, "parkiraj", true))
    {
        Veh_Park(playerid);
    }
    else if (!strcmp(action, "kljuc", true))
    {
        Veh_GiveKey(playerid, rest);
    }
    else if (!strcmp(action, "tablice", true))
    {
        Plate_MountPending(playerid);
    }
    else
    {
        Msg_Usage(playerid, "/v (lista, GPS i kljucevi) ili /v [motor / zakljucaj / svetla / hauba / gepek / parkiraj / kljuc / tablice]");
    }
    return 1;
}
alias:v("vozilo")

cmd:dajvozilo(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }

    new targetid, model;
    if (sscanf(params, "ud", targetid, model))
    {
        Msg_Usage(playerid, "/dajvozilo [id ili ime] [model 400-611] (vozilo se pojavi pored igraca)");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }
    if (!Veh_IsOwnableModel(model))
    {
        Msg_Error(playerid, "Taj model ne moze biti licno vozilo (prikolice, vozovi i RC igracke ne mogu).");
        return 1;
    }
    if (GetPlayerInterior(targetid) != 0 || GetPlayerVirtualWorld(targetid) != 0)
    {
        Msg_Error(playerid, "Igrac mora biti napolju, jer se vozilo pojavljuje pored njega.");
        return 1;
    }
    if (Veh_CountOwned(g_Player[targetid][pDbId]) >= VEH_MAX_PER_PLAYER)
    {
        Msg_Error(playerid, "Igrac vec ima najvise vozila (%d).", VEH_MAX_PER_PLAYER);
        return 1;
    }

    Veh_CreateFor(playerid, targetid, model);
    return 1;
}

cmd:obrisivozilo(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }

    new vehicleid = GetPlayerVehicleID(playerid), slot = g_VehSlotOf[vehicleid];
    if (vehicleid == 0 || slot == -1)
    {
        Msg_Usage(playerid, "/obrisivozilo (dok sedite u licnom vozilu; brise ga zauvek)");
        return 1;
    }

    new query[64], details[64];
    mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `vehicles` WHERE `id` = %d", g_Veh[slot][vDbId]);
    mysql_tquery(g_SQL, query);

    format(details, sizeof(details), "vozilo %d: %s", g_Veh[slot][vDbId], Veh_Name(slot));
    Staff_Log(playerid, "deleteveh", INVALID_PLAYER_ID, details);
    Msg_Admin(playerid, "Obrisano je %s.", details);

    new ownerid = g_Veh[slot][vOwnerId];
    if (ownerid != INVALID_PLAYER_ID && ownerid != playerid)
    {
        Msg_Admin(ownerid, "Admin %s je obrisao vase vozilo %s.", Account_Name(playerid), Veh_Name(slot));
    }
    Veh_Despawn(slot, false);
    return 1;
}

cmd:resetvozila(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_SENIOR))
    {
        return 1;
    }

    new seconds;
    if (sscanf(params, "D(15)", seconds) || seconds < 5 || seconds > 60)
    {
        Msg_Usage(playerid, "/resetvozila [sekunde 5-60] (bez broja 15 s; vraca prazna vozila na parking)");
        return 1;
    }
    if (!Veh_StartReset(playerid, seconds))
    {
        Msg_Error(playerid, "Reset vozila je vec pokrenut.");
        return 1;
    }
    new message[160];
    format(message, sizeof(message), "[ADMIN] %s vraca sva prazna vozila na parking za %d sekundi. Sedite u svoje vozilo ako zelite da ostane gde je.", Account_Name(playerid), seconds);
    Chat_SendToAll(COLOR_ADMIN, message);
    return 1;
}

// The vehicle in the admin's own seat, or the nearest one within range, for commands that work on any vehicle.
static stock Veh_FindNearestAny(playerid, Float:range)
{
    new vehicleid = GetPlayerVehicleID(playerid);
    if (vehicleid != 0)
    {
        return vehicleid;
    }
    new best = 0, Float:best_distance = range, world = GetPlayerVirtualWorld(playerid), Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    for (new v = 1; v < MAX_VEHICLES; v++)
    {
        if (!IsValidVehicle(v) || GetVehicleVirtualWorld(v) != world)
        {
            continue;
        }
        new Float:distance = GetVehicleDistanceFromPoint(v, x, y, z);
        if (distance < best_distance)
        {
            best_distance = distance;
            best = v;
        }
    }
    return best;
}

// Returns the vehicle ID typed after the command, or 0 with the usage or error already shown.
static stock Veh_ParseAdminTarget(playerid, const params[], const usage[])
{
    new vehicleid;
    if (sscanf(params, "d", vehicleid))
    {
        Msg_Usage(playerid, "%s", usage);
        return 0;
    }
    if (vehicleid < 1 || vehicleid >= MAX_VEHICLES || !IsValidVehicle(vehicleid))
    {
        Msg_Error(playerid, "Vozilo sa tim ID-jem ne postoji (ID vozila se vidi sa /vinfo).");
        return 0;
    }
    return vehicleid;
}

static stock Veh_LogAdmin(playerid, vehicleid, const action[], const extra[] = "")
{
    new slot = g_VehSlotOf[vehicleid], details[80], ownerid = INVALID_PLAYER_ID;
    if (slot != -1)
    {
        ownerid = Account_FindOnlineByDbId(g_Veh[slot][vOwnerDbId]);
        format(details, sizeof(details), "vozilo %d (%s) %s", vehicleid, Veh_Name(slot), extra);
    }
    else
    {
        format(details, sizeof(details), "vozilo %d %s", vehicleid, extra);
    }
    Staff_Log(playerid, action, ownerid, details);
}

cmd:vinfo(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }
    new vehicleid = isnull(params) ? Veh_FindNearestAny(playerid, VEH_KEY_RANGE) : Veh_ParseAdminTarget(playerid, params, "/vinfo [id vozila] (bez ID-ja: vozilo u kome sedite ili najblize)");
    if (vehicleid == 0)
    {
        if (isnull(params))
        {
            Msg_Error(playerid, "U blizini (5 m) nema vozila.");
        }
        return 1;
    }

    new Float:health, model = GetVehicleModel(vehicleid), slot = g_VehSlotOf[vehicleid];
    GetVehicleHealth(vehicleid, health);
    Msg_Admin(playerid, "Vozilo ID %d: %s, HP %.0f, gorivo %.0f l, motor %s.", vehicleid, Veh_ModelName(model), health, g_VehFuel[vehicleid], Veh_IsEngineOn(vehicleid) ? ("radi") : ("ugasen"));
    if (slot != -1)
    {
        new registration[24], key[MAX_PLAYER_NAME + 1] = "nema";
        Veh_RegistrationText(slot, registration);
        if (g_Veh[slot][vKeyDbId] != 0)
        {
            strcopy(key, g_Veh[slot][vKeyName]);
        }
        Msg_Admin(playerid, "Licno vozilo %d u bazi: vlasnik %s, %s, registracija: %s, kopija kljuca: %s.", g_Veh[slot][vDbId], g_Veh[slot][vOwnerName], Veh_Name(slot), registration, key);
    }
    else if (g_IsAdminVehicle[vehicleid])
    {
        Msg_Admin(playerid, "Admin vozilo (/veh).");
    }
    return 1;
}

cmd:gotoveh(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }
    new vehicleid = Veh_ParseAdminTarget(playerid, params, "/gotoveh [id vozila]");
    if (vehicleid == 0)
    {
        return 1;
    }
    new Float:x, Float:y, Float:z;
    GetVehiclePos(vehicleid, x, y, z);
    Staff_TeleportTo(playerid, x + 2.0, y, z + 1.0, GetVehicleInterior(vehicleid), GetVehicleVirtualWorld(vehicleid));
    Msg_Admin(playerid, "Teleportovani ste do vozila %d.", vehicleid);
    return 1;
}

cmd:getveh(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_ADMIN))
    {
        return 1;
    }
    new vehicleid = Veh_ParseAdminTarget(playerid, params, "/getveh [id vozila] (dovodi prazno vozilo do vas)");
    if (vehicleid == 0)
    {
        return 1;
    }
    if (IsVehicleOccupied(vehicleid))
    {
        Msg_Error(playerid, "U vozilu neko sedi.");
        return 1;
    }
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    SetVehicleVelocity(vehicleid, 0.0, 0.0, 0.0);
    SetVehiclePos(vehicleid, x + 3.0, y, z);
    LinkVehicleToInterior(vehicleid, GetPlayerInterior(playerid));
    SetVehicleVirtualWorld(vehicleid, GetPlayerVirtualWorld(playerid));
    Veh_LogAdmin(playerid, vehicleid, "getveh");
    Msg_Admin(playerid, "Vozilo %d je dovedeno do vas.", vehicleid);
    return 1;
}

cmd:respawnveh(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_ADMIN))
    {
        return 1;
    }
    new vehicleid = Veh_ParseAdminTarget(playerid, params, "/respawnveh [id vozila] (licno ide na parking, admin vozilo se brise)");
    if (vehicleid == 0)
    {
        return 1;
    }
    if (IsVehicleOccupied(vehicleid))
    {
        Msg_Error(playerid, "U vozilu neko sedi.");
        return 1;
    }
    Veh_LogAdmin(playerid, vehicleid, "respawnveh");
    new slot = g_VehSlotOf[vehicleid];
    if (slot != -1)
    {
        if (!Veh_ReturnToParking(slot))
        {
            Msg_Error(playerid, "Vozilo se sada ne moze vratiti (unisteno je i vraca se samo).");
            return 1;
        }
        Msg_Admin(playerid, "Vozilo %d je vraceno na parking (bez popravke).", vehicleid);
    }
    else if (g_IsAdminVehicle[vehicleid])
    {
        DestroyVehicle(vehicleid);
        g_IsAdminVehicle[vehicleid] = false;
        Msg_Admin(playerid, "Admin vozilo %d je obrisano.", vehicleid);
    }
    else
    {
        SetVehicleToRespawn(vehicleid);
        Msg_Admin(playerid, "Vozilo %d je vraceno na svoje mesto.", vehicleid);
    }
    return 1;
}

cmd:setgorivo(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_SENIOR))
    {
        return 1;
    }
    new vehicleid, litres;
    if (sscanf(params, "dd", vehicleid, litres))
    {
        Msg_Usage(playerid, "/setgorivo [id vozila] [litara]");
        return 1;
    }
    if (vehicleid < 1 || vehicleid >= MAX_VEHICLES || !IsValidVehicle(vehicleid) || !Fuel_IsUsed(vehicleid))
    {
        Msg_Error(playerid, "To vozilo ne postoji ili ne koristi gorivo.");
        return 1;
    }
    new tank = Fuel_TankSize(GetVehicleModel(vehicleid));
    litres = clamp(litres, 0, tank);
    Fuel_Set(vehicleid, float(litres));
    Veh_SaveIfPersonal(vehicleid);
    new extra[16];
    format(extra, sizeof(extra), "%d l", litres);
    Veh_LogAdmin(playerid, vehicleid, "setfuel", extra);
    Msg_Admin(playerid, "Vozilo %d sada ima %d/%d l goriva.", vehicleid, litres, tank);
    return 1;
}
