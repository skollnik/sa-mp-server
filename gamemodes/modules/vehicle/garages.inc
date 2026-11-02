// GTA's mod shops and Pay'n'Spray give free tuning, paint and repairs, so their doors are closed with a gate, as is common
// on roleplay servers; repairs and paint are the mechanic's job.
enum E_GARAGE_GATE
{
    ggModel,
    Float:ggX,
    Float:ggY,
    Float:ggZ,
    Float:ggAngle
}

static const g_GarageGates[][E_GARAGE_GATE] =
{
    // Transfender, Loco Low Co and Wheel Arch Angels
    { 971, 1042.6240, -1026.0254, 30.8978, 0.0 },
    { 985, 2644.9624, -2039.1556, 11.0500, 0.0 },
    { 985, 2386.4036, 1043.5625, 10.1954, 0.0 },
    { 985, -1936.0916, 239.3553, 33.3583, 0.0 },
    { 985, -2716.2993, 217.7056, 3.8178, 270.0 },
    // Pay'n'Spray
    { 985, 2071.5596, -1830.8994, 13.2719, 270.0 },
    { 985, -100.2793, 1111.4160, 20.0133, 0.0 },
    { 985, 1025.8633, -1029.4434, 32.1082, 180.0 },
    { 985, 488.2510, -1735.0195, 10.9695, 352.0 },
    { 985, 720.2256, -462.5410, 15.5609, 0.0 },
    { 985, -1420.8809, 2591.2295, 56.4267, 0.0 },
    { 985, 1968.2570, 2162.1204, 10.6953, 91.5 },
    { 985, -1904.2504, 277.6322, 41.6970, 179.75 },
    { 985, -2425.9316, 1028.0869, 50.6696, 0.0 }
};

stock Garage_Init()
{
    for (new i = 0; i < sizeof(g_GarageGates); i++)
    {
        CreateDynamicObject(g_GarageGates[i][ggModel], g_GarageGates[i][ggX], g_GarageGates[i][ggY], g_GarageGates[i][ggZ],
            0.0, 0.0, g_GarageGates[i][ggAngle], 0, 0);
    }
}
