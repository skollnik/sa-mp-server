#define PLATE_CUSTOM_MIN        (2)
#define PLATE_CUSTOM_MAX        (8)

enum E_PLATE_PURPOSE
{
    PLATE_FIRST,
    PLATE_NEW,
    PLATE_CUSTOM,
    // An admin replaces an unfitting plate for free; it is mounted right away.
    PLATE_ADMIN
}

new g_PlateListed[MAX_PLAYERS][VEH_MAX_PER_PLAYER * 2];
new g_PlateSlot[MAX_PLAYERS];

static stock Plate_Price(playerid, E_PLATE_PURPOSE:purpose)
{
    switch (purpose)
    {
        case PLATE_FIRST: return PLATE_FIRST_PRICE;
        case PLATE_NEW: return PLATE_NEW_PRICE;
        case PLATE_CUSTOM: return Vip_GetLevel(playerid) >= VIP_GOLD ? PLATE_CUSTOM_PRICE / 2 : PLATE_CUSTOM_PRICE;
        default: return 0;
    }
    return 0;
}

// Plates of offline owners and plates waiting at the counter both count, so a plate is checked in the database.
static stock Plate_Check(playerid, dbid, E_PLATE_PURPOSE:purpose, const plate[])
{
    new query[160];
    mysql_format(g_SQL, query, sizeof(query), "SELECT `id` FROM `vehicles` WHERE `plate` = '%e' OR `pending_plate` = '%e'", plate, plate);
    mysql_tquery(g_SQL, query, "OnPlateChecked", "dddds", playerid, g_AccountRace[playerid], dbid, _:purpose, plate);
}

static stock Plate_RequestRandom(playerid, dbid, E_PLATE_PURPOSE:purpose)
{
    new plate[VEH_PLATE_LENGTH];
    Veh_RandomPlate(plate);
    Plate_Check(playerid, dbid, purpose, plate);
}

// The plate only shows on a freshly created vehicle, so mounting builds it again where it stands.
static stock Plate_Mount(slot, const plate[])
{
    strcopy(g_Veh[slot][vPlate], plate, VEH_PLATE_LENGTH);
    g_Veh[slot][vPendingPlate][0] = '\0';
    new query[128];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `vehicles` SET `plate` = '%e', `pending_plate` = NULL WHERE `id` = %d", plate, g_Veh[slot][vDbId]);
    mysql_tquery(g_SQL, query);
    Veh_Rebuild(slot);
}

forward OnPlateChecked(playerid, race, dbid, purpose, const plate[]);
public OnPlateChecked(playerid, race, dbid, purpose, const plate[])
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }
    new E_PLATE_PURPOSE:kind = E_PLATE_PURPOSE:purpose;
    if (cache_num_rows() > 0)
    {
        if (kind == PLATE_CUSTOM)
        {
            Msg_Error(playerid, "Tablica %s je zauzeta. Izaberite drugu.", plate);
        }
        else
        {
            Plate_RequestRandom(playerid, dbid, kind);
        }
        return 1;
    }
    new slot = Veh_FindByDbId(dbid);
    if (slot == -1)
    {
        return 1;
    }

    if (kind == PLATE_ADMIN)
    {
        new ownerid = Account_FindOnlineByDbId(g_Veh[slot][vOwnerDbId]), details[64];
        format(details, sizeof(details), "vozilo %d: %s -> %s", dbid, g_Veh[slot][vPlate], plate);
        Staff_Log(playerid, "resetplate", ownerid, details);
        Plate_Mount(slot, plate);
        Msg_Admin(playerid, "Vozilo %d ima nove tablice: %s.", dbid, plate);
        if (ownerid != INVALID_PLAYER_ID && ownerid != playerid)
        {
            Msg_Info(ownerid, "Admin %s je zamenio tablice vaseg vozila: %s.", Account_Name(playerid), Veh_Name(slot));
        }
        return 1;
    }

    new reason[48];
    format(reason, sizeof(reason), "tablice %s (vozilo %d)", plate, dbid);
    if (!Money_Take(playerid, Plate_Price(playerid, kind), reason))
    {
        return 1;
    }
    strcopy(g_Veh[slot][vPendingPlate], plate, VEH_PLATE_LENGTH);
    new query[128];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `vehicles` SET `pending_plate` = '%e' WHERE `id` = %d", plate, dbid);
    mysql_tquery(g_SQL, query);
    Msg_Success(playerid, "Izvadili ste tablice %s. Postavite ih kad stanete pored vozila: /v tablice.", plate);
    return 1;
}

