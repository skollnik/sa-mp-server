static g_WardrobeListed[MAX_PLAYERS][WARDROBE_MAX_SLOTS];
static g_WardrobeRows[MAX_PLAYERS];
// The wardrobe place and skin picked; the skin is compared again, since the menu may be answered late.
static g_WardrobePick[MAX_PLAYERS];
static g_WardrobePickSkin[MAX_PLAYERS];
static g_WardrobePutSkin[MAX_PLAYERS][INV_MAX_SLOTS];
// Stash rows counted when the menu opened; the stash row is shown only when there is something in it.
static g_WardrobeStash[MAX_PLAYERS];

// Every resident has their own wardrobe in each house they live in, so two houses or a room never share one.
stock Wardrobe_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `house_wardrobe` (" \
        "`house_id` INT UNSIGNED NOT NULL," \
        "`account_id` INT UNSIGNED NOT NULL," \
        "`slot` TINYINT UNSIGNED NOT NULL," \
        "`skin` SMALLINT UNSIGNED NOT NULL," \
        "PRIMARY KEY (`house_id`, `account_id`, `slot`)," \
        "INDEX `house_wardrobe_account` (`account_id`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
}

stock Wardrobe_Reset(playerid)
{
    for (new i = 0; i < WARDROBE_MAX_SLOTS; i++)
    {
        g_Wardrobe[playerid][i] = 0;
    }
    g_WardrobeHouse[playerid] = 0;
}

// Read once after the houses, so a sale can check the owner's wardrobe without asking the database.
stock Wardrobe_CountOwners()
{
    mysql_tquery(g_SQL, "SELECT w.`house_id`, COUNT(*) AS `outfits` FROM `house_wardrobe` w " \
        "JOIN `houses` h ON h.`id` = w.`house_id` AND h.`owner_id` = w.`account_id` GROUP BY w.`house_id`", "OnOwnerWardrobesCounted");
}

forward OnOwnerWardrobesCounted();
public OnOwnerWardrobesCounted()
{
    new dbid, outfits;
    for (new row = 0; row < cache_num_rows(); row++)
    {
        cache_get_value_name_int(row, "house_id", dbid);
        cache_get_value_name_int(row, "outfits", outfits);
        new slot = House_FindSlotByDbId(dbid);
        if (slot != -1)
        {
            g_Houses[slot][hOwnerWardrobe] = outfits;
        }
    }
    return 1;
}

// Rows of someone who no longer lives in that house (sold, taken, evicted, moved out) or of a deleted house.
#define WARDROBE_ORPHANS "FROM `house_wardrobe` w LEFT JOIN `houses` h ON h.`id` = w.`house_id` LEFT JOIN `accounts` a ON a.`id` = w.`account_id` " \
    "WHERE h.`id` IS NULL OR (h.`owner_id` <> w.`account_id` AND IFNULL(a.`room_house`, 0) <> w.`house_id`)"

// Called after anyone stops living somewhere; their outfits wait in the stash instead of being lost or left in someone else's home.
stock Wardrobe_StashOrphans()
{
    new query[512];
    mysql_format(g_SQL, query, sizeof(query), "INSERT INTO `stash` (`account_id`, `item`, `amount`, `house_id`, `source`) " \
        "SELECT w.`account_id`, %d, w.`skin`, w.`house_id`, 'ormar' " WARDROBE_ORPHANS, _:ITEM_CLOTHES);
    mysql_tquery(g_SQL, query);
    mysql_tquery(g_SQL, "DELETE w " WARDROBE_ORPHANS);
    // A wardrobe still loaded could otherwise hand out outfits that were just moved to the stash.
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        g_WardrobeHouse[i] = 0;
    }
}

static stock Wardrobe_Load(playerid, slot)
{
    new query[128];
    mysql_format(g_SQL, query, sizeof(query), "SELECT `slot`, `skin` FROM `house_wardrobe` WHERE `house_id` = %d AND `account_id` = %d",
        g_Houses[slot][hDbId], g_Player[playerid][pDbId]);
    mysql_tquery(g_SQL, query, "OnWardrobeLoaded", "ddd", playerid, g_AccountRace[playerid], g_Houses[slot][hDbId]);
}

