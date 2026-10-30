stock Acc_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `player_accessories` (" \
        "`account_id` INT UNSIGNED NOT NULL," \
        "`item` SMALLINT UNSIGNED NOT NULL," \
        "`x` FLOAT NOT NULL, `y` FLOAT NOT NULL, `z` FLOAT NOT NULL," \
        "`rx` FLOAT NOT NULL, `ry` FLOAT NOT NULL, `rz` FLOAT NOT NULL," \
        "`sx` FLOAT NOT NULL, `sy` FLOAT NOT NULL, `sz` FLOAT NOT NULL," \
        "`worn` TINYINT UNSIGNED NOT NULL DEFAULT 0," \
        "PRIMARY KEY (`account_id`, `item`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
}

stock Acc_OnPlayerConnect(playerid)
{
    for (new i = 0; i < sizeof(g_AccInfo); i++)
    {
        for (new E_ACC_PLACE:p; p < E_ACC_PLACE; p++)
        {
            g_AccPlace[playerid][i][p] = g_AccInfo[i][aiPlace][p];
        }
    }
    for (new E_ACC_PART:part; part < E_ACC_PART; part++)
    {
        g_AccWorn[playerid][part] = -1;
    }
    g_AccEditing[playerid] = -1;
}

stock Acc_IndexOf(E_ITEM:item)
{
    for (new i = 0; i < sizeof(g_AccInfo); i++)
    {
        if (g_AccInfo[i][aiItem] == item)
        {
            return i;
        }
    }
    return -1;
}

stock bool:Acc_IsAccessory(E_ITEM:item)
{
    return Acc_IndexOf(item) != -1;
}

stock bool:Acc_IsWorn(playerid, E_ITEM:item)
{
    new index = Acc_IndexOf(item);
    return index != -1 && g_AccWorn[playerid][g_AccInfo[index][aiPart]] == index;
}

static stock Acc_Attach(playerid, index)
{
    SetPlayerAttachedObject(playerid, ACC_FIRST_SLOT + _:g_AccInfo[index][aiPart], g_ItemInfo[g_AccInfo[index][aiItem]][itModel], g_AccInfo[index][aiBone],
        g_AccPlace[playerid][index][apX], g_AccPlace[playerid][index][apY], g_AccPlace[playerid][index][apZ],
        g_AccPlace[playerid][index][apRotX], g_AccPlace[playerid][index][apRotY], g_AccPlace[playerid][index][apRotZ],
        g_AccPlace[playerid][index][apScaleX], g_AccPlace[playerid][index][apScaleY], g_AccPlace[playerid][index][apScaleZ]);
}

// Attachments are put back on every spawn, since the client can drop them while the player is not spawned yet.
stock Acc_ApplyAll(playerid)
{
    for (new E_ACC_PART:part; part < E_ACC_PART; part++)
    {
        if (g_AccWorn[playerid][part] != -1)
        {
            Acc_Attach(playerid, g_AccWorn[playerid][part]);
        }
    }
}

stock Acc_OnPlayerSpawn(playerid)
{
    Acc_ApplyAll(playerid);
}

// One row per owned accessory that was ever worn or moved; a missing row means the starting place.
static stock Acc_Save(playerid, index)
{
    new query[512];
    mysql_format(g_SQL, query, sizeof(query),
        "INSERT INTO `player_accessories` (`account_id`, `item`, `x`, `y`, `z`, `rx`, `ry`, `rz`, `sx`, `sy`, `sz`, `worn`) " \
        "VALUES (%d, %d, %f, %f, %f, %f, %f, %f, %f, %f, %f, %d) ON DUPLICATE KEY UPDATE " \
        "`x` = VALUES(`x`), `y` = VALUES(`y`), `z` = VALUES(`z`), `rx` = VALUES(`rx`), `ry` = VALUES(`ry`), `rz` = VALUES(`rz`), " \
        "`sx` = VALUES(`sx`), `sy` = VALUES(`sy`), `sz` = VALUES(`sz`), `worn` = VALUES(`worn`)",
        g_Player[playerid][pDbId], _:g_AccInfo[index][aiItem],
        g_AccPlace[playerid][index][apX], g_AccPlace[playerid][index][apY], g_AccPlace[playerid][index][apZ],
        g_AccPlace[playerid][index][apRotX], g_AccPlace[playerid][index][apRotY], g_AccPlace[playerid][index][apRotZ],
        g_AccPlace[playerid][index][apScaleX], g_AccPlace[playerid][index][apScaleY], g_AccPlace[playerid][index][apScaleZ],
        g_AccWorn[playerid][g_AccInfo[index][aiPart]] == index);
    mysql_tquery(g_SQL, query);
}

// Called once the inventory is in memory, so a worn flag is honoured only for an item the player still has.
stock Acc_OnInventoryLoaded(playerid)
{
    new query[96];
    mysql_format(g_SQL, query, sizeof(query), "SELECT * FROM `player_accessories` WHERE `account_id` = %d", g_Player[playerid][pDbId]);
    mysql_tquery(g_SQL, query, "OnAccessoriesLoaded", "dd", playerid, g_AccountRace[playerid]);
}

forward OnAccessoriesLoaded(playerid, race);
public OnAccessoriesLoaded(playerid, race)
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }

    new rows = cache_num_rows(), item, index, worn;
    for (new row = 0; row < rows; row++)
    {
        cache_get_value_name_int(row, "item", item);
        index = Acc_IndexOf(E_ITEM:item);
        if (index == -1)
        {
            continue;
        }
        cache_get_value_name_float(row, "x", g_AccPlace[playerid][index][apX]);
        cache_get_value_name_float(row, "y", g_AccPlace[playerid][index][apY]);
        cache_get_value_name_float(row, "z", g_AccPlace[playerid][index][apZ]);
        cache_get_value_name_float(row, "rx", g_AccPlace[playerid][index][apRotX]);
        cache_get_value_name_float(row, "ry", g_AccPlace[playerid][index][apRotY]);
        cache_get_value_name_float(row, "rz", g_AccPlace[playerid][index][apRotZ]);
        cache_get_value_name_float(row, "sx", g_AccPlace[playerid][index][apScaleX]);
        cache_get_value_name_float(row, "sy", g_AccPlace[playerid][index][apScaleY]);
        cache_get_value_name_float(row, "sz", g_AccPlace[playerid][index][apScaleZ]);
        cache_get_value_name_int(row, "worn", worn);
        if (worn && Inv_Has(playerid, E_ITEM:item))
        {
            g_AccWorn[playerid][g_AccInfo[index][aiPart]] = index;
        }
    }
    Acc_ApplyAll(playerid);
    return 1;
}

