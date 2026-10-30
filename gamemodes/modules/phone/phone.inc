#define MAX_CONTACTS            (30)
#define PHONE_RING_SECONDS      (30)
// Numbers are stored as integers, so a leading zero would vanish; six digits always start at 1.
#define PHONE_NUMBER_MIN        (100000)
#define PHONE_NUMBER_MAX        (999999)
#define PHONE_EMERGENCY         (911)
#define PHONE_RANDOM_TRIES      (10)
#define PHONE_RING_RADIUS       (15.0)

// Two GTA sounds played in turn, one per second, while the phone rings.
new const g_Ringtones[][2] =
{
    {1137, 1138},
    {1056, 1056},
    {1052, 1052},
    {1052, 1053},
    {1139, 1139}
};

new const g_RingtoneNames[sizeof(g_Ringtones)][] =
{
    "Klasicna",
    "Odbrojavanje",
    "Pisak",
    "Dvostruki pisak",
    "Signal"
};

enum E_PHONE_CALL
{
    CALL_NONE,
    CALL_DIALING,
    CALL_RINGING,
    CALL_ACTIVE
}

// What a number check is for: a SIM card with a random or a chosen number, or a new number on the current SIM.
enum E_NUMBER_REQUEST
{
    NUMBER_SIM_RANDOM,
    NUMBER_SIM_CHOSEN,
    NUMBER_CHANGE
}

enum E_PHONE_STATE
{
    bool:phOff,
    E_PHONE_CALL:phCall,
    phPartner,
    phCallStart,
    phLastCharge,
    bool:phNumberPending,
    E_NUMBER_REQUEST:phNumberRequest,
    bool:phTopupFromBank
}

new g_Phone[MAX_PLAYERS][E_PHONE_STATE];

enum E_CONTACT
{
    ctDbId,
    ctName[MAX_PLAYER_NAME + 1],
    ctNumber
}

new g_Contacts[MAX_PLAYERS][MAX_CONTACTS][E_CONTACT];
new g_ContactSelected[MAX_PLAYERS];
new g_PhoneTimer;

stock Phone_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `phone_contacts` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`account_id` INT UNSIGNED NOT NULL," \
        "`name` VARCHAR(24) NOT NULL," \
        "`number` INT NOT NULL," \
        "INDEX `contact_account` (`account_id`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");

    new query[256];
    mysql_format(g_SQL, query, sizeof(query),
        "UPDATE `accounts` SET `phone_number` = 0, `phone_listed` = 0 " \
        "WHERE `phone_number` > 0 AND `last_login` IS NOT NULL AND `last_login` < NOW() - INTERVAL %d DAY",
        PHONE_INACTIVE_DAYS);
    mysql_tquery(g_SQL, query);

    g_PhoneTimer = SetTimer("Phone_Tick", 1000, true);
}

stock Phone_Exit()
{
    KillTimer(g_PhoneTimer);
}

stock Phone_OnPlayerConnect(playerid)
{
    static const empty[E_PHONE_STATE], empty_contact[E_CONTACT];
    g_Phone[playerid] = empty;
    g_Phone[playerid][phPartner] = INVALID_PLAYER_ID;
    for (new i = 0; i < MAX_CONTACTS; i++)
    {
        g_Contacts[playerid][i] = empty_contact;
    }
}

stock Phone_OnLogin(playerid)
{
    new query[96];
    mysql_format(g_SQL, query, sizeof(query), "SELECT `id`, `name`, `number` FROM `phone_contacts` WHERE `account_id` = %d LIMIT %d", g_Player[playerid][pDbId], MAX_CONTACTS);
    mysql_tquery(g_SQL, query, "OnContactsLoaded", "dd", playerid, g_AccountRace[playerid]);
}

forward OnContactsLoaded(playerid, race);
public OnContactsLoaded(playerid, race)
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }
    new rows = cache_num_rows();
    for (new row = 0; row < rows && row < MAX_CONTACTS; row++)
    {
        cache_get_value_name_int(row, "id", g_Contacts[playerid][row][ctDbId]);
        cache_get_value_name(row, "name", g_Contacts[playerid][row][ctName], MAX_PLAYER_NAME + 1);
        cache_get_value_name_int(row, "number", g_Contacts[playerid][row][ctNumber]);
    }
    return 1;
}

stock Phone_FindByNumber(number)
{
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (Account_IsLoggedIn(i) && g_Player[i][pPhoneNumber] == number)
        {
            return i;
        }
    }
    return INVALID_PLAYER_ID;
}

stock bool:Phone_IsReachable(playerid)
{
    return playerid != INVALID_PLAYER_ID && Inv_Has(playerid, ITEM_PHONE) && !g_Phone[playerid][phOff] && g_Player[playerid][pJailSeconds] == 0;
}

// What a listener's phone shows for a number: "Name (number)" for a saved contact, or the bare number.
stock Phone_Label(listener, number)
{
    new label[MAX_PLAYER_NAME + 12];
    for (new i = 0; i < MAX_CONTACTS; i++)
    {
        if (g_Contacts[listener][i][ctDbId] && g_Contacts[listener][i][ctNumber] == number)
        {
            format(label, sizeof(label), "%s (%d)", g_Contacts[listener][i][ctName], number);
            return label;
        }
    }
    format(label, sizeof(label), "%d", number);
    return label;
}

