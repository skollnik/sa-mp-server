#define MECH_VEHICLE_MODEL      (525)
#define MECH_VEHICLE_RESPAWN_S  (600)
// Around a service point; small, so a car on the street next to it is not repaired. A bigger service gets more points.
#define MECH_WORKSHOP_RADIUS    (8.0)
// Where the mechanic stands in front of the bonnet, past the front of the vehicle.
#define MECH_BONNET_GAP         (0.6)
#define MECH_BONNET_REACH       (1.5)
#define MECH_REACH              (6.0)
#define MECH_SERVICE_MS         (30000)
#define MECH_OFFER_S            (60)
#define MECH_CALL_S             (600)
#define MECH_CALL_COOLDOWN_S    (60)
#define MECH_TOW_RANGE          (8.0)
// A field repair only gets the vehicle to the workshop; everything above this is workshop work.
#define MECH_FIELD_PERCENT      (60)
#define MECH_REPAIR_BASE_MS     (5000)
#define MECH_REPAIR_PERCENT_MS  (500)
#define MECH_REPAIR_TYRE_MS     (2000)
#define MECH_REPAIR_MAX_MS      (45000)
// A vehicle moved further than this from where the repair began breaks the repair off.
#define MECH_REPAIR_DRIFT       (1.5)

#define MECH_BODY_LIGHTS        (0)
#define MECH_BODY_PANEL         (1)
#define MECH_BODY_DOOR          (2)

// The parking next to the workshop garages in Willowfield.
static const Float:g_MechVehicleSpots[][4] =
{
    { 1945.08, -1974.86, 13.42, 272.0 },
    { 1945.14, -1982.94, 13.42, 270.0 },
    { 1944.94, -1990.79, 13.42, 269.4 },
    { 1944.90, -1998.61, 13.43, 267.3 }
};

// The order in which a partial workshop repair fixes visible damage, what the driver notices most first:
// lights, windscreen, bonnet, bumpers, front doors, boot, then the corner panels. Each row is a kind and its bit shift.
static const g_MechBodyParts[][2] =
{
    { MECH_BODY_LIGHTS, 0 }, { MECH_BODY_PANEL, 16 }, { MECH_BODY_DOOR, 0 }, { MECH_BODY_PANEL, 20 },
    { MECH_BODY_PANEL, 24 }, { MECH_BODY_DOOR, 16 }, { MECH_BODY_DOOR, 24 }, { MECH_BODY_DOOR, 8 },
    { MECH_BODY_PANEL, 0 }, { MECH_BODY_PANEL, 4 }, { MECH_BODY_PANEL, 8 }, { MECH_BODY_PANEL, 12 }
};

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
    moFrom,
    moTarget,
    moTyres,
    moPrice,
    bool:moShop,
    moExpires
}

// A running repair, indexed by the mechanic; mwTimer is 0 when there is none.
enum E_MECH_WORK
{
    mwTimer,
    mwCustomer,
    mwCustomerRace,
    mwVehicle,
    mwFrom,
    mwTarget,
    mwTyres,
    mwPrice,
    bool:mwShop,
    mwEndsAt,
    Float:mwHealth,
    Float:mwX,
    Float:mwY,
    Float:mwZ
}

new g_MechOffer[MAX_PLAYERS][E_MECH_OFFER];
new g_MechWork[MAX_PLAYERS][E_MECH_WORK];
// Running workshop service (driver); 0 when none.
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
        Veh_SetLabel(vehicleid, "[MEHANICAR]", COLOR_GREY);
    }
}

stock Mech_OnPlayerConnect(playerid)
{
    static const empty_offer[E_MECH_OFFER], empty_work[E_MECH_WORK];
    g_MechOffer[playerid] = empty_offer;
    g_MechWork[playerid] = empty_work;
    g_MechServiceTimer[playerid] = 0;
    g_MechCallAt[playerid] = 0;
    g_MechCallLast[playerid] = 0;
}

