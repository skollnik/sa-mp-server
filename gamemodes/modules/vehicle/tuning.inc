#define TUNE_COMPONENT_FIRST    (1000)
#define TUNE_COMPONENT_LAST     (1193)
// Menu entries after the part slots.
#define TUNE_MENU_COLOUR        (100)
#define TUNE_MENU_PAINTJOB      (101)
#define TUNE_MENU_CLEAR         (102)
// GTA's own menu sits on the left of the screen, so the vehicle stays in view while it is tuned; a dialog would cover it.
// It holds at most 12 rows, so longer lists get a last row that turns the page.
#define TUNE_MENU_ROWS          (12)
#define TUNE_MAX_ENTRIES        (24)
#define TUNE_LABEL_LENGTH       (32)

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

enum E_TUNE_VIEW
{
    TUNE_VIEW_MAIN,
    TUNE_VIEW_PARTS,
    TUNE_VIEW_PAINTJOB
}

new Menu:g_TuneMenuId[MAX_PLAYERS] = { INVALID_MENU, ... };
new bool:g_TuneActive[MAX_PLAYERS];
new E_TUNE_VIEW:g_TuneView[MAX_PLAYERS];
new g_TuneTitle[MAX_PLAYERS][TUNE_LABEL_LENGTH];
new g_TuneLabels[MAX_PLAYERS][TUNE_MAX_ENTRIES][TUNE_LABEL_LENGTH];
new g_TuneValues[MAX_PLAYERS][TUNE_MAX_ENTRIES];
new g_TuneCount[MAX_PLAYERS];
new g_TunePage[MAX_PLAYERS];
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

// The camera looks at the vehicle from its front right corner and is aimed a little to the left of it,
// so the vehicle shows on the right half of the screen, away from the menu.
static stock Tune_FocusCamera(playerid, vehicleid)
{
    new Float:x, Float:y, Float:z, Float:angle;
    GetVehiclePos(vehicleid, x, y, z);
    GetVehicleZAngle(vehicleid, angle);
    new Float:cam_x = x - 5.0 * floatsin(angle, degrees) + 4.5 * floatcos(angle, degrees);
    new Float:cam_y = y + 5.0 * floatcos(angle, degrees) + 4.5 * floatsin(angle, degrees);
    new Float:dx = x - cam_x, Float:dy = y - cam_y, Float:length = floatsqroot(dx * dx + dy * dy);
    SetPlayerCameraPos(playerid, cam_x, cam_y, z + 1.6);
    SetPlayerCameraLookAt(playerid, x - 1.6 * dy / length, y + 1.6 * dx / length, z + 0.3);
}

static stock Tune_DestroyMenu(playerid)
{
    if (g_TuneMenuId[playerid] != INVALID_MENU)
    {
        HideMenuForPlayer(g_TuneMenuId[playerid], playerid);
        DestroyMenu(g_TuneMenuId[playerid]);
        g_TuneMenuId[playerid] = INVALID_MENU;
    }
}

// Also called when the admin leaves the vehicle, dies or disconnects, so the camera and controls always come back.
stock Tune_Close(playerid)
{
    if (!g_TuneActive[playerid])
    {
        return;
    }
    g_TuneActive[playerid] = false;
    Tune_DestroyMenu(playerid);
    if (IsPlayerConnected(playerid))
    {
        SetCameraBehindPlayer(playerid);
        if (!g_Staff[playerid][sFrozen])
        {
            TogglePlayerControllable(playerid, true);
        }
    }
}

static stock Tune_RowsOnPage(playerid)
{
    return g_TuneCount[playerid] <= TUNE_MENU_ROWS ? g_TuneCount[playerid] : TUNE_MENU_ROWS - 1;
}

