new g_HouseInteriorFor[MAX_PLAYERS];
new g_HouseCreatePrice[MAX_PLAYERS];
new g_HouseCreateLevel[MAX_PLAYERS];
new g_HousePicked[MAX_PLAYERS][HOUSE_MAX_PER_PLAYER + HOUSE_VIP_EXTRA];

stock House_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `houses` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`owner_id` INT UNSIGNED NOT NULL DEFAULT 0," \
        "`owner_name` VARCHAR(24) NOT NULL DEFAULT ''," \
        "`price` INT NOT NULL, `level` INT NOT NULL, `interior` TINYINT UNSIGNED NOT NULL," \
        "`x` FLOAT NOT NULL, `y` FLOAT NOT NULL, `z` FLOAT NOT NULL, `a` FLOAT NOT NULL," \
        "`locked` TINYINT(1) NOT NULL DEFAULT 0," \
        "`has_garage` TINYINT(1) NOT NULL DEFAULT 0," \
        "`garage_x` FLOAT NOT NULL DEFAULT 0, `garage_y` FLOAT NOT NULL DEFAULT 0, `garage_z` FLOAT NOT NULL DEFAULT 0, `garage_a` FLOAT NOT NULL DEFAULT 0," \
        "INDEX `house_owner` (`owner_id`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
    mysql_tquery(g_SQL, "ALTER TABLE `houses` ADD COLUMN IF NOT EXISTS `exit_x` FLOAT NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `houses` ADD COLUMN IF NOT EXISTS `exit_y` FLOAT NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `houses` ADD COLUMN IF NOT EXISTS `exit_z` FLOAT NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `houses` ADD COLUMN IF NOT EXISTS `exit_a` FLOAT NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `house_trades` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`house_id` INT UNSIGNED NOT NULL," \
        "`kind` VARCHAR(16) NOT NULL," \
        "`seller_id` INT UNSIGNED NOT NULL DEFAULT 0, `buyer_id` INT UNSIGNED NOT NULL DEFAULT 0," \
        "`amount` INT NOT NULL DEFAULT 0, `tax` INT NOT NULL DEFAULT 0," \
        "`created_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
    mysql_tquery(g_SQL, "ALTER TABLE `accounts` ADD COLUMN IF NOT EXISTS `house_spawn` INT UNSIGNED NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `accounts` ADD COLUMN IF NOT EXISTS `logout_at` INT UNSIGNED NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "SELECT `value` FROM `server_vars` WHERE `name` = 'house_interiors_v2'", "OnHouseInteriorsChecked");
    SetTimer("House_CheckInactive", HOUSE_INACTIVE_CHECK_MS, true);
}

// The first interior list was ordered by guesswork; houses made with it are moved to the same interior in the new list.
forward OnHouseInteriorsChecked();
public OnHouseInteriorsChecked()
{
    if (cache_num_rows() == 0)
    {
        mysql_tquery(g_SQL, "UPDATE `houses` SET `interior` = CASE `interior` WHEN 0 THEN 0 WHEN 1 THEN 7 WHEN 2 THEN 9 WHEN 3 THEN 12 WHEN 4 THEN 0 " \
            "WHEN 5 THEN 15 WHEN 6 THEN 16 WHEN 7 THEN 3 WHEN 8 THEN 8 WHEN 9 THEN 9 WHEN 10 THEN 5 WHEN 11 THEN 14 WHEN 12 THEN 21 WHEN 13 THEN 22 " \
            "WHEN 14 THEN 13 WHEN 15 THEN 23 WHEN 16 THEN 17 WHEN 17 THEN 26 ELSE `interior` END");
        mysql_tquery(g_SQL, "INSERT INTO `server_vars` (`name`, `value`) VALUES ('house_interiors_v2', 1)");
    }
    mysql_tquery(g_SQL, "SELECT * FROM `houses`", "OnHousesLoaded");
    return 1;
}

stock House_OnPlayerConnect(playerid)
{
    g_HouseSelected[playerid] = -1;
    g_HouseSpawn[playerid] = 0;
    g_HouseSpawnPending[playerid] = false;
    g_HouseInteriorFor[playerid] = 0;
}

// Remembered so a quick rejoin does not count as coming home; see HOUSE_SPAWN_AWAY_S.
stock House_OnPlayerDisconnect(playerid)
{
    if (!Account_IsLoggedIn(playerid))
    {
        return;
    }
    new query[96];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `accounts` SET `logout_at` = %d WHERE `id` = %d", gettime(), g_Player[playerid][pDbId]);
    mysql_tquery(g_SQL, query);
    House_ShowOwnerIcons(playerid, false);
}

// The next player with this playerid must not see the icon, so it goes away the moment the owner leaves.
static stock House_ShowOwnerIcons(playerid, bool:online)
{
    for (new slot = 0; slot < MAX_HOUSES; slot++)
    {
        if (g_Houses[slot][hUsed] && g_Houses[slot][hOwnerDbId] == g_Player[playerid][pDbId])
        {
            House_ShowIcon(slot, online ? playerid : INVALID_PLAYER_ID);
        }
    }
}

// Houses for sale are on everyone's map; a house with an owner only on the owner's, so hundreds of houses do not fill the map.
static stock House_ShowIcon(slot, ownerid)
{
    if (IsValidDynamicMapIcon(g_Houses[slot][hIcon])) DestroyDynamicMapIcon(g_Houses[slot][hIcon]);
    g_Houses[slot][hIcon] = STREAMER_TAG_MAP_ICON:INVALID_STREAMER_ID;
    new Float:x = g_Houses[slot][hX], Float:y = g_Houses[slot][hY], Float:z = g_Houses[slot][hZ];
    if (g_Houses[slot][hOwnerDbId] == 0)
    {
        g_Houses[slot][hIcon] = CreateDynamicMapIcon(x, y, z, HOUSE_ICON_SALE, 0, 0, 0, -1, HOUSE_ICON_DISTANCE, MAPICON_LOCAL);
    }
    else if (ownerid != INVALID_PLAYER_ID)
    {
        g_Houses[slot][hIcon] = CreateDynamicMapIcon(x, y, z, HOUSE_ICON_OWNED, 0, 0, 0, ownerid, HOUSE_OWN_ICON_DISTANCE, MAPICON_GLOBAL);
    }
}

forward OnHousesLoaded();
public OnHousesLoaded()
{
    new rows = cache_num_rows(), locked, garage;
    for (new row = 0; row < rows && row < MAX_HOUSES; row++)
    {
        g_Houses[row][hUsed] = true;
        cache_get_value_name_int(row, "id", g_Houses[row][hDbId]);
        cache_get_value_name_int(row, "owner_id", g_Houses[row][hOwnerDbId]);
        cache_get_value_name(row, "owner_name", g_Houses[row][hOwnerName], MAX_PLAYER_NAME + 1);
        cache_get_value_name_int(row, "price", g_Houses[row][hPrice]);
        cache_get_value_name_int(row, "level", g_Houses[row][hLevel]);
        cache_get_value_name_int(row, "interior", g_Houses[row][hInterior]);
        cache_get_value_name_float(row, "x", g_Houses[row][hX]);
        cache_get_value_name_float(row, "y", g_Houses[row][hY]);
        cache_get_value_name_float(row, "z", g_Houses[row][hZ]);
        cache_get_value_name_float(row, "a", g_Houses[row][hA]);
        cache_get_value_name_float(row, "exit_x", g_Houses[row][hExitX]);
        cache_get_value_name_float(row, "exit_y", g_Houses[row][hExitY]);
        cache_get_value_name_float(row, "exit_z", g_Houses[row][hExitZ]);
        cache_get_value_name_float(row, "exit_a", g_Houses[row][hExitA]);
        cache_get_value_name_int(row, "locked", locked);
        cache_get_value_name_int(row, "has_garage", garage);
        g_Houses[row][hLocked] = locked != 0;
        g_Houses[row][hHasGarage] = garage != 0;
        cache_get_value_name_float(row, "garage_x", g_Houses[row][hGarageX]);
        cache_get_value_name_float(row, "garage_y", g_Houses[row][hGarageY]);
        cache_get_value_name_float(row, "garage_z", g_Houses[row][hGarageZ]);
        cache_get_value_name_float(row, "garage_a", g_Houses[row][hGarageA]);
        if (g_Houses[row][hInterior] < 0 || g_Houses[row][hInterior] >= sizeof(g_HouseInteriors))
        {
            g_Houses[row][hInterior] = 0;
        }
        House_Refresh(row);
    }
    printf("[HOUSES] %d houses loaded.", min(rows, MAX_HOUSES));
    House_CheckInactive();
    return 1;
}

forward House_FindSlotByDbId(dbid);
public House_FindSlotByDbId(dbid)
{
    for (new i = 0; i < MAX_HOUSES; i++)
    {
        if (g_Houses[i][hUsed] && g_Houses[i][hDbId] == dbid)
        {
            return i;
        }
    }
    return -1;
}

static stock House_World(slot)
{
    return HOUSE_WORLD_BASE + g_Houses[slot][hDbId];
}

static stock E_HOUSE_SIZE:House_Size(slot)
{
    return g_HouseInteriors[g_Houses[slot][hInterior]][hiSize];
}

// One garage place per size step: a small house holds one vehicle, a large one three.
static stock House_GarageCapacity(slot)
{
    return _:House_Size(slot) + 1;
}

stock House_GarageCapacityOf(house_dbid)
{
    new slot = House_FindSlotByDbId(house_dbid);
    return slot == -1 ? 0 : House_GarageCapacity(slot);
}

stock bool:House_GarageHasRoom(house_dbid)
{
    return Veh_GarageCount(house_dbid) < House_GarageCapacityOf(house_dbid);
}

// The exit spot inside: the one an admin set, or the interior's own entry spot.
static stock House_ExitPos(slot, &Float:x, &Float:y, &Float:z, &Float:angle)
{
    if (g_Houses[slot][hExitX] != 0.0 || g_Houses[slot][hExitY] != 0.0)
    {
        x = g_Houses[slot][hExitX];
        y = g_Houses[slot][hExitY];
        z = g_Houses[slot][hExitZ];
        angle = g_Houses[slot][hExitA];
        return;
    }
    new index = g_Houses[slot][hInterior];
    x = g_HouseInteriors[index][hiX];
    y = g_HouseInteriors[index][hiY];
    z = g_HouseInteriors[index][hiZ];
    angle = 0.0;
}

static stock House_Address(slot, dest[], size = sizeof(dest))
{
    new zone[ZONE_NAME_LENGTH];
    Hud_ZoneName(g_Houses[slot][hX], g_Houses[slot][hY], zone);
    format(dest, size, "%s %d", zone, g_Houses[slot][hDbId]);
}

static stock House_DestroyVisuals(slot)
{
    if (IsValidDynamicPickup(g_Houses[slot][hPickup])) DestroyDynamicPickup(g_Houses[slot][hPickup]);
    if (IsValidDynamic3DTextLabel(g_Houses[slot][hLabel])) DestroyDynamic3DTextLabel(g_Houses[slot][hLabel]);
    if (IsValidDynamicMapIcon(g_Houses[slot][hIcon])) DestroyDynamicMapIcon(g_Houses[slot][hIcon]);
    if (IsValidDynamicPickup(g_Houses[slot][hExitPickup])) DestroyDynamicPickup(g_Houses[slot][hExitPickup]);
    if (IsValidDynamic3DTextLabel(g_Houses[slot][hExitLabel])) DestroyDynamic3DTextLabel(g_Houses[slot][hExitLabel]);
    if (IsValidDynamic3DTextLabel(g_Houses[slot][hGarageLabel])) DestroyDynamic3DTextLabel(g_Houses[slot][hGarageLabel]);
    g_Houses[slot][hPickup] = STREAMER_TAG_PICKUP:INVALID_STREAMER_ID;
    g_Houses[slot][hLabel] = STREAMER_TAG_3D_TEXT_LABEL:INVALID_STREAMER_ID;
    g_Houses[slot][hIcon] = STREAMER_TAG_MAP_ICON:INVALID_STREAMER_ID;
    g_Houses[slot][hExitPickup] = STREAMER_TAG_PICKUP:INVALID_STREAMER_ID;
    g_Houses[slot][hExitLabel] = STREAMER_TAG_3D_TEXT_LABEL:INVALID_STREAMER_ID;
    g_Houses[slot][hGarageLabel] = STREAMER_TAG_3D_TEXT_LABEL:INVALID_STREAMER_ID;
}

// A green house for sale and a blue one with an owner, as players know them from other servers' maps.
stock House_Refresh(slot)
{
    House_DestroyVisuals(slot);
    new text[256], address[48], bool:for_sale = g_Houses[slot][hOwnerDbId] == 0;
    House_Address(slot, address);
    if (for_sale)
    {
        format(text, sizeof(text), "Kuca na prodaju\n" C_WHITE "Adresa: %s\nVelicina: %s%s\nCena: $%d | Nivo: %d\n" C_GREY "/kupikucu",
            address, g_HouseSizeNames[House_Size(slot)], g_Houses[slot][hHasGarage] ? (", garaza") : (""), g_Houses[slot][hPrice], g_Houses[slot][hLevel]);
    }
    else
    {
        format(text, sizeof(text), "Kuca\n" C_WHITE "Adresa: %s\nVlasnik: %s\n" C_GREY "F ili /udji   /kucaj%s",
            address, g_Houses[slot][hOwnerName], g_Houses[slot][hLocked] ? (" " C_RED "(zakljucano)") : (""));
    }
    new Float:x = g_Houses[slot][hX], Float:y = g_Houses[slot][hY], Float:z = g_Houses[slot][hZ];
    g_Houses[slot][hPickup] = CreateDynamicPickup(for_sale ? HOUSE_PICKUP_SALE : HOUSE_PICKUP_OWNED, 1, x, y, z, 0, 0);
    g_Houses[slot][hLabel] = CreateDynamic3DTextLabel(text, for_sale ? COLOR_GREEN : COLOR_SERVER, x, y, z + 0.6, HOUSE_LABEL_DISTANCE,
        INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, 0, 0);
    House_ShowIcon(slot, for_sale ? INVALID_PLAYER_ID : Account_FindOnlineByDbId(g_Houses[slot][hOwnerDbId]));

    new interior = g_HouseInteriors[g_Houses[slot][hInterior]][hiInterior], Float:ix, Float:iy, Float:iz, Float:ia;
    House_ExitPos(slot, ix, iy, iz, ia);
    g_Houses[slot][hExitPickup] = CreateDynamicPickup(HOUSE_PICKUP_OWNED, 1, ix, iy, iz, House_World(slot), interior);
    g_Houses[slot][hExitLabel] = CreateDynamic3DTextLabel("Izlaz\n" C_GREY "F ili /izadji   /kuca", COLOR_WHITE, ix, iy, iz + 0.6, HOUSE_LABEL_DISTANCE,
        INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, House_World(slot), interior);
    if (g_Houses[slot][hHasGarage])
    {
        format(text, sizeof(text), "Garaza (kuca %d)\n" C_GREY "/garaza", g_Houses[slot][hDbId]);
        g_Houses[slot][hGarageLabel] = CreateDynamic3DTextLabel(text, COLOR_WHITE, g_Houses[slot][hGarageX], g_Houses[slot][hGarageY], g_Houses[slot][hGarageZ] + 0.8,
            HOUSE_LABEL_DISTANCE, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, 0, 0);
    }
}

stock House_Save(slot)
{
    new query[640];
    mysql_format(g_SQL, query, sizeof(query),
        "UPDATE `houses` SET `owner_id` = %d, `owner_name` = '%e', `price` = %d, `level` = %d, `interior` = %d, `x` = %f, `y` = %f, `z` = %f, `a` = %f, " \
        "`exit_x` = %f, `exit_y` = %f, `exit_z` = %f, `exit_a` = %f, " \
        "`locked` = %d, `has_garage` = %d, `garage_x` = %f, `garage_y` = %f, `garage_z` = %f, `garage_a` = %f WHERE `id` = %d",
        g_Houses[slot][hOwnerDbId], g_Houses[slot][hOwnerName], g_Houses[slot][hPrice], g_Houses[slot][hLevel], g_Houses[slot][hInterior],
        g_Houses[slot][hX], g_Houses[slot][hY], g_Houses[slot][hZ], g_Houses[slot][hA],
        g_Houses[slot][hExitX], g_Houses[slot][hExitY], g_Houses[slot][hExitZ], g_Houses[slot][hExitA], g_Houses[slot][hLocked] ? 1 : 0,
        g_Houses[slot][hHasGarage] ? 1 : 0, g_Houses[slot][hGarageX], g_Houses[slot][hGarageY], g_Houses[slot][hGarageZ], g_Houses[slot][hGarageA],
        g_Houses[slot][hDbId]);
    mysql_tquery(g_SQL, query);
}

static stock House_LogTrade(slot, const kind[], seller_dbid, buyer_dbid, amount, tax = 0)
{
    new query[256];
    mysql_format(g_SQL, query, sizeof(query),
        "INSERT INTO `house_trades` (`house_id`, `kind`, `seller_id`, `buyer_id`, `amount`, `tax`) VALUES (%d, '%e', %d, %d, %d, %d)",
        g_Houses[slot][hDbId], kind, seller_dbid, buyer_dbid, amount, tax);
    mysql_tquery(g_SQL, query);
}

static stock House_SetOwner(slot, account_dbid, const name[])
{
    g_Houses[slot][hOwnerDbId] = account_dbid;
    strcopy(g_Houses[slot][hOwnerName], name, MAX_PLAYER_NAME + 1);
    g_Houses[slot][hLocked] = false;
    House_Refresh(slot);
    House_Save(slot);
}

// Taken from the owner without them (inactivity, an admin): nobody spawns there any more, vehicles parked in the garage
// get a temporary spot elsewhere and the garage lets its vehicles out.
static stock House_Release(slot)
{
    new query[128], dbid = g_Houses[slot][hDbId];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `accounts` SET `house_spawn` = 0 WHERE `house_spawn` = %d", dbid);
    mysql_tquery(g_SQL, query);
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (g_HouseSpawn[i] == dbid)
        {
            g_HouseSpawn[i] = 0;
        }
    }
    Veh_ReleaseGarageParking(dbid);
    Veh_EmptyGarage(dbid);
    House_SetOwner(slot, 0, "");
}

stock House_CountOwned(account_dbid)
{
    new count;
    for (new i = 0; i < MAX_HOUSES; i++)
    {
        if (g_Houses[i][hUsed] && g_Houses[i][hOwnerDbId] == account_dbid)
        {
            count++;
        }
    }
    return count;
}

// Silver and Gold VIP get one more house; when VIP ends the houses stay, but no new one can be bought while over the limit.
stock House_MaxFor(playerid)
{
    return HOUSE_MAX_PER_PLAYER + (Vip_GetLevel(playerid) >= VIP_SILVER ? HOUSE_VIP_EXTRA : 0);
}

stock bool:House_HasFreeSlot(playerid)
{
    return House_CountOwned(g_Player[playerid][pDbId]) < House_MaxFor(playerid);
}

static stock bool:House_IsOwner(playerid, slot)
{
    return slot != -1 && g_Houses[slot][hOwnerDbId] != 0 && g_Houses[slot][hOwnerDbId] == g_Player[playerid][pDbId];
}

static stock House_FindOutside(playerid, Float:radius = HOUSE_USE_RADIUS)
{
    if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
    {
        return -1;
    }
    for (new i = 0; i < MAX_HOUSES; i++)
    {
        if (g_Houses[i][hUsed] && IsPlayerInRangeOfPoint(playerid, radius, g_Houses[i][hX], g_Houses[i][hY], g_Houses[i][hZ]))
        {
            return i;
        }
    }
    return -1;
}

// The house the player is inside, anywhere in it.
static stock House_FindInsideAnywhere(playerid)
{
    new world = GetPlayerVirtualWorld(playerid);
    if (world <= HOUSE_WORLD_BASE)
    {
        return -1;
    }
    return House_FindSlotByDbId(world - HOUSE_WORLD_BASE);
}

static stock House_FindAtExit(playerid)
{
    new slot = House_FindInsideAnywhere(playerid);
    if (slot == -1)
    {
        return -1;
    }
    new Float:x, Float:y, Float:z, Float:angle;
    House_ExitPos(slot, x, y, z, angle);
    return IsPlayerInRangeOfPoint(playerid, HOUSE_USE_RADIUS, x, y, z) ? slot : -1;
}

// In front of the door or anywhere inside: where the owner can lock, sell or offer the house.
static stock House_FindHere(playerid)
{
    new slot = House_FindInsideAnywhere(playerid);
    return slot != -1 ? slot : House_FindOutside(playerid, HOUSE_NEAR_RADIUS);
}

stock House_FindOwnedHere(playerid)
{
    new slot = House_FindHere(playerid);
    return House_IsOwner(playerid, slot) ? slot : -1;
}

// The player's own house whose garage they are at, by DB id, or 0.
stock House_OwnGarageAt(playerid)
{
    if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
    {
        return 0;
    }
    for (new i = 0; i < MAX_HOUSES; i++)
    {
        if (g_Houses[i][hUsed] && g_Houses[i][hHasGarage] && House_IsOwner(playerid, i)
            && IsPlayerInRangeOfPoint(playerid, HOUSE_GARAGE_RADIUS, g_Houses[i][hGarageX], g_Houses[i][hGarageY], g_Houses[i][hGarageZ]))
        {
            return g_Houses[i][hDbId];
        }
    }
    return 0;
}

static stock House_MoveInside(playerid, slot)
{
    new Float:x, Float:y, Float:z, Float:angle;
    House_ExitPos(slot, x, y, z, angle);
    Utils_MoveInto(playerid, x, y, z, angle, g_HouseInteriors[g_Houses[slot][hInterior]][hiInterior], House_World(slot));
}

static stock House_Enter(playerid, slot)
{
    if (g_Houses[slot][hLocked] && !House_IsOwner(playerid, slot) && !g_Staff[playerid][sAdminDuty])
    {
        Msg_Error(playerid, "Kuca je zakljucana. Mozete pokucati: /kucaj");
        return;
    }
    if (g_Houses[slot][hOwnerDbId] == 0 && !g_Staff[playerid][sAdminDuty])
    {
        Msg_Info(playerid, "Kuca je na prodaji (/kupikucu); u nju se ulazi kad ima vlasnika.");
        return;
    }
    if (!Door_CanUse(playerid))
    {
        return;
    }
    g_DoorLastUse[playerid] = GetTickCount();
    House_MoveInside(playerid, slot);
}

static stock House_Exit(playerid, slot)
{
    if (!Door_CanUse(playerid))
    {
        return;
    }
    g_DoorLastUse[playerid] = GetTickCount();
    Utils_MoveInto(playerid, g_Houses[slot][hX], g_Houses[slot][hY], g_Houses[slot][hZ], g_Houses[slot][hA] + 180.0, 0, 0);
}

// Called by the door code when the player is at no door: F, /udji and /izadji work for houses too. Returns true when a house was used.
stock bool:House_TryUse(playerid, bool:enter, bool:leave)
{
    new slot;
    if (enter && (slot = House_FindOutside(playerid)) != -1)
    {
        House_Enter(playerid, slot);
        return true;
    }
    if (leave && (slot = House_FindAtExit(playerid)) != -1)
    {
        House_Exit(playerid, slot);
        return true;
    }
    return false;
}

stock House_OnLoadAccount(playerid)
{
    new logout_at;
    cache_get_value_name_int(0, "house_spawn", g_HouseSpawn[playerid]);
    cache_get_value_name_int(0, "logout_at", logout_at);
    g_HouseSpawnPending[playerid] = g_HouseSpawn[playerid] != 0 && gettime() - logout_at >= HOUSE_SPAWN_AWAY_S;
    House_ShowOwnerIcons(playerid, true);
}

// Puts the player inside their spawn house when they chose one in /kuca; false when there is none.
stock bool:House_MoveToSpawn(playerid)
{
    new slot = House_FindSlotByDbId(g_HouseSpawn[playerid]);
    if (g_HouseSpawn[playerid] == 0 || !House_IsOwner(playerid, slot))
    {
        return false;
    }
    House_MoveInside(playerid, slot);
    return true;
}

// Only the first spawn after login; after death everyone goes to the hospital, and jail always wins.
stock House_OnPlayerSpawn(playerid)
{
    if (!g_HouseSpawnPending[playerid])
    {
        return;
    }
    g_HouseSpawnPending[playerid] = false;
    if (g_Player[playerid][pJailSeconds] == 0)
    {
        House_MoveToSpawn(playerid);
    }
}

stock bool:House_GaragePos(house_dbid, &Float:x, &Float:y, &Float:z)
{
    new slot = House_FindSlotByDbId(house_dbid);
    if (slot == -1)
    {
        return false;
    }
    x = g_Houses[slot][hGarageX];
    y = g_Houses[slot][hGarageY];
    z = g_Houses[slot][hGarageZ];
    return true;
}

// Someone else's house door or garage close by; the player's own are fine to park at.
stock bool:House_IsOtherDoorNear(playerid, vehicleid, Float:range)
{
    for (new i = 0; i < MAX_HOUSES; i++)
    {
        if (!g_Houses[i][hUsed] || House_IsOwner(playerid, i))
        {
            continue;
        }
        if (GetVehicleDistanceFromPoint(vehicleid, g_Houses[i][hX], g_Houses[i][hY], g_Houses[i][hZ]) < range
            || (g_Houses[i][hHasGarage] && GetVehicleDistanceFromPoint(vehicleid, g_Houses[i][hGarageX], g_Houses[i][hGarageY], g_Houses[i][hGarageZ]) < range))
        {
            return true;
        }
    }
    return false;
}

forward House_CheckInactive();
public House_CheckInactive()
{
    new query[256];
    mysql_format(g_SQL, query, sizeof(query),
        "SELECT h.`id` FROM `houses` h JOIN `accounts` a ON a.`id` = h.`owner_id` WHERE h.`owner_id` <> 0 AND a.`last_login` < NOW() - INTERVAL %d DAY",
        HOUSE_INACTIVE_DAYS);
    mysql_tquery(g_SQL, query, "OnHousesInactive");
    return 1;
}

forward OnHousesInactive();
public OnHousesInactive()
{
    new rows = cache_num_rows(), dbid, query[192];
    for (new row = 0; row < rows; row++)
    {
        cache_get_value_name_int(row, "id", dbid);
        new slot = House_FindSlotByDbId(dbid);
        if (slot == -1 || g_Houses[slot][hOwnerDbId] == 0)
        {
            continue;
        }
        new owner = g_Houses[slot][hOwnerDbId], refund = g_Houses[slot][hPrice] * HOUSE_INACTIVE_PERCENT / 100;
        mysql_format(g_SQL, query, sizeof(query), "UPDATE `accounts` SET `bank` = `bank` + %d WHERE `id` = %d", refund, owner);
        mysql_tquery(g_SQL, query);
        House_LogTrade(slot, "inactive", owner, 0, refund);
        printf("[HOUSES] House %d taken back from inactive account %d (%d refunded to the bank).", dbid, owner, refund);
        House_Release(slot);
    }
    return 1;
}

static stock House_ShowInteriors(playerid, for_dbid)
{
    new body[2048], line[64], stars[8];
    g_HouseInteriorFor[playerid] = for_dbid;
    strcopy(body, "Velicina\tEnterijer\tOcena\n");
    for (new i = 0; i < sizeof(g_HouseInteriors); i++)
    {
        if (g_HouseInteriors[i][hiStars] > 0)
        {
            format(stars, sizeof(stars), "%d/10", g_HouseInteriors[i][hiStars]);
        }
        else
        {
            strcopy(stars, "-");
        }
        format(line, sizeof(line), "%s\t%s\t%s\n", g_HouseSizeNames[g_HouseInteriors[i][hiSize]], g_HouseInteriors[i][hiName], stars);
        strcat(body, line);
    }
    ShowPlayerDialog(playerid, DIALOG_HOUSE_INTERIOR, DIALOG_STYLE_TABLIST_HEADERS, "Enterijer kuce", body, "Izaberi", "Odustani");
}

static stock House_Create(playerid, interior)
{
    new slot = -1;
    for (new i = 0; i < MAX_HOUSES; i++)
    {
        if (!g_Houses[i][hUsed])
        {
            slot = i;
            break;
        }
    }
    if (slot == -1)
    {
        Msg_Error(playerid, "Dostignut je najveci broj kuca (%d).", MAX_HOUSES);
        return;
    }
    static const empty[E_HOUSE];
    new dbid = Utils_FreeDbId("House_FindSlotByDbId");
    g_Houses[slot] = empty;
    g_Houses[slot][hUsed] = true;
    g_Houses[slot][hDbId] = dbid;
    g_Houses[slot][hPrice] = g_HouseCreatePrice[playerid];
    g_Houses[slot][hLevel] = g_HouseCreateLevel[playerid];
    g_Houses[slot][hInterior] = interior;
    GetPlayerPos(playerid, g_Houses[slot][hX], g_Houses[slot][hY], g_Houses[slot][hZ]);
    GetPlayerFacingAngle(playerid, g_Houses[slot][hA]);
    new query[256];
    mysql_format(g_SQL, query, sizeof(query), "INSERT INTO `houses` (`id`, `price`, `level`, `interior`, `x`, `y`, `z`, `a`) VALUES (%d, %d, %d, %d, %f, %f, %f, %f)",
        dbid, g_Houses[slot][hPrice], g_Houses[slot][hLevel], interior, g_Houses[slot][hX], g_Houses[slot][hY], g_Houses[slot][hZ], g_Houses[slot][hA]);
    mysql_tquery(g_SQL, query);
    House_Refresh(slot);

    new details[64];
    format(details, sizeof(details), "kuca %d: %s, $%d, nivo %d", dbid, g_HouseInteriors[interior][hiName], g_Houses[slot][hPrice], g_Houses[slot][hLevel]);
    Staff_Log(playerid, "house_create", INVALID_PLAYER_ID, details);
    Msg_Admin(playerid, "Napravljena je %s. Garaza i izlaz: /izmenikucu %d", details, dbid);
}

static stock House_ShowMenu(playerid, slot)
{
    new body[256], line[64];
    g_HouseSelected[playerid] = slot;
    strcopy(body, "Informacije\n");
    strcat(body, g_Houses[slot][hLocked] ? ("Otkljucaj\n") : ("Zakljucaj\n"));
    strcat(body, "Oznaci na GPS-u\n");
    format(line, sizeof(line), "Pojavljivanje u kuci: %s\n", g_HouseSpawn[playerid] == g_Houses[slot][hDbId] ? ("DA") : ("NE"));
    strcat(body, line);
    format(line, sizeof(line), "Prodaj drzavi ($%d)", g_Houses[slot][hPrice] * HOUSE_SELL_PERCENT / 100);
    strcat(body, line);
    new title[48], address[48];
    House_Address(slot, address);
    format(title, sizeof(title), "Kuca: %s", address);
    ShowPlayerDialog(playerid, DIALOG_HOUSE_MENU, DIALOG_STYLE_LIST, title, body, "Izaberi", "Zatvori");
}

static stock House_ShowInfo(playerid, slot)
{
    new text[512], address[48], garage[48];
    House_Address(slot, address);
    if (g_Houses[slot][hHasGarage])
    {
        format(garage, sizeof(garage), "ima, %d od %d mesta zauzeto", Veh_GarageCount(g_Houses[slot][hDbId]), House_GarageCapacity(slot));
    }
    else
    {
        strcopy(garage, "nema");
    }
    format(text, sizeof(text), "Adresa: %s\nVelicina: %s (%s)\nGaraza: %s\nVrata: %s\n\nVrednost: $%d\nProdaja drzavi: $%d (%d%%)\n\n" \
        "Pojavljivanje u kuci vazi pri ulasku na server (ako niste bili na serveru bar %d min) i posle zatvora.\n" \
        "Kuca se vraca na prodaju ako ne udjete na server %d dana (dobijate %d%% na racun).",
        address, g_HouseSizeNames[House_Size(slot)], g_HouseInteriors[g_Houses[slot][hInterior]][hiName], garage, g_Houses[slot][hLocked] ? ("zakljucana") : ("otkljucana"),
        g_Houses[slot][hPrice], g_Houses[slot][hPrice] * HOUSE_SELL_PERCENT / 100, HOUSE_SELL_PERCENT, HOUSE_SPAWN_AWAY_S / 60, HOUSE_INACTIVE_DAYS, HOUSE_INACTIVE_PERCENT);
    ShowPlayerDialog(playerid, DIALOG_HOUSE_INFO, DIALOG_STYLE_MSGBOX, "Informacije o kuci", text, "Nazad", "");
}

static stock bool:House_RequireHere(playerid, slot)
{
    if (House_FindHere(playerid) == slot)
    {
        return true;
    }
    Msg_Error(playerid, "Morate biti kod kuce (ispred vrata ili unutra).");
    return false;
}

static stock House_ToggleLock(playerid, slot)
{
    g_Houses[slot][hLocked] = !g_Houses[slot][hLocked];
    House_Refresh(slot);
    House_Save(slot);
    Chat_Action(playerid, g_Houses[slot][hLocked] ? ("zakljucava vrata kuce.") : ("otkljucava vrata kuce."));
}

static stock House_SetSpawn(playerid, dbid)
{
    new query[96];
    g_HouseSpawn[playerid] = dbid;
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `accounts` SET `house_spawn` = %d WHERE `id` = %d", dbid, g_Player[playerid][pDbId]);
    mysql_tquery(g_SQL, query);
}

// A house changes hands only with nothing of the old owner's in its garage, stored or parked there.
static stock bool:House_CheckGarageFree(playerid, slot)
{
    new dbid = g_Houses[slot][hDbId];
    if (Veh_GarageCount(dbid) > 0)
    {
        Msg_Error(playerid, "Prvo izvadite vozila iz garaze (/garaza).");
        return false;
    }
    if (Veh_GarageParkedCount(dbid) > 0)
    {
        Msg_Error(playerid, "Neko vozilo ima parking u ovoj garazi; parkirajte ga negde drugde (/v parkiraj).");
        return false;
    }
    return true;
}

static stock House_SellToState(playerid, slot)
{
    if (!House_IsOwner(playerid, slot) || !House_RequireHere(playerid, slot) || !House_CheckGarageFree(playerid, slot))
    {
        return;
    }
    new amount = g_Houses[slot][hPrice] * HOUSE_SELL_PERCENT / 100, reason[48];
    format(reason, sizeof(reason), "prodaja kuce %d drzavi", g_Houses[slot][hDbId]);
    Money_GiveCash(playerid, amount, reason);
    House_LogTrade(slot, "state", g_Player[playerid][pDbId], 0, amount);
    House_Release(slot);
    Account_Save(playerid);
    Msg_Success(playerid, "Prodali ste kucu drzavi za $%d.", amount);
}

// Called by /ponudi: the house the seller stands at, ready to change hands right now.
stock bool:House_IsSaleReady(slot, dbid, seller)
{
    return slot >= 0 && slot < MAX_HOUSES && g_Houses[slot][hUsed] && g_Houses[slot][hDbId] == dbid && House_IsOwner(seller, slot)
        && House_FindHere(seller) == slot && Veh_GarageCount(dbid) == 0 && Veh_GarageParkedCount(dbid) == 0;
}

stock House_Level(slot)
{
    return g_Houses[slot][hLevel];
}

stock House_DbId(slot)
{
    return g_Houses[slot][hDbId];
}

stock House_SaleText(slot, dest[], size = sizeof(dest))
{
    new address[48];
    House_Address(slot, address);
    format(dest, size, "Adresa: %s\nVelicina: %s (%s)\nGaraza: %s", address, g_HouseSizeNames[House_Size(slot)], g_HouseInteriors[g_Houses[slot][hInterior]][hiName],
        g_Houses[slot][hHasGarage] ? ("ima") : ("nema"));
}

stock House_TransferTo(slot, seller, buyer, amount, tax)
{
    if (g_HouseSpawn[seller] == g_Houses[slot][hDbId])
    {
        House_SetSpawn(seller, 0);
    }
    House_LogTrade(slot, "sale", g_Player[seller][pDbId], g_Player[buyer][pDbId], amount, tax);
    House_SetOwner(slot, g_Player[buyer][pDbId], g_Player[buyer][pName]);
}

static stock House_ShowGarage(playerid, slot)
{
    new body[256], line[64], count, dbid = g_Houses[slot][hDbId];
    for (new v = 0; v < MAX_PERSONAL_VEHICLES && count < sizeof(g_HouseGarageListed[]); v++)
    {
        if (g_Veh[v][vUsed] && g_Veh[v][vGarage] == dbid)
        {
            format(line, sizeof(line), "%s\t%d%%\n", Veh_Name(v), Veh_ConditionOf(g_Veh[v][vHealth]));
            strcat(body, line);
            g_HouseGarageListed[playerid][count++] = v;
        }
    }
    if (count == 0)
    {
        Msg_Info(playerid, "Garaza je prazna. Vozilo se ostavlja tako sto ga dovezete ovde i ukucate /garaza za volanom.");
        return;
    }
    g_HouseSelected[playerid] = slot;
    ShowPlayerDialog(playerid, DIALOG_HOUSE_GARAGE, DIALOG_STYLE_TABLIST, "Garaza: izaberite vozilo", body, "Izvadi", "Zatvori");
}

cmd:garaza(playerid, params[])
{
    new slot = -1;
    for (new i = 0; i < MAX_HOUSES; i++)
    {
        if (g_Houses[i][hUsed] && g_Houses[i][hHasGarage] && GetPlayerVirtualWorld(playerid) == 0 && GetPlayerInterior(playerid) == 0
            && IsPlayerInRangeOfPoint(playerid, HOUSE_GARAGE_RADIUS, g_Houses[i][hGarageX], g_Houses[i][hGarageY], g_Houses[i][hGarageZ]))
        {
            slot = i;
            break;
        }
    }
    if (slot == -1)
    {
        Msg_Error(playerid, "Niste ispred garaze kuce.");
        return 1;
    }
    if (!House_IsOwner(playerid, slot))
    {
        Msg_Error(playerid, "Ovo nije garaza vase kuce.");
        return 1;
    }
    if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
    {
        House_ShowGarage(playerid, slot);
        return 1;
    }
    new vehicleid = GetPlayerVehicleID(playerid), vslot = g_VehSlotOf[vehicleid];
    if (vslot == -1 || !Veh_IsOwner(playerid, vehicleid))
    {
        Msg_Error(playerid, "U garazu se ostavlja samo vase licno vozilo.");
        return 1;
    }
    if (!House_GarageHasRoom(g_Houses[slot][hDbId]))
    {
        Msg_Error(playerid, "Garaza je puna (%d/%d).", Veh_GarageCount(g_Houses[slot][hDbId]), House_GarageCapacity(slot));
        return 1;
    }
    if (Veh_SpeedKmh(vehicleid) > 5.0)
    {
        Msg_Error(playerid, "Prvo zaustavite vozilo.");
        return 1;
    }
    new name[48];
    strcopy(name, Veh_Name(vslot));
    Chat_Action(playerid, "uvozi vozilo u garazu.");
    Veh_StoreInGarage(vslot, g_Houses[slot][hDbId]);
    Msg_Success(playerid, "Vozilo %s je u garazi. Vadi se sa /garaza ispred garaze.", name);
    return 1;
}

cmd:kupikucu(playerid, params[])
{
    new slot = House_FindOutside(playerid, HOUSE_NEAR_RADIUS);
    if (slot == -1)
    {
        Msg_Error(playerid, "Niste ispred kuce.");
        return 1;
    }
    if (g_Houses[slot][hOwnerDbId] != 0)
    {
        Msg_Error(playerid, "Ova kuca nije na prodaju.");
        return 1;
    }
    if (g_Player[playerid][pLevel] < g_Houses[slot][hLevel])
    {
        Msg_Error(playerid, "Za ovu kucu treba nivo %d.", g_Houses[slot][hLevel]);
        return 1;
    }
    if (!House_HasFreeSlot(playerid))
    {
        Msg_Error(playerid, "Vec imate najvise kuca (%d). Silver i Gold VIP mogu imati jos jednu.", House_MaxFor(playerid));
        return 1;
    }
    new reason[32];
    format(reason, sizeof(reason), "kupovina kuce %d", g_Houses[slot][hDbId]);
    if (!Money_Take(playerid, g_Houses[slot][hPrice], reason))
    {
        return 1;
    }
    House_LogTrade(slot, "buy", 0, g_Player[playerid][pDbId], g_Houses[slot][hPrice]);
    House_SetOwner(slot, g_Player[playerid][pDbId], g_Player[playerid][pName]);
    Account_Save(playerid);
    Msg_Success(playerid, "Kupili ste kucu za $%d. Meni kuce: /kuca (zakljucavanje, GPS, pojavljivanje u kuci, prodaja).", g_Houses[slot][hPrice]);
    return 1;
}
alias:kupikucu("buyhouse")

cmd:kuca(playerid, params[])
{
    new slot = House_FindOwnedHere(playerid);
    if (slot != -1)
    {
        House_ShowMenu(playerid, slot);
        return 1;
    }
    new body[192], line[64], count, address[48];
    for (new i = 0; i < MAX_HOUSES && count < sizeof(g_HousePicked[]); i++)
    {
        if (House_IsOwner(playerid, i))
        {
            House_Address(i, address);
            format(line, sizeof(line), "%s\n", address);
            strcat(body, line);
            g_HousePicked[playerid][count++] = i;
        }
    }
    if (count == 0)
    {
        Msg_Error(playerid, "Nemate kucu. Kuce na prodaju imaju zelenu kucicu na mapi.");
    }
    else if (count == 1)
    {
        House_ShowMenu(playerid, g_HousePicked[playerid][0]);
    }
    else
    {
        ShowPlayerDialog(playerid, DIALOG_HOUSE_PICK, DIALOG_STYLE_LIST, "Vase kuce", body, "Izaberi", "Zatvori");
    }
    return 1;
}
alias:kuca("house")

// Heard only by the people inside that house, not by an owner who is somewhere else.
cmd:kucaj(playerid, params[])
{
    new slot = House_FindOutside(playerid, HOUSE_NEAR_RADIUS);
    if (slot == -1)
    {
        Msg_Error(playerid, "Niste ispred vrata kuce.");
        return 1;
    }
    new world = House_World(slot), inside;
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (Account_IsLoggedIn(i) && GetPlayerVirtualWorld(i) == world)
        {
            Msg_Info(i, "Neko kuca na vrata kuce.");
            inside++;
        }
    }
    Chat_Action(playerid, "kuca na vrata.");
    if (inside == 0)
    {
        Msg_Info(playerid, "Niko ne otvara.");
    }
    return 1;
}

