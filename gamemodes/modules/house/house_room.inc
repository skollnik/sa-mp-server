// The rent the player saw when asked to confirm, so a change by the owner in between is not charged unseen.
static g_RoomPriceSeen[MAX_PLAYERS];
static g_RoomTenantIds[MAX_PLAYERS][3];
static g_RoomTenantNames[MAX_PLAYERS][3][MAX_PLAYER_NAME + 1];
static g_RoomTenantPaid[MAX_PLAYERS][3];
static g_RoomEvictPick[MAX_PLAYERS];

stock Room_Init()
{
    mysql_tquery(g_SQL, "ALTER TABLE `houses` ADD COLUMN IF NOT EXISTS `rooms_open` TINYINT(1) NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `houses` ADD COLUMN IF NOT EXISTS `room_price` INT NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `accounts` ADD COLUMN IF NOT EXISTS `room_house` INT UNSIGNED NOT NULL DEFAULT 0");
    mysql_tquery(g_SQL, "ALTER TABLE `accounts` ADD INDEX IF NOT EXISTS `account_room_house` (`room_house`)");
    mysql_tquery(g_SQL, "ALTER TABLE `accounts` ADD COLUMN IF NOT EXISTS `room_paid` INT NOT NULL DEFAULT 0");
}

stock Room_OnHouseLoaded(row)
{
    new open;
    cache_get_value_name_int(row, "rooms_open", open);
    g_Houses[row][hRoomsOpen] = open != 0;
    cache_get_value_name_int(row, "room_price", g_Houses[row][hRoomPrice]);
}

// One room per size step, like the garage: a small house has one, a large one three.
stock Room_Count(slot)
{
    return _:House_Size(slot) + 1;
}

stock Room_CountTenants()
{
    mysql_tquery(g_SQL, "SELECT `room_house`, COUNT(*) AS `tenants` FROM `accounts` WHERE `room_house` <> 0 GROUP BY `room_house`", "OnRoomTenantsCounted");
}

forward OnRoomTenantsCounted();
public OnRoomTenantsCounted()
{
    for (new i = 0; i < MAX_HOUSES; i++)
    {
        g_Houses[i][hTenants] = 0;
    }
    new dbid, tenants;
    for (new row = 0; row < cache_num_rows(); row++)
    {
        cache_get_value_name_int(row, "room_house", dbid);
        cache_get_value_name_int(row, "tenants", tenants);
        new slot = House_FindSlotByDbId(dbid);
        if (slot != -1)
        {
            g_Houses[slot][hTenants] = tenants;
        }
    }
    for (new i = 0; i < MAX_HOUSES; i++)
    {
        if (g_Houses[i][hUsed] && g_Houses[i][hRoomsOpen])
        {
            House_Refresh(i);
        }
    }
    return 1;
}

stock bool:Room_IsTenant(playerid, slot)
{
    return slot != -1 && g_RoomHouse[playerid] != 0 && g_RoomHouse[playerid] == g_Houses[slot][hDbId];
}

static stock bool:Room_HasFree(slot)
{
    return g_Houses[slot][hRoomsOpen] && g_Houses[slot][hTenants] < Room_Count(slot);
}

// The door label of a house with a free room tells passers-by the rent.
stock Room_AddOfferText(slot, text[], size)
{
    if (!Room_HasFree(slot))
    {
        return;
    }
    new line[64];
    format(line, sizeof(line), "\n" C_GREEN "Soba: $%d po plati (/iznajmisobu)", g_Houses[slot][hRoomPrice]);
    strcat(text, line, size);
}

stock Room_ShowIcon(playerid)
{
    Room_HideIcon(playerid);
    new slot = House_FindSlotByDbId(g_RoomHouse[playerid]);
    if (g_RoomHouse[playerid] == 0 || slot == -1)
    {
        return;
    }
    g_RoomIcon[playerid] = CreateDynamicMapIcon(g_Houses[slot][hX], g_Houses[slot][hY], g_Houses[slot][hZ], HOUSE_ICON_OWNED, 0, 0, 0, playerid,
        HOUSE_OWN_ICON_DISTANCE, MAPICON_GLOBAL);
    Streamer_Update(playerid, STREAMER_TYPE_MAP_ICON);
}

