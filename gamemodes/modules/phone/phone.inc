#define MAX_CONTACTS            (20)
#define PHONE_RING_SECONDS      (30)
#define PHONE_NUMBER_MIN        (100000)
#define PHONE_NUMBER_MAX        (999999)
#define PHONE_EMERGENCY         (911)

enum E_PHONE_CALL
{
    CALL_NONE,
    CALL_DIALING,
    CALL_RINGING,
    CALL_ACTIVE
}

enum E_PHONE_STATE
{
    bool:phOff,
    E_PHONE_CALL:phCall,
    phPartner,
    phCallStart,
    phLastCharge,
    phNumberTries
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

// What a listener's phone shows for a number: the saved contact name, or the bare number.
stock Phone_Label(listener, number)
{
    new label[MAX_PLAYER_NAME + 1];
    for (new i = 0; i < MAX_CONTACTS; i++)
    {
        if (g_Contacts[listener][i][ctDbId] && g_Contacts[listener][i][ctNumber] == number)
        {
            strcopy(label, g_Contacts[listener][i][ctName]);
            return label;
        }
    }
    format(label, sizeof(label), "%d", number);
    return label;
}

// Prints the reason and returns false when the player cannot use the phone right now.
stock bool:Phone_Require(playerid)
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
    if (g_Player[playerid][pPhoneNumber] == 0)
    {
        Msg_Error(playerid, "Vas telefon jos nema broj, pokusajte za trenutak.");
        Phone_AssignNumber(playerid);
        return false;
    }
    return true;
}

stock Phone_AssignNumber(playerid)
{
    new number = PHONE_NUMBER_MIN + random(PHONE_NUMBER_MAX - PHONE_NUMBER_MIN + 1), query[96];
    mysql_format(g_SQL, query, sizeof(query), "SELECT 1 FROM `accounts` WHERE `phone_number` = %d LIMIT 1", number);
    mysql_tquery(g_SQL, query, "OnPhoneNumberChecked", "dddd", playerid, g_AccountRace[playerid], number, 0);
}

// requested: 1 when a VIP picked the number, so a taken number is reported instead of drawing a new one.
forward OnPhoneNumberChecked(playerid, race, number, requested);
public OnPhoneNumberChecked(playerid, race, number, requested)
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }
    if (cache_num_rows() > 0 || Phone_FindByNumber(number) != INVALID_PLAYER_ID)
    {
        if (requested)
        {
            Msg_Error(playerid, "Broj %d je zauzet, izaberite drugi.", number);
        }
        else if (++g_Phone[playerid][phNumberTries] < 10)
        {
            Phone_AssignNumber(playerid);
        }
        return 1;
    }

    Phone_EndCall(playerid, "Poziv je prekinut zbog promene broja.");
    g_Player[playerid][pPhoneNumber] = number;
    Account_Save(playerid);
    Msg_Info(playerid, "Vas broj telefona je %d.", number);
    return 1;
}

