#define MAX_GPS_LOCATIONS       (100)
#define GPS_NAME_LENGTH         (48)
#define GPS_CHECKPOINT_SIZE     (5.0)
#define GPS_ARRIVE_RADIUS       (15.0)
#define GPS_MAP_ICON            (41)

new const g_GpsCategories[][] =
{
    "Javne sluzbe", "Banke i bankomati", "Prodavnice", "Posao", "Ostalo"
};

enum E_GPS
{
    bool:gUsed,
    gDbId,
    gCategory,
    gName[GPS_NAME_LENGTH],
    Float:gX,
    Float:gY,
    Float:gZ
}

new g_Gps[MAX_GPS_LOCATIONS][E_GPS];

enum E_GPS_PLAYER
{
    bool:gpActive,
    // Slot of a saved location, or -1 for any other spot (e.g. a vehicle).
    gpTarget,
    gpName[GPS_NAME_LENGTH],
    Float:gpX,
    Float:gpY,
    Float:gpZ,
    STREAMER_TAG_MAP_ICON:gpIcon,
    gpCategory,
    gpListed[MAX_GPS_LOCATIONS],
    gpListedCount
}

new g_GpsPlayer[MAX_PLAYERS][E_GPS_PLAYER];

static const g_GpsAddMenu[][E_MENU_ITEM] =
{
    { ADMIN_LEVEL_HEAD, "Javne sluzbe", "dodajgps 1", "Ime lokacije:" },
    { ADMIN_LEVEL_HEAD, "Banke i bankomati", "dodajgps 2", "Ime lokacije:" },
    { ADMIN_LEVEL_HEAD, "Prodavnice", "dodajgps 3", "Ime lokacije:" },
    { ADMIN_LEVEL_HEAD, "Posao", "dodajgps 4", "Ime lokacije:" },
    { ADMIN_LEVEL_HEAD, "Ostalo", "dodajgps 5", "Ime lokacije:" }
};

stock Gps_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `gps_locations` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`category` TINYINT UNSIGNED NOT NULL," \
        "`name` VARCHAR(48) NOT NULL," \
        "`x` FLOAT NOT NULL, `y` FLOAT NOT NULL, `z` FLOAT NOT NULL" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");

    mysql_tquery(g_SQL, "SELECT * FROM `gps_locations` ORDER BY `category`, `name`", "OnGpsLoaded");
}

forward OnGpsLoaded();
public OnGpsLoaded()
{
    new rows = cache_num_rows();
    for (new row = 0; row < rows && row < MAX_GPS_LOCATIONS; row++)
    {
        g_Gps[row][gUsed] = true;
        cache_get_value_name_int(row, "id", g_Gps[row][gDbId]);
        cache_get_value_name_int(row, "category", g_Gps[row][gCategory]);
        cache_get_value_name(row, "name", g_Gps[row][gName], GPS_NAME_LENGTH);
        cache_get_value_name_float(row, "x", g_Gps[row][gX]);
        cache_get_value_name_float(row, "y", g_Gps[row][gY]);
        cache_get_value_name_float(row, "z", g_Gps[row][gZ]);
    }
    printf("[GPS] %d locations loaded.", min(rows, MAX_GPS_LOCATIONS));
    return 1;
}

stock Gps_OnPlayerConnect(playerid)
{
    g_GpsPlayer[playerid][gpActive] = false;
    g_GpsPlayer[playerid][gpTarget] = -1;
    g_GpsPlayer[playerid][gpIcon] = STREAMER_TAG_MAP_ICON:INVALID_STREAMER_ID;
}

stock Gps_OnPlayerDisconnect(playerid)
{
    Gps_Stop(playerid);
}

// Fills dest with the locations of a category (-1 for all) and remembers which slot each line is,
// so the same list serves /gps and the admin teleport.
stock Gps_BuildList(playerid, category, dest[], size = sizeof(dest), bool:with_ids = false)
{
    new line[80], count;
    dest[0] = '\0';
    for (new i = 0; i < MAX_GPS_LOCATIONS; i++)
    {
        if (!g_Gps[i][gUsed] || (category != -1 && g_Gps[i][gCategory] != category))
        {
            continue;
        }
        if (with_ids) format(line, sizeof(line), "%s " C_GREY "(%d)\n", g_Gps[i][gName], g_Gps[i][gDbId]);
        else format(line, sizeof(line), "%s\n", g_Gps[i][gName]);
        strcat(dest, line, size);
        g_GpsPlayer[playerid][gpListed][count++] = i;
    }
    g_GpsPlayer[playerid][gpListedCount] = count;
    return count;
}

