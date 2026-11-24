// Business exterior (step 4.5): SEVEN 24 supermarket signs on the game's 24/7 by Unity station.
stock Map_BizSevenUnity()
{
    new STREAMER_TAG_OBJECT:o;
    Map_Exterior(18659, 1850.007813, -1876.897949, 14.359398, 0.000000, 0.000000, 90.000000);
    o = Map_Exterior(2789, 1848.864868, -1873.752563, 16.412804, 0.000001, -0.000025, -90.700462);
    SetDynamicObjectMaterialText(o, 0, "SUPERMARKET", 130, "Impact", 60, 0, 0xFFFFFFFF, 0, 0);
    SetDynamicObjectMaterial(o, 1, 19480, "signsurf", "sign", 0);
    o = Map_Exterior(2789, 1848.732422, -1872.360962, 18.142807, -0.000048, -0.000003, 89.299782);
    SetDynamicObjectMaterialText(o, 0, "-", 30, "Impact", 199, 0, 0xFFE42C00, 0, 1);
    SetDynamicObjectMaterial(o, 1, 19480, "signsurf", "sign", 0);
    o = Map_Exterior(2789, 1848.892090, -1872.363403, 18.142807, 0.000001, -0.000025, -90.700462);
    SetDynamicObjectMaterialText(o, 0, "-", 30, "Impact", 199, 0, 0xFFE42C00, 0, 1);
    SetDynamicObjectMaterial(o, 1, 19480, "signsurf", "sign", 0);
    o = Map_Exterior(19325, 1848.841309, -1871.195435, 15.542011, -0.000011, 360.000000, -0.700094);
    SetDynamicObjectMaterial(o, 0, 1349, "cj_dyn_prop", "CJ_STROLLY", 0);
    o = Map_Exterior(13646, 1848.266357, -1869.771362, 17.505615, 89.999992, 1575.900513, -46.600689);
    SetDynamicObjectMaterial(o, 0, 19480, "signsurf", "sign", 0);
    SetDynamicObjectMaterial(o, 1, 16640, "a51", "scratchedmetal", 0);
    o = Map_Exterior(18664, 1848.773682, -1869.774292, 16.961346, -0.000025, -0.000001, -0.700125);
    SetDynamicObjectMaterialText(o, 0, "SEVEN", 100, "Impact", 80, 0, 0xFFE72C03, 0, 1);
    o = Map_Exterior(18664, 1848.773682, -1869.774292, 17.731350, -0.000025, -0.000001, -0.700125);
    SetDynamicObjectMaterialText(o, 0, "24", 100, "Impact", 199, 0, 0xFF0E7908, 0, 1);
    o = Map_Exterior(13646, 1849.415894, -1869.785034, 17.505615, 89.999992, 1755.476685, -46.176987);
    SetDynamicObjectMaterial(o, 0, 19480, "signsurf", "sign", 0);
    SetDynamicObjectMaterial(o, 1, 10765, "airportgnd_sfse", "white", 0);
}