static stock Acc_LowerName(index, name[], size = sizeof(name))
{
    strcopy(name, g_ItemInfo[g_AccInfo[index][aiItem]][itName], size);
    name[0] = tolower(name[0]);
}

static stock Acc_TakeOff(playerid, index, bool:announce)
{
    new E_ACC_PART:part = g_AccInfo[index][aiPart];
    if (g_AccWorn[playerid][part] != index)
    {
        return;
    }
    if (g_AccEditing[playerid] == index)
    {
        g_AccEditing[playerid] = -1;
    }
    g_AccWorn[playerid][part] = -1;
    RemovePlayerAttachedObject(playerid, ACC_FIRST_SLOT + _:part);
    Acc_Save(playerid, index);
    if (announce)
    {
        new name[24], action[48];
        Acc_LowerName(index, name);
        format(action, sizeof(action), "skida %s.", name);
        Chat_Action(playerid, action);
    }
}

// Wearing something on a part that is taken swaps the two.
stock Acc_Toggle(playerid, E_ITEM:item)
{
    new index = Acc_IndexOf(item);
    if (index == -1)
    {
        return;
    }
    new E_ACC_PART:part = g_AccInfo[index][aiPart];
    if (g_AccWorn[playerid][part] == index)
    {
        Acc_TakeOff(playerid, index, true);
        return;
    }
    if (g_AccWorn[playerid][part] != -1)
    {
        Acc_TakeOff(playerid, g_AccWorn[playerid][part], false);
    }

    g_AccWorn[playerid][part] = index;
    Acc_Attach(playerid, index);
    Acc_Save(playerid, index);

    new name[24], action[48];
    Acc_LowerName(index, name);
    format(action, sizeof(action), "stavlja %s.", name);
    Chat_Action(playerid, action);
    Msg_Info(playerid, "Polozaj na telu podesavate u /dodaci.");
}

