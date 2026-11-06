#define MECH_VEHICLE_MODEL      (525)
#define MECH_VEHICLE_RESPAWN_S  (600)
// From the workshop point, so the lift, the garages and the parking next to them all count as the workshop.
#define MECH_WORKSHOP_RADIUS    (35.0)
#define MECH_REACH              (6.0)
#define MECH_REPAIR_MS          (10000)
#define MECH_SERVICE_MS         (30000)
#define MECH_OFFER_S            (60)
#define MECH_CALL_S             (600)
#define MECH_CALL_COOLDOWN_S    (60)
#define MECH_TOW_RANGE          (8.0)
// A field repair gets the engine going again (60%); the rest of the work is done in the workshop.
#define MECH_FIELD_HEALTH       (700.0)

#define MECH_PART_ENGINE        (1)
#define MECH_PART_BODY          (2)
#define MECH_PART_TYRES         (4)

// The parking next to the workshop garages in Willowfield.
static const Float:g_MechVehicleSpots[][4] =
{
    { 1945.08, -1974.86, 13.42, 272.0 },
    { 1945.14, -1982.94, 13.42, 270.0 },
    { 1944.94, -1990.79, 13.42, 269.4 },
    { 1944.90, -1998.61, 13.43, 267.3 }
};

static const g_MechFieldParts[] = { MECH_PART_ENGINE, MECH_PART_TYRES, MECH_PART_ENGINE | MECH_PART_TYRES };
static const g_MechShopParts[] = { MECH_PART_ENGINE, MECH_PART_BODY, MECH_PART_TYRES, MECH_PART_ENGINE | MECH_PART_BODY | MECH_PART_TYRES };

enum E_MECH_OFFER_KIND
{
    MECH_OFFER_NONE,
    MECH_OFFER_REPAIR,
    MECH_OFFER_TUNING
}

// Indexed by the customer: one offer at a time.
enum E_MECH_OFFER
{
    E_MECH_OFFER_KIND:moKind,
    moMechanic,
    moVehicle,
    moParts,
    moPrice,
    bool:moWorkshop,
    moExpires
}

new g_MechOffer[MAX_PLAYERS][E_MECH_OFFER];
// What the mechanic is picking in the parts dialog.
new g_MechPickTarget[MAX_PLAYERS];
new g_MechPickPrice[MAX_PLAYERS];
new bool:g_MechPickWorkshop[MAX_PLAYERS];
// Running repair (mechanic) and workshop service (driver); 0 when none.
new g_MechWorkTimer[MAX_PLAYERS];
new g_MechServiceTimer[MAX_PLAYERS];
// An open 555 call and the last one, by the caller.
new g_MechCallAt[MAX_PLAYERS];
new g_MechCallLast[MAX_PLAYERS];

stock Mech_Init()
{
    for (new i = 0; i < sizeof(g_MechVehicleSpots); i++)
    {
        new vehicleid = CreateVehicle(MECH_VEHICLE_MODEL, g_MechVehicleSpots[i][0], g_MechVehicleSpots[i][1], g_MechVehicleSpots[i][2], g_MechVehicleSpots[i][3],
            6, 1, MECH_VEHICLE_RESPAWN_S);
        if (vehicleid == INVALID_VEHICLE_ID)
        {
            continue;
        }
        g_JobVehicle[vehicleid] = JOB_MECHANIC;
        Fuel_Set(vehicleid, float(Fuel_TankSize(MECH_VEHICLE_MODEL)));
        Veh_SetLabel(vehicleid, "MEHANICAR", COLOR_YELLOW);
    }
}

stock Mech_OnPlayerConnect(playerid)
{
    static const empty[E_MECH_OFFER];
    g_MechOffer[playerid] = empty;
    g_MechWorkTimer[playerid] = 0;
    g_MechServiceTimer[playerid] = 0;
    g_MechCallAt[playerid] = 0;
    g_MechCallLast[playerid] = 0;
}

