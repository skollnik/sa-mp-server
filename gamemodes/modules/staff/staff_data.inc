forward bool:Staff_IsAdmin(playerid);
forward bool:Staff_IsHelper(playerid);
forward bool:Staff_IsStaff(playerid);
forward bool:Staff_IsAdminOnDuty(playerid);
forward bool:Staff_CheckAdmin(playerid, level);
forward bool:Staff_CheckHelper(playerid, level);
forward bool:Staff_CheckTarget(playerid, targetid, const self_error[] = "");
forward bool:Staff_CheckOutranks(playerid, targetid);

#define ADMIN_LEVEL_TRIAL       (1)
#define ADMIN_LEVEL_ADMIN       (2)
#define ADMIN_LEVEL_SENIOR      (3)
#define ADMIN_LEVEL_HEAD        (4)
#define ADMIN_LEVEL_MANAGEMENT  (5)

#define HELPER_LEVEL_HELPER     (1)
#define HELPER_LEVEL_SENIOR     (2)
#define HELPER_LEVEL_HEAD       (3)

#define STAFF_TAG_OFFSET_Z      (0.45)
#define STAFF_TAG_DRAW_DISTANCE (20.0)

// The usual staff outfit while on admin duty (clothes shop staff models).
#define ADMIN_DUTY_SKIN_MALE    (217)
#define ADMIN_DUTY_SKIN_FEMALE  (211)

// Admin jail: the cell in the LSPD interior, in its own virtual world so jailed players only see each other.
#define JAIL_X                  (264.63)
#define JAIL_Y                  (77.57)
#define JAIL_Z                  (1001.04)
#define JAIL_INTERIOR           (6)
#define JAIL_WORLD              (9999)
#define JAIL_ESCAPE_RADIUS      (15.0)

#define MAX_WARNINGS            (3)
#define WARNING_BAN_DAYS        (3)
#define TRIAL_MAX_JAIL_MINUTES  (30)
#define ADMIN_MAX_BAN_DAYS      (7)
#define MAX_PUNISH_MINUTES      (1440)

new const g_AdminRankNames[][] =
{
    "Igrac", "Admin pocetnik", "Admin", "Visi admin", "Glavni admin", "Menadzment", "Vlasnik"
};

new const g_HelperRankNames[][] =
{
    "Igrac", "Helper", "Iskusni helper", "Glavni helper"
};

#define MAX_REPORTS             (30)
#define REPORT_COOLDOWN_S       (60)
#define ASK_COOLDOWN_S          (30)
#define NEWBIE_COOLDOWN_S       (30)
#define NEWBIE_MAX_PLAYER_LEVEL (3)

enum E_REPORT
{
    bool:rUsed,
    rReporter,
    rTarget,
    rReason[96],
    rCreatedAt
}

new g_Reports[MAX_REPORTS][E_REPORT];

enum E_STAFF_STATE
{
    bool:sAdminDuty,
    bool:sHelperDuty,
    bool:sSpectating,
    bool:sRestoreAfterSpec,
    sSpecTarget,
    Float:sSpecX,
    Float:sSpecY,
    Float:sSpecZ,
    Float:sSpecA,
    sSpecInterior,
    sSpecWorld,
    bool:sFrozen,
    sLastReport,
    sLastAsk,
    sLastNewbie,
    bool:sHasOpenQuestion,
    STREAMER_TAG_3D_TEXT_LABEL:sDutyTag,
    bool:sPmSpy
}

new g_Staff[MAX_PLAYERS][E_STAFF_STATE];

new bool:g_IsAdminVehicle[MAX_VEHICLES];
new g_AdminVehicleOwner[MAX_VEHICLES];

enum E_TELEPORT
{
    tName[32],
    Float:tX,
    Float:tY,
    Float:tZ,
    tInterior
}

new const g_AdminTeleports[][E_TELEPORT] =
{
    { "LS - Aerodrom",          1642.30, -2333.63, 13.55, 0 },
    { "LS - Unity stanica",     1742.90, -1861.40, 13.58, 0 },
    { "LS - Pershing Square",   1479.60, -1639.60, 14.10, 0 },
    { "LS - Policija (LSPD)",   1544.60, -1675.80, 13.56, 0 },
    { "LS - Bolnica All Saints",1178.70, -1323.90, 14.10, 0 },
    { "LS - Grove Street",      2495.30, -1687.40, 13.52, 0 },
    { "LS - Plaza Santa Maria", 330.00,  -1800.00, 4.60,  0 },
    { "San Fierro - centar",    -1990.00, 288.00,  34.20, 0 },
    { "Las Venturas - Strip",   2037.00, 1343.00,  10.82, 0 }
};
