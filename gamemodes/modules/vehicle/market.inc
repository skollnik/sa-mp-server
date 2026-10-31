#define MAX_MARKET_LISTINGS     (40)
#define MARKET_BUY_RANGE        (4.0)
#define MARKET_LABEL_DISTANCE   (20.0)
// Bought vehicles appear here, so the spot has to be free and nobody parks on it.
#define MARKET_EXIT_RADIUS      (6.0)
#define MARKET_MIN_LEVEL        (2)
#define MARKET_MAX_PRICE        (10000000)

enum E_MARKET_LISTING
{
    bool:mlUsed,
    mlDbId,
    mlModel,
    mlPrice,
    mlStock,
    Float:mlX,
    Float:mlY,
    Float:mlZ,
    Float:mlA,
    mlVehicleId,
    STREAMER_TAG_3D_TEXT_LABEL:mlLabel
}

new g_MarketListings[MAX_MARKET_LISTINGS][E_MARKET_LISTING];
new bool:g_MarketOpen;
new bool:g_MarketHasExit;
new Float:g_MarketExit[4];
// Display vehicles stand still and locked; nobody may drive them away.
new bool:g_IsMarketVehicle[MAX_VEHICLES];
new g_MarketSelected[MAX_PLAYERS];

stock Market_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `market` (" \
        "`id` TINYINT UNSIGNED NOT NULL PRIMARY KEY," \
        "`is_open` TINYINT UNSIGNED NOT NULL DEFAULT 0," \
        "`exit_x` FLOAT NOT NULL DEFAULT 0, `exit_y` FLOAT NOT NULL DEFAULT 0, `exit_z` FLOAT NOT NULL DEFAULT 0, `exit_a` FLOAT NOT NULL DEFAULT 0," \
        "`has_exit` TINYINT UNSIGNED NOT NULL DEFAULT 0" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
    mysql_tquery(g_SQL, "INSERT IGNORE INTO `market` (`id`) VALUES (1)");
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `market_listings` (" \
        "`id` INT UNSIGNED NOT NULL PRIMARY KEY," \
        "`model` SMALLINT UNSIGNED NOT NULL, `price` INT NOT NULL, `stock` INT NOT NULL," \
        "`x` FLOAT NOT NULL, `y` FLOAT NOT NULL, `z` FLOAT NOT NULL, `a` FLOAT NOT NULL" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
    mysql_tquery(g_SQL, "SELECT * FROM `market` WHERE `id` = 1", "OnMarketLoaded");
    mysql_tquery(g_SQL, "SELECT * FROM `market_listings` ORDER BY `id`", "OnMarketListingsLoaded");
}

forward OnMarketLoaded();
public OnMarketLoaded()
{
    if (cache_num_rows() == 0)
    {
        return 1;
    }
    new value;
    cache_get_value_name_int(0, "is_open", value);
    g_MarketOpen = value != 0;
    cache_get_value_name_int(0, "has_exit", value);
    g_MarketHasExit = value != 0;
    cache_get_value_name_float(0, "exit_x", g_MarketExit[0]);
    cache_get_value_name_float(0, "exit_y", g_MarketExit[1]);
    cache_get_value_name_float(0, "exit_z", g_MarketExit[2]);
    cache_get_value_name_float(0, "exit_a", g_MarketExit[3]);
    return 1;
}

static stock Market_UpdateLabel(slot)
{
    new text[128];
    format(text, sizeof(text), "%s\n" C_WHITE "Cena: $%d\nKomada: %d\n" C_GREY "%s", Veh_ModelName(g_MarketListings[slot][mlModel]),
        g_MarketListings[slot][mlPrice], g_MarketListings[slot][mlStock], g_MarketOpen ? ("/kupivozilo") : ("pijaca je zatvorena"));
    UpdateDynamic3DTextLabelText(g_MarketListings[slot][mlLabel], COLOR_SERVER, text);
}

static stock Market_SpawnDisplayVehicle(slot)
{
    new vehicleid = CreateVehicle(g_MarketListings[slot][mlModel], g_MarketListings[slot][mlX], g_MarketListings[slot][mlY], g_MarketListings[slot][mlZ],
        g_MarketListings[slot][mlA], 1, 1, -1);
    g_MarketListings[slot][mlVehicleId] = vehicleid;
    if (vehicleid != INVALID_VEHICLE_ID)
    {
        g_IsMarketVehicle[vehicleid] = true;
        SetVehicleParamsEx(vehicleid, VEHICLE_PARAMS_OFF, VEHICLE_PARAMS_OFF, VEHICLE_PARAMS_OFF, VEHICLE_PARAMS_ON, VEHICLE_PARAMS_OFF, VEHICLE_PARAMS_OFF, VEHICLE_PARAMS_OFF);
    }
}

static stock Market_CreateDisplay(slot)
{
    Market_SpawnDisplayVehicle(slot);
    g_MarketListings[slot][mlLabel] = CreateDynamic3DTextLabel("_", COLOR_SERVER, g_MarketListings[slot][mlX], g_MarketListings[slot][mlY], g_MarketListings[slot][mlZ] + 1.5,
        MARKET_LABEL_DISTANCE, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, 0, 0);
    Market_UpdateLabel(slot);
}

static stock Market_RemoveListing(slot)
{
    new vehicleid = g_MarketListings[slot][mlVehicleId];
    if (vehicleid != INVALID_VEHICLE_ID)
    {
        DestroyVehicle(vehicleid);
        g_IsMarketVehicle[vehicleid] = false;
    }
    DestroyDynamic3DTextLabel(g_MarketListings[slot][mlLabel]);
    new query[64];
    mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `market_listings` WHERE `id` = %d", g_MarketListings[slot][mlDbId]);
    mysql_tquery(g_SQL, query);
    g_MarketListings[slot][mlUsed] = false;
}

forward OnMarketListingsLoaded();
public OnMarketListingsLoaded()
{
    new rows = cache_num_rows();
    for (new row = 0; row < rows && row < MAX_MARKET_LISTINGS; row++)
    {
        g_MarketListings[row][mlUsed] = true;
        cache_get_value_name_int(row, "id", g_MarketListings[row][mlDbId]);
        cache_get_value_name_int(row, "model", g_MarketListings[row][mlModel]);
        cache_get_value_name_int(row, "price", g_MarketListings[row][mlPrice]);
        cache_get_value_name_int(row, "stock", g_MarketListings[row][mlStock]);
        cache_get_value_name_float(row, "x", g_MarketListings[row][mlX]);
        cache_get_value_name_float(row, "y", g_MarketListings[row][mlY]);
        cache_get_value_name_float(row, "z", g_MarketListings[row][mlZ]);
        cache_get_value_name_float(row, "a", g_MarketListings[row][mlA]);
        Market_CreateDisplay(row);
    }
    return 1;
}

// A display vehicle that was blown up comes back new a moment later, so the market never shows a wreck.
stock Market_OnVehicleDeath(vehicleid)
{
    if (!g_IsMarketVehicle[vehicleid])
    {
        return;
    }
    for (new i = 0; i < MAX_MARKET_LISTINGS; i++)
    {
        if (g_MarketListings[i][mlUsed] && g_MarketListings[i][mlVehicleId] == vehicleid)
        {
            SetTimerEx("Market_RestoreDisplay", 5000, false, "dd", g_MarketListings[i][mlDbId], vehicleid);
            return;
        }
    }
}

forward Market_RestoreDisplay(dbid, vehicleid);
public Market_RestoreDisplay(dbid, vehicleid)
{
    new slot = Market_FindByDbId(dbid);
    if (slot == -1 || g_MarketListings[slot][mlVehicleId] != vehicleid)
    {
        return 1;
    }
    DestroyVehicle(vehicleid);
    g_IsMarketVehicle[vehicleid] = false;
    Market_SpawnDisplayVehicle(slot);
    return 1;
}

stock bool:Market_IsNearExit(vehicleid)
{
    return g_MarketHasExit && GetVehicleDistanceFromPoint(vehicleid, g_MarketExit[0], g_MarketExit[1], g_MarketExit[2]) < MARKET_EXIT_RADIUS * 3.0;
}

// Display vehicles are locked, but a driver seat is still reachable by desync, so anyone at the wheel is put out.
stock Market_OnPlayerStateChange(playerid, PLAYER_STATE:newstate)
{
    if (newstate == PLAYER_STATE_DRIVER && g_IsMarketVehicle[GetPlayerVehicleID(playerid)])
    {
        RemovePlayerFromVehicle(playerid);
        Msg_Info(playerid, "Ovo vozilo je na prodaju. Kupuje se sa /kupivozilo dok stojite pored njega.");
    }
}

static stock Market_SaveState()
{
    new query[256];
    mysql_format(g_SQL, query, sizeof(query),
        "UPDATE `market` SET `is_open` = %d, `has_exit` = %d, `exit_x` = %f, `exit_y` = %f, `exit_z` = %f, `exit_a` = %f WHERE `id` = 1",
        g_MarketOpen ? 1 : 0, g_MarketHasExit ? 1 : 0, g_MarketExit[0], g_MarketExit[1], g_MarketExit[2], g_MarketExit[3]);
    mysql_tquery(g_SQL, query);
}

static stock Market_RefreshLabels()
{
    for (new i = 0; i < MAX_MARKET_LISTINGS; i++)
    {
        if (g_MarketListings[i][mlUsed])
        {
            Market_UpdateLabel(i);
        }
    }
}

static stock Market_FindNearest(playerid, Float:range)
{
    new best = -1, Float:best_distance = range, Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    for (new i = 0; i < MAX_MARKET_LISTINGS; i++)
    {
        if (!g_MarketListings[i][mlUsed] || g_MarketListings[i][mlVehicleId] == INVALID_VEHICLE_ID)
        {
            continue;
        }
        new Float:distance = GetVehicleDistanceFromPoint(g_MarketListings[i][mlVehicleId], x, y, z);
        if (distance < best_distance)
        {
            best_distance = distance;
            best = i;
        }
    }
    return best;
}

static stock Market_FindByDbId(dbid)
{
    for (new i = 0; i < MAX_MARKET_LISTINGS; i++)
    {
        if (g_MarketListings[i][mlUsed] && g_MarketListings[i][mlDbId] == dbid)
        {
            return i;
        }
    }
    return -1;
}

static stock Market_FreeDbId()
{
    new dbid = 1;
    while (Market_FindByDbId(dbid) != -1)
    {
        dbid++;
    }
    return dbid;
}

static stock Market_Open(playerid)
{
    if (!g_MarketHasExit)
    {
        Msg_Error(playerid, "Prvo postavite izlaz pijace (mesto gde se pojavljuju kupljena vozila).");
        return;
    }
    g_MarketOpen = true;
    Market_SaveState();
    Market_RefreshLabels();
    Chat_SendToAll(COLOR_SERVER, "[AUTO PIJACA] Auto pijaca je otvorena! Vozila se kupuju sa /kupivozilo, a pijaca se trazi sa /pijaca.");
    Staff_Log(playerid, "market_open", INVALID_PLAYER_ID, "");
}

// Closing ends the event: what was not sold is taken off the market.
static stock Market_Close(playerid)
{
    new count;
    for (new i = 0; i < MAX_MARKET_LISTINGS; i++)
    {
        if (g_MarketListings[i][mlUsed])
        {
            Market_RemoveListing(i);
            count++;
        }
    }
    g_MarketOpen = false;
    Market_SaveState();
    Chat_SendToAll(COLOR_SERVER, "[AUTO PIJACA] Auto pijaca je zatvorena.");
    new details[32];
    format(details, sizeof(details), "%d neprodatih ponuda", count);
    Staff_Log(playerid, "market_close", INVALID_PLAYER_ID, details);
}

static stock Market_AddListing(playerid, model, price, pieces)
{
    new slot = -1;
    for (new i = 0; i < MAX_MARKET_LISTINGS; i++)
    {
        if (!g_MarketListings[i][mlUsed])
        {
            slot = i;
            break;
        }
    }
    if (slot == -1)
    {
        Msg_Error(playerid, "Dostignut je najveci broj vozila na pijaci (%d).", MAX_MARKET_LISTINGS);
        return;
    }
    new Float:x, Float:y, Float:z, Float:angle;
    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, angle);
    g_MarketListings[slot][mlDbId] = Market_FreeDbId();
    g_MarketListings[slot][mlUsed] = true;
    g_MarketListings[slot][mlModel] = model;
    g_MarketListings[slot][mlPrice] = price;
    g_MarketListings[slot][mlStock] = pieces;
    g_MarketListings[slot][mlX] = x;
    g_MarketListings[slot][mlY] = y;
    g_MarketListings[slot][mlZ] = z;
    g_MarketListings[slot][mlA] = angle;
    Market_CreateDisplay(slot);
    // The admin stood where the vehicle now is; step aside so they are not stuck in it.
    Staff_TeleportTo(playerid, x + 3.0, y, z, 0, 0);

    new query[192], details[64];
    mysql_format(g_SQL, query, sizeof(query), "INSERT INTO `market_listings` (`id`, `model`, `price`, `stock`, `x`, `y`, `z`, `a`) VALUES (%d, %d, %d, %d, %f, %f, %f, %f)",
        g_MarketListings[slot][mlDbId], model, price, pieces, x, y, z, angle);
    mysql_tquery(g_SQL, query);
    format(details, sizeof(details), "%s $%d x%d", Veh_ModelName(model), price, pieces);
    Staff_Log(playerid, "market_add", INVALID_PLAYER_ID, details);
    Msg_Admin(playerid, "Na pijaci je %s.", details);
}

