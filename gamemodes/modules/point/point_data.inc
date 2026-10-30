#define MAX_POINTS              (150)
#define POINT_LABEL_DISTANCE    (8.0)
#define POINT_PICKUP_MODEL      (1239)
#define POINT_ATM_MODEL         (19324)
#define POINT_FUEL_PICKUP_MODEL (1650)

// Places where a service is used: standing next to one unlocks its command.
enum E_POINT_TYPE
{
    POINT_BANK,
    POINT_ATM,
    POINT_CITYHALL,
    POINT_SHOP,
    POINT_CLOTHES,
    POINT_FUEL,
    POINT_INSPECTION
}

enum E_POINT_INFO
{
    piKey[16],
    piName[32],
    piCommand[16],
    // How close the player must stand; a pump and the inspection are used from the driver's seat, so they reach further.
    Float:piRadius,
    piPickup
}

new const g_PointInfo[E_POINT_TYPE][E_POINT_INFO] =
{
    { "banka",      "Banka - salter",   "/banka",    2.0, POINT_PICKUP_MODEL },
    { "bankomat",   "Bankomat",         "/bankomat", 2.0, POINT_PICKUP_MODEL },
    { "opstina",    "Opstina - salter", "/opstina",  2.0, POINT_PICKUP_MODEL },
    { "prodavnica", "Prodavnica 24/7",  "/kupi",     2.0, POINT_PICKUP_MODEL },
    { "odeca",      "Prodavnica odece", "/kupi",     2.0, POINT_PICKUP_MODEL },
    { "pumpa",      "Benzinska pumpa",  "/gorivo",   6.0, POINT_FUEL_PICKUP_MODEL },
    { "pregled",    "Tehnicki pregled", "/registruj", 6.0, POINT_PICKUP_MODEL }
};

enum E_POINT
{
    bool:ptUsed,
    ptDbId,
    E_POINT_TYPE:ptType,
    Float:ptX,
    Float:ptY,
    Float:ptZ,
    Float:ptA,
    ptInterior,
    ptWorld,
    STREAMER_TAG_PICKUP:ptPickup,
    STREAMER_TAG_3D_TEXT_LABEL:ptLabel,
    STREAMER_TAG_OBJECT:ptObject
}

new g_Points[MAX_POINTS][E_POINT];

forward bool:Point_IsNear(playerid, E_POINT_TYPE:type);
forward bool:Point_Require(playerid, E_POINT_TYPE:type);
forward bool:Point_OnDialogResponse(playerid, dialogid, response, listitem);
