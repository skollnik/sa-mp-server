#define TRUNK_MAX_SLOTS         (20)
#define TRUNK_RANGE             (2.0)

// Belongs to the vehicle, not the owner: items stay in it when it is sold or traded.
new g_Trunk[MAX_PERSONAL_VEHICLES][TRUNK_MAX_SLOTS][E_INV_SLOT];
new bool:g_TrunkLoaded[MAX_PERSONAL_VEHICLES];
// Grows on every change, so an offer for the vehicle notices that the trunk was emptied after it was sent.
new g_TrunkVersion[MAX_PERSONAL_VEHICLES];

new g_TrunkSlot[MAX_PLAYERS] = { -1, ... };
new g_TrunkDbId[MAX_PLAYERS];
new g_TrunkListed[MAX_PLAYERS][TRUNK_MAX_SLOTS];
new g_TrunkRows[MAX_PLAYERS];
// The trunk slot or inventory slot picked, while the amount is asked for.
new g_TrunkPick[MAX_PLAYERS];
new E_ITEM:g_TrunkPickItem[MAX_PLAYERS];

stock Trunk_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `vehicle_trunk` (" \
        "`vehicle_id` INT UNSIGNED NOT NULL," \
        "`slot` TINYINT UNSIGNED NOT NULL," \
        "`item` SMALLINT UNSIGNED NOT NULL," \
        "`amount` INT NOT NULL," \
        "PRIMARY KEY (`vehicle_id`, `slot`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
    // Anyone may take from an open, unlocked trunk, so who took what is kept for theft reports.
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `trunk_log` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`vehicle_id` INT UNSIGNED NOT NULL," \
        "`account_id` INT UNSIGNED NOT NULL," \
        "`action` VARCHAR(8) NOT NULL," \
        "`item` SMALLINT UNSIGNED NOT NULL," \
        "`amount` INT NOT NULL," \
        "`created_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP," \
        "INDEX `trunk_log_vehicle` (`vehicle_id`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
}

// Cars 6, vans and pickups 12, big trucks 20; two-wheelers, boats, aircraft, trailers and trains have none.
stock Trunk_Capacity(model)
{
    switch (model)
    {
        case 481, 509, 510, 448, 461, 462, 463, 468, 471, 521, 522, 523, 581, 586,
            430, 446, 452, 453, 454, 472, 473, 484, 493, 595,
            417, 425, 447, 460, 469, 476, 487, 488, 497, 511, 512, 513, 519, 520, 548, 553, 563, 577, 592, 593,
            435, 450, 584, 591, 606, 607, 608, 610, 611, 441, 464, 465, 501, 564, 594, 449, 537, 538, 569, 570, 590:
        {
            return 0;
        }
        case 403, 406, 408, 414, 433, 443, 455, 456, 498, 499, 514, 515, 524, 573, 578, 609:
        {
            return 20;
        }
        case 413, 418, 422, 423, 428, 440, 459, 478, 482, 483, 508, 543, 554, 582, 588, 600, 605, 431, 437:
        {
            return 12;
        }
    }
    return 6;
}

static stock Trunk_SaveSlot(slot, index)
{
    new query[160];
    if (g_Trunk[slot][index][isItem] == ITEM_NONE)
    {
        mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `vehicle_trunk` WHERE `vehicle_id` = %d AND `slot` = %d", g_Veh[slot][vDbId], index);
    }
    else
    {
        mysql_format(g_SQL, query, sizeof(query), "REPLACE INTO `vehicle_trunk` (`vehicle_id`, `slot`, `item`, `amount`) VALUES (%d, %d, %d, %d)",
            g_Veh[slot][vDbId], index, _:g_Trunk[slot][index][isItem], g_Trunk[slot][index][isAmount]);
    }
    mysql_tquery(g_SQL, query);
    g_TrunkVersion[slot]++;
}

stock Trunk_Reset(slot)
{
    static const empty[E_INV_SLOT];
    for (new i = 0; i < TRUNK_MAX_SLOTS; i++)
    {
        g_Trunk[slot][i] = empty;
    }
    g_TrunkLoaded[slot] = false;
    g_TrunkVersion[slot]++;
}

stock Trunk_Load(slot)
{
    Trunk_Reset(slot);
    new query[96];
    mysql_format(g_SQL, query, sizeof(query), "SELECT `slot`, `item`, `amount` FROM `vehicle_trunk` WHERE `vehicle_id` = %d", g_Veh[slot][vDbId]);
    mysql_tquery(g_SQL, query, "OnTrunkLoaded", "dd", slot, g_Veh[slot][vDbId]);
}

forward OnTrunkLoaded(slot, dbid);
public OnTrunkLoaded(slot, dbid)
{
    if (!g_Veh[slot][vUsed] || g_Veh[slot][vDbId] != dbid)
    {
        return 1;
    }
    new index, item;
    for (new row = 0; row < cache_num_rows(); row++)
    {
        cache_get_value_name_int(row, "slot", index);
        cache_get_value_name_int(row, "item", item);
        if (index < 0 || index >= TRUNK_MAX_SLOTS || item <= 0 || item >= _:E_ITEM)
        {
            continue;
        }
        g_Trunk[slot][index][isItem] = E_ITEM:item;
        cache_get_value_name_int(row, "amount", g_Trunk[slot][index][isAmount]);
    }
    g_TrunkLoaded[slot] = true;
    return 1;
}

stock Trunk_Empty(slot)
{
    Trunk_Reset(slot);
    g_TrunkLoaded[slot] = true;
    new query[80];
    mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `vehicle_trunk` WHERE `vehicle_id` = %d", g_Veh[slot][vDbId]);
    mysql_tquery(g_SQL, query);
}

stock Trunk_Count(slot)
{
    new count;
    for (new i = 0; i < TRUNK_MAX_SLOTS; i++)
    {
        if (g_Trunk[slot][i][isItem] != ITEM_NONE)
        {
            count++;
        }
    }
    return count;
}

// "prazan" or how many kinds of items are inside, for an offer or an admin.
stock Trunk_Summary(slot, dest[], size = sizeof(dest))
{
    new count = Trunk_Count(slot);
    if (Trunk_Capacity(g_Veh[slot][vModel]) == 0)
    {
        strcopy(dest, "nema gepek", size);
    }
    else if (count == 0)
    {
        strcopy(dest, "prazan", size);
    }
    else
    {
        format(dest, size, "%d vrsta predmeta (ostaju u vozilu)", count);
    }
}

static stock Trunk_Log(playerid, slot, const action[], E_ITEM:item, amount)
{
    new query[192];
    mysql_format(g_SQL, query, sizeof(query), "INSERT INTO `trunk_log` (`vehicle_id`, `account_id`, `action`, `item`, `amount`) VALUES (%d, %d, '%e', %d, %d)",
        g_Veh[slot][vDbId], g_Player[playerid][pDbId], action, _:item, amount);
    mysql_tquery(g_SQL, query);
}

static stock bool:Trunk_IsBehind(playerid, vehicleid)
{
    new Float:x, Float:y, Float:z, Float:angle, Float:size_x, Float:size_y, Float:size_z;
    GetVehiclePos(vehicleid, x, y, z);
    GetVehicleZAngle(vehicleid, angle);
    GetVehicleModelInfo(GetVehicleModel(vehicleid), VEHICLE_MODEL_INFO_SIZE, size_x, size_y, size_z);
    return IsPlayerInRangeOfPoint(playerid, TRUNK_RANGE, x + floatsin(angle, degrees) * size_y / 2.0, y - floatcos(angle, degrees) * size_y / 2.0, z);
}

// The personal vehicle whose rear the player stands at, or -1.
static stock Trunk_FindBehind(playerid)
{
    new world = GetPlayerVirtualWorld(playerid);
    for (new slot = 0; slot < MAX_PERSONAL_VEHICLES; slot++)
    {
        new vehicleid = g_Veh[slot][vVehicleId];
        if (g_Veh[slot][vUsed] && vehicleid != INVALID_VEHICLE_ID && GetVehicleVirtualWorld(vehicleid) == world
            && Trunk_Capacity(g_Veh[slot][vModel]) > 0 && Trunk_IsBehind(playerid, vehicleid))
        {
            return slot;
        }
    }
    return -1;
}

// Checked again on every dialog answer, since the owner may close or lock the trunk or drive off meanwhile.
static stock bool:Trunk_CanUse(playerid, slot, bool:tell)
{
    new message[96];
    if (slot == -1 || !g_Veh[slot][vUsed] || (g_TrunkSlot[playerid] == slot && g_Veh[slot][vDbId] != g_TrunkDbId[playerid])
        || !Trunk_IsBehind(playerid, g_Veh[slot][vVehicleId]))
    {
        strcopy(message, "Stanite iza vozila sa otvorenim gepekom (otvara ga vlasnik: /v gepek).");
    }
    else if (Inv_IsBusy(playerid))
    {
        strcopy(message, "Gepek se koristi peske.");
    }
    else if (!g_TrunkLoaded[slot])
    {
        strcopy(message, "Gepek se jos ucitava, pokusajte ponovo.");
    }
    else if (g_Veh[slot][vLocked])
    {
        strcopy(message, "Vozilo je zakljucano.");
    }
    else if (!Veh_GetParam(g_Veh[slot][vVehicleId], VEH_PARAM_BOOT))
    {
        strcopy(message, "Gepek je zatvoren. Otvara ga vlasnik ili ko ima kljuc: /v gepek.");
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

static stock Trunk_Show(playerid, slot)
{
    new body[1024], line[64], count, capacity = Trunk_Capacity(g_Veh[slot][vModel]);
    strcopy(body, "Predmet\tKolicina\n");
    for (new i = 0; i < TRUNK_MAX_SLOTS; i++)
    {
        if (g_Trunk[slot][i][isItem] == ITEM_NONE)
        {
            continue;
        }
        format(line, sizeof(line), "%s\t%d\n", g_ItemInfo[g_Trunk[slot][i][isItem]][itName], g_Trunk[slot][i][isAmount]);
        strcat(body, line);
        g_TrunkListed[playerid][count++] = i;
    }
    strcat(body, "> Stavi predmet iz inventara\t");
    g_TrunkSlot[playerid] = slot;
    g_TrunkDbId[playerid] = g_Veh[slot][vDbId];

    new title[64];
    format(title, sizeof(title), "Gepek: %s (%d/%d mesta)", Veh_ModelName(g_Veh[slot][vModel]), count, capacity);
    // The last row is the "put" row.
    g_TrunkRows[playerid] = count;
    ShowPlayerDialog(playerid, DIALOG_TRUNK, DIALOG_STYLE_TABLIST_HEADERS, title, body, "Izaberi", "Zatvori");
}

static stock Trunk_ShowPut(playerid)
{
    new body[1024], line[64], count;
    strcopy(body, "Predmet\tKolicina\n");
    for (new slot = 0; slot < INV_MAX_SLOTS; slot++)
    {
        if (g_Inv[playerid][slot][isItem] == ITEM_NONE)
        {
            continue;
        }
        format(line, sizeof(line), "%s\t%d\n", g_ItemInfo[g_Inv[playerid][slot][isItem]][itName], g_Inv[playerid][slot][isAmount]);
        strcat(body, line);
        g_InvListed[playerid][count++] = slot;
    }
    if (count == 0)
    {
        Msg_Error(playerid, "Inventar je prazan.");
        Trunk_Show(playerid, g_TrunkSlot[playerid]);
        return;
    }
    ShowPlayerDialog(playerid, DIALOG_TRUNK_PUT, DIALOG_STYLE_TABLIST_HEADERS, "Stavi u gepek", body, "Izaberi", "Nazad");
}

static stock Trunk_AskAmount(playerid, dialogid, max)
{
    new text[64];
    format(text, sizeof(text), "Kolicina (1-%d):", max);
    ShowPlayerDialog(playerid, dialogid, DIALOG_STYLE_INPUT, "Kolicina", text, "U redu", "Nazad");
}

// Everything is checked again here: another player at the same trunk may have taken the item first.
static stock Trunk_Take(playerid, index, amount)
{
    new slot = g_TrunkSlot[playerid];
    if (!Trunk_CanUse(playerid, slot, true))
    {
        return;
    }
    new E_ITEM:item = g_Trunk[slot][index][isItem];
    if (item != g_TrunkPickItem[playerid] || item == ITEM_NONE)
    {
        Msg_Error(playerid, "Tog predmeta vise nema u gepeku.");
        Trunk_Show(playerid, slot);
        return;
    }
    if (amount < 1 || amount > g_Trunk[slot][index][isAmount])
    {
        Msg_Error(playerid, "U gepeku je samo %d.", g_Trunk[slot][index][isAmount]);
        Trunk_Show(playerid, slot);
        return;
    }
    if (!Inv_CanAdd(playerid, item, amount))
    {
        Msg_Error(playerid, "Nemate mesta u inventaru.");
        Trunk_Show(playerid, slot);
        return;
    }
    g_Trunk[slot][index][isAmount] -= amount;
    if (g_Trunk[slot][index][isAmount] <= 0)
    {
        g_Trunk[slot][index][isItem] = ITEM_NONE;
        g_Trunk[slot][index][isAmount] = 0;
    }
    Trunk_SaveSlot(slot, index);
    Inv_Add(playerid, item, amount);
    Trunk_Log(playerid, slot, "take", item, amount);

    new action[64];
    format(action, sizeof(action), "vadi %s iz gepeka.", g_ItemInfo[item][itName]);
    Chat_Action(playerid, action);
    Trunk_Show(playerid, slot);
}

static stock Trunk_Put(playerid, inv_slot, amount)
{
    new slot = g_TrunkSlot[playerid];
    if (!Trunk_CanUse(playerid, slot, true))
    {
        return;
    }
    new E_ITEM:item = g_Inv[playerid][inv_slot][isItem];
    if (item != g_TrunkPickItem[playerid] || item == ITEM_NONE || amount < 1 || amount > g_Inv[playerid][inv_slot][isAmount])
    {
        Msg_Error(playerid, "Toliko tog predmeta nemate.");
        Trunk_Show(playerid, slot);
        return;
    }
    if (Acc_IsAccessory(item) && Acc_IsWorn(playerid, item))
    {
        Msg_Error(playerid, "Prvo skinite %s (/dodaci).", g_ItemInfo[item][itName]);
        Trunk_Show(playerid, slot);
        return;
    }
    new capacity = Trunk_Capacity(g_Veh[slot][vModel]);
    if (!Container_CanAdd(g_Trunk[slot], TRUNK_MAX_SLOTS, capacity, item, amount))
    {
        Msg_Error(playerid, "Nema mesta u gepeku.");
        Trunk_Show(playerid, slot);
        return;
    }
    Inv_TakeFromSlot(playerid, inv_slot, amount);
    new changed = Container_Add(g_Trunk[slot], TRUNK_MAX_SLOTS, capacity, item, amount);
    for (new i = 0; i < TRUNK_MAX_SLOTS; i++)
    {
        if (changed & (1 << i))
        {
            Trunk_SaveSlot(slot, i);
        }
    }
    Trunk_Log(playerid, slot, "put", item, amount);

    new action[64];
    format(action, sizeof(action), "stavlja %s u gepek.", g_ItemInfo[item][itName]);
    Chat_Action(playerid, action);
    Trunk_Show(playerid, slot);
}

cmd:gepek(playerid, params[])
{
    new slot = Trunk_FindBehind(playerid);
    g_TrunkSlot[playerid] = slot;
    if (slot != -1)
    {
        g_TrunkDbId[playerid] = g_Veh[slot][vDbId];
    }
    if (Trunk_CanUse(playerid, slot, true))
    {
        Trunk_Show(playerid, slot);
    }
    return 1;
}
alias:gepek("trunk")

stock bool:Trunk_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
    switch (dialogid)
    {
        case DIALOG_TRUNK:
        {
            new slot = g_TrunkSlot[playerid];
            if (!response || !Trunk_CanUse(playerid, slot, true))
            {
                return true;
            }
            if (listitem == g_TrunkRows[playerid])
            {
                Trunk_ShowPut(playerid);
                return true;
            }
            if (listitem < 0 || listitem > g_TrunkRows[playerid])
            {
                return true;
            }
            new index = g_TrunkListed[playerid][listitem];
            g_TrunkPickItem[playerid] = g_Trunk[slot][index][isItem];
            if (g_Trunk[slot][index][isAmount] == 1 || Item_IsWhole(g_Trunk[slot][index][isItem]))
            {
                Trunk_Take(playerid, index, g_Trunk[slot][index][isAmount]);
                return true;
            }
            g_TrunkPick[playerid] = index;
            Trunk_AskAmount(playerid, DIALOG_TRUNK_TAKE, g_Trunk[slot][index][isAmount]);
        }
        case DIALOG_TRUNK_TAKE:
        {
            if (!response)
            {
                if (Trunk_CanUse(playerid, g_TrunkSlot[playerid], true))
                {
                    Trunk_Show(playerid, g_TrunkSlot[playerid]);
                }
                return true;
            }
            Trunk_Take(playerid, g_TrunkPick[playerid], strval(inputtext));
        }
        case DIALOG_TRUNK_PUT:
        {
            if (!response || listitem < 0 || listitem >= INV_MAX_SLOTS)
            {
                if (Trunk_CanUse(playerid, g_TrunkSlot[playerid], true))
                {
                    Trunk_Show(playerid, g_TrunkSlot[playerid]);
                }
                return true;
            }
            new inv_slot = g_InvListed[playerid][listitem];
            g_TrunkPickItem[playerid] = g_Inv[playerid][inv_slot][isItem];
            if (g_Inv[playerid][inv_slot][isAmount] == 1 || Item_IsWhole(g_Inv[playerid][inv_slot][isItem]))
            {
                Trunk_Put(playerid, inv_slot, g_Inv[playerid][inv_slot][isAmount]);
                return true;
            }
            g_TrunkPick[playerid] = inv_slot;
            Trunk_AskAmount(playerid, DIALOG_TRUNK_PUT_AMOUNT, g_Inv[playerid][inv_slot][isAmount]);
        }
        case DIALOG_TRUNK_PUT_AMOUNT:
        {
            if (!response)
            {
                if (Trunk_CanUse(playerid, g_TrunkSlot[playerid], true))
                {
                    Trunk_ShowPut(playerid);
                }
                return true;
            }
            Trunk_Put(playerid, g_TrunkPick[playerid], strval(inputtext));
        }
        default:
        {
            return false;
        }
    }
    return true;
}

cmd:proverigepek(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_ADMIN))
    {
        return 1;
    }
    new vehicleid;
    if (sscanf(params, "d", vehicleid) || vehicleid < 1 || vehicleid >= MAX_VEHICLES || g_VehSlotOf[vehicleid] == -1)
    {
        Msg_Usage(playerid, "/proverigepek [id vozila] (licno vozilo; ID se vidi sa /vinfo)");
        return 1;
    }
    new slot = g_VehSlotOf[vehicleid], body[1024], line[64], title[64];
    strcopy(body, "Predmet\tKolicina\n");
    for (new i = 0; i < TRUNK_MAX_SLOTS; i++)
    {
        if (g_Trunk[slot][i][isItem] != ITEM_NONE)
        {
            format(line, sizeof(line), "%s\t%d\n", g_ItemInfo[g_Trunk[slot][i][isItem]][itName], g_Trunk[slot][i][isAmount]);
            strcat(body, line);
        }
    }
    if (Trunk_Count(slot) == 0)
    {
        strcat(body, "(prazan)\t-\n");
    }
    format(title, sizeof(title), "Gepek vozila %d (%s, vlasnik %s)", vehicleid, Veh_ModelName(g_Veh[slot][vModel]), g_Veh[slot][vOwnerName]);
    ShowPlayerDialog(playerid, DIALOG_NONE, DIALOG_STYLE_TABLIST_HEADERS, title, body, "Zatvori", "");
    return 1;
}
