stock Inv_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `inventory` (" \
        "`account_id` INT UNSIGNED NOT NULL," \
        "`slot` TINYINT UNSIGNED NOT NULL," \
        "`item` SMALLINT UNSIGNED NOT NULL," \
        "`amount` INT NOT NULL," \
        "PRIMARY KEY (`account_id`, `slot`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");

    g_DropTimer = SetTimer("Inv_DropTick", 60 * 1000, true);
}

stock Inv_Exit()
{
    KillTimer(g_DropTimer);
}

stock Inv_OnPlayerConnect(playerid)
{
    static const empty[E_INV_SLOT];
    for (new slot = 0; slot < INV_MAX_SLOTS; slot++)
    {
        g_Inv[playerid][slot] = empty;
    }
    g_InvLoaded[playerid] = false;
}

stock Inv_OnLogin(playerid)
{
    new query[96];
    mysql_format(g_SQL, query, sizeof(query), "SELECT `slot`, `item`, `amount` FROM `inventory` WHERE `account_id` = %d", g_Player[playerid][pDbId]);
    mysql_tquery(g_SQL, query, "OnInventoryLoaded", "dd", playerid, g_AccountRace[playerid]);
}

forward OnInventoryLoaded(playerid, race);
public OnInventoryLoaded(playerid, race)
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }

    new rows = cache_num_rows(), slot, item;
    for (new row = 0; row < rows; row++)
    {
        cache_get_value_name_int(row, "slot", slot);
        cache_get_value_name_int(row, "item", item);
        if (slot < 0 || slot >= INV_MAX_SLOTS || item <= 0 || item >= _:E_ITEM)
        {
            continue;
        }
        g_Inv[playerid][slot][isItem] = E_ITEM:item;
        cache_get_value_name_int(row, "amount", g_Inv[playerid][slot][isAmount]);
    }
    g_InvLoaded[playerid] = true;
    return 1;
}

// VIP slots disappear with VIP; items already in them stay, but nothing new goes in.
stock Inv_Capacity(playerid)
{
    return min(INV_BASE_SLOTS + g_VipInventorySlots[Vip_GetLevel(playerid)], INV_MAX_SLOTS);
}

stock Inv_SaveSlot(playerid, slot)
{
    new query[160];
    if (g_Inv[playerid][slot][isItem] == ITEM_NONE)
    {
        mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `inventory` WHERE `account_id` = %d AND `slot` = %d", g_Player[playerid][pDbId], slot);
    }
    else
    {
        mysql_format(g_SQL, query, sizeof(query), "REPLACE INTO `inventory` (`account_id`, `slot`, `item`, `amount`) VALUES (%d, %d, %d, %d)",
            g_Player[playerid][pDbId], slot, _:g_Inv[playerid][slot][isItem], g_Inv[playerid][slot][isAmount]);
    }
    mysql_tquery(g_SQL, query);
}

stock Inv_Count(playerid, E_ITEM:item)
{
    new count;
    for (new slot = 0; slot < INV_MAX_SLOTS; slot++)
    {
        if (g_Inv[playerid][slot][isItem] == item)
        {
            count += g_Inv[playerid][slot][isAmount];
        }
    }
    return count;
}

stock bool:Inv_Has(playerid, E_ITEM:item)
{
    return Inv_Count(playerid, item) > 0;
}

stock bool:Inv_CanAdd(playerid, E_ITEM:item, amount)
{
    if (!g_InvLoaded[playerid])
    {
        return false;
    }

    new room, stack = g_ItemInfo[item][itMaxStack], capacity = Inv_Capacity(playerid);
    for (new slot = 0; slot < INV_MAX_SLOTS; slot++)
    {
        if (g_Inv[playerid][slot][isItem] == item)
        {
            room += stack - g_Inv[playerid][slot][isAmount];
        }
        else if (g_Inv[playerid][slot][isItem] == ITEM_NONE && slot < capacity)
        {
            room += stack;
        }
    }
    return room >= amount;
}

stock bool:Inv_Add(playerid, E_ITEM:item, amount)
{
    if (!Inv_CanAdd(playerid, item, amount))
    {
        return false;
    }

    new stack = g_ItemInfo[item][itMaxStack], capacity = Inv_Capacity(playerid);
    for (new slot = 0; slot < INV_MAX_SLOTS && amount > 0; slot++)
    {
        if (g_Inv[playerid][slot][isItem] == item && g_Inv[playerid][slot][isAmount] < stack)
        {
            new put = min(amount, stack - g_Inv[playerid][slot][isAmount]);
            g_Inv[playerid][slot][isAmount] += put;
            amount -= put;
            Inv_SaveSlot(playerid, slot);
        }
    }
    for (new slot = 0; slot < capacity && amount > 0; slot++)
    {
        if (g_Inv[playerid][slot][isItem] == ITEM_NONE)
        {
            new put = min(amount, stack);
            g_Inv[playerid][slot][isItem] = item;
            g_Inv[playerid][slot][isAmount] = put;
            amount -= put;
            Inv_SaveSlot(playerid, slot);
        }
    }
    return true;
}