static stock Market_ShowAdmin(playerid)
{
    new body[256];
    format(body, sizeof(body), "%s\nDodaj vozilo ovde (model, cena, komada)\nObrisi najblize vozilo\nPostavi izlaz ovde (u vozilu ili peske)\nGPS do izlaza",
        g_MarketOpen ? ("Zatvori pijacu (neprodato se uklanja)") : ("Otvori pijacu"));
    ShowPlayerDialog(playerid, DIALOG_MARKET_ADMIN, DIALOG_STYLE_LIST, g_MarketOpen ? ("Auto pijaca (otvorena)") : ("Auto pijaca (zatvorena)"), body, "Izaberi", "Zatvori");
}

cmd:pijaca(playerid, params[])
{
    if (Staff_IsAdminOnDuty(playerid) && g_Player[playerid][pAdminLevel] >= ADMIN_LEVEL_HEAD)
    {
        Market_ShowAdmin(playerid);
        return 1;
    }
    if (!g_MarketOpen || !g_MarketHasExit)
    {
        Msg_Info(playerid, "Auto pijaca je zatvorena. Otvara je admin povremeno, a svi dobijaju obavestenje.");
        return 1;
    }
    Gps_StartAt(playerid, -1, "Auto pijaca", g_MarketExit[0], g_MarketExit[1], g_MarketExit[2]);
    Msg_Info(playerid, "Auto pijaca je otvorena i oznacena je na GPS-u. Vozila se kupuju sa /kupivozilo.");
    return 1;
}