// The last piece left the inventory (given, dropped), so it cannot stay on the body.
stock Acc_OnItemGone(playerid, E_ITEM:item)
{
    new index = Acc_IndexOf(item);
    if (index != -1)
    {
        Acc_TakeOff(playerid, index, false);
    }
}

stock Acc_ShowShop(playerid)
{
    new body[640], line[64];
    strcopy(body, "Predmet\tDeo tela\tCena\n");
    for (new i = 0; i < sizeof(g_AccInfo); i++)
    {
        format(line, sizeof(line), "%s\t%s\t$%d\n", g_ItemInfo[g_AccInfo[i][aiItem]][itName], g_AccPartNames[g_AccInfo[i][aiPart]], g_AccInfo[i][aiPrice]);
        strcat(body, line);
    }
    ShowPlayerDialog(playerid, DIALOG_ACC_SHOP, DIALOG_STYLE_TABLIST_HEADERS, "Prodavnica odece", body, "Kupi", "Zatvori");
}

static stock Acc_Buy(playerid, index)
{
    new E_ITEM:item = g_AccInfo[index][aiItem];
    if (!Inv_CanAdd(playerid, item, 1))
    {
        Msg_Error(playerid, "Nemate mesta u inventaru.");
        return;
    }
    new reason[48];
    format(reason, sizeof(reason), "kupovina: %s", g_ItemInfo[item][itName]);
    if (!Money_Take(playerid, g_AccInfo[index][aiPrice], reason))
    {
        return;
    }
    Inv_Add(playerid, item, 1);
    Account_Save(playerid);
    Msg_Success(playerid, "Kupili ste: %s za $%d. Stavlja se u /dodaci ili /inv.", g_ItemInfo[item][itName], g_AccInfo[index][aiPrice]);
}

stock Acc_ShowList(playerid)
{
    new body[640], line[80], count;
    strcopy(body, "Predmet\tDeo tela\tStanje\n");
    for (new i = 0; i < sizeof(g_AccInfo); i++)
    {
        if (!Inv_Has(playerid, g_AccInfo[i][aiItem]))
        {
            continue;
        }
        format(line, sizeof(line), "%s\t%s\t%s\n", g_ItemInfo[g_AccInfo[i][aiItem]][itName], g_AccPartNames[g_AccInfo[i][aiPart]],
            g_AccWorn[playerid][g_AccInfo[i][aiPart]] == i ? (C_GREEN "na vama") : (C_GREY "u inventaru"));
        strcat(body, line);
        g_AccListed[playerid][count++] = i;
    }
    if (count == 0)
    {
        Msg_Info(playerid, "Nemate dodatke (naocare, kape, ranac, torba). Kupuju se u prodavnici odece (/kupi).");
        return;
    }
    ShowPlayerDialog(playerid, DIALOG_ACC_LIST, DIALOG_STYLE_TABLIST_HEADERS, "Dodaci na telu", body, "Izaberi", "Zatvori");
}

static stock Acc_ShowItem(playerid, index)
{
    g_AccSelected[playerid] = index;
    new body[128];
    format(body, sizeof(body), "%s\nPodesi polozaj (misem)\nVrati na pocetni polozaj",
        g_AccWorn[playerid][g_AccInfo[index][aiPart]] == index ? ("Skini") : ("Stavi"));
    ShowPlayerDialog(playerid, DIALOG_ACC_ITEM, DIALOG_STYLE_LIST, g_ItemInfo[g_AccInfo[index][aiItem]][itName], body, "Izaberi", "Nazad");
}

static stock Acc_StartEdit(playerid, index)
{
    if (g_AccWorn[playerid][g_AccInfo[index][aiPart]] != index)
    {
        Msg_Error(playerid, "Prvo stavite taj predmet na sebe.");
        return;
    }
    g_AccEditing[playerid] = index;
    EditAttachedObject(playerid, ACC_FIRST_SLOT + _:g_AccInfo[index][aiPart]);
    Msg_Info(playerid, "Pomerajte predmet strelicama. Disketa cuva polozaj, ESC odustaje.");
}

static stock Acc_ResetPlace(playerid, index)
{
    for (new E_ACC_PLACE:p; p < E_ACC_PLACE; p++)
    {
        g_AccPlace[playerid][index][p] = g_AccInfo[index][aiPlace][p];
    }
    if (g_AccWorn[playerid][g_AccInfo[index][aiPart]] == index)
    {
        Acc_Attach(playerid, index);
    }
    Acc_Save(playerid, index);
    Msg_Success(playerid, "%s je vracen na pocetni polozaj.", g_ItemInfo[g_AccInfo[index][aiItem]][itName]);
}

