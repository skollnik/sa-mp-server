static g_WardrobeListed[MAX_PLAYERS][WARDROBE_MAX_SLOTS];
static g_WardrobeRows[MAX_PLAYERS];
// The wardrobe place and skin picked; the skin is compared again, since the menu may be answered late.
static g_WardrobePick[MAX_PLAYERS];
static g_WardrobePickSkin[MAX_PLAYERS];
static g_WardrobePutSkin[MAX_PLAYERS][INV_MAX_SLOTS];

stock Wardrobe_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `wardrobe` (" \
        "`account_id` INT UNSIGNED NOT NULL," \
        "`slot` TINYINT UNSIGNED NOT NULL," \
        "`skin` SMALLINT UNSIGNED NOT NULL," \
        "PRIMARY KEY (`account_id`, `slot`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
}

stock Wardrobe_Reset(playerid)
{
    for (new i = 0; i < WARDROBE_MAX_SLOTS; i++)
    {
        g_Wardrobe[playerid][i] = 0;
    }
    g_WardrobeLoaded[playerid] = false;
}

stock Wardrobe_OnLogin(playerid)
{
    new query[96];
    mysql_format(g_SQL, query, sizeof(query), "SELECT `slot`, `skin` FROM `wardrobe` WHERE `account_id` = %d", g_Player[playerid][pDbId]);
    mysql_tquery(g_SQL, query, "OnWardrobeLoaded", "dd", playerid, g_AccountRace[playerid]);
}

forward OnWardrobeLoaded(playerid, race);
public OnWardrobeLoaded(playerid, race)
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }
    new index, skin;
    for (new row = 0; row < cache_num_rows(); row++)
    {
        cache_get_value_name_int(row, "slot", index);
        cache_get_value_name_int(row, "skin", skin);
        if (index >= 0 && index < WARDROBE_MAX_SLOTS)
        {
            g_Wardrobe[playerid][index] = skin;
        }
    }
    g_WardrobeLoaded[playerid] = true;
    return 1;
}

static stock Wardrobe_Set(playerid, index, skin)
{
    new query[128];
    g_Wardrobe[playerid][index] = skin;
    if (skin == 0)
    {
        mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `wardrobe` WHERE `account_id` = %d AND `slot` = %d", g_Player[playerid][pDbId], index);
    }
    else
    {
        mysql_format(g_SQL, query, sizeof(query), "REPLACE INTO `wardrobe` (`account_id`, `slot`, `skin`) VALUES (%d, %d, %d)", g_Player[playerid][pDbId], index, skin);
    }
    mysql_tquery(g_SQL, query);
}

// The wardrobe is the player's, but how much fits depends on the house it stands in.
static stock Wardrobe_Capacity(slot)
{
    static const capacity[E_HOUSE_SIZE] = { 3, 5, 8 };
    return capacity[House_Size(slot)];
}

static stock Wardrobe_Count(playerid)
{
    new count;
    for (new i = 0; i < WARDROBE_MAX_SLOTS; i++)
    {
        if (g_Wardrobe[playerid][i] != 0)
        {
            count++;
        }
    }
    return count;
}

// The house the player can use the wardrobe in right now, or -1 after saying why not.
static stock Wardrobe_HouseHere(playerid)
{
    new slot = House_FindInsideAnywhere(playerid);
    if (slot == -1 || !House_CanLiveIn(playerid, slot))
    {
        Msg_Error(playerid, "Ormar je u vasoj kuci ili iznajmljenoj sobi; udjite unutra.");
        return -1;
    }
    if (!g_WardrobeLoaded[playerid] || !g_InvLoaded[playerid])
    {
        Msg_Error(playerid, "Ormar se jos ucitava, pokusajte ponovo.");
        return -1;
    }
    if (Inv_IsBusy(playerid))
    {
        Msg_Error(playerid, "Ormar se koristi peske.");
        return -1;
    }
    return slot;
}

static stock Wardrobe_Show(playerid)
{
    new slot = Wardrobe_HouseHere(playerid);
    if (slot == -1)
    {
        return;
    }
    new body[384], line[48], count;
    for (new i = 0; i < WARDROBE_MAX_SLOTS; i++)
    {
        if (g_Wardrobe[playerid][i] != 0)
        {
            format(line, sizeof(line), "Odeca (skin %d)\n", g_Wardrobe[playerid][i]);
            strcat(body, line);
            g_WardrobeListed[playerid][count++] = i;
        }
    }
    g_WardrobeRows[playerid] = count;
    strcat(body, "> Ostavi kesu sa odecom iz inventara");
    new title[48];
    format(title, sizeof(title), "Ormar (%d/%d mesta)", count, Wardrobe_Capacity(slot));
    ShowPlayerDialog(playerid, DIALOG_WARDROBE, DIALOG_STYLE_LIST, title, body, "Izaberi", "Zatvori");
}