stock Room_HideIcon(playerid)
{
    if (IsValidDynamicMapIcon(g_RoomIcon[playerid])) DestroyDynamicMapIcon(g_RoomIcon[playerid]);
    g_RoomIcon[playerid] = STREAMER_TAG_MAP_ICON:INVALID_STREAMER_ID;
}

// Only what the server keeps for an online tenant; the database is updated by the caller.
static stock Room_Forget(playerid)
{
    if (g_HouseSpawn[playerid] != 0 && g_HouseSpawn[playerid] == g_RoomHouse[playerid])
    {
        g_HouseSpawn[playerid] = 0;
    }
    g_RoomHouse[playerid] = 0;
    g_RoomPaid[playerid] = 0;
    Room_HideIcon(playerid);
}

// The spawn in the room goes with the room; the same column order keeps house_spawn readable before room_house is cleared.
#define ROOM_LEAVE_SET "`house_spawn` = IF(`house_spawn` = `room_house`, 0, `house_spawn`), `room_house` = 0, `room_paid` = 0"

static stock Room_SetPaid(playerid, amount)
{
    new query[96];
    g_RoomPaid[playerid] = amount;
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `accounts` SET `room_paid` = %d WHERE `id` = %d", amount, g_Player[playerid][pDbId]);
    mysql_tquery(g_SQL, query);
}

stock Room_End(playerid, slot)
{
    new query[256];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `accounts` SET " ROOM_LEAVE_SET " WHERE `id` = %d", g_Player[playerid][pDbId]);
    mysql_tquery(g_SQL, query);
    Room_Forget(playerid);
    Wardrobe_StashOrphans();
    if (slot != -1 && g_Houses[slot][hTenants] > 0)
    {
        g_Houses[slot][hTenants]--;
        House_Refresh(slot);
    }
}

stock Room_EvictAll(slot)
{
    new query[256], dbid = g_Houses[slot][hDbId], address[48];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `accounts` SET " ROOM_LEAVE_SET " WHERE `room_house` = %d", dbid);
    mysql_tquery(g_SQL, query);
    House_Address(slot, address);
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (IsPlayerConnected(i) && g_RoomHouse[i] == dbid)
        {
            Room_Forget(i);
            Msg_Info(i, "Kuca %s je promenila vlasnika, pa vise niste stanar u njoj.", address);
        }
    }
    g_Houses[slot][hTenants] = 0;
}

// A tenant who gets a house of their own moves out of the room.
stock Room_EndForAccount(account_dbid)
{
    new playerid = Account_FindOnlineByDbId(account_dbid);
    if (playerid == INVALID_PLAYER_ID || g_RoomHouse[playerid] == 0)
    {
        return;
    }
    Room_End(playerid, House_FindSlotByDbId(g_RoomHouse[playerid]));
    Msg_Info(playerid, "Imate svoju kucu, pa ste se iselili iz iznajmljene sobe.");
}

// Tenants away too long; online tenants cannot match, since a login renews last_login.
stock Room_CheckInactive()
{
    new query[256];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `accounts` SET " ROOM_LEAVE_SET " WHERE `room_house` <> 0 AND `last_login` < NOW() - INTERVAL %d DAY",
        ROOM_INACTIVE_DAYS);
    mysql_tquery(g_SQL, query);
    Wardrobe_StashOrphans();
    Room_CountTenants();
}

// Bank first, then cash; false when neither covers the rent.
static stock bool:Room_TakeRent(playerid, amount, const reason[])
{
    if (g_Player[playerid][pHasBankAccount] && g_Player[playerid][pBank] >= amount)
    {
        Money_GiveBank(playerid, -amount, reason);
        return true;
    }
    if (g_Player[playerid][pCash] >= amount)
    {
        Money_GiveCash(playerid, -amount, reason);
        return true;
    }
    return false;
}

static stock Room_PayOwner(playerid, slot, amount)
{
    new reason[48];
    format(reason, sizeof(reason), "kirija za sobu u kuci %d", g_Houses[slot][hDbId]);
    new ownerid = Money_PayAccount(g_Houses[slot][hOwnerDbId], amount, reason);
    if (ownerid != INVALID_PLAYER_ID)
    {
        Msg_Info(ownerid, "Stanar %s je platio kiriju $%d (na racun u banci).", Account_Name(playerid), amount);
    }
}