stock Phone_OnPhoneBought(playerid)
{
    if (g_Player[playerid][pPhoneNumber] == 0)
    {
        Phone_AssignNumber(playerid);
    }
    else
    {
        Msg_Info(playerid, "Vas broj telefona je %d.", g_Player[playerid][pPhoneNumber]);
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
        // The caller pays per started minute.
        if (g_Phone[i][phCall] == CALL_ACTIVE && now - g_Phone[i][phLastCharge] >= 60 && g_Phone[i][phCallStart] == -1)
        {
            if (g_Player[i][pCash] < PHONE_CALL_MINUTE_PRICE)
            {
                Phone_EndCall(i, "Poziv je prekinut: nemate novca za sledeci minut.");
                continue;
            }
            Money_GiveCash(i, -PHONE_CALL_MINUTE_PRICE, "telefonski poziv");
            g_Phone[i][phLastCharge] = now;
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

cmd:pozovi(playerid, params[])
{
    if (!Phone_Require(playerid))
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
        Chat_Action(playerid, "vadi telefon i zove hitne sluzbe.");
        Phone_Emergency(playerid, text);
        return 1;
    }
    if (number == g_Player[playerid][pPhoneNumber])
    {
        Msg_Error(playerid, "Ne mozete pozvati sami sebe.");
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
    if (g_Player[playerid][pCash] < PHONE_CALL_MINUTE_PRICE)
    {
        Msg_Error(playerid, "Nemate novca za poziv ($%d po minutu).", PHONE_CALL_MINUTE_PRICE);
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
    if (g_Player[callerid][pCash] < PHONE_CALL_MINUTE_PRICE)
    {
        Phone_EndCall(callerid, "Poziv je prekinut: nemate novca za poziv.");
        return 1;
    }
    Money_GiveCash(callerid, -PHONE_CALL_MINUTE_PRICE, "telefonski poziv");

    // phCallStart -1 marks the paying side of an active call.
    g_Phone[callerid][phCall] = CALL_ACTIVE;
    g_Phone[callerid][phCallStart] = -1;
    g_Phone[callerid][phLastCharge] = gettime();
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
        Msg_Usage(playerid, "/sms [broj] [poruka] ($%d po poruci)", PHONE_SMS_PRICE);
        return 1;
    }

    new targetid = Phone_FindByNumber(number);
    if (!Phone_IsReachable(targetid) || targetid == playerid)
    {
        Msg_Info(playerid, "Poruka nije isporucena: broj nije dostupan.");
        return 1;
    }
    if (!Money_Take(playerid, PHONE_SMS_PRICE, "SMS"))
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

cmd:telefon(playerid, params[])
{
    if (!Inv_Has(playerid, ITEM_PHONE))
    {
        Msg_Error(playerid, "Nemate telefon. Kupuje se u prodavnici 24/7.");
        return 1;
    }
    g_Phone[playerid][phOff] = !g_Phone[playerid][phOff];
    if (g_Phone[playerid][phOff])
    {
        Phone_EndCall(playerid, "Poziv je prekinut.");
    }
    Msg_Info(playerid, "Telefon je %s. Vas broj: %d.", g_Phone[playerid][phOff] ? ("iskljucen") : ("ukljucen"), g_Player[playerid][pPhoneNumber]);
    return 1;
}

stock Phone_ShowContacts(playerid)
{
    new body[1024], line[64];
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
            ShowPlayerDialog(playerid, DIALOG_CONTACT_ACTION, DIALOG_STYLE_LIST, g_Contacts[playerid][slot][ctName], "Pozovi\nObrisi", "Izaberi", "Nazad");
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
            if (listitem == 0)
            {
                new command[16];
                format(command, sizeof(command), "/pozovi %d", g_Contacts[playerid][slot][ctNumber]);
                PC_EmulateCommand(playerid, command);
                return true;
            }

            new query[96];
            mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `phone_contacts` WHERE `id` = %d AND `account_id` = %d", g_Contacts[playerid][slot][ctDbId], g_Player[playerid][pDbId]);
            mysql_tquery(g_SQL, query);
            Msg_Info(playerid, "Kontakt %s je obrisan.", g_Contacts[playerid][slot][ctName]);
            g_Contacts[playerid][slot][ctDbId] = 0;
            Phone_ShowContacts(playerid);
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

cmd:promenibroj(playerid, params[])
{
    if (!Vip_HasLevel(playerid, VIP_SILVER))
    {
        return 1;
    }
    new number;
    if (sscanf(params, "d", number) || number < PHONE_NUMBER_MIN || number > PHONE_NUMBER_MAX)
    {
        Msg_Usage(playerid, "/promenibroj [broj od 6 cifara] (Silver i Gold VIP)");
        return 1;
    }
    if (!Inv_Has(playerid, ITEM_PHONE))
    {
        Msg_Error(playerid, "Nemate telefon.");
        return 1;
    }

    new query[96];
    mysql_format(g_SQL, query, sizeof(query), "SELECT 1 FROM `accounts` WHERE `phone_number` = %d LIMIT 1", number);
    mysql_tquery(g_SQL, query, "OnPhoneNumberChecked", "dddd", playerid, g_AccountRace[playerid], number, 1);
    return 1;
}

flags:sms(CMD_FLAG_CHAT)
flags:pozovi(CMD_FLAG_CHAT)