forward OnWardrobeLoaded(playerid, race, house_dbid);
public OnWardrobeLoaded(playerid, race, house_dbid)
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }
    Wardrobe_Reset(playerid);
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
    g_WardrobeHouse[playerid] = house_dbid;
    Wardrobe_Show(playerid);
    return 1;
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

static stock Wardrobe_Set(playerid, slot, index, skin)
{
    new query[160], house_dbid = g_Houses[slot][hDbId];
    g_Wardrobe[playerid][index] = skin;
    if (skin == 0)
    {
        mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `house_wardrobe` WHERE `house_id` = %d AND `account_id` = %d AND `slot` = %d",
            house_dbid, g_Player[playerid][pDbId], index);
    }
    else
    {
        mysql_format(g_SQL, query, sizeof(query), "REPLACE INTO `house_wardrobe` (`house_id`, `account_id`, `slot`, `skin`) VALUES (%d, %d, %d, %d)",
            house_dbid, g_Player[playerid][pDbId], index, skin);
    }
    mysql_tquery(g_SQL, query);
    if (House_IsOwner(playerid, slot))
    {
        g_Houses[slot][hOwnerWardrobe] = Wardrobe_Count(playerid);
    }
}

// How much fits depends on the house it stands in.
static stock Wardrobe_Capacity(slot)
{
    static const capacity[E_HOUSE_SIZE] = { 3, 5, 8 };
    return capacity[House_Size(slot)];
}

// The house whose wardrobe the player can use right now, or -1 after saying why not (or after starting to load it).
static stock Wardrobe_HouseHere(playerid)
{
    new slot = House_LivingHere(playerid);
    if (slot == -1)
    {
        return -1;
    }
    if (Inv_IsBusy(playerid))
    {
        Msg_Error(playerid, "Ormar se koristi peske.");
        return -1;
    }
    if (g_WardrobeHouse[playerid] != g_Houses[slot][hDbId] || !g_InvLoaded[playerid])
    {
        Wardrobe_Load(playerid, slot);
        return -1;
    }
    return slot;
}

stock Wardrobe_Show(playerid)
{
    if (Wardrobe_HouseHere(playerid) == -1)
    {
        return;
    }
    new query[96];
    mysql_format(g_SQL, query, sizeof(query), "SELECT COUNT(*) AS `rows` FROM `stash` WHERE `account_id` = %d", g_Player[playerid][pDbId]);
    mysql_tquery(g_SQL, query, "OnWardrobeStashCounted", "dd", playerid, g_AccountRace[playerid]);
}

forward OnWardrobeStashCounted(playerid, race);
public OnWardrobeStashCounted(playerid, race)
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }
    new slot = Wardrobe_HouseHere(playerid);
    if (slot == -1)
    {
        return 1;
    }
    g_WardrobeStash[playerid] = 0;
    cache_get_value_name_int(0, "rows", g_WardrobeStash[playerid]);
    new body[192], line[64], title[48];
    format(body, sizeof(body), "Uzmi ili obuci garderobu (%d u ormaru)\nOstavi garderobu iz inventara", Wardrobe_Count(playerid));
    if (g_WardrobeStash[playerid] > 0)
    {
        format(line, sizeof(line), "\nStare stvari iz magacina (%d)", g_WardrobeStash[playerid]);
        strcat(body, line);
    }
    format(title, sizeof(title), "Ormar (%d/%d mesta)", Wardrobe_Count(playerid), Wardrobe_Capacity(slot));
    ShowPlayerDialog(playerid, DIALOG_WARDROBE, DIALOG_STYLE_LIST, title, body, "Izaberi", "Zatvori");
    return 1;
}

