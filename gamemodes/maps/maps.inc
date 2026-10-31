// Every map file is included here. Objects are created once on startup through the streamer,
// so only those near a player are sent to them. Removed GTA buildings must be removed per player on connect.

#define MAP_STREAM_DISTANCE     (300.0)
#define MAP_INTERIOR_STREAM     (200.0)

// Custom interiors float high above the city in an interior ID that GTA itself never uses,
// so they never show up in the open world and the city never shows through their walls.
#define MAP_INTERIOR_ID         (20)

// The biggest interior has almost 1000 objects; the streamer default of 500 leaves holes in its walls.
#define MAP_VISIBLE_OBJECTS     (1000)

// Ground planes and rebuilt GTA buildings stand in for removed GTA LOD models, which are what the game
// shows from far away; with the normal distance they vanish and leave a hole in the city.
#define MAP_FAR_DISTANCE        (800.0)

static stock bool:Map_IsFarModel(modelid)
{
    switch (modelid)
    {
        case 4600, 4806, 19529, 19531, 19536, 19547: return true;
    }
    return false;
}

// The draw distance is set explicitly; with the default the client uses the model's own, often only 150 m.
stock STREAMER_TAG_OBJECT:Map_Exterior(modelid, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz)
{
    new Float:distance = Map_IsFarModel(modelid) ? MAP_FAR_DISTANCE : MAP_STREAM_DISTANCE;
    return CreateDynamicObject(modelid, x, y, z, rx, ry, rz, 0, 0, -1, distance, distance);
}

stock STREAMER_TAG_OBJECT:Map_Interior(modelid, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz)
{
    return CreateDynamicObject(modelid, x, y, z, rx, ry, rz, -1, MAP_INTERIOR_ID, -1, MAP_INTERIOR_STREAM);
}

#include "maps/exteriors/cityhall_exterior.inc"
#include "maps/exteriors/cityhall_parking.inc"
#include "maps/exteriors/car_market.inc"
#include "maps/exteriors/bank_exterior.inc"
#include "maps/exteriors/vehicle_inspection.inc"
#include "maps/exteriors/hospital_exterior.inc"

#include "maps/interiors/cityhall_a.inc"
#include "maps/interiors/police_a.inc"
#include "maps/interiors/prison.inc"
#include "maps/interiors/bank.inc"
#include "maps/interiors/hospital.inc"

stock Maps_Init()
{
    Streamer_VisibleItems(STREAMER_TYPE_OBJECT, MAP_VISIBLE_OBJECTS);

    Map_CityHallExterior();
    Map_CityHallParking();
    Map_CarMarket();
    Map_BankExterior();
    Map_VehicleInspection();

    Map_HospitalExterior();

    Map_CityHallInteriorA();
    Map_PoliceInteriorA();
    Map_PrisonInterior();
    Map_BankInterior();
    Map_HospitalInterior();

    printf("[MAPS] %d objects created.", CountDynamicObjects());
}

stock Maps_OnPlayerConnect(playerid)
{
    Map_CityHallExterior_Remove(playerid);
    Map_CityHallParking_Remove(playerid);
    Map_BankExterior_Remove(playerid);
    Map_VehicleInspection_Remove(playerid);
}