cmd:napravikucu(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }
    new price, level;
    if (sscanf(params, "dd", price, level))
    {
        Msg_Usage(playerid, "/napravikucu [cena] [nivo] (stanite ispred vrata kuce; enterijer se bira iz liste)");
        return 1;
    }
    if (price < HOUSE_MIN_PRICE || price > HOUSE_MAX_PRICE)
    {
        Msg_Error(playerid, "Cena kuce je od $%d do $%d.", HOUSE_MIN_PRICE, HOUSE_MAX_PRICE);
        return 1;
    }
    if (level < 1 || level > 100)
    {
        Msg_Error(playerid, "Nivo je od 1 do 100.");
        return 1;
    }
    if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
    {
        Msg_Error(playerid, "Kuca se pravi napolju, ispred njenih vrata.");
        return 1;
    }
    g_HouseCreatePrice[playerid] = price;
    g_HouseCreateLevel[playerid] = level;
    House_ShowInteriors(playerid, 0);
    return 1;
}

static stock bool:House_ParseSlot(playerid, const params[], const usage[], &slot)
{
    new dbid;
    if (sscanf(params, "d", dbid))
    {
        Msg_Usage(playerid, "%s", usage);
        return false;
    }
    slot = House_FindSlotByDbId(dbid);
    if (slot == -1)
    {
        Msg_Error(playerid, "Kuca sa tim ID-jem ne postoji. Spisak: /kuce");
        return false;
    }
    return true;
}

