#define MAX_CAMERAS             (100)
#define CAMERA_MODEL            (18880)
#define CAMERA_RADIUS           (10.0)
#define CAMERA_LABEL_DISTANCE   (25.0)
// A few km/h over the limit is measuring error, as with real cameras.
#define CAMERA_SPEED_TOLERANCE  (5)
#define CAMERA_COOLDOWN_S       (600)
#define CAMERA_FLASH_SOUND      (1132)
#define CAMERA_STREAM_DISTANCE  (300.0)

enum E_CAMERA
{
    bool:camUsed,
    camDbId,
    Float:camX,
    Float:camY,
    Float:camZ,
    Float:camA,
    camLimit,
    STREAMER_TAG_OBJECT:camObject,
    STREAMER_TAG_AREA:camArea,
    STREAMER_TAG_3D_TEXT_LABEL:camLabel
}

new g_Cameras[MAX_CAMERAS][E_CAMERA];

// Starting set of Los Santos cameras with their limits; written once, after that the admin adds and removes them in game.
static const Float:g_CameraSeed[][5] =
{
    { 1619.3, -1726.97, 12.5391, 270.342, 50.0 },
    { 1394.6, -1791.84, 12.5469, 90.5103, 50.0 },
    { 1317.8, -1751.28, 12.5469, 180.461, 50.0 },
    { 1292.02, -1677.66, 12.5469, 0.918984, 50.0 },
    { 1350.43, -1163.68, 23.4406, 85.7112, 60.0 },
    { 1361.76, -1049.95, 25.7767, 89.7845, 60.0 },
    { 1317.97, -1624.9, 12.5469, 4.13503, 60.0 },
    { 1363.01, -1421.15, 12.5391, 352.855, 70.0 },
    { 1363.08, -1287.79, 12.5469, 359.122, 80.0 },
    { 1212.47, -1135.21, 22.9368, 87.7958, 70.0 },
    { 1059.14, -1135.16, 22.8281, 89.6758, 70.0 },
    { 791.768, -1166.04, 22.1861, 181.17, 70.0 },
    { 791.25, -1384.59, 12.6996, 168.637, 70.0 },
    { 1040.25, -1411.38, 12.4017, 271.388, 70.0 },
    { 1247.98, -1411.51, 12.2283, 269.508, 70.0 },
    { 1535.17, -1687.93, 12.5469, 355.988, 60.0 },
    { 1447.87, -1586.14, 12.5469, 90.6157, 60.0 },
    { 1460.35, -1461.57, 12.5469, 7.26828, 70.0 },
    { 1432.14, -1435.21, 12.5547, 84.6858, 50.0 },
    { 1536.97, -1309.0, 14.9673, 265.457, 60.0 },
    { 1311.72, -2062.12, 57.1736, 92.2057, 60.0 },
    { 1435.42, -1995.86, 49.5242, 220.047, 50.0 },
    { 1244.95, -1920.9, 30.3919, 143.69, 60.0 },
    { 1409.4, -1922.26, 15.4148, 88.7591, 60.0 },
    { 1512.3, -1878.31, 12.5469, 275.797, 50.0 },
    { 1575.14, -1807.91, 12.5167, 357.578, 60.0 },
    { 1453.84, -1745.1, 12.5469, 1.68272, 30.0 },
    { 1505.72, -1745.53, 12.5469, 1.65918, 40.0 },
    { 1458.2, -1041.44, 22.8281, 357.156, 60.0 },
    { 434.813, -1508.75, 30.0254, 285.841, 60.0 },
    { 1024.55, -1156.03, 22.8203, 0.048457, 80.0 },
    { 1813.03, -1681.3, 12.5469, 265.89, 60.0 },
    { 2419.03, -1264.07, 23.0695, 358.568, 60.0 },
    { 2305.6, -1667.88, 13.59, 358.474, 60.0 },
    { 1343.44, -1723.67, 12.5685, 174.382, 50.0 },
    { 1812.85, -1843.12, 12.5781, 271.446, 60.0 },
    { 1001.26, -977.377, 40.3967, 0.074019, 60.0 },
    { 2485.83, -1726.12, 12.5469, 175.785, 60.0 },
    { 1930.16, -1744.91, 12.5469, 180.125, 60.0 },
    { 2001.29, -1473.37, 12.5576, 356.463, 60.0 },
    { 770.815, -1328.38, 12.5469, 0.48899, 70.0 },
    { 771.4, -1387.3, 12.6482, 183.768, 90.0 }
};

