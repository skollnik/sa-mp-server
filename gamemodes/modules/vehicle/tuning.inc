#define TUNE_COMPONENT_FIRST    (1000)
#define TUNE_COMPONENT_LAST     (1193)
// Menu entries after the part slots.
#define TUNE_MENU_COLOUR        (100)
#define TUNE_MENU_PAINTJOB      (101)
#define TUNE_MENU_CLEAR         (102)
#define TUNE_MENU_NEON          (103)
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

// What a part costs at the mechanic, by slot (nitro by size, see Tune_PartPrice); docs/ECONOMY.md lists them.
static const g_TunePartPrices[VEH_TUNE_SLOTS] = { 800, 600, 500, 700, 300, 0, 500, 1200, 800, 3000, 600, 600, 300, 300, 400, 400 };

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
    TUNE_VIEW_PAINTJOB,
    TUNE_VIEW_NEON
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
// The mechanic serving the player, or INVALID_PLAYER_ID for an admin's free /atune.
new g_TuneMechanic[MAX_PLAYERS] = { INVALID_PLAYER_ID, ... };
// At a mechanic the first pick only tries a part on; it is paid on the second pick and put back when the player looks elsewhere.
new g_TunePreview[MAX_PLAYERS];
new g_TunePreviewOld[MAX_PLAYERS];
new g_TunePreviewPaintjob[MAX_PLAYERS] = { -1, ... };
new g_TunePreviewNeon[MAX_PLAYERS] = { -1, ... };
new bool:g_TuneColourPreview[MAX_PLAYERS];
new g_TuneOldColours[MAX_PLAYERS][2];
new g_TuneVehicle[MAX_PLAYERS];

