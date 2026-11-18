// One set of menus for every place items are kept (a trunk, a house safe); each place only says who may use it and where its slots are.
// Included after the places themselves, so their slot arrays are known here.
static const g_StoreName[E_STORE_KIND][] = { "", "Gepek", "Sef" };
static const g_StoreFrom[E_STORE_KIND][] = { "", "iz gepeka", "iz sefa" };
static const g_StoreInto[E_STORE_KIND][] = { "", "u gepek", "u sef" };

// The place a player has open, by slot, and its DB id: the slot may hold another vehicle or house by the time a menu is answered.
new E_STORE_KIND:g_StoreKind[MAX_PLAYERS];
new g_StoreRef[MAX_PLAYERS];
new g_StoreDbId[MAX_PLAYERS];
new g_StoreListed[MAX_PLAYERS][STORE_MAX_SLOTS];
// Rows before the items (the safe's money) and the item rows; the row after them is "put".
new g_StoreExtraRows[MAX_PLAYERS];
new g_StoreRows[MAX_PLAYERS];
// The slot or inventory slot picked while the amount is asked for, and its item, which may be gone by then.
new g_StorePick[MAX_PLAYERS];
new E_ITEM:g_StorePickItem[MAX_PLAYERS];

stock Store_OnPlayerConnect(playerid)
{
    g_StoreKind[playerid] = STORE_NONE;
}

static stock Store_Size(E_STORE_KIND:kind)
{
    return kind == STORE_TRUNK ? TRUNK_MAX_SLOTS : SAFE_MAX_SLOTS;
}

static stock Store_Capacity(E_STORE_KIND:kind, ref)
{
    return kind == STORE_TRUNK ? Trunk_Capacity(g_Veh[ref][vModel]) : Safe_Capacity(ref);
}

static stock Store_DbId(E_STORE_KIND:kind, ref)
{
    return kind == STORE_TRUNK ? g_Veh[ref][vDbId] : g_Houses[ref][hDbId];
}

static stock Store_Get(E_STORE_KIND:kind, ref, index, &E_ITEM:item, &amount)
{
    if (kind == STORE_TRUNK)
    {
        item = g_Trunk[ref][index][isItem];
        amount = g_Trunk[ref][index][isAmount];
    }
    else
    {
        item = g_HouseSafe[ref][index][isItem];
        amount = g_HouseSafe[ref][index][isAmount];
    }
}

static stock Store_SaveSlot(E_STORE_KIND:kind, ref, index)
{
    if (kind == STORE_TRUNK)
    {
        Trunk_SaveSlot(ref, index);
    }
    else
    {
        Safe_SaveSlot(ref, index);
    }
}

static stock Store_Set(E_STORE_KIND:kind, ref, index, E_ITEM:item, amount)
{
    if (amount <= 0)
    {
        item = ITEM_NONE;
        amount = 0;
    }
    if (kind == STORE_TRUNK)
    {
        g_Trunk[ref][index][isItem] = item;
        g_Trunk[ref][index][isAmount] = amount;
    }
    else
    {
        g_HouseSafe[ref][index][isItem] = item;
        g_HouseSafe[ref][index][isAmount] = amount;
    }
    Store_SaveSlot(kind, ref, index);
}

static stock bool:Store_CanAdd(E_STORE_KIND:kind, ref, E_ITEM:item, amount)
{
    if (kind == STORE_TRUNK)
    {
        return Container_CanAdd(g_Trunk[ref], TRUNK_MAX_SLOTS, Store_Capacity(kind, ref), item, amount);
    }
    return Container_CanAdd(g_HouseSafe[ref], SAFE_MAX_SLOTS, Store_Capacity(kind, ref), item, amount);
}

static stock Store_Add(E_STORE_KIND:kind, ref, E_ITEM:item, amount)
{
    new changed;
    if (kind == STORE_TRUNK)
    {
        changed = Container_Add(g_Trunk[ref], TRUNK_MAX_SLOTS, Store_Capacity(kind, ref), item, amount);
    }
    else
    {
        changed = Container_Add(g_HouseSafe[ref], SAFE_MAX_SLOTS, Store_Capacity(kind, ref), item, amount);
    }
    for (new i = 0; i < Store_Size(kind); i++)
    {
        if (changed & (1 << i))
        {
            Store_SaveSlot(kind, ref, i);
        }
    }
}

stock Store_Count(E_STORE_KIND:kind, ref)
{
    new count, E_ITEM:item, amount;
    for (new i = 0; i < Store_Size(kind); i++)
    {
        Store_Get(kind, ref, i, item, amount);
        if (item != ITEM_NONE)
        {
            count++;
        }
    }
    return count;
}

// The item rows of a place, for its menu and for admins; listed[] maps each row back to a slot. Returns the row count.
stock Store_ListItems(E_STORE_KIND:kind, ref, body[], size)
{
    new listed[STORE_MAX_SLOTS];
    return Store_ListItemsInto(kind, ref, body, size, listed);
}