stock Gps_FindSlotByDbId(dbid)
{
    for (new i = 0; i < MAX_GPS_LOCATIONS; i++)
    {
        if (g_Gps[i][gUsed] && g_Gps[i][gDbId] == dbid)
        {
            return i;
        }
    }
    return -1;
}

// IDs of deleted locations are given out again, so the IDs admins type stay short. Called before the new slot is marked used:
// a reused slot still holds the deleted entry's ID, which would look taken.
stock Gps_FreeDbId()
{
    new dbid = 1;
    while (Gps_FindSlotByDbId(dbid) != -1)
    {
        dbid++;
    }
    return dbid;
}

stock Gps_GetListed(playerid, listitem)
{
    if (listitem < 0 || listitem >= g_GpsPlayer[playerid][gpListedCount])
    {
        return -1;
    }
    new slot = g_GpsPlayer[playerid][gpListed][listitem];
    return g_Gps[slot][gUsed] ? slot : -1;
}

stock Gps_Stop(playerid)
{
    Checkpoint_Clear(playerid, CP_OWNER_GPS);
    if (IsValidDynamicMapIcon(g_GpsPlayer[playerid][gpIcon]))
    {
        DestroyDynamicMapIcon(g_GpsPlayer[playerid][gpIcon]);
    }
    g_GpsPlayer[playerid][gpIcon] = STREAMER_TAG_MAP_ICON:INVALID_STREAMER_ID;
    g_GpsPlayer[playerid][gpTarget] = -1;
    g_GpsPlayer[playerid][gpActive] = false;
}

stock Gps_Start(playerid, slot)
{
    Gps_StartAt(playerid, slot, g_Gps[slot][gName], g_Gps[slot][gX], g_Gps[slot][gY], g_Gps[slot][gZ]);
}

stock Gps_StartAt(playerid, slot, const name[], Float:x, Float:y, Float:z)
{
    Gps_Stop(playerid);
    g_GpsPlayer[playerid][gpActive] = true;
    g_GpsPlayer[playerid][gpTarget] = slot;
    strcopy(g_GpsPlayer[playerid][gpName], name, GPS_NAME_LENGTH);
    g_GpsPlayer[playerid][gpX] = x;
    g_GpsPlayer[playerid][gpY] = y;
    g_GpsPlayer[playerid][gpZ] = z;

    if (Checkpoint_Set(playerid, CP_OWNER_GPS, x, y, z, GPS_CHECKPOINT_SIZE))
    {
        Msg_Info(playerid, "GPS: %s je oznacen crvenim checkpointom. Iskljucivanje: /gps", name);
        return;
    }

    // The checkpoint belongs to another system (e.g. a job), so the destination is only marked on the map.
    g_GpsPlayer[playerid][gpIcon] = CreateDynamicMapIcon(x, y, z, GPS_MAP_ICON, 0, -1, -1, playerid, 6000.0, MAPICON_GLOBAL);
    Msg_Info(playerid, "GPS: %s je oznacen na mapi (checkpoint trenutno koristi drugi sistem).", name);
}

stock Gps_OnArrive(playerid)
{
    if (g_GpsPlayer[playerid][gpActive])
    {
        Msg_Info(playerid, "GPS: stigli ste na odrediste (%s).", g_GpsPlayer[playerid][gpName]);
    }
    Gps_Stop(playerid);
}

// Only the map icon fallback needs a distance check; the checkpoint reports arrival by itself.
stock Gps_Tick(playerid)
{
    if (!g_GpsPlayer[playerid][gpActive] || !IsValidDynamicMapIcon(g_GpsPlayer[playerid][gpIcon]))
    {
        return;
    }
    if (IsPlayerInRangeOfPoint(playerid, GPS_ARRIVE_RADIUS, g_GpsPlayer[playerid][gpX], g_GpsPlayer[playerid][gpY], g_GpsPlayer[playerid][gpZ]))
    {
        Gps_OnArrive(playerid);
    }
}