// The device alone: enough for 911. Prints the reason and returns false when it cannot be used.
stock bool:Phone_RequireDevice(playerid)
{
    if (!Inv_Has(playerid, ITEM_PHONE))
    {
        Msg_Error(playerid, "Nemate telefon. Kupuje se u prodavnici 24/7.");
        return false;
    }
    if (g_Phone[playerid][phOff])
    {
        Msg_Error(playerid, "Telefon vam je iskljucen (/telefon).");
        return false;
    }
    return true;
}

// Device and SIM card: needed for everything except 911.
stock bool:Phone_Require(playerid)
{
    if (!Phone_RequireDevice(playerid))
    {
        return false;
    }
    if (g_Player[playerid][pPhoneNumber] == 0)
    {
        Msg_Error(playerid, "Nemate SIM karticu. Kupuje se u prodavnici 24/7 (/kupi).");
        return false;
    }
    return true;
}

stock bool:Phone_TakeCredit(playerid, amount)
{
    if (g_Player[playerid][pPhoneCredit] < amount)
    {
        Msg_Error(playerid, "Nemate dovoljno kredita (imate $%d). Dopuna: prodavnica 24/7 ili /telefon.", g_Player[playerid][pPhoneCredit]);
        return false;
    }
    g_Player[playerid][pPhoneCredit] -= amount;
    return true;
}

// The caller pays every started minute; without credit the call ends.
stock bool:Phone_ChargeCall(callerid)
{
    if (g_Player[callerid][pPhoneCredit] < PHONE_CALL_MINUTE_PRICE)
    {
        Phone_EndCall(callerid, "Poziv je prekinut: nemate vise kredita (dopuna: prodavnica 24/7 ili /telefon).");
        return false;
    }
    g_Player[callerid][pPhoneCredit] -= PHONE_CALL_MINUTE_PRICE;
    g_Phone[callerid][phLastCharge] = gettime();
    return true;
}

stock bool:Phone_PicksForFree(playerid)
{
    return Vip_GetLevel(playerid) >= VIP_SILVER || Staff_IsAdmin(playerid);
}

stock Phone_NumberPrice(playerid, E_NUMBER_REQUEST:request)
{
    new price = (request == NUMBER_CHANGE) ? 0 : PHONE_SIM_PRICE;
    if (request != NUMBER_SIM_RANDOM && !Phone_PicksForFree(playerid))
    {
        price += PHONE_PICK_NUMBER_PRICE;
    }
    return price;
}

// Returns the number, or 0 when the text is not exactly six digits from 100000 to 999999.
stock Phone_ParseNumber(const input[])
{
    if (strlen(input) != 6)
    {
        return 0;
    }
    for (new i = 0; i < 6; i++)
    {
        if (input[i] < '0' || input[i] > '9')
        {
            return 0;
        }
    }
    new number = strval(input);
    return (number >= PHONE_NUMBER_MIN && number <= PHONE_NUMBER_MAX) ? number : 0;
}

// The number is checked against every account, online or not; payment happens only once it turns out free.
stock Phone_CheckNumber(playerid, number, E_NUMBER_REQUEST:request, tries = 0)
{
    new price = Phone_NumberPrice(playerid, request);
    if (g_Player[playerid][pCash] < price)
    {
        Msg_Error(playerid, "Nemate dovoljno novca (potrebno je $%d, imate $%d).", price, g_Player[playerid][pCash]);
        return;
    }

    g_Phone[playerid][phNumberPending] = true;
    new query[96];
    mysql_format(g_SQL, query, sizeof(query), "SELECT 1 FROM `accounts` WHERE `phone_number` = %d LIMIT 1", number);
    mysql_tquery(g_SQL, query, "OnPhoneNumberChecked", "ddddd", playerid, g_AccountRace[playerid], number, _:request, tries);
}

stock Phone_RequestNumber(playerid, number, E_NUMBER_REQUEST:request)
{
    if (g_Phone[playerid][phNumberPending])
    {
        Msg_Error(playerid, "Provera broja je vec u toku, sacekajte trenutak.");
        return;
    }
    if (number == g_Player[playerid][pPhoneNumber])
    {
        Msg_Error(playerid, "To je vec vas broj.");
        return;
    }
    Phone_CheckNumber(playerid, number, request);
}

stock Phone_RandomNumber()
{
    return PHONE_NUMBER_MIN + random(PHONE_NUMBER_MAX - PHONE_NUMBER_MIN + 1);
}

