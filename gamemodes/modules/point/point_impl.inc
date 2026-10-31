stock Point_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `points` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`type` TINYINT UNSIGNED NOT NULL," \
        "`x` FLOAT NOT NULL, `y` FLOAT NOT NULL, `z` FLOAT NOT NULL, `a` FLOAT NOT NULL," \
        "`interior` INT NOT NULL DEFAULT 0, `world` INT NOT NULL DEFAULT 0" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");

    mysql_tquery(g_SQL, "SELECT * FROM `points`", "OnPointsLoaded");
}

forward OnPointsLoaded();
public OnPointsLoaded()
{
    new rows = cache_num_rows(), type;
    for (new row = 0; row < rows && row < MAX_POINTS; row++)
    {
        g_Points[row][ptUsed] = true;
        cache_get_value_name_int(row, "id", g_Points[row][ptDbId]);
        cache_get_value_name_int(row, "type", type);
        g_Points[row][ptType] = E_POINT_TYPE:type;
        cache_get_value_name_float(row, "x", g_Points[row][ptX]);
        cache_get_value_name_float(row, "y", g_Points[row][ptY]);
        cache_get_value_name_float(row, "z", g_Points[row][ptZ]);
        cache_get_value_name_float(row, "a", g_Points[row][ptA]);
        cache_get_value_name_int(row, "interior", g_Points[row][ptInterior]);
        cache_get_value_name_int(row, "world", g_Points[row][ptWorld]);
        Point_CreateVisuals(row);
    }
    printf("[POINTS] %d service points loaded.", min(rows, MAX_POINTS));
    return 1;
}

stock Point_CreateVisuals(slot)
{
    new text[64], E_POINT_TYPE:type = g_Points[slot][ptType];
    format(text, sizeof(text), "%s\n" C_GREY "%s", g_PointInfo[type][piName], g_PointInfo[type][piCommand]);

    new Float:x = g_Points[slot][ptX], Float:y = g_Points[slot][ptY], Float:z = g_Points[slot][ptZ];
    if (type == POINT_ATM)
    {
        // The machine stands a step in front of the spot where the admin stood, facing them.
        new Float:angle = g_Points[slot][ptA];
        g_Points[slot][ptObject] = CreateDynamicObject(POINT_ATM_MODEL, x + 0.8 * floatsin(-angle, degrees), y + 0.8 * floatcos(-angle, degrees), z - 0.35,
            0.0, 0.0, angle, g_Points[slot][ptWorld], g_Points[slot][ptInterior]);
    }
    else
    {
        g_Points[slot][ptPickup] = CreateDynamicPickup(g_PointInfo[type][piPickup], 1, x, y, z, g_Points[slot][ptWorld], g_Points[slot][ptInterior]);
    }
    g_Points[slot][ptLabel] = CreateDynamic3DTextLabel(text, COLOR_WHITE, x, y, z + 0.5, POINT_LABEL_DISTANCE,
        INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, g_Points[slot][ptWorld], g_Points[slot][ptInterior]);
}

static stock Point_DestroyVisuals(slot)
{
    if (IsValidDynamicPickup(g_Points[slot][ptPickup])) DestroyDynamicPickup(g_Points[slot][ptPickup]);
    if (IsValidDynamic3DTextLabel(g_Points[slot][ptLabel])) DestroyDynamic3DTextLabel(g_Points[slot][ptLabel]);
    if (IsValidDynamicObject(g_Points[slot][ptObject])) DestroyDynamicObject(g_Points[slot][ptObject]);
    g_Points[slot][ptPickup] = STREAMER_TAG_PICKUP:INVALID_STREAMER_ID;
    g_Points[slot][ptLabel] = STREAMER_TAG_3D_TEXT_LABEL:INVALID_STREAMER_ID;
    g_Points[slot][ptObject] = STREAMER_TAG_OBJECT:INVALID_STREAMER_ID;
}

stock Point_Destroy(slot)
{
    new query[64];
    mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `points` WHERE `id` = %d", g_Points[slot][ptDbId]);
    mysql_tquery(g_SQL, query);
    Point_DestroyVisuals(slot);

    static const empty[E_POINT];
    g_Points[slot] = empty;
}

stock Point_FindSlotByDbId(dbid)
{
    for (new i = 0; i < MAX_POINTS; i++)
    {
        if (g_Points[i][ptUsed] && g_Points[i][ptDbId] == dbid)
        {
            return i;
        }
    }
    return -1;
}

// IDs of deleted points are given out again, so the IDs admins see stay short. Called before the new slot is marked used:
// a reused slot still holds the deleted entry's ID, which would look taken.
stock Point_FreeDbId()
{
    new dbid = 1;
    while (Point_FindSlotByDbId(dbid) != -1)
    {
        dbid++;
    }
    return dbid;
}

// Used when the door that led into this world is deleted; returns how many points were removed.
stock Point_DestroyInWorld(world)
{
    new count;
    for (new i = 0; i < MAX_POINTS; i++)
    {
        if (g_Points[i][ptUsed] && g_Points[i][ptWorld] == world)
        {
            Point_Destroy(i);
            count++;
        }
    }
    return count;
}