stock Mech_OnPlayerDisconnect(playerid)
{
    if (g_MechWorkTimer[playerid] != 0)
    {
        KillTimer(g_MechWorkTimer[playerid]);
    }
    if (g_MechServiceTimer[playerid] != 0)
    {
        KillTimer(g_MechServiceTimer[playerid]);
    }
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (g_MechOffer[i][moKind] != MECH_OFFER_NONE && g_MechOffer[i][moMechanic] == playerid)
        {
            g_MechOffer[i][moKind] = MECH_OFFER_NONE;
        }
    }
}

stock bool:Mech_InWorkshop(playerid)
{
    return Point_IsWithin(playerid, POINT_WORKSHOP, MECH_WORKSHOP_RADIUS);
}

static stock Mech_OnDutyCount()
{
    new count;
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (IsPlayerConnected(i) && Job_IsOnDuty(i, JOB_MECHANIC))
        {
            count++;
        }
    }
    return count;
}

static stock bool:Mech_RequireOnDuty(playerid)
{
    if (Job_IsOnDuty(playerid, JOB_MECHANIC))
    {
        return true;
    }
    Msg_Error(playerid, "Ovo radi mehanicar na poslu (/oprema u radionici).");
    return false;
}

// The vehicle the customer drives, when the mechanic stands next to it on foot; 0 with the reason told.
static stock Mech_CustomerVehicle(mechanic, customer)
{
    if (GetPlayerState(customer) != PLAYER_STATE_DRIVER)
    {
        Msg_Error(mechanic, "Igrac mora da sedi za volanom vozila.");
        return 0;
    }
    if (GetPlayerState(mechanic) != PLAYER_STATE_ONFOOT)
    {
        Msg_Error(mechanic, "Izadjite iz vozila.");
        return 0;
    }
    new vehicleid = GetPlayerVehicleID(customer), Float:x, Float:y, Float:z;
    GetPlayerPos(mechanic, x, y, z);
    if (GetVehicleDistanceFromPoint(vehicleid, x, y, z) > MECH_REACH)
    {
        Msg_Error(mechanic, "Stanite pored vozila (do %.0f m).", MECH_REACH);
        return 0;
    }
    return vehicleid;
}

static stock Mech_PartsText(parts, bool:workshop, dest[], size = sizeof(dest))
{
    dest[0] = '\0';
    if (parts & MECH_PART_ENGINE)
    {
        strcat(dest, workshop ? ("motor") : ("motor (do 60%)"), size);
    }
    if (parts & MECH_PART_BODY)
    {
        strcat(dest, dest[0] ? (", karoserija") : ("karoserija"), size);
    }
    if (parts & MECH_PART_TYRES)
    {
        strcat(dest, dest[0] ? (", gume") : ("gume"), size);
    }
}

static stock Mech_ApplyRepair(vehicleid, parts, bool:workshop)
{
    if (parts & MECH_PART_ENGINE)
    {
        new Float:health, Float:repaired = workshop ? VEH_HEALTH_FULL : MECH_FIELD_HEALTH;
        GetVehicleHealth(vehicleid, health);
        if (health < repaired)
        {
            SetVehicleHealth(vehicleid, repaired);
        }
        g_VehStallAt[vehicleid] = 0;
    }
    if (parts & (MECH_PART_BODY | MECH_PART_TYRES))
    {
        new VEHICLE_PANEL_STATUS:panels, VEHICLE_DOOR_STATUS:doors, VEHICLE_LIGHT_STATUS:lights, VEHICLE_TYRE_STATUS:tyres;
        GetVehicleDamageStatus(vehicleid, panels, doors, lights, tyres);
        if (parts & MECH_PART_BODY)
        {
            panels = VEHICLE_PANEL_STATUS:0;
            doors = VEHICLE_DOOR_STATUS:0;
            lights = VEHICLE_LIGHT_STATUS:0;
        }
        if (parts & MECH_PART_TYRES)
        {
            tyres = VEHICLE_TYRE_STATUS:0;
        }
        UpdateVehicleDamageStatus(vehicleid, panels, doors, lights, tyres);
    }
    Veh_SaveIfPersonal(vehicleid);
}