stock Inv_TakeFromSlot(playerid, slot, amount)
{
    new E_ITEM:item = g_Inv[playerid][slot][isItem];
    g_Inv[playerid][slot][isAmount] -= amount;
    if (g_Inv[playerid][slot][isAmount] <= 0)
    {
        g_Inv[playerid][slot][isItem] = ITEM_NONE;
        g_Inv[playerid][slot][isAmount] = 0;
    }
    Inv_SaveSlot(playerid, slot);

    if (item == ITEM_PHONE && !Inv_Has(playerid, ITEM_PHONE))
    {
        Phone_OnPhoneLost(playerid);
    }
}

stock bool:Inv_Remove(playerid, E_ITEM:item, amount)
{
    if (Inv_Count(playerid, item) < amount)
    {
        return false;
    }
    for (new slot = INV_MAX_SLOTS - 1; slot >= 0 && amount > 0; slot--)
    {
        if (g_Inv[playerid][slot][isItem] == item)
        {
            new take = min(amount, g_Inv[playerid][slot][isAmount]);
            amount -= take;
            Inv_TakeFromSlot(playerid, slot, take);
        }
    }
    return true;
}

stock Inv_Show(playerid)
{
    if (!g_InvLoaded[playerid])
    {
        Msg_Error(playerid, "Inventar se jos ucitava, pokusajte ponovo.");
        return;
    }

    new body[1024], line[64], count, capacity = Inv_Capacity(playerid), used;
    strcopy(body, "Predmet\tKolicina\n");
    for (new slot = 0; slot < INV_MAX_SLOTS; slot++)
    {
        if (g_Inv[playerid][slot][isItem] == ITEM_NONE)
        {
            continue;
        }
        used++;
        format(line, sizeof(line), "%s\t%d\n", g_ItemInfo[g_Inv[playerid][slot][isItem]][itName], g_Inv[playerid][slot][isAmount]);
        strcat(body, line);
        g_InvListed[playerid][count++] = slot;
    }

    if (count == 0)
    {
        Msg_Info(playerid, "Inventar je prazan (%d mesta). Predmeti se kupuju u prodavnicama 24/7 (/kupi).", capacity);
        return;
    }

    new title[64];
    format(title, sizeof(title), "Inventar (%d/%d mesta)", used, capacity);
    ShowPlayerDialog(playerid, DIALOG_INVENTORY, DIALOG_STYLE_TABLIST_HEADERS, title, body, "Izaberi", "Zatvori");
}

stock bool:Inv_IsBusy(playerid)
{
    if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT)
    {
        return true;
    }
    return g_Player[playerid][pJailSeconds] > 0 || g_Staff[playerid][sFrozen];
}