// Called on payday, only to a tenant who got the payday.
stock Room_ChargeRent(playerid)
{
    new slot = House_FindSlotByDbId(g_RoomHouse[playerid]);
    if (g_RoomHouse[playerid] == 0 || slot == -1)
    {
        return;
    }
    new price = g_Houses[slot][hRoomPrice];
    if (price == 0)
    {
        if (g_RoomPaid[playerid] != 0)
        {
            Room_SetPaid(playerid, 0);
        }
        return;
    }
    new reason[48];
    format(reason, sizeof(reason), "kirija za sobu u kuci %d", g_Houses[slot][hDbId]);
    if (!Room_TakeRent(playerid, price, reason))
    {
        Room_End(playerid, slot);
        SendClientMessage(playerid, COLOR_WHITE, "Kirija za sobu: " C_RED "$%d" C_WHITE " - nemate toliko ni na racunu ni u dzepu, pa ste iseljeni iz sobe.", price);
        new ownerid = Account_FindOnlineByDbId(g_Houses[slot][hOwnerDbId]);
        if (ownerid != INVALID_PLAYER_ID)
        {
            Msg_Info(ownerid, "Stanar %s nije imao za kiriju i iselio se.", Account_Name(playerid));
        }
        return;
    }
    Room_PayOwner(playerid, slot, price);
    Room_SetPaid(playerid, price);
    SendClientMessage(playerid, COLOR_WHITE, "Kirija za sobu: " C_RED "-$%d" C_WHITE " (placa se sa racuna, a bez dovoljno na racunu iz dzepa).", price);
}

// Everything that must hold to rent a room here, checked when asking and again on the answer.
static stock bool:Room_CheckCanRent(playerid, slot)
{
    if (slot == -1)
    {
        Msg_Error(playerid, "Niste ispred kuce.");
        return false;
    }
    if (g_Houses[slot][hOwnerDbId] == 0)
    {
        Msg_Error(playerid, "Kuca nema vlasnika; sobe izdaje vlasnik.");
        return false;
    }
    if (House_IsOwner(playerid, slot))
    {
        Msg_Error(playerid, "Ovo je vasa kuca.");
        return false;
    }
    if (g_RoomHouse[playerid] != 0)
    {
        Msg_Error(playerid, "Vec iznajmljujete sobu. Iz nje se iseljava sa /napustisobu.");
        return false;
    }
    if (House_CountOwned(g_Player[playerid][pDbId]) > 0)
    {
        Msg_Error(playerid, "Imate kucu; soba se iznajmljuje samo kad nemate svoju.");
        return false;
    }
    if (g_Player[playerid][pLevel] < ROOM_MIN_LEVEL)
    {
        Msg_Error(playerid, "Za iznajmljivanje sobe treba nivo %d.", ROOM_MIN_LEVEL);
        return false;
    }
    if (!g_Houses[slot][hRoomsOpen])
    {
        Msg_Error(playerid, "Vlasnik ove kuce ne izdaje sobe.");
        return false;
    }
    if (!Room_HasFree(slot))
    {
        Msg_Error(playerid, "Sve sobe u ovoj kuci su zauzete (%d/%d).", g_Houses[slot][hTenants], Room_Count(slot));
        return false;
    }
    return true;
}

