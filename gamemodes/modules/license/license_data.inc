// New licenses go at the end: each one is a bit in `accounts`.`licenses`.
enum E_LICENSE
{
    LIC_CAR,
    LIC_MOTO,
    LIC_TRUCK,
    LIC_BOAT,
    LIC_FLY,
    LIC_FISH
}

#define LIC_NONE                (E_LICENSE:-1)

enum E_LIC_QUESTIONS
{
    LIC_Q_NONE,
    LIC_Q_ROAD,
    LIC_Q_BOAT,
    LIC_Q_AIR
}

enum E_LIC_INFO
{
    liName[24],
    liKey[10],
    liPrice,
    liLevel,
    E_LICENSE:liRequires,
    liModel,
    E_LIC_QUESTIONS:liQuestions,
    // 0: no speed limit on the test.
    liMaxSpeed,
    liTimeLimit,
    bool:liAir
}

new const g_LicInfo[E_LICENSE][E_LIC_INFO] =
{
    { "Auto (B)",   "auto",    LIC_PRICE_CAR,   1, LIC_NONE, 405, LIC_Q_ROAD, 90, 15 * 60, false },
    { "Motor (A)",  "motor",   LIC_PRICE_MOTO,  1, LIC_NONE, 462, LIC_Q_ROAD, 90, 15 * 60, false },
    { "Kamion (C)", "kamion",  LIC_PRICE_TRUCK, 3, LIC_CAR,  414, LIC_Q_ROAD, 90, 15 * 60, false },
    { "Brod",       "brod",    LIC_PRICE_BOAT,  3, LIC_NONE, 473, LIC_Q_BOAT, 0,  5 * 60,  false },
    { "Letenje",    "letenje", LIC_PRICE_FLY,   5, LIC_NONE, 487, LIC_Q_AIR,  0,  8 * 60,  true },
    { "Ribolov",    "ribolov", LIC_PRICE_FISH,  1, LIC_NONE, 0,   LIC_Q_NONE, 0,  0,       false }
};

// The vehicle a test runs in, by its tester; set to the player while the test lasts.
new g_TestVehicleOwner[MAX_VEHICLES] = { INVALID_PLAYER_ID, ... };

forward bool:Lic_Has(playerid, E_LICENSE:license);
forward E_LICENSE:Lic_ForModel(model);
forward bool:Lic_OnDialogResponse(playerid, dialogid, response, listitem);