static const g_HouseEditMenu[][E_MENU_ITEM] =
{
    { ADMIN_LEVEL_HEAD, "Cena", "izmenikucu %s cena", "Nova cena:" },
    { ADMIN_LEVEL_HEAD, "Nivo", "izmenikucu %s nivo", "Potreban nivo:" },
    { ADMIN_LEVEL_HEAD, "Enterijer (lista)", "izmenikucu %s enterijer", "" },
    { ADMIN_LEVEL_HEAD, "Pomeri ulaz ovde (napolju, gde stojite)", "izmenikucu %s ovde", "" },
    { ADMIN_LEVEL_HEAD, "Pomeri izlaz ovde (u kuci, gde stojite)", "izmenikucu %s izlaz", "" },
    { ADMIN_LEVEL_HEAD, "Garaza ovde (sedite u vozilo)", "izmenikucu %s garaza", "" },
    { ADMIN_LEVEL_HEAD, "Ukloni garazu", "izmenikucu %s bezgaraze", "" },
    { ADMIN_LEVEL_HEAD, "Oduzmi vlasniku (vrati na prodaju)", "izmenikucu %s oduzmi", "" },
    { ADMIN_LEVEL_HEAD, "Udji u kucu", "izmenikucu %s udji", "" }
};

cmd:izmenikucu(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }
    new dbid, option[16], value[16];
    if (sscanf(params, "dS()[16]S()[16]", dbid, option, value))
    {
        Msg_Usage(playerid, "/izmenikucu [id] (otvara meni sa svim opcijama; spisak kuca: /kuce)");
        return 1;
    }
    new slot = House_FindSlotByDbId(dbid);
    if (slot == -1)
    {
        Msg_Error(playerid, "Kuca sa tim ID-jem ne postoji. Spisak: /kuce");
        return 1;
    }
    if (isnull(option))
    {
        new title[32], subject[8];
        format(title, sizeof(title), "Kuca %d", dbid);
        valstr(subject, dbid);
        Menu_Show(playerid, title, g_HouseEditMenu, sizeof(g_HouseEditMenu), subject);
        return 1;
    }
    new details[64], number = strval(value);
    if (!strcmp(option, "cena", true))
    {
        if (number < HOUSE_MIN_PRICE || number > HOUSE_MAX_PRICE)
        {
            Msg_Error(playerid, "Cena je od $%d do $%d.", HOUSE_MIN_PRICE, HOUSE_MAX_PRICE);
            return 1;
        }
        g_Houses[slot][hPrice] = number;
        format(details, sizeof(details), "kuca %d: cena $%d", dbid, number);
    }
    else if (!strcmp(option, "nivo", true))
    {
        if (number < 1 || number > 100)
        {
            Msg_Error(playerid, "Nivo je od 1 do 100.");
            return 1;
        }
        g_Houses[slot][hLevel] = number;
        format(details, sizeof(details), "kuca %d: nivo %d", dbid, number);
    }
    else if (!strcmp(option, "enterijer", true))
    {
        House_ShowInteriors(playerid, dbid);
        return 1;
    }
    else if (!strcmp(option, "ovde", true))
    {
        if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
        {
            Msg_Error(playerid, "Ulaz kuce mora biti napolju.");
            return 1;
        }
        GetPlayerPos(playerid, g_Houses[slot][hX], g_Houses[slot][hY], g_Houses[slot][hZ]);
        GetPlayerFacingAngle(playerid, g_Houses[slot][hA]);
        format(details, sizeof(details), "kuca %d: ulaz pomeren", dbid);
    }
    else if (!strcmp(option, "izlaz", true))
    {
        if (GetPlayerVirtualWorld(playerid) != House_World(slot))
        {
            Msg_Error(playerid, "Udjite u tu kucu (/izmenikucu %d udji) i stanite gde treba da bude izlaz.", dbid);
            return 1;
        }
        GetPlayerPos(playerid, g_Houses[slot][hExitX], g_Houses[slot][hExitY], g_Houses[slot][hExitZ]);
        GetPlayerFacingAngle(playerid, g_Houses[slot][hExitA]);
        format(details, sizeof(details), "kuca %d: izlaz pomeren", dbid);
    }
    else if (!strcmp(option, "garaza", true))
    {
        // From the driver's seat, so the spot and direction are exactly where a vehicle comes out, clear of the garage door.
        if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER || GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
        {
            Msg_Error(playerid, "Sedite u vozilo i parkirajte ga ispred garaze, okrenuto kako treba da izlazi, pa ponovite.");
            return 1;
        }
        new vehicleid = GetPlayerVehicleID(playerid);
        GetVehiclePos(vehicleid, g_Houses[slot][hGarageX], g_Houses[slot][hGarageY], g_Houses[slot][hGarageZ]);
        GetVehicleZAngle(vehicleid, g_Houses[slot][hGarageA]);
        g_Houses[slot][hHasGarage] = true;
        format(details, sizeof(details), "kuca %d: garaza postavljena", dbid);
    }
    else if (!strcmp(option, "bezgaraze", true))
    {
        Veh_ReleaseGarageParking(dbid);
        Veh_EmptyGarage(dbid);
        g_Houses[slot][hHasGarage] = false;
        format(details, sizeof(details), "kuca %d: garaza uklonjena", dbid);
    }
    else if (!strcmp(option, "oduzmi", true))
    {
        if (g_Houses[slot][hOwnerDbId] == 0)
        {
            Msg_Error(playerid, "Kuca nema vlasnika.");
            return 1;
        }
        format(details, sizeof(details), "kuca %d: oduzeta od %s", dbid, g_Houses[slot][hOwnerName]);
        new ownerid = Account_FindOnlineByDbId(g_Houses[slot][hOwnerDbId]);
        House_LogTrade(slot, "admin", g_Houses[slot][hOwnerDbId], 0, 0);
        House_Release(slot);
        if (ownerid != INVALID_PLAYER_ID)
        {
            Msg_Info(ownerid, "Admin %s vam je oduzeo kucu %d; vratila se na prodaju.", Account_Name(playerid), dbid);
        }
    }
    else if (!strcmp(option, "udji", true))
    {
        House_MoveInside(playerid, slot);
        return 1;
    }
    else
    {
        Msg_Error(playerid, "Nepoznata opcija. Sve opcije otvara /izmenikucu %d", dbid);
        return 1;
    }
    House_Refresh(slot);
    House_Save(slot);
    Staff_Log(playerid, "house_edit", INVALID_PLAYER_ID, details);
    Msg_Admin(playerid, "Izmenjena je %s.", details);
    return 1;
}