stock Store_ListItemsInto(E_STORE_KIND:kind, ref, body[], size, listed[STORE_MAX_SLOTS])
{
    new line[64], count, E_ITEM:item, amount;
    for (new i = 0; i < Store_Size(kind); i++)
    {
        Store_Get(kind, ref, i, item, amount);
        if (item == ITEM_NONE)
        {
            continue;
        }
        format(line, sizeof(line), "%s\t%d\n", g_ItemInfo[item][itName], amount);
        strcat(body, line, size);
        listed[count++] = i;
    }
    return count;
}

// Checked again on every dialog answer, since the place may have been closed, locked or left meanwhile.
static stock bool:Store_CanUse(playerid, bool:tell)
{
    new E_STORE_KIND:kind = g_StoreKind[playerid], ref = g_StoreRef[playerid];
    if (kind == STORE_NONE || Store_DbId(kind, ref) != g_StoreDbId[playerid])
    {
        return false;
    }
    if (Inv_IsBusy(playerid))
    {
        if (tell)
        {
            Msg_Error(playerid, "%s se koristi peske.", g_StoreName[kind]);
        }
        return false;
    }
    return kind == STORE_TRUNK ? Trunk_CanUse(playerid, ref, tell) : Safe_CanUse(playerid, ref, tell);
}

stock Store_Open(playerid, E_STORE_KIND:kind, ref)
{
    g_StoreKind[playerid] = kind;
    g_StoreRef[playerid] = ref;
    g_StoreDbId[playerid] = Store_DbId(kind, ref);
    if (Store_CanUse(playerid, true))
    {
        Store_Show(playerid);
    }
}

stock Store_Show(playerid)
{
    new E_STORE_KIND:kind = g_StoreKind[playerid], ref = g_StoreRef[playerid], body[1536], title[64];
    strcopy(body, "Predmet\tKolicina\n");
    g_StoreExtraRows[playerid] = kind == STORE_SAFE ? Safe_ListMoney(ref, body, sizeof(body)) : 0;
    g_StoreRows[playerid] = Store_ListItemsInto(kind, ref, body, sizeof(body), g_StoreListed[playerid]);
    strcat(body, "> Stavi predmet iz inventara\t");
    new what[40];
    if (kind == STORE_TRUNK)
    {
        format(what, sizeof(what), ": %s", Veh_ModelName(g_Veh[ref][vModel]));
    }
    format(title, sizeof(title), "%s%s (%d/%d mesta)", g_StoreName[kind], what, g_StoreRows[playerid], Store_Capacity(kind, ref));
    ShowPlayerDialog(playerid, DIALOG_STORE, DIALOG_STYLE_TABLIST_HEADERS, title, body, "Izaberi", "Zatvori");
}

static stock Store_ShowPut(playerid)
{
    new body[1024], line[64], count;
    strcopy(body, "Predmet\tKolicina\n");
    for (new slot = 0; slot < INV_MAX_SLOTS; slot++)
    {
        if (g_Inv[playerid][slot][isItem] == ITEM_NONE)
        {
            continue;
        }
        format(line, sizeof(line), "%s\t%d\n", g_ItemInfo[g_Inv[playerid][slot][isItem]][itName], g_Inv[playerid][slot][isAmount]);
        strcat(body, line);
        g_InvListed[playerid][count++] = slot;
    }
    if (count == 0)
    {
        Msg_Error(playerid, "Inventar je prazan.");
        Store_Show(playerid);
        return;
    }
    new title[32];
    format(title, sizeof(title), "Stavi %s", g_StoreInto[g_StoreKind[playerid]]);
    ShowPlayerDialog(playerid, DIALOG_STORE_PUT, DIALOG_STYLE_TABLIST_HEADERS, title, body, "Izaberi", "Nazad");
}

static stock Store_AskAmount(playerid, dialogid, max)
{
    new text[64];
    format(text, sizeof(text), "Kolicina (1-%d):", max);
    ShowPlayerDialog(playerid, dialogid, DIALOG_STYLE_INPUT, "Kolicina", text, "U redu", "Nazad");
}

static stock Store_Log(playerid, const action[], E_ITEM:item, amount)
{
    if (g_StoreKind[playerid] == STORE_TRUNK)
    {
        Trunk_Log(playerid, g_StoreRef[playerid], action, item, amount);
    }
    else
    {
        Safe_Log(playerid, g_StoreRef[playerid], action, item, amount);
    }
}

