#define SCHOOL_BLUE             (0xFF0C8AF7)

// Text with a transparent background makes the whole material invisible.
static stock Map_SchoolHide(STREAMER_TAG_OBJECT:objectid, index)
{
    SetDynamicObjectMaterialText(objectid, index, " ", 10, "Arial", 1, 0, 0, 0x00000000, 0);
}

// Text planes are one-sided, so a sign seen from the north and the south is two planes back to back.
static stock Map_SchoolPanel(modelid, Float:x, Float:y, Float:z, const text[], materialsize, fontsize)
{
    new STREAMER_TAG_OBJECT:north = Map_Exterior(modelid, x, y + 0.03, z, 0.00000, 0.00000, 90.00000);
    new STREAMER_TAG_OBJECT:south = Map_Exterior(modelid, x, y - 0.03, z, 0.00000, 0.00000, 270.00000);
    SetDynamicObjectMaterialText(north, 0, text, materialsize, "Arial", fontsize, 1, 0xFFFFFFFF, SCHOOL_BLUE, 1);
    SetDynamicObjectMaterialText(south, 0, text, materialsize, "Arial", fontsize, 1, 0xFFFFFFFF, SCHOOL_BLUE, 1);
}

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

    // The model's own pole sign cannot be moved, so it is hidden and rebuilt on the pavement closer to the road.
    Map_SchoolHide(building, 1);
    Map_SchoolHide(building, 9);
    Map_SchoolHide(building, 10);
    Map_SchoolHide(building, 11);
    Map_Exterior(3440, 2074.50000, -1903.60000, 14.93000, 0.00000, 0.00000, 0.00000);
    Map_SchoolPanel(19477, 2074.50000, -1903.60000, 18.10000, "AUTO\nSKOLA", 130, 100);
    Map_SchoolPanel(19475, 2074.50000, -1903.60000, 19.05000, "L", 30, 40);
    // The hidden pole may keep its collision; the pot gives it something to be.
    Map_Exterior(948, 2070.95000, -1903.75000, 12.55000, 0.00000, 0.00000, 0.00000);

    // The background is transparent so the letters sit on the blue band and can be taller than the plane fits.
    new STREAMER_TAG_OBJECT:sign = Map_Exterior(19482, 2045.68000, -1908.00000, 15.87500, 0.00000, 0.00000, 0.00000);
    SetDynamicObjectMaterialText(sign, 0, "AUTO SKOLA", 130, "Arial", 80, 1, 0xFFFFFFFF, 0x00000000, 1);

    Map_Exterior(1536, 2044.89124, -1909.50879, 12.56572, 0.00000, 0.00000, 90.84454);
    Map_Exterior(1536, 2044.85901, -1906.50696, 12.56572, 0.00000, 0.00000, 268.81448);
    Map_Exterior(18981, 2059.39844, -1910.08118, 11.87875, 0.00000, 90.00000, 0.00000);

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