cmd:obrisikucu(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }
    new slot;
    if (!House_ParseSlot(playerid, params, "/obrisikucu [id] (samo kuca bez vlasnika)", slot))
    {
        return 1;
    }
    if (g_Houses[slot][hOwnerDbId] != 0)
    {
        Msg_Error(playerid, "Kuca ima vlasnika (%s). Prvo je oduzmite: /izmenikucu %d", g_Houses[slot][hOwnerName], g_Houses[slot][hDbId]);
        return 1;
    }
    // The id is given out again, so nobody may stay in this house's world and turn up in the next one.
    new world = House_World(slot);
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (IsPlayerConnected(i) && GetPlayerVirtualWorld(i) == world)
        {
            Utils_MoveInto(i, g_Houses[slot][hX], g_Houses[slot][hY], g_Houses[slot][hZ], g_Houses[slot][hA] + 180.0, 0, 0);
        }
    }
    new query[64], details[32];
    format(details, sizeof(details), "kuca %d", g_Houses[slot][hDbId]);
    mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `houses` WHERE `id` = %d", g_Houses[slot][hDbId]);
    mysql_tquery(g_SQL, query);
    House_DestroyVisuals(slot);
    g_Houses[slot][hUsed] = false;
    Staff_Log(playerid, "house_delete", INVALID_PLAYER_ID, details);
    Msg_Admin(playerid, "Obrisana je %s.", details);
    return 1;
}

