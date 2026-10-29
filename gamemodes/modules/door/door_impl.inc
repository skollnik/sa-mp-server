stock Door_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `doors` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`name` VARCHAR(48) NOT NULL," \
        "`out_x` FLOAT NOT NULL, `out_y` FLOAT NOT NULL, `out_z` FLOAT NOT NULL, `out_a` FLOAT NOT NULL," \
        "`out_interior` INT NOT NULL DEFAULT 0, `out_world` INT NOT NULL DEFAULT 0," \
        "`has_exit` TINYINT(1) NOT NULL DEFAULT 0," \
        "`in_x` FLOAT NOT NULL DEFAULT 0, `in_y` FLOAT NOT NULL DEFAULT 0, `in_z` FLOAT NOT NULL DEFAULT 0, `in_a` FLOAT NOT NULL DEFAULT 0," \
        "`in_interior` INT NOT NULL DEFAULT 0, `in_world` INT NOT NULL DEFAULT 0," \
        "`locked` TINYINT(1) NOT NULL DEFAULT 0," \
        "`icon` TINYINT UNSIGNED NOT NULL DEFAULT 0" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");

    mysql_tquery(g_SQL, "SELECT * FROM `doors`", "OnDoorsLoaded");
}

stock Door_OnPlayerConnect(playerid)
{
    g_DoorLastUse[playerid] = 0;
}

forward OnDoorsLoaded();
public OnDoorsLoaded()
{
    new rows = cache_num_rows(), loaded;
    for (new row = 0; row < rows && row < MAX_DOORS; row++)
    {
        new slot = row;
        g_Doors[slot][dUsed] = true;
        cache_get_value_name_int(row, "id", g_Doors[slot][dDbId]);
        cache_get_value_name(row, "name", g_Doors[slot][dName], DOOR_NAME_LENGTH);
        cache_get_value_name_float(row, "out_x", g_Doors[slot][dOutX]);
        cache_get_value_name_float(row, "out_y", g_Doors[slot][dOutY]);
        cache_get_value_name_float(row, "out_z", g_Doors[slot][dOutZ]);
        cache_get_value_name_float(row, "out_a", g_Doors[slot][dOutA]);
        cache_get_value_name_int(row, "out_interior", g_Doors[slot][dOutInterior]);
        cache_get_value_name_int(row, "out_world", g_Doors[slot][dOutWorld]);
        cache_get_value_name_bool(row, "has_exit", g_Doors[slot][dHasExit]);
        cache_get_value_name_float(row, "in_x", g_Doors[slot][dInX]);
        cache_get_value_name_float(row, "in_y", g_Doors[slot][dInY]);
        cache_get_value_name_float(row, "in_z", g_Doors[slot][dInZ]);
        cache_get_value_name_float(row, "in_a", g_Doors[slot][dInA]);
        cache_get_value_name_int(row, "in_interior", g_Doors[slot][dInInterior]);
        cache_get_value_name_int(row, "in_world", g_Doors[slot][dInWorld]);
        cache_get_value_name_bool(row, "locked", g_Doors[slot][dLocked]);
        cache_get_value_name_int(row, "icon", g_Doors[slot][dIcon]);
        Door_Refresh(slot);
        loaded++;
    }
    printf("[DOORS] %d doors loaded.", loaded);
    return 1;
}

stock Door_FindSlotByDbId(dbid)
{
    for (new i = 0; i < MAX_DOORS; i++)
    {
        if (g_Doors[i][dUsed] && g_Doors[i][dDbId] == dbid)
        {
            return i;
        }
    }
    return -1;
}

stock Door_FindFreeSlot()
{
    for (new i = 0; i < MAX_DOORS; i++)
    {
        if (!g_Doors[i][dUsed])
        {
            return i;
        }
    }
    return -1;
}