stock Gps_ShowCategories(playerid)
{
    new list[256];
    if (g_GpsPlayer[playerid][gpActive])
    {
        strcat(list, C_RED "Iskljuci GPS\n");
    }
    for (new i = 0; i < sizeof(g_GpsCategories); i++)
    {
        strcat(list, g_GpsCategories[i]);
        strcat(list, "\n");
    }
    ShowPlayerDialog(playerid, DIALOG_GPS_CATEGORY, DIALOG_STYLE_LIST, SERVER_NAME " - GPS", list, "Izaberi", "Zatvori");
}

stock bool:Gps_OnDialogResponse(playerid, dialogid, response, listitem)
{
    switch (dialogid)
    {
        case DIALOG_GPS_CATEGORY:
        {
            if (!response)
            {
                return true;
            }
            if (g_GpsPlayer[playerid][gpActive])
            {
                if (listitem == 0)
                {
                    Gps_Stop(playerid);
                    Msg_Info(playerid, "GPS je iskljucen.");
                    return true;
                }
                listitem--;
            }
            if (listitem < 0 || listitem >= sizeof(g_GpsCategories))
            {
                return true;
            }

            new list[2048];
            if (Gps_BuildList(playerid, listitem, list, sizeof(list), g_Staff[playerid][sAdminDuty]) == 0)
            {
                Msg_Info(playerid, "U ovoj kategoriji jos nema lokacija.");
                Gps_ShowCategories(playerid);
                return true;
            }
            ShowPlayerDialog(playerid, DIALOG_GPS_LOCATION, DIALOG_STYLE_LIST, g_GpsCategories[listitem], list, "Oznaci", "Nazad");
        }
        case DIALOG_GPS_LOCATION:
        {
            if (!response)
            {
                Gps_ShowCategories(playerid);
                return true;
            }
            new slot = Gps_GetListed(playerid, listitem);
            if (slot != -1)
            {
                Gps_Start(playerid, slot);
            }
        }
        default:
        {
            return false;
        }
    }
    return true;
}

cmd:gps(playerid, params[])
{
    Gps_ShowCategories(playerid);
    return 1;
}

cmd:dodajgps(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }

    new category, name[GPS_NAME_LENGTH];
    if (sscanf(params, "ds[48]", category, name) || category < 1 || category > sizeof(g_GpsCategories))
    {
        if (isnull(params))
        {
            Menu_Show(playerid, "Nova GPS lokacija (stanite ispred ulaza)", g_GpsAddMenu, sizeof(g_GpsAddMenu));
            return 1;
        }
        Msg_Usage(playerid, "/dodajgps (meni) ili /dodajgps [kategorija] [ime] (1 javne sluzbe, 2 banke, 3 prodavnice, 4 posao, 5 ostalo)");
        return 1;
    }
    if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
    {
        Msg_Error(playerid, "GPS lokacija mora biti napolju (enterijer 0, svet 0), ispred ulaza.");
        return 1;
    }

    new slot = -1;
    for (new i = 0; i < MAX_GPS_LOCATIONS; i++)
    {
        if (!g_Gps[i][gUsed])
        {
            slot = i;
            break;
        }
    }
    if (slot == -1)
    {
        Msg_Error(playerid, "Dostignut je najveci broj GPS lokacija (%d).", MAX_GPS_LOCATIONS);
        return 1;
    }

    g_Gps[slot][gDbId] = Gps_FreeDbId();
    g_Gps[slot][gUsed] = true;
    g_Gps[slot][gCategory] = category - 1;
    strcopy(g_Gps[slot][gName], name, GPS_NAME_LENGTH);
    GetPlayerPos(playerid, g_Gps[slot][gX], g_Gps[slot][gY], g_Gps[slot][gZ]);

    new query[256], details[80];
    mysql_format(g_SQL, query, sizeof(query), "INSERT INTO `gps_locations` (`id`, `category`, `name`, `x`, `y`, `z`) VALUES (%d, %d, '%e', %f, %f, %f)",
        g_Gps[slot][gDbId], category - 1, name, g_Gps[slot][gX], g_Gps[slot][gY], g_Gps[slot][gZ]);
    mysql_tquery(g_SQL, query);

    format(details, sizeof(details), "gps %d: %s", g_Gps[slot][gDbId], g_Gps[slot][gName]);
    Staff_Log(playerid, "gps_create", INVALID_PLAYER_ID, details);
    Msg_Admin(playerid, "Dodata je GPS lokacija %d: %s (%s).", g_Gps[slot][gDbId], g_Gps[slot][gName], g_GpsCategories[g_Gps[slot][gCategory]]);
    return 1;
}