// Everything is checked again here: another player at the same place may have taken the item first.
static stock Store_Take(playerid, index, amount)
{
    if (!Store_CanUse(playerid, true))
    {
        return;
    }
    new E_STORE_KIND:kind = g_StoreKind[playerid], ref = g_StoreRef[playerid], E_ITEM:item, have;
    Store_Get(kind, ref, index, item, have);
    if (item != g_StorePickItem[playerid] || item == ITEM_NONE)
    {
        Msg_Error(playerid, "Tog predmeta vise nema.");
        Store_Show(playerid);
        return;
    }
    if (amount < 1 || amount > have)
    {
        Msg_Error(playerid, "Ima samo %d.", have);
        Store_Show(playerid);
        return;
    }
    if (!Inv_CanAdd(playerid, item, amount))
    {
        Msg_Error(playerid, "Nemate mesta u inventaru.");
        Store_Show(playerid);
        return;
    }
    Store_Set(kind, ref, index, item, have - amount);
    Inv_Add(playerid, item, amount);
    Store_Log(playerid, "take", item, amount);

    new action[64];
    format(action, sizeof(action), "vadi %s %s.", g_ItemInfo[item][itName], g_StoreFrom[kind]);
    Chat_Action(playerid, action);
    Store_Show(playerid);
}

static stock Store_Put(playerid, inv_slot, amount)
{
    if (!Store_CanUse(playerid, true))
    {
        return;
    }
    new E_STORE_KIND:kind = g_StoreKind[playerid], ref = g_StoreRef[playerid], E_ITEM:item = g_Inv[playerid][inv_slot][isItem];
    if (item != g_StorePickItem[playerid] || item == ITEM_NONE || amount < 1 || amount > g_Inv[playerid][inv_slot][isAmount])
    {
        Msg_Error(playerid, "Toliko tog predmeta nemate.");
        Store_Show(playerid);
        return;
    }
    if (Acc_IsAccessory(item) && Acc_IsWorn(playerid, item))
    {
        Msg_Error(playerid, "Prvo skinite %s (/dodaci).", g_ItemInfo[item][itName]);
        Store_Show(playerid);
        return;
    }
    if (!Store_CanAdd(kind, ref, item, amount))
    {
        Msg_Error(playerid, "Nema mesta.");
        Store_Show(playerid);
        return;
    }
    Inv_TakeFromSlot(playerid, inv_slot, amount);
    Store_Add(kind, ref, item, amount);
    Store_Log(playerid, "put", item, amount);

    new action[64];
    format(action, sizeof(action), "stavlja %s %s.", g_ItemInfo[item][itName], g_StoreInto[kind]);
    Chat_Action(playerid, action);
    Store_Show(playerid);
}

static stock Store_Back(playerid, bool:put_list)
{
    if (!Store_CanUse(playerid, true))
    {
        return;
    }
    if (put_list)
    {
        Store_ShowPut(playerid);
    }
    else
    {
        Store_Show(playerid);
    }
}

stock bool:Store_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
    switch (dialogid)
    {
        case DIALOG_STORE:
        {
            if (!response || !Store_CanUse(playerid, true) || listitem < 0)
            {
                return true;
            }
            new extra = g_StoreExtraRows[playerid], rows = g_StoreRows[playerid];
            if (listitem < extra)
            {
                Safe_OnMoneyRow(playerid, g_StoreRef[playerid]);
                return true;
            }
            if (listitem == extra + rows)
            {
                Store_ShowPut(playerid);
                return true;
            }
            if (listitem > extra + rows)
            {
                return true;
            }
            new index = g_StoreListed[playerid][listitem - extra], E_ITEM:item, amount;
            Store_Get(g_StoreKind[playerid], g_StoreRef[playerid], index, item, amount);
            g_StorePickItem[playerid] = item;
            if (amount == 1 || Item_IsWhole(item))
            {
                Store_Take(playerid, index, amount);
                return true;
            }
            g_StorePick[playerid] = index;
            Store_AskAmount(playerid, DIALOG_STORE_TAKE, amount);
        }
        case DIALOG_STORE_TAKE:
        {
            if (!response)
            {
                Store_Back(playerid, false);
                return true;
            }
            Store_Take(playerid, g_StorePick[playerid], strval(inputtext));
        }
        case DIALOG_STORE_PUT:
        {
            if (!response || listitem < 0 || listitem >= INV_MAX_SLOTS)
            {
                Store_Back(playerid, false);
                return true;
            }
            new inv_slot = g_InvListed[playerid][listitem];
            g_StorePickItem[playerid] = g_Inv[playerid][inv_slot][isItem];
            if (g_Inv[playerid][inv_slot][isAmount] == 1 || Item_IsWhole(g_Inv[playerid][inv_slot][isItem]))
            {
                Store_Put(playerid, inv_slot, g_Inv[playerid][inv_slot][isAmount]);
                return true;
            }
            g_StorePick[playerid] = inv_slot;
            Store_AskAmount(playerid, DIALOG_STORE_PUT_AMOUNT, g_Inv[playerid][inv_slot][isAmount]);
        }
        case DIALOG_STORE_PUT_AMOUNT:
        {
            if (!response)
            {
                Store_Back(playerid, true);
                return true;
            }
            Store_Put(playerid, g_StorePick[playerid], strval(inputtext));
        }
        default:
        {
            return false;
        }
    }
    return true;
}
