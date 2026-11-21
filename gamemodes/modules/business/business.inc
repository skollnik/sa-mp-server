new g_BizSelected[MAX_PLAYERS];
new g_BizCreatePrice[MAX_PLAYERS];
// The business whose type an admin is picking (DB id), or 0 for a new one.
new g_BizTypeFor[MAX_PLAYERS];
// The prices a shop menu showed (business slot and its price share), so a purchase never costs more than the menu said.
new g_BizShownSlot[MAX_PLAYERS];
new g_BizShownPercent[MAX_PLAYERS];
new g_BizMealListed[MAX_PLAYERS][BIZ_MEALS_LISTED];
// The club a player was asked to pay the entry fee for.
new g_BizEntryFor[MAX_PLAYERS];

enum E_BIZ_ROW
{
    BIZ_ROW_INFO,
    BIZ_ROW_LOCK,
    BIZ_ROW_GPS,
    BIZ_ROW_WITHDRAW,
    BIZ_ROW_DEPOSIT,
    BIZ_ROW_PERCENT,
    BIZ_ROW_ORDER,
    BIZ_ROW_NAME,
    BIZ_ROW_FEE,
    BIZ_ROW_SELL
}
new E_BIZ_ROW:g_BizMenuRows[MAX_PLAYERS][E_BIZ_ROW];
new g_BizMenuCount[MAX_PLAYERS];

