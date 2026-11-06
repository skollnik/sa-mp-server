#define SCHOOL_BLUE             (0xFF0C8AF7)

// The driving school in El Corona: the GTA building at 2045,-1903 with a blue band, the sign above the entrance
// and its pole sign at the corner of the lot.
stock Map_DrivingSchoolElCorona()
{
    new STREAMER_TAG_OBJECT:building = Map_Exterior(5134, 2045.49219, -1903.61719, 16.18750, 0.00000, 0.00000, 0.00000);
    SetDynamicObjectMaterial(building, 8, 5810, "lawnstripm", "sl_lavicdtwall1");
    SetDynamicObjectMaterial(building, 4, 4007, "lanblokc", "bonavwall1_LAn");
    SetDynamicObjectMaterial(building, 2, 2361, "shopping_freezers", "white", SCHOOL_BLUE);
    SetDynamicObjectMaterial(building, 7, 12931, "sw_brewery", "sw_brewbrick01");
    SetDynamicObjectMaterial(building, 3, 9485, "roadsfe", "ws_carpark2");

    // The building's own pole sign at the corner of the lot becomes the school's.
    SetDynamicObjectMaterial(building, 9, 3440, "airportpillar", "metalic_64");
    SetDynamicObjectMaterialText(building, 10, "L", 110, "Arial", 56, 1, 0xFFFFFFFF, SCHOOL_BLUE, 1);
    SetDynamicObjectMaterialText(building, 11, "AUTO\nSKOLA", 130, "Arial", 100, 1, 0xFFFFFFFF, SCHOOL_BLUE, 1);

    // The background is transparent so the letters sit on the blue band and can be taller than the plane fits.
    new STREAMER_TAG_OBJECT:sign = Map_Exterior(19482, 2045.68000, -1908.00000, 15.87500, 0.00000, 0.00000, 0.00000);
    SetDynamicObjectMaterialText(sign, 0, "AUTO SKOLA", 130, "Arial", 80, 1, 0xFFFFFFFF, 0x00000000, 1);

    Map_Exterior(1536, 2044.89124, -1909.50879, 12.56572, 0.00000, 0.00000, 90.84454);
    Map_Exterior(1536, 2044.85901, -1906.50696, 12.56572, 0.00000, 0.00000, 268.81448);
    Map_Exterior(18981, 2059.39844, -1910.08118, 11.87875, 0.00000, 90.00000, 0.00000);
}

stock Map_DrivingSchoolElCorona_Remove(playerid)
{
    RemoveBuildingForPlayer(playerid, 5134, 2045.4922, -1903.6172, 16.1875, 0.25);
    RemoveBuildingForPlayer(playerid, 5321, 2045.4922, -1903.6172, 16.1875, 0.25);
}