static stock Door_DestroyVisuals(slot)
{
    if (IsValidDynamicPickup(g_Doors[slot][dOutPickup])) DestroyDynamicPickup(g_Doors[slot][dOutPickup]);
    if (IsValidDynamic3DTextLabel(g_Doors[slot][dOutLabel])) DestroyDynamic3DTextLabel(g_Doors[slot][dOutLabel]);
    if (IsValidDynamicPickup(g_Doors[slot][dInPickup])) DestroyDynamicPickup(g_Doors[slot][dInPickup]);
    if (IsValidDynamic3DTextLabel(g_Doors[slot][dInLabel])) DestroyDynamic3DTextLabel(g_Doors[slot][dInLabel]);
    if (IsValidDynamicMapIcon(g_Doors[slot][dMapIcon])) DestroyDynamicMapIcon(g_Doors[slot][dMapIcon]);

    g_Doors[slot][dOutPickup] = STREAMER_TAG_PICKUP:INVALID_STREAMER_ID;
    g_Doors[slot][dOutLabel] = STREAMER_TAG_3D_TEXT_LABEL:INVALID_STREAMER_ID;
    g_Doors[slot][dInPickup] = STREAMER_TAG_PICKUP:INVALID_STREAMER_ID;
    g_Doors[slot][dInLabel] = STREAMER_TAG_3D_TEXT_LABEL:INVALID_STREAMER_ID;
    g_Doors[slot][dMapIcon] = STREAMER_TAG_MAP_ICON:INVALID_STREAMER_ID;
}

stock Door_Refresh(slot)
{
    Door_DestroyVisuals(slot);

    new text[128];
    format(text, sizeof(text), "%s\n" C_GREY "F ili /udji%s", g_Doors[slot][dName], g_Doors[slot][dLocked] ? (" " C_RED "(zakljucano)") : (""));
    g_Doors[slot][dOutPickup] = CreateDynamicPickup(DOOR_PICKUP_MODEL, 1, g_Doors[slot][dOutX], g_Doors[slot][dOutY], g_Doors[slot][dOutZ],
        g_Doors[slot][dOutWorld], g_Doors[slot][dOutInterior]);
    g_Doors[slot][dOutLabel] = CreateDynamic3DTextLabel(text, COLOR_WHITE, g_Doors[slot][dOutX], g_Doors[slot][dOutY], g_Doors[slot][dOutZ] + 0.6,
        DOOR_LABEL_DISTANCE, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, g_Doors[slot][dOutWorld], g_Doors[slot][dOutInterior]);

    if (g_Doors[slot][dHasExit])
    {
        g_Doors[slot][dInPickup] = CreateDynamicPickup(DOOR_PICKUP_MODEL, 1, g_Doors[slot][dInX], g_Doors[slot][dInY], g_Doors[slot][dInZ],
            g_Doors[slot][dInWorld], g_Doors[slot][dInInterior]);
        g_Doors[slot][dInLabel] = CreateDynamic3DTextLabel("Izlaz\n" C_GREY "F ili /izadji", COLOR_WHITE, g_Doors[slot][dInX], g_Doors[slot][dInY], g_Doors[slot][dInZ] + 0.6,
            DOOR_LABEL_DISTANCE, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, g_Doors[slot][dInWorld], g_Doors[slot][dInInterior]);
    }

    if (g_Doors[slot][dIcon] > 0)
    {
        g_Doors[slot][dMapIcon] = CreateDynamicMapIcon(g_Doors[slot][dOutX], g_Doors[slot][dOutY], g_Doors[slot][dOutZ], g_Doors[slot][dIcon], 0,
            g_Doors[slot][dOutWorld], g_Doors[slot][dOutInterior], -1, DOOR_ICON_DISTANCE, MAPICON_GLOBAL);
    }
}

stock Door_Save(slot)
{
    new query[640];
    mysql_format(g_SQL, query, sizeof(query),
        "UPDATE `doors` SET `name` = '%e', `out_x` = %f, `out_y` = %f, `out_z` = %f, `out_a` = %f, `out_interior` = %d, `out_world` = %d, " \
        "`has_exit` = %d, `in_x` = %f, `in_y` = %f, `in_z` = %f, `in_a` = %f, `in_interior` = %d, `in_world` = %d, " \
        "`locked` = %d, `icon` = %d WHERE `id` = %d",
        g_Doors[slot][dName], g_Doors[slot][dOutX], g_Doors[slot][dOutY], g_Doors[slot][dOutZ], g_Doors[slot][dOutA],
        g_Doors[slot][dOutInterior], g_Doors[slot][dOutWorld], g_Doors[slot][dHasExit] ? 1 : 0,
        g_Doors[slot][dInX], g_Doors[slot][dInY], g_Doors[slot][dInZ], g_Doors[slot][dInA],
        g_Doors[slot][dInInterior], g_Doors[slot][dInWorld], g_Doors[slot][dLocked] ? 1 : 0, g_Doors[slot][dIcon],
        g_Doors[slot][dDbId]);
    mysql_tquery(g_SQL, query);
}