stock Point_FindNearest(playerid, Float:radius)

{
    new interior = GetPlayerInterior(playerid), world = GetPlayerVirtualWorld(playerid);
    for (new i = 0; i < MAX_POINTS; i++)
    {
        if (g_Points[i][ptUsed] && g_Points[i][ptInterior] == interior && g_Points[i][ptWorld] == world
            && IsPlayerInRangeOfPoint(playerid, radius, g_Points[i][ptX], g_Points[i][ptY], g_Points[i][ptZ]))
        {
            return i;
        }
    }
    return -1;
}

stock bool:Point_IsNear(playerid, E_POINT_TYPE:type)
{
    new interior = GetPlayerInterior(playerid), world = GetPlayerVirtualWorld(playerid);
    for (new i = 0; i < MAX_POINTS; i++)
    {
        if (g_Points[i][ptUsed] && g_Points[i][ptType] == type && g_Points[i][ptInterior] == interior && g_Points[i][ptWorld] == world
            && IsPlayerInRangeOfPoint(playerid, g_PointInfo[type][piRadius], g_Points[i][ptX], g_Points[i][ptY], g_Points[i][ptZ]))
        {
            return true;
        }
    }
    return false;
}

// Prints where the service is when the player is not at one; returns true when they are.
stock bool:Point_Require(playerid, E_POINT_TYPE:type)
{
    if (Point_IsNear(playerid, type))
    {
        return true;
    }
    Msg_Error(playerid, "Morate biti kod mesta: %s.", g_PointInfo[type][piName]);
    return false;
}

static stock Point_Create(playerid, E_POINT_TYPE:type)
{
    new slot = -1;
    for (new i = 0; i < MAX_POINTS; i++)
    {
        if (!g_Points[i][ptUsed])
        {
            slot = i;
            break;
        }
    }
    if (slot == -1)
    {
        Msg_Error(playerid, "Dostignut je najveci broj mesta (%d).", MAX_POINTS);
        return;
    }

    g_Points[slot][ptDbId] = Point_FreeDbId();
    g_Points[slot][ptUsed] = true;
    g_Points[slot][ptType] = type;
    GetPlayerPos(playerid, g_Points[slot][ptX], g_Points[slot][ptY], g_Points[slot][ptZ]);
    GetPlayerFacingAngle(playerid, g_Points[slot][ptA]);
    g_Points[slot][ptInterior] = GetPlayerInterior(playerid);
    g_Points[slot][ptWorld] = GetPlayerVirtualWorld(playerid);
    Point_CreateVisuals(slot);

    new query[256], details[48];
    mysql_format(g_SQL, query, sizeof(query),
        "INSERT INTO `points` (`id`, `type`, `x`, `y`, `z`, `a`, `interior`, `world`) VALUES (%d, %d, %f, %f, %f, %f, %d, %d)",
        g_Points[slot][ptDbId], _:type, g_Points[slot][ptX], g_Points[slot][ptY], g_Points[slot][ptZ], g_Points[slot][ptA],
        g_Points[slot][ptInterior], g_Points[slot][ptWorld]);
    mysql_tquery(g_SQL, query);

    format(details, sizeof(details), "mesto %d: %s", g_Points[slot][ptDbId], g_PointInfo[g_Points[slot][ptType]][piKey]);
    Staff_Log(playerid, "point_create", INVALID_PLAYER_ID, details);
    Msg_Admin(playerid, "Napravljeno je %s (%s).", details, g_PointInfo[g_Points[slot][ptType]][piName]);
}

static stock Point_ShowTypes(playerid)
{
    new body[512], line[64];
    for (new E_POINT_TYPE:t; t < E_POINT_TYPE; t++)
    {
        format(line, sizeof(line), "%s\t%s\n", g_PointInfo[t][piName], g_PointInfo[t][piCommand]);
        strcat(body, line);
    }
    ShowPlayerDialog(playerid, DIALOG_POINT_TYPE, DIALOG_STYLE_TABLIST, "Novo mesto (stanite tamo gde igrac treba da stoji)", body, "Napravi", "Zatvori");
}

stock bool:Point_OnDialogResponse(playerid, dialogid, response, listitem)
{
    if (dialogid != DIALOG_POINT_TYPE)
    {
        return false;
    }
    if (response && listitem >= 0 && listitem < _:E_POINT_TYPE && Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        Point_Create(playerid, E_POINT_TYPE:listitem);
    }
    return true;
}

// Without a parameter a list of the types opens, so nobody has to remember the keys.
cmd:napravimesto(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }
    if (isnull(params))
    {
        Point_ShowTypes(playerid);
        return 1;
    }
    for (new E_POINT_TYPE:t; t < E_POINT_TYPE; t++)
    {
        if (!strcmp(params, g_PointInfo[t][piKey], true))
        {
            Point_Create(playerid, t);
            return 1;
        }
    }
    Msg_Usage(playerid, "/napravimesto (lista) ili /napravimesto [banka / bankomat / opstina / prodavnica / odeca / pumpa / pregled / policija]");
    return 1;
}

