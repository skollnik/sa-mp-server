enum E_BIZ_INTERIOR
{
    biName[40],
    biInterior,
    Float:biX,
    Float:biY,
    Float:biZ,
    Float:biA
}

// Candidate interiors for businesses: GTA's own ones that reference gamemodes use, and mapped ones in MAP_INTERIOR_ID.
// The owner walks through them with /enterijerifirmi and keeps the good ones; once businesses store an interior by
// index, new ones are only appended.
new const g_BizInteriors[][E_BIZ_INTERIOR] =
{
    { "24/7 - igra 1",                  17, -25.8845,  -185.8689, 1003.5469, 0.0 },
    { "24/7 - igra 2",                  10, 6.0912,    -29.2719,  1003.5494, 0.0 },
    { "24/7 - igra 3",                  18, -30.9467,  -89.6096,  1003.5469, 0.0 },
    { "24/7 - igra 4",                  16, -25.1326,  -139.0670, 1003.5469, 0.0 },
    { "24/7 - igra 5",                  4,  -27.3123,  -29.2776,  1003.5573, 0.0 },
    { "24/7 - igra 6",                  6,  -26.6916,  -55.7149,  1003.5469, 0.0 },
    { "24/7 - mapa A (veliki)",         MAP_INTERIOR_ID, 2585.8799, 1435.1289, 1800.9688, 180.0 },
    { "24/7 - mapa B (mali)",           MAP_INTERIOR_ID, 2063.5000, -1428.3000, 3001.1000, 0.0 },
    { "24/7 - mapa C (supermarket)",    MAP_INTERIOR_ID, 1285.8000, -1142.1000, -94.2000, 270.0 },
    { "Benzinska - mapa",               MAP_INTERIOR_ID, 665.4157, -569.5200, 1700.7394, 0.0 },
    { "Oruzje - igra 1",                1,  286.1490,  -40.6444,  1001.5156, 0.0 },
    { "Oruzje - igra 2",                4,  286.8010,  -82.5476,  1001.5156, 0.0 },
    { "Oruzje - igra 3",                6,  296.9200,  -108.0720, 1001.5156, 0.0 },
    { "Oruzje - igra 4",                7,  314.8210,  -141.4320, 999.6016,  0.0 },
    { "Oruzje - igra 5",                6,  316.5250,  -167.7070, 999.5938,  0.0 },
    { "Oruzje - mapa",                  MAP_INTERIOR_ID, -729.5000, 1449.5000, -90.5000, 270.0 },
    { "Hrana - Burger Shot",            10, 375.9625,  -65.8168,  1001.5078, 0.0 },
    { "Hrana - Cluckin' Bell",          9,  369.5795,  -4.4873,   1001.8589, 0.0 },
    { "Hrana - pica",                   5,  373.8257,  -117.2709, 1001.4995, 0.0 },
    { "Hrana - krofne",                 17, 377.0034,  -192.5079, 1000.6328, 0.0 },
    { "Hrana - Jay's diner",            4,  457.3047,  -88.4285,  999.5547,  0.0 },
    { "Hrana - Marco's bistro",         1,  -794.8064, 497.7380,  1376.1953, 0.0 },
    { "Hrana - mapa (restoran)",        MAP_INTERIOR_ID, 1079.0709, 937.4053, 1604.8663, 0.0 },
    { "Bar - Ten Green Bottles",        11, 501.9810,  -69.1502,  998.7578,  0.0 },
    { "Bar - Lil' Probe Inn",           18, -229.1880, 1401.1770, 27.7656,   0.0 },
    { "Klub - Alhambra",                17, 493.3910,  -22.7228,  1000.6797, 0.0 },
    { "Klub - Pig Pen",                 2,  1204.8099, -11.5868,  1000.9219, 0.0 },
    { "Klub - Jizzy's",                 3,  -2636.7190, 1402.9170, 906.4609, 0.0 },
    { "Odeca - Binco",                  15, 207.7380,  -109.0200, 1005.1328, 0.0 },
    { "Odeca - SubUrban",               1,  203.7780,  -48.4924,  1001.8047, 0.0 },
    { "Odeca - ProLaps",                3,  207.0550,  -138.8050, 1003.5078, 0.0 },
    { "Odeca - Victim",                 5,  226.2940,  -7.4315,   1002.2109, 0.0 },
    { "Odeca - Zip",                    18, 161.3910,  -93.1592,  1001.8047, 0.0 },
    { "Odeca - Didier Sachs",           14, 204.3330,  -166.6950, 1000.5234, 0.0 }
};

forward bool:Biz_OnDialogResponse(playerid, dialogid, response, listitem);