stock Door_GetPlayerSpot(playerid, &Float:x, &Float:y, &Float:z, &Float:angle)
{
    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, angle);
}

stock Door_SetOutside(slot, playerid)
{
    Door_GetPlayerSpot(playerid, g_Doors[slot][dOutX], g_Doors[slot][dOutY], g_Doors[slot][dOutZ], g_Doors[slot][dOutA]);
    g_Doors[slot][dOutInterior] = GetPlayerInterior(playerid);
    g_Doors[slot][dOutWorld] = GetPlayerVirtualWorld(playerid);
}

// The admin is moved into the door's own world right away, so what they see is what players will see.
stock Door_SetInside(slot, playerid)
{
    Door_GetPlayerSpot(playerid, g_Doors[slot][dInX], g_Doors[slot][dInY], g_Doors[slot][dInZ], g_Doors[slot][dInA]);
    g_Doors[slot][dInInterior] = GetPlayerInterior(playerid);
    if (!g_Doors[slot][dHasExit])
    {
        g_Doors[slot][dInWorld] = DOOR_WORLD_BASE + g_Doors[slot][dDbId];
    }
    g_Doors[slot][dHasExit] = true;
    SetPlayerVirtualWorld(playerid, g_Doors[slot][dInWorld]);
}

// IDs of deleted doors are given out again, so the IDs admins type stay short.
stock Door_FreeDbId()
{
    new dbid = 1;
    while (Door_FindSlotByDbId(dbid) != -1)
    {
        dbid++;
    }
    return dbid;
}

// Whoever is inside a deleted door's own world would be stuck without an exit, and since IDs are reused,
// they and the service points there would turn up inside the next door with this ID.
// Returns how many service points were removed.
static stock Door_ClearInside(slot)
{
    new world = DOOR_WORLD_BASE + g_Doors[slot][dDbId];
    if (!g_Doors[slot][dHasExit] || g_Doors[slot][dInWorld] != world)
    {
        return 0;
    }
    for (new i = 0; i < MAX_DOORS; i++)
    {
        if (i != slot && g_Doors[i][dUsed] && g_Doors[i][dHasExit] && g_Doors[i][dInWorld] == world)
        {
            return 0;
        }
    }

    new Float:x = g_Doors[slot][dOutX], Float:y = g_Doors[slot][dOutY], Float:z = g_Doors[slot][dOutZ];
    new Float:angle = g_Doors[slot][dOutA] + 180.0, interior = g_Doors[slot][dOutInterior], out_world = g_Doors[slot][dOutWorld];
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (!Account_IsLoggedIn(i))
        {
            continue;
        }
        if (g_Staff[i][sSpectating])
        {
            if (g_Staff[i][sSpecWorld] == world)
            {
                g_Staff[i][sSpecX] = x;
                g_Staff[i][sSpecY] = y;
                g_Staff[i][sSpecZ] = z;
                g_Staff[i][sSpecInterior] = interior;
                g_Staff[i][sSpecWorld] = out_world;
            }
        }
        else if (GetPlayerVirtualWorld(i) == world)
        {
            Utils_MoveInto(i, x, y, z, angle, interior, out_world, !g_Staff[i][sFrozen]);
            Msg_Info(i, "Ulaz u ovu zgradu je uklonjen, pa ste premesteni napolje.");
        }
    }

    new query[256];
    mysql_format(g_SQL, query, sizeof(query),
        "UPDATE `accounts` SET `pos_x` = %f, `pos_y` = %f, `pos_z` = %f, `pos_a` = %f, `interior` = %d, `virtual_world` = %d WHERE `virtual_world` = %d",
        x, y, z, angle, interior, out_world, world);
    mysql_tquery(g_SQL, query);

    return Point_DestroyInWorld(world);
}