// Returns true when the item was used up.
stock bool:Inv_Use(playerid, slot)
{
    new E_ITEM:item = g_Inv[playerid][slot][isItem];
    switch (item)
    {
        case ITEM_BURGER, ITEM_PIZZA:
        {
            if (Hunger_IsFull(playerid))
            {
                Msg_Error(playerid, "Siti ste (traka gladi %d/%d). Jesti mozete kad traka padne na %d.", g_Player[playerid][pHunger], HUNGER_MAX, HUNGER_FULL);
                return false;
            }
            new Float:health;
            GetPlayerHealth(playerid, health);
            SetPlayerHealth(playerid, Utils_FloatMin(health + (item == ITEM_PIZZA ? 8.0 : 5.0), 100.0));
            ApplyAnimation(playerid, "FOOD", "EAT_Burger", 4.1, false, false, false, false, 0, SYNC_ALL);
            Chat_Action(playerid, item == ITEM_PIZZA ? ("jede parce pice.") : ("jede hamburger."));
        }
        case ITEM_WATER, ITEM_JUICE:
        {
            SetPlayerSpecialAction(playerid, SPECIAL_ACTION_DRINK_SPRUNK);
            Chat_Action(playerid, item == ITEM_WATER ? ("otvara flasu vode.") : ("otvara sok."));
            Inv_ShowHandHint(playerid, "gutljaj", "bacate flasu");
        }
        case ITEM_BEER:
        {
            SetPlayerSpecialAction(playerid, SPECIAL_ACTION_DRINK_BEER);
            SetPlayerDrunkLevel(playerid, min(GetPlayerDrunkLevel(playerid) + 1500, 10000));
            Chat_Action(playerid, "otvara pivo.");
            Inv_ShowHandHint(playerid, "gutljaj", "bacate flasu");
        }
        case ITEM_CIGARETTE:
        {
            if (!Inv_Has(playerid, ITEM_LIGHTER))
            {
                Msg_Error(playerid, "Treba vam upaljac.");
                return false;
            }
            SetPlayerSpecialAction(playerid, SPECIAL_ACTION_SMOKE_CIGGY);
            Chat_Action(playerid, "vadi cigaretu i pali je upaljacem.");
            Inv_ShowHandHint(playerid, "dim", "bacate cigaretu");
        }
        case ITEM_FIRST_AID:
        {
            new Float:health;
            GetPlayerHealth(playerid, health);
            if (Account_SecondsSinceDamage(playerid) < FIRST_AID_COMBAT_LOCK_S)
            {
                Msg_Error(playerid, "Prva pomoc se ne moze koristiti minut posle primljene stete.");
                return false;
            }
            if (health >= FIRST_AID_MAX_HP)
            {
                Msg_Error(playerid, "Prva pomoc leci samo do %.0f HP. Za vise idite u bolnicu.", FIRST_AID_MAX_HP);
                return false;
            }
            SetPlayerHealth(playerid, Utils_FloatMin(health + FIRST_AID_HP, FIRST_AID_MAX_HP));
            Chat_Action(playerid, "otvara kutiju prve pomoci i previja se.");
        }
        case ITEM_PHONE:
        {
            Phone_ShowMenu(playerid);
            return false;
        }
        default:
        {
            Msg_Info(playerid, "Ovaj predmet se ne koristi direktno.");
            return false;
        }
    }

    if (g_ItemInfo[item][itHunger] > 0)
    {
        Hunger_Add(playerid, g_ItemInfo[item][itHunger]);
    }
    Inv_TakeFromSlot(playerid, slot, 1);
    return true;
}

stock Inv_ShowHandHint(playerid, const use[], const drop[])
{
    Msg_Info(playerid, "Levi klik: %s. F: %s.", use, drop);
}

// F throws away a cigarette or a bottle in hand; it returns true so the same press does not also open a door.
stock bool:Inv_OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys)
{
    if (!(newkeys & KEY_SECONDARY_ATTACK) || (oldkeys & KEY_SECONDARY_ATTACK))
    {
        return false;
    }

    new action[24];
    switch (GetPlayerSpecialAction(playerid))
    {
        case SPECIAL_ACTION_SMOKE_CIGGY: strcopy(action, "baca cigaretu.");
        case SPECIAL_ACTION_DRINK_BEER, SPECIAL_ACTION_DRINK_SPRUNK, SPECIAL_ACTION_DRINK_WINE: strcopy(action, "baca flasu.");
        default: return false;
    }
    SetPlayerSpecialAction(playerid, SPECIAL_ACTION_NONE);
    Chat_Action(playerid, action);
    return true;
}

stock Inv_Give(playerid, slot, targetid, amount)
{
    new E_ITEM:item = g_Inv[playerid][slot][isItem];
    if (!Chat_CheckBeside(playerid, targetid, INV_GIVE_RADIUS))
    {
        return;
    }
    if (amount < 1 || amount > g_Inv[playerid][slot][isAmount])
    {
        Msg_Error(playerid, "Nemate toliko komada.");
        return;
    }
    if (!Inv_CanAdd(targetid, item, amount))
    {
        Msg_Error(playerid, "Taj igrac nema mesta u inventaru.");
        return;
    }

    // Taken from the giver before the receiver gets it, so a failure in between can never leave two copies.
    Inv_TakeFromSlot(playerid, slot, amount);
    Inv_Add(targetid, item, amount);

    new action[96];
    format(action, sizeof(action), "daje %s (%d) osobi %s.", g_ItemInfo[item][itName], amount, Account_Name(targetid));
    Chat_Action(playerid, action);
    Msg_Info(targetid, "%s vam je dao: %s (%d).", Account_Name(playerid), g_ItemInfo[item][itName], amount);
}

