#define MAX_HOUSES              (500)
// Each house's inside gets its own virtual world (base + house ID), above the doors' range.
#define HOUSE_WORLD_BASE        (5000)
#define HOUSE_USE_RADIUS        (1.5)
#define HOUSE_NEAR_RADIUS       (4.0)
#define HOUSE_GARAGE_RADIUS     (5.0)
#define HOUSE_LABEL_DISTANCE    (15.0)
#define HOUSE_ICON_DISTANCE     (400.0)
// The owner sees their own house from anywhere on the map.
#define HOUSE_OWN_ICON_DISTANCE (6000.0)
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
// Spawning at home on login only after this long away, so leaving and rejoining is no quicker than driving home.
#define HOUSE_SPAWN_AWAY_S      (600)
#define HOUSE_SAFE_RADIUS       (1.5)
#define SAFE_MAX_SLOTS          (30)
// A tenant away this long loses the room, so a room is not held for good by someone who stopped playing.
#define ROOM_INACTIVE_DAYS      (30)
#define ROOM_MIN_LEVEL          (1)
// A house for sale can be looked at from inside for this long; after being put out, the same house waits the cooldown.
#define HOUSE_VIEW_SECONDS      (300)
#define HOUSE_VIEW_COOLDOWN_S   (300)
#define HOUSE_VIEW_TICK_MS      (5000)
#define WARDROBE_MAX_SLOTS      (8)

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
    // How a reference gamemode rates the interior (1-10 stars); 0 where it gives no rating.
    hiStars,
    hiInterior,
    Float:hiX,
    Float:hiY,
    Float:hiZ
}

// GTA's own house interiors, the ones reference gamemodes use for houses, sorted into sizes by their rating.
// A house stores its interior by index, so new ones are only appended.
new const g_HouseInteriors[][E_HOUSE_INTERIOR] =
{
    { "Mala kuca 1",   HOUSE_SMALL,  1,  10, 2259.5068, -1135.9337, 1050.6328 },
    { "Mala kuca 2",   HOUSE_SMALL,  2,  6,  -68.8411,  1351.3397,  1080.2109 },
    { "Mala kuca 3",   HOUSE_SMALL,  2,  9,  260.8800,  1237.2365,  1084.2578 },
    { "Mala kuca 4",   HOUSE_SMALL,  3,  2,  226.4436,  1239.9277,  1082.1406 },
    { "Mala kuca 5",   HOUSE_SMALL,  3,  8,  -42.5525,  1405.6432,  1084.4297 },
    { "Mala kuca 6",   HOUSE_SMALL,  3,  11, 2282.8831, -1140.0713, 1050.8984 },
    { "Mala kuca 7",   HOUSE_SMALL,  3,  1,  2218.3875, -1076.1580, 1050.4844 },
    { "Mala kuca 8",   HOUSE_SMALL,  0,  1,  224.2880,  1289.1907,  1082.1406 },
    { "Srednja kuca 1", HOUSE_MEDIUM, 4, 15, 295.2479,  1472.2650,  1080.2578 },
    { "Srednja kuca 2", HOUSE_MEDIUM, 4, 4,  261.1874,  1284.2982,  1080.2578 },
    { "Srednja kuca 3", HOUSE_MEDIUM, 5, 15, -283.6001, 1471.2211,  1084.3750 },
    { "Srednja kuca 4", HOUSE_MEDIUM, 5, 5,  22.9848,   1403.3345,  1084.4370 },
    { "Srednja kuca 5", HOUSE_MEDIUM, 6, 3,  235.2748,  1186.6809,  1080.2578 },
    { "Srednja kuca 6", HOUSE_MEDIUM, 6, 5,  226.2956,  1114.1615,  1080.9929 },
    { "Srednja kuca 7", HOUSE_MEDIUM, 6, 8,  2365.3345, -1135.5907, 1050.8826 },
    { "Srednja kuca 8", HOUSE_MEDIUM, 6, 2,  2468.2080, -1698.2988, 1013.5078 },
    { "Srednja kuca 9 (Johnson)", HOUSE_MEDIUM, 0, 3, 2496.0549, -1695.1749, 1014.7422 },
    { "Velika kuca 1", HOUSE_LARGE,  7,  9,  2317.7983, -1026.7651, 1050.2178 },
    { "Velika kuca 2", HOUSE_LARGE,  7,  6,  2196.8469, -1204.3524, 1049.0234 },
    { "Velika kuca 3", HOUSE_LARGE,  8,  4,  -260.4934, 1456.8430,  1084.3672 },
    { "Velika kuca 4", HOUSE_LARGE,  8,  10, 24.0716,   1340.1615,  1084.3750 },
    { "Velika kuca 5", HOUSE_LARGE,  9,  5,  140.2660,  1366.1091,  1083.8594 },
    { "Velika kuca 6", HOUSE_LARGE,  9,  6,  234.1541,  1063.7206,  1084.2123 },
    { "Velika kuca 7", HOUSE_LARGE,  9,  9,  83.0791,   1322.2808,  1083.8662 },
    { "Velika kuca 8", HOUSE_LARGE,  9,  2,  447.2238,  1397.2926,  1084.3047 },
    { "Velika kuca 9", HOUSE_LARGE,  10, 2,  2237.5413, -1081.1516, 1049.0400 },
    { "Vila (Madd Dogg)", HOUSE_LARGE, 10, 5, 1267.8407, -776.9587, 1091.9063 }
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
    // Where the exit stands inside; all zero means the interior's own entry spot.
    Float:hExitX,
    Float:hExitY,
    Float:hExitZ,
    Float:hExitA,
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
    STREAMER_TAG_3D_TEXT_LABEL:hGarageLabel,
    bool:hHasSafe,
    Float:hSafeX,
    Float:hSafeY,
    Float:hSafeZ,
    hSafeMoney,
    STREAMER_TAG_3D_TEXT_LABEL:hSafeLabel,
    bool:hRoomsOpen,
    hRoomPrice,
    // Counted from the accounts on start and kept up to date, so a full house is known without a query.
    hTenants
}