static stock Wardrobe_ShowPut(playerid)
{
    new body[512], line[48], count;
    for (new i = 0; i < INV_MAX_SLOTS; i++)
    {
        if (g_Inv[playerid][i][isItem] == ITEM_CLOTHES)
        {
            format(line, sizeof(line), "Kesa: skin %d\n", g_Inv[playerid][i][isAmount]);
            strcat(body, line);
            g_WardrobePutSkin[playerid][count] = g_Inv[playerid][i][isAmount];
            g_InvListed[playerid][count++] = i;
        }
    }
    if (count == 0)
    {
        Msg_Error(playerid, "Nemate kesu sa odecom. Kupuje se u prodavnici odece (/kupi).");
        Wardrobe_Show(playerid);
        return;
    }
    g_WardrobeRows[playerid] = count;
    ShowPlayerDialog(playerid, DIALOG_WARDROBE_PUT, DIALOG_STYLE_LIST, "Ostavi u ormar", body, "Ostavi", "Nazad");
}

static stock Wardrobe_Put(playerid, inv_slot, skin)
{
    new slot = Wardrobe_HouseHere(playerid);
    if (slot == -1)
    {
        return;
    }
    if (g_Inv[playerid][inv_slot][isItem] != ITEM_CLOTHES || g_Inv[playerid][inv_slot][isAmount] != skin)
    {
        Msg_Error(playerid, "Te kese vise nemate.");
        Wardrobe_Show(playerid);
        return;
    }
    if (Wardrobe_Count(playerid) >= Wardrobe_Capacity(slot))
    {
        Msg_Error(playerid, "Ormar je pun (%d/%d). Veca kuca ima veci ormar.", Wardrobe_Count(playerid), Wardrobe_Capacity(slot));
        Wardrobe_Show(playerid);
        return;
    }
    for (new i = 0; i < WARDROBE_MAX_SLOTS; i++)
    {
        if (g_Wardrobe[playerid][i] == 0)
        {
            // Out of the inventory before into the wardrobe, so nothing is ever in both.
            Inv_SetSlot(playerid, inv_slot, ITEM_NONE, 0);
            Wardrobe_Set(playerid, i, skin);
            Chat_Action(playerid, "ostavlja odecu u ormar.");
            break;
        }
    }
    Wardrobe_Show(playerid);
}

static stock Wardrobe_Take(playerid, bool:wear)
{
    new slot = Wardrobe_HouseHere(playerid), index = g_WardrobePick[playerid], skin = g_WardrobePickSkin[playerid];
    if (slot == -1)
    {
        return;
    }
    if (g_Wardrobe[playerid][index] != skin || skin == 0)
    {
        Wardrobe_Show(playerid);
        return;
    }
    if (wear)
    {
        if (!Clothes_IsForGender(playerid, skin))
        {
            Msg_Error(playerid, "Ta odeca nije za vas pol.");
            return;
        }
        if (!Clothes_CanChange(playerid))
        {
            return;
        }
        // The clothes taken off go to the same place, so the wardrobe never needs room for them.
        Wardrobe_Set(playerid, index, Clothes_Wear(playerid, skin));
        Msg_Success(playerid, "Obukli ste skin %d iz ormara.", skin);
    }
    else
    {
        if (!Inv_CanAdd(playerid, ITEM_CLOTHES, skin))
        {
            Msg_Error(playerid, "Nemate mesta u inventaru za kesu.");
            return;
        }
        Wardrobe_Set(playerid, index, 0);
        Inv_Add(playerid, ITEM_CLOTHES, skin);
        Chat_Action(playerid, "pakuje odecu iz ormara u kesu.");
    }
    Wardrobe_Show(playerid);
}

cmd:ormar(playerid, params[])
{
    Wardrobe_Show(playerid);
    return 1;
}
alias:ormar("wardrobe")

stock bool:Wardrobe_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
    #pragma unused inputtext
    switch (dialogid)
    {
        case DIALOG_WARDROBE:
        {
            if (!response || listitem < 0 || listitem > g_WardrobeRows[playerid])
            {
                return true;
            }
            if (listitem == g_WardrobeRows[playerid])
            {
                if (Wardrobe_HouseHere(playerid) != -1)
                {
                    Wardrobe_ShowPut(playerid);
                }
                return true;
            }
            new index = g_WardrobeListed[playerid][listitem], title[32];
            g_WardrobePick[playerid] = index;
            g_WardrobePickSkin[playerid] = g_Wardrobe[playerid][index];
            format(title, sizeof(title), "Odeca (skin %d)", g_Wardrobe[playerid][index]);
            ShowPlayerDialog(playerid, DIALOG_WARDROBE_ITEM, DIALOG_STYLE_LIST, title, "Obuci (sadasnja odeca ide na njeno mesto)\nSpakuj u kesu (u inventar)", "Izaberi", "Nazad");
        }
        case DIALOG_WARDROBE_ITEM:
        {
            if (!response)
            {
                Wardrobe_Show(playerid);
                return true;
            }
            Wardrobe_Take(playerid, listitem == 0);
        }
        case DIALOG_WARDROBE_PUT:
        {
            if (!response || listitem < 0 || listitem >= g_WardrobeRows[playerid])
            {
                Wardrobe_Show(playerid);
                return true;
            }
            Wardrobe_Put(playerid, g_InvListed[playerid][listitem], g_WardrobePutSkin[playerid][listitem]);
        }
        default:
        {
            return false;
        }
    }
    return true;
}
