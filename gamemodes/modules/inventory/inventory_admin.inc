#define INV_ADMIN_MAX_AMOUNT    (100)

cmd:proveriinv(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_ADMIN))
    {
        return 1;
    }
    new targetid = Staff_ParseTarget(playerid, params, "/proveriinv [id ili ime]");
    if (targetid == INVALID_PLAYER_ID)
    {
        return 1;
    }
    new body[1024], line[64], count;
    strcopy(body, "Mesto\tPredmet\tKolicina\n");
    for (new slot = 0; slot < INV_MAX_SLOTS; slot++)
    {
        new E_ITEM:item = g_Inv[targetid][slot][isItem];
        if (item == ITEM_NONE)
        {
            continue;
        }
        format(line, sizeof(line), "%d\t%s\t%d\n", slot + 1, g_ItemInfo[item][itName], g_Inv[targetid][slot][isAmount]);
        strcat(body, line);
        count++;
    }
    if (count == 0)
    {
        Msg_Admin(playerid, "Inventar igraca %s je prazan.", Account_Name(targetid));
        return 1;
    }
    new title[64];
    format(title, sizeof(title), "Inventar: %s", Account_Name(targetid));
    ShowPlayerDialog(playerid, DIALOG_ADMIN_INFO, DIALOG_STYLE_TABLIST_HEADERS, title, body, "Zatvori", "");
    return 1;
}

// Without an item a list of items opens, so nobody has to know the item numbers.
static stock InvAdmin_ShowItems(playerid, targetid, bool:give)
{
    new items[_:E_ITEM][E_MENU_ITEM], count, subject[8], title[64];
    for (new E_ITEM:item = E_ITEM:1; item < E_ITEM; item++)
    {
        if (!give && !Inv_Has(targetid, item))
        {
            continue;
        }
        strcopy(items[count][miLabel], g_ItemInfo[item][itName]);
        format(items[count][miCommand], MENU_COMMAND_LENGTH, "%s %d %d", give ? ("dajpredmet") : ("uzmipredmet"), targetid, _:item);
        strcopy(items[count][miPrompt], "Kolicina:");
        count++;
    }
    if (count == 0)
    {
        Msg_Error(playerid, "Igrac nema predmete.");
        return;
    }
    format(title, sizeof(title), "%s: %s", give ? ("Daj predmet") : ("Uzmi predmet"), Account_Name(targetid));
    Menu_Show(playerid, title, items, count, subject);
}

static stock InvAdmin_Change(playerid, const params[], bool:give)
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return;
    }
    new targetid, item, amount;
    if (sscanf(params, "udd", targetid, item, amount))
    {
        if (!sscanf(params, "u", targetid) && Staff_CheckTarget(playerid, targetid))
        {
            InvAdmin_ShowItems(playerid, targetid, give);
            return;
        }
        Msg_Usage(playerid, "/%s [id ili ime] [predmet] [kolicina] (bez predmeta: lista)", give ? ("dajpredmet") : ("uzmipredmet"));
        return;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return;
    }
    if (item < 1 || item >= _:E_ITEM || amount < 1 || amount > INV_ADMIN_MAX_AMOUNT)
    {
        Msg_Error(playerid, "Nepostojeci predmet ili kolicina van 1-%d.", INV_ADMIN_MAX_AMOUNT);
        return;
    }
    new E_ITEM:what = E_ITEM:item, details[64];
    if (give)
    {
        if (!Inv_CanAdd(targetid, what, amount))
        {
            Msg_Error(playerid, "Igrac nema mesta u inventaru.");
            return;
        }
        Inv_Add(targetid, what, amount);
    }
    else if (!Inv_Remove(targetid, what, amount))
    {
        Msg_Error(playerid, "Igrac nema toliko tog predmeta (ima %d).", Inv_Count(targetid, what));
        return;
    }
    format(details, sizeof(details), "%s %dx %s", give ? ("vam je dao") : ("vam je uzeo"), amount, g_ItemInfo[what][itName]);
    Staff_NotifyTarget(playerid, targetid, details);
    format(details, sizeof(details), "%s %dx %s", give ? ("daje") : ("uzima"), amount, g_ItemInfo[what][itName]);
    Staff_Log(playerid, give ? ("giveitem") : ("takeitem"), targetid, details);
    Msg_Admin(playerid, "Igrac %s: %s.", Account_Name(targetid), details);
}

cmd:dajpredmet(playerid, params[])
{
    InvAdmin_Change(playerid, params, true);
    return 1;
}

cmd:uzmipredmet(playerid, params[])
{
    InvAdmin_Change(playerid, params, false);
    return 1;
}
