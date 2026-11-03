// Candidate driving school in El Corona: the GTA building at 2045,-1903 rebuilt with other textures, a fenced yard and a floor that keeps vehicles from falling through.
stock Map_DrivingSchoolElCorona()
{
    new STREAMER_TAG_OBJECT:building = Map_Exterior(5134, 2045.49219, -1903.61719, 16.18750, 0.00000, 0.00000, 0.00000);
    SetDynamicObjectMaterial(building, 8, 5810, "lawnstripm", "sl_lavicdtwall1");
    SetDynamicObjectMaterial(building, 4, 4007, "lanblokc", "bonavwall1_LAn");
    SetDynamicObjectMaterial(building, 2, 2361, "shopping_freezers", "white", 0xFF0C8AF7);
    SetDynamicObjectMaterial(building, 7, 12931, "sw_brewery", "sw_brewbrick01");
    SetDynamicObjectMaterial(building, 3, 9485, "roadsfe", "ws_carpark2");
    new STREAMER_TAG_OBJECT:fence0 = Map_Exterior(19377, 2071.58643, -1902.00854, 8.01126, 0.00000, 0.00000, 0.20100);
    new STREAMER_TAG_OBJECT:fence1 = Map_Exterior(19377, 2066.78320, -1922.25342, 8.01126, 0.00000, 0.00000, 89.83002);
    new STREAMER_TAG_OBJECT:fence2 = Map_Exterior(19377, 2071.66626, -1917.53564, 8.01126, 0.00000, 0.00000, 0.20100);
    new STREAMER_TAG_OBJECT:fence3 = Map_Exterior(19377, 2049.86963, -1922.22595, 8.01126, 0.00000, 0.00000, 89.83002);
    SetDynamicObjectMaterial(fence0, 0, 3310, "sw_poorhouse", "sw_wallbrick_06");
    SetDynamicObjectMaterial(fence1, 0, 3310, "sw_poorhouse", "sw_wallbrick_06");
    SetDynamicObjectMaterial(fence2, 0, 3310, "sw_poorhouse", "sw_wallbrick_06");
    SetDynamicObjectMaterial(fence3, 0, 3310, "sw_poorhouse", "sw_wallbrick_06");
    Map_Exterior(1536, 2044.89124, -1909.50879, 12.56572, 0.00000, 0.00000, 90.84454);
    Map_Exterior(1536, 2044.85901, -1906.50696, 12.56572, 0.00000, 0.00000, 268.81448);
    Map_Exterior(18981, 2059.39844, -1910.08118, 11.87875, 0.00000, 90.00000, 0.00000);
}

stock Map_DrivingSchoolElCorona_Remove(playerid)
{
    RemoveBuildingForPlayer(playerid, 5134, 2045.4922, -1903.6172, 16.1875, 0.25);
    RemoveBuildingForPlayer(playerid, 5321, 2045.4922, -1903.6172, 16.1875, 0.25);
}
