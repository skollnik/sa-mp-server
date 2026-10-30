#define TUNE_COMPONENT_FIRST    (1000)
#define TUNE_COMPONENT_LAST     (1193)
#define TUNE_MAX_PARTS          (24)
// Menu entries after the part slots.
#define TUNE_MENU_COLOUR        (100)
#define TUNE_MENU_PAINTJOB      (101)
#define TUNE_MENU_CLEAR         (102)

static const g_TuneSlotNames[VEH_TUNE_SLOTS][] =
{
    "Spojler", "Hauba", "Krov", "Bocni pragovi", "Svetla", "Nitro", "Auspuh", "Felne",
    "Stereo (bas)", "Hidraulika", "Prednji branik", "Zadnji branik", "Desni otvor", "Levi otvor",
    "Prednja zastita", "Zadnja zastita"
};

static const g_TuneWheelIds[] = { 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1096, 1097, 1098 };
static const g_TuneWheelNames[][] =
{
    "Offroad", "Shadow", "Mega", "Rimshine", "Wires", "Classic", "Twist", "Cutter", "Switch",
    "Grove", "Import", "Dollar", "Trance", "Atomic", "Ahab", "Virtual", "Access"
};

new g_TuneMenu[MAX_PLAYERS][VEH_TUNE_SLOTS + 3];
new g_TuneParts[MAX_PLAYERS][TUNE_MAX_PARTS];
new g_TunePartCount[MAX_PLAYERS];
new g_TuneSlot[MAX_PLAYERS];

// Admin vehicles are for any admin; a personal vehicle is someone's property, so free tuning on it is for senior admins.
// Returns the vehicle or 0, with the reason already told.
static stock Tune_GetVehicle(playerid)
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 0;
    }
    if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
    {
        Msg_Error(playerid, "Sedite za volan vozila koje tunirate.");
        return 0;
    }
    new vehicleid = GetPlayerVehicleID(playerid);
    if (g_IsAdminVehicle[vehicleid])
    {
        return vehicleid;
    }
    if (g_VehSlotOf[vehicleid] == -1)
    {
        Msg_Error(playerid, "Tunirati se mogu admin vozila (/veh) i licna vozila.");
        return 0;
    }
    if (g_Player[playerid][pAdminLevel] < ADMIN_LEVEL_SENIOR)
    {
        Msg_Error(playerid, "Licna vozila igraca tunira visi admin (nivo 3+); vi mozete admin vozila (/veh).");
        return 0;
    }
    return vehicleid;
}

static stock Tune_PartName(component, number, dest[], size = sizeof(dest))
{
    for (new i = 0; i < sizeof(g_TuneWheelIds); i++)
    {
        if (g_TuneWheelIds[i] == component)
        {
            strcopy(dest, g_TuneWheelNames[i], size);
            return;
        }
    }
    switch (component)
    {
        case 1008: strcopy(dest, "Nitro x2", size);
        case 1009: strcopy(dest, "Nitro x5", size);
        case 1010: strcopy(dest, "Nitro x10", size);
        default: format(dest, size, "Varijanta %d", number);
    }
}

static stock Tune_ShowMenu(playerid, vehicleid)
{
    new body[512], count, model = GetVehicleModel(vehicleid);
    for (new slot = 0; slot < VEH_TUNE_SLOTS; slot++)
    {
        for (new c = TUNE_COMPONENT_FIRST; c <= TUNE_COMPONENT_LAST; c++)
        {
            if (_:GetVehicleComponentType(c) == slot && VehicleCanHaveComponent(model, c))
            {
                strcat(body, g_TuneSlotNames[slot]);
                strcat(body, "\n");
                g_TuneMenu[playerid][count++] = slot;
                break;
            }
        }
    }
    strcat(body, "Boja\nPaintjob\nSkini sav tuning");
    g_TuneMenu[playerid][count++] = TUNE_MENU_COLOUR;
    g_TuneMenu[playerid][count++] = TUNE_MENU_PAINTJOB;
    g_TuneMenu[playerid][count] = TUNE_MENU_CLEAR;
    ShowPlayerDialog(playerid, DIALOG_TUNE_MENU, DIALOG_STYLE_LIST, "Tuning vozila", body, "Izaberi", "Zatvori");
}

