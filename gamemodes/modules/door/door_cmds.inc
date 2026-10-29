cmd:udji(playerid, params[])
{
    new slot = Door_FindOutside(playerid);
    if (slot == -1)
    {
        Msg_Error(playerid, "Niste ispred ulaza.");
        return 1;
    }
    Door_Enter(playerid, slot);
    return 1;
}

cmd:izadji(playerid, params[])
{
    new slot = Door_FindInside(playerid);
    if (slot == -1)
    {
        Msg_Error(playerid, "Niste kod izlaza.");
        return 1;
    }
    Door_Exit(playerid, slot);
    return 1;
}

// Parses "[door ID]" at the start of params; prints the usage or error itself and returns -1 when the command should stop.
stock Door_ParseSlot(playerid, const params[], const usage[])
{
    new dbid;
    if (sscanf(params, "d", dbid))
    {
        Msg_Usage(playerid, "%s", usage);
        return -1;
    }
    new slot = Door_FindSlotByDbId(dbid);
    if (slot == -1)
    {
        Msg_Error(playerid, "Ulaz sa tim ID-jem ne postoji. Spisak: /ulazi");
    }
    return slot;
}

cmd:napraviulaz(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }
    if (isnull(params) || strlen(params) >= DOOR_NAME_LENGTH)
    {
        Msg_Usage(playerid, "/napraviulaz [ime] (stanite na vrata spolja, licem prema zgradi)");
        return 1;
    }

    new slot = Door_FindFreeSlot();
    if (slot == -1)
    {
        Msg_Error(playerid, "Dostignut je najveci broj ulaza (%d).", MAX_DOORS);
        return 1;
    }

    g_Doors[slot][dUsed] = true;
    g_Doors[slot][dDbId] = Door_FreeDbId();
    g_Doors[slot][dHasExit] = false;
    g_Doors[slot][dLocked] = false;
    g_Doors[slot][dIcon] = 0;
    g_Doors[slot][dModel] = DOOR_PICKUP_MODEL;
    strcopy(g_Doors[slot][dName], params, DOOR_NAME_LENGTH);
    Door_SetOutside(slot, playerid);

    new query[320], details[64];
    mysql_format(g_SQL, query, sizeof(query),
        "INSERT INTO `doors` (`id`, `name`, `out_x`, `out_y`, `out_z`, `out_a`, `out_interior`, `out_world`) VALUES (%d, '%e', %f, %f, %f, %f, %d, %d)",
        g_Doors[slot][dDbId], g_Doors[slot][dName], g_Doors[slot][dOutX], g_Doors[slot][dOutY], g_Doors[slot][dOutZ], g_Doors[slot][dOutA],
        g_Doors[slot][dOutInterior], g_Doors[slot][dOutWorld]);
    mysql_tquery(g_SQL, query);
    Door_Refresh(slot);

    format(details, sizeof(details), "ulaz %d: %s", g_Doors[slot][dDbId], g_Doors[slot][dName]);
    Staff_Log(playerid, "door_create", INVALID_PLAYER_ID, details);
    Msg_Admin(playerid, "Napravljen je ulaz %d (%s). Udjite u enterijer i postavite izlaz: /izmeniulaz %d izlaz", g_Doors[slot][dDbId], g_Doors[slot][dName], g_Doors[slot][dDbId]);
    return 1;
}

cmd:izmeniulaz(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }

    new dbid, option[16], value[DOOR_NAME_LENGTH];
    if (sscanf(params, "ds[16]S()[48]", dbid, option, value))
    {
        Msg_Usage(playerid, "/izmeniulaz [id] [ime / ulaz / izlaz / ikona / strelica / zakljucaj / svet] [vrednost]");
        Msg_Info(playerid, "ulaz i izlaz se postavljaju na mesto gde stojite; ikona 0-63 (0 bez ikone); svet deli enterijer sa drugim ulazom.");
        return 1;
    }
    new slot = Door_FindSlotByDbId(dbid);
    if (slot == -1)
    {
        Msg_Error(playerid, "Ulaz sa tim ID-jem ne postoji. Spisak: /ulazi");
        return 1;
    }

    new number = strval(value), details[96];
    if (!strcmp(option, "ime", true))
    {
        if (isnull(value))
        {
            Msg_Usage(playerid, "/izmeniulaz [id] ime [novo ime]");
            return 1;
        }
        strcopy(g_Doors[slot][dName], value, DOOR_NAME_LENGTH);
        Msg_Admin(playerid, "Ulaz %d se sada zove: %s", dbid, value);
    }
    else if (!strcmp(option, "ulaz", true))
    {
        Door_SetOutside(slot, playerid);
        Msg_Admin(playerid, "Spoljna strana ulaza %d je pomerena na vase mesto.", dbid);
    }
    else if (!strcmp(option, "izlaz", true))
    {
        if (GetPlayerInterior(playerid) == 0)
        {
            Msg_Error(playerid, "Izlaz se postavlja unutar enterijera (vas enterijer je 0).");
            return 1;
        }
        Door_SetInside(slot, playerid);
        Msg_Admin(playerid, "Izlaz ulaza %d je postavljen ovde (enterijer %d, svet %d).", dbid, g_Doors[slot][dInInterior], g_Doors[slot][dInWorld]);
    }
    else if (!strcmp(option, "ikona", true))
    {
        if (number < 0 || number > 63)
        {
            Msg_Usage(playerid, "/izmeniulaz [id] ikona [0-63] (0 uklanja ikonu sa mape)");
            return 1;
        }
        g_Doors[slot][dIcon] = number;
        Msg_Admin(playerid, "Ikona ulaza %d na mapi: %d.", dbid, number);
    }
    else if (!strcmp(option, "strelica", true))
    {
        // Saved when a picture is clicked (Door_OnArrowSelected).
        g_DoorArrowEdit[playerid] = dbid;
        Preview_Show(playerid, PREVIEW_DOOR_ARROW, PREVIEW_KIND_OBJECT, "Strelica za ulaz i izlaz", g_DoorArrowModels, sizeof(g_DoorArrowModels));
        return 1;
    }
    else if (!strcmp(option, "zakljucaj", true))
    {
        g_Doors[slot][dLocked] = !g_Doors[slot][dLocked];
        Msg_Admin(playerid, "Ulaz %d je %s.", dbid, g_Doors[slot][dLocked] ? ("zakljucan") : ("otkljucan"));
    }
    else if (!strcmp(option, "svet", true))
    {
        if (!g_Doors[slot][dHasExit] || number < 0)
        {
            Msg_Error(playerid, "Prvo postavite izlaz, pa onda svet (broj 0 ili veci).");
            return 1;
        }
        g_Doors[slot][dInWorld] = number;
        Msg_Admin(playerid, "Unutrasnjost ulaza %d je sada u svetu %d.", dbid, number);
    }
    else
    {
        Msg_Error(playerid, "Nepoznata opcija. Opcije: ime, ulaz, izlaz, ikona, strelica, zakljucaj, svet.");
        return 1;
    }

    Door_Save(slot);
    Door_Refresh(slot);
    format(details, sizeof(details), "ulaz %d: %s %s", dbid, option, value);
    Staff_Log(playerid, "door_edit", INVALID_PLAYER_ID, details);
    return 1;
}