forward OnPhoneNumberChecked(playerid, race, number, request, tries);
public OnPhoneNumberChecked(playerid, race, number, request, tries)
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }
    g_Phone[playerid][phNumberPending] = false;

    new E_NUMBER_REQUEST:kind = E_NUMBER_REQUEST:request;
    if (cache_num_rows() > 0 || Phone_FindByNumber(number) != INVALID_PLAYER_ID)
    {
        if (kind == NUMBER_SIM_RANDOM && tries < PHONE_RANDOM_TRIES)
        {
            Phone_CheckNumber(playerid, Phone_RandomNumber(), kind, tries + 1);
        }
        else
        {
            Msg_Error(playerid, "Broj %d je zauzet, izaberite drugi.", number);
        }
        return 1;
    }

    new price = Phone_NumberPrice(playerid, kind);
    if (price > 0 && !Money_Take(playerid, price, (kind == NUMBER_CHANGE) ? ("promena broja telefona") : ("SIM kartica")))
    {
        return 1;
    }

    Phone_EndCall(playerid, "Poziv je prekinut zbog promene broja.");
    g_Player[playerid][pPhoneNumber] = number;
    Account_Save(playerid);
    if (kind == NUMBER_CHANGE)
    {
        Msg_Success(playerid, "Vas novi broj telefona je %d.", number);
    }
    else
    {
        Msg_Success(playerid, "Kupili ste SIM karticu za $%d. Vas broj je %d. Kredit dopunjujete u 24/7 ili preko /telefon.", price, number);
    }
    return 1;
}

stock Phone_ShowSimMenu(playerid)
{
    new body[160], title[48];
    if (Phone_PicksForFree(playerid))
    {
        format(body, sizeof(body), "Nasumican broj\t$%d\nIzaberi svoj broj\t$%d (izbor je besplatan za vas)", PHONE_SIM_PRICE, PHONE_SIM_PRICE);
    }
    else
    {
        format(body, sizeof(body), "Nasumican broj\t$%d\nIzaberi svoj broj\t$%d", PHONE_SIM_PRICE, PHONE_SIM_PRICE + PHONE_PICK_NUMBER_PRICE);
    }
    if (g_Player[playerid][pPhoneNumber] > 0)
    {
        format(title, sizeof(title), "SIM kartica (broj %d se gasi)", g_Player[playerid][pPhoneNumber]);
    }
    else
    {
        strcopy(title, "SIM kartica");
    }
    ShowPlayerDialog(playerid, DIALOG_PHONE_SIM, DIALOG_STYLE_TABLIST, title, body, "Kupi", "Nazad");
}

stock Phone_ShowNumberInput(playerid, E_NUMBER_REQUEST:request, const error[] = "")
{
    g_Phone[playerid][phNumberRequest] = request;
    new text[256], price = Phone_NumberPrice(playerid, request);
    format(text, sizeof(text), "%s%sUnesite broj od 6 cifara (od 100000 do 999999, ne moze poceti nulom).\nServer odmah proverava da li je slobodan. Cena: " C_GREEN "$%d",
        error, error[0] ? ("\n\n" C_WHITE) : (C_WHITE), price);
    ShowPlayerDialog(playerid, DIALOG_PHONE_NUMBER, DIALOG_STYLE_INPUT, "Izbor broja", text, "Proveri", "Nazad");
}

stock Phone_ShowTopup(playerid, bool:from_bank)
{
    g_Phone[playerid][phTopupFromBank] = from_bank;
    new body[128], line[24], title[64];
    for (new i = 0; i < sizeof(g_PhoneTopups); i++)
    {
        format(line, sizeof(line), "$%d\n", g_PhoneTopups[i]);
        strcat(body, line);
    }
    format(title, sizeof(title), "Dopuna %s (kredit: $%d)", from_bank ? ("sa racuna") : ("gotovinom"), g_Player[playerid][pPhoneCredit]);
    ShowPlayerDialog(playerid, DIALOG_PHONE_TOPUP, DIALOG_STYLE_LIST, title, body, "Uplati", "Nazad");
}

static stock Phone_Topup(playerid, amount)
{
    if (g_Player[playerid][pPhoneNumber] == 0)
    {
        Msg_Error(playerid, "Nemate SIM karticu. Kupuje se u prodavnici 24/7.");
        return;
    }
    if (g_Player[playerid][pPhoneCredit] + amount > PHONE_CREDIT_MAX)
    {
        Msg_Error(playerid, "Na kartici moze biti najvise $%d kredita (sada imate $%d).", PHONE_CREDIT_MAX, g_Player[playerid][pPhoneCredit]);
        return;
    }

    if (g_Phone[playerid][phTopupFromBank])
    {
        if (!Phone_RequireDevice(playerid) || !Money_RequireAccount(playerid))
        {
            return;
        }
        if (g_Player[playerid][pBank] < amount)
        {
            Msg_Error(playerid, "Na racunu nemate dovoljno novca (imate $%d).", g_Player[playerid][pBank]);
            return;
        }
        Money_GiveBank(playerid, -amount, "dopuna kredita");
    }
    else if (!Point_Require(playerid, POINT_SHOP) || !Money_Take(playerid, amount, "dopuna kredita"))
    {
        return;
    }

    g_Player[playerid][pPhoneCredit] += amount;
    Account_Save(playerid);
    Msg_Success(playerid, "Dopunili ste kredit za $%d. Stanje kredita: $%d.", amount, g_Player[playerid][pPhoneCredit]);
}

stock Phone_OnPhoneBought(playerid)
{
    if (g_Player[playerid][pPhoneNumber] == 0)
    {
        Msg_Info(playerid, "Za broj vam treba SIM kartica (ista prodavnica). Bez nje telefon zove samo 911. Meni: /telefon");
    }
    else
    {
        Msg_Info(playerid, "Vas broj telefona je %d. Meni telefona: /telefon", g_Player[playerid][pPhoneNumber]);
    }
}