static stock Tune_Present(playerid)
{
    Tune_DestroyMenu(playerid);
    new rows = Tune_RowsOnPage(playerid), first = g_TunePage[playerid] * rows;
    new Menu:menu = CreateMenu(g_TuneTitle[playerid], 1, 30.0, 150.0, 200.0);
    if (menu == INVALID_MENU)
    {
        Msg_Error(playerid, "Meni trenutno nije dostupan, pokusajte ponovo.");
        Tune_Close(playerid);
        return;
    }
    for (new i = first; i < first + rows && i < g_TuneCount[playerid]; i++)
    {
        AddMenuItem(menu, 0, g_TuneLabels[playerid][i]);
    }
    if (g_TuneCount[playerid] > TUNE_MENU_ROWS)
    {
        AddMenuItem(menu, 0, first + rows < g_TuneCount[playerid] ? ("Sledeca strana >>") : ("<< Prva strana"));
    }
    g_TuneMenuId[playerid] = menu;
    ShowMenuForPlayer(menu, playerid);
}

static stock Tune_Begin(playerid, E_TUNE_VIEW:view, const title[])
{
    g_TuneView[playerid] = view;
    g_TuneCount[playerid] = 0;
    g_TunePage[playerid] = 0;
    strcopy(g_TuneTitle[playerid], title, TUNE_LABEL_LENGTH);
}

static stock Tune_Add(playerid, const label[], value)
{
    new i = g_TuneCount[playerid];
    if (i >= TUNE_MAX_ENTRIES)
    {
        return;
    }
    strcopy(g_TuneLabels[playerid][i], label, TUNE_LABEL_LENGTH);
    g_TuneValues[playerid][i] = value;
    g_TuneCount[playerid]++;
}

static stock Tune_ShowMain(playerid, vehicleid)
{
    new model = GetVehicleModel(vehicleid);
    Tune_Begin(playerid, TUNE_VIEW_MAIN, "Tuning");
    for (new slot = 0; slot < VEH_TUNE_SLOTS; slot++)
    {
        for (new c = TUNE_COMPONENT_FIRST; c <= TUNE_COMPONENT_LAST; c++)
        {
            if (_:GetVehicleComponentType(c) == slot && VehicleCanHaveComponent(model, c))
            {
                Tune_Add(playerid, g_TuneSlotNames[slot], slot);
                break;
            }
        }
    }
    Tune_Add(playerid, "Boja", TUNE_MENU_COLOUR);
    Tune_Add(playerid, "Paintjob", TUNE_MENU_PAINTJOB);
    Tune_Add(playerid, "Skini sav tuning", TUNE_MENU_CLEAR);
    Tune_Present(playerid);
}

static stock Tune_ShowParts(playerid, vehicleid, slot, page = 0)
{
    new name[24], label[TUNE_LABEL_LENGTH], count, model = GetVehicleModel(vehicleid);
    Tune_Begin(playerid, TUNE_VIEW_PARTS, g_TuneSlotNames[slot]);
    g_TuneSlot[playerid] = slot;
    Tune_Add(playerid, "Bez dela (skini)", 0);
    for (new c = TUNE_COMPONENT_FIRST; c <= TUNE_COMPONENT_LAST; c++)
    {
        if (_:GetVehicleComponentType(c) == slot && VehicleCanHaveComponent(model, c))
        {
            Tune_PartName(c, ++count, name);
            format(label, sizeof(label), "%s%s", name, GetVehicleComponentInSlot(vehicleid, CARMODTYPE:slot) == c ? (" (stavljeno)") : (""));
            Tune_Add(playerid, label, c);
        }
    }
    g_TunePage[playerid] = page;
    Tune_Present(playerid);
}

static stock Tune_ShowPaintjobs(playerid)
{
    Tune_Begin(playerid, TUNE_VIEW_PAINTJOB, "Paintjob");
    Tune_Add(playerid, "Paintjob 1", 0);
    Tune_Add(playerid, "Paintjob 2", 1);
    Tune_Add(playerid, "Paintjob 3", 2);
    Tune_Add(playerid, "Bez paintjob-a", VEH_PAINTJOB_NONE);
    Tune_Present(playerid);
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
    if (vehicleid == 0)
    {
        return 1;
    }
    // Frozen so the menu keys do not drive the vehicle away from the camera.
    g_TuneActive[playerid] = true;
    TogglePlayerControllable(playerid, false);
    Tune_FocusCamera(playerid, vehicleid);
    Tune_ShowMain(playerid, vehicleid);
    Msg_Info(playerid, "Strelice gore i dole biraju, Space potvrdjuje, Enter vraca nazad i zatvara meni.");
    return 1;
}

