forward bool:Preview_IsOpen(playerid);
forward bool:Preview_OnClickPlayerTextDraw(playerid, PlayerText:td);
forward PlayerText:Preview_CreateButton(playerid, Float:x, const text[]);

#define PREVIEW_PER_PAGE        (10)
#define PREVIEW_COLUMNS         (5)
// The most pieces of furniture one house can hold (80 for a large house plus 50 from an admin).
#define PREVIEW_MAX_ITEMS       (130)

#define PREVIEW_HOVER_COLOR     (0xFFD700FF)
#define PREVIEW_SLOT_COLOR      (0x2A2A2AFF)

enum E_PREVIEW_MENU
{
    PREVIEW_NONE,
    PREVIEW_VIP_SKIN,
    PREVIEW_DOOR_ARROW,
    PREVIEW_CLOTHES_SHOP,
    PREVIEW_WARDROBE_TAKE,
    PREVIEW_WARDROBE_PUT,
    PREVIEW_FURN_SHOP,
    PREVIEW_FURN_PLACE,
    PREVIEW_FURN_MOVE,
    PREVIEW_FURN_SELL
}

enum E_PREVIEW_KIND
{
    PREVIEW_KIND_SKIN,
    PREVIEW_KIND_VEHICLE,
    PREVIEW_KIND_OBJECT,
    PREVIEW_KIND_FURNITURE
}

enum E_PREVIEW_STATE
{
    E_PREVIEW_MENU:pvMenu,
    E_PREVIEW_KIND:pvKind,
    pvModels[PREVIEW_MAX_ITEMS],
    pvCount,
    pvPage,
    PlayerText:pvBackground,
    PlayerText:pvTitle,
    PlayerText:pvSlots[PREVIEW_PER_PAGE],
    PlayerText:pvPrev,
    PlayerText:pvNext,
    PlayerText:pvPageText,
    PlayerText:pvClose
}

new g_Preview[MAX_PLAYERS][E_PREVIEW_STATE];