cmd:obrisimesto(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }

    new slot = Point_FindNearest(playerid, 3.0);
    if (slot == -1)
    {
        Msg_Error(playerid, "U blizini (3 m) nema mesta za brisanje.");
        return 1;
    }

    new details[48];
    format(details, sizeof(details), "mesto %d: %s", g_Points[slot][ptDbId], g_PointInfo[g_Points[slot][ptType]][piKey]);
    Staff_Log(playerid, "point_delete", INVALID_PLAYER_ID, details);
    Msg_Admin(playerid, "Obrisano je %s.", details);
    Point_Destroy(slot);
    return 1;
}

cmd:mesta(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }

    new body[2048], line[96], count;
    for (new i = 0; i < MAX_POINTS; i++)
    {
        if (!g_Points[i][ptUsed])
        {
            continue;
        }
        format(line, sizeof(line), "%d\t%s\t%.0f, %.0f, %.0f (enterijer %d)\n", g_Points[i][ptDbId], g_PointInfo[g_Points[i][ptType]][piName],
            g_Points[i][ptX], g_Points[i][ptY], g_Points[i][ptZ], g_Points[i][ptInterior]);
        if (strlen(body) + strlen(line) >= sizeof(body))
        {
            break;
        }
        strcat(body, line);
        count++;
    }

    if (count == 0)
    {
        Msg_Info(playerid, "Jos nema mesta. Napravite ih sa /napravimesto.");
        return 1;
    }
    ShowPlayerDialog(playerid, DIALOG_ADMIN_INFO, DIALOG_STYLE_TABLIST, SERVER_NAME " - Mesta", body, "Zatvori", "");
    return 1;
}

static const g_PointEditMenu[][E_MENU_ITEM] =
{
    { ADMIN_LEVEL_HEAD, "Pomeri ovde (gde stojite)", "izmenimesto %s ovde", "" },
    { ADMIN_LEVEL_HEAD, "Promeni tip", "izmenimesto %s tip", "Tip (banka, bankomat, opstina, prodavnica, odeca, pumpa...):" }
};

cmd:izmenimesto(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }
    new dbid, option[16], value[16];
    if (sscanf(params, "dS()[16]S()[16]", dbid, option, value))
    {
        Msg_Usage(playerid, "/izmenimesto [id] (meni) ili /izmenimesto [id] [ovde / tip] [tip] (ID: /mesta)");
        return 1;
    }
    new slot = Point_FindSlotByDbId(dbid);
    if (slot == -1)
    {
        Msg_Error(playerid, "Mesto sa tim ID-jem ne postoji. Spisak: /mesta");
        return 1;
    }
    if (isnull(option))
    {
        new title[48], subject[8];
        format(title, sizeof(title), "Mesto %d: %s", dbid, g_PointInfo[g_Points[slot][ptType]][piName]);
        valstr(subject, dbid);
        Menu_Show(playerid, title, g_PointEditMenu, sizeof(g_PointEditMenu), subject);
        return 1;
    }

    if (!strcmp(option, "ovde", true))
    {
        GetPlayerPos(playerid, g_Points[slot][ptX], g_Points[slot][ptY], g_Points[slot][ptZ]);
        GetPlayerFacingAngle(playerid, g_Points[slot][ptA]);
        g_Points[slot][ptInterior] = GetPlayerInterior(playerid);
        g_Points[slot][ptWorld] = GetPlayerVirtualWorld(playerid);
    }
    else if (!strcmp(option, "tip", true))
    {
        new E_POINT_TYPE:type = E_POINT_TYPE:-1;
        for (new E_POINT_TYPE:t; t < E_POINT_TYPE; t++)
        {
            if (!strcmp(value, g_PointInfo[t][piKey], true) && value[0])
            {
                type = t;
            }
        }
        if (type == E_POINT_TYPE:-1)
        {
            Msg_Error(playerid, "Nepoznat tip. Tipovi: banka, bankomat, opstina, prodavnica, odeca, pumpa, pregled, policija.");
            return 1;
        }
        g_Points[slot][ptType] = type;
    }
    else
    {
        Msg_Error(playerid, "Opcije: ovde, tip.");
        return 1;
    }

    Point_DestroyVisuals(slot);
    Point_CreateVisuals(slot);
    new query[224], details[48];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `points` SET `type` = %d, `x` = %f, `y` = %f, `z` = %f, `a` = %f, `interior` = %d, `world` = %d WHERE `id` = %d",
        _:g_Points[slot][ptType], g_Points[slot][ptX], g_Points[slot][ptY], g_Points[slot][ptZ], g_Points[slot][ptA], g_Points[slot][ptInterior], g_Points[slot][ptWorld], dbid);
    mysql_tquery(g_SQL, query);
    format(details, sizeof(details), "mesto %d: %s %s", dbid, option, value);
    Staff_Log(playerid, "point_edit", INVALID_PLAYER_ID, details);
    Msg_Admin(playerid, "Izmenjeno je %s.", details);
    return 1;
}