static stock Room_Rent(playerid, slot)
{
    if (!Room_CheckCanRent(playerid, slot))
    {
        return;
    }
    new price = g_Houses[slot][hRoomPrice];
    if (price != g_RoomPriceSeen[playerid])
    {
        Msg_Error(playerid, "Vlasnik je upravo promenio kiriju na $%d. Ukucajte ponovo /iznajmisobu.", price);
        return;
    }
    // The first rent is paid now, so a room cannot be had free until the first payday.
    if (price > 0)
    {
        new reason[48];
        format(reason, sizeof(reason), "kirija za sobu u kuci %d", g_Houses[slot][hDbId]);
        if (!Room_TakeRent(playerid, price, reason))
        {
            Msg_Error(playerid, "Nemate $%d za prvu kiriju (ni na racunu ni u dzepu).", price);
            return;
        }
        Room_PayOwner(playerid, slot, price);
    }
    g_RoomHouse[playerid] = g_Houses[slot][hDbId];
    g_RoomPaid[playerid] = price;
    new query[128];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `accounts` SET `room_house` = %d, `room_paid` = %d WHERE `id` = %d", g_RoomHouse[playerid], price, g_Player[playerid][pDbId]);
    mysql_tquery(g_SQL, query);
    Account_Save(playerid);
    g_Houses[slot][hTenants]++;
    House_Refresh(slot);
    if (g_HouseIconsShown[playerid])
    {
        Room_ShowIcon(playerid);
    }
    Chat_Action(playerid, "dobija kljuc od sobe.");
    Msg_Success(playerid, "Iznajmili ste sobu. Ulazite i kad je zakljucano; meni sobe: /kuca, ormar: /ormar, iseljenje: /napustisobu.");
    new ownerid = Account_FindOnlineByDbId(g_Houses[slot][hOwnerDbId]);
    if (ownerid != INVALID_PLAYER_ID)
    {
        Msg_Info(ownerid, "%s je iznajmio sobu u vasoj kuci (kirija $%d po plati).", Account_Name(playerid), price);
    }
}

cmd:iznajmisobu(playerid, params[])
{
    new slot = House_FindOutside(playerid, HOUSE_NEAR_RADIUS);
    if (!Room_CheckCanRent(playerid, slot))
    {
        return 1;
    }
    g_HouseSelected[playerid] = slot;
    g_RoomPriceSeen[playerid] = g_Houses[slot][hRoomPrice];
    new text[320], address[48];
    House_Address(slot, address);
    format(text, sizeof(text), "Kuca: %s (vlasnik %s)\nKirija: $%d po plati\n\nPrva kirija se placa odmah, a posle na svakoj plati: sa racuna, ili iz dzepa ako na racunu nema dovoljno.\n" \
        "Ako nemate za kiriju, iseljavate se.\n\nStanar ulazi i kad je zakljucano, moze da se pojavljuje u kuci i ima ormar (/ormar).",
        address, g_Houses[slot][hOwnerName], g_Houses[slot][hRoomPrice]);
    ShowPlayerDialog(playerid, DIALOG_ROOM_CONFIRM, DIALOG_STYLE_MSGBOX, "Iznajmljivanje sobe", text, "Iznajmi", "Odustani");
    return 1;
}
alias:iznajmisobu("rentroom")

static stock Room_AskLeave(playerid)
{
    if (g_RoomHouse[playerid] == 0)
    {
        Msg_Error(playerid, "Ne iznajmljujete sobu.");
        return;
    }
    ShowPlayerDialog(playerid, DIALOG_ROOM_LEAVE, DIALOG_STYLE_MSGBOX, "Napustanje sobe", "Iseljavate se iz sobe? Placena kirija se ne vraca.\n\nGarderoba iz vaseg ormara ide u magacin; uzimate je iz ormara u sledecem domu (/ormar).", "Iseli se", "Odustani");
}

cmd:napustisobu(playerid, params[])
{
    Room_AskLeave(playerid);
    return 1;
}
alias:napustisobu("unrentroom")

static stock Room_Leave(playerid)
{
    new slot = House_FindSlotByDbId(g_RoomHouse[playerid]);
    if (g_RoomHouse[playerid] == 0)
    {
        return;
    }
    Room_End(playerid, slot);
    Msg_Success(playerid, "Iselili ste se iz sobe.");
    if (slot != -1)
    {
        new ownerid = Account_FindOnlineByDbId(g_Houses[slot][hOwnerDbId]);
        if (ownerid != INVALID_PLAYER_ID)
        {
            Msg_Info(ownerid, "%s se iselio iz sobe u vasoj kuci.", Account_Name(playerid));
        }
    }
}