stock Tune_OnPlayerSelectedMenuRow(playerid, row)
{
    if (!g_TuneActive[playerid])
    {
        return;
    }
    // The admin could have left the vehicle or gone off duty while the menu was open.
    new vehicleid = Tune_GetVehicle(playerid);
    if (vehicleid == 0)
    {
        Tune_Close(playerid);
        return;
    }
    new rows = Tune_RowsOnPage(playerid);
    if (g_TuneCount[playerid] > TUNE_MENU_ROWS && row == rows)
    {
        new pages = (g_TuneCount[playerid] + rows - 1) / rows;
        g_TunePage[playerid] = (g_TunePage[playerid] + 1) % pages;
        Tune_Present(playerid);
        return;
    }
    new index = g_TunePage[playerid] * rows + row;
    if (row < 0 || row >= rows || index >= g_TuneCount[playerid])
    {
        Tune_Present(playerid);
        return;
    }
    new value = g_TuneValues[playerid][index];
    switch (g_TuneView[playerid])
    {
        case TUNE_VIEW_MAIN:
        {
            switch (value)
            {
                case TUNE_MENU_COLOUR:
                {
                    Tune_DestroyMenu(playerid);
                    ShowPlayerDialog(playerid, DIALOG_TUNE_COLOUR, DIALOG_STYLE_INPUT, "Boja vozila", "Upisite dve boje (0-255), npr: 1 0", "Ofarbaj", "Nazad");
                }
                case TUNE_MENU_PAINTJOB:
                {
                    Tune_ShowPaintjobs(playerid);
                }
                case TUNE_MENU_CLEAR:
                {
                    Tune_Clear(playerid, vehicleid);
                    Msg_Admin(playerid, "Sav tuning je skinut sa vozila.");
                    Tune_ShowMain(playerid, vehicleid);
                }
                default:
                {
                    Tune_ShowParts(playerid, vehicleid, value);
                }
            }
        }
        case TUNE_VIEW_PARTS:
        {
            Tune_SetPart(playerid, vehicleid, g_TuneSlot[playerid], value);
            Tune_ShowParts(playerid, vehicleid, g_TuneSlot[playerid], g_TunePage[playerid]);
        }
        case TUNE_VIEW_PAINTJOB:
        {
            Tune_SetPaintjob(playerid, vehicleid, value);
            Tune_ShowMain(playerid, vehicleid);
        }
    }
}

// Enter in a sub-menu goes back to the main one; in the main menu it ends tuning.
stock Tune_OnPlayerExitedMenu(playerid)
{
    if (!g_TuneActive[playerid])
    {
        return;
    }
    new vehicleid = Tune_GetVehicle(playerid);
    if (vehicleid == 0 || g_TuneView[playerid] == TUNE_VIEW_MAIN)
    {
        Tune_Close(playerid);
        return;
    }
    Tune_ShowMain(playerid, vehicleid);
}

stock Tune_OnPlayerStateChange(playerid, PLAYER_STATE:newstate)
{
    if (newstate != PLAYER_STATE_DRIVER)
    {
        Tune_Close(playerid);
    }
}

stock bool:Tune_OnDialogResponse(playerid, dialogid, response, const inputtext[])
{
    if (dialogid != DIALOG_TUNE_COLOUR)
    {
        return false;
    }
    if (!g_TuneActive[playerid])
    {
        return true;
    }
    new vehicleid = Tune_GetVehicle(playerid);
    if (vehicleid == 0)
    {
        Tune_Close(playerid);
        return true;
    }
    if (response)
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
    }
    Tune_ShowMain(playerid, vehicleid);
    return true;
}