stock Phone_OnPhoneLost(playerid)
{
    Phone_EndCall(playerid, "Poziv je prekinut jer vise nemate telefon.");
}

// Ends the call on both sides; reason goes to this player, the partner just hears the line drop.
stock Phone_EndCall(playerid, const reason[] = "")
{
    if (g_Phone[playerid][phCall] == CALL_NONE)
    {
        return;
    }

    new partner = g_Phone[playerid][phPartner];
    g_Phone[playerid][phCall] = CALL_NONE;
    g_Phone[playerid][phPartner] = INVALID_PLAYER_ID;
    if (reason[0])
    {
        Msg_Info(playerid, "%s", reason);
    }

    if (partner != INVALID_PLAYER_ID && g_Phone[partner][phPartner] == playerid)
    {
        g_Phone[partner][phCall] = CALL_NONE;
        g_Phone[partner][phPartner] = INVALID_PLAYER_ID;
        Msg_Info(partner, "Veza je prekinuta.");
    }
}

stock Phone_OnPlayerDisconnect(playerid)
{
    Phone_EndCall(playerid);
}

stock Phone_OnPlayerDeath(playerid)
{
    Phone_EndCall(playerid, "Poziv je prekinut.");
}

// A value from the database is not trusted as an array index.
stock Phone_Ringtone(playerid)
{
    new ringtone = g_Player[playerid][pRingtone];
    return (ringtone >= 0 && ringtone < sizeof(g_Ringtones)) ? ringtone : 0;
}

// Heard by the ringing player and everyone close by, from where the phone is.
static stock Phone_PlayRing(playerid, tick)
{
    new sound = g_Ringtones[Phone_Ringtone(playerid)][tick % 2];
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (Account_IsLoggedIn(i) && (i == playerid || Chat_IsInRange(i, playerid, PHONE_RING_RADIUS)))
        {
            PlayerPlaySound(i, sound, x, y, z);
        }
    }
}

forward Phone_Tick();
public Phone_Tick()
{
    new now = gettime();
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (g_Phone[i][phCall] == CALL_NONE || !Account_IsLoggedIn(i))
        {
            continue;
        }
        if (g_Player[i][pJailSeconds] > 0)
        {
            Phone_EndCall(i, "Poziv je prekinut.");
            continue;
        }
        if (g_Phone[i][phCall] == CALL_DIALING && now - g_Phone[i][phCallStart] >= PHONE_RING_SECONDS)
        {
            new partner = g_Phone[i][phPartner];
            Msg_Info(partner, "Propusten poziv: %s.", Phone_Label(partner, g_Player[i][pPhoneNumber]));
            Phone_EndCall(i, "Niko se ne javlja.");
            continue;
        }
        if (g_Phone[i][phCall] == CALL_RINGING)
        {
            Phone_PlayRing(i, now);
        }
        // phCallStart -1 marks the paying side of an active call.
        if (g_Phone[i][phCall] == CALL_ACTIVE && g_Phone[i][phCallStart] == -1 && now - g_Phone[i][phLastCharge] >= 60)
        {
            Phone_ChargeCall(i);
        }
    }
    return 1;
}

// Returns true when the text went into a phone call instead of the normal chat.
stock bool:Phone_OnPlayerText(playerid, const text[])
{
    if (g_Phone[playerid][phCall] != CALL_ACTIVE)
    {
        return false;
    }

    new partner = g_Phone[playerid][phPartner], message[192];
    format(message, sizeof(message), "%s kaze (telefon): %s", Account_Name(playerid), text);
    Chat_SendNearby(playerid, CHAT_RADIUS_TALK, COLOR_WHITE, message, true);
    format(message, sizeof(message), "[Telefon] %s: %s", Phone_Label(partner, g_Player[playerid][pPhoneNumber]), text);
    Chat_Send(partner, COLOR_YELLOW, message);
    return true;
}

stock Phone_Emergency(playerid, const text[])
{
    new message[192], place[ZONE_NAME_LENGTH] = "enterijer", Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    new zone = GetPlayerInterior(playerid) == 0 ? Hud_FindZone(x, y) : -1;
    if (zone != -1)
    {
        strcopy(place, g_Zones[zone][zName]);
    }
    format(message, sizeof(message), "[911] %s [%d] (%s): %s", Account_Name(playerid), playerid, place, text);
    Staff_SendToAdmins(message, CHANNEL_REPORTS);
    Msg_Info(playerid, "Dispecer: Hitne sluzbe jos ne rade, vas poziv je prosledjen administraciji.");
}

