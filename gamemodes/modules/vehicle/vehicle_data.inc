// Every vehicle of an active owner stays in the world; the rest of MAX_VEHICLES is left for admin, job and faction vehicles.
#define MAX_PERSONAL_VEHICLES   (1000)
// Vehicles of owners who have not logged in for this long are not spawned on startup, so the vehicle limit is not filled
// by players who left; they come back as soon as the owner logs in.
#define VEH_INACTIVE_DAYS       (14)
#define VEH_PLATE_LENGTH        (12)
#define VEH_KEY_RANGE           (5.0)
// Extra vehicle slot for Silver and Gold VIP.
#define VEH_VIP_EXTRA           (1)
#define VEH_DEATH_RESPAWN_S     (10)
// A destroyed vehicle comes back smoking, so a crash still costs something (repair: mechanic, phase 3.5).
#define VEH_RESPAWN_HEALTH      (500.0)
// Saved below this it would catch fire again right after spawning.
#define VEH_MIN_SAVED_HEALTH    (300.0)
#define VEH_ENGINE_START_MS     (1500)
// Below this the engine sometimes fails to start on the first try.
#define VEH_ENGINE_WEAK_HEALTH  (400.0)
#define VEH_REGISTRATION_DAYS   (60)
// Renewing is allowed only near the end, so nobody stacks years of registration in advance.
#define VEH_REGISTRATION_RENEW_DAYS (15)
#define VEH_REGISTRATION_WARN_DAYS  (7)
// Until the mechanic exists (step 3.5) a heavily damaged vehicle could never pass; raise this once repairs can be bought.
#define VEH_INSPECTION_MIN_HEALTH   (650.0)
#define VEH_INSPECTION_MS       (5000)
// One saved component per tuning slot (CARMODTYPE 0-15).
#define VEH_TUNE_SLOTS          (16)
#define VEH_PAINTJOB_NONE       (3)

enum E_VEH
{
    bool:vUsed,
    vDbId,
    vOwnerDbId,
    vOwnerId,
    vOwnerName[MAX_PLAYER_NAME + 1],
    vVehicleId,
    vModel,
    vColor1,
    vColor2,
    // Empty until the vehicle is registered for the first time.
    vPlate[VEH_PLATE_LENGTH],
    // Issued at the police counter but not mounted yet (/v tablice).
    vPendingPlate[VEH_PLATE_LENGTH],
    vRegisteredUntil,
    Float:vParkX,
    Float:vParkY,
    Float:vParkZ,
    Float:vParkA,
    // False for a market vehicle until the owner leaves it somewhere (its spot is the market exit until then).
    bool:vParked,
    Float:vFuel,
    Float:vHealth,
    VEHICLE_PANEL_STATUS:vPanels,
    VEHICLE_DOOR_STATUS:vDoors,
    VEHICLE_LIGHT_STATUS:vLights,
    VEHICLE_TYRE_STATUS:vTyres,
    bool:vLocked,
    vKeyDbId,
    vKeyName[MAX_PLAYER_NAME + 1],
    vMods[VEH_TUNE_SLOTS],
    vPaintjob,
    vDiedAt,
    // The last camera that photographed the vehicle and when, so one camera fines a vehicle only once in a while.
    vCameraSlot,
    vCameraAt
}

new g_Veh[MAX_PERSONAL_VEHICLES][E_VEH];
// Slot in g_Veh for every vehicle ID, or -1 for vehicles that are not personal.
new g_VehSlotOf[MAX_VEHICLES];
// Fuel is kept for every vehicle ID, so job and faction vehicles can use it later.
new Float:g_VehFuel[MAX_VEHICLES];
new bool:g_VehFuelReserveWarned[MAX_VEHICLES];
new g_VehListed[MAX_PLAYERS][VEH_MAX_PER_PLAYER + VEH_VIP_EXTRA + 1];
new g_VehSelected[MAX_PLAYERS];
// A running engine start or technical inspection; 0 when none.
new g_VehStartTimer[MAX_PLAYERS];
new g_VehInspectTimer[MAX_PLAYERS];
new g_VehTimer;
new g_VehResetTimer;

enum E_VEH_PARAM
{
    VEH_PARAM_ENGINE,
    VEH_PARAM_LIGHTS,
    VEH_PARAM_ALARM,
    VEH_PARAM_DOORS,
    VEH_PARAM_BONNET,
    VEH_PARAM_BOOT
}

forward bool:Fuel_IsUsed(vehicleid);
forward Float:Veh_SpeedKmh(vehicleid);
forward bool:Veh_GetParam(vehicleid, E_VEH_PARAM:param);
forward bool:Veh_IsEngineOn(vehicleid);
forward bool:Veh_HasKey(playerid, vehicleid);
forward bool:Veh_IsOwner(playerid, vehicleid);
forward bool:Veh_OnDialogResponse(playerid, dialogid, response, listitem);
forward bool:Veh_OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys);
forward bool:Veh_IsRegistered(slot);
forward bool:Veh_HasFreeSlot(playerid);
forward bool:Market_IsNearExit(vehicleid);
forward bool:Market_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[]);
forward bool:Offer_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[]);
forward bool:Plate_StoreFromVehicle(playerid, slot);
forward bool:Plate_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[]);
forward bool:Fine_OnDialogResponse(playerid, dialogid, response);
forward bool:Tune_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[]);