// The tenant's /kuca; false when the player rents no room.
stock bool:Room_ShowMenu(playerid)
{
    new slot = House_FindSlotByDbId(g_RoomHouse[playerid]);
    if (g_RoomHouse[playerid] == 0 || slot == -1)
    {
        return false;
    }
    new body[160], address[48], title[64];
    format(body, sizeof(body), "Informacije\nOznaci na GPS-u\nPojavljivanje u sobi: %s\nNapusti sobu", g_HouseSpawn[playerid] == g_RoomHouse[playerid] ? ("DA") : ("NE"));
    House_Address(slot, address);
    format(title, sizeof(title), "Soba: %s", address);
    ShowPlayerDialog(playerid, DIALOG_ROOM_MENU, DIALOG_STYLE_LIST, title, body, "Izaberi", "Zatvori");
    return true;
}

static stock Room_ShowInfo(playerid, slot)
{
    new text[384], address[48];
    House_Address(slot, address);
    format(text, sizeof(text), "Adresa: %s\nVlasnik: %s\nKirija: $%d po plati\n\nKirija se placa na svakoj plati koju dobijete: sa racuna, ili iz dzepa ako na racunu nema dovoljno.\n" \
        "Bez novca za kiriju iseljavate se. Soba se gubi i ako ne udjete na server %d dana ili kuca promeni vlasnika.\nOrmar: /ormar u kuci.",
        address, g_Houses[slot][hOwnerName], g_Houses[slot][hRoomPrice], ROOM_INACTIVE_DAYS);
    ShowPlayerDialog(playerid, DIALOG_NONE, DIALOG_STYLE_MSGBOX, "Iznajmljena soba", text, "Zatvori", "");
}

stock Room_ShowOwnerMenu(playerid, slot)
{
    g_HouseSelected[playerid] = slot;
    new body[192];
    format(body, sizeof(body), "Sobe: %s\nKirija: $%d po plati (promeni)\nStanari (%d/%d)",
        g_Houses[slot][hRoomsOpen] ? ("OTVORENE (klik zatvara)") : ("ZATVORENE (klik otvara)"), g_Houses[slot][hRoomPrice], g_Houses[slot][hTenants], Room_Count(slot));
    ShowPlayerDialog(playerid, DIALOG_ROOM_OWNER, DIALOG_STYLE_LIST, "Izdavanje soba", body, "Izaberi", "Nazad");
}

static stock Room_AskPrice(playerid, const error[] = "")
{
    new text[192];
    format(text, sizeof(text), "Kirija po plati, od $0 do $%d (0 je za porodicu i prijatelje).\nNova kirija vazi od sledece plate.%s%s",
        ROOM_PRICE_MAX, error[0] ? ("\n\n" C_RED) : (""), error);
    ShowPlayerDialog(playerid, DIALOG_ROOM_PRICE, DIALOG_STYLE_INPUT, "Kirija", text, "Potvrdi", "Nazad");
}

static stock Room_SetPrice(playerid, slot, price)
{
    g_Houses[slot][hRoomPrice] = price;
    House_Refresh(slot);
    House_Save(slot);
    new address[48];
    House_Address(slot, address);
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (IsPlayerConnected(i) && g_RoomHouse[i] == g_Houses[slot][hDbId])
        {
            Msg_Info(i, "Vlasnik kuce %s je promenio kiriju na $%d po plati (od sledece plate).", address, price);
        }
    }
    Msg_Success(playerid, "Kirija je $%d po plati.", price);
    Room_ShowOwnerMenu(playerid, slot);
}

static stock Room_ListTenants(playerid, slot)
{
    new query[128];
    mysql_format(g_SQL, query, sizeof(query), "SELECT `id`, `name`, `room_paid` FROM `accounts` WHERE `room_house` = %d LIMIT 3", g_Houses[slot][hDbId]);
    mysql_tquery(g_SQL, query, "OnRoomTenantsListed", "ddd", playerid, g_AccountRace[playerid], g_Houses[slot][hDbId]);
}

