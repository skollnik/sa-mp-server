// Attached-object slots 0-2 belong to the accessories (one per body part); other systems take slot 3 and up.
#define ACC_FIRST_SLOT          (0)
// Limits on what the mouse editor may save, so nobody hides a huge object over the screen or far from the body.
#define ACC_MAX_OFFSET          (0.5)
#define ACC_MIN_SCALE           (0.5)
#define ACC_MAX_SCALE           (1.5)

#define BONE_SPINE              (1)
#define BONE_HEAD               (2)
#define BONE_LEFT_HAND          (5)

enum E_ACC_PART
{
    ACC_PART_HEAD,
    ACC_PART_FACE,
    ACC_PART_BACK
}

new const g_AccPartNames[E_ACC_PART][] = { "Glava", "Lice", "Ledja i ruka" };

enum E_ACC_PLACE
{
    Float:apX,
    Float:apY,
    Float:apZ,
    Float:apRotX,
    Float:apRotY,
    Float:apRotZ,
    Float:apScaleX,
    Float:apScaleY,
    Float:apScaleZ
}

enum E_ACC_INFO
{
    E_ITEM:aiItem,
    E_ACC_PART:aiPart,
    aiBone,
    aiPrice,
    Float:aiPlace[E_ACC_PLACE]
}

// The starting place fits most skins; each player moves it with the mouse in /dodaci and that is saved per item.
new const g_AccInfo[][E_ACC_INFO] =
{
    { ITEM_GLASSES_SUN,     ACC_PART_FACE, BONE_HEAD,      150, { 0.103, 0.031, -0.001, 0.0, 90.0, 90.5, 1.0, 1.058, 1.0 } },
    { ITEM_GLASSES_AVIATOR, ACC_PART_FACE, BONE_HEAD,      200, { 0.103, 0.031, -0.001, 0.0, 90.0, 90.5, 1.0, 1.058, 1.0 } },
    { ITEM_GLASSES_DARK,    ACC_PART_FACE, BONE_HEAD,      250, { 0.103, 0.031, -0.001, 0.0, 90.0, 90.5, 1.0, 1.058, 1.0 } },
    { ITEM_HAT_BOWLER,      ACC_PART_HEAD, BONE_HEAD,      350, { 0.149, -0.014, -0.001, 0.0, 179.4, -154.4, 1.0, 1.0, 1.07 } },
    { ITEM_HAT_PANAMA,      ACC_PART_HEAD, BONE_HEAD,      300, { 0.141, 0.016, -0.005, 0.0, 90.0, 93.5, 1.0, 1.0, 1.0 } },
    { ITEM_HAT_KNIT,        ACC_PART_HEAD, BONE_HEAD,      150, { 0.141, 0.016, -0.002, 0.0, 90.0, 89.9, 1.0, 1.0, 1.0 } },
    { ITEM_CAP,             ACC_PART_HEAD, BONE_HEAD,      200, { 0.141, 0.016, -0.014, -175.4, 174.0, -178.3, 1.0, 1.0, 1.072 } },
    { ITEM_BACKPACK,        ACC_PART_BACK, BONE_SPINE,     600, { -0.058, -0.111, 0.0, 0.0, 0.0, 0.0, 0.759, 0.929, 0.77 } },
    // Carried in the left hand, so it never clashes with a weapon in the right one.
    { ITEM_BAG,             ACC_PART_BACK, BONE_LEFT_HAND, 400, { 0.222, 0.047, -0.017, 60.2, -83.0, 0.0, 0.454, 0.811, 0.9 } }
};

new Float:g_AccPlace[MAX_PLAYERS][sizeof(g_AccInfo)][E_ACC_PLACE];
// Index into g_AccInfo of what is worn on each part, or -1.
new g_AccWorn[MAX_PLAYERS][E_ACC_PART];
new g_AccEditing[MAX_PLAYERS];
new g_AccSelected[MAX_PLAYERS];
new g_AccListed[MAX_PLAYERS][sizeof(g_AccInfo)];

forward bool:Acc_IsAccessory(E_ITEM:item);
forward bool:Acc_IsWorn(playerid, E_ITEM:item);
forward bool:Acc_OnDialogResponse(playerid, dialogid, response, listitem);