static stock Tune_ShowParts(playerid, vehicleid, slot)
{
    new body[640], name[24], line[48], count, model = GetVehicleModel(vehicleid);
    strcopy(body, "Bez dela (skini)\n");
    g_TuneParts[playerid][count++] = 0;
    for (new c = TUNE_COMPONENT_FIRST; c <= TUNE_COMPONENT_LAST && count < TUNE_MAX_PARTS; c++)
    {
        if (_:GetVehicleComponentType(c) == slot && VehicleCanHaveComponent(model, c))
        {
            Tune_PartName(c, count, name);
            format(line, sizeof(line), "%s%s\n", name, GetVehicleComponentInSlot(vehicleid, CARMODTYPE:slot) == c ? (" (stavljeno)") : (""));
            strcat(body, line);
            g_TuneParts[playerid][count++] = c;
        }
    }
    g_TunePartCount[playerid] = count;
    g_TuneSlot[playerid] = slot;
    ShowPlayerDialog(playerid, DIALOG_TUNE_PARTS, DIALOG_STYLE_LIST, g_TuneSlotNames[slot], body, "Stavi", "Nazad");
}

// A personal vehicle keeps its tuning, and the change is logged since it is free value for the owner.
static stock Tune_Saved(playerid, vehicleid, const what[])
{
    new slot = g_VehSlotOf[vehicleid];
    if (slot == -1)
    {
        return;
    }
    new details[64], ownerid = Account_FindOnlineByDbId(g_Veh[slot][vOwnerDbId]);
    format(details, sizeof(details), "vozilo %d: %s", g_Veh[slot][vDbId], what);
    Staff_Log(playerid, "atune", ownerid, details);
    if (ownerid != INVALID_PLAYER_ID && ownerid != playerid)
    {
        Msg_Info(ownerid, "Admin %s je tunirao vase vozilo %s (%s).", Account_Name(playerid), Veh_Name(slot), what);
    }
}

static stock Tune_SetPart(playerid, vehicleid, slot, component)
{
    new current = GetVehicleComponentInSlot(vehicleid, CARMODTYPE:slot), what[48];
    if (component == 0)
    {
        if (current != 0)
        {
            RemoveVehicleComponent(vehicleid, current);
        }
        format(what, sizeof(what), "%s skinut", g_TuneSlotNames[slot]);
    }
    else
    {
        AddVehicleComponent(vehicleid, component);
        format(what, sizeof(what), "%s %d", g_TuneSlotNames[slot], component);
    }
    if (g_VehSlotOf[vehicleid] != -1)
    {
        Veh_SetMod(g_VehSlotOf[vehicleid], CARMODTYPE:slot, component);
    }
    Tune_Saved(playerid, vehicleid, what);
}

static stock Tune_SetPaintjob(playerid, vehicleid, paintjob)
{
    ChangeVehiclePaintjob(vehicleid, paintjob);
    if (g_VehSlotOf[vehicleid] != -1)
    {
        Veh_SetPaintjob(g_VehSlotOf[vehicleid], paintjob);
    }
    Tune_Saved(playerid, vehicleid, paintjob == VEH_PAINTJOB_NONE ? ("paintjob skinut") : ("paintjob"));
}

static stock Tune_Clear(playerid, vehicleid)
{
    for (new slot = 0; slot < VEH_TUNE_SLOTS; slot++)
    {
        new component = GetVehicleComponentInSlot(vehicleid, CARMODTYPE:slot);
        if (component != 0)
        {
            RemoveVehicleComponent(vehicleid, component);
        }
        if (g_VehSlotOf[vehicleid] != -1)
        {
            g_Veh[g_VehSlotOf[vehicleid]][vMods][slot] = 0;
        }
    }
    Tune_SetPaintjob(playerid, vehicleid, VEH_PAINTJOB_NONE);
}

