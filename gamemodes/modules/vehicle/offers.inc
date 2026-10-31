#define OFFER_RANGE             (5.0)
// Both players stand by the vehicle, so the buyer sees exactly what they buy.
#define OFFER_VEHICLE_RANGE     (8.0)
#define OFFER_TIMEOUT_S         (60)
#define OFFER_COOLDOWN_S        (60)
// Below this a sale looks like moving a vehicle to another account.
#define OFFER_MIN_PRICE         (1000)
#define OFFER_MAX_PRICE         (10000000)
#define OFFER_TAX_PERCENT       (5)
#define OFFER_MIN_LEVEL         (2)

enum E_OFFER_KIND
{
    OFFER_SALE,
    OFFER_TRADE
}

enum E_OFFER
{
    E_OFFER_KIND:ofKind,
    ofTarget,
    ofTargetRace,
    ofSlot,
    ofDbId,
    ofTheirSlot,
    ofTheirDbId,
    // Sale: the price. Trade: what the offering player pays on top, negative when they ask for it.
    ofAmount,
    bool:ofKeepPlate,
    bool:ofTheirKeepPlate,
    ofSentAt
}

new g_Offer[MAX_PLAYERS][E_OFFER];
new g_OfferFrom[MAX_PLAYERS];
new g_OfferFromRace[MAX_PLAYERS];
new g_OfferNextAt[MAX_PLAYERS];
new g_OfferListed[MAX_PLAYERS][VEH_MAX_PER_PLAYER + VEH_VIP_EXTRA + 1];
// Trade surcharge direction picked in the dialog: 0 none, 1 the offering player pays, 2 they ask for it.
new g_OfferExtraKind[MAX_PLAYERS];

