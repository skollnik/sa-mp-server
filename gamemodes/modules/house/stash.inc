// Things a player lost their home with (a house taken, a room ended, a house deleted) wait here, and are taken from any home's wardrobe.
static g_StashIds[MAX_PLAYERS][STASH_LIST_SIZE];
static E_ITEM:g_StashItems[MAX_PLAYERS][STASH_LIST_SIZE];
static g_StashAmounts[MAX_PLAYERS][STASH_LIST_SIZE];

stock Stash_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `stash` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`account_id` INT UNSIGNED NOT NULL," \
        "`item` SMALLINT UNSIGNED NOT NULL," \
        "`amount` INT NOT NULL," \
        "`house_id` INT UNSIGNED NOT NULL DEFAULT 0," \
        "`source` VARCHAR(8) NOT NULL," \
        "`created_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP," \
        "INDEX `stash_account` (`account_id`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
    mysql_tquery(g_SQL, "SELECT COUNT(*) AS `found` FROM information_schema.tables WHERE table_schema = DATABASE() AND table_name = 'wardrobe'",
        "OnOldWardrobeChecked");
}

// The first wardrobe belonged to the player, not to a house; what was in it waits in the stash, to be taken in any home.
forward OnOldWardrobeChecked();
public OnOldWardrobeChecked()
{
    new found;
    cache_get_value_name_int(0, "found", found);
    if (found == 0)
    {
        return 1;
    }
    new query[192];
    mysql_format(g_SQL, query, sizeof(query), "INSERT INTO `stash` (`account_id`, `item`, `amount`, `source`) SELECT `account_id`, %d, `skin`, 'ormar' FROM `wardrobe`",
        _:ITEM_CLOTHES);
    mysql_tquery(g_SQL, query);
    mysql_tquery(g_SQL, "DROP TABLE `wardrobe`");
    print("[HOUSES] The old wardrobes were moved to the stash.");
    return 1;
}

stock Stash_AddSafe(slot, account_dbid)
{
    new query[256];
    mysql_format(g_SQL, query, sizeof(query), "INSERT INTO `stash` (`account_id`, `item`, `amount`, `house_id`, `source`) " \
        "SELECT %d, `item`, `amount`, `house_id`, 'sef' FROM `house_safe` WHERE `house_id` = %d", account_dbid, g_Houses[slot][hDbId]);
    mysql_tquery(g_SQL, query);
    new playerid = Account_FindOnlineByDbId(account_dbid);
    if (playerid != INVALID_PLAYER_ID)
    {
        Msg_Info(playerid, "Predmeti iz sefa su u magacinu; uzimaju se iz ormara u bilo kojoj kuci ili sobi gde zivite (/ormar).");
    }
}

stock Stash_Show(playerid)
{
    new query[160];
    mysql_format(g_SQL, query, sizeof(query), "SELECT `id`, `item`, `amount` FROM `stash` WHERE `account_id` = %d ORDER BY `id` LIMIT %d",
        g_Player[playerid][pDbId], STASH_LIST_SIZE);
    mysql_tquery(g_SQL, query, "OnStashListed", "dd", playerid, g_AccountRace[playerid]);
}

forward OnStashListed(playerid, race);
public OnStashListed(playerid, race)
{
    if (race != g_AccountRace[playerid] || House_LivingHere(playerid) == -1)
    {
        return 1;
    }
    new rows = min(cache_num_rows(), STASH_LIST_SIZE), item, body[1536], line[64];
    for (new row = 0; row < STASH_LIST_SIZE; row++)
    {
        g_StashIds[playerid][row] = 0;
    }
    if (rows == 0)
    {
        Msg_Info(playerid, "Magacin je prazan.");
        Wardrobe_Show(playerid);
        return 1;
    }
    strcopy(body, "Predmet\tKolicina\n");
    for (new row = 0; row < rows; row++)
    {
        cache_get_value_name_int(row, "id", g_StashIds[playerid][row]);
        cache_get_value_name_int(row, "item", item);
        cache_get_value_name_int(row, "amount", g_StashAmounts[playerid][row]);
        // A row with an item that no longer exists is listed as nothing and cannot be taken.
        g_StashItems[playerid][row] = ITEM_NONE;
        if (item > 0 && item < _:E_ITEM)
        {
            g_StashItems[playerid][row] = E_ITEM:item;
        }
        format(line, sizeof(line), "%s\t%d\n", g_ItemInfo[g_StashItems[playerid][row]][itName], Item_ShownAmount(g_StashItems[playerid][row], g_StashAmounts[playerid][row]));
        strcat(body, line);
    }
    ShowPlayerDialog(playerid, DIALOG_STASH, DIALOG_STYLE_TABLIST_HEADERS, "Magacin: klik uzima u inventar", body, "Uzmi", "Nazad");
    return 1;
}

// The row is deleted first and the item given only when the delete went through, so the same row is never taken twice.
static stock Stash_Take(playerid, row)
{
    new id = g_StashIds[playerid][row], E_ITEM:item = g_StashItems[playerid][row], amount = g_StashAmounts[playerid][row];
    if (id == 0 || item == ITEM_NONE || House_LivingHere(playerid) == -1)
    {
        return;
    }
    if (!Inv_CanAdd(playerid, item, amount))
    {
        Msg_Error(playerid, "Nemate mesta u inventaru.");
        Stash_Show(playerid);
        return;
    }
    g_StashIds[playerid][row] = 0;
    new query[128];
    mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `stash` WHERE `id` = %d AND `account_id` = %d", id, g_Player[playerid][pDbId]);
    mysql_tquery(g_SQL, query, "OnStashTaken", "ddddd", playerid, g_AccountRace[playerid], _:item, amount, g_Player[playerid][pDbId]);
}

forward OnStashTaken(playerid, race, item, amount, account_dbid);
public OnStashTaken(playerid, race, item, amount, account_dbid)
{
    if (cache_affected_rows() != 1)
    {
        return 1;
    }
    // Gone or full by now: the item goes back, never lost.
    if (race != g_AccountRace[playerid] || !Inv_CanAdd(playerid, E_ITEM:item, amount))
    {
        new query[160];
        mysql_format(g_SQL, query, sizeof(query), "INSERT INTO `stash` (`account_id`, `item`, `amount`, `source`) VALUES (%d, %d, %d, 'vraceno')", account_dbid, item, amount);
        mysql_tquery(g_SQL, query);
        return 1;
    }
    Inv_Add(playerid, E_ITEM:item, amount);
    Msg_Success(playerid, "Uzeli ste %s iz magacina.", g_ItemInfo[E_ITEM:item][itName]);
    Stash_Show(playerid);
    return 1;
}

stock bool:Stash_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
    #pragma unused inputtext
    if (dialogid != DIALOG_STASH)
    {
        return false;
    }
    if (!response)
    {
        Wardrobe_Show(playerid);
    }
    else if (listitem >= 0 && listitem < STASH_LIST_SIZE)
    {
        Stash_Take(playerid, listitem);
    }
    return true;
}