cmd:atune(playerid, params[])
{
    new vehicleid = Tune_GetVehicle(playerid);
    if (vehicleid != 0)
    {
        Tune_ShowMenu(playerid, vehicleid);
    }
    return 1;
}

stock bool:Tune_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
    if (dialogid != DIALOG_TUNE_MENU && dialogid != DIALOG_TUNE_PARTS && dialogid != DIALOG_TUNE_COLOUR && dialogid != DIALOG_TUNE_PAINTJOB)
    {
        return false;
    }
    if (!response && dialogid == DIALOG_TUNE_MENU)
    {
        return true;
    }
    // The player could have left the vehicle or the duty while the dialog was open.
    new vehicleid = Tune_GetVehicle(playerid);
    if (vehicleid == 0)
    {
        return true;
    }
    if (!response)
    {
        Tune_ShowMenu(playerid, vehicleid);
        return true;
    }

    switch (dialogid)
    {
        case DIALOG_TUNE_MENU:
        {
            if (listitem < 0 || listitem >= sizeof(g_TuneMenu[]))
            {
                return true;
            }
            switch (g_TuneMenu[playerid][listitem])
            {
                case TUNE_MENU_COLOUR:
                {
                    ShowPlayerDialog(playerid, DIALOG_TUNE_COLOUR, DIALOG_STYLE_INPUT, "Boja vozila", "Upisite dve boje (0-255), npr: 1 0", "Ofarbaj", "Nazad");
                }
                case TUNE_MENU_PAINTJOB:
                {
                    ShowPlayerDialog(playerid, DIALOG_TUNE_PAINTJOB, DIALOG_STYLE_LIST, "Paintjob (samo neki modeli)", "Paintjob 1\nPaintjob 2\nPaintjob 3\nBez paintjob-a", "Izaberi", "Nazad");
                }
                case TUNE_MENU_CLEAR:
                {
                    Tune_Clear(playerid, vehicleid);
                    Msg_Admin(playerid, "Sav tuning je skinut sa vozila.");
                    Tune_ShowMenu(playerid, vehicleid);
                }
                default:
                {
                    Tune_ShowParts(playerid, vehicleid, g_TuneMenu[playerid][listitem]);
                }
            }
        }
        case DIALOG_TUNE_PARTS:
        {
            if (listitem >= 0 && listitem < g_TunePartCount[playerid])
            {
                Tune_SetPart(playerid, vehicleid, g_TuneSlot[playerid], g_TuneParts[playerid][listitem]);
            }
            Tune_ShowParts(playerid, vehicleid, g_TuneSlot[playerid]);
        }
        case DIALOG_TUNE_COLOUR:
        {
            new colour1, colour2;
            if (sscanf(inputtext, "dd", colour1, colour2) || colour1 < 0 || colour1 > 255 || colour2 < 0 || colour2 > 255)
            {
                Msg_Error(playerid, "Upisite dve boje od 0 do 255, npr: 1 0");
            }
            else
            {
                ChangeVehicleColours(vehicleid, colour1, colour2);
                Veh_OnColoursChanged(vehicleid, colour1, colour2);
                new what[24];
                format(what, sizeof(what), "boja %d %d", colour1, colour2);
                Tune_Saved(playerid, vehicleid, what);
            }
            Tune_ShowMenu(playerid, vehicleid);
        }
        case DIALOG_TUNE_PAINTJOB:
        {
            if (listitem >= 0 && listitem <= VEH_PAINTJOB_NONE)
            {
                Tune_SetPaintjob(playerid, vehicleid, listitem);
            }
            Tune_ShowMenu(playerid, vehicleid);
        }
    }
    return true;
}