cmd:gotokuca(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }
    new slot;
    if (!House_ParseSlot(playerid, params, "/gotokuca [id] (spisak: /kuce)", slot))
    {
        return 1;
    }
    Utils_MoveInto(playerid, g_Houses[slot][hX], g_Houses[slot][hY], g_Houses[slot][hZ], g_Houses[slot][hA] + 180.0, 0, 0);
    return 1;
}

cmd:kuce(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }
    new body[4096], line[96], address[48], count;
    strcopy(body, "ID\tAdresa\tVlasnik\tCena\n");
    for (new i = 0; i < MAX_HOUSES && strlen(body) < sizeof(body) - sizeof(line); i++)
    {
        if (!g_Houses[i][hUsed])
        {
            continue;
        }
        new owner[MAX_PLAYER_NAME + 1] = "na prodaju";
        if (g_Houses[i][hOwnerDbId] != 0)
        {
            strcopy(owner, g_Houses[i][hOwnerName]);
        }
        House_Address(i, address);
        format(line, sizeof(line), "%d\t%s\t%s\t$%d\n", g_Houses[i][hDbId], address, owner, g_Houses[i][hPrice]);
        strcat(body, line);
        count++;
    }
    if (count == 0)
    {
        Msg_Info(playerid, "Jos nema kuca. Pravi se sa /napravikucu [cena] [nivo].");
        return 1;
    }
    ShowPlayerDialog(playerid, DIALOG_ADMIN_INFO, DIALOG_STYLE_TABLIST_HEADERS, SERVER_NAME " - Kuce", body, "Zatvori", "");
    return 1;
}