stock Offer_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `vehicle_trades` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`kind` ENUM('sale','trade') NOT NULL," \
        "`vehicle_id` INT UNSIGNED NOT NULL, `other_vehicle_id` INT UNSIGNED NOT NULL DEFAULT 0," \
        "`seller_id` INT UNSIGNED NOT NULL, `buyer_id` INT UNSIGNED NOT NULL," \
        "`amount` INT NOT NULL, `tax` INT NOT NULL," \
        "`created_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
}

stock Offer_OnPlayerConnect(playerid)
{
    g_OfferFrom[playerid] = INVALID_PLAYER_ID;
    g_OfferNextAt[playerid] = 0;
    g_Offer[playerid][ofTarget] = INVALID_PLAYER_ID;
}

// A vehicle of ownerid that can change hands right now; both players have to stand by it.
static stock bool:Offer_IsVehicleReady(slot, dbid, owner, other)
{
    if (slot < 0 || slot >= MAX_PERSONAL_VEHICLES || !g_Veh[slot][vUsed] || g_Veh[slot][vDbId] != dbid
        || g_Veh[slot][vOwnerDbId] != g_Player[owner][pDbId] || g_Veh[slot][vDiedAt] != 0 || g_Veh[slot][vPendingPlate][0])
    {
        return false;
    }
    new vehicleid = g_Veh[slot][vVehicleId], Float:x, Float:y, Float:z, Float:ox, Float:oy, Float:oz;
    GetPlayerPos(owner, x, y, z);
    GetPlayerPos(other, ox, oy, oz);
    return !IsVehicleOccupied(vehicleid) && GetVehicleDistanceFromPoint(vehicleid, x, y, z) < OFFER_VEHICLE_RANGE
        && GetVehicleDistanceFromPoint(vehicleid, ox, oy, oz) < OFFER_VEHICLE_RANGE;
}

static stock bool:Offer_ListVehicles(playerid, owner, other, dialogid, const title[])
{
    new body[512], line[96], registration[24], count;
    for (new slot = 0; slot < MAX_PERSONAL_VEHICLES && count < sizeof(g_OfferListed[]); slot++)
    {
        if (!g_Veh[slot][vUsed] || !Offer_IsVehicleReady(slot, g_Veh[slot][vDbId], owner, other))
        {
            continue;
        }
        Veh_RegistrationText(slot, registration);
        format(line, sizeof(line), "%s\t%s\n", Veh_Name(slot), registration);
        strcat(body, line);
        g_OfferListed[playerid][count++] = slot;
    }
    if (count == 0)
    {
        Msg_Error(playerid, "Pored vas nema vozila za ovo (oba igraca stoje pored vozila, u njemu niko ne sedi, izvadjene tablice su postavljene).");
        return false;
    }
    ShowPlayerDialog(playerid, dialogid, DIALOG_STYLE_TABLIST, title, body, "Izaberi", "Odustani");
    return true;
}

static stock Offer_SlotAt(playerid, listitem)
{
    return (listitem >= 0 && listitem < sizeof(g_OfferListed[])) ? g_OfferListed[playerid][listitem] : -1;
}

// Checked when the offer goes out and again when it is accepted, since anything may have changed in between.
static stock bool:Offer_IsValid(seller, bool:tell)
{
    new target = g_Offer[seller][ofTarget];
    if (target == INVALID_PLAYER_ID || !Account_IsLoggedIn(target) || g_AccountRace[target] != g_Offer[seller][ofTargetRace] || !Account_IsLoggedIn(seller))
    {
        return false;
    }
    new message[96];
    if (!Chat_IsInRange(seller, target, OFFER_RANGE) || GetPlayerState(target) == PLAYER_STATE_SPECTATING)
    {
        strcopy(message, "Igraci vise nisu jedan pored drugog.");
    }
    else if (!Offer_IsVehicleReady(g_Offer[seller][ofSlot], g_Offer[seller][ofDbId], seller, target))
    {
        strcopy(message, "Vozilo vise nije spremno (neko sedi u njemu ili niste pored njega).");
    }
    else if (g_Offer[seller][ofKind] == OFFER_TRADE && !Offer_IsVehicleReady(g_Offer[seller][ofTheirSlot], g_Offer[seller][ofTheirDbId], target, seller))
    {
        strcopy(message, "Drugo vozilo vise nije spremno.");
    }
    else if (g_Offer[seller][ofKind] == OFFER_SALE && (g_Player[target][pLevel] < OFFER_MIN_LEVEL || !Veh_HasFreeSlot(target)))
    {
        format(message, sizeof(message), "Kupac mora biti nivo %d+ i imati slobodno mesto za vozilo.", OFFER_MIN_LEVEL);
    }
    else if (g_Offer[seller][ofKeepPlate] && Plate_FreeStoredSlot(seller) == -1)
    {
        strcopy(message, "Imate najvise sacuvanih tablica; iskoristite neku u policiji (/tablice).");
    }
    else
    {
        return true;
    }
    if (tell)
    {
        Msg_Error(seller, "%s", message);
        Msg_Error(target, "%s", message);
    }
    return false;
}

static stock Offer_Describe(seller, dest[], size = sizeof(dest))
{
    new slot = g_Offer[seller][ofSlot], registration[24], Float:health;
    Veh_RegistrationText(slot, registration);
    GetVehicleHealth(g_Veh[slot][vVehicleId], health);
    if (g_Offer[seller][ofKind] == OFFER_SALE)
    {
        format(dest, size, "%s vam prodaje vozilo:\n\n%s\nRegistracija: %s\nGorivo: %.0f l\nStanje: %.0f / 1000\nTablice: %s\n\nCena: $%d (gotovinom)\n\nPonuda vazi %d sekundi.",
            Account_Name(seller), Veh_Name(slot), registration, g_VehFuel[g_Veh[slot][vVehicleId]], health,
            g_Offer[seller][ofKeepPlate] ? ("prodavac ih zadrzava") : ("ostaju na vozilu"), g_Offer[seller][ofAmount], OFFER_TIMEOUT_S);
        return;
    }
    new theirs = g_Offer[seller][ofTheirSlot], extra[64];
    if (g_Offer[seller][ofAmount] > 0)
    {
        format(extra, sizeof(extra), "%s vam doplacuje $%d", Account_Name(seller), g_Offer[seller][ofAmount]);
    }
    else if (g_Offer[seller][ofAmount] < 0)
    {
        format(extra, sizeof(extra), "vi doplacujete $%d (gotovinom)", -g_Offer[seller][ofAmount]);
    }
    else
    {
        strcopy(extra, "bez doplate");
    }
    format(dest, size, "%s nudi zamenu vozila:\n\nDobijate: %s (registracija: %s, stanje %.0f / 1000)\nDajete: %s\nDoplata: %s\nNjegove tablice: %s\n\nPonuda vazi %d sekundi.",
        Account_Name(seller), Veh_Name(slot), registration, health, Veh_Name(theirs), extra,
        g_Offer[seller][ofKeepPlate] ? ("zadrzava ih") : ("ostaju na vozilu"), OFFER_TIMEOUT_S);
}

static stock Offer_Send(seller)
{
    if (!Offer_IsValid(seller, true))
    {
        return;
    }
    new target = g_Offer[seller][ofTarget], text[512];
    g_Offer[seller][ofSentAt] = gettime();
    g_OfferNextAt[seller] = gettime() + OFFER_COOLDOWN_S;
    g_OfferFrom[target] = seller;
    g_OfferFromRace[target] = g_AccountRace[seller];
    Offer_Describe(seller, text);
    ShowPlayerDialog(target, DIALOG_OFFER_ACCEPT, DIALOG_STYLE_MSGBOX, "Ponuda za vozilo", text, "Prihvati", "Odbij");
    Msg_Info(seller, "Ponuda je poslata igracu %s.", Account_Name(target));
}

// Who pays the surcharge and how much the other side receives after the state tax.
static stock Offer_Tax(amount)
{
    return amount * OFFER_TAX_PERCENT / 100;
}

static stock Offer_Log(E_OFFER_KIND:kind, dbid, other_dbid, seller_dbid, buyer_dbid, amount, tax)
{
    new query[256];
    mysql_format(g_SQL, query, sizeof(query),
        "INSERT INTO `vehicle_trades` (`kind`, `vehicle_id`, `other_vehicle_id`, `seller_id`, `buyer_id`, `amount`, `tax`) VALUES ('%e', %d, %d, %d, %d, %d, %d)",
        kind == OFFER_SALE ? ("sale") : ("trade"), dbid, other_dbid, seller_dbid, buyer_dbid, amount, tax);
    mysql_tquery(g_SQL, query);
}

static stock Offer_Complete(seller)
{
    new target = g_Offer[seller][ofTarget], slot = g_Offer[seller][ofSlot], amount = g_Offer[seller][ofAmount], reason[64];
    g_OfferFrom[target] = INVALID_PLAYER_ID;
    if (g_Offer[seller][ofKind] == OFFER_SALE)
    {
        if (g_Player[target][pCash] < amount)
        {
            Msg_Error(target, "Nemate dovoljno gotovine (potrebno je $%d).", amount);
            Msg_Error(seller, "Kupac nema dovoljno gotovine.");
            return;
        }
        new tax = Offer_Tax(amount);
        format(reason, sizeof(reason), "kupovina vozila %d", g_Veh[slot][vDbId]);
        Money_GiveCash(target, -amount, reason, g_Player[seller][pDbId]);
        format(reason, sizeof(reason), "prodaja vozila %d (taksa $%d)", g_Veh[slot][vDbId], tax);
        Money_GiveCash(seller, amount - tax, reason, g_Player[target][pDbId]);
        if (g_Offer[seller][ofKeepPlate])
        {
            Plate_StoreFromVehicle(seller, slot);
        }
        Offer_Log(OFFER_SALE, g_Veh[slot][vDbId], 0, g_Player[seller][pDbId], g_Player[target][pDbId], amount, tax);
        Veh_TransferTo(slot, target);
        Account_Save(seller);
        Account_Save(target);
        Chat_Action(seller, "predaje kljuceve od vozila.");
        Msg_Success(seller, "Prodali ste %s za $%d (taksa drzave $%d, dobili ste $%d).", Veh_Name(slot), amount, tax, amount - tax);
        Msg_Success(target, "Kupili ste %s za $%d. Vozilo je parkirano ovde (/v parkiraj za drugo mesto).", Veh_Name(slot), amount);
        return;
    }

    new theirs = g_Offer[seller][ofTheirSlot], payer = amount > 0 ? seller : target, receiver = amount > 0 ? target : seller, extra = amount > 0 ? amount : -amount;
    if (extra > 0 && g_Player[payer][pCash] < extra)
    {
        Msg_Error(payer, "Nemate dovoljno gotovine za doplatu ($%d).", extra);
        Msg_Error(receiver, "Druga strana nema dovoljno gotovine za doplatu.");
        return;
    }
    new tax = Offer_Tax(extra);
    if (extra > 0)
    {
        format(reason, sizeof(reason), "doplata za zamenu vozila %d", g_Veh[slot][vDbId]);
        Money_GiveCash(payer, -extra, reason, g_Player[receiver][pDbId]);
        format(reason, sizeof(reason), "doplata za zamenu vozila (taksa $%d)", tax);
        Money_GiveCash(receiver, extra - tax, reason, g_Player[payer][pDbId]);
    }
    if (g_Offer[seller][ofKeepPlate])
    {
        Plate_StoreFromVehicle(seller, slot);
    }
    if (g_Offer[seller][ofTheirKeepPlate])
    {
        Plate_StoreFromVehicle(target, theirs);
    }
    Offer_Log(OFFER_TRADE, g_Veh[slot][vDbId], g_Veh[theirs][vDbId], g_Player[seller][pDbId], g_Player[target][pDbId], amount, tax);
    Veh_TransferTo(slot, target);
    Veh_TransferTo(theirs, seller);
    Account_Save(seller);
    Account_Save(target);
    Chat_Action(seller, "menja kljuceve od vozila.");
    Msg_Success(seller, "Zamena je obavljena: dobili ste %s.", Veh_Name(theirs));
    Msg_Success(target, "Zamena je obavljena: dobili ste %s.", Veh_Name(slot));
}

cmd:ponudi(playerid, params[])
{
    new targetid;
    if (sscanf(params, "u", targetid))
    {
        Msg_Usage(playerid, "/ponudi [id ili ime] (prodaja ili zamena vozila igracu pored vas)");
        return 1;
    }
    if (targetid == INVALID_PLAYER_ID || !Account_IsLoggedIn(targetid))
    {
        Msg_Error(playerid, "Taj igrac nije pronadjen.");
        return 1;
    }
    if (targetid == playerid)
    {
        Msg_Error(playerid, "Ne mozete ponuditi sebi.");
        return 1;
    }
    if (!Chat_CheckBeside(playerid, targetid, OFFER_RANGE))
    {
        return 1;
    }
    if (gettime() < g_OfferNextAt[playerid])
    {
        Msg_Error(playerid, "Sledecu ponudu mozete poslati za %d s.", g_OfferNextAt[playerid] - gettime());
        return 1;
    }
    g_Offer[playerid][ofTarget] = targetid;
    g_Offer[playerid][ofTargetRace] = g_AccountRace[targetid];
    g_Offer[playerid][ofKeepPlate] = false;
    g_Offer[playerid][ofTheirKeepPlate] = false;
    g_Offer[playerid][ofAmount] = 0;
    ShowPlayerDialog(playerid, DIALOG_OFFER_TYPE, DIALOG_STYLE_LIST, "Ponuda", "Prodaja vozila\nZamena vozila (uz doplatu ili bez)", "Izaberi", "Odustani");
    return 1;
}
alias:ponudi("offer")

static stock Offer_AskPlate(playerid)
{
    if (!g_Veh[g_Offer[playerid][ofSlot]][vPlate][0])
    {
        Offer_Send(playerid);
        return;
    }
    ShowPlayerDialog(playerid, DIALOG_OFFER_PLATE, DIALOG_STYLE_LIST, "Tablice",
        "Tablice ostaju na vozilu\nZadrzavam tablice (cuvaju se u policiji za drugo vozilo)", "Izaberi", "Odustani");
}

stock bool:Offer_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
    switch (dialogid)
    {
        case DIALOG_OFFER_TYPE:
        {
            if (response && (listitem == 0 || listitem == 1))
            {
                g_Offer[playerid][ofKind] = listitem == 0 ? OFFER_SALE : OFFER_TRADE;
                Offer_ListVehicles(playerid, playerid, g_Offer[playerid][ofTarget], DIALOG_OFFER_VEHICLE, "Vase vozilo");
            }
        }
        case DIALOG_OFFER_VEHICLE:
        {
            new slot = Offer_SlotAt(playerid, listitem), target = g_Offer[playerid][ofTarget];
            if (!response || slot == -1 || target == INVALID_PLAYER_ID || !Account_IsLoggedIn(target))
            {
                return true;
            }
            g_Offer[playerid][ofSlot] = slot;
            g_Offer[playerid][ofDbId] = g_Veh[slot][vDbId];
            if (g_Offer[playerid][ofKind] == OFFER_SALE)
            {
                g_OfferExtraKind[playerid] = 0;
                ShowPlayerDialog(playerid, DIALOG_OFFER_AMOUNT, DIALOG_STYLE_INPUT, "Cena", "Upisite cenu vozila (najmanje $1.000). Drzava uzima taksu 5% od cene.", "Dalje", "Odustani");
            }
            else
            {
                Offer_ListVehicles(playerid, target, playerid, DIALOG_OFFER_THEIR_VEHICLE, "Vozilo koje trazite u zamenu");
            }
        }
        case DIALOG_OFFER_THEIR_VEHICLE:
        {
            new slot = Offer_SlotAt(playerid, listitem);
            if (!response || slot == -1)
            {
                return true;
            }
            g_Offer[playerid][ofTheirSlot] = slot;
            g_Offer[playerid][ofTheirDbId] = g_Veh[slot][vDbId];
            ShowPlayerDialog(playerid, DIALOG_OFFER_EXTRA_KIND, DIALOG_STYLE_LIST, "Doplata", "Bez doplate\nJa doplacujem\nTrazim doplatu", "Izaberi", "Odustani");
        }
        case DIALOG_OFFER_EXTRA_KIND:
        {
            if (!response || listitem < 0 || listitem > 2)
            {
                return true;
            }
            g_OfferExtraKind[playerid] = listitem;
            if (listitem == 0)
            {
                g_Offer[playerid][ofAmount] = 0;
                Offer_AskPlate(playerid);
                return true;
            }
            ShowPlayerDialog(playerid, DIALOG_OFFER_AMOUNT, DIALOG_STYLE_INPUT, "Doplata", "Upisite iznos doplate. Drzava uzima taksu 5% od doplate.", "Dalje", "Odustani");
        }
        case DIALOG_OFFER_AMOUNT:
        {
            if (!response)
            {
                return true;
            }
            new amount = Money_ParseAmount(inputtext);
            new minimum = g_Offer[playerid][ofKind] == OFFER_SALE ? OFFER_MIN_PRICE : 1;
            if (amount < minimum || amount > OFFER_MAX_PRICE)
            {
                Msg_Error(playerid, "Iznos mora biti od $%d do $%d.", minimum, OFFER_MAX_PRICE);
                return true;
            }
            g_Offer[playerid][ofAmount] = g_OfferExtraKind[playerid] == 2 ? -amount : amount;
            Offer_AskPlate(playerid);
        }
        case DIALOG_OFFER_PLATE:
        {
            if (response && (listitem == 0 || listitem == 1))
            {
                g_Offer[playerid][ofKeepPlate] = listitem == 1;
                Offer_Send(playerid);
            }
        }
        case DIALOG_OFFER_ACCEPT:
        {
            new seller = g_OfferFrom[playerid];
            if (seller == INVALID_PLAYER_ID || g_AccountRace[seller] != g_OfferFromRace[playerid] || g_Offer[seller][ofTarget] != playerid)
            {
                return true;
            }
            if (!response)
            {
                g_OfferFrom[playerid] = INVALID_PLAYER_ID;
                Msg_Info(seller, "%s je odbio ponudu.", Account_Name(playerid));
                return true;
            }
            if (gettime() - g_Offer[seller][ofSentAt] > OFFER_TIMEOUT_S)
            {
                g_OfferFrom[playerid] = INVALID_PLAYER_ID;
                Msg_Error(playerid, "Ponuda je istekla.");
                return true;
            }
            if (!Offer_IsValid(seller, true))
            {
                g_OfferFrom[playerid] = INVALID_PLAYER_ID;
                return true;
            }
            // In a trade the other side also decides about their own plates.
            if (g_Offer[seller][ofKind] == OFFER_TRADE && g_Veh[g_Offer[seller][ofTheirSlot]][vPlate][0])
            {
                ShowPlayerDialog(playerid, DIALOG_OFFER_THEIR_PLATE, DIALOG_STYLE_LIST, "Vase tablice",
                    "Tablice ostaju na vozilu\nZadrzavam tablice (cuvaju se u policiji za drugo vozilo)", "Izaberi", "Odustani");
                return true;
            }
            Offer_Complete(seller);
        }
        case DIALOG_OFFER_THEIR_PLATE:
        {
            new seller = g_OfferFrom[playerid];
            if (seller == INVALID_PLAYER_ID || g_AccountRace[seller] != g_OfferFromRace[playerid] || g_Offer[seller][ofTarget] != playerid)
            {
                return true;
            }
            if (!response || gettime() - g_Offer[seller][ofSentAt] > OFFER_TIMEOUT_S)
            {
                g_OfferFrom[playerid] = INVALID_PLAYER_ID;
                return true;
            }
            g_Offer[seller][ofTheirKeepPlate] = listitem == 1;
            if (g_Offer[seller][ofTheirKeepPlate] && Plate_FreeStoredSlot(playerid) == -1)
            {
                Msg_Error(playerid, "Imate najvise sacuvanih tablica; iskoristite neku u policiji (/tablice).");
                return true;
            }
            if (Offer_IsValid(seller, true))
            {
                Offer_Complete(seller);
            }
        }
        default:
        {
            return false;
        }
    }
    return true;
}
