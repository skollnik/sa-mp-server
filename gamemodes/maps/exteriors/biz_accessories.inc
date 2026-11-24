// Business exterior (step 4.5): the accessory shop front by Unity station, for a clothes shop; its inside is maps/interiors/biz_accessories.inc.
stock Map_BizAccessories()
{
    new STREAMER_TAG_OBJECT:o;
    o = Map_Exterior(2789, 1834.195313, -1844.875977, 17.379280, -0.000015, -2.800017, -90.000183);
    SetDynamicObjectMaterialText(o, 0, "AKSESOARA", 130, "Segoe Print", 50, 1, 0xFFFFFFFF, 0, 1);
    SetDynamicObjectMaterial(o, 1, 19480, "signsurf", "sign", 0);
    o = Map_Exterior(2789, 1834.195313, -1844.852051, 17.868685, -0.000015, -2.800017, -90.000183);
    SetDynamicObjectMaterialText(o, 0, "PRODAVNICA", 130, "Impact", 60, 0, 0xFFFFFFFF, 0, 1);
    SetDynamicObjectMaterial(o, 1, 19480, "signsurf", "sign", 0);
    o = Map_Exterior(16002, 1834.458008, -1844.806152, 9.161342, -0.000015, 0.000000, -90.000198);
    SetDynamicObjectMaterial(o, 0, 19480, "signsurf", "sign", 0);
    SetDynamicObjectMaterial(o, 1, 19480, "signsurf", "sign", 0);
    SetDynamicObjectMaterial(o, 2, 19480, "signsurf", "sign", 0);
    SetDynamicObjectMaterial(o, 3, 19480, "signsurf", "sign", 0);
    SetDynamicObjectMaterial(o, 4, 19480, "signsurf", "sign", 0);
    SetDynamicObjectMaterial(o, 5, 18632, "fishingrod", "rodpole2", 0);
    SetDynamicObjectMaterial(o, 6, 10101, "2notherbuildsfe", "Bow_Abpave_Gen", 0);
    o = Map_Exterior(16002, 1834.428223, -1842.637695, 9.161342, -0.000015, 0.000000, -90.000198);
    SetDynamicObjectMaterial(o, 0, 19480, "signsurf", "sign", 0);
    SetDynamicObjectMaterial(o, 1, 19480, "signsurf", "sign", 0);
    SetDynamicObjectMaterial(o, 2, 19480, "signsurf", "sign", 0);
    SetDynamicObjectMaterial(o, 4, 19480, "signsurf", "sign", 0);
    SetDynamicObjectMaterial(o, 5, 19480, "signsurf", "sign", 0);
    o = Map_Exterior(19466, 1833.916992, -1841.517578, 17.967531, 7.199991, -0.000011, -179.999771);
    SetDynamicObjectMaterial(o, 0, 19029, "matglasses", "glassestype6map", 0);
    o = Map_Exterior(16002, 1834.428223, -1840.566406, 9.161342, -0.000015, 0.000000, -90.000198);
    SetDynamicObjectMaterial(o, 0, 19480, "signsurf", "sign", 0);
    SetDynamicObjectMaterial(o, 1, 19480, "signsurf", "sign", 0);
    SetDynamicObjectMaterial(o, 2, 19480, "signsurf", "sign", 0);
    SetDynamicObjectMaterial(o, 3, 19480, "signsurf", "sign", 0);
    SetDynamicObjectMaterial(o, 4, 19480, "signsurf", "sign", 0);
    SetDynamicObjectMaterial(o, 5, 19523, "sampicons", "yellograd32", 0);
    SetDynamicObjectMaterial(o, 6, 10101, "2notherbuildsfe", "Bow_Abpave_Gen", 0);
    o = Map_Exterior(19466, 1833.916992, -1839.315430, 17.689272, -7.199991, 0.000018, -0.000158);
    SetDynamicObjectMaterial(o, 0, 19029, "matglasses", "glassestype8map", 0);
}