// The outfits are picked by their pictures, so no list has to name them.
static stock Wardrobe_ShowTake(playerid)
{
    new skins[WARDROBE_MAX_SLOTS], count;
    for (new i = 0; i < WARDROBE_MAX_SLOTS; i++)
    {
        if (g_Wardrobe[playerid][i] != 0)
        {
            skins[count] = g_Wardrobe[playerid][i];
            g_WardrobeListed[playerid][count++] = i;
        }
    }
    if (count == 0)
    {
        Msg_Error(playerid, "Ormar je prazan.");
        Wardrobe_Show(playerid);
        return;
    }
    Preview_Show(playerid, PREVIEW_WARDROBE_TAKE, PREVIEW_KIND_SKIN, "Ormar", skins, count);
}

static stock Wardrobe_ShowPut(playerid)
{
    new skins[INV_MAX_SLOTS], count;
    for (new i = 0; i < INV_MAX_SLOTS; i++)
    {
        if (g_Inv[playerid][i][isItem] == ITEM_CLOTHES)
        {
            skins[count] = g_Inv[playerid][i][isAmount];
            g_WardrobePutSkin[playerid][count] = skins[count];
            g_InvListed[playerid][count++] = i;
        }
    }
    if (count == 0)
    {
        Msg_Error(playerid, "Nemate garderobu u inventaru. Kupuje se u prodavnici odece (/kupi).");
        Wardrobe_Show(playerid);
        return;
    }
    g_WardrobeRows[playerid] = count;
    Preview_Show(playerid, PREVIEW_WARDROBE_PUT, PREVIEW_KIND_SKIN, "Ostavi u ormar", skins, count);
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
        Msg_Error(playerid, "Te garderobe vise nema u inventaru.");
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
            Wardrobe_Set(playerid, slot, i, skin);
            Chat_Action(playerid, "ostavlja garderobu u ormar.");
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
            Msg_Error(playerid, "Ta garderoba nije za vas pol.");
            return;
        }
        if (!Clothes_CanChange(playerid))
        {
            return;
        }
        // The clothes taken off go to the same place, so the wardrobe never needs room for them.
        Wardrobe_Set(playerid, slot, index, Clothes_Wear(playerid, skin));
        Msg_Success(playerid, "Obukli ste garderobu iz ormara.");
    }
    else
    {
        if (!Inv_CanAdd(playerid, ITEM_CLOTHES, skin))
        {
            Msg_Error(playerid, "Nemate mesta u inventaru za kesu sa garderobom.");
            return;
        }
        Wardrobe_Set(playerid, slot, index, 0);
        Inv_Add(playerid, ITEM_CLOTHES, skin);
        Chat_Action(playerid, "pakuje garderobu iz ormara u kesu.");
    }
    Wardrobe_Show(playerid);
}

stock Wardrobe_OnTakeSelected(playerid, index)
{
    new count = Wardrobe_Count(playerid);
    if (index < 0 || index >= count)
    {
        return;
    }
    g_WardrobePick[playerid] = g_WardrobeListed[playerid][index];
    g_WardrobePickSkin[playerid] = g_Wardrobe[playerid][g_WardrobePick[playerid]];
    ShowPlayerDialog(playerid, DIALOG_WARDROBE_ITEM, DIALOG_STYLE_LIST, "Garderoba", "Obuci (sadasnja garderoba ide na njeno mesto)\nSpakuj u kesu (u inventar)", "Izaberi", "Nazad");
}

stock Wardrobe_OnPutSelected(playerid, index)
{
    if (index >= 0 && index < g_WardrobeRows[playerid])
    {
        Wardrobe_Put(playerid, g_InvListed[playerid][index], g_WardrobePutSkin[playerid][index]);
    }
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
            if (!response || Wardrobe_HouseHere(playerid) == -1)
            {
                return true;
            }
            switch (listitem)
            {
                case 0: Wardrobe_ShowTake(playerid);
                case 1: Wardrobe_ShowPut(playerid);
                case 2: Stash_Show(playerid);
            }
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
        default:
        {
            return false;
        }
    }
    return true;
}
