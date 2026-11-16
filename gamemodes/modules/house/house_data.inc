#define MAX_HOUSES              (500)
// Each house's inside gets its own virtual world (base + house ID), above the doors' range.
#define HOUSE_WORLD_BASE        (5000)
#define HOUSE_USE_RADIUS        (1.5)
#define HOUSE_NEAR_RADIUS       (4.0)
#define HOUSE_GARAGE_RADIUS     (5.0)
#define HOUSE_LABEL_DISTANCE    (15.0)
#define HOUSE_ICON_DISTANCE     (400.0)
#define HOUSE_PICKUP_SALE       (1273)
#define HOUSE_PICKUP_OWNED      (1272)
#define HOUSE_ICON_SALE         (31)
#define HOUSE_ICON_OWNED        (32)
#define HOUSE_MAX_PER_PLAYER    (1)
#define HOUSE_VIP_EXTRA         (1)
#define HOUSE_SELL_PERCENT      (70)
// An owner away this long loses the house; the money goes to the bank so it is waiting when they come back.
#define HOUSE_INACTIVE_DAYS     (30)
#define HOUSE_INACTIVE_PERCENT  (50)
#define HOUSE_INACTIVE_CHECK_MS (3600000)
#define HOUSE_MIN_PRICE         (1000)
#define HOUSE_MAX_PRICE         (100000000)

enum E_HOUSE_SIZE
{
    HOUSE_SMALL,
    HOUSE_MEDIUM,
    HOUSE_LARGE
}

new const g_HouseSizeNames[E_HOUSE_SIZE][] = { "Mala", "Srednja", "Velika" };

enum E_HOUSE_INTERIOR
{
    hiName[32],
    E_HOUSE_SIZE:hiSize,
    hiInterior,
    Float:hiX,
    Float:hiY,
    Float:hiZ
}

// GTA's own house interiors (open.mp interior list), so nothing has to be downloaded. New ones are only appended,
// since a house stores its interior by index.
new const g_HouseInteriors[][E_HOUSE_INTERIOR] =
{
    { "Soba (Safehouse 1)",       HOUSE_SMALL,  5,  2233.6919, -1112.8107, 1050.8828 },
    { "Mala kuca 1",              HOUSE_SMALL,  1,  224.2880,  1289.1907,  1082.1406 },
    { "Mala kuca 2",              HOUSE_SMALL,  4,  261.1165,  1287.2197,  1080.2578 },
    { "Mala kuca 3",              HOUSE_SMALL,  3,  234.6087,  1187.8195,  1080.2578 },
    { "Stan (Safehouse 4)",       HOUSE_SMALL,  10, 2261.0977, -1137.8833, 1050.6328 },
    { "Ryder-ova kuca",           HOUSE_SMALL,  2,  2447.8704, -1704.4509, 1013.5078 },
    { "Johnson kuca",             HOUSE_MEDIUM, 3,  2496.0549, -1695.1749, 1014.7422 },
    { "Srednja kuca 1",           HOUSE_MEDIUM, 2,  225.5707,  1240.0643,  1082.1406 },
    { "Srednja kuca 2",           HOUSE_MEDIUM, 15, 295.1391,  1473.3719,  1080.2578 },
    { "Srednja kuca 3",           HOUSE_MEDIUM, 4,  221.6766,  1142.4962,  1082.6094 },
    { "Stan (Willowfield)",       HOUSE_MEDIUM, 11, 2282.9766, -1140.2861, 1050.8984 },
    { "Stan (Verdant Bluffs)",    HOUSE_MEDIUM, 8,  2365.1089, -1133.0795, 1050.8750 },
    { "Velika kuca 1",            HOUSE_LARGE,  5,  140.3679,  1367.8837,  1083.8621 },
    { "Velika kuca 2",            HOUSE_LARGE,  6,  234.2826,  1065.2290,  1084.2101 },
    { "Velika kuca 3",            HOUSE_LARGE,  5,  227.7559,  1114.3844,  1080.9922 },
    { "Velika kuca 4",            HOUSE_LARGE,  9,  84.9244,   1324.2983,  1083.8594 },
    { "Velika kuca 5 (Safehouse)", HOUSE_LARGE, 12, 2323.7063, -1147.6509, 1050.7101 },
    { "Vila (Madd Dogg)",         HOUSE_LARGE,  5,  1267.8407, -776.9587,  1091.9063 }
};

enum E_HOUSE
{
    bool:hUsed,
    hDbId,
    hOwnerDbId,
    hOwnerName[MAX_PLAYER_NAME + 1],
    hPrice,
    hLevel,
    hInterior,
    Float:hX,
    Float:hY,
    Float:hZ,
    Float:hA,
    bool:hLocked,
    bool:hHasGarage,
    Float:hGarageX,
    Float:hGarageY,
    Float:hGarageZ,
    Float:hGarageA,
    STREAMER_TAG_PICKUP:hPickup,
    STREAMER_TAG_3D_TEXT_LABEL:hLabel,
    STREAMER_TAG_MAP_ICON:hIcon,
    STREAMER_TAG_PICKUP:hExitPickup,
    STREAMER_TAG_3D_TEXT_LABEL:hExitLabel,
    STREAMER_TAG_3D_TEXT_LABEL:hGarageLabel
}

new g_Houses[MAX_HOUSES][E_HOUSE];
// The house a player is working with in a menu, by slot, and the account setting to spawn at home (house DB id or 0).
new g_HouseSelected[MAX_PLAYERS];
new g_HouseSpawn[MAX_PLAYERS];
new bool:g_HouseSpawnPending[MAX_PLAYERS];
new g_HouseGarageListed[MAX_PLAYERS][4];

forward bool:House_GaragePos(house_dbid, &Float:x, &Float:y, &Float:z);
forward bool:House_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[]);
forward bool:House_TryUse(playerid, bool:enter, bool:leave);
forward bool:House_IsSaleReady(slot, dbid, seller);
forward bool:House_HasFreeSlot(playerid);