stock Phone_ShowMenu(playerid)
{
    if (!Inv_Has(playerid, ITEM_PHONE))
    {
        Msg_Error(playerid, "Nemate telefon. Kupuje se u prodavnici 24/7.");
        return;
    }

    new title[64], body[320];
    if (g_Player[playerid][pPhoneNumber] > 0)
    {
        format(title, sizeof(title), "Telefon | Broj: %d | Kredit: $%d", g_Player[playerid][pPhoneNumber], g_Player[playerid][pPhoneCredit]);
    }
    else
    {
        strcopy(title, "Telefon | nema SIM kartice");
    }
    format(body, sizeof(body),
        "Pozovi\nPoruka (SMS)\nKontakti\nJavni imenik ($%d kredita)\nDopuna sa racuna\nPromeni broj\nUpis u javni imenik: %s\nMelodija: " C_SERVER "%s" C_WHITE "\n%s telefon",
        PHONE_DIRECTORY_PRICE, g_Player[playerid][pPhoneListed] ? (C_GREEN "da") : (C_GREY "ne"),
        g_RingtoneNames[Phone_Ringtone(playerid)], g_Phone[playerid][phOff] ? ("Ukljuci") : ("Iskljuci"));
    ShowPlayerDialog(playerid, DIALOG_PHONE_MENU, DIALOG_STYLE_LIST, title, body, "Izaberi", "Zatvori");
}

static stock Phone_ShowRingtones(playerid)
{
    new body[256], line[48], current = Phone_Ringtone(playerid);
    for (new i = 0; i < sizeof(g_RingtoneNames); i++)
    {
        format(line, sizeof(line), "%s%s%s\n", i == current ? (C_GREEN) : (C_WHITE), g_RingtoneNames[i], i == current ? (" (izabrana)") : (""));
        strcat(body, line);
    }
    ShowPlayerDialog(playerid, DIALOG_PHONE_RINGTONE, DIALOG_STYLE_LIST, "Melodija (klik pusta i bira)", body, "Izaberi", "Nazad");
}

static stock Phone_OnMenuSelect(playerid, listitem)
{
    switch (listitem)
    {
        case 0: ShowPlayerDialog(playerid, DIALOG_PHONE_CALL, DIALOG_STYLE_INPUT, "Pozovi",
            C_WHITE "Unesite broj. Za hitne sluzbe: " C_SERVER "911 [sta se desilo i gde]", "Pozovi", "Nazad");
        case 1: ShowPlayerDialog(playerid, DIALOG_PHONE_SMS, DIALOG_STYLE_INPUT, "Poruka",
            C_WHITE "Unesite broj i poruku, na primer: " C_SERVER "123456 Gde si?", "Posalji", "Nazad");
        case 2: Phone_ShowContacts(playerid);
        case 3: ShowPlayerDialog(playerid, DIALOG_PHONE_DIRECTORY, DIALOG_STYLE_INPUT, "Javni imenik",
            C_WHITE "Unesite ime i prezime osobe, na primer: " C_SERVER "Marko_Markovic\n" \
            C_GREY "Broj se prikazuje samo ako se ta osoba upisala u javni imenik.", "Trazi", "Nazad");
        case 4: Phone_ShowTopup(playerid, true);
        case 5:
        {
            if (g_Player[playerid][pPhoneNumber] == 0)
            {
                Msg_Error(playerid, "Nemate SIM karticu. Kupuje se u prodavnici 24/7.");
                Phone_ShowMenu(playerid);
                return;
            }
            Phone_ShowNumberInput(playerid, NUMBER_CHANGE);
        }
        case 6:
        {
            if (g_Player[playerid][pPhoneNumber] == 0)
            {
                Msg_Error(playerid, "Nemate SIM karticu. Kupuje se u prodavnici 24/7.");
            }
            else
            {
                g_Player[playerid][pPhoneListed] = !g_Player[playerid][pPhoneListed];
                Account_Save(playerid);
                Msg_Info(playerid, "%s", g_Player[playerid][pPhoneListed] ? ("Upisani ste u javni imenik: ko zna vase ime moze da nadje vas broj.") : ("Vise niste u javnom imeniku."));
            }
            Phone_ShowMenu(playerid);
        }
        case 7: Phone_ShowRingtones(playerid);
        case 8:
        {
            g_Phone[playerid][phOff] = !g_Phone[playerid][phOff];
            if (g_Phone[playerid][phOff])
            {
                Phone_EndCall(playerid, "Poziv je prekinut.");
            }
            Msg_Info(playerid, "Telefon je %s.", g_Phone[playerid][phOff] ? ("iskljucen: niko vas ne moze dobiti") : ("ukljucen"));
        }
    }
}

// Dialog input becomes the same command the player could type, so all checks stay in one place.
static stock Phone_RunCommand(playerid, const command[], const input[])
{
    new line[160];
    format(line, sizeof(line), "%s %s", command, input);
    PC_EmulateCommand(playerid, line);
}

forward OnPhoneDirectory(playerid, race, const name[]);
public OnPhoneDirectory(playerid, race, const name[])
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }
    new number;
    if (cache_num_rows() > 0)
    {
        cache_get_value_name_int(0, "phone_number", number);
    }
    if (number > 0)
    {
        Msg_Info(playerid, "Javni imenik: broj osobe %s je %d. Sacuvajte ga u kontakte (/kontakti).", name, number);
    }
    else
    {
        Msg_Info(playerid, "Javni imenik: osoba %s nije upisana u imenik.", name);
    }
    return 1;
}