static stock Float:Acc_Clamp(Float:value, Float:low, Float:high, &bool:clamped)
{
    if (value < low)
    {
        clamped = true;
        return low;
    }
    if (value > high)
    {
        clamped = true;
        return high;
    }
    return value;
}

stock Acc_OnPlayerEditAttached(playerid, EDIT_RESPONSE:response, slot, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, Float:sx, Float:sy, Float:sz)
{
    new index = g_AccEditing[playerid];
    if (index == -1 || slot != ACC_FIRST_SLOT + _:g_AccInfo[index][aiPart] || g_AccWorn[playerid][g_AccInfo[index][aiPart]] != index)
    {
        return;
    }
    g_AccEditing[playerid] = -1;
    if (response != EDIT_RESPONSE_FINAL)
    {
        // The client already moved the object while editing; putting the saved place back undoes that.
        Acc_Attach(playerid, index);
        return;
    }

    new bool:clamped;
    g_AccPlace[playerid][index][apX] = Acc_Clamp(x, -ACC_MAX_OFFSET, ACC_MAX_OFFSET, clamped);
    g_AccPlace[playerid][index][apY] = Acc_Clamp(y, -ACC_MAX_OFFSET, ACC_MAX_OFFSET, clamped);
    g_AccPlace[playerid][index][apZ] = Acc_Clamp(z, -ACC_MAX_OFFSET, ACC_MAX_OFFSET, clamped);
    g_AccPlace[playerid][index][apRotX] = rx;
    g_AccPlace[playerid][index][apRotY] = ry;
    g_AccPlace[playerid][index][apRotZ] = rz;
    g_AccPlace[playerid][index][apScaleX] = Acc_Clamp(sx, ACC_MIN_SCALE, ACC_MAX_SCALE, clamped);
    g_AccPlace[playerid][index][apScaleY] = Acc_Clamp(sy, ACC_MIN_SCALE, ACC_MAX_SCALE, clamped);
    g_AccPlace[playerid][index][apScaleZ] = Acc_Clamp(sz, ACC_MIN_SCALE, ACC_MAX_SCALE, clamped);
    Acc_Attach(playerid, index);
    Acc_Save(playerid, index);

    if (clamped)
    {
        Msg_Info(playerid, "Polozaj je sacuvan, ali predmet je vracen blize telu ili na dozvoljenu velicinu.");
    }
    else
    {
        Msg_Success(playerid, "Polozaj je sacuvan.");
    }
}

stock bool:Acc_OnDialogResponse(playerid, dialogid, response, listitem)
{
    switch (dialogid)
    {
        case DIALOG_ACC_SHOP:
        {
            if (response && listitem >= 0 && listitem < sizeof(g_AccInfo) && Point_Require(playerid, POINT_CLOTHES))
            {
                Acc_Buy(playerid, listitem);
            }
        }
        case DIALOG_ACC_LIST:
        {
            if (response && listitem >= 0 && listitem < sizeof(g_AccInfo))
            {
                Acc_ShowItem(playerid, g_AccListed[playerid][listitem]);
            }
        }
        case DIALOG_ACC_ITEM:
        {
            new index = g_AccSelected[playerid];
            if (!response)
            {
                Acc_ShowList(playerid);
                return true;
            }
            if (!Inv_Has(playerid, g_AccInfo[index][aiItem]))
            {
                return true;
            }
            if (Inv_IsBusy(playerid))
            {
                Msg_Error(playerid, "Trenutno ne mozete menjati dodatke.");
                return true;
            }
            switch (listitem)
            {
                case 0: Acc_Toggle(playerid, g_AccInfo[index][aiItem]);
                case 1: Acc_StartEdit(playerid, index);
                case 2: Acc_ResetPlace(playerid, index);
            }
        }
        default:
        {
            return false;
        }
    }
    return true;
}

cmd:dodaci(playerid, params[])
{
    if (!g_InvLoaded[playerid])
    {
        Msg_Error(playerid, "Inventar se jos ucitava, pokusajte ponovo.");
        return 1;
    }
    Acc_ShowList(playerid);
    return 1;
}
alias:dodaci("accessories")