cmd:kupivozilo(playerid, params[])
{
    if (!g_MarketOpen)
    {
        Msg_Error(playerid, "Auto pijaca je zatvorena.");
        return 1;
    }
    new slot = Market_FindNearest(playerid, MARKET_BUY_RANGE);
    if (slot == -1)
    {
        Msg_Error(playerid, "Stanite pored vozila na auto pijaci.");
        return 1;
    }
    if (g_Player[playerid][pLevel] < MARKET_MIN_LEVEL)
    {
        Msg_Error(playerid, "Vozilo se kupuje od nivoa %d.", MARKET_MIN_LEVEL);
        return 1;
    }
    if (!Veh_HasFreeSlot(playerid))
    {
        Msg_Error(playerid, "Imate najvise vozila (%d). Prodajte jedno (/ponudi) pre kupovine.", Veh_MaxFor(playerid));
        return 1;
    }
    new body[256];
    format(body, sizeof(body), "Plati gotovinom ($%d)\nPlati sa racuna ($%d)", g_MarketListings[slot][mlPrice], g_MarketListings[slot][mlPrice]);
    g_MarketSelected[playerid] = g_MarketListings[slot][mlDbId];
    new title[64];
    format(title, sizeof(title), "%s - $%d (pola rezervoara, bez tablica)", Veh_ModelName(g_MarketListings[slot][mlModel]), g_MarketListings[slot][mlPrice]);
    ShowPlayerDialog(playerid, DIALOG_MARKET_BUY, DIALOG_STYLE_LIST, title, body, "Kupi", "Odustani");
    return 1;
}
alias:kupivozilo("buycar")