static stock Mech_SendOffer(mechanic, customer, vehicleid, E_MECH_OFFER_KIND:kind, parts, price, bool:workshop)
{
    g_MechOffer[customer][moKind] = kind;
    g_MechOffer[customer][moMechanic] = mechanic;
    g_MechOffer[customer][moVehicle] = vehicleid;
    g_MechOffer[customer][moParts] = parts;
    g_MechOffer[customer][moPrice] = price;
    g_MechOffer[customer][moWorkshop] = workshop;
    g_MechOffer[customer][moExpires] = gettime() + MECH_OFFER_S;

    new text[320], what[48];
    if (kind == MECH_OFFER_REPAIR)
    {
        Mech_PartsText(parts, workshop, what);
        format(text, sizeof(text), "Mehanicar %s nudi popravku vaseg vozila.\n\nPopravlja se: %s\nCena: $%d\n\nPopravka traje %d sekundi; ne pomerajte vozilo.",
            Account_Name(mechanic), what, price, MECH_REPAIR_MS / 1000);
    }
    else
    {
        format(text, sizeof(text), "Mehanicar %s nudi tjuning i farbanje vaseg vozila.\n\nSvaki deo placate kada ga izaberete u meniju (cena pise pored dela).\nSkidanje delova je besplatno.",
            Account_Name(mechanic));
    }
    ShowPlayerDialog(customer, DIALOG_MECH_OFFER, DIALOG_STYLE_MSGBOX, "Mehanicar", text, "Prihvati", "Odbij");
    Msg_Info(mechanic, "Ponuda je poslata igracu %s (vazi %d sekundi).", Account_Name(customer), MECH_OFFER_S);
}

static stock Mech_StartRepair(mechanic, customer)
{
    new vehicleid = g_MechOffer[customer][moVehicle];
    ApplyAnimation(mechanic, "CAR", "Fixn_Car_Loop", 4.1, true, false, false, false, 0, SYNC_ALL);
    Chat_Action(mechanic, "otvara haubu i popravlja vozilo.");
    GameTextForPlayer(customer, "~w~popravka...", MECH_REPAIR_MS, 6);
    g_MechWorkTimer[mechanic] = SetTimerEx("Mech_FinishRepair", MECH_REPAIR_MS, false, "dddddddd", mechanic, customer, vehicleid,
        g_MechOffer[customer][moParts], g_MechOffer[customer][moPrice], g_MechOffer[customer][moWorkshop], g_AccountRace[mechanic], g_AccountRace[customer]);
}

// Paid only when the work is done, so a repair broken off half way costs nothing.
forward Mech_FinishRepair(mechanic, customer, vehicleid, parts, price, bool:workshop, mechanic_race, customer_race);
public Mech_FinishRepair(mechanic, customer, vehicleid, parts, price, bool:workshop, mechanic_race, customer_race)
{
    if (mechanic_race != g_AccountRace[mechanic])
    {
        return 1;
    }
    g_MechWorkTimer[mechanic] = 0;
    ClearAnimations(mechanic, SYNC_ALL);
    if (customer_race != g_AccountRace[customer] || !IsValidVehicle(vehicleid))
    {
        Msg_Error(mechanic, "Popravka je prekinuta.");
        return 1;
    }
    new Float:x, Float:y, Float:z;
    GetPlayerPos(mechanic, x, y, z);
    if (GetVehicleDistanceFromPoint(vehicleid, x, y, z) > MECH_REACH)
    {
        Msg_Error(mechanic, "Popravka je prekinuta: vozilo se udaljilo.");
        Msg_Error(customer, "Popravka je prekinuta: vozilo se udaljilo od mehanicara.");
        return 1;
    }
    if (!Money_Take(customer, price, "popravka vozila"))
    {
        Msg_Error(mechanic, "Igrac nema dovoljno novca; popravka nije uradjena.");
        return 1;
    }
    Money_GiveCash(mechanic, price, "mehanicar: popravka", g_Player[customer][pDbId]);
    Mech_ApplyRepair(vehicleid, parts, workshop);
    new what[48];
    Mech_PartsText(parts, workshop, what);
    Msg_Success(customer, "Vozilo je popravljeno (%s) za $%d. Stanje: %d%%.", what, price, Veh_Condition(vehicleid));
    Msg_Success(mechanic, "Popravili ste vozilo (%s) i zaradili $%d.", what, price);
    return 1;
}