// Admin vehicles are for any admin; a personal vehicle is someone's property, so free tuning on it is for senior admins.
// Returns the vehicle or 0, with the reason already told.
static stock Tune_GetVehicle(playerid)
{
    if (g_TuneMechanic[playerid] != INVALID_PLAYER_ID)
    {
        return Mech_TuningVehicle(playerid, g_TuneMechanic[playerid]);
    }
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

static stock Tune_PartPrice(slot, component)
{
    switch (component)
    {
        case 1008: return TUNE_PRICE_NITRO_X2;
        case 1009: return TUNE_PRICE_NITRO_X5;
        case 1010: return TUNE_PRICE_NITRO_X10;
    }
    return g_TunePartPrices[slot];
}

static stock bool:Tune_AtMechanic(playerid)
{
    return g_TuneMechanic[playerid] != INVALID_PLAYER_ID;
}

// Free for an admin; at a mechanic the customer pays and the mechanic gets a share, the rest leaves the game.
static stock bool:Tune_Pay(playerid, price, const what[])
{
    if (!Tune_AtMechanic(playerid))
    {
        return true;
    }
    new reason[48], mechanic = g_TuneMechanic[playerid];
    format(reason, sizeof(reason), "tjuning: %s", what);
    if (!Money_Take(playerid, price, reason))
    {
        return false;
    }
    new share = price * TUNE_MECHANIC_SHARE / 100;
    Money_GiveCash(mechanic, share, "mehanicar: tjuning", g_Player[playerid][pDbId]);
    Msg_Info(mechanic, "Tjuning za %s (%s, $%d): vasa zarada $%d.", Account_Name(playerid), what, price, share);
    return true;
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

// Puts back whatever was only being tried on, so nothing unpaid stays on the vehicle.
static stock Tune_RevertPreview(playerid, vehicleid)
{
    if (g_TunePreview[playerid] != 0)
    {
        RemoveVehicleComponent(vehicleid, g_TunePreview[playerid]);
        if (g_TunePreviewOld[playerid] != 0)
        {
            AddVehicleComponent(vehicleid, g_TunePreviewOld[playerid]);
        }
        g_TunePreview[playerid] = 0;
    }
    if (g_TunePreviewPaintjob[playerid] != -1)
    {
        new slot = g_VehSlotOf[vehicleid];
        ChangeVehiclePaintjob(vehicleid, slot == -1 ? VEH_PAINTJOB_NONE : g_Veh[slot][vPaintjob]);
        g_TunePreviewPaintjob[playerid] = -1;
    }
    if (g_TunePreviewNeon[playerid] != -1)
    {
        new slot = g_VehSlotOf[vehicleid];
        if (slot != -1)
        {
            Veh_ApplyNeon(slot);
        }
        g_TunePreviewNeon[playerid] = -1;
    }
    if (g_TuneColourPreview[playerid])
    {
        ChangeVehicleColours(vehicleid, g_TuneOldColours[playerid][0], g_TuneOldColours[playerid][1]);
        g_TuneColourPreview[playerid] = false;
    }
}

// Also called when the admin leaves the vehicle, dies or disconnects, so the camera and controls always come back.
stock Tune_Close(playerid)
{
    if (g_TuneActive[playerid] && IsValidVehicle(g_TuneVehicle[playerid]))
    {
        Tune_RevertPreview(playerid, g_TuneVehicle[playerid]);
    }
    g_TunePreview[playerid] = 0;
    g_TunePreviewPaintjob[playerid] = -1;
    g_TunePreviewNeon[playerid] = -1;
    g_TuneColourPreview[playerid] = false;
    g_TuneMechanic[playerid] = INVALID_PLAYER_ID;
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

// The last page can be shorter, so the page button is not always on the same row.
static stock Tune_ItemsOnPage(playerid)
{
    new rows = Tune_RowsOnPage(playerid), left = g_TuneCount[playerid] - g_TunePage[playerid] * rows;
    return left < rows ? left : rows;
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
    Tune_Begin(playerid, TUNE_VIEW_MAIN, Tune_AtMechanic(playerid) ? ("Mehanicar - tjuning") : ("Tuning"));
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
    if (Veh_CanHaveNeon(vehicleid))
    {
        Tune_Add(playerid, "Neon", TUNE_MENU_NEON);
    }
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
            if (c == g_TunePreview[playerid])
            {
                format(label, sizeof(label), "%s - kupi $%d", name, Tune_PartPrice(slot, c));
            }
            else if (c == Tune_Installed(playerid, vehicleid, slot))
            {
                format(label, sizeof(label), "%s (stavljeno)", name);
            }
            else if (Tune_AtMechanic(playerid))
            {
                format(label, sizeof(label), "%s ($%d)", name, Tune_PartPrice(slot, c));
            }
            else
            {
                strcopy(label, name);
            }
            Tune_Add(playerid, label, c);
        }
    }
    g_TunePage[playerid] = page;
    Tune_Present(playerid);
}

static stock Tune_ShowPaintjobs(playerid)
{
    new label[TUNE_LABEL_LENGTH];
    Tune_Begin(playerid, TUNE_VIEW_PAINTJOB, "Paintjob");
    for (new paintjob = 0; paintjob < VEH_PAINTJOB_NONE; paintjob++)
    {
        if (paintjob == g_TunePreviewPaintjob[playerid])
        {
            format(label, sizeof(label), "Paintjob %d - kupi $%d", paintjob + 1, TUNE_PRICE_PAINTJOB);
        }
        else if (Tune_AtMechanic(playerid))
        {
            format(label, sizeof(label), "Paintjob %d ($%d)", paintjob + 1, TUNE_PRICE_PAINTJOB);
        }
        else
        {
            format(label, sizeof(label), "Paintjob %d", paintjob + 1);
        }
        Tune_Add(playerid, label, paintjob);
    }
    Tune_Add(playerid, "Bez paintjob-a", VEH_PAINTJOB_NONE);
    Tune_Present(playerid);
}

static stock Tune_NeonPrice(playerid)
{
    return Vip_GetLevel(playerid) >= VIP_GOLD ? TUNE_PRICE_NEON / 2 : TUNE_PRICE_NEON;
}

static stock Tune_ShowNeon(playerid, vehicleid)
{
    new label[TUNE_LABEL_LENGTH], installed = g_Veh[g_VehSlotOf[vehicleid]][vNeon];
    Tune_Begin(playerid, TUNE_VIEW_NEON, "Neon");
    for (new colour = 1; colour <= VEH_NEON_COLOURS; colour++)
    {
        if (colour == g_TunePreviewNeon[playerid])
        {
            format(label, sizeof(label), "%s - kupi $%d", g_VehNeonNames[colour - 1], Tune_NeonPrice(playerid));
        }
        else if (colour == installed)
        {
            format(label, sizeof(label), "%s (stavljeno)", g_VehNeonNames[colour - 1]);
        }
        else if (Tune_AtMechanic(playerid))
        {
            format(label, sizeof(label), "%s ($%d)", g_VehNeonNames[colour - 1], Tune_NeonPrice(playerid));
        }
        else
        {
            strcopy(label, g_VehNeonNames[colour - 1]);
        }
        Tune_Add(playerid, label, colour);
    }
    Tune_Add(playerid, "Bez neona", 0);
    Tune_Present(playerid);
}

// A personal vehicle keeps its tuning, and the change is logged since it is free value for the owner.
static stock Tune_Saved(playerid, vehicleid, const what[])
{
    if (Tune_AtMechanic(playerid))
    {
        return;
    }
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

// What is really on the vehicle in this slot, not the part being tried on.
static stock Tune_Installed(playerid, vehicleid, slot)
{
    return g_TunePreview[playerid] != 0 ? g_TunePreviewOld[playerid] : GetVehicleComponentInSlot(vehicleid, CARMODTYPE:slot);
}

static stock Tune_SetPart(playerid, vehicleid, slot, component)
{
    new installed = Tune_Installed(playerid, vehicleid, slot), what[48];
    if (component == 0)
    {
        Tune_RevertPreview(playerid, vehicleid);
        if (installed != 0)
        {
            RemoveVehicleComponent(vehicleid, installed);
        }
        format(what, sizeof(what), "%s skinut", g_TuneSlotNames[slot]);
    }
    else
    {
        if (component == installed)
        {
            Tune_RevertPreview(playerid, vehicleid);
            return;
        }
        if (Tune_AtMechanic(playerid) && g_TunePreview[playerid] != component)
        {
            Tune_RevertPreview(playerid, vehicleid);
            g_TunePreviewOld[playerid] = installed;
            g_TunePreview[playerid] = component;
            AddVehicleComponent(vehicleid, component);
            return;
        }
        format(what, sizeof(what), "%s %d", g_TuneSlotNames[slot], component);
        if (!Tune_Pay(playerid, Tune_PartPrice(slot, component), g_TuneSlotNames[slot]))
        {
            Tune_RevertPreview(playerid, vehicleid);
            return;
        }
        g_TunePreview[playerid] = 0;
        AddVehicleComponent(vehicleid, component);
    }
    if (g_VehSlotOf[vehicleid] != -1)
    {
        Veh_SetMod(g_VehSlotOf[vehicleid], CARMODTYPE:slot, component);
    }
    Tune_Saved(playerid, vehicleid, what);
}

static stock Tune_SetNeon(playerid, vehicleid, colour)
{
    new slot = g_VehSlotOf[vehicleid];
    if (colour == g_Veh[slot][vNeon])
    {
        Tune_RevertPreview(playerid, vehicleid);
        return;
    }
    if (colour != 0 && Tune_AtMechanic(playerid))
    {
        if (g_TunePreviewNeon[playerid] != colour)
        {
            g_TunePreviewNeon[playerid] = colour;
            Veh_ShowNeon(vehicleid, colour);
            return;
        }
        if (!Tune_Pay(playerid, Tune_NeonPrice(playerid), "neon"))
        {
            Tune_RevertPreview(playerid, vehicleid);
            return;
        }
    }
    g_TunePreviewNeon[playerid] = -1;
    Veh_SetNeon(slot, colour);
    Tune_Saved(playerid, vehicleid, colour == 0 ? ("neon skinut") : ("neon"));
}

static stock Tune_SetPaintjob(playerid, vehicleid, paintjob)
{
    new slot = g_VehSlotOf[vehicleid];
    if (Tune_AtMechanic(playerid) && paintjob != VEH_PAINTJOB_NONE && (slot == -1 || g_Veh[slot][vPaintjob] != paintjob))
    {
        if (g_TunePreviewPaintjob[playerid] != paintjob)
        {
            g_TunePreviewPaintjob[playerid] = paintjob;
            ChangeVehiclePaintjob(vehicleid, paintjob);
            return;
        }
        if (!Tune_Pay(playerid, TUNE_PRICE_PAINTJOB, "paintjob"))
        {
            Tune_RevertPreview(playerid, vehicleid);
            return;
        }
    }
    g_TunePreviewPaintjob[playerid] = -1;
    ChangeVehiclePaintjob(vehicleid, paintjob);
    if (g_VehSlotOf[vehicleid] != -1)
    {
        Veh_SetPaintjob(g_VehSlotOf[vehicleid], paintjob);
    }
    Tune_Saved(playerid, vehicleid, paintjob == VEH_PAINTJOB_NONE ? ("paintjob skinut") : ("paintjob"));
}

static stock Tune_Clear(playerid, vehicleid)
{
    Tune_RevertPreview(playerid, vehicleid);
    if (g_VehSlotOf[vehicleid] != -1 && g_Veh[g_VehSlotOf[vehicleid]][vNeon] != 0)
    {
        Veh_SetNeon(g_VehSlotOf[vehicleid], 0);
    }
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

// Frozen so the menu keys do not drive the vehicle away from the camera.
static stock Tune_Open(playerid, vehicleid)
{
    g_TuneActive[playerid] = true;
    g_TuneVehicle[playerid] = vehicleid;
    TogglePlayerControllable(playerid, false);
    Tune_FocusCamera(playerid, vehicleid);
    Tune_ShowMain(playerid, vehicleid);
    Msg_Info(playerid, "Strelice gore i dole biraju, Space potvrdjuje, Enter vraca nazad i zatvara meni.");
}

cmd:atune(playerid, params[])
{
    g_TuneMechanic[playerid] = INVALID_PLAYER_ID;
    new vehicleid = Tune_GetVehicle(playerid);
    if (vehicleid == 0)
    {
        return 1;
    }
    Tune_Open(playerid, vehicleid);
    return 1;
}

// Opened when the customer accepts the mechanic's offer.
stock Tune_OpenForCustomer(playerid, mechanic)
{
    g_TuneMechanic[playerid] = mechanic;
    new vehicleid = Tune_GetVehicle(playerid);
    if (vehicleid == 0)
    {
        g_TuneMechanic[playerid] = INVALID_PLAYER_ID;
        return;
    }
    Tune_Open(playerid, vehicleid);
    Msg_Info(mechanic, "%s je otvorio meni za tjuning.", Account_Name(playerid));
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
    new rows = Tune_RowsOnPage(playerid), items = Tune_ItemsOnPage(playerid);
    if (g_TuneCount[playerid] > TUNE_MENU_ROWS && row == items)
    {
        new pages = (g_TuneCount[playerid] + rows - 1) / rows;
        g_TunePage[playerid] = (g_TunePage[playerid] + 1) % pages;
        Tune_Present(playerid);
        return;
    }
    new index = g_TunePage[playerid] * rows + row;
    if (row < 0 || row >= items)
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
                    new text[80] = "Upisite dve boje (0-255), npr: 1 0";
                    if (Tune_AtMechanic(playerid))
                    {
                        format(text, sizeof(text), "Upisite dve boje (0-255), npr: 1 0\n\nBoja se prvo prikaze; farbanje kosta $%d.", TUNE_PRICE_PAINT);
                    }
                    ShowPlayerDialog(playerid, DIALOG_TUNE_COLOUR, DIALOG_STYLE_INPUT, "Boja vozila", text, "Ofarbaj", "Nazad");
                }
                case TUNE_MENU_PAINTJOB:
                {
                    Tune_ShowPaintjobs(playerid);
                }
                case TUNE_MENU_NEON:
                {
                    Tune_ShowNeon(playerid, vehicleid);
                }
                case TUNE_MENU_CLEAR:
                {
                    Tune_Clear(playerid, vehicleid);
                    Msg_Info(playerid, "Sav tuning je skinut sa vozila.");
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
        case TUNE_VIEW_NEON:
        {
            Tune_SetNeon(playerid, vehicleid, value);
            if (g_TunePreviewNeon[playerid] != -1)
            {
                Tune_ShowNeon(playerid, vehicleid);
            }
            else
            {
                Tune_ShowMain(playerid, vehicleid);
            }
        }
        case TUNE_VIEW_PAINTJOB:
        {
            Tune_SetPaintjob(playerid, vehicleid, value);
            if (g_TunePreviewPaintjob[playerid] != -1)
            {
                Tune_ShowPaintjobs(playerid);
            }
            else
            {
                Tune_ShowMain(playerid, vehicleid);
            }
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
    Tune_RevertPreview(playerid, vehicleid);
    Tune_ShowMain(playerid, vehicleid);
}

stock Tune_OnPlayerStateChange(playerid, PLAYER_STATE:newstate)
{
    if (newstate != PLAYER_STATE_DRIVER)
    {
        Tune_Close(playerid);
    }
}

static stock Tune_SaveColours(playerid, vehicleid, colour1, colour2)
{
    Veh_OnColoursChanged(vehicleid, colour1, colour2);
    new what[24];
    format(what, sizeof(what), "boja %d %d", colour1, colour2);
    Tune_Saved(playerid, vehicleid, what);
}

stock bool:Tune_OnDialogResponse(playerid, dialogid, response, const inputtext[])
{
    if (dialogid == DIALOG_TUNE_COLOUR_BUY)
    {
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
        if (response && g_TuneColourPreview[playerid] && Tune_Pay(playerid, TUNE_PRICE_PAINT, "farbanje"))
        {
            new colour1, colour2;
            GetVehicleColours(vehicleid, colour1, colour2);
            g_TuneColourPreview[playerid] = false;
            Tune_SaveColours(playerid, vehicleid, colour1, colour2);
        }
        Tune_RevertPreview(playerid, vehicleid);
        Tune_ShowMain(playerid, vehicleid);
        return true;
    }
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
        else if (Tune_AtMechanic(playerid))
        {
            new text[96];
            // The saved colours, not the game's, so "Vrati" always brings back what the owner paid for.
            new slot = g_VehSlotOf[vehicleid];
            g_TuneOldColours[playerid][0] = g_Veh[slot][vColor1];
            g_TuneOldColours[playerid][1] = g_Veh[slot][vColor2];
            g_TuneColourPreview[playerid] = true;
            ChangeVehicleColours(vehicleid, colour1, colour2);
            format(text, sizeof(text), "Ovako izgleda boja %d %d.\n\nFarbanje kosta $%d.", colour1, colour2, TUNE_PRICE_PAINT);
            ShowPlayerDialog(playerid, DIALOG_TUNE_COLOUR_BUY, DIALOG_STYLE_MSGBOX, "Boja vozila", text, "Kupi", "Vrati");
            return true;
        }
        else
        {
            ChangeVehicleColours(vehicleid, colour1, colour2);
            Tune_SaveColours(playerid, vehicleid, colour1, colour2);
        }
    }
    Tune_ShowMain(playerid, vehicleid);
    return true;
}