// Returns how many service points inside the door were removed with it.
stock Door_Destroy(slot)
{
    new points = Door_ClearInside(slot), query[64];
    mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `doors` WHERE `id` = %d", g_Doors[slot][dDbId]);
    mysql_tquery(g_SQL, query);

    Door_DestroyVisuals(slot);
    g_Doors[slot][dUsed] = false;
    return points;
}

stock Door_FindOutside(playerid)
{
    new interior = GetPlayerInterior(playerid), world = GetPlayerVirtualWorld(playerid);
    for (new i = 0; i < MAX_DOORS; i++)
    {
        if (g_Doors[i][dUsed] && g_Doors[i][dOutInterior] == interior && g_Doors[i][dOutWorld] == world
            && IsPlayerInRangeOfPoint(playerid, DOOR_USE_RADIUS, g_Doors[i][dOutX], g_Doors[i][dOutY], g_Doors[i][dOutZ]))
        {
            return i;
        }
    }
    return -1;
}

stock Door_FindInside(playerid)
{
    new interior = GetPlayerInterior(playerid), world = GetPlayerVirtualWorld(playerid);
    for (new i = 0; i < MAX_DOORS; i++)
    {
        if (g_Doors[i][dUsed] && g_Doors[i][dHasExit] && g_Doors[i][dInInterior] == interior && g_Doors[i][dInWorld] == world
            && IsPlayerInRangeOfPoint(playerid, DOOR_USE_RADIUS, g_Doors[i][dInX], g_Doors[i][dInY], g_Doors[i][dInZ]))
        {
            return i;
        }
    }
    return -1;
}

// Prints why the door cannot be used; returns true when it can.
stock bool:Door_CanUse(playerid)
{
    if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT)
    {
        Msg_Error(playerid, "Kroz vrata se prolazi samo peske.");
        return false;
    }
    // A frozen player would be unfrozen by the interior loading, so freezing must hold here too.
    if (g_Player[playerid][pJailSeconds] > 0 || g_Staff[playerid][sFrozen])
    {
        Msg_Error(playerid, "Trenutno ne mozete koristiti vrata.");
        return false;
    }
    if (Account_SecondsSinceDamage(playerid) < DOOR_COMBAT_LOCK_S)
    {
        Msg_Error(playerid, "Ne mozete proci kroz vrata odmah posle primljene stete.");
        return false;
    }
    if (GetTickCount() - g_DoorLastUse[playerid] < DOOR_USE_COOLDOWN_MS)
    {
        return false;
    }
    return true;
}

stock Door_Enter(playerid, slot)
{
    if (!g_Doors[slot][dHasExit])
    {
        Msg_Error(playerid, "Ova vrata jos nisu povezana sa enterijerom.");
        return;
    }
    if (g_Doors[slot][dLocked] && !g_Staff[playerid][sAdminDuty])
    {
        Msg_Error(playerid, "Vrata su zakljucana.");
        return;
    }
    if (!Door_CanUse(playerid))
    {
        return;
    }

    g_DoorLastUse[playerid] = GetTickCount();
    Utils_MoveInto(playerid, g_Doors[slot][dInX], g_Doors[slot][dInY], g_Doors[slot][dInZ], g_Doors[slot][dInA],
        g_Doors[slot][dInInterior], g_Doors[slot][dInWorld]);
}

stock Door_Exit(playerid, slot)
{
    if (!Door_CanUse(playerid))
    {
        return;
    }

    g_DoorLastUse[playerid] = GetTickCount();
    Utils_MoveInto(playerid, g_Doors[slot][dOutX], g_Doors[slot][dOutY], g_Doors[slot][dOutZ], g_Doors[slot][dOutA] + 180.0,
        g_Doors[slot][dOutInterior], g_Doors[slot][dOutWorld]);
}

// F (enter vehicle key) works on both sides of a door.
stock Door_OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys)
{
    if (!(newkeys & KEY_SECONDARY_ATTACK) || (oldkeys & KEY_SECONDARY_ATTACK) || GetPlayerState(playerid) != PLAYER_STATE_ONFOOT)
    {
        return;
    }

    new slot = Door_FindOutside(playerid);
    if (slot != -1)
    {
        Door_Enter(playerid, slot);
        return;
    }
    slot = Door_FindInside(playerid);
    if (slot != -1)
    {
        Door_Exit(playerid, slot);
    }
}