stock bool:Mech_OnDialogResponse(playerid, dialogid, response, listitem)
{
    switch (dialogid)
    {
        case DIALOG_MECH_PARTS:
        {
            new customer = g_MechPickTarget[playerid];
            if (!response || !IsPlayerConnected(customer) || !Mech_RequireOnDuty(playerid))
            {
                return true;
            }
            new bool:workshop = g_MechPickWorkshop[playerid];
            if (listitem < 0 || listitem >= (workshop ? sizeof(g_MechShopParts) : sizeof(g_MechFieldParts)))
            {
                return true;
            }
            new vehicleid = Mech_CustomerVehicle(playerid, customer);
            if (vehicleid != 0)
            {
                Mech_SendOffer(playerid, customer, vehicleid, MECH_OFFER_REPAIR, workshop ? g_MechShopParts[listitem] : g_MechFieldParts[listitem],
                    g_MechPickPrice[playerid], workshop);
            }
        }
        case DIALOG_MECH_OFFER:
        {
            new E_MECH_OFFER_KIND:kind = g_MechOffer[playerid][moKind], mechanic = g_MechOffer[playerid][moMechanic];
            g_MechOffer[playerid][moKind] = MECH_OFFER_NONE;
            if (kind == MECH_OFFER_NONE || gettime() > g_MechOffer[playerid][moExpires])
            {
                Msg_Error(playerid, "Ponuda je istekla.");
                return true;
            }
            if (!response)
            {
                Msg_Info(mechanic, "%s je odbio ponudu.", Account_Name(playerid));
                return true;
            }
            if (!Job_IsOnDuty(mechanic, JOB_MECHANIC) || Mech_CustomerVehicle(mechanic, playerid) != g_MechOffer[playerid][moVehicle])
            {
                Msg_Error(playerid, "Mehanicar vise nije pored vaseg vozila.");
                return true;
            }
            if (kind == MECH_OFFER_TUNING)
            {
                Tune_OpenForCustomer(playerid, mechanic);
                return true;
            }
            if (g_MechWorkTimer[mechanic] != 0)
            {
                Msg_Error(playerid, "Mehanicar trenutno radi na drugom vozilu.");
                return true;
            }
            if (g_Player[playerid][pCash] < g_MechOffer[playerid][moPrice])
            {
                Msg_Error(playerid, "Nemate dovoljno novca kod sebe ($%d).", g_MechOffer[playerid][moPrice]);
                Msg_Info(mechanic, "%s nema dovoljno novca za popravku.", Account_Name(playerid));
                return true;
            }
            Mech_StartRepair(mechanic, playerid);
        }
        default:
        {
            return false;
        }
    }
    return true;
}

