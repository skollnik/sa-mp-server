#define TRUNK_MAX_SLOTS         (20)
#define TRUNK_RANGE             (2.0)

// Belongs to the vehicle, not the owner: items stay in it when it is sold or traded.
new g_Trunk[MAX_PERSONAL_VEHICLES][TRUNK_MAX_SLOTS][E_INV_SLOT];
new bool:g_TrunkLoaded[MAX_PERSONAL_VEHICLES];
// Grows on every change, so an offer for the vehicle notices that the trunk was emptied after it was sent.
new g_TrunkVersion[MAX_PERSONAL_VEHICLES];

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

// Two-wheelers, boats, aircraft, trailers and trains have none.
stock Trunk_Capacity(model)
{
    switch (Veh_Class(model))
    {
        case VEH_CLASS_CAR: return 6;
        case VEH_CLASS_VAN: return 12;
        case VEH_CLASS_TRUCK: return 20;
        default: return 0;
    }
    return 0;
}

stock Trunk_SaveSlot(slot, index)
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


// "prazan" or how many kinds of items are inside, for an offer or an admin.
stock Trunk_Summary(slot, dest[], size = sizeof(dest))
{
    new count = Store_Count(STORE_TRUNK, slot);
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

stock Trunk_Log(playerid, slot, const action[], E_ITEM:item, amount)
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

// Checked by the storage menus on every answer, since the owner may close or lock the trunk or drive off meanwhile.
stock bool:Trunk_CanUse(playerid, slot, bool:tell)
{
    new message[96];
    if (!g_Veh[slot][vUsed] || g_Veh[slot][vVehicleId] == INVALID_VEHICLE_ID || !Trunk_IsBehind(playerid, g_Veh[slot][vVehicleId]))
    {
        strcopy(message, "Stanite iza vozila sa otvorenim gepekom (otvara ga vlasnik: /v gepek).");
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

cmd:gepek(playerid, params[])
{
    new slot = Trunk_FindBehind(playerid);
    if (slot == -1)
    {
        Msg_Error(playerid, "Stanite iza vozila sa otvorenim gepekom (otvara ga vlasnik: /v gepek).");
        return 1;
    }
    Store_Open(playerid, STORE_TRUNK, slot);
    return 1;
}
alias:gepek("trunk")

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
    new slot = g_VehSlotOf[vehicleid], body[1024], title[64];
    strcopy(body, "Predmet\tKolicina\n");
    if (Store_ListItems(STORE_TRUNK, slot, body, sizeof(body)) == 0)
    {
        strcat(body, "(prazan)\t-\n");
    }
    format(title, sizeof(title), "Gepek vozila %d (%s, vlasnik %s)", vehicleid, Veh_ModelName(g_Veh[slot][vModel]), g_Veh[slot][vOwnerName]);
    ShowPlayerDialog(playerid, DIALOG_NONE, DIALOG_STYLE_TABLIST_HEADERS, title, body, "Zatvori", "");
    return 1;
}
