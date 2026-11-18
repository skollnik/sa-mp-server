// Everyday clothes sold in the clothing shop: no VIP skins, uniforms, gang skins or the staff's and the mechanic's skins.
static const g_ShopSkinsMale[] =
{
    2, 14, 15, 19, 20, 21, 22, 23, 24, 25, 26, 29, 30, 32, 33, 34, 36, 37, 44, 45, 48, 66, 67, 72, 73, 94, 95, 96, 119, 128,
    142, 143, 144, 146, 154, 158, 161, 176, 177, 180, 184, 186, 187, 206, 223, 228, 229, 234, 236, 241, 242, 258, 259, 261, 262, 289, 299
};
static const g_ShopSkinsFemale[] =
{
    9, 10, 13, 31, 38, 39, 41, 53, 54, 76, 77, 88, 89, 90, 129, 130, 131, 148, 151, 157, 190, 193, 194, 195, 196, 197, 198, 199, 201, 214,
    215, 218, 219, 224, 225, 226, 231, 232, 263, 298
};

static g_ClothesPick[MAX_PLAYERS];

static stock bool:Clothes_InList(skin, const list[], size)
{
    for (new i = 0; i < size; i++)
    {
        if (list[i] == skin)
        {
            return true;
        }
    }
    return false;
}

static stock bool:Clothes_IsMale(skin)
{
    return Clothes_InList(skin, g_ShopSkinsMale, sizeof(g_ShopSkinsMale)) || Clothes_InList(skin, g_StarterSkinsMale, sizeof(g_StarterSkinsMale));
}

static stock bool:Clothes_IsFemale(skin)
{
    return Clothes_InList(skin, g_ShopSkinsFemale, sizeof(g_ShopSkinsFemale)) || Clothes_InList(skin, g_StarterSkinsFemale, sizeof(g_StarterSkinsFemale));
}

// Only everyday clothes go into a bag or a wardrobe: a VIP skin is picked again in /vip, a uniform belongs to the job.
stock bool:Clothes_CanBag(skin)
{
    return Clothes_IsMale(skin) || Clothes_IsFemale(skin);
}

stock bool:Clothes_IsForGender(playerid, skin)
{
    return g_Player[playerid][pGender] == GENDER_FEMALE ? Clothes_IsFemale(skin) : Clothes_IsMale(skin);
}

// Not while a uniform or the staff skin is on, so the change is never hidden under it.
stock bool:Clothes_CanChange(playerid)
{
    if (g_Staff[playerid][sAdminDuty] || Job_Skin(playerid) != -1)
    {
        Msg_Error(playerid, "Prvo skinite uniformu (kraj smene ili admin duznosti).");
        return false;
    }
    if (Inv_IsBusy(playerid))
    {
        Msg_Error(playerid, "Presvlaci se peske.");
        return false;
    }
    return true;
}

// Puts the skin on. Returns the old skin when it can go into a bag, otherwise 0 (it is not kept).
stock Clothes_Wear(playerid, skin)
{
    new old = g_Player[playerid][pSkin];
    g_Player[playerid][pSkin] = skin;
    Staff_UpdateLook(playerid);
    Account_Save(playerid);
    Chat_Action(playerid, "se presvlaci.");
    return Clothes_CanBag(old) ? old : 0;
}

// A bag used from the inventory: at home (own house or rented room) or in the clothing shop.
stock Clothes_WearBag(playerid, inv_slot)
{
    new skin = g_Inv[playerid][inv_slot][isAmount], house = House_FindInsideAnywhere(playerid);
    if ((house == -1 || !House_CanLiveIn(playerid, house)) && !Point_IsNear(playerid, POINT_CLOTHES))
    {
        Msg_Error(playerid, "Presvlaci se kod kuce (u svojoj kuci ili iznajmljenoj sobi) ili u prodavnici odece.");
        return;
    }
    if (!Clothes_IsForGender(playerid, skin))
    {
        Msg_Error(playerid, "Ta odeca nije za vas pol.");
        return;
    }
    if (!Clothes_CanChange(playerid))
    {
        return;
    }
    new old = Clothes_Wear(playerid, skin);
    // The same bag takes the old clothes, so a full inventory never loses them.
    Inv_SetSlot(playerid, inv_slot, old ? ITEM_CLOTHES : ITEM_NONE, old);
    if (old)
    {
        Msg_Success(playerid, "Obukli ste skin %d; stara odeca (skin %d) je u kesi u inventaru.", skin, old);
    }
    else
    {
        Msg_Success(playerid, "Obukli ste skin %d. Stara odeca se ne cuva (VIP ili posebna odeca).", skin);
    }
}