static stock bool:Plate_IsOwnSlot(playerid, slot)
{
    return slot >= 0 && slot < MAX_PERSONAL_VEHICLES && g_Veh[slot][vUsed] && g_Veh[slot][vOwnerDbId] == g_Player[playerid][pDbId];
}

static stock Plate_ShowVehicles(playerid)
{
    new body[640], line[128], registration[24], count, dbid = g_Player[playerid][pDbId];
    strcopy(body, "Vozilo\tTablice\tRegistracija\n");
    for (new slot = 0; slot < MAX_PERSONAL_VEHICLES && count < sizeof(g_PlateListed[]); slot++)
    {
        if (!g_Veh[slot][vUsed] || g_Veh[slot][vOwnerDbId] != dbid || Veh_IsBicycle(g_Veh[slot][vModel]))
        {
            continue;
        }
        Veh_RegistrationText(slot, registration);
        if (g_Veh[slot][vPendingPlate][0])
        {
            format(line, sizeof(line), "%s\t%s (ceka postavljanje)\t%s\n", Veh_ModelName(g_Veh[slot][vModel]), g_Veh[slot][vPendingPlate], registration);
        }
        else
        {
            new plate[VEH_PLATE_LENGTH] = "nema";
            if (g_Veh[slot][vPlate][0])
            {
                strcopy(plate, g_Veh[slot][vPlate]);
            }
            format(line, sizeof(line), "%s\t%s\t%s\n", Veh_ModelName(g_Veh[slot][vModel]), plate, registration);
        }
        strcat(body, line);
        g_PlateListed[playerid][count++] = slot;
    }
    if (count == 0)
    {
        Msg_Info(playerid, "Nemate vozilo kome trebaju tablice.");
        return;
    }
    ShowPlayerDialog(playerid, DIALOG_PLATE_VEHICLE, DIALOG_STYLE_TABLIST_HEADERS, "Policija - tablice", body, "Izaberi", "Zatvori");
}

static stock Plate_ShowActions(playerid, slot)
{
    if (!Veh_IsRegistered(slot))
    {
        Msg_Error(playerid, "Tablice se izdaju samo registrovanom vozilu. Registracija je na tehnickom pregledu (/registruj).");
        return;
    }
    if (g_Veh[slot][vPendingPlate][0])
    {
        Msg_Info(playerid, "Tablice %s vec cekaju: postavite ih pored vozila (/v tablice).", g_Veh[slot][vPendingPlate]);
        return;
    }
    new body[192], line[96];
    if (g_Veh[slot][vPlate][0])
    {
        format(body, sizeof(body), "Nove nasumicne tablice ($%d)\n", Plate_Price(playerid, PLATE_NEW));
    }
    else
    {
        format(body, sizeof(body), "Prve tablice ($%d)\n", Plate_Price(playerid, PLATE_FIRST));
    }
    format(line, sizeof(line), "Sopstvena tablica ($%d%s)", Plate_Price(playerid, PLATE_CUSTOM), Vip_GetLevel(playerid) >= VIP_GOLD ? (", Gold VIP popust") : (""));
    strcat(body, line);
    g_PlateSlot[playerid] = slot;
    ShowPlayerDialog(playerid, DIALOG_PLATE_ACTION, DIALOG_STYLE_LIST, Veh_Name(slot), body, "Izaberi", "Nazad");
}

// Letters, digits, space and dash, so the plate reads on the vehicle and cannot carry colour codes.
static stock bool:Plate_ParseCustom(const input[], plate[VEH_PLATE_LENGTH])
{
    new length = strlen(input);
    if (length < PLATE_CUSTOM_MIN || length > PLATE_CUSTOM_MAX)
    {
        return false;
    }
    new letters;
    for (new i = 0; i < length; i++)
    {
        new c = toupper(input[i]);
        if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
        {
            letters++;
        }
        else if (c != ' ' && c != '-')
        {
            return false;
        }
        plate[i] = c;
    }
    plate[length] = '\0';
    return letters >= PLATE_CUSTOM_MIN && plate[0] != ' ' && plate[length - 1] != ' ';
}