new g_Houses[MAX_HOUSES][E_HOUSE];
// Belongs to the house, like the trunk to the vehicle; only the owner opens it.
new g_HouseSafe[MAX_HOUSES][SAFE_MAX_SLOTS][E_INV_SLOT];
new bool:g_HouseSafesLoaded;
// The house a player rents a room in (DB id or 0) and its map icon.
new g_RoomHouse[MAX_PLAYERS];
new STREAMER_TAG_MAP_ICON:g_RoomIcon[MAX_PLAYERS];
// The last rent the tenant paid, given back when the owner evicts them.
new g_RoomPaid[MAX_PLAYERS];
// The house for sale a player is looking at (DB id or 0), until when, and the house they were last put out of.
new g_HouseViewing[MAX_PLAYERS];
new g_HouseViewEnd[MAX_PLAYERS];
new g_HouseViewBlocked[MAX_PLAYERS];
new g_HouseViewBlockedUntil[MAX_PLAYERS];
// Skins kept in the player's wardrobe (0 is an empty place); it stays with the player, not with a house.
new g_Wardrobe[MAX_PLAYERS][WARDROBE_MAX_SLOTS];
new bool:g_WardrobeLoaded[MAX_PLAYERS];
// The house a player is working with in a menu, by slot, and the account setting to spawn at home (house DB id or 0).
new g_HouseSelected[MAX_PLAYERS];
new g_HouseSpawn[MAX_PLAYERS];
new bool:g_HouseSpawnPending[MAX_PLAYERS];
// Own-house icons wait for the first spawn: the client drops map icons sent while it is still on the login screen.
new bool:g_HouseIconsShown[MAX_PLAYERS];
new g_HouseGarageListed[MAX_PLAYERS][4];

forward bool:House_GaragePos(house_dbid, &Float:x, &Float:y, &Float:z);
forward bool:House_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[]);
forward bool:House_TryUse(playerid, bool:enter, bool:leave);
forward bool:House_IsSaleReady(slot, dbid, seller, problem[], size);
forward bool:House_HasFreeSlot(playerid);
forward bool:House_GarageHasRoom(house_dbid);
forward bool:House_MoveToSpawn(playerid);
forward bool:House_CanLiveIn(playerid, slot);
forward bool:Safe_CanUse(playerid, slot, bool:tell);
forward bool:Safe_IsEmpty(slot);
forward bool:Room_IsTenant(playerid, slot);
forward bool:Room_ShowMenu(playerid);
forward bool:HouseView_Start(playerid, slot);
forward bool:Store_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[]);
forward bool:Safe_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[]);
forward bool:Room_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[]);
forward bool:Clothes_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[]);
forward bool:Wardrobe_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[]);