// Called by the tuning menu for every choice, since the mechanic may walk away or end the shift while it is open.
stock Mech_TuningVehicle(customer, mechanic)
{
    if (!IsPlayerConnected(mechanic) || !Job_IsOnDuty(mechanic, JOB_MECHANIC) || !Mech_InWorkshop(mechanic))
    {
        Msg_Error(customer, "Mehanicar vise nije u radionici; tjuning je zavrsen.");
        return 0;
    }
    if (GetPlayerState(customer) != PLAYER_STATE_DRIVER)
    {
        return 0;
    }
    new vehicleid = GetPlayerVehicleID(customer), Float:x, Float:y, Float:z;
    GetPlayerPos(mechanic, x, y, z);
    if (!Veh_IsOwner(customer, vehicleid) || GetVehicleDistanceFromPoint(vehicleid, x, y, z) > MECH_REACH * 2.0)
    {
        Msg_Error(customer, "Mehanicar vise nije pored vaseg vozila; tjuning je zavrsen.");
        return 0;
    }
    return vehicleid;
}

stock Mech_Call(playerid, const text[])
{
    new now = gettime();
    if (now - g_MechCallLast[playerid] < MECH_CALL_COOLDOWN_S)
    {
        Msg_Error(playerid, "Mehanicare ste vec zvali; sacekajte malo pre novog poziva.");
        return;
    }
    new count = Mech_OnDutyCount();
    if (count == 0)
    {
        Msg_Info(playerid, "Dispecer: nijedan mehanicar trenutno ne radi. Pokusajte kasnije.");
        return;
    }
    g_MechCallAt[playerid] = now;
    g_MechCallLast[playerid] = now;
    new place[ZONE_NAME_LENGTH];
    Hud_PlayerPlace(playerid, place);
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (IsPlayerConnected(i) && Job_IsOnDuty(i, JOB_MECHANIC))
        {
            SendClientMessage(i, COLOR_YELLOW, "[555] %s [%d] (%s): %s", Account_Name(playerid), playerid, place, text);
            Msg_Info(i, "Prihvatanje poziva: /prihvatipoziv %d", playerid);
        }
    }
    Msg_Info(playerid, "Dispecer: poziv je prosledjen mehanicarima (na poslu: %d). Sacekajte da ga neko prihvati.", count);
}

cmd:prihvatipoziv(playerid, params[])
{
    if (!Mech_RequireOnDuty(playerid))
    {
        return 1;
    }
    new caller = Staff_ParseTarget(playerid, params, "/prihvatipoziv [id pozivaoca]", "Ne mozete prihvatiti svoj poziv.");
    if (caller == INVALID_PLAYER_ID)
    {
        return 1;
    }
    if (g_MechCallAt[caller] == 0 || gettime() - g_MechCallAt[caller] > MECH_CALL_S)
    {
        Msg_Error(playerid, "Taj igrac nema otvoren poziv (ili ga je vec preuzeo drugi mehanicar).");
        return 1;
    }
    g_MechCallAt[caller] = 0;
    new Float:x, Float:y, Float:z, name[32];
    GetPlayerPos(caller, x, y, z);
    format(name, sizeof(name), "Poziv: %s", Account_Name(caller));
    Gps_StartAt(playerid, -1, name, x, y, z);
    Msg_Success(caller, "Mehanicar %s je prihvatio vas poziv i dolazi.", Account_Name(playerid));
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (i != playerid && IsPlayerConnected(i) && Job_IsOnDuty(i, JOB_MECHANIC))
        {
            Msg_Info(i, "Poziv igraca %s je preuzeo %s.", Account_Name(caller), Account_Name(playerid));
        }
    }
    return 1;
}