cmd:obrisiulaz(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }
    new slot = Door_ParseSlot(playerid, params, "/obrisiulaz [id]");
    if (slot == -1)
    {
        return 1;
    }

    new details[64];
    format(details, sizeof(details), "ulaz %d: %s", g_Doors[slot][dDbId], g_Doors[slot][dName]);
    Staff_Log(playerid, "door_delete", INVALID_PLAYER_ID, details);
    Msg_Admin(playerid, "Obrisan je %s.", details);
    new points = Door_Destroy(slot);
    if (points > 0)
    {
        Msg_Admin(playerid, "Obrisano je i %d mesta (salter, bankomat...) koja su bila unutra.", points);
    }
    return 1;
}

cmd:ulazi(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }

    new body[4096], line[96], count;
    for (new i = 0; i < MAX_DOORS; i++)
    {
        if (!g_Doors[i][dUsed])
        {
            continue;
        }
        format(line, sizeof(line), "%d\t%s\t%s\n", g_Doors[i][dDbId], g_Doors[i][dName],
            !g_Doors[i][dHasExit] ? (C_RED "bez izlaza") : (g_Doors[i][dLocked] ? (C_GREY "zakljucan") : (C_GREEN "otvoren")));
        if (strlen(body) + strlen(line) >= sizeof(body))
        {
            break;
        }
        strcat(body, line);
        count++;
    }

    if (count == 0)
    {
        Msg_Info(playerid, "Jos nema ulaza. Napravite ga sa /napraviulaz [ime].");
        return 1;
    }
    ShowPlayerDialog(playerid, DIALOG_ADMIN_INFO, DIALOG_STYLE_TABLIST, SERVER_NAME " - Ulazi", body, "Zatvori", "");
    return 1;
}

cmd:gotoulaz(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_ADMIN))
    {
        return 1;
    }
    new slot = Door_ParseSlot(playerid, params, "/gotoulaz [id]");
    if (slot == -1)
    {
        return 1;
    }

    Staff_TeleportTo(playerid, g_Doors[slot][dOutX], g_Doors[slot][dOutY], g_Doors[slot][dOutZ], g_Doors[slot][dOutInterior], g_Doors[slot][dOutWorld]);
    Msg_Admin(playerid, "Teleportovani ste do ulaza %d (%s).", g_Doors[slot][dDbId], g_Doors[slot][dName]);
    return 1;
}

stock Door_OnArrowSelected(playerid, index)
{
    // Rank and door are checked again: the menu may be answered after a demotion or after the door was deleted.
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD) || index < 0 || index >= sizeof(g_DoorArrowModels))
    {
        return;
    }
    new slot = Door_FindSlotByDbId(g_DoorArrowEdit[playerid]);
    if (slot == -1)
    {
        Msg_Error(playerid, "Taj ulaz vise ne postoji.");
        return;
    }

    g_Doors[slot][dModel] = g_DoorArrowModels[index];
    Door_Save(slot);
    Door_Refresh(slot);
    Msg_Admin(playerid, "Ulaz %d sada ima strelicu (model %d) na ulazu i izlazu.", g_Doors[slot][dDbId], g_Doors[slot][dModel]);

    new details[64];
    format(details, sizeof(details), "ulaz %d: strelica %d", g_Doors[slot][dDbId], g_Doors[slot][dModel]);
    Staff_Log(playerid, "door_edit", INVALID_PLAYER_ID, details);
}
