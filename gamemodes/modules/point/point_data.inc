#define MAX_POINTS              (150)
#define POINT_USE_RADIUS        (2.0)
#define POINT_LABEL_DISTANCE    (8.0)
#define POINT_PICKUP_MODEL      (1239)
#define POINT_ATM_MODEL         (19324)

// Places where a service is used: standing next to one unlocks its command.
enum E_POINT_TYPE
{
    POINT_BANK,
    POINT_ATM,
    POINT_CITYHALL,
    POINT_SHOP,
    POINT_CLOTHES
}

enum E_POINT_INFO
{
    piKey[16],
    piName[32],
    piCommand[16]
}

new const g_PointInfo[E_POINT_TYPE][E_POINT_INFO] =
{
    { "banka",      "Banka - salter",   "/banka" },
    { "bankomat",   "Bankomat",         "/bankomat" },
    { "opstina",    "Opstina - salter", "/opstina" },
    { "prodavnica", "Prodavnica 24/7",  "/kupi" },
    { "odeca",      "Prodavnica odece", "/kupi" }
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