stock Inv_Drop(playerid, slot)
{
    new drop = -1;
    for (new i = 0; i < MAX_DROPS; i++)
    {
        if (!g_Drops[i][drUsed])
        {
            drop = i;
            break;
        }
    }
    if (drop == -1)
    {
        Msg_Error(playerid, "Na mapi je trenutno previse bacenih predmeta, pokusajte kasnije.");
        return;
    }
    if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT)
    {
        Msg_Error(playerid, "Predmeti se bacaju samo peske.");
        return;
    }

    new E_ITEM:item = g_Inv[playerid][slot][isItem], amount = g_Inv[playerid][slot][isAmount];
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    Inv_TakeFromSlot(playerid, slot, amount);

    g_Drops[drop][drUsed] = true;
    g_Drops[drop][drItem] = item;
    g_Drops[drop][drAmount] = amount;
    g_Drops[drop][drX] = x;
    g_Drops[drop][drY] = y;
    g_Drops[drop][drZ] = z;
    g_Drops[drop][drInterior] = GetPlayerInterior(playerid);
    g_Drops[drop][drWorld] = GetPlayerVirtualWorld(playerid);
    g_Drops[drop][drCreatedAt] = gettime();

    new text[48];
    format(text, sizeof(text), "%s (%d)\n" C_GREY "/podigni", g_ItemInfo[item][itName], amount);
    g_Drops[drop][drObject] = CreateDynamicObject(g_ItemInfo[item][itModel], x, y, z - 0.95, 0.0, 0.0, 0.0, g_Drops[drop][drWorld], g_Drops[drop][drInterior]);
    g_Drops[drop][drLabel] = CreateDynamic3DTextLabel(text, COLOR_WHITE, x, y, z - 0.6, 5.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1,
        g_Drops[drop][drWorld], g_Drops[drop][drInterior]);

    ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.1, false, false, false, false, 0, SYNC_ALL);
    new action[64];
    format(action, sizeof(action), "baca %s na zemlju.", g_ItemInfo[item][itName]);
    Chat_Action(playerid, action);
}

stock Inv_DestroyDrop(drop)
{
    if (IsValidDynamicObject(g_Drops[drop][drObject])) DestroyDynamicObject(g_Drops[drop][drObject]);
    if (IsValidDynamic3DTextLabel(g_Drops[drop][drLabel])) DestroyDynamic3DTextLabel(g_Drops[drop][drLabel]);
    g_Drops[drop][drUsed] = false;
}

forward Inv_DropTick();
public Inv_DropTick()
{
    new now = gettime();
    for (new i = 0; i < MAX_DROPS; i++)
    {
        if (g_Drops[i][drUsed] && now - g_Drops[i][drCreatedAt] >= DROP_LIFETIME_S)
        {
            Inv_DestroyDrop(i);
        }
    }
    return 1;
}

stock bool:Inv_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
    switch (dialogid)
    {
        case DIALOG_INVENTORY:
        {
            if (!response || listitem < 0 || listitem >= INV_MAX_SLOTS)
            {
                return true;
            }
            new slot = g_InvListed[playerid][listitem];
            if (g_Inv[playerid][slot][isItem] == ITEM_NONE)
            {
                return true;
            }
            g_InvSelected[playerid] = slot;
            ShowPlayerDialog(playerid, DIALOG_INVENTORY_ITEM, DIALOG_STYLE_LIST, g_ItemInfo[g_Inv[playerid][slot][isItem]][itName],
                "Koristi\nDaj igracu\nBaci", "Izaberi", "Nazad");
        }
        case DIALOG_INVENTORY_ITEM:
        {
            new slot = g_InvSelected[playerid];
            if (!response)
            {
                Inv_Show(playerid);
                return true;
            }
            if (g_Inv[playerid][slot][isItem] == ITEM_NONE)
            {
                return true;
            }
            if (Inv_IsBusy(playerid))
            {
                Msg_Error(playerid, "Trenutno ne mozete koristiti inventar.");
                return true;
            }

            switch (listitem)
            {
                case 0: Inv_Use(playerid, slot);
                case 1:
                {
                    ShowPlayerDialog(playerid, DIALOG_INVENTORY_GIVE, DIALOG_STYLE_INPUT, "Daj igracu",
                        C_WHITE "Unesite ID igraca i kolicinu, na primer: " C_SERVER "5 1", "Daj", "Nazad");
                }
                case 2: Inv_Drop(playerid, slot);
            }
        }
        case DIALOG_INVENTORY_GIVE:
        {
            new slot = g_InvSelected[playerid], targetid, amount;
            if (!response || g_Inv[playerid][slot][isItem] == ITEM_NONE || Inv_IsBusy(playerid))
            {
                return true;
            }
            if (sscanf(inputtext, "ud", targetid, amount))
            {
                Msg_Error(playerid, "Unesite ID igraca i kolicinu, na primer: 5 1");
                return true;
            }
            if (Staff_CheckTarget(playerid, targetid, "Ne mozete dati predmet samom sebi."))
            {
                Inv_Give(playerid, slot, targetid, amount);
            }
        }
        default:
        {
            return false;
        }
    }
    return true;
}

