#define MAX_BUSINESSES          (200)
// Each business's inside gets its own virtual world (base + business ID), above the houses' range.
#define BIZ_WORLD_BASE          (6000)
#define BIZ_USE_RADIUS          (1.5)
#define BIZ_NEAR_RADIUS         (4.0)
// The pumps of a gas station are the fuel points this close to the station's spot.
#define BIZ_PUMP_RADIUS         (50.0)
#define BIZ_LABEL_DISTANCE      (15.0)
#define BIZ_ICON_DISTANCE       (300.0)
#define BIZ_PICKUP_SALE         (1274)
#define BIZ_PICKUP_OWNED        (1239)
#define BIZ_MIN_LEVEL           (6)
#define BIZ_MAX_PER_PLAYER      (1)
// Most extra business places an admin can give (/setmesta firme), like for houses.
#define BIZ_ADMIN_EXTRA_MAX     (5)
#define BIZ_LIST_SIZE           (BIZ_MAX_PER_PLAYER + BIZ_ADMIN_EXTRA_MAX + 1)
#define BIZ_MIN_PRICE           (1000)
#define BIZ_MAX_PRICE           (100000000)
#define BIZ_SELL_PERCENT        (70)
#define BIZ_INACTIVE_DAYS       (30)
#define BIZ_INACTIVE_PERCENT    (50)
#define BIZ_INACTIVE_CHECK_MS   (3600000)
// The owner sets one price for the whole business, as a share of the state's price; the limits keep a business from
// being a way to pass money between accounts or to rob the customers.
#define BIZ_PERCENT_MIN         (50)
#define BIZ_PERCENT_MAX         (200)
// Each item costs the owner this share of its state price, and arrives right away.
#define BIZ_STOCK_COST_PERCENT  (35)
// A business bought from the state comes with this share of every item, so it can sell from the first minute.
#define BIZ_START_STOCK_PERCENT (50)
// The owner is told when an item falls to this share.
#define BIZ_LOW_STOCK_PERCENT   (10)
// Stock is kept per item: up to 100 pieces of each, and the fuel in litres.
#define BIZ_ITEM_MAX            (100)
#define BIZ_FUEL_MAX            (10000)
// The longest item list of any type (the clothing shop: clothes and twelve accessories).
#define BIZ_MAX_PRODUCTS        (16)
// Tax per payday the owner gets, in hundredths of a percent of the business value (5 is 0.05%).
#define BIZ_TAX_PER_10000       (5)
// Unpaid tax above this share of the value and the state takes the business.
#define BIZ_DEBT_PERCENT        (1)
#define BIZ_FEE_MAX             (500)
#define BIZ_NAME_MIN            (3)
#define BIZ_NAME_MAX            (32)

enum E_BIZ_INTERIOR
{
    biName[40],
    biInterior,
    Float:biX,
    Float:biY,
    Float:biZ,
    Float:biA
}

// The game's own interiors the owner picked for businesses. A business type points at one by index, so new ones
// are only appended. The gun shop waits for the weapon license (LIC-3).
new const g_BizInteriors[][E_BIZ_INTERIOR] =
{
    { "24/7",                           17, -25.8845,  -185.8689, 1003.5469, 0.0 },
    { "Oruzje",                         7,  314.8210,  -141.4320, 999.6016,  0.0 },
    { "Restoran (Cluckin' Bell)",       9,  365.6730,  -10.7132,  1001.8700, 0.0 },
    { "Bar (Ten Green Bottles)",        11, 501.9810,  -69.1502,  998.7578,  0.0 },
    { "Klub (Pig Pen)",                 2,  1204.8099, -11.5868,  1000.9219, 0.0 },
    { "Odeca (Binco)",                  15, 207.7380,  -109.0200, 1005.1328, 0.0 }
};

enum E_BIZ_TYPE
{
    BIZ_247,
    BIZ_RESTAURANT,
    BIZ_BAR,
    BIZ_CLUB,
    BIZ_CLOTHES,
    BIZ_GAS
}

enum E_BIZ_TYPE_INFO
{
    btName[16],
    // Index in g_BizInteriors; -1 for the gas station, which is only its pumps outside.
    btInterior,
    btIcon,
    bool:btHasFee
}

