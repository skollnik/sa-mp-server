// Everything an admin can give extra room for; a new system adds a row here and a case in Slots_Extra and Slots_SetExtra.
enum E_SLOT_KIND
{
    SLOT_VEHICLES,
    SLOT_HOUSES,
    SLOT_FURNITURE,
    SLOT_BUSINESSES
}

enum E_SLOT_INFO
{
    slWord[12],
    slColumn[24],
    slMax,
    slWhat[40]
}

static const g_SlotInfo[E_SLOT_KIND][E_SLOT_INFO] =
{
    { "vozila",   "extra_vehicle_slots",   VEH_ADMIN_EXTRA_MAX,   "mesta za vozila (/v)" },
    { "kuce",     "extra_house_slots",     HOUSE_ADMIN_EXTRA_MAX, "mesta za kuce (/kuca)" },
    { "namestaj", "extra_furniture_slots", FURN_ADMIN_EXTRA_MAX,  "komada namestaja po kuci" },
    { "firme",    "extra_business_slots",  BIZ_ADMIN_EXTRA_MAX,   "mesta za firme (/firma)" }
};

static stock Slots_SetExtra(playerid, E_SLOT_KIND:kind, extra)
{
    switch (kind)
    {
        case SLOT_VEHICLES: g_Player[playerid][pExtraVehSlots] = extra;
        case SLOT_HOUSES: g_Player[playerid][pExtraHouseSlots] = extra;
        case SLOT_BUSINESSES: g_Player[playerid][pExtraBizSlots] = extra;
        default: g_Player[playerid][pExtraFurnSlots] = extra;
    }
}

static stock bool:Slots_FindKind(const word[], &E_SLOT_KIND:kind)
{
    for (new E_SLOT_KIND:i = SLOT_VEHICLES; i < E_SLOT_KIND; i++)
    {
        if (!strcmp(word, g_SlotInfo[i][slWord], true))
        {
            kind = i;
            return true;
        }
    }
    return false;
}

// Taking slots back takes nothing away, same as when VIP ends: only nothing new fits until the player is under the limit.
cmd:setmesta(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }
    new targetid, word[12], extra, E_SLOT_KIND:kind;
    if (sscanf(params, "us[12]d", targetid, word, extra) || !Slots_FindKind(word, kind))
    {
        Msg_Usage(playerid, "/setmesta [id ili ime] [vozila|kuce|namestaj|firme] [broj] (dodatna mesta, pored obicnih i VIP)");
        return 1;
    }
    if (extra < 0 || extra > g_SlotInfo[kind][slMax])
    {
        Msg_Error(playerid, "Dodatnih %s moze biti od 0 do %d.", g_SlotInfo[kind][slWhat], g_SlotInfo[kind][slMax]);
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }
    Slots_SetExtra(targetid, kind, extra);
    new query[128], details[48];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `accounts` SET `%s` = %d WHERE `id` = %d", g_SlotInfo[kind][slColumn], extra, g_Player[targetid][pDbId]);
    mysql_tquery(g_SQL, query);

    format(details, sizeof(details), "%s: %d", g_SlotInfo[kind][slWord], extra);
    Staff_Log(playerid, "setslots", targetid, details);
    Msg_Admin(playerid, "Igrac %s sada ima %d dodatnih %s.", Account_Name(targetid), extra, g_SlotInfo[kind][slWhat]);
    if (targetid != playerid)
    {
        Msg_Info(targetid, "Admin %s vam je podesio dodatna %s: %d.", Account_Name(playerid), g_SlotInfo[kind][slWhat], extra);
    }
    return 1;
}