stock Clothes_ShowShop(playerid)
{
    new title[32];
    format(title, sizeof(title), "Odeca ($%d)", CLOTHES_PRICE);
    if (g_Player[playerid][pGender] == GENDER_FEMALE)
    {
        Preview_Show(playerid, PREVIEW_CLOTHES_SHOP, PREVIEW_KIND_SKIN, title, g_ShopSkinsFemale, sizeof(g_ShopSkinsFemale));
    }
    else
    {
        Preview_Show(playerid, PREVIEW_CLOTHES_SHOP, PREVIEW_KIND_SKIN, title, g_ShopSkinsMale, sizeof(g_ShopSkinsMale));
    }
}

stock Clothes_OnSkinSelected(playerid, index)
{
    new bool:female = g_Player[playerid][pGender] == GENDER_FEMALE;
    if (index < 0 || index >= (female ? sizeof(g_ShopSkinsFemale) : sizeof(g_ShopSkinsMale)) || !Point_Require(playerid, POINT_CLOTHES))
    {
        return;
    }
    g_ClothesPick[playerid] = female ? g_ShopSkinsFemale[index] : g_ShopSkinsMale[index];
    new body[128], title[32];
    format(body, sizeof(body), "Obuci odmah (stara odeca ide u kesu u inventaru)\nSpakuj u kesu (u inventar)");
    format(title, sizeof(title), "Skin %d za $%d", g_ClothesPick[playerid], CLOTHES_PRICE);
    ShowPlayerDialog(playerid, DIALOG_CLOTHES_BUY, DIALOG_STYLE_LIST, title, body, "Kupi", "Odustani");
}

static stock Clothes_Buy(playerid, bool:wear)
{
    new skin = g_ClothesPick[playerid];
    if (!Point_Require(playerid, POINT_CLOTHES) || !Clothes_IsForGender(playerid, skin))
    {
        return;
    }
    if (wear && !Clothes_CanChange(playerid))
    {
        return;
    }
    if (!wear && !Inv_CanAdd(playerid, ITEM_CLOTHES, skin))
    {
        Msg_Error(playerid, "Nemate mesta u inventaru za kesu.");
        return;
    }
    new reason[32];
    format(reason, sizeof(reason), "kupovina odece (skin %d)", skin);
    if (!Money_Take(playerid, CLOTHES_PRICE, reason))
    {
        return;
    }
    if (!wear)
    {
        Inv_Add(playerid, ITEM_CLOTHES, skin);
        Account_Save(playerid);
        Msg_Success(playerid, "Kupili ste odecu (skin %d) za $%d. Kesa je u inventaru; oblaci se kod kuce ili ovde (/inv).", skin, CLOTHES_PRICE);
        return;
    }
    new old = Clothes_Wear(playerid, skin);
    if (old == 0)
    {
        Msg_Success(playerid, "Kupili ste i obukli skin %d. Stara odeca se ne cuva (VIP ili posebna odeca).", skin);
    }
    else if (Inv_Add(playerid, ITEM_CLOTHES, old))
    {
        Msg_Success(playerid, "Kupili ste i obukli skin %d. Stara odeca (skin %d) je u kesi u inventaru.", skin, old);
    }
    else
    {
        Msg_Success(playerid, "Kupili ste i obukli skin %d. Nemate mesta u inventaru, pa je stara odeca ostala u prodavnici.", skin);
    }
}

stock bool:Clothes_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
    #pragma unused inputtext
    if (dialogid != DIALOG_CLOTHES_BUY)
    {
        return false;
    }
    if (response && (listitem == 0 || listitem == 1))
    {
        Clothes_Buy(playerid, listitem == 0);
    }
    return true;
}