new const g_BizTypes[E_BIZ_TYPE][E_BIZ_TYPE_INFO] =
{
    { "24/7",       0,  17, false },
    { "Restoran",   2,  50, false },
    { "Bar",        3,  49, false },
    { "Klub",       4,  48, true },
    { "Odeca",      5,  45, false },
    { "Benzinska",  -1, 47, false }
};

// Eaten or drunk on the spot in a restaurant, bar or club.
enum E_BIZ_MEAL
{
    E_BIZ_TYPE:bmType,
    bmName[24],
    bmPrice,
    bmHunger,
    Float:bmHealth,
    bmDrunk
}

new const g_BizMeals[][E_BIZ_MEAL] =
{
    { BIZ_RESTAURANT, "Mali meni",       8,  30, 5.0,  0 },
    { BIZ_RESTAURANT, "Srednji meni",    14, 55, 8.0,  0 },
    { BIZ_RESTAURANT, "Veliki meni",     20, 85, 12.0, 0 },
    { BIZ_RESTAURANT, "Sok",             3,  5,  0.0,  0 },
    { BIZ_RESTAURANT, "Kafa",            3,  5,  0.0,  0 },
    { BIZ_BAR,        "Pivo",            5,  0,  0.0,  1500 },
    { BIZ_BAR,        "Vino",            8,  0,  0.0,  2000 },
    { BIZ_BAR,        "Viski",           12, 0,  0.0,  3000 },
    { BIZ_BAR,        "Sok",             3,  5,  0.0,  0 },
    { BIZ_BAR,        "Kikiriki",        4,  20, 2.0,  0 },
    { BIZ_CLUB,       "Pivo",            8,  0,  0.0,  1500 },
    { BIZ_CLUB,       "Koktel",          15, 0,  0.0,  2500 },
    { BIZ_CLUB,       "Viski",           15, 0,  0.0,  3000 },
    { BIZ_CLUB,       "Energetsko pice", 6,  5,  0.0,  0 },
    { BIZ_CLUB,       "Sok",             5,  5,  0.0,  0 }
};
// Most rows one business type has in g_BizMeals.
#define BIZ_MEALS_LISTED        (8)
// The drunk level stops here, the same cap as a beer from the inventory.
#define BIZ_DRUNK_MAX           (10000)

enum E_BIZ
{
    bool:bUsed,
    bDbId,
    bOwnerDbId,
    bOwnerName[MAX_PLAYER_NAME + 1],
    E_BIZ_TYPE:bType,
    bName[BIZ_NAME_MAX + 1],
    bPrice,
    Float:bX,
    Float:bY,
    Float:bZ,
    Float:bA,
    bool:bLocked,
    bTill,
    bPercent,
    bFee,
    bTaxDebt,
    STREAMER_TAG_PICKUP:bPickup,
    STREAMER_TAG_3D_TEXT_LABEL:bLabel,
    STREAMER_TAG_MAP_ICON:bIcon,
    STREAMER_TAG_PICKUP:bExitPickup,
    STREAMER_TAG_3D_TEXT_LABEL:bExitLabel
}

new g_Biz[MAX_BUSINESSES][E_BIZ];
// Pieces of each item by its row in the type's list (the same row as in the shop menu); the fuel is row 0, in litres.
new g_BizStock[MAX_BUSINESSES][BIZ_MAX_PRODUCTS];

forward bool:Biz_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[]);
forward bool:Biz_TryUse(playerid, bool:enter, bool:leave);
forward bool:Biz_IsSaleReady(slot, dbid, seller, problem[], size);
forward bool:Biz_HasFreeSlot(playerid);
forward bool:Biz_RequireShop(playerid, E_POINT_TYPE:point, E_BIZ_TYPE:type);
forward bool:Biz_IsShopHere(playerid, E_POINT_TYPE:point, E_BIZ_TYPE:type);
forward bool:Biz_ShowShop(playerid);
forward bool:Biz_Pay(playerid, row, base, const reason[], &paid = 0);
forward bool:Biz_PayFuel(playerid, litres, const reason[], &paid = 0);