static stock Phone_SearchDirectory(playerid, const input[])
{
    new name[MAX_PLAYER_NAME + 1];
    strcopy(name, input);
    for (new i = 0; name[i]; i++)
    {
        if (name[i] == ' ')
        {
            name[i] = '_';
        }
    }
    if (strlen(name) < 3 || strlen(input) > MAX_PLAYER_NAME)
    {
        Msg_Error(playerid, "Unesite ime i prezime, na primer: Marko_Markovic");
        return;
    }
    if (!Phone_Require(playerid) || !Phone_TakeCredit(playerid, PHONE_DIRECTORY_PRICE))
    {
        return;
    }

    new query[160];
    mysql_format(g_SQL, query, sizeof(query),
        "SELECT `phone_number` FROM `accounts` WHERE `name` = '%e' AND `phone_listed` = 1 AND `phone_number` > 0 LIMIT 1", name);
    mysql_tquery(g_SQL, query, "OnPhoneDirectory", "dds", playerid, g_AccountRace[playerid], name);
}

cmd:telefon(playerid, params[])
{
    Phone_ShowMenu(playerid);
    return 1;
}

cmd:pozovi(playerid, params[])
{
    if (!Phone_RequireDevice(playerid))
    {
        return 1;
    }
    new number, text[128];
    if (sscanf(params, "dS()[128]", number, text))
    {
        Msg_Usage(playerid, "/pozovi [broj] (911 [opis] za hitne sluzbe)");
        return 1;
    }
    if (g_Phone[playerid][phCall] != CALL_NONE)
    {
        Msg_Error(playerid, "Vec ste u razgovoru. Prekid: /prekini");
        return 1;
    }
    if (number == PHONE_EMERGENCY)
    {
        if (isnull(text))
        {
            Msg_Usage(playerid, "/pozovi 911 [sta se desilo i gde]");
            return 1;
        }
        if (!Punish_CanSpeak(playerid, text))
        {
            return 1;
        }
        Chat_Action(playerid, "vadi telefon i zove hitne sluzbe.");
        Phone_Emergency(playerid, text);
        return 1;
    }
    if (!Phone_Require(playerid))
    {
        return 1;
    }
    if (number == g_Player[playerid][pPhoneNumber])
    {
        Msg_Error(playerid, "Ne mozete pozvati sami sebe.");
        return 1;
    }
    if (g_Player[playerid][pPhoneCredit] < PHONE_CALL_MINUTE_PRICE)
    {
        Msg_Error(playerid, "Nemate kredita za poziv ($%d po minutu). Dopuna: prodavnica 24/7 ili /telefon.", PHONE_CALL_MINUTE_PRICE);
        return 1;
    }

    new targetid = Phone_FindByNumber(number);
    Chat_Action(playerid, "vadi telefon i bira broj.");
    if (!Phone_IsReachable(targetid))
    {
        Msg_Info(playerid, "Broj koji ste pozvali trenutno nije dostupan.");
        return 1;
    }
    if (g_Phone[targetid][phCall] != CALL_NONE)
    {
        Msg_Info(playerid, "Broj je zauzet.");
        return 1;
    }

    g_Phone[playerid][phCall] = CALL_DIALING;
    g_Phone[playerid][phPartner] = targetid;
    g_Phone[playerid][phCallStart] = gettime();
    g_Phone[targetid][phCall] = CALL_RINGING;
    g_Phone[targetid][phPartner] = playerid;

    new message[96];
    format(message, sizeof(message), "* Telefon zvoni (( %s ))", Account_Name(targetid));
    Chat_SendNearby(targetid, CHAT_RADIUS_ACTION, COLOR_PURPLE, message);
    Msg_Info(targetid, "Telefon zvoni! Poziva: %s. Javljanje: /javi, odbijanje: /prekini", Phone_Label(targetid, g_Player[playerid][pPhoneNumber]));
    Msg_Info(playerid, "Zvoni... (prekid: /prekini)");
    return 1;
}

cmd:javi(playerid, params[])
{
    if (g_Phone[playerid][phCall] != CALL_RINGING)
    {
        Msg_Error(playerid, "Niko vas ne zove.");
        return 1;
    }
    if (!Phone_Require(playerid))
    {
        return 1;
    }

    new callerid = g_Phone[playerid][phPartner];
    if (!Phone_ChargeCall(callerid))
    {
        return 1;
    }

    g_Phone[callerid][phCall] = CALL_ACTIVE;
    g_Phone[callerid][phCallStart] = -1;
    g_Phone[playerid][phCall] = CALL_ACTIVE;
    g_Phone[playerid][phCallStart] = 0;

    Chat_Action(playerid, "se javlja na telefon.");
    Msg_Info(playerid, "Javili ste se. Sve sto pisete u chat cuje sagovornik. Prekid: /prekini");
    Msg_Info(callerid, "Javio se. Sve sto pisete u chat cuje sagovornik. Prekid: /prekini");
    return 1;
}

cmd:prekini(playerid, params[])
{
    if (g_Phone[playerid][phCall] == CALL_NONE)
    {
        Msg_Error(playerid, "Niste u razgovoru.");
        return 1;
    }
    Chat_Action(playerid, "spusta telefon.");
    Phone_EndCall(playerid, "Prekinuli ste poziv.");
    return 1;
}

