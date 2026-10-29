#define MAX_DOORS               (200)
#define DOOR_NAME_LENGTH        (48)
#define DOOR_USE_RADIUS         (1.5)
#define DOOR_LABEL_DISTANCE     (10.0)
#define DOOR_PICKUP_MODEL       (1318)
#define DOOR_ICON_DISTANCE      (3000.0)
#define DOOR_USE_COOLDOWN_MS    (2000)
// Doors are no way out of a fight: they stay shut for a while after the player takes damage.
#define DOOR_COMBAT_LOCK_S      (30)
// Each door's inside gets its own virtual world (base + door ID), so two doors sharing an interior never meet.
#define DOOR_WORLD_BASE         (1000)

enum E_DOOR
{
    bool:dUsed,
    dDbId,
    dName[DOOR_NAME_LENGTH],
    Float:dOutX,
    Float:dOutY,
    Float:dOutZ,
    Float:dOutA,
    dOutInterior,
    dOutWorld,
    bool:dHasExit,
    Float:dInX,
    Float:dInY,
    Float:dInZ,
    Float:dInA,
    dInInterior,
    dInWorld,
    bool:dLocked,
    dIcon,
    dModel,
    STREAMER_TAG_PICKUP:dOutPickup,
    STREAMER_TAG_3D_TEXT_LABEL:dOutLabel,
    STREAMER_TAG_PICKUP:dInPickup,
    STREAMER_TAG_3D_TEXT_LABEL:dInLabel,
    STREAMER_TAG_MAP_ICON:dMapIcon
}

new g_Doors[MAX_DOORS][E_DOOR];
new g_DoorLastUse[MAX_PLAYERS];
// The door whose arrow an admin is picking in the picture menu (database ID).
new g_DoorArrowEdit[MAX_PLAYERS];

// Models offered for the marker at both sides of a door; the white arrow is the default.
new const g_DoorArrowModels[] =
{
    1318, 1559, 1239, 1272, 1273, 1314, 1274, 1247, 19197, 19198, 19605, 19606, 19607, 19902
};