cmd:popravi(playerid, params[])
{
    if (!Mech_RequireOnDuty(playerid))
    {
        return 1;
    }
    new customer, price;
    if (sscanf(params, "ud", customer, price))
    {
        Msg_Usage(playerid, "/popravi [id ili ime vozaca] [cena $%d-%d]", MECH_REPAIR_MIN, MECH_REPAIR_MAX);
        return 1;
    }
    if (!Staff_CheckTarget(playerid, customer, "Svoje vozilo ne popravljate sami; pozovite drugog mehanicara."))
    {
        return 1;
    }
    if (price < MECH_REPAIR_MIN || price > MECH_REPAIR_MAX)
    {
        Msg_Error(playerid, "Cena popravke je od $%d do $%d.", MECH_REPAIR_MIN, MECH_REPAIR_MAX);
        return 1;
    }
    if (g_MechWorkTimer[playerid] != 0)
    {
        Msg_Error(playerid, "Vec radite na jednom vozilu.");
        return 1;
    }
    if (Mech_CustomerVehicle(playerid, customer) == 0)
    {
        return 1;
    }
    new bool:workshop = Mech_InWorkshop(playerid);
    g_MechPickTarget[playerid] = customer;
    g_MechPickPrice[playerid] = price;
    g_MechPickWorkshop[playerid] = workshop;
    if (workshop)
    {
        ShowPlayerDialog(playerid, DIALOG_MECH_PARTS, DIALOG_STYLE_LIST, "Popravka u radionici", "Motor (do 100%)\nKaroserija (ulubljenja, vrata, svetla)\nGume\nSve", "Ponudi", "Odustani");
    }
    else
    {
        ShowPlayerDialog(playerid, DIALOG_MECH_PARTS, DIALOG_STYLE_LIST, "Popravka na terenu", "Motor (do 60%)\nGume\nMotor i gume", "Ponudi", "Odustani");
    }
    return 1;
}

cmd:tjuning(playerid, params[])
{
    if (!Mech_RequireOnDuty(playerid))
    {
        return 1;
    }
    new customer = Staff_ParseTarget(playerid, params, "/tjuning [id ili ime vozaca] (u radionici)", "Svoje vozilo ne tjuningujete sami; zamolite drugog mehanicara.");
    if (customer == INVALID_PLAYER_ID)
    {
        return 1;
    }
    if (!Mech_InWorkshop(playerid))
    {
        Msg_Error(playerid, "Tjuning i farbanje rade se samo u radionici.");
        return 1;
    }
    new vehicleid = Mech_CustomerVehicle(playerid, customer);
    if (vehicleid == 0)
    {
        return 1;
    }
    if (!Veh_IsOwner(customer, vehicleid))
    {
        Msg_Error(playerid, "Tjuning se radi samo na licnom vozilu, dok vlasnik sedi za volanom.");
        return 1;
    }
    Mech_SendOffer(playerid, customer, vehicleid, MECH_OFFER_TUNING, 0, 0, true);
    return 1;
}

cmd:vuci(playerid, params[])
{
    if (!Mech_RequireOnDuty(playerid))
    {
        return 1;
    }
    new tow = GetPlayerVehicleID(playerid);
    if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER || g_JobVehicle[tow] != JOB_MECHANIC || GetVehicleModel(tow) != MECH_VEHICLE_MODEL)
    {
        Msg_Error(playerid, "Vozila se kace iz slepera mehanicara (za volanom).");
        return 1;
    }
    new trailer = GetVehicleTrailer(tow);
    if (trailer != 0 && trailer != INVALID_VEHICLE_ID)
    {
        DetachTrailerFromVehicle(tow);
        Chat_Action(playerid, "otkacinje vozilo sa slepera.");
        return 1;
    }
    new Float:x, Float:y, Float:z, target = INVALID_VEHICLE_ID, Float:best = MECH_TOW_RANGE, world = GetVehicleVirtualWorld(tow);
    GetVehiclePos(tow, x, y, z);
    for (new v = 1; v < MAX_VEHICLES; v++)
    {
        if (v == tow || !IsValidVehicle(v) || GetVehicleVirtualWorld(v) != world)
        {
            continue;
        }
        new Float:distance = GetVehicleDistanceFromPoint(v, x, y, z);
        if (distance < best)
        {
            best = distance;
            target = v;
        }
    }
    if (target == INVALID_VEHICLE_ID)
    {
        Msg_Error(playerid, "Pored slepera (do %.0f m) nema vozila.", MECH_TOW_RANGE);
        return 1;
    }
    new E_VEH_CLASS:class = Veh_Class(GetVehicleModel(target));
    if (class == VEH_CLASS_BOAT || class == VEH_CLASS_AIR || class == VEH_CLASS_OTHER)
    {
        Msg_Error(playerid, "To vozilo ne moze na sleper.");
        return 1;
    }
    if (GetVehicleDriver(target) != INVALID_PLAYER_ID)
    {
        Msg_Error(playerid, "U tom vozilu sedi vozac.");
        return 1;
    }
    // Only a vehicle that cannot drive on its own is towed, so the tow truck never carries off someone's car.
    new Float:health;
    GetVehicleHealth(target, health);
    if (health >= VEH_ENGINE_WEAK_HEALTH && !(Fuel_IsUsed(target) && g_VehFuel[target] <= 0.0))
    {
        Msg_Error(playerid, "Na sleper se kace samo pokvarena vozila (stanje ispod %d%%) ili vozila bez goriva.", floatround((VEH_ENGINE_WEAK_HEALTH - VEH_HEALTH_FIRE) * 100.0 / (VEH_HEALTH_FULL - VEH_HEALTH_FIRE)));
        return 1;
    }
    AttachTrailerToVehicle(target, tow);
    Chat_Action(playerid, "kaci vozilo na sleper.");
    Msg_Info(playerid, "Otkacinjanje: ponovo /vuci.");
    return 1;
}

