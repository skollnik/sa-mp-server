enum E_BIZ_INTERIOR
{
    biName[40],
    biInterior,
    Float:biX,
    Float:biY,
    Float:biZ,
    Float:biA
}

// The interior each business type uses: the game's own ones the owner picked, and the restaurant mapped in MAP_INTERIOR_ID.
// Once businesses store an interior by index, new ones are only appended.
new const g_BizInteriors[][E_BIZ_INTERIOR] =
{
    { "24/7",                           17, -25.8845,  -185.8689, 1003.5469, 0.0 },
    { "Oruzje",                         7,  314.8210,  -141.4320, 999.6016,  0.0 },
    { "Restoran (kineski)",             MAP_INTERIOR_ID, 1026.8577, -1309.5942, -20.8354, 0.0 },
    { "Brza hrana (Cluckin' Bell)",     9,  369.5795,  -4.4873,   1001.8589, 0.0 },
    { "Bar (Ten Green Bottles)",        11, 501.9810,  -69.1502,  998.7578,  0.0 },
    { "Klub (Pig Pen)",                 2,  1204.8099, -11.5868,  1000.9219, 0.0 },
    { "Odeca (Binco)",                  15, 207.7380,  -109.0200, 1005.1328, 0.0 }
};

forward bool:Biz_OnDialogResponse(playerid, dialogid, response, listitem);