forward OnRoomTenantsListed(playerid, race, house_dbid);
public OnRoomTenantsListed(playerid, race, house_dbid)
{
    new slot = House_FindSlotByDbId(house_dbid);
    if (race != g_AccountRace[playerid] || slot == -1 || !House_IsOwner(playerid, slot))
    {
        return 1;
    }
    new rows = min(cache_num_rows(), sizeof(g_RoomTenantIds[])), body[128];
    if (rows == 0)
    {
        Msg_Info(playerid, "Nemate stanara.");
        Room_ShowOwnerMenu(playerid, slot);
        return 1;
    }
    for (new row = 0; row < rows; row++)
    {
        cache_get_value_name_int(row, "id", g_RoomTenantIds[playerid][row]);
        cache_get_value_name(row, "name", g_RoomTenantNames[playerid][row], MAX_PLAYER_NAME + 1);
        cache_get_value_name_int(row, "room_paid", g_RoomTenantPaid[playerid][row]);
        strcat(body, g_RoomTenantNames[playerid][row]);
        strcat(body, "\n");
    }
    g_HouseSelected[playerid] = slot;
    ShowPlayerDialog(playerid, DIALOG_ROOM_TENANTS, DIALOG_STYLE_LIST, "Stanari (klik iseljava)", body, "Iseli", "Nazad");
    return 1;
}

// The last rent the tenant paid; an online tenant's own value is newer than the list the owner opened.
static stock Room_EvictRefund(playerid, pick)
{
    new tenant = Account_FindOnlineByDbId(g_RoomTenantIds[playerid][pick]);
    return tenant != INVALID_PLAYER_ID && g_RoomHouse[tenant] != 0 ? g_RoomPaid[tenant] : g_RoomTenantPaid[playerid][pick];
}

// The owner gives back the last rent paid, since the tenant paid for a payday they will not live there.
// The tenant may be offline: then only the database changes, and the count is read again from it.
static stock Room_Evict(playerid, slot, pick)
{
    new account_id = g_RoomTenantIds[playerid][pick], refund = Room_EvictRefund(playerid, pick), reason[48];
    new online = Account_FindOnlineByDbId(account_id);
    if (online != INVALID_PLAYER_ID && !Room_IsTenant(online, slot))
    {
        Msg_Error(playerid, "%s vise nije stanar u ovoj kuci.", g_RoomTenantNames[playerid][pick]);
        return;
    }
    if (refund > 0)
    {
        format(reason, sizeof(reason), "vracena kirija (iseljenje iz kuce %d)", g_Houses[slot][hDbId]);
        if (!Room_TakeRent(playerid, refund, reason))
        {
            Msg_Error(playerid, "Pri iseljenju vracate poslednju kiriju ($%d), a nemate toliko ni na racunu ni u dzepu.", refund);
            return;
        }
        Account_Save(playerid);
        Money_PayAccount(account_id, refund, reason);
    }
    new tenant = Account_FindOnlineByDbId(account_id);
    if (tenant != INVALID_PLAYER_ID && Room_IsTenant(tenant, slot))
    {
        Room_End(tenant, slot);
        if (refund > 0)
        {
            Msg_Info(tenant, "Vlasnik kuce vas je iselio iz sobe i vratio poslednju kiriju ($%d) u banku.", refund);
        }
        else
        {
            Msg_Info(tenant, "Vlasnik kuce vas je iselio iz sobe.");
        }
    }
    else
    {
        new query[256];
        mysql_format(g_SQL, query, sizeof(query), "UPDATE `accounts` SET " ROOM_LEAVE_SET " WHERE `id` = %d AND `room_house` = %d", account_id, g_Houses[slot][hDbId]);
        mysql_tquery(g_SQL, query);
        Wardrobe_StashOrphans();
        Room_CountTenants();
    }
    if (refund > 0)
    {
        Msg_Success(playerid, "Iselili ste stanara %s i vratili mu poslednju kiriju ($%d).", g_RoomTenantNames[playerid][pick], refund);
    }
    else
    {
        Msg_Success(playerid, "Iselili ste stanara %s.", g_RoomTenantNames[playerid][pick]);
    }
}

