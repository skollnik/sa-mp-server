#define INV_BASE_SLOTS          (10)
#define INV_MAX_SLOTS           (16)
#define INV_GIVE_RADIUS         (3.0)

#define MAX_DROPS               (200)
#define DROP_PICKUP_RADIUS      (2.0)
#define DROP_LIFETIME_S         (15 * 60)

// Food and first aid only restore part of the HP; full health is the hospital's and the medics' job.
#define FOOD_FULL_MINUTES       (10)
#define FIRST_AID_HP            (30.0)
#define FIRST_AID_MAX_HP        (75.0)
#define FIRST_AID_COMBAT_LOCK_S (60)

enum E_ITEM
{
    ITEM_NONE,
    ITEM_PHONE,
    ITEM_BURGER,
    ITEM_PIZZA,
    ITEM_WATER,
    ITEM_JUICE,
    ITEM_BEER,
    ITEM_CIGARETTE,
    ITEM_LIGHTER,
    ITEM_CANISTER,
    ITEM_FIRST_AID
}

forward bool:Inv_Add(playerid, E_ITEM:item, amount);
forward bool:Inv_CanAdd(playerid, E_ITEM:item, amount);
forward bool:Inv_Remove(playerid, E_ITEM:item, amount);
forward bool:Inv_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[]);

enum E_ITEM_INFO
{
    itName[24],
    itMaxStack,
    itPrice,
    itShopAmount,
    itModel
}

// itPrice is for one shop purchase of itShopAmount pieces; 0 means it is not sold in 24/7 shops.
new const g_ItemInfo[E_ITEM][E_ITEM_INFO] =
{
    { "-",                  0,  0,   0,  0 },
    { "Telefon",            1,  PHONE_PRICE, 1, 18868 },
    { "Hamburger",          5,  10,  1,  2703 },
    { "Pica",               5,  15,  1,  2702 },
    { "Voda",               5,  2,   1,  1484 },
    { "Sok",                5,  3,   1,  1546 },
    { "Pivo",               5,  5,   1,  1486 },
    { "Cigareta",           20, 12,  20, 19896 },
    { "Upaljac",            1,  3,   1,  19998 },
    { "Kanister (prazan)",  1,  50,  1,  1650 },
    { "Prva pomoc",         3,  150, 1,  11738 }
};

enum E_INV_SLOT
{
    E_ITEM:isItem,
    isAmount
}

new g_Inv[MAX_PLAYERS][INV_MAX_SLOTS][E_INV_SLOT];
new bool:g_InvLoaded[MAX_PLAYERS];
new g_InvSelected[MAX_PLAYERS];
new g_InvListed[MAX_PLAYERS][INV_MAX_SLOTS];
new g_InvFullUntil[MAX_PLAYERS];

enum E_DROP
{
    bool:drUsed,
    E_ITEM:drItem,
    drAmount,
    Float:drX,
    Float:drY,
    Float:drZ,
    drInterior,
    drWorld,
    drCreatedAt,
    STREAMER_TAG_OBJECT:drObject,
    STREAMER_TAG_3D_TEXT_LABEL:drLabel
}

new g_Drops[MAX_DROPS][E_DROP];
new g_DropTimer;