stock Mech_OnPlayerDisconnect(playerid)
{
    if (g_MechWork[playerid][mwTimer] != 0)
    {
        KillTimer(g_MechWork[playerid][mwTimer]);
        g_MechWork[playerid][mwTimer] = 0;
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

// Cars and motorbikes are serviced at a "servis" point or an inspection that is also a service ("pregledservis");
// aircraft and boats at their own service, since they cannot get there. The workshop point itself is only the mechanics' /oprema.
static stock bool:Mech_AtServicePlace(playerid, vehicleid)
{
    new E_POINT_TYPE:type = Veh_ServicePoint(vehicleid);
    if (type != POINT_SERVICE)
    {
        return Point_IsWithin(playerid, type, POINT_HANGAR_RADIUS);
    }
    return Point_IsWithin(playerid, POINT_SERVICE, MECH_WORKSHOP_RADIUS) || Point_IsWithin(playerid, POINT_INSPECTION_SERVICE, MECH_WORKSHOP_RADIUS);
}

// Engine work on a car, van or truck is done from the front, under the open bonnet.
static stock bool:Mech_NeedsBonnet(vehicleid)
{
    new E_VEH_CLASS:class = Veh_Class(GetVehicleModel(vehicleid));
    return class == VEH_CLASS_CAR || class == VEH_CLASS_VAN || class == VEH_CLASS_TRUCK;
}

static stock Mech_BonnetSpot(vehicleid, &Float:x, &Float:y)
{
    new Float:z, Float:angle, Float:size_x, Float:size_y, Float:size_z;
    GetVehiclePos(vehicleid, x, y, z);
    GetVehicleZAngle(vehicleid, angle);
    GetVehicleModelInfo(GetVehicleModel(vehicleid), VEHICLE_MODEL_INFO_SIZE, size_x, size_y, size_z);
    new Float:distance = size_y / 2.0 + MECH_BONNET_GAP;
    x -= distance * floatsin(angle, degrees);
    y += distance * floatcos(angle, degrees);
}

// The mechanic walks to the bonnet on foot, so nobody is ever placed into a wall the vehicle is parked against.
static stock bool:Mech_AtBonnet(mechanic, vehicleid)
{
    new Float:x, Float:y, Float:px, Float:py, Float:pz;
    Mech_BonnetSpot(vehicleid, x, y);
    GetPlayerPos(mechanic, px, py, pz);
    return floatsqroot((px - x) * (px - x) + (py - y) * (py - y)) <= MECH_BONNET_REACH;
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

static stock Mech_TyresPopped(vehicleid)
{
    new VEHICLE_PANEL_STATUS:panels, VEHICLE_DOOR_STATUS:doors, VEHICLE_LIGHT_STATUS:lights, VEHICLE_TYRE_STATUS:tyres, count;
    GetVehicleDamageStatus(vehicleid, panels, doors, lights, tyres);
    for (new bit = 0; bit < 4; bit++)
    {
        if (_:tyres & (1 << bit))
        {
            count++;
        }
    }
    return count;
}

static stock bool:Mech_BodyPartDamaged(part, panels, doors, lights)
{
    new shift = g_MechBodyParts[part][1];
    switch (g_MechBodyParts[part][0])
    {
        case MECH_BODY_LIGHTS: return lights != 0;
        case MECH_BODY_PANEL: return ((panels >>> shift) & 0xF) != 0;
    }
    // A door byte: bit 1 is a swinging door, 2 a dented one, 4 a missing one.
    return ((doors >>> shift) & 0x6) != 0;
}

// Fixes this share (0.0 to 1.0) of the damaged body parts, in the order of g_MechBodyParts.
static stock Mech_RepairBody(vehicleid, Float:share)
{
    new VEHICLE_PANEL_STATUS:panel_status, VEHICLE_DOOR_STATUS:door_status, VEHICLE_LIGHT_STATUS:light_status, VEHICLE_TYRE_STATUS:tyres;
    GetVehicleDamageStatus(vehicleid, panel_status, door_status, light_status, tyres);
    new panels = _:panel_status, doors = _:door_status, lights = _:light_status, damaged;
    for (new part = 0; part < sizeof(g_MechBodyParts); part++)
    {
        if (Mech_BodyPartDamaged(part, panels, doors, lights))
        {
            damaged++;
        }
    }
    new fix = floatround(damaged * share, floatround_ceil);
    for (new part = 0; part < sizeof(g_MechBodyParts) && fix > 0; part++)
    {
        if (!Mech_BodyPartDamaged(part, panels, doors, lights))
        {
            continue;
        }
        new shift = g_MechBodyParts[part][1];
        switch (g_MechBodyParts[part][0])
        {
            case MECH_BODY_LIGHTS: lights = 0;
            case MECH_BODY_PANEL: panels &= ~(0xF << shift);
            default: doors &= ~(0xFF << shift);
        }
        fix--;
    }
    UpdateVehicleDamageStatus(vehicleid, VEHICLE_PANEL_STATUS:panels, VEHICLE_DOOR_STATUS:doors, VEHICLE_LIGHT_STATUS:lights, tyres);
}

// How much of the visible damage a workshop repair from one percent to another fixes: the same share as of the engine.
static stock Float:Mech_BodyShare(from, target)
{
    if (target >= 100 || from >= 100)
    {
        return 1.0;
    }
    return target > from ? float(target - from) / float(100 - from) : 0.0;
}

// Tells the mechanic to go to the bonnet when the repair needs it; returns true when it can start.
static stock bool:Mech_CheckBonnet(mechanic, vehicleid, bool:engine)
{
    if (!engine || !Mech_NeedsBonnet(vehicleid) || Mech_AtBonnet(mechanic, vehicleid))
    {
        return true;
    }
    Msg_Error(mechanic, "Motor se popravlja ispred vozila: stanite ispred haube.");
    return false;
}

static stock Mech_Price(percent, tyres, bool:shop)
{
    return percent * (shop ? MECH_PRICE_SHOP_PERCENT : MECH_PRICE_FIELD_PERCENT) + tyres * MECH_PRICE_TYRE;
}

static stock Mech_RepairMs(percent, tyres)
{
    new ms = MECH_REPAIR_BASE_MS + percent * MECH_REPAIR_PERCENT_MS + tyres * MECH_REPAIR_TYRE_MS;
    return ms > MECH_REPAIR_MAX_MS ? MECH_REPAIR_MAX_MS : ms;
}

static stock Mech_ApplyRepair(vehicleid, from, target, bool:shop)
{
    new Float:health, Float:repaired = Veh_HealthAt(target);
    GetVehicleHealth(vehicleid, health);
    if (health < repaired)
    {
        SetVehicleHealth(vehicleid, repaired);
    }
    g_VehStallAt[vehicleid] = 0;
    if (shop)
    {
        Mech_RepairBody(vehicleid, Mech_BodyShare(from, target));
    }
    new VEHICLE_PANEL_STATUS:panels, VEHICLE_DOOR_STATUS:doors, VEHICLE_LIGHT_STATUS:lights, VEHICLE_TYRE_STATUS:tyres;
    GetVehicleDamageStatus(vehicleid, panels, doors, lights, tyres);
    UpdateVehicleDamageStatus(vehicleid, panels, doors, lights, VEHICLE_TYRE_STATUS:0);
    Veh_SaveIfPersonal(vehicleid);
}

static stock Mech_SendOffer(mechanic, customer, vehicleid, E_MECH_OFFER_KIND:kind, target = 0, tyres = 0, price = 0, bool:shop = false)
{
    g_MechOffer[customer][moKind] = kind;
    g_MechOffer[customer][moMechanic] = mechanic;
    g_MechOffer[customer][moVehicle] = vehicleid;
    g_MechOffer[customer][moFrom] = Veh_Condition(vehicleid);
    g_MechOffer[customer][moTarget] = target;
    g_MechOffer[customer][moTyres] = tyres;
    g_MechOffer[customer][moPrice] = price;
    g_MechOffer[customer][moShop] = shop;
    g_MechOffer[customer][moExpires] = gettime() + MECH_OFFER_S;

    new text[400];
    if (kind == MECH_OFFER_REPAIR)
    {
        new from = Veh_Condition(vehicleid), body[48], percent = target > from ? target - from : 0;
        if (!shop)
        {
            strcopy(body, "samo u servisu");
        }
        else
        {
            format(body, sizeof(body), "%d%% ostecenja", floatround(Mech_BodyShare(from, target) * 100.0));
        }
        format(text, sizeof(text), "Mehanicar %s nudi popravku vaseg vozila.\n\nStanje: %d%% -> %d%%\nKaroserija: %s\nPukle gume: %d\nCena: $%d\n\nPopravka traje oko %d sekundi; ne pomerajte vozilo.",
            Account_Name(mechanic), from, percent > 0 ? target : from, body, tyres, price, Mech_RepairMs(percent, tyres) / 1000);
    }
    else
    {
        format(text, sizeof(text), "Mehanicar %s nudi tjuning i farbanje vaseg vozila.\n\nDeo se prvo proba (Space), a placa se tek kad ga izaberete jos jednom.\nCena pise pored dela. Skidanje delova je besplatno.",
            Account_Name(mechanic));
    }
    ShowPlayerDialog(customer, DIALOG_MECH_OFFER, DIALOG_STYLE_MSGBOX, "Mehanicar", text, "Prihvati", "Odbij");
    Msg_Info(mechanic, "Ponuda je poslata igracu %s (vazi %d sekundi).", Account_Name(customer), MECH_OFFER_S);
}

// Unfreezes the mechanic and closes the bonnet; the caller tells why the work ended.
static stock Mech_EndWork(mechanic)
{
    if (g_MechWork[mechanic][mwTimer] != 0)
    {
        KillTimer(g_MechWork[mechanic][mwTimer]);
        g_MechWork[mechanic][mwTimer] = 0;
    }
    ClearAnimations(mechanic, SYNC_ALL);
    if (!g_Staff[mechanic][sFrozen])
    {
        TogglePlayerControllable(mechanic, true);
    }
    if (IsValidVehicle(g_MechWork[mechanic][mwVehicle]))
    {
        Veh_SetParam(g_MechWork[mechanic][mwVehicle], VEH_PARAM_BONNET, false);
    }
}

static stock Mech_StopRepair(mechanic, const reason[])
{
    new customer = g_MechWork[mechanic][mwCustomer];
    Mech_EndWork(mechanic);
    Msg_Error(mechanic, "Popravka je prekinuta: %s", reason);
    if (g_MechWork[mechanic][mwCustomerRace] == g_AccountRace[customer])
    {
        Msg_Error(customer, "Popravka je prekinuta: %s Nista niste platili.", reason);
        HideGameTextForPlayer(customer, 6);
    }
}

// The mechanic is frozen for the whole repair, so the animation cannot be walked out of.
static stock Mech_StartRepair(mechanic, customer)
{
    new vehicleid = g_MechOffer[customer][moVehicle], from = Veh_Condition(vehicleid), target = g_MechOffer[customer][moTarget];
    new tyres = g_MechOffer[customer][moTyres], percent = target > from ? target - from : 0, ms = Mech_RepairMs(percent, tyres);
    new Float:health, Float:armour, Float:px, Float:py, Float:pz;
    GetPlayerHealth(mechanic, health);
    GetPlayerArmour(mechanic, armour);
    GetPlayerPos(mechanic, px, py, pz);

    g_MechWork[mechanic][mwCustomer] = customer;
    g_MechWork[mechanic][mwCustomerRace] = g_AccountRace[customer];
    g_MechWork[mechanic][mwVehicle] = vehicleid;
    g_MechWork[mechanic][mwFrom] = from;
    g_MechWork[mechanic][mwTarget] = target;
    g_MechWork[mechanic][mwTyres] = tyres;
    g_MechWork[mechanic][mwPrice] = g_MechOffer[customer][moPrice];
    g_MechWork[mechanic][mwShop] = g_MechOffer[customer][moShop];
    g_MechWork[mechanic][mwEndsAt] = GetTickCount() + ms;
    g_MechWork[mechanic][mwHealth] = health + armour;
    GetVehiclePos(vehicleid, g_MechWork[mechanic][mwX], g_MechWork[mechanic][mwY], g_MechWork[mechanic][mwZ]);

    TogglePlayerControllable(mechanic, false);
    // Already within reach of the bonnet, so the short step onto its middle cannot end in a wall.
    if (percent > 0 && Mech_NeedsBonnet(vehicleid))
    {
        Mech_BonnetSpot(vehicleid, px, py);
        AC_SetPlayerPos(mechanic, px, py, pz);
    }
    SetPlayerFacingAngle(mechanic, atan2(g_MechWork[mechanic][mwY] - py, g_MechWork[mechanic][mwX] - px) - 90.0);
    // Standing bent over the open engine; changing only tyres is done crouching by the wheel.
    if (percent > 0)
    {
        Veh_SetParam(vehicleid, VEH_PARAM_BONNET, true);
        ApplyAnimation(mechanic, "BD_FIRE", "null", 4.1, false, false, false, false, 0, SYNC_NONE);
        ApplyAnimation(mechanic, "BD_FIRE", "wash_up", 4.1, true, false, false, false, 0, SYNC_ALL);
        Chat_Action(mechanic, "otvara haubu i radi na motoru.");
    }
    else
    {
        ApplyAnimation(mechanic, "BOMBER", "BOM_Plant_Loop", 4.1, true, false, false, false, 0, SYNC_ALL);
        Chat_Action(mechanic, "menja gume na vozilu.");
    }
    GameTextForPlayer(customer, "~w~popravka...", ms, 6);
    g_MechWork[mechanic][mwTimer] = SetTimerEx("Mech_RepairTick", 1000, true, "dd", mechanic, g_AccountRace[mechanic]);
}

// Paid only when the work is done, so a repair broken off half way costs nothing.
static stock Mech_FinishRepair(mechanic)
{
    new customer = g_MechWork[mechanic][mwCustomer], vehicleid = g_MechWork[mechanic][mwVehicle], price = g_MechWork[mechanic][mwPrice];
    Mech_EndWork(mechanic);
    if (!Money_Take(customer, price, "popravka vozila"))
    {
        Msg_Error(mechanic, "Igrac nema dovoljno novca; popravka nije uradjena.");
        return;
    }
    new earned = price * MECH_SHARE / 100;
    Money_GiveCash(mechanic, earned, "mehanicar: popravka", g_Player[customer][pDbId]);
    Mech_ApplyRepair(vehicleid, g_MechWork[mechanic][mwFrom], g_MechWork[mechanic][mwTarget], g_MechWork[mechanic][mwShop]);
    Msg_Success(customer, "Vozilo je popravljeno za $%d. Stanje: %d%%.", price, Veh_Condition(vehicleid));
    Msg_Success(mechanic, "Popravka je zavrsena: naplaceno $%d, vasa zarada $%d (ostalo su delovi).", price, earned);
}

forward Mech_RepairTick(mechanic, race);
public Mech_RepairTick(mechanic, race)
{
    if (race != g_AccountRace[mechanic])
    {
        return 1;
    }
    new customer = g_MechWork[mechanic][mwCustomer], vehicleid = g_MechWork[mechanic][mwVehicle];
    if (g_MechWork[mechanic][mwCustomerRace] != g_AccountRace[customer] || !IsValidVehicle(vehicleid))
    {
        Mech_StopRepair(mechanic, "vozac je otisao.");
        return 1;
    }
    if (!Job_IsOnDuty(mechanic, JOB_MECHANIC))
    {
        Mech_StopRepair(mechanic, "mehanicar je zavrsio smenu.");
        return 1;
    }
    new Float:health, Float:armour;
    GetPlayerHealth(mechanic, health);
    GetPlayerArmour(mechanic, armour);
    if (health + armour < g_MechWork[mechanic][mwHealth] - 0.5)
    {
        Mech_StopRepair(mechanic, "mehanicar je povredjen.");
        return 1;
    }
    if (GetVehicleDistanceFromPoint(vehicleid, g_MechWork[mechanic][mwX], g_MechWork[mechanic][mwY], g_MechWork[mechanic][mwZ]) > MECH_REPAIR_DRIFT)
    {
        Mech_StopRepair(mechanic, "vozilo se pomerilo.");
        return 1;
    }
    if (GetTickCount() >= g_MechWork[mechanic][mwEndsAt])
    {
        Mech_FinishRepair(mechanic);
    }
    return 1;
}

stock bool:Mech_OnDialogResponse(playerid, dialogid, response, listitem)
{
    #pragma unused listitem
    if (dialogid == DIALOG_MECH_SERVICE)
    {
        if (response)
        {
            Mech_StartService(playerid);
        }
        return true;
    }
    if (dialogid != DIALOG_MECH_OFFER)
    {
        return false;
    }
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
    if (g_MechWork[mechanic][mwTimer] != 0)
    {
        Msg_Error(playerid, "Mehanicar trenutno radi na drugom vozilu.");
        return true;
    }
    // The price was set for the vehicle as it was; a crash after the offer would otherwise be repaired for free.
    if (Veh_Condition(g_MechOffer[playerid][moVehicle]) != g_MechOffer[playerid][moFrom] || Mech_TyresPopped(g_MechOffer[playerid][moVehicle]) != g_MechOffer[playerid][moTyres])
    {
        Msg_Error(playerid, "Stanje vozila se promenilo posle ponude; mehanicar mora da da novu ponudu.");
        Msg_Info(mechanic, "Stanje vozila igraca %s se promenilo; dajte novu ponudu (/popravi).", Account_Name(playerid));
        return true;
    }
    if (g_Player[playerid][pCash] < g_MechOffer[playerid][moPrice])
    {
        Msg_Error(playerid, "Nemate dovoljno novca kod sebe ($%d).", g_MechOffer[playerid][moPrice]);
        Msg_Info(mechanic, "%s nema dovoljno novca za popravku.", Account_Name(playerid));
        return true;
    }
    if (!Mech_CheckBonnet(mechanic, g_MechOffer[playerid][moVehicle], g_MechOffer[playerid][moTarget] > g_MechOffer[playerid][moFrom]))
    {
        Msg_Error(playerid, "Mehanicar mora da stoji ispred haube; prihvatite ponudu ponovo kad stane tamo.");
        return true;
    }
    Mech_StartRepair(mechanic, playerid);
    return true;
}

// Called by the tuning menu for every choice, since the mechanic may walk away or end the shift while it is open.
stock Mech_TuningVehicle(customer, mechanic)
{
    if (GetPlayerState(customer) != PLAYER_STATE_DRIVER)
    {
        return 0;
    }
    if (!IsPlayerConnected(mechanic) || !Job_IsOnDuty(mechanic, JOB_MECHANIC))
    {
        Msg_Error(customer, "Mehanicar vise nije na poslu; tjuning je zavrsen.");
        return 0;
    }
    new vehicleid = GetPlayerVehicleID(customer), Float:x, Float:y, Float:z;
    GetPlayerPos(mechanic, x, y, z);
    if (!Veh_IsOwner(customer, vehicleid) || !Point_IsNear(customer, POINT_TUNING) || GetVehicleDistanceFromPoint(vehicleid, x, y, z) > MECH_REACH * 2.0)
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
        Msg_Info(playerid, "Dispecer: nijedan mehanicar sada ne radi. Vozilo mozete sami popraviti u servisu vozila: /servis.");
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

// The price is fixed per percent: in the field the engine goes to 60%, in the workshop to the percent the driver asks for.
cmd:popravi(playerid, params[])
{
    if (!Mech_RequireOnDuty(playerid))
    {
        return 1;
    }
    new customer, asked;
    if (sscanf(params, "uI(100)", customer, asked))
    {
        Msg_Usage(playerid, "/popravi [id ili ime vozaca] [do koliko %, samo u servisu; bez broja do 100%]");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, customer, "Svoje vozilo ne popravljate sami; pozovite drugog mehanicara."))
    {
        return 1;
    }
    if (g_MechWork[playerid][mwTimer] != 0)
    {
        Msg_Error(playerid, "Vec radite na jednom vozilu.");
        return 1;
    }
    new vehicleid = Mech_CustomerVehicle(playerid, customer);
    if (vehicleid == 0)
    {
        return 1;
    }
    new bool:shop = Mech_AtServicePlace(playerid, vehicleid), from = Veh_Condition(vehicleid), tyres = Mech_TyresPopped(vehicleid);
    if (shop && (asked < 1 || asked > 100))
    {
        Msg_Error(playerid, "Procenat je od 1 do 100.");
        return 1;
    }
    new target = shop ? asked : MECH_FIELD_PERCENT, percent = target > from ? target - from : 0;
    if (percent == 0 && tyres == 0)
    {
        if (shop)
        {
            Msg_Error(playerid, "Vozilo je vec na %d%%; upisite veci procenat (do 100).", from);
        }
        else
        {
            Msg_Error(playerid, "Na terenu se popravlja do %d%%, a vozilo je na %d%%. Za vise mora u servis.", MECH_FIELD_PERCENT, from);
        }
        return 1;
    }
    if (!Mech_CheckBonnet(playerid, vehicleid, percent > 0))
    {
        return 1;
    }
    if (!shop && asked != 100 && asked != MECH_FIELD_PERCENT)
    {
        Msg_Info(playerid, "Na terenu se vozilo uvek popravlja do %d%%; procenat se bira u servisu.", MECH_FIELD_PERCENT);
    }
    Mech_SendOffer(playerid, customer, vehicleid, MECH_OFFER_REPAIR, percent > 0 ? target : from, tyres, Mech_Price(percent, tyres, shop), shop);
    return 1;
}

cmd:tune(playerid, params[])
{
    if (!Mech_RequireOnDuty(playerid))
    {
        return 1;
    }
    new customer = Staff_ParseTarget(playerid, params, "/tune [id ili ime vozaca] (vozilo stoji na mestu za tjuning)", "Svoje vozilo ne tjuningujete sami; zamolite drugog mehanicara.");
    if (customer == INVALID_PLAYER_ID)
    {
        return 1;
    }
    new vehicleid = Mech_CustomerVehicle(playerid, customer);
    if (vehicleid == 0)
    {
        return 1;
    }
    // On its own spot only, so a car is never tuned on the street next to the workshop.
    if (!Point_IsNear(customer, POINT_TUNING))
    {
        Msg_Error(playerid, "Tjuning i farbanje rade se samo kad vozilo stoji na mestu za tjuning (/gps).");
        return 1;
    }
    if (!Veh_IsOwner(customer, vehicleid))
    {
        Msg_Error(playerid, "Tjuning se radi samo na licnom vozilu, dok vlasnik sedi za volanom.");
        return 1;
    }
    Mech_SendOffer(playerid, customer, vehicleid, MECH_OFFER_TUNING);
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

static stock Mech_ServicePrice(vehicleid)
{
    return (100 - Veh_Condition(vehicleid)) * MECH_SERVICE_PERCENT + Mech_TyresPopped(vehicleid) * MECH_PRICE_TYRE;
}

// The workshop's own repair: always to 100%, dearer than a mechanic, for when no mechanic is at work.
// A job vehicle is the company's own, so its driver on duty gets it free and without waiting for mechanics to log off.
// Returns the price (0 for a job vehicle) or -1 with the reason told.
static stock Mech_ServiceCheck(playerid)
{
    if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
    {
        Msg_Error(playerid, "Sedite za volan vozila koje servisirate.");
        return -1;
    }
    new vehicleid = GetPlayerVehicleID(playerid);
    if (!Fuel_IsUsed(vehicleid))
    {
        Msg_Error(playerid, "Ovo vozilo se ne servisira.");
        return -1;
    }
    if (!Mech_AtServicePlace(playerid, vehicleid))
    {
        new E_POINT_TYPE:type = Veh_ServicePoint(vehicleid);
        if (type == POINT_SERVICE)
        {
            Msg_Error(playerid, "Ovo vozilo se servisira u servisu vozila ili na tehnickom pregledu sa servisom (/gps).");
        }
        else
        {
            Msg_Error(playerid, "Ovo vozilo se servisira u: %s (/gps).", g_PointInfo[type][piName]);
        }
        return -1;
    }
    if (g_MechServiceTimer[playerid] != 0)
    {
        Msg_Error(playerid, "Servis je vec u toku.");
        return -1;
    }
    new bool:company = g_JobVehicle[vehicleid] != JOB_NONE && Veh_HasKey(playerid, vehicleid), count = Mech_OnDutyCount();
    if (!company && count > 0)
    {
        Msg_Error(playerid, "Mehanicari su na poslu (%d); pozovite ih: /pozovi 555 [opis].", count);
        return -1;
    }
    if (Veh_Condition(vehicleid) >= 100 && Mech_TyresPopped(vehicleid) == 0)
    {
        Msg_Info(playerid, "Vozilo je ispravno (100%) i ne treba mu servis.");
        return -1;
    }
    new price = company ? 0 : Mech_ServicePrice(vehicleid);
    if (g_Player[playerid][pCash] < price)
    {
        Msg_Error(playerid, "Servis ovog vozila kosta $%d (gotovinom).", price);
        return -1;
    }
    return price;
}

cmd:servis(playerid, params[])
{
    new price = Mech_ServiceCheck(playerid);
    if (price == -1)
    {
        return 1;
    }
    new vehicleid = GetPlayerVehicleID(playerid), text[256], cost[48];
    if (price == 0)
    {
        strcopy(cost, "besplatno (vozilo firme)");
    }
    else
    {
        format(cost, sizeof(cost), "$%d (gotovinom)", price);
    }
    format(text, sizeof(text), "Servis popravlja vozilo do 100%%, sa karoserijom i gumama.\n\nStanje sada: %d%%\nPukle gume: %d\nCena: %s\n\nServis traje %d sekundi; vozilo ne mozete pomerati.",
        Veh_Condition(vehicleid), Mech_TyresPopped(vehicleid), cost, MECH_SERVICE_MS / 1000);
    ShowPlayerDialog(playerid, DIALOG_MECH_SERVICE, DIALOG_STYLE_MSGBOX, "Servis vozila", text, "Servisiraj", "Odustani");
    return 1;
}

// Everything is checked again, since the player could have driven off or lost money while the dialog was open.
static stock Mech_StartService(playerid)
{
    new price = Mech_ServiceCheck(playerid);
    if (price == -1)
    {
        return;
    }
    new vehicleid = GetPlayerVehicleID(playerid);
    Veh_SetParam(vehicleid, VEH_PARAM_ENGINE, false);
    TogglePlayerControllable(playerid, false);
    GameTextForPlayer(playerid, "~w~servis...", MECH_SERVICE_MS, 6);
    Chat_Action(playerid, "predaje vozilo na servis.");
    g_MechServiceTimer[playerid] = SetTimerEx("Mech_FinishService", MECH_SERVICE_MS, false, "dddd", playerid, vehicleid, price, g_AccountRace[playerid]);
}

forward Mech_FinishService(playerid, vehicleid, price, race);
public Mech_FinishService(playerid, vehicleid, price, race)
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
    if (GetPlayerVehicleID(playerid) != vehicleid || (price > 0 && !Money_Take(playerid, price, "servis vozila")))
    {
        Msg_Error(playerid, "Servis nije uradjen.");
        return 1;
    }
    RepairVehicle(vehicleid);
    g_VehStallAt[vehicleid] = 0;
    Veh_SaveIfPersonal(vehicleid);
    if (price > 0)
    {
        Msg_Success(playerid, "Vozilo je servisirano za $%d (stanje 100%%).", price);
    }
    else
    {
        Msg_Success(playerid, "Vozilo firme je servisirano (stanje 100%).");
    }
    return 1;
}
