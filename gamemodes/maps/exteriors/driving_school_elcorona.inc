#define SCHOOL_BLUE             (0xFF0C8AF7)

// The driving school in El Corona: the GTA building at 2045,-1903 rebuilt with new materials
// (the pole sign, the gang mural and the tagged shop windows are part of the model itself).
stock Map_DrivingSchoolElCorona()
{
    new STREAMER_TAG_OBJECT:building = Map_Exterior(5134, 2045.49219, -1903.61719, 16.18750, 0.00000, 0.00000, 0.00000);
    SetDynamicObjectMaterial(building, 0, 14674, "civic02cj", "sl_hotelwallplain1");
    SetDynamicObjectMaterial(building, 2, 2361, "shopping_freezers", "white", SCHOOL_BLUE);
    SetDynamicObjectMaterial(building, 3, 9485, "roadsfe", "ws_carpark2");
    SetDynamicObjectMaterial(building, 4, 4007, "lanblokc", "bonavwall1_LAn");
    SetDynamicObjectMaterial(building, 6, 3741, "cehillhse14", "comptwindo4");
    SetDynamicObjectMaterial(building, 7, 12931, "sw_brewery", "sw_brewbrick01");
    SetDynamicObjectMaterial(building, 8, 5810, "lawnstripm", "sl_lavicdtwall1");
    SetDynamicObjectMaterial(building, 9, 3440, "airportpillar", "metalic_64");
    SetDynamicObjectMaterialText(building, 10, "L", 110, "Arial", 56, 1, 0xFFFFFFFF, SCHOOL_BLUE, 1);
    SetDynamicObjectMaterialText(building, 11, "AUTO\nSKOLA", 130, "Arial", 100, 1, 0xFFFFFFFF, SCHOOL_BLUE, 1);

    new STREAMER_TAG_OBJECT:sign = Map_Exterior(19477, 2045.68000, -1908.00000, 15.87500, 0.00000, 0.00000, 0.00000);
    SetDynamicObjectMaterialText(sign, 0, "AUTO SKOLA", 130, "Arial", 72, 1, 0xFFFFFFFF, SCHOOL_BLUE, 1);

    Map_Exterior(1536, 2044.89124, -1909.50879, 12.56572, 0.00000, 0.00000, 90.84454);
    Map_Exterior(1536, 2044.85901, -1906.50696, 12.56572, 0.00000, 0.00000, 268.81448);
    Map_Exterior(18981, 2059.39844, -1910.08118, 11.87875, 0.00000, 90.00000, 0.00000);

    // The middle of the lot stays clear: the test vehicle appears there.
    Map_Exterior(1238, 2070.90000, -1919.00000, 12.87000, 0.00000, 0.00000, 0.00000);
    Map_Exterior(1238, 2070.90000, -1916.00000, 12.87000, 0.00000, 0.00000, 0.00000);
    Map_Exterior(1238, 2070.90000, -1910.00000, 12.87000, 0.00000, 0.00000, 0.00000);
    Map_Exterior(1238, 2070.90000, -1907.00000, 12.87000, 0.00000, 0.00000, 0.00000);
    Map_Exterior(1238, 2053.40000, -1920.80000, 12.87000, 0.00000, 0.00000, 0.00000);
    Map_Exterior(1238, 2059.80000, -1920.80000, 12.87000, 0.00000, 0.00000, 0.00000);
    Map_Exterior(19966, 2052.60000, -1921.60000, 12.55000, 0.00000, 0.00000, 180.00000);

    Map_Exterior(948, 2045.30000, -1905.60000, 12.55000, 0.00000, 0.00000, 0.00000);
    Map_Exterior(948, 2045.30000, -1910.40000, 12.55000, 0.00000, 0.00000, 0.00000);
    Map_Exterior(1280, 2045.45000, -1914.00000, 12.95000, 0.00000, 0.00000, 180.00000);
    Map_Exterior(1339, 2045.30000, -1916.00000, 13.20000, 0.00000, 0.00000, 0.00000);
}

stock Map_DrivingSchoolElCorona_Remove(playerid)
{
    RemoveBuildingForPlayer(playerid, 5134, 2045.4922, -1903.6172, 16.1875, 0.25);
    RemoveBuildingForPlayer(playerid, 5321, 2045.4922, -1903.6172, 16.1875, 0.25);
}