cmd:inv(playerid, params[])
{
    Inv_Show(playerid);
    return 1;
}
alias:inv("inventar")

cmd:podigni(playerid, params[])
{
    if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT || Inv_IsBusy(playerid))
    {
        Msg_Error(playerid, "Trenutno ne mozete podizati predmete.");
        return 1;
    }

    new interior = GetPlayerInterior(playerid), world = GetPlayerVirtualWorld(playerid);
    for (new i = 0; i < MAX_DROPS; i++)
    {
        if (!g_Drops[i][drUsed] || g_Drops[i][drInterior] != interior || g_Drops[i][drWorld] != world
            || !IsPlayerInRangeOfPoint(playerid, DROP_PICKUP_RADIUS, g_Drops[i][drX], g_Drops[i][drY], g_Drops[i][drZ]))
        {
            continue;
        }

        new E_ITEM:item = g_Drops[i][drItem], amount = g_Drops[i][drAmount];
        if (!Inv_CanAdd(playerid, item, amount))
        {
            Msg_Error(playerid, "Nemate mesta u inventaru.");
            return 1;
        }
        // The drop is gone before the item is added, so two players can never pick up the same one.
        Inv_DestroyDrop(i);
        Inv_Add(playerid, item, amount);

        new action[64];
        format(action, sizeof(action), "podize %s sa zemlje.", g_ItemInfo[item][itName]);
        Chat_Action(playerid, action);
        return 1;
    }
    Msg_Error(playerid, "Pored vas nema bacenih predmeta.");
    return 1;
}

cmd:kupi(playerid, params[])
{
    if (!Point_Require(playerid, POINT_SHOP))
    {
        return 1;
    }

    new body[1024], line[96];
    strcopy(body, "Predmet\tCena\n");
    for (new E_ITEM:item = ITEM_PHONE; item < E_ITEM; item++)
    {
        if (g_ItemInfo[item][itShopAmount] == 1) format(line, sizeof(line), "%s\t$%d\n", g_ItemInfo[item][itName], g_ItemInfo[item][itPrice]);
        else format(line, sizeof(line), "%s (%d kom.)\t$%d\n", g_ItemInfo[item][itName], g_ItemInfo[item][itShopAmount], g_ItemInfo[item][itPrice]);
        strcat(body, line);
    }
    // Services come after the items; the SIM card and credit belong to the account, not the inventory.
    format(line, sizeof(line), "SIM kartica (broj telefona)\t$%d\nDopuna kredita za telefon\t$%d - $%d\n", PHONE_SIM_PRICE, g_PhoneTopups[0], g_PhoneTopups[sizeof(g_PhoneTopups) - 1]);
    strcat(body, line);
    ShowPlayerDialog(playerid, DIALOG_SHOP, DIALOG_STYLE_TABLIST_HEADERS, "Prodavnica 24/7", body, "Kupi", "Zatvori");
    return 1;
}

stock bool:Shop_OnDialogResponse(playerid, dialogid, response, listitem)
{
    if (dialogid != DIALOG_SHOP)
    {
        return false;
    }
    if (!response || !Point_Require(playerid, POINT_SHOP))
    {
        return true;
    }

    new E_ITEM:item = E_ITEM:(listitem + _:ITEM_PHONE);
    if (item >= E_ITEM)
    {
        switch (_:item - _:E_ITEM)
        {
            case 0: Phone_ShowSimMenu(playerid);
            case 1: Phone_ShowTopup(playerid, false);
        }
        return true;
    }
    if (item <= ITEM_NONE)
    {
        return true;
    }
    new amount = g_ItemInfo[item][itShopAmount];
    if (!Inv_CanAdd(playerid, item, amount))
    {
        Msg_Error(playerid, "Nemate mesta u inventaru za %s.", g_ItemInfo[item][itName]);
        return true;
    }

    new reason[48];
    format(reason, sizeof(reason), "kupovina: %s", g_ItemInfo[item][itName]);
    if (!Money_Take(playerid, g_ItemInfo[item][itPrice], reason))
    {
        return true;
    }
    Inv_Add(playerid, item, amount);
    Account_Save(playerid);
    Msg_Success(playerid, "Kupili ste: %s za $%d. Inventar: /inv", g_ItemInfo[item][itName], g_ItemInfo[item][itPrice]);

    if (item == ITEM_PHONE)
    {
        Phone_OnPhoneBought(playerid);
    }
    return true;
}