// Whatever happened since the dialog opened (sold out, the player walked away) is checked again before paying.
static stock Market_Buy(playerid, bool:from_bank)
{
    new slot = Market_FindByDbId(g_MarketSelected[playerid]);
    if (!g_MarketOpen || slot == -1 || g_MarketListings[slot][mlStock] < 1 || Market_FindNearest(playerid, MARKET_BUY_RANGE) != slot)
    {
        Msg_Error(playerid, "To vozilo vise nije dostupno ili niste pored njega.");
        return;
    }
    if (!Veh_HasFreeSlot(playerid) || g_Player[playerid][pLevel] < MARKET_MIN_LEVEL)
    {
        return;
    }
    for (new v = 1; v < MAX_VEHICLES; v++)
    {
        if (IsValidVehicle(v) && GetVehicleDistanceFromPoint(v, g_MarketExit[0], g_MarketExit[1], g_MarketExit[2]) < MARKET_EXIT_RADIUS)
        {
            Msg_Error(playerid, "Izlaz sa pijace je zauzet drugim vozilom. Sacekajte da se skloni, pa pokusajte ponovo.");
            return;
        }
    }

    new price = g_MarketListings[slot][mlPrice], model = g_MarketListings[slot][mlModel], reason[48];
    format(reason, sizeof(reason), "auto pijaca: %s", Veh_ModelName(model));
    // Cash or the account, never both: the whole price comes from one place.
    if (from_bank)
    {
        if (!Money_RequireAccount(playerid))
        {
            return;
        }
        if (g_Player[playerid][pBank] < price)
        {
            Msg_Error(playerid, "Na racunu nemate dovoljno (potrebno je $%d, imate $%d).", price, g_Player[playerid][pBank]);
            return;
        }
        Money_GiveBank(playerid, -price, reason);
    }
    else if (!Money_Take(playerid, price, reason))
    {
        return;
    }

    g_MarketListings[slot][mlStock]--;
    if (g_MarketListings[slot][mlStock] < 1)
    {
        Market_RemoveListing(slot);
    }
    else
    {
        new query[96];
        mysql_format(g_SQL, query, sizeof(query), "UPDATE `market_listings` SET `stock` = %d WHERE `id` = %d", g_MarketListings[slot][mlStock], g_MarketListings[slot][mlDbId]);
        mysql_tquery(g_SQL, query);
        Market_UpdateLabel(slot);
    }

    Veh_Insert(INVALID_PLAYER_ID, playerid, model, g_MarketExit[0], g_MarketExit[1], g_MarketExit[2], g_MarketExit[3], float(Fuel_TankSize(model)) / 2.0, false);
    Account_Save(playerid);
    Gps_StartAt(playerid, -1, "Vase novo vozilo", g_MarketExit[0], g_MarketExit[1], g_MarketExit[2]);
    Msg_Success(playerid, "Kupili ste %s za $%d. Ceka vas na izlazu sa pijace (GPS). Registracija: /registruj, tablice: /tablice.", Veh_ModelName(model), price);
}