cmd:obrisigps(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }

    new dbid;
    if (sscanf(params, "d", dbid))
    {
        Msg_Usage(playerid, "/obrisigps [id] (ID se vidi u /gps dok ste na admin duznosti)");
        return 1;
    }
    new slot = Gps_FindSlotByDbId(dbid);
    if (slot == -1)
    {
        Msg_Error(playerid, "GPS lokacija sa tim ID-jem ne postoji.");
        return 1;
    }

    // Anyone heading there loses the marker, so nobody is left following a deleted spot.
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (g_GpsPlayer[i][gpTarget] == slot)
        {
            Gps_Stop(i);
        }
    }

    new query[64], details[80];
    mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `gps_locations` WHERE `id` = %d", dbid);
    mysql_tquery(g_SQL, query);
    format(details, sizeof(details), "gps %d: %s", dbid, g_Gps[slot][gName]);
    Staff_Log(playerid, "gps_delete", INVALID_PLAYER_ID, details);
    Msg_Admin(playerid, "Obrisana je %s.", details);
    g_Gps[slot][gUsed] = false;
    return 1;
}


static const g_GpsEditMenu[][E_MENU_ITEM] =
{
    { ADMIN_LEVEL_HEAD, "Ime", "izmenigps %s ime", "Novo ime lokacije:" },
    { ADMIN_LEVEL_HEAD, "Kategorija", "izmenigps %s kategorija", "1 javne sluzbe, 2 banke, 3 prodavnice, 4 posao, 5 ostalo:" },
    { ADMIN_LEVEL_HEAD, "Pomeri ovde (gde stojite)", "izmenigps %s ovde", "" },
    { ADMIN_LEVEL_HEAD, "Obrisi", "obrisigps %s", "" }
};

cmd:izmenigps(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }
    new dbid, option[16], value[GPS_NAME_LENGTH];
    if (sscanf(params, "dS()[16]S()[48]", dbid, option, value))
    {
        Msg_Usage(playerid, "/izmenigps [id] (meni) ili /izmenigps [id] [ime / kategorija / ovde] [vrednost] (ID se vidi u /gps na duznosti)");
        return 1;
    }
    new slot = Gps_FindSlotByDbId(dbid);
    if (slot == -1)
    {
        Msg_Error(playerid, "GPS lokacija sa tim ID-jem ne postoji.");
        return 1;
    }
    if (isnull(option))
    {
        new title[64], subject[8];
        format(title, sizeof(title), "GPS %d: %s", dbid, g_Gps[slot][gName]);
        valstr(subject, dbid);
        Menu_Show(playerid, title, g_GpsEditMenu, sizeof(g_GpsEditMenu), subject);
        return 1;
    }

    if (!strcmp(option, "ime", true) && value[0])
    {
        strcopy(g_Gps[slot][gName], value, GPS_NAME_LENGTH);
    }
    else if (!strcmp(option, "kategorija", true) && strval(value) >= 1 && strval(value) <= sizeof(g_GpsCategories))
    {
        g_Gps[slot][gCategory] = strval(value) - 1;
    }
    else if (!strcmp(option, "ovde", true))
    {
        if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
        {
            Msg_Error(playerid, "GPS lokacija mora biti napolju.");
            return 1;
        }
        GetPlayerPos(playerid, g_Gps[slot][gX], g_Gps[slot][gY], g_Gps[slot][gZ]);
    }
    else
    {
        Msg_Error(playerid, "Opcije: ime [ime], kategorija [1-5], ovde.");
        return 1;
    }

    new query[256], details[80];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `gps_locations` SET `category` = %d, `name` = '%e', `x` = %f, `y` = %f, `z` = %f WHERE `id` = %d",
        g_Gps[slot][gCategory], g_Gps[slot][gName], g_Gps[slot][gX], g_Gps[slot][gY], g_Gps[slot][gZ], dbid);
    mysql_tquery(g_SQL, query);
    format(details, sizeof(details), "gps %d: %s", dbid, option);
    Staff_Log(playerid, "gps_edit", INVALID_PLAYER_ID, details);
    Msg_Admin(playerid, "GPS lokacija %d je izmenjena (%s): %s.", dbid, option, g_Gps[slot][gName]);
    return 1;
}
