forward bool:AC_CheckChat(playerid, const text[]);
forward bool:AC_CheckCommand(playerid);

#define AC_WEAPON_SLOTS             (13)
#define AC_TICK_MS                  (1000)
#define AC_ALERT_COOLDOWN_S         (30)
#define AC_MOVE_GRACE_S             (3)

// Sprinting is about 9 m/s; anything far above that on foot, horizontally, is not legit movement.
#define AC_MAX_FOOT_SPEED           (20.0)
#define AC_TELEPORT_DISTANCE        (80.0)
#define AC_MAX_VEHICLE_KMH          (280.0)

#define AC_CHAT_LIMIT               (3)
#define AC_CHAT_WINDOW_S            (3)
#define AC_REPEAT_LIMIT             (2)
#define AC_CMD_LIMIT                (5)
#define AC_CMD_WINDOW_S             (3)
#define AC_FLOOD_WARNINGS_TO_MUTE   (3)
#define AC_FLOOD_WARNING_WINDOW_S   (60)
#define AC_FLOOD_MUTE_MINUTES       (2)

enum E_AC_TYPE
{
    AC_TYPE_WEAPON,
    AC_TYPE_JETPACK,
    AC_TYPE_HEALTH,
    AC_TYPE_ARMOUR,
    AC_TYPE_FOOT_SPEED,
    AC_TYPE_VEHICLE_SPEED,
    AC_TYPE_TELEPORT
}

new const g_AcTypeNames[E_AC_TYPE][] =
{
    "oruzje", "jetpack", "HP", "armor", "brzina peske", "brzina vozila", "teleport"
};

enum E_AC_STATE
{
    WEAPON:acWeapon[AC_WEAPON_SLOTS],
    Float:acArmour,
    Float:acLastX,
    Float:acLastY,
    Float:acLastZ,
    bool:acHasLastPos,
    acGraceUntil,
    acLastAlert[E_AC_TYPE],
    acChatWindowStart,
    acChatCount,
    acRepeatCount,
    acLastMessage[128],
    acFloodWarnings,
    acFloodWarningStart,
    acCmdWindowStart,
    acCmdCount,
    PlayerText:acSpecText
}

new g_Ac[MAX_PLAYERS][E_AC_STATE];
new g_AcTimer;