cmd:sms(playerid, params[])
{
    if (!Phone_Require(playerid))
    {
        return 1;
    }
    new number, text[128];
    if (sscanf(params, "ds[128]", number, text))
    {
        Msg_Usage(playerid, "/sms [broj] [poruka] ($%d kredita po poruci)", PHONE_SMS_PRICE);
        return 1;
    }

    new targetid = Phone_FindByNumber(number);
    if (!Phone_IsReachable(targetid) || targetid == playerid)
    {
        Msg_Info(playerid, "Poruka nije isporucena: broj nije dostupan.");
        return 1;
    }
    if (!Phone_TakeCredit(playerid, PHONE_SMS_PRICE))
    {
        return 1;
    }

    new message[192];
    format(message, sizeof(message), "[SMS] od %s: %s", Phone_Label(targetid, g_Player[playerid][pPhoneNumber]), text);
    Chat_Send(targetid, COLOR_YELLOW, message);
    format(message, sizeof(message), "[SMS] za %s: %s", Phone_Label(playerid, number), text);
    Chat_Send(playerid, COLOR_YELLOW, message);
    Chat_Action(playerid, "kuca poruku na telefonu.");
    return 1;
}

stock Phone_ShowContacts(playerid)
{
    new body[1536], line[64];
    strcopy(body, C_GREEN "+ Dodaj kontakt\n");
    for (new i = 0; i < MAX_CONTACTS; i++)
    {
        if (g_Contacts[playerid][i][ctDbId])
        {
            format(line, sizeof(line), "%s\t%d\n", g_Contacts[playerid][i][ctName], g_Contacts[playerid][i][ctNumber]);
            strcat(body, line);
        }
    }
    ShowPlayerDialog(playerid, DIALOG_CONTACTS, DIALOG_STYLE_TABLIST, "Kontakti", body, "Izaberi", "Zatvori");
}

// The list shows only filled contacts, so the n-th line is the n-th used slot.
stock Phone_ContactSlot(playerid, listitem)
{
    for (new i = 0, n = 0; i < MAX_CONTACTS; i++)
    {
        if (g_Contacts[playerid][i][ctDbId] && n++ == listitem)
        {
            return i;
        }
    }
    return -1;
}

cmd:kontakti(playerid, params[])
{
    if (Inv_Has(playerid, ITEM_PHONE))
    {
        Phone_ShowContacts(playerid);
    }
    else
    {
        Msg_Error(playerid, "Nemate telefon. Kupuje se u prodavnici 24/7.");
    }
    return 1;
}