stock bool:Market_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
    switch (dialogid)
    {
        case DIALOG_MARKET_BUY:
        {
            if (response && (listitem == 0 || listitem == 1))
            {
                Market_Buy(playerid, listitem == 1);
            }
        }
        case DIALOG_MARKET_ADMIN:
        {
            if (!response || !Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
            {
                return true;
            }
            switch (listitem)
            {
                case 0:
                {
                    if (g_MarketOpen)
                    {
                        Market_Close(playerid);
                    }
                    else
                    {
                        Market_Open(playerid);
                    }
                }
                case 1:
                {
                    ShowPlayerDialog(playerid, DIALOG_MARKET_ADD, DIALOG_STYLE_INPUT, "Novo vozilo na pijaci",
                        "Stanite tamo gde vozilo treba da stoji i upisite: model cena komada\nNpr: 560 350000 2", "Dodaj", "Nazad");
                }
                case 2:
                {
                    new slot = Market_FindNearest(playerid, MARKET_BUY_RANGE);
                    if (slot == -1)
                    {
                        Msg_Error(playerid, "U blizini nema vozila sa pijace.");
                        return true;
                    }
                    new details[48];
                    format(details, sizeof(details), "%s $%d", Veh_ModelName(g_MarketListings[slot][mlModel]), g_MarketListings[slot][mlPrice]);
                    Staff_Log(playerid, "market_remove", INVALID_PLAYER_ID, details);
                    Market_RemoveListing(slot);
                    Msg_Admin(playerid, "Sa pijace je uklonjeno: %s.", details);
                }
                case 3:
                {
                    if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
                    {
                        Msg_Error(playerid, "Izlaz mora biti napolju.");
                        return true;
                    }
                    GetPlayerPos(playerid, g_MarketExit[0], g_MarketExit[1], g_MarketExit[2]);
                    GetPlayerFacingAngle(playerid, g_MarketExit[3]);
                    if (IsPlayerInAnyVehicle(playerid))
                    {
                        GetVehicleZAngle(GetPlayerVehicleID(playerid), g_MarketExit[3]);
                    }
                    g_MarketHasExit = true;
                    Market_SaveState();
                    Staff_Log(playerid, "market_exit", INVALID_PLAYER_ID, "");
                    Msg_Admin(playerid, "Izlaz pijace je postavljen ovde. Ovde se pojavljuju kupljena vozila.");
                }
                case 4:
                {
                    if (g_MarketHasExit)
                    {
                        Gps_StartAt(playerid, -1, "Izlaz pijace", g_MarketExit[0], g_MarketExit[1], g_MarketExit[2]);
                    }
                }
            }
        }
        case DIALOG_MARKET_ADD:
        {
            if (!response)
            {
                Market_ShowAdmin(playerid);
                return true;
            }
            if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
            {
                return true;
            }
            new model, price, pieces;
            if (sscanf(inputtext, "ddd", model, price, pieces) || !Veh_IsOwnableModel(model) || price < 1 || price > MARKET_MAX_PRICE || pieces < 1 || pieces > 50)
            {
                Msg_Error(playerid, "Upisite model (400-611, bez prikolica i vozova), cenu ($1-$%d) i broj komada (1-50).", MARKET_MAX_PRICE);
                return true;
            }
            if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0 || IsPlayerInAnyVehicle(playerid))
            {
                Msg_Error(playerid, "Stanite peske napolju, tamo gde vozilo treba da stoji.");
                return true;
            }
            Market_AddListing(playerid, model, price, pieces);
        }
        default:
        {
            return false;
        }
    }
    return true;
}
