#define SAFETY_UPDATE_MS        (200)
// A hit counts as a crash only above this loss in one step and from this speed, so shots and scrapes do not hurt.
#define SAFETY_CRASH_HP         (50.0)
#define SAFETY_CRASH_MIN_KMH    (30)
#define SAFETY_EJECT_HP         (250.0)
#define SAFETY_SCHOOL_HELMET    (18645)

enum E_SAFETY_GEAR
{
    SAFETY_GEAR_NONE,
    SAFETY_GEAR_BELT,
    SAFETY_GEAR_HELMET
}

new bool:g_SafetyBelt[MAX_PLAYERS];
// A helmet the driving school lends for the motorbike test; worn in the head accessory slot.
new bool:g_SafetySchoolLent[MAX_PLAYERS];
new bool:g_SafetySchoolOn[MAX_PLAYERS];
new g_SafetyVehicle[MAX_PLAYERS];
new Float:g_SafetyLastHealth[MAX_PLAYERS];
new g_SafetyLastSpeed[MAX_PLAYERS];
new g_SafetyTimer;

stock Safety_Init()
{
    g_SafetyTimer = SetTimer("Safety_Tick", SAFETY_UPDATE_MS, true);
}

stock Safety_Exit()
{
    KillTimer(g_SafetyTimer);
}

stock Safety_OnPlayerConnect(playerid)
{
    g_SafetyBelt[playerid] = false;
    g_SafetySchoolLent[playerid] = false;
    g_SafetySchoolOn[playerid] = false;
    g_SafetyVehicle[playerid] = INVALID_VEHICLE_ID;
}

stock E_SAFETY_GEAR:Safety_GearFor(model)
{
    switch (Veh_Class(model))
    {
        case VEH_CLASS_CAR, VEH_CLASS_VAN, VEH_CLASS_TRUCK, VEH_CLASS_AIR: return SAFETY_GEAR_BELT;
        case VEH_CLASS_MOTO: return SAFETY_GEAR_HELMET;
        default: return SAFETY_GEAR_NONE;
    }
    return SAFETY_GEAR_NONE;
}

stock bool:Safety_IsHelmet(E_ITEM:item)
{
    return item == ITEM_HELMET_BLACK || item == ITEM_HELMET_RED || item == ITEM_HELMET_WHITE;
}

stock bool:Safety_WearsHelmet(playerid)
{
    if (g_SafetySchoolOn[playerid])
    {
        return true;
    }
    new index = g_AccWorn[playerid][ACC_PART_HEAD];
    return index != -1 && Safety_IsHelmet(g_AccInfo[index][aiItem]);
}

// True when the player has what the vehicle asks for (or it asks for nothing: bicycles, boats).
stock bool:Safety_IsProtected(playerid, vehicleid)
{
    switch (Safety_GearFor(GetVehicleModel(vehicleid)))
    {
        case SAFETY_GEAR_BELT: return g_SafetyBelt[playerid];
        case SAFETY_GEAR_HELMET: return Safety_WearsHelmet(playerid);
        default: return true;
    }
    return true;
}

// The belt comes off with every seat change, exit and death; a new seat always starts unbuckled.
stock Safety_OnPlayerStateChange(playerid)
{
    g_SafetyBelt[playerid] = false;
    g_SafetyVehicle[playerid] = INVALID_VEHICLE_ID;
}

// The speedometer tag: 0 none, 1 belt off, 2 belt on, 3 helmet off, 4 helmet on.
stock Safety_SpeedoState(playerid, vehicleid)
{
    switch (Safety_GearFor(GetVehicleModel(vehicleid)))
    {
        case SAFETY_GEAR_BELT: return g_SafetyBelt[playerid] ? 2 : 1;
        case SAFETY_GEAR_HELMET: return Safety_WearsHelmet(playerid) ? 4 : 3;
        default: return 0;
    }
    return 0;
}

static stock Safety_ShowSchoolHelmet(playerid, bool:on)
{
    g_SafetySchoolOn[playerid] = on;
    if (on)
    {
        new index = Acc_IndexOf(ITEM_HELMET_BLACK);
        SetPlayerAttachedObject(playerid, ACC_FIRST_SLOT + _:ACC_PART_HEAD, SAFETY_SCHOOL_HELMET, BONE_HEAD,
            g_AccInfo[index][aiX], g_AccInfo[index][aiY], g_AccInfo[index][aiZ], g_AccInfo[index][aiRotX], g_AccInfo[index][aiRotY], g_AccInfo[index][aiRotZ]);
    }
    else
    {
        // Puts back whatever the player wears on the head, or clears the slot.
        Acc_ApplyAll(playerid);
    }
}

// The motorbike test lends a helmet to a player without one; it never goes to the inventory.
stock Safety_LendHelmet(playerid, bool:lend)
{
    g_SafetySchoolLent[playerid] = lend;
    if (!lend && g_SafetySchoolOn[playerid])
    {
        Safety_ShowSchoolHelmet(playerid, false);
    }
}

// Called before an accessory goes on or off the head, which replaces the lent helmet in that slot.
stock Safety_OnHeadAccessory(playerid)
{
    g_SafetySchoolOn[playerid] = false;
}

static stock Float:Safety_Clamp(Float:value, Float:low, Float:high)
{
    return value < low ? low : (value > high ? high : value);
}