stock bool:Room_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
    switch (dialogid)
    {
        case DIALOG_ROOM_CONFIRM:
        {
            if (response)
            {
                new slot = House_FindOutside(playerid, HOUSE_NEAR_RADIUS);
                Room_Rent(playerid, slot == g_HouseSelected[playerid] ? slot : -1);
            }
        }
        case DIALOG_ROOM_LEAVE:
        {
            if (response)
            {
                Room_Leave(playerid);
            }
        }
        case DIALOG_ROOM_MENU:
        {
            new slot = House_FindSlotByDbId(g_RoomHouse[playerid]);
            if (!response || g_RoomHouse[playerid] == 0 || slot == -1)
            {
                return true;
            }
            switch (listitem)
            {
                case 0: Room_ShowInfo(playerid, slot);
                case 1:
                {
                    new address[48];
                    House_Address(slot, address);
                    Gps_StartAt(playerid, -1, address, g_Houses[slot][hX], g_Houses[slot][hY], g_Houses[slot][hZ]);
                }
                case 2:
                {
                    new bool:on = g_HouseSpawn[playerid] != g_RoomHouse[playerid];
                    House_SetSpawn(playerid, on ? g_RoomHouse[playerid] : 0);
                    if (on)
                    {
                        Msg_Success(playerid, "Pojavljujete se u sobi pri ulasku na server i posle zatvora.");
                    }
                    else
                    {
                        Msg_Success(playerid, "Pojavljivanje u sobi je iskljuceno.");
                    }
                    Room_ShowMenu(playerid);
                }
                case 3: Room_AskLeave(playerid);
            }
        }
        case DIALOG_ROOM_OWNER:
        {
            new slot = g_HouseSelected[playerid];
            if (!House_IsOwner(playerid, slot))
            {
                return true;
            }
            if (!response)
            {
                House_ShowMenu(playerid, slot);
                return true;
            }
            switch (listitem)
            {
                case 0:
                {
                    g_Houses[slot][hRoomsOpen] = !g_Houses[slot][hRoomsOpen];
                    House_Refresh(slot);
                    House_Save(slot);
                    if (g_Houses[slot][hRoomsOpen])
                    {
                        Msg_Success(playerid, "Sobe su otvorene: igraci ih iznajmljuju sa /iznajmisobu ispred vrata.");
                    }
                    else
                    {
                        Msg_Success(playerid, "Sobe su zatvorene za nove stanare; sadasnji ostaju.");
                    }
                    Room_ShowOwnerMenu(playerid, slot);
                }
                case 1: Room_AskPrice(playerid);
                case 2: Room_ListTenants(playerid, slot);
            }
        }
        case DIALOG_ROOM_PRICE:
        {
            new slot = g_HouseSelected[playerid], price;
            if (!House_IsOwner(playerid, slot))
            {
                return true;
            }
            if (!response)
            {
                Room_ShowOwnerMenu(playerid, slot);
                return true;
            }
            if (sscanf(inputtext, "d", price) || price < 0 || price > ROOM_PRICE_MAX)
            {
                Room_AskPrice(playerid, "Unesite ceo broj iz navedenog raspona.");
                return true;
            }
            Room_SetPrice(playerid, slot, price);
        }
        case DIALOG_ROOM_TENANTS:
        {
            new slot = g_HouseSelected[playerid];
            if (!House_IsOwner(playerid, slot))
            {
                return true;
            }
            if (!response || listitem < 0 || listitem >= sizeof(g_RoomTenantIds[]))
            {
                Room_ShowOwnerMenu(playerid, slot);
                return true;
            }
            g_RoomEvictPick[playerid] = listitem;
            new text[160], refund = Room_EvictRefund(playerid, listitem);
            if (refund > 0)
            {
                format(text, sizeof(text), "Iseljavate stanara %s?\n\nVracate mu poslednju placenu kiriju: $%d (sa racuna, a ako nema dovoljno iz dzepa).",
                    g_RoomTenantNames[playerid][listitem], refund);
            }
            else
            {
                format(text, sizeof(text), "Iseljavate stanara %s?", g_RoomTenantNames[playerid][listitem]);
            }
            ShowPlayerDialog(playerid, DIALOG_ROOM_EVICT, DIALOG_STYLE_MSGBOX, "Iseljenje stanara", text, "Iseli", "Odustani");
        }
        case DIALOG_ROOM_EVICT:
        {
            new slot = g_HouseSelected[playerid], pick = g_RoomEvictPick[playerid];
            if (response && House_IsOwner(playerid, slot))
            {
                Room_Evict(playerid, slot, pick);
            }
        }
        default:
        {
            return false;
        }
    }
    return true;
}
