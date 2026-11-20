stock Safe_Init()
{
    mysql_tquery(g_SQL, "ALTER TABLE `houses` ADD COLUMN IF NOT EXISTS `has_safe` TINYINT(1) NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `houses` ADD COLUMN IF NOT EXISTS `safe_x` FLOAT NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `houses` ADD COLUMN IF NOT EXISTS `safe_y` FLOAT NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `houses` ADD COLUMN IF NOT EXISTS `safe_z` FLOAT NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `houses` ADD COLUMN IF NOT EXISTS `safe_money` INT NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `houses` ADD COLUMN IF NOT EXISTS `safe_rx` FLOAT NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `houses` ADD COLUMN IF NOT EXISTS `safe_ry` FLOAT NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `houses` ADD COLUMN IF NOT EXISTS `safe_rz` FLOAT NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "SELECT `value` FROM `server_vars` WHERE `name` = 'safe_object_v1'", "OnSafeObjectsChecked");
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `house_safe` (" \
        "`house_id` INT UNSIGNED NOT NULL," \
        "`slot` TINYINT UNSIGNED NOT NULL," \
        "`item` SMALLINT UNSIGNED NOT NULL," \
        "`amount` INT NOT NULL," \
        "PRIMARY KEY (`house_id`, `slot`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
    // Money and items in a safe leave the economy's sight, so every move is kept.
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `house_safe_log` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`house_id` INT UNSIGNED NOT NULL," \
        "`account_id` INT UNSIGNED NOT NULL," \
        "`action` VARCHAR(12) NOT NULL," \
        "`item` SMALLINT UNSIGNED NOT NULL," \
        "`amount` INT NOT NULL," \
        "`created_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP," \
        "INDEX `house_safe_log_house` (`house_id`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
}

// Safes bought before they became objects kept the player's position, a metre above the floor where the object now stands.
// This runs before the houses are read, since its query was sent first.
forward OnSafeObjectsChecked();
public OnSafeObjectsChecked()
{
    if (cache_num_rows() == 0)
    {
        mysql_tquery(g_SQL, "UPDATE `houses` SET `safe_z` = `safe_z` - 1.0 WHERE `has_safe` = 1");
        mysql_tquery(g_SQL, "INSERT INTO `server_vars` (`name`, `value`) VALUES ('safe_object_v1', 1)");
    }
    return 1;
}

stock Safe_OnHouseLoaded(slot)
{
    new has_safe;
    cache_get_value_name_int(slot, "has_safe", has_safe);
    g_Houses[slot][hHasSafe] = has_safe != 0;
    cache_get_value_name_float(slot, "safe_x", g_Houses[slot][hSafeX]);
    cache_get_value_name_float(slot, "safe_y", g_Houses[slot][hSafeY]);
    cache_get_value_name_float(slot, "safe_z", g_Houses[slot][hSafeZ]);
    cache_get_value_name_float(slot, "safe_rx", g_Houses[slot][hSafeRX]);
    cache_get_value_name_float(slot, "safe_ry", g_Houses[slot][hSafeRY]);
    cache_get_value_name_float(slot, "safe_rz", g_Houses[slot][hSafeRZ]);
    cache_get_value_name_int(slot, "safe_money", g_Houses[slot][hSafeMoney]);
    // Older safes were put a metre under the player's middle, which sinks this model; they are stood back on the floor.
    if (g_Houses[slot][hHasSafe])
    {
        Place_FloorZ(HOUSE_SAFE_MODEL, g_Houses[slot][hSafeX], g_Houses[slot][hSafeY], g_Houses[slot][hSafeZ] + 1.0, g_Houses[slot][hSafeZ]);
        g_Houses[slot][hSafeRX] = 0.0;
        g_Houses[slot][hSafeRY] = 0.0;
    }
}

// All safes at once after the houses: there are few rows, and a safe must never be opened before its items are in.
stock Safe_LoadAll()
{
    mysql_tquery(g_SQL, "SELECT `house_id`, `slot`, `item`, `amount` FROM `house_safe`", "OnHouseSafesLoaded");
}

forward OnHouseSafesLoaded();
public OnHouseSafesLoaded()
{
    new house_id, index, item;
    for (new row = 0; row < cache_num_rows(); row++)
    {
        cache_get_value_name_int(row, "house_id", house_id);
        cache_get_value_name_int(row, "slot", index);
        cache_get_value_name_int(row, "item", item);
        new slot = House_FindSlotByDbId(house_id);
        if (slot == -1 || index < 0 || index >= SAFE_MAX_SLOTS || item <= 0 || item >= _:E_ITEM)
        {
            continue;
        }
        g_HouseSafe[slot][index][isItem] = E_ITEM:item;
        cache_get_value_name_int(row, "amount", g_HouseSafe[slot][index][isAmount]);
    }
    g_HouseSafesLoaded = true;
    return 1;
}

stock Safe_Capacity(slot)
{
    static const capacity[E_HOUSE_SIZE] = { 10, 20, 30 };
    return capacity[House_Size(slot)];
}

stock Safe_SaveSlot(slot, index)
{
    new query[160];
    if (g_HouseSafe[slot][index][isItem] == ITEM_NONE)
    {
        mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `house_safe` WHERE `house_id` = %d AND `slot` = %d", g_Houses[slot][hDbId], index);
    }
    else
    {
        mysql_format(g_SQL, query, sizeof(query), "REPLACE INTO `house_safe` (`house_id`, `slot`, `item`, `amount`) VALUES (%d, %d, %d, %d)",
            g_Houses[slot][hDbId], index, _:g_HouseSafe[slot][index][isItem], g_HouseSafe[slot][index][isAmount]);
    }
    mysql_tquery(g_SQL, query);
}

stock Safe_Log(playerid, slot, const action[], E_ITEM:item, amount)
{
    new query[192];
    mysql_format(g_SQL, query, sizeof(query), "INSERT INTO `house_safe_log` (`house_id`, `account_id`, `action`, `item`, `amount`) VALUES (%d, %d, '%e', %d, %d)",
        g_Houses[slot][hDbId], playerid == INVALID_PLAYER_ID ? 0 : g_Player[playerid][pDbId], action, _:item, amount);
    mysql_tquery(g_SQL, query);
}

stock Safe_Reset(slot)
{
    static const empty[E_INV_SLOT];
    for (new i = 0; i < SAFE_MAX_SLOTS; i++)
    {
        g_HouseSafe[slot][i] = empty;
    }
}

stock bool:Safe_IsEmpty(slot)
{
    return g_Houses[slot][hSafeMoney] == 0 && Store_Count(STORE_SAFE, slot) == 0;
}

// Empties the safe without paying anyone; the caller saves the house.
stock Safe_Clear(slot)
{
    Safe_Reset(slot);
    g_Houses[slot][hSafeMoney] = 0;
    new query[80];
    mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `house_safe` WHERE `house_id` = %d", g_Houses[slot][hDbId]);
    mysql_tquery(g_SQL, query);
}

// When the house is taken (inactivity, an admin): the money goes to the owner's bank and the items to their stash.
stock Safe_ReturnToOwner(slot)
{
    new owner = g_Houses[slot][hOwnerDbId], money = g_Houses[slot][hSafeMoney];
    if (owner == 0 || Safe_IsEmpty(slot))
    {
        return;
    }
    if (money > 0)
    {
        new reason[48];
        format(reason, sizeof(reason), "novac iz sefa kuce %d", g_Houses[slot][hDbId]);
        new playerid = Money_PayAccount(owner, money, reason);
        if (playerid != INVALID_PLAYER_ID)
        {
            Msg_Info(playerid, "Novac iz sefa kuce ($%d) je prebacen u banku (/banka).", money);
        }
    }
    if (Store_Count(STORE_SAFE, slot) > 0)
    {
        Stash_AddSafe(slot, owner);
    }
    Safe_Log(INVALID_PLAYER_ID, slot, "returned", ITEM_NONE, money);
    Safe_Clear(slot);
}

// The house whose safe the player stands at, or -1.
stock Safe_FindNear(playerid)
{
    new slot = House_FindInsideAnywhere(playerid);
    if (slot == -1 || !g_Houses[slot][hHasSafe]
        || !IsPlayerInRangeOfPoint(playerid, HOUSE_SAFE_RADIUS, g_Houses[slot][hSafeX], g_Houses[slot][hSafeY], g_Houses[slot][hSafeZ]))
    {
        return -1;
    }
    return slot;
}

stock bool:Safe_CanUse(playerid, slot, bool:tell)
{
    new message[80];
    if (Safe_FindNear(playerid) != slot)
    {
        strcopy(message, "Stanite pored sefa u kuci.");
    }
    else if (!House_IsOwner(playerid, slot))
    {
        strcopy(message, "Sef otvara samo vlasnik kuce.");
    }
    else if (!g_HouseSafesLoaded)
    {
        strcopy(message, "Sef se jos ucitava, pokusajte ponovo.");
    }
    else
    {
        return true;
    }
    if (tell)
    {
        Msg_Error(playerid, "%s", message);
    }
    return false;
}

// The money row above the items in the safe menu; returns how many rows it added.
stock Safe_ListMoney(slot, body[], size)
{
    new line[48];
    format(line, sizeof(line), "Novac\t$%d\n", g_Houses[slot][hSafeMoney]);
    strcat(body, line, size);
    return 1;
}

stock Safe_OnMoneyRow(playerid, slot)
{
    new body[96];
    format(body, sizeof(body), "Ostavi novac u sef\nUzmi novac iz sefa ($%d)", g_Houses[slot][hSafeMoney]);
    ShowPlayerDialog(playerid, DIALOG_SAFE_MONEY, DIALOG_STYLE_LIST, "Sef: novac", body, "Izaberi", "Nazad");
}

static stock Safe_AskMoney(playerid, bool:deposit, const error[] = "")
{
    new text[64];
    format(text, sizeof(text), "U sefu: $%d. Koliko %s?", g_Houses[g_HouseSelected[playerid]][hSafeMoney], deposit ? ("ostavljate") : ("uzimate"));
    Money_ShowAmountInput(playerid, deposit ? DIALOG_SAFE_DEPOSIT : DIALOG_SAFE_WITHDRAW, "Sef: novac", text, error);
}

static stock Safe_MoveMoney(playerid, slot, amount, bool:deposit)
{
    if (amount <= 0)
    {
        Safe_AskMoney(playerid, deposit, "Unesite iznos veci od nule.");
        return;
    }
    if (deposit && amount > g_Player[playerid][pCash])
    {
        Safe_AskMoney(playerid, deposit, "Toliko nemate u dzepu.");
        return;
    }
    if (!deposit && amount > g_Houses[slot][hSafeMoney])
    {
        Safe_AskMoney(playerid, deposit, "Toliko nema u sefu.");
        return;
    }
    new reason[32];
    format(reason, sizeof(reason), "sef kuce %d", g_Houses[slot][hDbId]);
    // Taken from one side before the other gets it, and both saved, so a crash in between cannot copy money.
    if (deposit)
    {
        Money_GiveCash(playerid, -amount, reason);
        g_Houses[slot][hSafeMoney] += amount;
    }
    else
    {
        g_Houses[slot][hSafeMoney] -= amount;
        Money_GiveCash(playerid, amount, reason);
    }
    House_Save(slot);
    Account_Save(playerid);
    Safe_Log(playerid, slot, deposit ? ("deposit") : ("withdraw"), ITEM_NONE, amount);
    Chat_Action(playerid, deposit ? ("ostavlja novac u sef.") : ("uzima novac iz sefa."));
    Store_Open(playerid, STORE_SAFE, slot);
}

// Placed like an ATM: it stands on the floor a step in front of the owner, facing them, and moves by standing elsewhere.
static stock Safe_ShowMove(playerid, slot)
{
    g_HouseSelected[playerid] = slot;
    ShowPlayerDialog(playerid, DIALOG_SAFE_MOVE, DIALOG_STYLE_LIST, "Sef",
        "Pomeri ovde (ispred sebe)\nOkreni (90 stepeni)", "Izaberi", "Zatvori");
}

static stock bool:Safe_FitsAt(playerid, slot, &Float:x, &Float:y, &Float:z, &Float:rz)
{
    if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT)
    {
        Msg_Error(playerid, "Sef se postavlja peske.");
        return false;
    }
    if (!Place_IsInsideHouse(slot, x, y, z))
    {
        Msg_Error(playerid, "Sef mora biti u kuci.");
        return false;
    }
    if (Place_BlocksExit(slot, x, y))
    {
        Msg_Error(playerid, "Sef ne sme stajati na izlazu (najmanje %.1f m od njega).", FURN_EXIT_CLEAR);
        return false;
    }
    new Float:rx, Float:ry;
    return Place_Settle(playerid, slot, HOUSE_SAFE_MODEL, false, -1, x, y, z, rx, ry, rz);
}

static stock Safe_PutAt(slot, Float:x, Float:y, Float:z, Float:rz)
{
    g_Houses[slot][hSafeX] = x;
    g_Houses[slot][hSafeY] = y;
    g_Houses[slot][hSafeZ] = z;
    g_Houses[slot][hSafeRX] = 0.0;
    g_Houses[slot][hSafeRY] = 0.0;
    g_Houses[slot][hSafeRZ] = rz;
    House_Refresh(slot);
    House_Save(slot);
}

static stock Safe_Move(playerid, slot, bool:turn)
{
    new Float:x, Float:y, Float:z, Float:rz;
    if (turn)
    {
        x = g_Houses[slot][hSafeX];
        y = g_Houses[slot][hSafeY];
        z = g_Houses[slot][hSafeZ];
        rz = g_Houses[slot][hSafeRZ] + SAFE_TURN;
    }
    else
    {
        Place_InFront(playerid, HOUSE_SAFE_MODEL, x, y, z, rz);
    }
    if (!Safe_FitsAt(playerid, slot, x, y, z, rz))
    {
        return;
    }
    Safe_PutAt(slot, x, y, z, rz);
    Safe_Log(playerid, slot, "moved", ITEM_NONE, 0);
    Safe_ShowMove(playerid, slot);
}

static stock Safe_Buy(playerid, slot)
{
    if (!House_IsOwner(playerid, slot) || House_FindInsideAnywhere(playerid) != slot || g_Houses[slot][hHasSafe])
    {
        Msg_Error(playerid, "Sef se kupuje u kuci; udjite unutra.");
        return;
    }
    new Float:x, Float:y, Float:z, Float:rz;
    Place_InFront(playerid, HOUSE_SAFE_MODEL, x, y, z, rz);
    if (!Safe_FitsAt(playerid, slot, x, y, z, rz))
    {
        return;
    }
    new reason[32];
    format(reason, sizeof(reason), "sef za kucu %d", g_Houses[slot][hDbId]);
    if (!Money_Take(playerid, HOUSE_SAFE_PRICE, reason))
    {
        return;
    }
    Account_Save(playerid);
    g_Houses[slot][hHasSafe] = true;
    Safe_PutAt(slot, x, y, z, rz);
    Safe_Log(playerid, slot, "bought", ITEM_NONE, HOUSE_SAFE_PRICE);
    Msg_Success(playerid, "Kupili ste sef za $%d; otvara se sa /sef ili F pored njega. Pomera se u /kuca.", HOUSE_SAFE_PRICE);
}

// After an admin gives the house another interior the old spot means nothing there, so the safe goes a few steps in from the exit.
stock Safe_PutAtExit(slot)
{
    new Float:angle, Float:x, Float:y, Float:z;
    House_ExitPos(slot, x, y, z, angle);
    x += (FURN_EXIT_CLEAR + 0.5) * floatsin(-angle, degrees);
    y += (FURN_EXIT_CLEAR + 0.5) * floatcos(-angle, degrees);
    g_Houses[slot][hSafeX] = x;
    g_Houses[slot][hSafeY] = y;
    if (!Place_FloorZ(HOUSE_SAFE_MODEL, x, y, z, g_Houses[slot][hSafeZ]))
    {
        g_Houses[slot][hSafeZ] = z - 1.0;
    }
    g_Houses[slot][hSafeRX] = 0.0;
    g_Houses[slot][hSafeRY] = 0.0;
    g_Houses[slot][hSafeRZ] = angle;
}

// The "safe" row of the owner's house menu.
stock Safe_OnMenu(playerid, slot)
{
    if (House_FindInsideAnywhere(playerid) != slot)
    {
        Msg_Error(playerid, "Udjite u kucu; sef se postavlja unutra.");
        return;
    }
    if (g_Houses[slot][hHasSafe])
    {
        Safe_ShowMove(playerid, slot);
        return;
    }
    g_HouseSelected[playerid] = slot;
    new text[192];
    format(text, sizeof(text), "Kupujete sef za $%d. Stoji na podu ispred vas; pomera se tako sto stanete gde ga zelite (/kuca). Otvarate ga samo vi (/sef ili F).\n\nU sef staje novac i %d predmeta.",
        HOUSE_SAFE_PRICE, Safe_Capacity(slot));
    ShowPlayerDialog(playerid, DIALOG_SAFE_BUY, DIALOG_STYLE_MSGBOX, "Sef", text, "Kupi", "Odustani");
}

stock Safe_ShowToAdmin(playerid, slot)
{
    if (!g_Houses[slot][hHasSafe])
    {
        Msg_Error(playerid, "Kuca nema sef.");
        return;
    }
    new body[1024], title[64], owner[MAX_PLAYER_NAME + 1] = "nema";
    strcopy(body, "Predmet\tKolicina\n");
    Safe_ListMoney(slot, body, sizeof(body));
    Store_ListItems(STORE_SAFE, slot, body, sizeof(body));
    if (g_Houses[slot][hOwnerDbId] != 0)
    {
        strcopy(owner, g_Houses[slot][hOwnerName]);
    }
    format(title, sizeof(title), "Sef kuce %d (vlasnik %s)", g_Houses[slot][hDbId], owner);
    ShowPlayerDialog(playerid, DIALOG_NONE, DIALOG_STYLE_TABLIST_HEADERS, title, body, "Zatvori", "");
}

cmd:sef(playerid, params[])
{
    new slot = Safe_FindNear(playerid);
    if (slot == -1)
    {
        Msg_Error(playerid, "Stanite pored sefa u kuci. Sef se kupuje u /kuca.");
        return 1;
    }
    Store_Open(playerid, STORE_SAFE, slot);
    return 1;
}
alias:sef("safe")

stock bool:Safe_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
    switch (dialogid)
    {
        case DIALOG_SAFE_BUY:
        {
            if (response)
            {
                Safe_Buy(playerid, g_HouseSelected[playerid]);
            }
        }
        case DIALOG_SAFE_MOVE:
        {
            new slot = g_HouseSelected[playerid];
            if (slot == -1 || !g_Houses[slot][hHasSafe] || !House_IsOwner(playerid, slot) || House_FindInsideAnywhere(playerid) != slot)
            {
                return true;
            }
            if (response)
            {
                Safe_Move(playerid, slot, listitem == 1);
            }
        }
        case DIALOG_SAFE_MONEY, DIALOG_SAFE_DEPOSIT, DIALOG_SAFE_WITHDRAW:
        {
            new slot = Safe_FindNear(playerid);
            if (slot == -1 || !Safe_CanUse(playerid, slot, true))
            {
                return true;
            }
            g_HouseSelected[playerid] = slot;
            if (!response)
            {
                if (dialogid == DIALOG_SAFE_MONEY)
                {
                    Store_Open(playerid, STORE_SAFE, slot);
                }
                else
                {
                    Safe_OnMoneyRow(playerid, slot);
                }
                return true;
            }
            if (dialogid == DIALOG_SAFE_MONEY)
            {
                Safe_AskMoney(playerid, listitem == 0);
                return true;
            }
            Safe_MoveMoney(playerid, slot, Money_ParseAmount(inputtext), dialogid == DIALOG_SAFE_DEPOSIT);
        }
        default:
        {
            return false;
        }
    }
    return true;
}