// Thrown forward over the bonnet; set above the roof, so a wall the vehicle hit does not trap the player.
static stock Safety_Eject(playerid, vehicleid)
{
    new Float:x, Float:y, Float:z, Float:angle;
    GetVehiclePos(vehicleid, x, y, z);
    GetVehicleZAngle(vehicleid, angle);
    new Float:dx = -floatsin(angle, degrees), Float:dy = floatcos(angle, degrees);
    AC_SetPlayerPos(playerid, x + dx, y + dy, z + 1.8);
    AC_GraceMovement(playerid);
    SetPlayerVelocity(playerid, dx * 0.25, dy * 0.25, 0.12);
    Chat_Action(playerid, "izlece kroz sofersajbnu.");
}

static stock Safety_OnCrash(playerid, vehicleid, Float:loss)
{
    new E_SAFETY_GEAR:gear = Safety_GearFor(GetVehicleModel(vehicleid)), Float:damage;
    if (gear == SAFETY_GEAR_NONE || Safety_IsProtected(playerid, vehicleid))
    {
        return;
    }
    if (gear == SAFETY_GEAR_HELMET)
    {
        damage = Safety_Clamp(loss / 6.0, 5.0, 30.0);
    }
    else
    {
        damage = Safety_Clamp(loss / 8.0, 5.0, 25.0);
    }
    new Float:health;
    GetPlayerHealth(playerid, health);
    SetPlayerHealth(playerid, health > damage ? health - damage : 0.0);
    GameTextForPlayer(playerid, gear == SAFETY_GEAR_BELT ? ("~r~udes bez pojasa!") : ("~r~udes bez kacige!"), 1500, 3);
    if (gear == SAFETY_GEAR_BELT && loss >= SAFETY_EJECT_HP && Veh_Class(GetVehicleModel(vehicleid)) != VEH_CLASS_AIR)
    {
        Safety_Eject(playerid, vehicleid);
    }
}

forward Safety_Tick();
public Safety_Tick()
{
    for (new playerid = 0; playerid < MAX_PLAYERS; playerid++)
    {
        if (!IsPlayerConnected(playerid))
        {
            continue;
        }
        new PLAYER_STATE:seat = GetPlayerState(playerid);
        if (seat != PLAYER_STATE_DRIVER && seat != PLAYER_STATE_PASSENGER)
        {
            continue;
        }
        new vehicleid = GetPlayerVehicleID(playerid), Float:health, speed = floatround(Veh_SpeedKmh(vehicleid));
        GetVehicleHealth(vehicleid, health);
        if (vehicleid != g_SafetyVehicle[playerid])
        {
            g_SafetyVehicle[playerid] = vehicleid;
        }
        else if (g_SafetyLastHealth[playerid] - health >= SAFETY_CRASH_HP && g_SafetyLastSpeed[playerid] >= SAFETY_CRASH_MIN_KMH)
        {
            Safety_OnCrash(playerid, vehicleid, g_SafetyLastHealth[playerid] - health);
        }
        g_SafetyLastHealth[playerid] = health;
        g_SafetyLastSpeed[playerid] = speed;
    }
    return 1;
}

cmd:pojas(playerid, params[])
{
    new vehicleid = GetPlayerVehicleID(playerid);
    if (vehicleid == 0)
    {
        Msg_Error(playerid, "Niste u vozilu.");
        return 1;
    }
    new E_SAFETY_GEAR:gear = Safety_GearFor(GetVehicleModel(vehicleid));
    if (gear == SAFETY_GEAR_HELMET)
    {
        Msg_Error(playerid, "Na motoru nema pojasa; nosi se kaciga (/kaciga).");
        return 1;
    }
    if (gear == SAFETY_GEAR_NONE)
    {
        Msg_Error(playerid, "Ovo vozilo nema pojas.");
        return 1;
    }
    g_SafetyBelt[playerid] = !g_SafetyBelt[playerid];
    Chat_Action(playerid, g_SafetyBelt[playerid] ? ("vezuje pojas.") : ("odvezuje pojas."));
    return 1;
}
alias:pojas("seatbelt")

cmd:kaciga(playerid, params[])
{
    if (!g_InvLoaded[playerid] || Inv_IsBusy(playerid))
    {
        Msg_Error(playerid, "Trenutno ne mozete menjati dodatke.");
        return 1;
    }
    new worn = g_AccWorn[playerid][ACC_PART_HEAD];
    if (worn != -1 && Safety_IsHelmet(g_AccInfo[worn][aiItem]) && !g_SafetySchoolOn[playerid])
    {
        Acc_Toggle(playerid, g_AccInfo[worn][aiItem]);
        return 1;
    }
    if (g_SafetySchoolOn[playerid])
    {
        Safety_ShowSchoolHelmet(playerid, false);
        Chat_Action(playerid, "skida kacigu.");
        return 1;
    }
    for (new E_ITEM:item = ITEM_HELMET_BLACK; item <= ITEM_HELMET_WHITE; item++)
    {
        if (Inv_Has(playerid, item))
        {
            Acc_Toggle(playerid, item);
            return 1;
        }
    }
    if (g_SafetySchoolLent[playerid])
    {
        Safety_ShowSchoolHelmet(playerid, true);
        Chat_Action(playerid, "stavlja kacigu auto skole.");
        return 1;
    }
    Msg_Error(playerid, "Nemate kacigu. Kupuje se u prodavnici odece (/kupi).");
    return 1;
}
alias:kaciga("helmet")