stock bool:House_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
    #pragma unused inputtext
    switch (dialogid)
    {
        case DIALOG_HOUSE_INTERIOR:
        {
            if (!response || listitem < 0 || listitem >= sizeof(g_HouseInteriors) || !Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
            {
                return true;
            }
            if (g_HouseInteriorFor[playerid] == 0)
            {
                House_Create(playerid, listitem);
                return true;
            }
            new slot = House_FindSlotByDbId(g_HouseInteriorFor[playerid]);
            if (slot == -1)
            {
                return true;
            }
            // A new interior has its own layout, so a moved exit from the old one would hang in the air.
            g_Houses[slot][hInterior] = listitem;
            g_Houses[slot][hExitX] = 0.0;
            g_Houses[slot][hExitY] = 0.0;
            g_Houses[slot][hExitZ] = 0.0;
            g_Houses[slot][hExitA] = 0.0;
            House_Refresh(slot);
            House_Save(slot);
            new details[64];
            format(details, sizeof(details), "kuca %d: enterijer %s", g_Houses[slot][hDbId], g_HouseInteriors[listitem][hiName]);
            Staff_Log(playerid, "house_edit", INVALID_PLAYER_ID, details);
            Msg_Admin(playerid, "Izmenjena je %s.", details);
        }
        case DIALOG_HOUSE_PICK:
        {
            if (response && listitem >= 0 && listitem < sizeof(g_HousePicked[]) && House_IsOwner(playerid, g_HousePicked[playerid][listitem]))
            {
                House_ShowMenu(playerid, g_HousePicked[playerid][listitem]);
            }
        }
        case DIALOG_HOUSE_INFO:
        {
            if (House_IsOwner(playerid, g_HouseSelected[playerid]))
            {
                House_ShowMenu(playerid, g_HouseSelected[playerid]);
            }
        }
        case DIALOG_HOUSE_MENU:
        {
            new slot = g_HouseSelected[playerid];
            if (!response || !House_IsOwner(playerid, slot))
            {
                return true;
            }
            switch (listitem)
            {
                case 0: House_ShowInfo(playerid, slot);
                case 1:
                {
                    if (House_RequireHere(playerid, slot))
                    {
                        House_ToggleLock(playerid, slot);
                    }
                }
                case 2:
                {
                    new address[48];
                    House_Address(slot, address);
                    Gps_StartAt(playerid, -1, address, g_Houses[slot][hX], g_Houses[slot][hY], g_Houses[slot][hZ]);
                }
                case 3:
                {
                    new bool:on = g_HouseSpawn[playerid] != g_Houses[slot][hDbId];
                    House_SetSpawn(playerid, on ? g_Houses[slot][hDbId] : 0);
                    if (on)
                    {
                        Msg_Success(playerid, "Pojavljujete se u ovoj kuci pri ulasku na server i posle zatvora.");
                    }
                    else
                    {
                        Msg_Success(playerid, "Pojavljivanje u kuci je iskljuceno.");
                    }
                    House_ShowMenu(playerid, slot);
                }
                case 4:
                {
                    new text[192];
                    format(text, sizeof(text), "Prodajete kucu drzavi za $%d (%d%% vrednosti).\n\nGaraza mora biti prazna i nijedno vozilo ne sme imati parking u njoj.",
                        g_Houses[slot][hPrice] * HOUSE_SELL_PERCENT / 100, HOUSE_SELL_PERCENT);
                    ShowPlayerDialog(playerid, DIALOG_HOUSE_SELL, DIALOG_STYLE_MSGBOX, "Prodaja kuce", text, "Prodaj", "Odustani");
                }
            }
        }
        case DIALOG_HOUSE_SELL:
        {
            if (response)
            {
                House_SellToState(playerid, g_HouseSelected[playerid]);
            }
        }
        case DIALOG_HOUSE_GARAGE:
        {
            new slot = g_HouseSelected[playerid];
            if (!response || listitem < 0 || listitem >= sizeof(g_HouseGarageListed[]) || !House_IsOwner(playerid, slot))
            {
                return true;
            }
            new vslot = g_HouseGarageListed[playerid][listitem];
            if (!g_Veh[vslot][vUsed] || g_Veh[vslot][vGarage] != g_Houses[slot][hDbId]
                || !IsPlayerInRangeOfPoint(playerid, HOUSE_GARAGE_RADIUS * 2.0, g_Houses[slot][hGarageX], g_Houses[slot][hGarageY], g_Houses[slot][hGarageZ]))
            {
                return true;
            }
            if (!Veh_TakeFromGarage(vslot, g_Houses[slot][hGarageX], g_Houses[slot][hGarageY], g_Houses[slot][hGarageZ], g_Houses[slot][hGarageA]))
            {
                Msg_Error(playerid, "Vozilo trenutno ne moze da izadje; pokusajte ponovo.");
                return true;
            }
            Chat_Action(playerid, "izvozi vozilo iz garaze.");
            Msg_Success(playerid, "Vozilo %s je izaslo iz garaze.", Veh_Name(vslot));
        }
        default:
        {
            return false;
        }
    }
    return true;
}