stock Cam_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `cameras` (" \
        "`id` INT UNSIGNED NOT NULL PRIMARY KEY," \
        "`x` FLOAT NOT NULL, `y` FLOAT NOT NULL, `z` FLOAT NOT NULL, `a` FLOAT NOT NULL," \
        "`speed_limit` SMALLINT UNSIGNED NOT NULL" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
    // The flag keeps deleted starting cameras from coming back on the next restart.
    mysql_tquery(g_SQL, "SELECT `value` FROM `server_vars` WHERE `name` = 'cameras_seeded'", "OnCameraSeedChecked");
}

forward OnCameraSeedChecked();
public OnCameraSeedChecked()
{
    if (cache_num_rows() == 0)
    {
        new query[160];
        for (new i = 0; i < sizeof(g_CameraSeed); i++)
        {
            mysql_format(g_SQL, query, sizeof(query), "INSERT IGNORE INTO `cameras` (`id`, `x`, `y`, `z`, `a`, `speed_limit`) VALUES (%d, %f, %f, %f, %f, %d)",
                i + 1, g_CameraSeed[i][0], g_CameraSeed[i][1], g_CameraSeed[i][2], g_CameraSeed[i][3], floatround(g_CameraSeed[i][4]));
            mysql_tquery(g_SQL, query);
        }
        mysql_tquery(g_SQL, "INSERT INTO `server_vars` (`name`, `value`) VALUES ('cameras_seeded', 1)");
    }
    mysql_tquery(g_SQL, "SELECT * FROM `cameras` ORDER BY `id`", "OnCamerasLoaded");
    return 1;
}

static stock Cam_CreateVisuals(slot)
{
    g_Cameras[slot][camObject] = CreateDynamicObject(CAMERA_MODEL, g_Cameras[slot][camX], g_Cameras[slot][camY], g_Cameras[slot][camZ],
        0.0, 0.0, g_Cameras[slot][camA], 0, 0, -1, CAMERA_STREAM_DISTANCE, CAMERA_STREAM_DISTANCE);
    g_Cameras[slot][camArea] = CreateDynamicSphere(g_Cameras[slot][camX], g_Cameras[slot][camY], g_Cameras[slot][camZ], CAMERA_RADIUS, 0, 0);
    // The limit is shown like a road sign, so a fine never comes from a limit nobody could know.
    new text[48];
    format(text, sizeof(text), "Kamera\n" C_WHITE "Ogranicenje %d km/h", g_Cameras[slot][camLimit]);
    g_Cameras[slot][camLabel] = CreateDynamic3DTextLabel(text, COLOR_SERVER, g_Cameras[slot][camX], g_Cameras[slot][camY], g_Cameras[slot][camZ] + 3.5,
        CAMERA_LABEL_DISTANCE, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, 0, 0);
}

static stock Cam_Destroy(slot)
{
    DestroyDynamicObject(g_Cameras[slot][camObject]);
    DestroyDynamicArea(g_Cameras[slot][camArea]);
    DestroyDynamic3DTextLabel(g_Cameras[slot][camLabel]);
    g_Cameras[slot][camUsed] = false;
}

forward OnCamerasLoaded();
public OnCamerasLoaded()
{
    new rows = cache_num_rows();
    for (new row = 0; row < rows && row < MAX_CAMERAS; row++)
    {
        g_Cameras[row][camUsed] = true;
        cache_get_value_name_int(row, "id", g_Cameras[row][camDbId]);
        cache_get_value_name_float(row, "x", g_Cameras[row][camX]);
        cache_get_value_name_float(row, "y", g_Cameras[row][camY]);
        cache_get_value_name_float(row, "z", g_Cameras[row][camZ]);
        cache_get_value_name_float(row, "a", g_Cameras[row][camA]);
        cache_get_value_name_int(row, "speed_limit", g_Cameras[row][camLimit]);
        Cam_CreateVisuals(row);
    }
    printf("[CAMERAS] %d cameras loaded.", min(rows, MAX_CAMERAS));
    return 1;
}

