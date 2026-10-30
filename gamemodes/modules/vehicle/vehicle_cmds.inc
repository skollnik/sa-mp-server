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
    else
    {
        Msg_Usage(playerid, "/v (lista, GPS i kljucevi) ili /v [motor / zakljucaj / svetla / hauba / gepek / parkiraj / kljuc]");
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

    new count = Veh_ReturnAllToParking(), message[144], details[16];
    format(message, sizeof(message), "[ADMIN] %s je vratio sva prazna licna vozila na njihove parkinge.", Account_Name(playerid));
    Chat_SendToAll(COLOR_ADMIN, message);
    Msg_Admin(playerid, "Vraceno vozila: %d (vozila u kojima neko sedi nisu pomerena).", count);
    format(details, sizeof(details), "%d vozila", count);
    Staff_Log(playerid, "resetveh", INVALID_PLAYER_ID, details);
    return 1;
}