stock bool:Phone_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
    switch (dialogid)
    {
        case DIALOG_PHONE_MENU:
        {
            if (response && Inv_Has(playerid, ITEM_PHONE))
            {
                Phone_OnMenuSelect(playerid, listitem);
            }
        }
        case DIALOG_PHONE_RINGTONE:
        {
            if (!response || !Inv_Has(playerid, ITEM_PHONE))
            {
                Phone_ShowMenu(playerid);
                return true;
            }
            if (listitem >= 0 && listitem < sizeof(g_RingtoneNames))
            {
                g_Player[playerid][pRingtone] = listitem;
                PlayerPlaySound(playerid, g_Ringtones[listitem][0], 0.0, 0.0, 0.0);
                Msg_Info(playerid, "Melodija telefona: %s.", g_RingtoneNames[listitem]);
            }
            Phone_ShowRingtones(playerid);
        }
        case DIALOG_PHONE_CALL, DIALOG_PHONE_SMS:
        {
            if (!response || isnull(inputtext))
            {
                Phone_ShowMenu(playerid);
                return true;
            }
            Phone_RunCommand(playerid, dialogid == DIALOG_PHONE_CALL ? ("/pozovi") : ("/sms"), inputtext);
        }
        case DIALOG_PHONE_DIRECTORY:
        {
            if (!response)
            {
                Phone_ShowMenu(playerid);
                return true;
            }
            Phone_SearchDirectory(playerid, inputtext);
        }
        case DIALOG_PHONE_TOPUP:
        {
            if (!response || listitem < 0 || listitem >= sizeof(g_PhoneTopups))
            {
                if (g_Phone[playerid][phTopupFromBank])
                {
                    Phone_ShowMenu(playerid);
                }
                return true;
            }
            Phone_Topup(playerid, g_PhoneTopups[listitem]);
        }
        case DIALOG_PHONE_SIM:
        {
            if (!response || !Point_Require(playerid, POINT_SHOP))
            {
                return true;
            }
            if (listitem == 0)
            {
                Phone_RequestNumber(playerid, Phone_RandomNumber(), NUMBER_SIM_RANDOM);
            }
            else
            {
                Phone_ShowNumberInput(playerid, NUMBER_SIM_CHOSEN);
            }
        }
        case DIALOG_PHONE_NUMBER:
        {
            new E_NUMBER_REQUEST:request = g_Phone[playerid][phNumberRequest];
            if (!response)
            {
                if (request == NUMBER_CHANGE) Phone_ShowMenu(playerid);
                else Phone_ShowSimMenu(playerid);
                return true;
            }
            // The SIM is bought at the counter, so the player must still be there; a number change works anywhere.
            if (request == NUMBER_SIM_CHOSEN && !Point_Require(playerid, POINT_SHOP))
            {
                return true;
            }
            if (request == NUMBER_CHANGE && g_Player[playerid][pPhoneNumber] == 0)
            {
                return true;
            }
            new number = Phone_ParseNumber(inputtext);
            if (number == 0)
            {
                Phone_ShowNumberInput(playerid, request, C_RED "Broj mora imati tacno 6 cifara, od 100000 do 999999.");
                return true;
            }
            Phone_RequestNumber(playerid, number, request);
        }
        case DIALOG_CONTACTS:
        {
            if (!response)
            {
                return true;
            }
            if (listitem == 0)
            {
                ShowPlayerDialog(playerid, DIALOG_CONTACT_ADD, DIALOG_STYLE_INPUT, "Novi kontakt",
                    C_WHITE "Unesite broj i ime, na primer: " C_SERVER "123456 Marko", "Sacuvaj", "Nazad");
                return true;
            }
            new slot = Phone_ContactSlot(playerid, listitem - 1);
            if (slot == -1)
            {
                return true;
            }
            g_ContactSelected[playerid] = slot;
            ShowPlayerDialog(playerid, DIALOG_CONTACT_ACTION, DIALOG_STYLE_LIST, g_Contacts[playerid][slot][ctName], "Pozovi\nPosalji poruku\nObrisi", "Izaberi", "Nazad");
        }
        case DIALOG_CONTACT_ADD:
        {
            if (!response)
            {
                Phone_ShowContacts(playerid);
                return true;
            }
            new number, name[MAX_PLAYER_NAME + 1], slot = -1;
            if (sscanf(inputtext, "ds[25]", number, name) || number < PHONE_NUMBER_MIN || number > PHONE_NUMBER_MAX)
            {
                Msg_Error(playerid, "Unesite broj od 6 cifara i ime, na primer: 123456 Marko");
                return true;
            }
            for (new i = 0; i < MAX_CONTACTS; i++)
            {
                if (!g_Contacts[playerid][i][ctDbId])
                {
                    slot = i;
                    break;
                }
            }
            if (slot == -1)
            {
                Msg_Error(playerid, "Imenik je pun (%d kontakata).", MAX_CONTACTS);
                return true;
            }

            Chat_StripColors(name);
            g_Contacts[playerid][slot][ctDbId] = -1;
            g_Contacts[playerid][slot][ctNumber] = number;
            strcopy(g_Contacts[playerid][slot][ctName], name, MAX_PLAYER_NAME + 1);

            new query[160];
            mysql_format(g_SQL, query, sizeof(query), "INSERT INTO `phone_contacts` (`account_id`, `name`, `number`) VALUES (%d, '%e', %d)",
                g_Player[playerid][pDbId], name, number);
            mysql_tquery(g_SQL, query, "OnContactAdded", "ddd", playerid, g_AccountRace[playerid], slot);
            Msg_Success(playerid, "Kontakt %s (%d) je sacuvan.", name, number);
        }
        case DIALOG_CONTACT_ACTION:
        {
            new slot = g_ContactSelected[playerid];
            if (!response || !g_Contacts[playerid][slot][ctDbId])
            {
                Phone_ShowContacts(playerid);
                return true;
            }
            switch (listitem)
            {
                case 0:
                {
                    new number[8];
                    valstr(number, g_Contacts[playerid][slot][ctNumber]);
                    Phone_RunCommand(playerid, "/pozovi", number);
                }
                case 1:
                {
                    new text[96];
                    format(text, sizeof(text), C_WHITE "Poruka za %s (%d):", g_Contacts[playerid][slot][ctName], g_Contacts[playerid][slot][ctNumber]);
                    ShowPlayerDialog(playerid, DIALOG_CONTACT_SMS, DIALOG_STYLE_INPUT, "Poruka", text, "Posalji", "Nazad");
                }
                default:
                {
                    new query[96];
                    mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `phone_contacts` WHERE `id` = %d AND `account_id` = %d", g_Contacts[playerid][slot][ctDbId], g_Player[playerid][pDbId]);
                    mysql_tquery(g_SQL, query);
                    Msg_Info(playerid, "Kontakt %s je obrisan.", g_Contacts[playerid][slot][ctName]);
                    g_Contacts[playerid][slot][ctDbId] = 0;
                    Phone_ShowContacts(playerid);
                }
            }
        }
        case DIALOG_CONTACT_SMS:
        {
            new slot = g_ContactSelected[playerid];
            if (!response || isnull(inputtext) || !g_Contacts[playerid][slot][ctDbId])
            {
                Phone_ShowContacts(playerid);
                return true;
            }
            new input[144];
            format(input, sizeof(input), "%d %s", g_Contacts[playerid][slot][ctNumber], inputtext);
            Phone_RunCommand(playerid, "/sms", input);
        }
        default:
        {
            return false;
        }
    }
    return true;
}

forward OnContactAdded(playerid, race, slot);
public OnContactAdded(playerid, race, slot)
{
    if (race == g_AccountRace[playerid] && g_Contacts[playerid][slot][ctDbId] == -1)
    {
        g_Contacts[playerid][slot][ctDbId] = cache_insert_id();
    }
    return 1;
}

flags:sms(CMD_FLAG_CHAT)