static stock Cam_FindByArea(STREAMER_TAG_AREA:areaid)
{
    for (new i = 0; i < MAX_CAMERAS; i++)
    {
        if (g_Cameras[i][camUsed] && g_Cameras[i][camArea] == areaid)
        {
            return i;
        }
    }
    return -1;
}

static stock Cam_FindByDbId(dbid)
{
    for (new i = 0; i < MAX_CAMERAS; i++)
    {
        if (g_Cameras[i][camUsed] && g_Cameras[i][camDbId] == dbid)
        {
            return i;
        }
    }
    return -1;
}

// IDs of deleted cameras are given out again; called before the new slot is marked used.
static stock Cam_FreeDbId()
{
    new dbid = 1;
    while (Cam_FindByDbId(dbid) != -1)
    {
        dbid++;
    }
    return dbid;
}

static stock Cam_SpeedFine(over)
{
    if (over <= 20)
    {
        return FINE_SPEED_LOW;
    }
    return over <= 40 ? FINE_SPEED_MID : FINE_SPEED_HIGH;
}

// Only personal vehicles are photographed for now; job and faction vehicles get their own rules when they exist.
stock Cam_OnPlayerEnterArea(playerid, STREAMER_TAG_AREA:areaid)
{
    if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
    {
        return;
    }
    new cam = Cam_FindByArea(areaid);
    if (cam == -1)
    {
        return;
    }
    new vehicleid = GetPlayerVehicleID(playerid), slot = g_VehSlotOf[vehicleid];
    if (slot == -1 || Fuel_TankSize(g_Veh[slot][vModel]) == 0)
    {
        return;
    }
    new now = gettime();
    if (g_Veh[slot][vCameraSlot] == cam && now - g_Veh[slot][vCameraAt] < CAMERA_COOLDOWN_S)
    {
        return;
    }

    new speed = floatround(Veh_SpeedKmh(vehicleid)), over = speed - g_Cameras[cam][camLimit];
    new bool:speeding = over > CAMERA_SPEED_TOLERANCE, bool:unregistered = !Veh_IsRegistered(slot);
    if (!speeding && !unregistered)
    {
        return;
    }
    g_Veh[slot][vCameraSlot] = cam;
    g_Veh[slot][vCameraAt] = now;
    PlayerPlaySound(playerid, CAMERA_FLASH_SOUND, 0.0, 0.0, 0.0);
    GameTextForPlayer(playerid, "~w~blic!", 1000, 6);

    new reason[64], total, ownerid = INVALID_PLAYER_ID, owner_dbid = g_Veh[slot][vOwnerDbId];
    if (speeding)
    {
        format(reason, sizeof(reason), "brzina %d km/h (ogranicenje %d), kamera %d", speed, g_Cameras[cam][camLimit], g_Cameras[cam][camDbId]);
        total += Cam_SpeedFine(over);
        ownerid = Fine_Add(owner_dbid, g_Veh[slot][vDbId], g_Veh[slot][vPlate], reason, Cam_SpeedFine(over));
    }
    if (unregistered)
    {
        format(reason, sizeof(reason), "neregistrovano vozilo, kamera %d", g_Cameras[cam][camDbId]);
        total += FINE_UNREGISTERED;
        ownerid = Fine_Add(owner_dbid, g_Veh[slot][vDbId], g_Veh[slot][vPlate], reason, FINE_UNREGISTERED);
    }

    if (ownerid != INVALID_PLAYER_ID)
    {
        Msg_Info(ownerid, "Kamera je snimila vase vozilo %s (%s%s%s). Kazna: $%d, placa se u opstini ili policiji (/kazne).", Veh_Name(slot), speeding ? ("prebrza voznja") : (""), speeding && unregistered ? (", ") : (""), unregistered ? ("nije registrovano") : (""), total);
    }
    if (ownerid != playerid)
    {
        Msg_Info(playerid, "Kamera vas je snimila. Kazna stize vlasniku vozila.");
    }
}