// The workshop's own repair for when no mechanic is at work; the vehicle stands still while it lasts.
cmd:servis(playerid, params[])
{
    if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
    {
        Msg_Error(playerid, "Sedite za volan vozila koje servisirate.");
        return 1;
    }
    if (!Mech_InWorkshop(playerid))
    {
        Msg_Error(playerid, "Servis radi samo u mehanicarskoj radionici (/gps).");
        return 1;
    }
    new vehicleid = GetPlayerVehicleID(playerid), count = Mech_OnDutyCount();
    if (!Fuel_IsUsed(vehicleid))
    {
        Msg_Error(playerid, "Ovo vozilo se ne servisira.");
        return 1;
    }
    if (count > 0)
    {
        Msg_Error(playerid, "Mehanicari su na poslu (%d); pozovite ih: /pozovi 555 [opis].", count);
        return 1;
    }
    if (g_MechServiceTimer[playerid] != 0)
    {
        Msg_Error(playerid, "Servis je vec u toku.");
        return 1;
    }
    if (g_Player[playerid][pCash] < MECH_SERVICE_PRICE)
    {
        Msg_Error(playerid, "Servis kosta $%d (gotovinom).", MECH_SERVICE_PRICE);
        return 1;
    }
    Veh_SetParam(vehicleid, VEH_PARAM_ENGINE, false);
    TogglePlayerControllable(playerid, false);
    GameTextForPlayer(playerid, "~w~servis...", MECH_SERVICE_MS, 6);
    Chat_Action(playerid, "predaje vozilo na servis.");
    g_MechServiceTimer[playerid] = SetTimerEx("Mech_FinishService", MECH_SERVICE_MS, false, "ddd", playerid, vehicleid, g_AccountRace[playerid]);
    return 1;
}

forward Mech_FinishService(playerid, vehicleid, race);
public Mech_FinishService(playerid, vehicleid, race)
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }
    g_MechServiceTimer[playerid] = 0;
    if (!g_Staff[playerid][sFrozen])
    {
        TogglePlayerControllable(playerid, true);
    }
    if (GetPlayerVehicleID(playerid) != vehicleid || !Money_Take(playerid, MECH_SERVICE_PRICE, "servis vozila"))
    {
        Msg_Error(playerid, "Servis nije uradjen.");
        return 1;
    }
    RepairVehicle(vehicleid);
    g_VehStallAt[vehicleid] = 0;
    Veh_SaveIfPersonal(vehicleid);
    Msg_Success(playerid, "Vozilo je servisirano za $%d (stanje 100%%).", MECH_SERVICE_PRICE);
    return 1;
}
