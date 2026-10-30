// Room for every online owner's vehicles plus the ones still waiting to despawn after their owner left.
#define MAX_PERSONAL_VEHICLES   (MAX_PLAYERS * VEH_MAX_PER_PLAYER * 2)
#define VEH_PLATE_LENGTH        (12)
#define VEH_KEY_RANGE           (5.0)
// A vehicle stays this long after its owner leaves, so a short reconnect or a passenger is not left on foot.
#define VEH_DESPAWN_DELAY_S     (300)
#define VEH_DEATH_RESPAWN_S     (10)
// A destroyed vehicle comes back smoking, so a crash still costs something (repair: mechanic, phase 3.5).
#define VEH_RESPAWN_HEALTH      (500.0)
// Saved below this it would catch fire again right after spawning.
#define VEH_MIN_SAVED_HEALTH    (300.0)

enum E_VEH
{
    bool:vUsed,
    vDbId,
    vOwnerDbId,
    vOwnerId,
    vVehicleId,
    vModel,
    vColor1,
    vColor2,
    vPlate[VEH_PLATE_LENGTH],
    Float:vParkX,
    Float:vParkY,
    Float:vParkZ,
    Float:vParkA,
    Float:vFuel,
    Float:vHealth,
    VEHICLE_PANEL_STATUS:vPanels,
    VEHICLE_DOOR_STATUS:vDoors,
    VEHICLE_LIGHT_STATUS:vLights,
    VEHICLE_TYRE_STATUS:vTyres,
    bool:vLocked,
    vKeyDbId,
    vKeyName[MAX_PLAYER_NAME + 1],
    vDespawnAt,
    vDiedAt
}

new g_Veh[MAX_PERSONAL_VEHICLES][E_VEH];
// Slot in g_Veh for every vehicle ID, or -1 for vehicles that are not personal.
new g_VehSlotOf[MAX_VEHICLES];
// Fuel is kept for every vehicle ID, so job and faction vehicles can use it later.
new Float:g_VehFuel[MAX_VEHICLES];
new bool:g_VehFuelReserveWarned[MAX_VEHICLES];
new g_VehListed[MAX_PLAYERS][VEH_MAX_PER_PLAYER * 2];
new g_VehSelected[MAX_PLAYERS];
new g_VehTimer;

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