cmd:napravikameru(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }
    new limit;
    if (sscanf(params, "d", limit) || limit < 20 || limit > 200)
    {
        Msg_Usage(playerid, "/napravikameru [ogranicenje 20-200 km/h] (stanite pored puta, kamera snima vozila do 10 m)");
        return 1;
    }
    if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
    {
        Msg_Error(playerid, "Kamera mora biti napolju.");
        return 1;
    }
    new slot = -1;
    for (new i = 0; i < MAX_CAMERAS; i++)
    {
        if (!g_Cameras[i][camUsed])
        {
            slot = i;
            break;
        }
    }
    if (slot == -1)
    {
        Msg_Error(playerid, "Dostignut je najveci broj kamera (%d).", MAX_CAMERAS);
        return 1;
    }

    g_Cameras[slot][camDbId] = Cam_FreeDbId();
    g_Cameras[slot][camUsed] = true;
    g_Cameras[slot][camLimit] = limit;
    GetPlayerPos(playerid, g_Cameras[slot][camX], g_Cameras[slot][camY], g_Cameras[slot][camZ]);
    GetPlayerFacingAngle(playerid, g_Cameras[slot][camA]);
    // The player stands on the ground; the pole stands on it too.
    g_Cameras[slot][camZ] -= 1.0;
    Cam_CreateVisuals(slot);

    new query[160], details[48];
    mysql_format(g_SQL, query, sizeof(query), "INSERT INTO `cameras` (`id`, `x`, `y`, `z`, `a`, `speed_limit`) VALUES (%d, %f, %f, %f, %f, %d)",
        g_Cameras[slot][camDbId], g_Cameras[slot][camX], g_Cameras[slot][camY], g_Cameras[slot][camZ], g_Cameras[slot][camA], limit);
    mysql_tquery(g_SQL, query);
    format(details, sizeof(details), "kamera %d: %d km/h", g_Cameras[slot][camDbId], limit);
    Staff_Log(playerid, "camera_create", INVALID_PLAYER_ID, details);
    Msg_Admin(playerid, "Napravljena je %s.", details);
    return 1;
}

cmd:obrisikameru(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }
    new Float:x, Float:y, Float:z, slot = -1, Float:best = 5.0;
    GetPlayerPos(playerid, x, y, z);
    for (new i = 0; i < MAX_CAMERAS; i++)
    {
        if (!g_Cameras[i][camUsed])
        {
            continue;
        }
        new Float:distance = VectorSize(g_Cameras[i][camX] - x, g_Cameras[i][camY] - y, g_Cameras[i][camZ] - z);
        if (distance < best)
        {
            best = distance;
            slot = i;
        }
    }
    if (slot == -1)
    {
        Msg_Error(playerid, "U blizini (5 m) nema kamere.");
        return 1;
    }

    new query[64], details[48];
    mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `cameras` WHERE `id` = %d", g_Cameras[slot][camDbId]);
    mysql_tquery(g_SQL, query);
    format(details, sizeof(details), "kamera %d: %d km/h", g_Cameras[slot][camDbId], g_Cameras[slot][camLimit]);
    Staff_Log(playerid, "camera_delete", INVALID_PLAYER_ID, details);
    Msg_Admin(playerid, "Obrisana je %s.", details);
    Cam_Destroy(slot);
    return 1;
}

cmd:kamere(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }
    new body[4000], line[80], count;
    strcopy(body, "ID\tOgranicenje\tKvart\n");
    for (new i = 0; i < MAX_CAMERAS; i++)
    {
        if (!g_Cameras[i][camUsed])
        {
            continue;
        }
        new zone = Hud_FindZone(g_Cameras[i][camX], g_Cameras[i][camY]), place[32] = "van grada";
        if (zone != -1)
        {
            strcopy(place, g_Zones[zone][zName]);
        }
        format(line, sizeof(line), "%d\t%d km/h\t%s\n", g_Cameras[i][camDbId], g_Cameras[i][camLimit], place);
        strcat(body, line);
        count++;
    }
    if (count == 0)
    {
        Msg_Info(playerid, "Nema kamera. Nova: /napravikameru [ogranicenje].");
        return 1;
    }
    ShowPlayerDialog(playerid, DIALOG_ADMIN_INFO, DIALOG_STYLE_TABLIST_HEADERS, "Kamere (/gotopos do kamere, /obrisikameru pored nje)", body, "Zatvori", "");
    return 1;
}