stock bool:Plate_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
    switch (dialogid)
    {
        case DIALOG_PLATE_VEHICLE:
        {
            if (response && listitem >= 0 && listitem < sizeof(g_PlateListed[]) && Plate_IsOwnSlot(playerid, g_PlateListed[playerid][listitem]))
            {
                Plate_ShowActions(playerid, g_PlateListed[playerid][listitem]);
            }
        }
        case DIALOG_PLATE_ACTION:
        {
            new slot = g_PlateSlot[playerid];
            if (!Plate_IsOwnSlot(playerid, slot))
            {
                return true;
            }
            if (!response)
            {
                Plate_ShowVehicles(playerid);
                return true;
            }
            if (!Point_Require(playerid, POINT_POLICE) || !Veh_IsRegistered(slot) || g_Veh[slot][vPendingPlate][0])
            {
                return true;
            }
            if (listitem == 0)
            {
                new E_PLATE_PURPOSE:purpose = g_Veh[slot][vPlate][0] ? PLATE_NEW : PLATE_FIRST;
                if (g_Player[playerid][pCash] < Plate_Price(playerid, purpose))
                {
                    Msg_Error(playerid, "Nemate dovoljno novca (potrebno je $%d).", Plate_Price(playerid, purpose));
                    return true;
                }
                Plate_RequestRandom(playerid, g_Veh[slot][vDbId], purpose);
            }
            else if (listitem == 1)
            {
                ShowPlayerDialog(playerid, DIALOG_PLATE_CUSTOM, DIALOG_STYLE_INPUT, "Sopstvena tablica",
                    "Upisite tablicu: 2 do 8 znakova, slova (bez nasih), brojevi, razmak i crtica.\nAdmin moze da zameni uvredljivu tablicu bez povracaja novca.", "Izvadi", "Nazad");
            }
        }
        case DIALOG_PLATE_CUSTOM:
        {
            new slot = g_PlateSlot[playerid];
            if (!Plate_IsOwnSlot(playerid, slot))
            {
                return true;
            }
            if (!response)
            {
                Plate_ShowActions(playerid, slot);
                return true;
            }
            new plate[VEH_PLATE_LENGTH];
            if (!Plate_ParseCustom(inputtext, plate))
            {
                Msg_Error(playerid, "Tablica mora imati 2 do 8 znakova: slova bez nasih, brojevi, razmak i crtica.");
                Plate_ShowActions(playerid, slot);
                return true;
            }
            if (!Point_Require(playerid, POINT_POLICE) || !Veh_IsRegistered(slot) || g_Veh[slot][vPendingPlate][0])
            {
                return true;
            }
            if (g_Player[playerid][pCash] < Plate_Price(playerid, PLATE_CUSTOM))
            {
                Msg_Error(playerid, "Nemate dovoljno novca (potrebno je $%d).", Plate_Price(playerid, PLATE_CUSTOM));
                return true;
            }
            Plate_Check(playerid, g_Veh[slot][vDbId], PLATE_CUSTOM, plate);
        }
        default:
        {
            return false;
        }
    }
    return true;
}

cmd:tablice(playerid, params[])
{
    if (!Point_Require(playerid, POINT_POLICE))
    {
        return 1;
    }
    Plate_ShowVehicles(playerid);
    return 1;
}

// The owner mounts the plates standing next to the vehicle (or sitting in it), wherever it is parked.
stock Plate_MountPending(playerid)
{
    new vehicleid = Veh_FindNearby(playerid, true);
    if (vehicleid == INVALID_VEHICLE_ID)
    {
        Msg_Error(playerid, "Stanite pored svog vozila.");
        return;
    }
    new slot = g_VehSlotOf[vehicleid];
    if (!g_Veh[slot][vPendingPlate][0])
    {
        Msg_Error(playerid, "Za ovo vozilo nemate izvadjene tablice. Vade se u policiji (/tablice).");
        return;
    }
    new plate[VEH_PLATE_LENGTH];
    strcopy(plate, g_Veh[slot][vPendingPlate]);
    Plate_Mount(slot, plate);
    Chat_Action(playerid, "postavlja nove tablice na vozilo.");
    Msg_Success(playerid, "Tablice %s su postavljene.", plate);
}

cmd:resettablice(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_ADMIN))
    {
        return 1;
    }
    new vehicleid;
    if (sscanf(params, "d", vehicleid) || vehicleid < 1 || vehicleid >= MAX_VEHICLES)
    {
        Msg_Usage(playerid, "/resettablice [id vozila] (uvredljiva tablica postaje nova nasumicna, besplatno; ID sa /vinfo)");
        return 1;
    }
    new slot = g_VehSlotOf[vehicleid];
    if (slot == -1 || !g_Veh[slot][vPlate][0])
    {
        Msg_Error(playerid, "To nije licno vozilo sa tablicama.");
        return 1;
    }
    Plate_RequestRandom(playerid, g_Veh[slot][vDbId], PLATE_ADMIN);
    return 1;
}
