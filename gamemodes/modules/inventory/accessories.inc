stock Acc_OnPlayerConnect(playerid)
{
    for (new E_ACC_PART:part; part < E_ACC_PART; part++)
    {
        g_AccWorn[playerid][part] = -1;
    }
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
        g_AccInfo[index][aiX], g_AccInfo[index][aiY], g_AccInfo[index][aiZ],
        g_AccInfo[index][aiRotX], g_AccInfo[index][aiRotY], g_AccInfo[index][aiRotZ],
        g_AccInfo[index][aiScaleX], g_AccInfo[index][aiScaleY], g_AccInfo[index][aiScaleZ]);
}

// Attachments are put back on every spawn, since the client can drop them while the player is not spawned yet.
// Empty parts are cleared too: after a server restart the client keeps showing what was worn before.
stock Acc_ApplyAll(playerid)
{
    for (new E_ACC_PART:part; part < E_ACC_PART; part++)
    {
        if (g_AccWorn[playerid][part] != -1)
        {
            Acc_Attach(playerid, g_AccWorn[playerid][part]);
        }
        else
        {
            RemovePlayerAttachedObject(playerid, ACC_FIRST_SLOT + _:part);
        }
    }
}

stock Acc_OnPlayerSpawn(playerid)
{
    Acc_ApplyAll(playerid);
}

static stock Acc_SaveWorn(playerid)
{
    new bits;
    for (new E_ACC_PART:part; part < E_ACC_PART; part++)
    {
        if (g_AccWorn[playerid][part] != -1)
        {
            bits |= 1 << g_AccWorn[playerid][part];
        }
    }
    g_Player[playerid][pAccWornBits] = bits;
    Account_Save(playerid);
}

// Called once the inventory is in memory, so a saved accessory goes back on only if the player still has it.
stock Acc_OnInventoryLoaded(playerid)
{
    for (new i = 0; i < sizeof(g_AccInfo); i++)
    {
        if ((g_Player[playerid][pAccWornBits] & (1 << i)) && Inv_Has(playerid, g_AccInfo[i][aiItem]))
        {
            g_AccWorn[playerid][g_AccInfo[i][aiPart]] = i;
        }
    }
    Acc_ApplyAll(playerid);
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
    g_AccWorn[playerid][part] = -1;
    RemovePlayerAttachedObject(playerid, ACC_FIRST_SLOT + _:part);
    Acc_SaveWorn(playerid);
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
    Acc_SaveWorn(playerid);

    new name[24], action[48];
    Acc_LowerName(index, name);
    format(action, sizeof(action), "stavlja %s.", name);
    Chat_Action(playerid, action);
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
    ShowPlayerDialog(playerid, DIALOG_ACC_LIST, DIALOG_STYLE_TABLIST_HEADERS, "Dodaci na telu (klik stavlja ili skida)", body, "Izaberi", "Zatvori");
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
            if (!response || listitem < 0 || listitem >= sizeof(g_AccInfo))
            {
                return true;
            }
            new index = g_AccListed[playerid][listitem];
            if (!Inv_Has(playerid, g_AccInfo[index][aiItem]))
            {
                return true;
            }
            if (Inv_IsBusy(playerid))
            {
                Msg_Error(playerid, "Trenutno ne mozete menjati dodatke.");
                return true;
            }
            Acc_Toggle(playerid, g_AccInfo[index][aiItem]);
            Acc_ShowList(playerid);
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