stock Biz_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `businesses` (" \
        "`id` INT UNSIGNED NOT NULL PRIMARY KEY," \
        "`owner_id` INT UNSIGNED NOT NULL DEFAULT 0," \
        "`owner_name` VARCHAR(24) NOT NULL DEFAULT ''," \
        "`type` TINYINT UNSIGNED NOT NULL, `name` VARCHAR(32) NOT NULL DEFAULT '', `price` INT NOT NULL," \
        "`x` FLOAT NOT NULL, `y` FLOAT NOT NULL, `z` FLOAT NOT NULL, `a` FLOAT NOT NULL," \
        "`locked` TINYINT(1) NOT NULL DEFAULT 0, `till` INT NOT NULL DEFAULT 0, `stock` INT NOT NULL DEFAULT 0," \
        "`percent` INT NOT NULL DEFAULT 100, `fee` INT NOT NULL DEFAULT 0, `tax_debt` INT NOT NULL DEFAULT 0," \
        "INDEX `business_owner` (`owner_id`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `business_trades` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`business_id` INT UNSIGNED NOT NULL," \
        "`kind` VARCHAR(16) NOT NULL," \
        "`seller_id` INT UNSIGNED NOT NULL DEFAULT 0, `buyer_id` INT UNSIGNED NOT NULL DEFAULT 0," \
        "`amount` INT NOT NULL DEFAULT 0, `tax` INT NOT NULL DEFAULT 0," \
        "`created_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
    mysql_tquery(g_SQL, "SELECT * FROM `businesses`", "OnBusinessesLoaded");
    SetTimer("Biz_CheckInactive", BIZ_INACTIVE_CHECK_MS, true);
}

stock Biz_OnPlayerConnect(playerid)
{
    g_BizSelected[playerid] = -1;
    g_BizTypeFor[playerid] = 0;
    g_BizShownSlot[playerid] = -1;
    g_BizEntryFor[playerid] = -1;
}

forward OnBusinessesLoaded();
public OnBusinessesLoaded()
{
    new rows = cache_num_rows(), type, locked;
    for (new row = 0; row < rows && row < MAX_BUSINESSES; row++)
    {
        g_Biz[row][bUsed] = true;
        cache_get_value_name_int(row, "id", g_Biz[row][bDbId]);
        cache_get_value_name_int(row, "owner_id", g_Biz[row][bOwnerDbId]);
        cache_get_value_name(row, "owner_name", g_Biz[row][bOwnerName], MAX_PLAYER_NAME + 1);
        cache_get_value_name_int(row, "type", type);
        cache_get_value_name(row, "name", g_Biz[row][bName], BIZ_NAME_MAX + 1);
        cache_get_value_name_int(row, "price", g_Biz[row][bPrice]);
        cache_get_value_name_float(row, "x", g_Biz[row][bX]);
        cache_get_value_name_float(row, "y", g_Biz[row][bY]);
        cache_get_value_name_float(row, "z", g_Biz[row][bZ]);
        cache_get_value_name_float(row, "a", g_Biz[row][bA]);
        cache_get_value_name_int(row, "locked", locked);
        cache_get_value_name_int(row, "till", g_Biz[row][bTill]);
        cache_get_value_name_int(row, "stock", g_Biz[row][bStock]);
        cache_get_value_name_int(row, "percent", g_Biz[row][bPercent]);
        cache_get_value_name_int(row, "fee", g_Biz[row][bFee]);
        cache_get_value_name_int(row, "tax_debt", g_Biz[row][bTaxDebt]);
        g_Biz[row][bType] = (type >= 0 && type < _:E_BIZ_TYPE) ? (E_BIZ_TYPE:type) : BIZ_247;
        g_Biz[row][bLocked] = locked != 0;
        Biz_Refresh(row);
    }
    printf("[BUSINESSES] %d businesses loaded.", min(rows, MAX_BUSINESSES));
    Biz_CheckInactive();
    return 1;
}

forward Biz_FindSlotByDbId(dbid);
public Biz_FindSlotByDbId(dbid)
{
    for (new i = 0; i < MAX_BUSINESSES; i++)
    {
        if (g_Biz[i][bUsed] && g_Biz[i][bDbId] == dbid)
        {
            return i;
        }
    }
    return -1;
}

stock Biz_World(slot)
{
    return BIZ_WORLD_BASE + g_Biz[slot][bDbId];
}

stock bool:Biz_HasInterior(slot)
{
    return g_BizTypes[g_Biz[slot][bType]][btInterior] != -1;
}

static stock Biz_GameInterior(slot)
{
    return g_BizInteriors[g_BizTypes[g_Biz[slot][bType]][btInterior]][biInterior];
}

static stock Biz_ExitPos(slot, &Float:x, &Float:y, &Float:z, &Float:angle)
{
    new index = g_BizTypes[g_Biz[slot][bType]][btInterior];
    x = g_BizInteriors[index][biX];
    y = g_BizInteriors[index][biY];
    z = g_BizInteriors[index][biZ];
    angle = g_BizInteriors[index][biA];
}

stock Biz_Address(slot, dest[], size = sizeof(dest))
{
    new zone[ZONE_NAME_LENGTH];
    Hud_ZoneName(g_Biz[slot][bX], g_Biz[slot][bY], zone);
    format(dest, size, "%s %d", zone, g_Biz[slot][bDbId]);
}

stock Biz_MaxStock(slot)
{
    return g_BizTypes[g_Biz[slot][bType]][btMaxStock];
}

// The share of the state's price customers pay here; a business without an owner is the state's own shop.
static stock Biz_PercentOf(slot)
{
    return g_Biz[slot][bOwnerDbId] == 0 ? 100 : g_Biz[slot][bPercent];
}

// Rounded up, so a cheap item at a lower share never costs nothing.
static stock Biz_Price(slot, base)
{
    return max(1, (base * Biz_PercentOf(slot) + 99) / 100);
}

static stock Biz_Units(slot, base)
{
    new value = g_BizTypes[g_Biz[slot][bType]][btUnitValue];
    return max(1, (base + value - 1) / value);
}

static stock Biz_StockCost(slot, units)
{
    return (units * g_BizTypes[g_Biz[slot][bType]][btUnitValue] * BIZ_STOCK_COST_PERCENT + 99) / 100;
}

static stock Biz_Tax(slot)
{
    return max(1, g_Biz[slot][bPrice] / 10000 * BIZ_TAX_PER_10000);
}

static stock Biz_DestroyVisuals(slot)
{
    if (IsValidDynamicPickup(g_Biz[slot][bPickup])) DestroyDynamicPickup(g_Biz[slot][bPickup]);
    if (IsValidDynamic3DTextLabel(g_Biz[slot][bLabel])) DestroyDynamic3DTextLabel(g_Biz[slot][bLabel]);
    if (IsValidDynamicMapIcon(g_Biz[slot][bIcon])) DestroyDynamicMapIcon(g_Biz[slot][bIcon]);
    if (IsValidDynamicPickup(g_Biz[slot][bExitPickup])) DestroyDynamicPickup(g_Biz[slot][bExitPickup]);
    if (IsValidDynamic3DTextLabel(g_Biz[slot][bExitLabel])) DestroyDynamic3DTextLabel(g_Biz[slot][bExitLabel]);
    g_Biz[slot][bPickup] = STREAMER_TAG_PICKUP:INVALID_STREAMER_ID;
    g_Biz[slot][bLabel] = STREAMER_TAG_3D_TEXT_LABEL:INVALID_STREAMER_ID;
    g_Biz[slot][bIcon] = STREAMER_TAG_MAP_ICON:INVALID_STREAMER_ID;
    g_Biz[slot][bExitPickup] = STREAMER_TAG_PICKUP:INVALID_STREAMER_ID;
    g_Biz[slot][bExitLabel] = STREAMER_TAG_3D_TEXT_LABEL:INVALID_STREAMER_ID;
}

// The line under the owner: what a customer pays to get in or for a litre.
static stock Biz_PriceLine(slot, dest[], size = sizeof(dest))
{
    dest[0] = EOS;
    if (g_Biz[slot][bType] == BIZ_GAS)
    {
        format(dest, size, "Gorivo: $%.2f po litru\n", float(FUEL_PRICE_PER_LITRE * Biz_PercentOf(slot)) / 100.0);
    }
    else if (g_BizTypes[g_Biz[slot][bType]][btHasFee] && g_Biz[slot][bOwnerDbId] != 0 && g_Biz[slot][bFee] > 0)
    {
        format(dest, size, "Ulaz: $%d\n", g_Biz[slot][bFee]);
    }
}

stock Biz_Refresh(slot)
{
    Biz_DestroyVisuals(slot);
    new text[320], address[48], prices[40], use[48], bool:for_sale = g_Biz[slot][bOwnerDbId] == 0, E_BIZ_TYPE:type = g_Biz[slot][bType];
    Biz_Address(slot, address);
    Biz_PriceLine(slot, prices);
    if (type == BIZ_GAS)
    {
        strcopy(use, "/gorivo kod pumpe");
    }
    else
    {
        strcopy(use, "F ili /udji   /kupi unutra");
    }
    if (for_sale)
    {
        format(text, sizeof(text), "Firma na prodaju: %s\n" C_WHITE "Adresa: %s\nCena: $%d | Nivo: %d\n%s" C_GREY "/kupifirmu   %s",
            g_BizTypes[type][btName], address, g_Biz[slot][bPrice], BIZ_MIN_LEVEL, prices, use);
    }
    else
    {
        format(text, sizeof(text), "%s\n" C_WHITE "%s | Vlasnik: %s\n%s" C_GREY "%s%s", g_Biz[slot][bName], g_BizTypes[type][btName],
            g_Biz[slot][bOwnerName], prices, use, g_Biz[slot][bLocked] ? (" " C_RED "(zatvoreno)") : (""));
    }
    new Float:x = g_Biz[slot][bX], Float:y = g_Biz[slot][bY], Float:z = g_Biz[slot][bZ];
    g_Biz[slot][bPickup] = CreateDynamicPickup(for_sale ? BIZ_PICKUP_SALE : BIZ_PICKUP_OWNED, 1, x, y, z, 0, 0);
    g_Biz[slot][bLabel] = CreateDynamic3DTextLabel(text, for_sale ? COLOR_GREEN : COLOR_SERVER, x, y, z + 0.6, BIZ_LABEL_DISTANCE,
        INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, 0, 0);
    g_Biz[slot][bIcon] = CreateDynamicMapIcon(x, y, z, g_BizTypes[type][btIcon], 0, 0, 0, -1, BIZ_ICON_DISTANCE, MAPICON_LOCAL);
    if (!Biz_HasInterior(slot))
    {
        return;
    }
    new Float:ix, Float:iy, Float:iz, Float:ia, interior = Biz_GameInterior(slot);
    Biz_ExitPos(slot, ix, iy, iz, ia);
    g_Biz[slot][bExitPickup] = CreateDynamicPickup(BIZ_PICKUP_OWNED, 1, ix, iy, iz, Biz_World(slot), interior);
    g_Biz[slot][bExitLabel] = CreateDynamic3DTextLabel("Izlaz\n" C_GREY "F ili /izadji\nKupovina: /kupi", COLOR_WHITE, ix, iy, iz + 0.6, BIZ_LABEL_DISTANCE,
        INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, Biz_World(slot), interior);
}

stock Biz_Save(slot)
{
    new query[640];
    mysql_format(g_SQL, query, sizeof(query),
        "UPDATE `businesses` SET `owner_id` = %d, `owner_name` = '%e', `type` = %d, `name` = '%e', `price` = %d, `x` = %f, `y` = %f, `z` = %f, `a` = %f, " \
        "`locked` = %d, `till` = %d, `stock` = %d, `percent` = %d, `fee` = %d, `tax_debt` = %d WHERE `id` = %d",
        g_Biz[slot][bOwnerDbId], g_Biz[slot][bOwnerName], _:g_Biz[slot][bType], g_Biz[slot][bName], g_Biz[slot][bPrice],
        g_Biz[slot][bX], g_Biz[slot][bY], g_Biz[slot][bZ], g_Biz[slot][bA], g_Biz[slot][bLocked] ? 1 : 0, g_Biz[slot][bTill], g_Biz[slot][bStock],
        g_Biz[slot][bPercent], g_Biz[slot][bFee], g_Biz[slot][bTaxDebt], g_Biz[slot][bDbId]);
    mysql_tquery(g_SQL, query);
}

stock Biz_LogTrade(slot, const kind[], seller_dbid, buyer_dbid, amount, tax = 0)
{
    new query[256];
    mysql_format(g_SQL, query, sizeof(query),
        "INSERT INTO `business_trades` (`business_id`, `kind`, `seller_id`, `buyer_id`, `amount`, `tax`) VALUES (%d, '%e', %d, %d, %d, %d)",
        g_Biz[slot][bDbId], kind, seller_dbid, buyer_dbid, amount, tax);
    mysql_tquery(g_SQL, query);
}

// Every change of owner goes through here; the new owner starts with the business open at the state's prices.
static stock Biz_SetOwner(slot, account_dbid, const name[])
{
    g_Biz[slot][bOwnerDbId] = account_dbid;
    strcopy(g_Biz[slot][bOwnerName], name, MAX_PLAYER_NAME + 1);
    g_Biz[slot][bLocked] = false;
    g_Biz[slot][bPercent] = 100;
    g_Biz[slot][bFee] = 0;
    Biz_Refresh(slot);
    Biz_Save(slot);
}

// Back to the state: the till goes to the old owner's bank, the stock and the name go with the business.
static stock Biz_Release(slot)
{
    if (g_Biz[slot][bOwnerDbId] != 0 && g_Biz[slot][bTill] > 0)
    {
        new reason[48];
        format(reason, sizeof(reason), "kasa firme %d", g_Biz[slot][bDbId]);
        Money_PayAccount(g_Biz[slot][bOwnerDbId], g_Biz[slot][bTill], reason);
    }
    g_Biz[slot][bTill] = 0;
    g_Biz[slot][bStock] = 0;
    g_Biz[slot][bTaxDebt] = 0;
    strcopy(g_Biz[slot][bName], g_BizTypes[g_Biz[slot][bType]][btName], BIZ_NAME_MAX + 1);
    Biz_SetOwner(slot, 0, "");
}

stock Biz_CountOwned(account_dbid)
{
    new count;
    for (new i = 0; i < MAX_BUSINESSES; i++)
    {
        if (g_Biz[i][bUsed] && g_Biz[i][bOwnerDbId] == account_dbid)
        {
            count++;
        }
    }
    return count;
}

stock bool:Biz_HasFreeSlot(playerid)
{
    return Biz_CountOwned(g_Player[playerid][pDbId]) < BIZ_MAX_PER_PLAYER;
}

stock bool:Biz_IsOwner(playerid, slot)
{
    return slot != -1 && g_Biz[slot][bOwnerDbId] != 0 && g_Biz[slot][bOwnerDbId] == g_Player[playerid][pDbId];
}

static stock Biz_FindOutside(playerid, Float:radius = BIZ_USE_RADIUS)
{
    if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
    {
        return -1;
    }
    for (new i = 0; i < MAX_BUSINESSES; i++)
    {
        if (g_Biz[i][bUsed] && IsPlayerInRangeOfPoint(playerid, radius, g_Biz[i][bX], g_Biz[i][bY], g_Biz[i][bZ]))
        {
            return i;
        }
    }
    return -1;
}

stock Biz_FindInside(playerid)
{
    new world = GetPlayerVirtualWorld(playerid);
    if (world <= BIZ_WORLD_BASE)
    {
        return -1;
    }
    return Biz_FindSlotByDbId(world - BIZ_WORLD_BASE);
}

static stock Biz_FindAtExit(playerid)
{
    new slot = Biz_FindInside(playerid);
    if (slot == -1 || !Biz_HasInterior(slot))
    {
        return -1;
    }
    new Float:x, Float:y, Float:z, Float:angle;
    Biz_ExitPos(slot, x, y, z, angle);
    return IsPlayerInRangeOfPoint(playerid, BIZ_USE_RADIUS, x, y, z) ? slot : -1;
}

// In front of the entrance or anywhere inside: where the owner handles the till, the lock and the sale.
static stock Biz_FindHere(playerid)
{
    new slot = Biz_FindInside(playerid);
    return slot != -1 ? slot : Biz_FindOutside(playerid, BIZ_NEAR_RADIUS);
}

stock Biz_FindOwnedHere(playerid)
{
    new slot = Biz_FindHere(playerid);
    return Biz_IsOwner(playerid, slot) ? slot : -1;
}

stock Biz_FindOwned(playerid)
{
    for (new i = 0; i < MAX_BUSINESSES; i++)
    {
        if (Biz_IsOwner(playerid, i))
        {
            return i;
        }
    }
    return -1;
}

// The gas station whose pumps the player is at.
static stock Biz_FindStation(playerid)
{
    if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
    {
        return -1;
    }
    for (new i = 0; i < MAX_BUSINESSES; i++)
    {
        if (g_Biz[i][bUsed] && g_Biz[i][bType] == BIZ_GAS && IsPlayerInRangeOfPoint(playerid, BIZ_PUMP_RADIUS, g_Biz[i][bX], g_Biz[i][bY], g_Biz[i][bZ]))
        {
            return i;
        }
    }
    return -1;
}

static stock bool:Biz_RequireHere(playerid, slot)
{
    if (Biz_FindHere(playerid) == slot)
    {
        return true;
    }
    Msg_Error(playerid, "Morate biti kod firme (ispred ulaza ili unutra).");
    return false;
}

static stock Biz_MoveInside(playerid, slot)
{
    new Float:x, Float:y, Float:z, Float:angle;
    Biz_ExitPos(slot, x, y, z, angle);
    Utils_MoveInto(playerid, x, y, z, angle, Biz_GameInterior(slot), Biz_World(slot));
}

static stock Biz_PutOutside(playerid, slot)
{
    Utils_MoveInto(playerid, g_Biz[slot][bX], g_Biz[slot][bY], g_Biz[slot][bZ], g_Biz[slot][bA] + 180.0, 0, 0);
}

// The owner and admins on duty get in free and through a locked door.
static stock bool:Biz_IsInsider(playerid, slot)
{
    return Biz_IsOwner(playerid, slot) || g_Staff[playerid][sAdminDuty];
}

// A closed business sells nothing to anyone but its owner, also to those who were already inside.
static stock bool:Biz_CheckOpen(playerid, slot)
{
    if (g_Biz[slot][bOwnerDbId] != 0 && g_Biz[slot][bLocked] && !Biz_IsOwner(playerid, slot))
    {
        Msg_Error(playerid, "Firma je zatvorena.");
        return false;
    }
    return true;
}

static stock Biz_Enter(playerid, slot)
{
    if (g_Biz[slot][bOwnerDbId] != 0 && g_Biz[slot][bLocked] && !Biz_IsInsider(playerid, slot))
    {
        Msg_Error(playerid, "Firma je zatvorena.");
        return;
    }
    if (!Door_CanUse(playerid))
    {
        return;
    }
    if (g_BizTypes[g_Biz[slot][bType]][btHasFee] && g_Biz[slot][bOwnerDbId] != 0 && g_Biz[slot][bFee] > 0 && !Biz_IsInsider(playerid, slot))
    {
        new text[96];
        g_BizEntryFor[playerid] = slot;
        format(text, sizeof(text), "Ulaz u %s kosta $%d.", g_Biz[slot][bName], g_Biz[slot][bFee]);
        ShowPlayerDialog(playerid, DIALOG_BIZ_ENTRY, DIALOG_STYLE_MSGBOX, "Ulaz", text, "Plati", "Odustani");
        return;
    }
    g_DoorLastUse[playerid] = GetTickCount();
    Biz_MoveInside(playerid, slot);
}

static stock Biz_PayEntry(playerid)
{
    new slot = g_BizEntryFor[playerid];
    g_BizEntryFor[playerid] = -1;
    if (slot == -1 || Biz_FindOutside(playerid, BIZ_NEAR_RADIUS) != slot || !g_Biz[slot][bUsed] || !Biz_CheckOpen(playerid, slot))
    {
        return;
    }
    new fee = g_Biz[slot][bFee], reason[32];
    format(reason, sizeof(reason), "ulaz u firmu %d", g_Biz[slot][bDbId]);
    if (fee > 0 && !Money_Take(playerid, fee, reason))
    {
        return;
    }
    g_Biz[slot][bTill] += fee;
    Biz_Save(slot);
    g_DoorLastUse[playerid] = GetTickCount();
    Biz_MoveInside(playerid, slot);
}

// Called by the door code after the houses: F, /udji and /izadji work for businesses too. Returns true when one was used.
stock bool:Biz_TryUse(playerid, bool:enter, bool:leave)
{
    new slot;
    if (enter && (slot = Biz_FindOutside(playerid)) != -1 && Biz_HasInterior(slot))
    {
        Biz_Enter(playerid, slot);
        return true;
    }
    if (leave && (slot = Biz_FindAtExit(playerid)) != -1)
    {
        if (Door_CanUse(playerid))
        {
            g_DoorLastUse[playerid] = GetTickCount();
            Biz_PutOutside(playerid, slot);
        }
        return true;
    }
    return false;
}

// For a shop's own checks: inside a business of that type, or at the state's shop point outside any business.
stock bool:Biz_RequireShop(playerid, E_POINT_TYPE:point, E_BIZ_TYPE:type)
{
    new slot = Biz_FindInside(playerid);
    if (slot != -1 && g_Biz[slot][bType] == type)
    {
        return Biz_CheckOpen(playerid, slot);
    }
    return Point_Require(playerid, point);
}

// The same place as Biz_RequireShop, without saying anything.
stock bool:Biz_IsShopHere(playerid, E_POINT_TYPE:point, E_BIZ_TYPE:type)
{
    new slot = Biz_FindInside(playerid);
    return slot != -1 ? g_Biz[slot][bType] == type : Point_IsNear(playerid, point);
}

// The price shown in a shop menu; also remembers it, see Biz_Pay.
stock Biz_PriceFor(playerid, base)
{
    new slot = Biz_FindInside(playerid);
    g_BizShownSlot[playerid] = slot;
    if (slot == -1)
    {
        return base;
    }
    g_BizShownPercent[playerid] = Biz_PercentOf(slot);
    return Biz_Price(slot, base);
}

static stock Biz_NotifyLowStock(slot, before)
{
    new limit = Biz_MaxStock(slot) * BIZ_LOW_STOCK_PERCENT / 100, ownerid = Account_FindOnlineByDbId(g_Biz[slot][bOwnerDbId]);
    if (ownerid != INVALID_PLAYER_ID && before > limit && g_Biz[slot][bStock] <= limit)
    {
        Msg_Info(ownerid, "Firmi %s ponestaje robe (%d/%d). Naruci se u /firma.", g_Biz[slot][bName], g_Biz[slot][bStock], Biz_MaxStock(slot));
    }
}

// A sale in an owned business: the stock goes down and the money into the till. The state's businesses never run out.
static stock bool:Biz_Sell(playerid, slot, base, units, const reason[], &paid)
{
    if (g_Biz[slot][bOwnerDbId] == 0)
    {
        paid = base;
        return Money_Take(playerid, base, reason);
    }
    if (g_Biz[slot][bStock] < units)
    {
        Msg_Error(playerid, "Firma nema dovoljno robe za to.");
        Biz_NotifyLowStock(slot, cellmax);
        return false;
    }
    paid = Biz_Price(slot, base);
    if (!Money_Take(playerid, paid, reason))
    {
        return false;
    }
    new before = g_Biz[slot][bStock];
    g_Biz[slot][bTill] += paid;
    g_Biz[slot][bStock] -= units;
    Biz_NotifyLowStock(slot, before);
    Biz_Save(slot);
    return true;
}

// Pays for something from a shop menu at what the menu showed; outside a business it is the state's price.
stock bool:Biz_Pay(playerid, base, const reason[], &paid = 0)
{
    new slot = Biz_FindInside(playerid);
    if (slot == -1)
    {
        paid = base;
        return Money_Take(playerid, base, reason);
    }
    if (!Biz_CheckOpen(playerid, slot))
    {
        return false;
    }
    if (g_BizShownSlot[playerid] != slot || g_BizShownPercent[playerid] != Biz_PercentOf(slot))
    {
        Msg_Error(playerid, "Cene su se u medjuvremenu promenile; otvorite meni ponovo (/kupi).");
        return false;
    }
    return Biz_Sell(playerid, slot, base, Biz_Units(slot, base), reason, paid);
}

// Fuel at a gas station's pumps: one unit of stock per litre. Pumps that belong to no station are the state's.
stock bool:Biz_PayFuel(playerid, litres, const reason[], &paid = 0)
{
    new slot = Biz_FindStation(playerid), base = litres * FUEL_PRICE_PER_LITRE;
    if (slot == -1)
    {
        paid = base;
        return Money_Take(playerid, base, reason);
    }
    if (!Biz_CheckOpen(playerid, slot))
    {
        return false;
    }
    return Biz_Sell(playerid, slot, base, litres, reason, paid);
}

static stock Biz_ShowMeals(playerid, slot)
{
    new body[512], line[64], count, E_BIZ_TYPE:type = g_Biz[slot][bType];
    strcopy(body, "Ponuda\tCena\n");
    for (new i = 0; i < sizeof(g_BizMeals) && count < BIZ_MEALS_LISTED; i++)
    {
        if (g_BizMeals[i][bmType] != type)
        {
            continue;
        }
        format(line, sizeof(line), "%s\t$%d\n", g_BizMeals[i][bmName], Biz_PriceFor(playerid, g_BizMeals[i][bmPrice]));
        strcat(body, line);
        g_BizMealListed[playerid][count++] = i;
    }
    for (new i = count; i < BIZ_MEALS_LISTED; i++)
    {
        g_BizMealListed[playerid][i] = -1;
    }
    ShowPlayerDialog(playerid, DIALOG_BIZ_MEALS, DIALOG_STYLE_TABLIST_HEADERS, g_Biz[slot][bName], body, "Kupi", "Zatvori");
}

static stock Biz_BuyMeal(playerid, index)
{
    new slot = Biz_FindInside(playerid);
    if (slot == -1 || g_Biz[slot][bType] != g_BizMeals[index][bmType])
    {
        return;
    }
    if (g_BizMeals[index][bmHunger] >= 20 && Hunger_IsFull(playerid))
    {
        Msg_Error(playerid, "Siti ste (traka gladi %d/%d). Jesti mozete kad traka padne na %d.", g_Player[playerid][pHunger], HUNGER_MAX, HUNGER_FULL);
        return;
    }
    if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT || Inv_IsBusy(playerid))
    {
        Msg_Error(playerid, "Trenutno ne mozete jesti ni piti.");
        return;
    }
    new reason[48], paid;
    format(reason, sizeof(reason), "%s: %s", g_BizTypes[g_Biz[slot][bType]][btName], g_BizMeals[index][bmName]);
    if (!Biz_Pay(playerid, g_BizMeals[index][bmPrice], reason, paid))
    {
        return;
    }
    new action[64];
    if (g_BizMeals[index][bmHunger] >= 20)
    {
        new Float:health;
        GetPlayerHealth(playerid, health);
        SetPlayerHealth(playerid, Utils_FloatMin(health + g_BizMeals[index][bmHealth], 100.0));
        ApplyAnimation(playerid, "FOOD", "EAT_Burger", 4.1, false, false, false, false, 0, SYNC_ALL);
        format(action, sizeof(action), "jede (%s).", g_BizMeals[index][bmName]);
    }
    else
    {
        SetPlayerSpecialAction(playerid, g_BizMeals[index][bmDrunk] > 0 ? SPECIAL_ACTION_DRINK_BEER : SPECIAL_ACTION_DRINK_SPRUNK);
        format(action, sizeof(action), "pije (%s).", g_BizMeals[index][bmName]);
        Inv_ShowHandHint(playerid, "gutljaj", "bacate flasu");
    }
    if (g_BizMeals[index][bmHunger] > 0)
    {
        Hunger_Add(playerid, g_BizMeals[index][bmHunger]);
    }
    if (g_BizMeals[index][bmDrunk] > 0)
    {
        SetPlayerDrunkLevel(playerid, min(GetPlayerDrunkLevel(playerid) + g_BizMeals[index][bmDrunk], BIZ_DRUNK_MAX));
    }
    Chat_Action(playerid, action);
    Account_Save(playerid);
    Msg_Success(playerid, "Platili ste $%d: %s.", paid, g_BizMeals[index][bmName]);
}

// /kupi inside a business: what it sells. Returns false when the player is in no business with a menu.
stock bool:Biz_ShowShop(playerid)
{
    new slot = Biz_FindInside(playerid);
    if (slot == -1)
    {
        return false;
    }
    if (!Biz_CheckOpen(playerid, slot))
    {
        return true;
    }
    switch (g_Biz[slot][bType])
    {
        case BIZ_247: Shop_Show247(playerid);
        case BIZ_CLOTHES: Acc_ShowShop(playerid);
        case BIZ_RESTAURANT, BIZ_BAR, BIZ_CLUB: Biz_ShowMeals(playerid, slot);
        default: return false;
    }
    return true;
}

forward Biz_CheckInactive();
public Biz_CheckInactive()
{
    new query[256];
    mysql_format(g_SQL, query, sizeof(query),
        "SELECT b.`id` FROM `businesses` b JOIN `accounts` a ON a.`id` = b.`owner_id` WHERE b.`owner_id` <> 0 AND a.`last_login` < NOW() - INTERVAL %d DAY",
        BIZ_INACTIVE_DAYS);
    mysql_tquery(g_SQL, query, "OnBusinessesInactive");
    return 1;
}

forward OnBusinessesInactive();
public OnBusinessesInactive()
{
    new rows = cache_num_rows(), dbid;
    for (new row = 0; row < rows; row++)
    {
        cache_get_value_name_int(row, "id", dbid);
        new slot = Biz_FindSlotByDbId(dbid);
        if (slot == -1 || g_Biz[slot][bOwnerDbId] == 0)
        {
            continue;
        }
        new owner = g_Biz[slot][bOwnerDbId], refund = g_Biz[slot][bPrice] * BIZ_INACTIVE_PERCENT / 100;
        Money_PayAccount(owner, refund, "firma vracena zbog neaktivnosti");
        Biz_LogTrade(slot, "inactive", owner, 0, refund);
        printf("[BUSINESSES] Business %d taken back from inactive account %d (%d refunded to the bank).", dbid, owner, refund);
        Biz_Release(slot);
    }
    return 1;
}

// Unpaid tax is taken from what comes into the till, so a debt never outlives the next deposit.
static stock Biz_PayDebtFromTill(slot)
{
    new paid = min(g_Biz[slot][bTaxDebt], g_Biz[slot][bTill]);
    g_Biz[slot][bTaxDebt] -= paid;
    g_Biz[slot][bTill] -= paid;
    return paid;
}

// The state takes the business over a tax debt; what it pays back goes to the bank, less the debt.
static stock Biz_Seize(slot)
{
    new owner = g_Biz[slot][bOwnerDbId], refund = max(0, g_Biz[slot][bPrice] * BIZ_INACTIVE_PERCENT / 100 - g_Biz[slot][bTaxDebt]);
    new ownerid = Account_FindOnlineByDbId(owner);
    if (refund > 0)
    {
        Money_PayAccount(owner, refund, "firma oduzeta zbog duga za porez");
    }
    Biz_LogTrade(slot, "tax", owner, 0, refund);
    if (ownerid != INVALID_PLAYER_ID)
    {
        Msg_Info(ownerid, "Drzava vam je oduzela firmu %s zbog duga za porez; na racun ste dobili $%d.", g_Biz[slot][bName], refund);
    }
    Biz_Release(slot);
}

// On the owner's payday: from the till first, then the bank; whatever is left is a debt.
stock Biz_ChargeTax(playerid)
{
    for (new slot = 0; slot < MAX_BUSINESSES; slot++)
    {
        if (!Biz_IsOwner(playerid, slot))
        {
            continue;
        }
        new tax = Biz_Tax(slot), from_till = min(tax, g_Biz[slot][bTill]);
        g_Biz[slot][bTill] -= from_till;
        new from_bank = min(tax - from_till, max(0, g_Player[playerid][pBank]));
        if (from_bank > 0)
        {
            Money_GiveBank(playerid, -from_bank, "porez za firmu");
        }
        g_Biz[slot][bTaxDebt] += tax - from_till - from_bank;
        if (g_Biz[slot][bTaxDebt] > 0)
        {
            SendClientMessage(playerid, COLOR_WHITE, "Porez za firmu: $%d (kasa $%d, racun $%d). " C_RED "Dug: $%d" C_WHITE " (oduzima se preko $%d).",
                tax, from_till, from_bank, g_Biz[slot][bTaxDebt], g_Biz[slot][bPrice] * BIZ_DEBT_PERCENT / 100);
        }
        else
        {
            SendClientMessage(playerid, COLOR_WHITE, "Porez za firmu: $%d (kasa $%d, racun $%d).", tax, from_till, from_bank);
        }
        if (g_Biz[slot][bTaxDebt] > g_Biz[slot][bPrice] * BIZ_DEBT_PERCENT / 100)
        {
            Biz_Seize(slot);
            continue;
        }
        Biz_Save(slot);
    }
}

static stock Biz_Create(playerid, E_BIZ_TYPE:type)
{
    new slot = -1;
    for (new i = 0; i < MAX_BUSINESSES; i++)
    {
        if (!g_Biz[i][bUsed])
        {
            slot = i;
            break;
        }
    }
    if (slot == -1)
    {
        Msg_Error(playerid, "Dostignut je najveci broj firmi (%d).", MAX_BUSINESSES);
        return;
    }
    static const empty[E_BIZ];
    new dbid = Utils_FreeDbId("Biz_FindSlotByDbId");
    g_Biz[slot] = empty;
    g_Biz[slot][bUsed] = true;
    g_Biz[slot][bDbId] = dbid;
    g_Biz[slot][bType] = type;
    g_Biz[slot][bPrice] = g_BizCreatePrice[playerid];
    g_Biz[slot][bPercent] = 100;
    strcopy(g_Biz[slot][bName], g_BizTypes[type][btName], BIZ_NAME_MAX + 1);
    GetPlayerPos(playerid, g_Biz[slot][bX], g_Biz[slot][bY], g_Biz[slot][bZ]);
    GetPlayerFacingAngle(playerid, g_Biz[slot][bA]);
    new query[256];
    mysql_format(g_SQL, query, sizeof(query), "INSERT INTO `businesses` (`id`, `type`, `name`, `price`, `x`, `y`, `z`, `a`) VALUES (%d, %d, '%e', %d, %f, %f, %f, %f)",
        dbid, _:type, g_Biz[slot][bName], g_Biz[slot][bPrice], g_Biz[slot][bX], g_Biz[slot][bY], g_Biz[slot][bZ], g_Biz[slot][bA]);
    mysql_tquery(g_SQL, query);
    Biz_Refresh(slot);

    new details[64];
    format(details, sizeof(details), "firma %d: %s, $%d", dbid, g_BizTypes[type][btName], g_Biz[slot][bPrice]);
    Staff_Log(playerid, "biz_create", INVALID_PLAYER_ID, details);
    Msg_Admin(playerid, "Napravljena je %s. Izmene: /izmenifirmu %d", details, dbid);
    if (type == BIZ_GAS)
    {
        new pumps = Point_CountNear(POINT_FUEL, g_Biz[slot][bX], g_Biz[slot][bY], g_Biz[slot][bZ], BIZ_PUMP_RADIUS);
        Msg_Admin(playerid, "Pumpe ove benzinske su mesta 'pumpa' do %.0f m odavde; sada ih ima %d.", BIZ_PUMP_RADIUS, pumps);
    }
}

static stock Biz_ShowTypes(playerid, for_dbid)
{
    new body[256], line[48];
    g_BizTypeFor[playerid] = for_dbid;
    for (new E_BIZ_TYPE:type = BIZ_247; type < E_BIZ_TYPE; type++)
    {
        format(line, sizeof(line), "%s\n", g_BizTypes[type][btName]);
        strcat(body, line);
    }
    ShowPlayerDialog(playerid, DIALOG_BIZ_TYPE, DIALOG_STYLE_LIST, "Tip firme", body, "Izaberi", "Odustani");
}

static stock Biz_AddRow(playerid, &count, E_BIZ_ROW:row, body[], size, const text[])
{
    g_BizMenuRows[playerid][E_BIZ_ROW:count++] = row;
    strcat(body, text, size);
    strcat(body, "\n", size);
}

stock Biz_ShowMenu(playerid, slot)
{
    new body[512], line[64], count;
    g_BizSelected[playerid] = slot;
    Biz_AddRow(playerid, count, BIZ_ROW_INFO, body, sizeof(body), "Informacije");
    Biz_AddRow(playerid, count, BIZ_ROW_LOCK, body, sizeof(body), g_Biz[slot][bLocked] ? ("Otvori firmu") : ("Zatvori firmu (ne prodaje)"));
    Biz_AddRow(playerid, count, BIZ_ROW_GPS, body, sizeof(body), "Oznaci na GPS-u");
    format(line, sizeof(line), "Kasa: $%d (podigni)", g_Biz[slot][bTill]);
    Biz_AddRow(playerid, count, BIZ_ROW_WITHDRAW, body, sizeof(body), line);
    Biz_AddRow(playerid, count, BIZ_ROW_DEPOSIT, body, sizeof(body), "Ostavi novac u kasu");
    format(line, sizeof(line), "Cene: %d%% drzavne cene", g_Biz[slot][bPercent]);
    Biz_AddRow(playerid, count, BIZ_ROW_PERCENT, body, sizeof(body), line);
    format(line, sizeof(line), "Naruci robu (%d/%d)", g_Biz[slot][bStock], Biz_MaxStock(slot));
    Biz_AddRow(playerid, count, BIZ_ROW_ORDER, body, sizeof(body), line);
    Biz_AddRow(playerid, count, BIZ_ROW_NAME, body, sizeof(body), "Ime firme");
    if (g_BizTypes[g_Biz[slot][bType]][btHasFee])
    {
        format(line, sizeof(line), "Ulaznica: $%d", g_Biz[slot][bFee]);
        Biz_AddRow(playerid, count, BIZ_ROW_FEE, body, sizeof(body), line);
    }
    format(line, sizeof(line), "Prodaj drzavi ($%d)", g_Biz[slot][bPrice] * BIZ_SELL_PERCENT / 100);
    Biz_AddRow(playerid, count, BIZ_ROW_SELL, body, sizeof(body), line);
    g_BizMenuCount[playerid] = count;
    ShowPlayerDialog(playerid, DIALOG_BIZ_MENU, DIALOG_STYLE_LIST, g_Biz[slot][bName], body, "Izaberi", "Zatvori");
}

static stock Biz_ShowInfo(playerid, slot)
{
    new text[1024], address[48], prices[40];
    Biz_Address(slot, address);
    Biz_PriceLine(slot, prices);
    format(text, sizeof(text), "Ime: %s\nTip: %s\nAdresa: %s\nVrata: %s\n%s\nKasa: $%d\nRoba: %d/%d (jedan komad vredi $%d po drzavnoj ceni, nabavka $%.2f)\n" \
        "Cene: %d%% drzavne cene\n\nVrednost: $%d\nPorez po plati: $%d (iz kase, pa sa racuna)\nDug za porez: $%d\nProdaja drzavi: $%d (%d%%)\n\n",
        g_Biz[slot][bName], g_BizTypes[g_Biz[slot][bType]][btName], address, g_Biz[slot][bLocked] ? ("zatvorena") : ("otvorena"), prices,
        g_Biz[slot][bTill], g_Biz[slot][bStock], Biz_MaxStock(slot), g_BizTypes[g_Biz[slot][bType]][btUnitValue],
        float(g_BizTypes[g_Biz[slot][bType]][btUnitValue] * BIZ_STOCK_COST_PERCENT) / 100.0, g_Biz[slot][bPercent],
        g_Biz[slot][bPrice], Biz_Tax(slot), g_Biz[slot][bTaxDebt], g_Biz[slot][bPrice] * BIZ_SELL_PERCENT / 100, BIZ_SELL_PERCENT);
    new rules[320];
    format(rules, sizeof(rules), "Svaka prodaja trosi robu i ide u kasu; bez robe firma ne prodaje.\nDug za porez veci od %d%% vrednosti: drzava oduzima firmu.\n" \
        "Firma se vraca na prodaju ako ne udjete na server %d dana: dobijate %d%% i novac iz kase na racun.",
        BIZ_DEBT_PERCENT, BIZ_INACTIVE_DAYS, BIZ_INACTIVE_PERCENT);
    strcat(text, rules);
    ShowPlayerDialog(playerid, DIALOG_BIZ_INFO, DIALOG_STYLE_MSGBOX, "Informacije o firmi", text, "Nazad", "");
}

static stock Biz_ToggleLock(playerid, slot)
{
    g_Biz[slot][bLocked] = !g_Biz[slot][bLocked];
    Biz_Refresh(slot);
    Biz_Save(slot);
    Chat_Action(playerid, g_Biz[slot][bLocked] ? ("zatvara firmu.") : ("otvara firmu."));
}

static stock Biz_ShowAmountInput(playerid, dialogid, const title[], const error[] = "")
{
    new text[96], slot = g_BizSelected[playerid];
    format(text, sizeof(text), "U kasi firme: " C_GREEN "$%d" C_WHITE ". Upisite iznos:", g_Biz[slot][bTill]);
    Money_ShowAmountInput(playerid, dialogid, title, text, error);
}

static stock Biz_ShowOrder(playerid, slot)
{
    new text[256];
    format(text, sizeof(text), "Roba: %d/%d. Jedan komad kosta $%.2f i stize odmah; placa se iz kase ($%d).\n\nUpisite koliko komada narucujete (najvise %d):",
        g_Biz[slot][bStock], Biz_MaxStock(slot), float(g_BizTypes[g_Biz[slot][bType]][btUnitValue] * BIZ_STOCK_COST_PERCENT) / 100.0,
        g_Biz[slot][bTill], Biz_MaxStock(slot) - g_Biz[slot][bStock]);
    ShowPlayerDialog(playerid, DIALOG_BIZ_ORDER, DIALOG_STYLE_INPUT, "Narudzbina robe", text, "Naruci", "Nazad");
}

static stock Biz_Order(playerid, slot, units)
{
    new room = Biz_MaxStock(slot) - g_Biz[slot][bStock];
    if (room <= 0)
    {
        Msg_Error(playerid, "Magacin firme je pun (%d/%d).", g_Biz[slot][bStock], Biz_MaxStock(slot));
        return;
    }
    if (units < 1 || units > room)
    {
        Msg_Error(playerid, "Mozete naruciti od 1 do %d komada.", room);
        return;
    }
    new cost = Biz_StockCost(slot, units);
    if (g_Biz[slot][bTill] < cost)
    {
        Msg_Error(playerid, "U kasi nema dovoljno novca (potrebno $%d, u kasi $%d). Novac se ostavlja u kasu iz /firma.", cost, g_Biz[slot][bTill]);
        return;
    }
    g_Biz[slot][bTill] -= cost;
    g_Biz[slot][bStock] += units;
    Biz_Save(slot);
    Biz_LogTrade(slot, "stock", g_Player[playerid][pDbId], 0, cost);
    Msg_Success(playerid, "Narucili ste %d komada robe za $%d iz kase. Na stanju: %d/%d.", units, cost, g_Biz[slot][bStock], Biz_MaxStock(slot));
}

static stock bool:Biz_IsValidName(const name[])
{
    new length = strlen(name);
    if (length < BIZ_NAME_MIN || length > BIZ_NAME_MAX)
    {
        return false;
    }
    // Braces would colour the label and % or ~ could be read as formatting elsewhere; '{' and up are only symbols.
    for (new i = 0; i < length; i++)
    {
        if (name[i] < ' ' || name[i] >= '{' || name[i] == '%')
        {
            return false;
        }
    }
    return true;
}

// The business changes hands only with an empty till and no tax debt, like a house with an empty safe.
static stock bool:Biz_FindLeftover(slot, problem[], size)
{
    if (g_Biz[slot][bTill] > 0)
    {
        strcopy(problem, "Prvo podignite novac iz kase (/firma).", size);
    }
    else if (g_Biz[slot][bTaxDebt] > 0)
    {
        format(problem, size, "Firma ima dug za porez ($%d); ostavite novac u kasu i dug se odmah placa.", g_Biz[slot][bTaxDebt]);
    }
    else
    {
        return false;
    }
    return true;
}

static stock Biz_SellToState(playerid, slot)
{
    new problem[96];
    if (!Biz_IsOwner(playerid, slot) || !Biz_RequireHere(playerid, slot))
    {
        return;
    }
    if (Biz_FindLeftover(slot, problem, sizeof(problem)))
    {
        Msg_Error(playerid, "%s", problem);
        return;
    }
    new amount = g_Biz[slot][bPrice] * BIZ_SELL_PERCENT / 100, reason[48];
    format(reason, sizeof(reason), "prodaja firme %d drzavi", g_Biz[slot][bDbId]);
    Money_GiveCash(playerid, amount, reason);
    Biz_LogTrade(slot, "state", g_Player[playerid][pDbId], 0, amount);
    Biz_Release(slot);
    Account_Save(playerid);
    Msg_Success(playerid, "Prodali ste firmu drzavi za $%d.", amount);
}

// Called by /ponudi: the business the seller stands at, ready to change hands right now; otherwise problem says why.
stock bool:Biz_IsSaleReady(slot, dbid, seller, problem[], size)
{
    if (slot < 0 || slot >= MAX_BUSINESSES || !g_Biz[slot][bUsed] || g_Biz[slot][bDbId] != dbid || !Biz_IsOwner(seller, slot))
    {
        strcopy(problem, "Firma vise nije prodavceva.", size);
        return false;
    }
    if (Biz_FindHere(seller) != slot)
    {
        strcopy(problem, "Prodavac mora stajati kod firme (ispred ulaza ili unutra).", size);
        return false;
    }
    return !Biz_FindLeftover(slot, problem, size);
}

stock Biz_DbId(slot)
{
    return g_Biz[slot][bDbId];
}

stock Biz_SaleText(slot, dest[], size = sizeof(dest))
{
    new address[48];
    Biz_Address(slot, address);
    format(dest, size, "Firma: %s (%s)\nAdresa: %s\nRoba: %d/%d (ide uz firmu)", g_Biz[slot][bName], g_BizTypes[g_Biz[slot][bType]][btName], address,
        g_Biz[slot][bStock], Biz_MaxStock(slot));
}

stock Biz_TransferTo(slot, seller, buyer, amount, tax)
{
    Biz_LogTrade(slot, "sale", g_Player[seller][pDbId], g_Player[buyer][pDbId], amount, tax);
    Biz_SetOwner(slot, g_Player[buyer][pDbId], g_Player[buyer][pName]);
}

cmd:kupifirmu(playerid, params[])
{
    new slot = Biz_FindOutside(playerid, BIZ_NEAR_RADIUS);
    if (slot == -1)
    {
        Msg_Error(playerid, "Niste ispred firme.");
        return 1;
    }
    if (g_Biz[slot][bOwnerDbId] != 0)
    {
        Msg_Error(playerid, "Ova firma nije na prodaju.");
        return 1;
    }
    if (g_Player[playerid][pLevel] < BIZ_MIN_LEVEL)
    {
        Msg_Error(playerid, "Za firmu treba nivo %d.", BIZ_MIN_LEVEL);
        return 1;
    }
    if (!Biz_HasFreeSlot(playerid))
    {
        Msg_Error(playerid, "Vec imate firmu (najvise %d po igracu).", BIZ_MAX_PER_PLAYER);
        return 1;
    }
    new reason[32];
    format(reason, sizeof(reason), "kupovina firme %d", g_Biz[slot][bDbId]);
    if (!Money_Take(playerid, g_Biz[slot][bPrice], reason))
    {
        return 1;
    }
    Biz_LogTrade(slot, "buy", 0, g_Player[playerid][pDbId], g_Biz[slot][bPrice]);
    g_Biz[slot][bStock] = Biz_MaxStock(slot) * BIZ_START_STOCK_PERCENT / 100;
    Biz_SetOwner(slot, g_Player[playerid][pDbId], g_Player[playerid][pName]);
    Account_Save(playerid);
    Msg_Success(playerid, "Kupili ste firmu za $%d, sa %d komada robe. Meni firme: /firma (kasa, cene, roba, ime).", g_Biz[slot][bPrice], g_Biz[slot][bStock]);
    return 1;
}
alias:kupifirmu("buybiz")

cmd:firma(playerid, params[])
{
    new slot = Biz_FindOwnedHere(playerid);
    if (slot == -1)
    {
        slot = Biz_FindOwned(playerid);
    }
    if (slot == -1)
    {
        Msg_Error(playerid, "Nemate firmu. Firma na prodaju ima zeleni natpis ispred ulaza (/kupifirmu).");
        return 1;
    }
    Biz_ShowMenu(playerid, slot);
    return 1;
}
alias:firma("biz")

cmd:napravifirmu(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }
    new price;
    if (sscanf(params, "d", price))
    {
        Msg_Usage(playerid, "/napravifirmu [cena] (stanite ispred ulaza; benzinska: kod pumpi; tip se bira iz liste)");
        return 1;
    }
    if (price < BIZ_MIN_PRICE || price > BIZ_MAX_PRICE)
    {
        Msg_Error(playerid, "Cena firme je od $%d do $%d.", BIZ_MIN_PRICE, BIZ_MAX_PRICE);
        return 1;
    }
    if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
    {
        Msg_Error(playerid, "Firma se pravi napolju, ispred njenog ulaza.");
        return 1;
    }
    g_BizCreatePrice[playerid] = price;
    Biz_ShowTypes(playerid, 0);
    return 1;
}

static stock bool:Biz_ParseSlot(playerid, const params[], const usage[], &slot)
{
    new dbid;
    if (sscanf(params, "d", dbid))
    {
        Msg_Usage(playerid, "%s", usage);
        return false;
    }
    slot = Biz_FindSlotByDbId(dbid);
    if (slot == -1)
    {
        Msg_Error(playerid, "Firma sa tim ID-jem ne postoji. Spisak: /firme");
        return false;
    }
    return true;
}

static const g_BizEditMenu[][E_MENU_ITEM] =
{
    { ADMIN_LEVEL_HEAD, "Cena", "izmenifirmu %s cena", "Nova cena:" },
    { ADMIN_LEVEL_HEAD, "Tip (lista, samo bez vlasnika)", "izmenifirmu %s tip", "" },
    { ADMIN_LEVEL_HEAD, "Ime", "izmenifirmu %s ime", "Novo ime (3-32 znaka):" },
    { ADMIN_LEVEL_HEAD, "Roba (broj komada)", "izmenifirmu %s roba", "Komada robe:" },
    { ADMIN_LEVEL_HEAD, "Pomeri ulaz ovde (napolju, gde stojite)", "izmenifirmu %s ovde", "" },
    { ADMIN_LEVEL_HEAD, "Oduzmi vlasniku (vrati na prodaju)", "izmenifirmu %s oduzmi", "" },
    { ADMIN_LEVEL_HEAD, "Udji u firmu", "izmenifirmu %s udji", "" }
};

cmd:izmenifirmu(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }
    new dbid, option[16], value[BIZ_NAME_MAX + 1];
    if (sscanf(params, "dS()[16]S()[33]", dbid, option, value))
    {
        Msg_Usage(playerid, "/izmenifirmu [id] (otvara meni sa svim opcijama; spisak firmi: /firme)");
        return 1;
    }
    new slot = Biz_FindSlotByDbId(dbid);
    if (slot == -1)
    {
        Msg_Error(playerid, "Firma sa tim ID-jem ne postoji. Spisak: /firme");
        return 1;
    }
    if (isnull(option))
    {
        new title[64], subject[8];
        if (g_Biz[slot][bOwnerDbId] != 0)
        {
            format(title, sizeof(title), "Firma %d - vlasnik %s", dbid, g_Biz[slot][bOwnerName]);
        }
        else
        {
            format(title, sizeof(title), "Firma %d - na prodaju", dbid);
        }
        valstr(subject, dbid);
        Menu_Show(playerid, title, g_BizEditMenu, sizeof(g_BizEditMenu), subject);
        return 1;
    }
    new details[80], number = strval(value);
    if (!strcmp(option, "cena", true))
    {
        if (number < BIZ_MIN_PRICE || number > BIZ_MAX_PRICE)
        {
            Msg_Error(playerid, "Cena je od $%d do $%d.", BIZ_MIN_PRICE, BIZ_MAX_PRICE);
            return 1;
        }
        g_Biz[slot][bPrice] = number;
        format(details, sizeof(details), "firma %d: cena $%d", dbid, number);
    }
    else if (!strcmp(option, "tip", true))
    {
        if (g_Biz[slot][bOwnerDbId] != 0)
        {
            Msg_Error(playerid, "Tip se menja samo firmi bez vlasnika (prvo: oduzmi).");
            return 1;
        }
        Biz_ShowTypes(playerid, dbid);
        return 1;
    }
    else if (!strcmp(option, "ime", true))
    {
        if (!Biz_IsValidName(value))
        {
            Msg_Error(playerid, "Ime ima %d-%d znakova, bez { } %% i ~.", BIZ_NAME_MIN, BIZ_NAME_MAX);
            return 1;
        }
        strcopy(g_Biz[slot][bName], value, BIZ_NAME_MAX + 1);
        format(details, sizeof(details), "firma %d: ime %s", dbid, value);
    }
    else if (!strcmp(option, "roba", true))
    {
        if (isnull(value) || number < 0 || number > Biz_MaxStock(slot))
        {
            Msg_Error(playerid, "Roba je od 0 do %d komada.", Biz_MaxStock(slot));
            return 1;
        }
        g_Biz[slot][bStock] = number;
        format(details, sizeof(details), "firma %d: roba %d", dbid, number);
    }
    else if (!strcmp(option, "ovde", true))
    {
        if (GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
        {
            Msg_Error(playerid, "Ulaz firme mora biti napolju.");
            return 1;
        }
        GetPlayerPos(playerid, g_Biz[slot][bX], g_Biz[slot][bY], g_Biz[slot][bZ]);
        GetPlayerFacingAngle(playerid, g_Biz[slot][bA]);
        format(details, sizeof(details), "firma %d: ulaz pomeren", dbid);
    }
    else if (!strcmp(option, "oduzmi", true))
    {
        if (g_Biz[slot][bOwnerDbId] == 0)
        {
            Msg_Error(playerid, "Firma nema vlasnika.");
            return 1;
        }
        format(details, sizeof(details), "firma %d: oduzeta od %s", dbid, g_Biz[slot][bOwnerName]);
        new ownerid = Account_FindOnlineByDbId(g_Biz[slot][bOwnerDbId]);
        Biz_LogTrade(slot, "admin", g_Biz[slot][bOwnerDbId], 0, 0);
        Biz_Release(slot);
        if (ownerid != INVALID_PLAYER_ID)
        {
            Msg_Info(ownerid, "Admin %s vam je oduzeo firmu %d; vratila se na prodaju, a novac iz kase je na racunu.", Account_Name(playerid), dbid);
        }
    }
    else if (!strcmp(option, "udji", true))
    {
        if (Biz_HasInterior(slot))
        {
            Biz_MoveInside(playerid, slot);
        }
        else
        {
            Biz_PutOutside(playerid, slot);
        }
        return 1;
    }
    else
    {
        Msg_Error(playerid, "Nepoznata opcija. Sve opcije otvara /izmenifirmu %d", dbid);
        return 1;
    }
    Biz_Refresh(slot);
    Biz_Save(slot);
    Staff_Log(playerid, "biz_edit", INVALID_PLAYER_ID, details);
    Msg_Admin(playerid, "Izmenjena je %s.", details);
    return 1;
}

// The id is given out again, so nobody may stay in this business's world and turn up in the next one.
static stock Biz_EmptyInside(slot)
{
    new world = Biz_World(slot);
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (IsPlayerConnected(i) && GetPlayerVirtualWorld(i) == world)
        {
            Biz_PutOutside(i, slot);
        }
    }
}

cmd:obrisifirmu(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }
    new slot;
    if (!Biz_ParseSlot(playerid, params, "/obrisifirmu [id] (samo firma bez vlasnika)", slot))
    {
        return 1;
    }
    if (g_Biz[slot][bOwnerDbId] != 0)
    {
        Msg_Error(playerid, "Firma ima vlasnika (%s). Prvo je oduzmite: /izmenifirmu %d", g_Biz[slot][bOwnerName], g_Biz[slot][bDbId]);
        return 1;
    }
    Biz_EmptyInside(slot);
    new query[64], details[32];
    format(details, sizeof(details), "firma %d", g_Biz[slot][bDbId]);
    mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `businesses` WHERE `id` = %d", g_Biz[slot][bDbId]);
    mysql_tquery(g_SQL, query);
    Biz_DestroyVisuals(slot);
    g_Biz[slot][bUsed] = false;
    Staff_Log(playerid, "biz_delete", INVALID_PLAYER_ID, details);
    Msg_Admin(playerid, "Obrisana je %s.", details);
    return 1;
}

cmd:gotofirma(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }
    new slot;
    if (!Biz_ParseSlot(playerid, params, "/gotofirma [id] (spisak: /firme)", slot))
    {
        return 1;
    }
    Biz_PutOutside(playerid, slot);
    return 1;
}

// Like /kinfo: the whole state of the business the admin stands at, or of the one with that ID.
cmd:finfo(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }
    new slot;
    if (isnull(params))
    {
        slot = Biz_FindHere(playerid);
        if (slot == -1)
        {
            Msg_Error(playerid, "Niste kod firme (ispred ulaza ili unutra). Sa ID-jem: /finfo [id], spisak: /firme");
            return 1;
        }
    }
    else if (!Biz_ParseSlot(playerid, params, "/finfo [id firme] (bez ID-ja: firma kod koje stojite)", slot))
    {
        return 1;
    }
    new body[1024], value[96], address[48];
    Biz_Address(slot, address);
    Staff_InfoBegin(body, sizeof(body));
    valstr(value, g_Biz[slot][bDbId]);
    Staff_InfoRow(body, sizeof(body), "ID firme", value);
    Staff_InfoRow(body, sizeof(body), "Ime", g_Biz[slot][bName]);
    Staff_InfoRow(body, sizeof(body), "Tip", g_BizTypes[g_Biz[slot][bType]][btName]);
    Staff_InfoRow(body, sizeof(body), "Adresa", address);
    if (g_Biz[slot][bOwnerDbId] == 0)
    {
        strcopy(value, "nema (na prodaju)");
    }
    else
    {
        format(value, sizeof(value), "%s (%s)", g_Biz[slot][bOwnerName],
            Account_FindOnlineByDbId(g_Biz[slot][bOwnerDbId]) != INVALID_PLAYER_ID ? ("na serveru") : ("nije na serveru"));
    }
    Staff_InfoRow(body, sizeof(body), "Vlasnik", value);
    format(value, sizeof(value), "$%d / nivo %d", g_Biz[slot][bPrice], BIZ_MIN_LEVEL);
    Staff_InfoRow(body, sizeof(body), "Cena / nivo", value);
    Staff_InfoRow(body, sizeof(body), "Vrata", g_Biz[slot][bLocked] ? ("zatvorena") : ("otvorena"));
    format(value, sizeof(value), "$%d", g_Biz[slot][bTill]);
    Staff_InfoRow(body, sizeof(body), "Kasa", value);
    format(value, sizeof(value), "%d/%d", g_Biz[slot][bStock], Biz_MaxStock(slot));
    Staff_InfoRow(body, sizeof(body), "Roba", value);
    format(value, sizeof(value), "%d%%, ulaznica $%d", g_Biz[slot][bPercent], g_Biz[slot][bFee]);
    Staff_InfoRow(body, sizeof(body), "Cene", value);
    format(value, sizeof(value), "$%d po plati, dug $%d", Biz_Tax(slot), g_Biz[slot][bTaxDebt]);
    Staff_InfoRow(body, sizeof(body), "Porez", value);
    if (g_Biz[slot][bType] == BIZ_GAS)
    {
        format(value, sizeof(value), "%d (mesta 'pumpa' do %.0f m)", Point_CountNear(POINT_FUEL, g_Biz[slot][bX], g_Biz[slot][bY], g_Biz[slot][bZ], BIZ_PUMP_RADIUS), BIZ_PUMP_RADIUS);
        Staff_InfoRow(body, sizeof(body), "Pumpe", value);
    }
    new title[48];
    format(title, sizeof(title), "Firma %d - podaci", g_Biz[slot][bDbId]);
    Staff_InfoShow(playerid, title, body);
    return 1;
}
alias:finfo("binfo")

cmd:firme(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }
    new body[4096], line[112], address[48], count;
    strcopy(body, "ID\tFirma\tVlasnik\tCena\n");
    for (new i = 0; i < MAX_BUSINESSES && strlen(body) < sizeof(body) - sizeof(line); i++)
    {
        if (!g_Biz[i][bUsed])
        {
            continue;
        }
        new owner[MAX_PLAYER_NAME + 1] = "na prodaju";
        if (g_Biz[i][bOwnerDbId] != 0)
        {
            strcopy(owner, g_Biz[i][bOwnerName]);
        }
        Biz_Address(i, address);
        format(line, sizeof(line), "%d\t%s (%s)\t%s\t$%d\n", g_Biz[i][bDbId], g_BizTypes[g_Biz[i][bType]][btName], address, owner, g_Biz[i][bPrice]);
        strcat(body, line);
        count++;
    }
    if (count == 0)
    {
        Msg_Info(playerid, "Jos nema firmi. Pravi se sa /napravifirmu [cena].");
        return 1;
    }
    ShowPlayerDialog(playerid, DIALOG_ADMIN_INFO, DIALOG_STYLE_TABLIST_HEADERS, SERVER_NAME " - Firme", body, "Zatvori", "");
    return 1;
}

cmd:enterijerifirmi(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }
    new list[sizeof(g_BizInteriors) * 44];
    for (new i = 0; i < sizeof(g_BizInteriors); i++)
    {
        strcat(list, g_BizInteriors[i][biName]);
        strcat(list, "\n");
    }
    ShowPlayerDialog(playerid, DIALOG_BIZ_INTERIORS, DIALOG_STYLE_LIST, "Enterijeri firmi", list, "Udji", "Zatvori");
    return 1;
}

static stock Biz_OnMenuRow(playerid, slot, E_BIZ_ROW:row)
{
    switch (row)
    {
        case BIZ_ROW_INFO: Biz_ShowInfo(playerid, slot);
        case BIZ_ROW_LOCK:
        {
            if (Biz_RequireHere(playerid, slot))
            {
                Biz_ToggleLock(playerid, slot);
            }
        }
        case BIZ_ROW_GPS:
        {
            new address[48];
            Biz_Address(slot, address);
            Gps_StartAt(playerid, -1, address, g_Biz[slot][bX], g_Biz[slot][bY], g_Biz[slot][bZ]);
        }
        case BIZ_ROW_WITHDRAW:
        {
            if (Biz_RequireHere(playerid, slot))
            {
                Biz_ShowAmountInput(playerid, DIALOG_BIZ_WITHDRAW, "Podigni iz kase");
            }
        }
        case BIZ_ROW_DEPOSIT:
        {
            if (Biz_RequireHere(playerid, slot))
            {
                Biz_ShowAmountInput(playerid, DIALOG_BIZ_DEPOSIT, "Ostavi u kasu");
            }
        }
        case BIZ_ROW_PERCENT:
        {
            new text[160];
            format(text, sizeof(text), "Sada: %d%% drzavne cene. Upisite novu cenu u procentima (%d-%d).\nVaze odmah za sve sto firma prodaje.",
                g_Biz[slot][bPercent], BIZ_PERCENT_MIN, BIZ_PERCENT_MAX);
            ShowPlayerDialog(playerid, DIALOG_BIZ_PERCENT, DIALOG_STYLE_INPUT, "Cene", text, "Postavi", "Nazad");
        }
        case BIZ_ROW_ORDER: Biz_ShowOrder(playerid, slot);
        case BIZ_ROW_NAME:
        {
            new text[96];
            format(text, sizeof(text), "Upisite novo ime firme (%d-%d znakova, bez { } %% i ~):", BIZ_NAME_MIN, BIZ_NAME_MAX);
            ShowPlayerDialog(playerid, DIALOG_BIZ_NAME, DIALOG_STYLE_INPUT, "Ime firme", text, "Promeni", "Nazad");
        }
        case BIZ_ROW_FEE:
        {
            new text[96];
            format(text, sizeof(text), "Sada: $%d. Upisite cenu ulaza ($0-%d); vi i admini ulazite besplatno.", g_Biz[slot][bFee], BIZ_FEE_MAX);
            ShowPlayerDialog(playerid, DIALOG_BIZ_FEE, DIALOG_STYLE_INPUT, "Ulaznica", text, "Postavi", "Nazad");
        }
        case BIZ_ROW_SELL:
        {
            new text[256];
            format(text, sizeof(text), "Prodajete firmu drzavi za $%d (%d%% vrednosti).\n\nKasa mora biti prazna i firma bez duga za porez.\nRoba ostaje firmi.",
                g_Biz[slot][bPrice] * BIZ_SELL_PERCENT / 100, BIZ_SELL_PERCENT);
            ShowPlayerDialog(playerid, DIALOG_BIZ_SELL, DIALOG_STYLE_MSGBOX, "Prodaja firme", text, "Prodaj", "Odustani");
        }
    }
}

stock bool:Biz_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
    switch (dialogid)
    {
        case DIALOG_BIZ_INTERIORS:
        {
            if (!response || listitem < 0 || listitem >= sizeof(g_BizInteriors) || !Staff_IsAdminOnDuty(playerid))
            {
                return true;
            }
            Staff_TeleportTo(playerid, g_BizInteriors[listitem][biX], g_BizInteriors[listitem][biY], g_BizInteriors[listitem][biZ],
                g_BizInteriors[listitem][biInterior], 0);
            SetPlayerFacingAngle(playerid, g_BizInteriors[listitem][biA]);
            SetCameraBehindPlayer(playerid);
            Msg_Admin(playerid, "Enterijer: %s. Izlaz: /enterijerifirmi pa drugi enterijer, ili /tp.", g_BizInteriors[listitem][biName]);
        }
        case DIALOG_BIZ_TYPE:
        {
            if (!response || listitem < 0 || listitem >= _:E_BIZ_TYPE || !Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
            {
                return true;
            }
            if (g_BizTypeFor[playerid] == 0)
            {
                Biz_Create(playerid, E_BIZ_TYPE:listitem);
                return true;
            }
            new slot = Biz_FindSlotByDbId(g_BizTypeFor[playerid]);
            if (slot == -1 || g_Biz[slot][bOwnerDbId] != 0)
            {
                return true;
            }
            // The old type's interior is gone, so nobody may stay inside it.
            Biz_EmptyInside(slot);
            g_Biz[slot][bType] = E_BIZ_TYPE:listitem;
            strcopy(g_Biz[slot][bName], g_BizTypes[g_Biz[slot][bType]][btName], BIZ_NAME_MAX + 1);
            Biz_Refresh(slot);
            Biz_Save(slot);
            new details[64];
            format(details, sizeof(details), "firma %d: tip %s", g_Biz[slot][bDbId], g_BizTypes[g_Biz[slot][bType]][btName]);
            Staff_Log(playerid, "biz_edit", INVALID_PLAYER_ID, details);
            Msg_Admin(playerid, "Izmenjena je %s.", details);
        }
        case DIALOG_BIZ_ENTRY:
        {
            if (response)
            {
                Biz_PayEntry(playerid);
            }
            else
            {
                g_BizEntryFor[playerid] = -1;
            }
        }
        case DIALOG_BIZ_MEALS:
        {
            if (!response || listitem < 0 || listitem >= BIZ_MEALS_LISTED || g_BizMealListed[playerid][listitem] == -1)
            {
                return true;
            }
            Biz_BuyMeal(playerid, g_BizMealListed[playerid][listitem]);
        }
        case DIALOG_BIZ_MENU:
        {
            new slot = g_BizSelected[playerid];
            if (!response || !Biz_IsOwner(playerid, slot) || listitem < 0 || listitem >= g_BizMenuCount[playerid])
            {
                return true;
            }
            Biz_OnMenuRow(playerid, slot, g_BizMenuRows[playerid][E_BIZ_ROW:listitem]);
        }
        case DIALOG_BIZ_INFO:
        {
            if (Biz_IsOwner(playerid, g_BizSelected[playerid]))
            {
                Biz_ShowMenu(playerid, g_BizSelected[playerid]);
            }
        }
        case DIALOG_BIZ_WITHDRAW, DIALOG_BIZ_DEPOSIT:
        {
            new slot = g_BizSelected[playerid];
            if (!Biz_IsOwner(playerid, slot))
            {
                return true;
            }
            if (!response)
            {
                Biz_ShowMenu(playerid, slot);
                return true;
            }
            if (!Biz_RequireHere(playerid, slot))
            {
                return true;
            }
            new amount = Money_ParseAmount(inputtext), bool:withdraw = dialogid == DIALOG_BIZ_WITHDRAW, reason[32];
            if (amount == 0 || (withdraw && amount > g_Biz[slot][bTill]) || (!withdraw && amount > g_Player[playerid][pCash]))
            {
                Biz_ShowAmountInput(playerid, dialogid, withdraw ? ("Podigni iz kase") : ("Ostavi u kasu"), "Neispravan iznos ili nema toliko novca.");
                return true;
            }
            format(reason, sizeof(reason), withdraw ? ("kasa firme %d: podignuto") : ("kasa firme %d: ostavljeno"), g_Biz[slot][bDbId]);
            Money_GiveCash(playerid, withdraw ? amount : -amount, reason);
            g_Biz[slot][bTill] += withdraw ? -amount : amount;
            new debt = withdraw ? 0 : Biz_PayDebtFromTill(slot);
            Biz_Save(slot);
            Account_Save(playerid);
            Chat_Action(playerid, withdraw ? ("uzima novac iz kase.") : ("ostavlja novac u kasu."));
            if (debt > 0)
            {
                Msg_Success(playerid, "U kasi je sada $%d; od uplate je placen dug za porez $%d.", g_Biz[slot][bTill], debt);
            }
            else
            {
                Msg_Success(playerid, "U kasi je sada $%d.", g_Biz[slot][bTill]);
            }
        }
        case DIALOG_BIZ_PERCENT:
        {
            new slot = g_BizSelected[playerid], percent;
            if (!Biz_IsOwner(playerid, slot))
            {
                return true;
            }
            if (!response)
            {
                Biz_ShowMenu(playerid, slot);
                return true;
            }
            if (sscanf(inputtext, "d", percent) || percent < BIZ_PERCENT_MIN || percent > BIZ_PERCENT_MAX)
            {
                Msg_Error(playerid, "Cena je od %d%% do %d%% drzavne cene.", BIZ_PERCENT_MIN, BIZ_PERCENT_MAX);
                return true;
            }
            g_Biz[slot][bPercent] = percent;
            Biz_Refresh(slot);
            Biz_Save(slot);
            Msg_Success(playerid, "Cene u firmi su sada %d%% drzavne cene.", percent);
        }
        case DIALOG_BIZ_ORDER:
        {
            new slot = g_BizSelected[playerid], units;
            if (!Biz_IsOwner(playerid, slot))
            {
                return true;
            }
            if (!response)
            {
                Biz_ShowMenu(playerid, slot);
                return true;
            }
            if (sscanf(inputtext, "d", units))
            {
                units = 0;
            }
            Biz_Order(playerid, slot, units);
        }
        case DIALOG_BIZ_NAME:
        {
            new slot = g_BizSelected[playerid];
            if (!Biz_IsOwner(playerid, slot))
            {
                return true;
            }
            if (!response)
            {
                Biz_ShowMenu(playerid, slot);
                return true;
            }
            if (!Biz_IsValidName(inputtext))
            {
                Msg_Error(playerid, "Ime ima %d-%d znakova, bez { } %% i ~.", BIZ_NAME_MIN, BIZ_NAME_MAX);
                return true;
            }
            strcopy(g_Biz[slot][bName], inputtext, BIZ_NAME_MAX + 1);
            Biz_Refresh(slot);
            Biz_Save(slot);
            Msg_Success(playerid, "Firma se sada zove: %s", g_Biz[slot][bName]);
        }
        case DIALOG_BIZ_FEE:
        {
            new slot = g_BizSelected[playerid], fee;
            if (!Biz_IsOwner(playerid, slot) || !g_BizTypes[g_Biz[slot][bType]][btHasFee])
            {
                return true;
            }
            if (!response)
            {
                Biz_ShowMenu(playerid, slot);
                return true;
            }
            if (sscanf(inputtext, "d", fee) || fee < 0 || fee > BIZ_FEE_MAX)
            {
                Msg_Error(playerid, "Ulaznica je od $0 do $%d.", BIZ_FEE_MAX);
                return true;
            }
            g_Biz[slot][bFee] = fee;
            Biz_Refresh(slot);
            Biz_Save(slot);
            Msg_Success(playerid, "Ulaz u firmu sada kosta $%d.", fee);
        }
        case DIALOG_BIZ_SELL:
        {
            if (response)
            {
                Biz_SellToState(playerid, g_BizSelected[playerid]);
            }
        }
        default:
        {
            return false;
        }
    }
    return true;
}
