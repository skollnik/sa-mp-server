// Business exterior (step 4.5): SEVEN 24 supermarket signs on the game's 24/7 in Commerce.
stock Map_BizSevenCommerce()
{
    new STREAMER_TAG_OBJECT:o;
    o = Map_Exterior(19445, 1366.216309, -1759.140381, 17.677952, 0.000000, 0.000000, 90.000000);
    SetDynamicObjectMaterial(o, 0, 6282, "beafron2_law2", "decoacwallbtmb21_256", 0);
    o = Map_Exterior(19445, 1356.656372, -1759.140381, 17.677952, 0.000000, 0.000000, 90.000000);
    SetDynamicObjectMaterial(o, 0, 6282, "beafron2_law2", "decoacwallbtmb21_256", 0);
    o = Map_Exterior(13646, 1354.581543, -1759.628540, 17.633204, 89.999992, 623.225952, -83.225998);
    SetDynamicObjectMaterial(o, 1, 10765, "airportgnd_sfse", "white", 0);
    SetDynamicObjectMaterial(o, 0, 19480, "signsurf", "sign", 0);
    o = Map_Exterior(18664, 1354.584717, -1758.986694, 17.858936, -0.000022, 0.000000, -89.999969);
    SetDynamicObjectMaterialText(o, 0, "24", 100, "Impact", 199, 0, 0xFF0E7908, 0, 1);
    o = Map_Exterior(18664, 1354.584717, -1758.986694, 17.088932, -0.000022, 0.000000, -89.999969);
    SetDynamicObjectMaterialText(o, 0, "SEVEN", 100, "Impact", 80, 0, 0xFFE72C03, 0, 1);
    o = Map_Exterior(2789, 1351.997314, -1759.136230, 18.270393, 0.000000, -0.000022, 179.999725);
    SetDynamicObjectMaterialText(o, 0, "-", 30, "Impact", 199, 0, 0xFFE42C00, 0, 1);
    SetDynamicObjectMaterial(o, 1, 19480, "signsurf", "sign", 0);
    o = Map_Exterior(2789, 1350.607544, -1759.126099, 16.540390, 0.000000, -0.000022, 179.999725);
    SetDynamicObjectMaterial(o, 1, 19480, "signsurf", "sign", 0);
    SetDynamicObjectMaterialText(o, 0, "SUPERMARKET", 130, "Impact", 60, 0, 0xFFFFFFFF, 0, 0);
    o = Map_Exterior(19445, 1347.056396, -1759.140381, 17.677952, 0.000000, 0.000000, 90.000000);
    SetDynamicObjectMaterial(o, 0, 6282, "beafron2_law2", "decoacwallbtmb21_256", 0);
}
