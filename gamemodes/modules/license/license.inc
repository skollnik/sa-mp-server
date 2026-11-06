#define LIC_QUESTIONS           (8)
#define LIC_MAX_ERRORS          (1)
#define LIC_MIN_HEALTH          (850.0)
#define LIC_OUT_LIMIT_S         (20)
// A moment over the limit is forgiven; this many seconds in a row fail the test.
#define LIC_FAST_LIMIT_S        (3)
#define LIC_ROUTE_MAX           (80)
#define LIC_SIZE_ROAD           (6.0)
#define LIC_SIZE_NARROW         (3.0)
#define LIC_SIZE_BOAT           (8.0)
#define LIC_SIZE_AIR            (15.0)
// A far teleport can take the client a few seconds to load; until then the vehicle cannot be entered.
#define LIC_SEAT_RETRY_MS       (500)
#define LIC_SEAT_TRIES          (16)
#define LIC_LABEL_DISTANCE      (30.0)

enum E_LIC_QUESTION
{
    E_LIC_QUESTIONS:lqSet,
    lqText[80],
    lqA[48],
    lqB[48],
    lqC[48],
    lqRight
}

static const g_LicQuestions[][E_LIC_QUESTION] =
{
    { LIC_Q_ROAD, "Sta znaci crveno svetlo na semaforu?", "Stanite i sacekajte zeleno", "Prodjite ako nema nikoga", "Ubrzajte pre zutog", 0 },
    { LIC_Q_ROAD, "Kojom stranom puta se vozi?", "Levom", "Desnom", "Sredinom", 1 },
    { LIC_Q_ROAD, "Iza vas je vozilo hitne sluzbe sa sirenom. Sta radite?", "Ubrzam da mu ne smetam", "Stanem na sredini puta", "Sklonim se udesno i propustim ga", 2 },
    { LIC_Q_ROAD, "Ko ima prednost na raskrsnici bez znakova?", "Vozilo sa desne strane", "Vozilo sa leve strane", "Brze vozilo", 0 },
    { LIC_Q_ROAD, "Sme li se voziti posle alkohola?", "Da, ako je malo", "Ne", "Samo nocu", 1 },
    { LIC_Q_ROAD, "Sta znaci znak STOP?", "Usporite", "Zabranjen prolaz", "Obavezno stanite, pa nastavite kad je bezbedno", 2 },
    { LIC_Q_ROAD, "Kada se pale svetla?", "Nocu i po losem vremenu", "Samo na autoputu", "Samo kad pada kisa", 0 },
    { LIC_Q_ROAD, "Sta radite pre skretanja?", "Zatrubim", "Ukljucim migavac i pogledam u retrovizor", "Naglo skrenem", 1 },
    { LIC_Q_ROAD, "Pesak je na pesackom prelazu. Sta radite?", "Zatrubim da se skloni", "Obidjem ga", "Stanem i propustim ga", 2 },
    { LIC_Q_ROAD, "Sme li se koristiti telefon u ruci tokom voznje?", "Ne", "Da", "Samo u gradu", 0 },
    { LIC_Q_ROAD, "Kada je preticanje zabranjeno?", "Nikad", "Preko pune linije i u nepreglednoj krivini", "Uvek je dozvoljeno", 1 },
    { LIC_Q_ROAD, "Sta mora da ima vozilo kojim vozite?", "Kanister", "Rezervne gume", "Registraciju i tablice", 2 },
    { LIC_Q_ROAD, "Ko mora da bude vezan pojasom u automobilu?", "Samo vozac", "Vozac i svi putnici", "Samo putnik napred", 1 },
    { LIC_Q_ROAD, "Sta je obavezno za vozaca motora?", "Kaciga", "Rukavice", "Zvono", 0 },
    { LIC_Q_BOAT, "Dva broda idu jedan prema drugom. Sta rade?", "Oba skrecu udesno", "Oba skrecu ulevo", "Brzi prolazi prvi", 0 },
    { LIC_Q_BOAT, "Sta je obavezno na brodu za svakog putnika?", "Pecaroski pribor", "Prsluk za spasavanje", "Kanister", 1 },
    { LIC_Q_BOAT, "Smete li brodom da pridjete kupacima kod plaze?", "Da, polako", "Da, ako trubite", "Ne, drzi se rastojanje", 2 },
    { LIC_Q_BOAT, "Neko je pao u vodu. Sta radite?", "Usporim, okrenem se i bacim pojas", "Nastavim dalje", "Ubrzam po pomoc", 0 },
    { LIC_Q_BOAT, "Sme li se upravljati brodom posle alkohola?", "Da, na otvorenom moru", "Ne", "Samo danju", 1 },
    { LIC_Q_BOAT, "Kako se ulazi u luku?", "Punom brzinom", "Bocno", "Polako, malom brzinom", 2 },
    { LIC_Q_BOAT, "Sta proveravate pre isplovljavanja?", "Vreme, gorivo i opremu", "Samo radio", "Nista", 0 },
    { LIC_Q_BOAT, "Sta znaci crveno svetlo na brodu nocu?", "Desnu stranu broda", "Levu stranu broda", "Kvar na brodu", 1 },
    { LIC_Q_AIR, "Sta proveravate pre poletanja?", "Samo radio", "Nista", "Gorivo, vreme i letelicu", 2 },
    { LIC_Q_AIR, "Gde se sme sleteti?", "Na aerodrom, heliodrom ili odobreno mesto", "Bilo gde u gradu", "Na krov svake zgrade", 0 },
    { LIC_Q_AIR, "Koliko nisko se sme leteti iznad grada?", "Izmedju zgrada", "Samo na bezbednoj visini iznad zgrada", "Sto nize", 1 },
    { LIC_Q_AIR, "Ostajete bez goriva. Sta radite?", "Nastavim dok ima", "Iskljucim motor u vazduhu", "Hitno sletim na najblize bezbedno mesto", 2 },
    { LIC_Q_AIR, "Ko ima prednost na pisti?", "Letelica koja slece", "Letelica koja polece", "Veca letelica", 0 },
    { LIC_Q_AIR, "Sme li se leteti posle alkohola?", "Da, ako je let kratak", "Ne", "Samo helikopterom", 1 },
    { LIC_Q_AIR, "Sta radite pre sletanja?", "Ugasim svetla", "Ubrzam", "Usporim i proverim da je mesto slobodno", 2 },
    { LIC_Q_AIR, "Iznad cega ne smete leteti nisko?", "Iznad ljudi i prometnih ulica", "Iznad mora", "Iznad pustinje", 0 }
};

// Default routes, written to the database the first time; admins change them in game with /rutaskole.
// The road tests leave the school lot in El Corona, loop through the city and come back to its south entrance.
static const Float:g_LicDefaultCity[][3] =
{
    { 2084.29, -1814.85, 12.90 },
    { 2114.69, -1395.56, 23.35 },
    { 1998.30, -1338.71, 23.34 },
    { 1853.11, -1271.92, 12.92 },
    { 1730.43, -1298.07, 12.94 },
    { 1607.25, -1297.44, 16.80 },
    { 1601.66, -1383.14, 14.03 },
    { 1643.13, -1443.35, 12.90 },
    { 1656.46, -1521.49, 12.90 },
    { 1734.00, -1599.84, 12.90 },
    { 1809.17, -1614.60, 12.87 },
    { 1819.55, -1765.34, 12.90 },
    { 1980.33, -1934.52, 12.90 },
    { 2056.48, -1920.80, 13.07 }
};

static const Float:g_LicDefaultBoat[][3] =
{
    { 100.0, -1880.0, 0.0 },
    { 105.0, -1960.0, 0.0 },
    { 150.0, -2030.0, 0.0 },
    { 250.0, -2050.0, 0.0 },
    { 330.0, -2110.0, 0.0 },
    { 380.0, -2130.0, 0.0 },
    { 440.0, -2100.0, 0.0 },
    { 560.0, -2060.0, 0.0 },
    { 680.0, -2080.0, 0.0 },
    { 560.0, -2130.0, 0.0 },
    { 380.0, -2160.0, 0.0 },
    { 200.0, -2100.0, 0.0 },
    { 120.0, -2010.0, 0.0 },
    { 100.0, -1920.0, 0.0 },
    { 112.0, -1850.0, 0.0 }
};

// The helicopter starts on open tarmac away from the boarding bridges, which its rotor would hit.
static const Float:g_LicDefaultAir[][3] =
{
    { 1600.0, -2460.0, 40.0 },
    { 1800.0, -2500.0, 70.0 },
    { 2050.0, -2400.0, 80.0 },
    { 1900.0, -2250.0, 80.0 },
    { 1600.0, -2250.0, 70.0 },
    { 1450.0, -2400.0, 45.0 },
    { 1500.0, -2560.0, 35.0 },
    { 1600.0, -2460.0, 14.5 }
};

// x, y, z and facing angle where the test vehicle appears.
static const Float:g_LicDefaultStart[E_LICENSE][4] =
{
    { 2066.47, -1913.96, 13.40, 90.0 },
    { 2066.47, -1913.96, 13.20, 90.0 },
    { 2066.47, -1913.96, 13.60, 90.0 },
    { 112.0, -1837.0, 0.0, 180.0 },
    { 1600.0, -2460.0, 13.5, 90.0 },
    { 0.0, 0.0, 0.0, 0.0 }
};

new Float:g_LicRoute[E_LICENSE][LIC_ROUTE_MAX][4];
new g_LicRouteCount[E_LICENSE];
new Float:g_LicStart[E_LICENSE][4];

enum E_LIC_STAGE
{
    LIC_STAGE_NONE,
    LIC_STAGE_THEORY,
    LIC_STAGE_DRIVE
}

enum E_LIC_TEST
{
    E_LIC_STAGE:ltStage,
    E_LICENSE:ltLicense,
    ltQuestion,
    ltErrors,
    ltOrder[LIC_QUESTIONS],
    ltCount,
    ltVehicle,
    STREAMER_TAG_3D_TEXT_LABEL:ltLabel,
    bool:ltSeated,
    ltPoint,
    ltStartedAt,
    ltOutSeconds,
    ltFastSeconds,
    Float:ltReturnX,
    Float:ltReturnY,
    Float:ltReturnZ,
    Float:ltReturnA,
    ltReturnInterior,
    ltReturnWorld
}

new g_LicTest[MAX_PLAYERS][E_LIC_TEST];
new E_LICENSE:g_LicChosen[MAX_PLAYERS];
new g_LicTimer;

stock bool:Lic_Has(playerid, E_LICENSE:license)
{
    return (g_Player[playerid][pLicenses] & (1 << _:license)) != 0;
}

stock Lic_Set(playerid, E_LICENSE:license, bool:has)
{
    if (has)
    {
        g_Player[playerid][pLicenses] |= 1 << _:license;
    }
    else
    {
        g_Player[playerid][pLicenses] &= ~(1 << _:license);
    }
    new query[96];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `accounts` SET `licenses` = %d WHERE `id` = %d", g_Player[playerid][pLicenses], g_Player[playerid][pDbId]);
    mysql_tquery(g_SQL, query);
}

// The license a vehicle needs, or LIC_NONE (bicycles, trailers).
stock E_LICENSE:Lic_ForModel(model)
{
    switch (Veh_Class(model))
    {
        case VEH_CLASS_MOTO: return LIC_MOTO;
        case VEH_CLASS_CAR, VEH_CLASS_VAN: return LIC_CAR;
        case VEH_CLASS_TRUCK: return LIC_TRUCK;
        case VEH_CLASS_BOAT: return LIC_BOAT;
        case VEH_CLASS_AIR: return LIC_FLY;
        default: return LIC_NONE;
    }
    return LIC_NONE;
}

stock E_LICENSE:Lic_FromKey(const key[])
{
    for (new E_LICENSE:l; l < E_LICENSE; l++)
    {
        if (!strcmp(key, g_LicInfo[l][liKey], true) && !isnull(key))
        {
            return l;
        }
    }
    return LIC_NONE;
}

// "Auto (B), Motor (A)" or "nema", for /stats and /dozvole.
stock Lic_Summary(playerid, dest[], size = sizeof(dest))
{
    dest[0] = '\0';
    for (new E_LICENSE:l; l < E_LICENSE; l++)
    {
        if (Lic_Has(playerid, l))
        {
            if (dest[0])
            {
                strcat(dest, ", ", size);
            }
            strcat(dest, g_LicInfo[l][liName], size);
        }
    }
    if (!dest[0])
    {
        strcopy(dest, "nema", size);
    }
}

static stock Lic_SaveRoutePoint(E_LICENSE:license, index)
{
    new query[256], Float:angle = index == 0 ? g_LicStart[license][3] : 0.0;
    new Float:x, Float:y, Float:z, Float:size;
    if (index == 0)
    {
        x = g_LicStart[license][0];
        y = g_LicStart[license][1];
        z = g_LicStart[license][2];
    }
    else
    {
        x = g_LicRoute[license][index - 1][0];
        y = g_LicRoute[license][index - 1][1];
        z = g_LicRoute[license][index - 1][2];
        size = g_LicRoute[license][index - 1][3];
    }
    mysql_format(g_SQL, query, sizeof(query),
        "REPLACE INTO `license_routes` (`license`, `idx`, `x`, `y`, `z`, `a`, `size`) VALUES (%d, %d, %f, %f, %f, %f, %f)",
        _:license, index, x, y, z, angle, size);
    mysql_tquery(g_SQL, query);
}

static stock bool:Lic_AddPoint(E_LICENSE:license, Float:x, Float:y, Float:z, Float:size)
{
    new count = g_LicRouteCount[license];
    if (count >= LIC_ROUTE_MAX)
    {
        return false;
    }
    g_LicRoute[license][count][0] = x;
    g_LicRoute[license][count][1] = y;
    g_LicRoute[license][count][2] = z;
    g_LicRoute[license][count][3] = size;
    g_LicRouteCount[license]++;
    Lic_SaveRoutePoint(license, count + 1);
    return true;
}

static stock Lic_AddDefaults(E_LICENSE:license, const Float:points[][3], count, Float:size)
{
    for (new i = 0; i < count; i++)
    {
        Lic_AddPoint(license, points[i][0], points[i][1], points[i][2], size);
    }
}

static stock Lic_SeedRoute(E_LICENSE:license)
{
    new query[80];
    mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `license_routes` WHERE `license` = %d", _:license);
    mysql_tquery(g_SQL, query);
    g_LicRouteCount[license] = 0;
    for (new i = 0; i < 4; i++)
    {
        g_LicStart[license][i] = g_LicDefaultStart[license][i];
    }
    Lic_SaveRoutePoint(license, 0);
    switch (license)
    {
        case LIC_CAR, LIC_MOTO, LIC_TRUCK: Lic_AddDefaults(license, g_LicDefaultCity, sizeof(g_LicDefaultCity), LIC_SIZE_ROAD);
        case LIC_BOAT: Lic_AddDefaults(license, g_LicDefaultBoat, sizeof(g_LicDefaultBoat), LIC_SIZE_BOAT);
        case LIC_FLY: Lic_AddDefaults(license, g_LicDefaultAir, sizeof(g_LicDefaultAir), LIC_SIZE_AIR);
        default: {}
    }
}

static stock bool:Lic_HasTest(E_LICENSE:license)
{
    return g_LicInfo[license][liModel] != 0;
}

stock Lic_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `license_routes` (" \
        "`license` TINYINT UNSIGNED NOT NULL," \
        "`idx` SMALLINT UNSIGNED NOT NULL," \
        "`x` FLOAT NOT NULL, `y` FLOAT NOT NULL, `z` FLOAT NOT NULL," \
        "`a` FLOAT NOT NULL DEFAULT 0," \
        "`size` FLOAT NOT NULL DEFAULT 0," \
        "PRIMARY KEY (`license`, `idx`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
    mysql_tquery(g_SQL, "SELECT * FROM `license_routes` ORDER BY `license`, `idx`", "OnLicenseRoutesLoaded");
    g_LicTimer = SetTimer("Lic_Tick", 1000, true);
}

stock Lic_Exit()
{
    KillTimer(g_LicTimer);
}

// Index 0 is the start (with the facing angle), the rest are the checkpoints in order.
forward OnLicenseRoutesLoaded();
public OnLicenseRoutesLoaded()
{
    new bool:has_start[E_LICENSE], license, index;
    for (new row = 0; row < cache_num_rows(); row++)
    {
        cache_get_value_name_int(row, "license", license);
        cache_get_value_name_int(row, "idx", index);
        if (license < 0 || license >= _:E_LICENSE)
        {
            continue;
        }
        new E_LICENSE:l = E_LICENSE:license, Float:x, Float:y, Float:z;
        cache_get_value_name_float(row, "x", x);
        cache_get_value_name_float(row, "y", y);
        cache_get_value_name_float(row, "z", z);
        if (index == 0)
        {
            g_LicStart[l][0] = x;
            g_LicStart[l][1] = y;
            g_LicStart[l][2] = z;
            cache_get_value_name_float(row, "a", g_LicStart[l][3]);
            has_start[l] = true;
        }
        else if (g_LicRouteCount[l] < LIC_ROUTE_MAX)
        {
            new count = g_LicRouteCount[l]++;
            g_LicRoute[l][count][0] = x;
            g_LicRoute[l][count][1] = y;
            g_LicRoute[l][count][2] = z;
            cache_get_value_name_float(row, "size", g_LicRoute[l][count][3]);
        }
    }
    for (new E_LICENSE:l; l < E_LICENSE; l++)
    {
        if (Lic_HasTest(l) && !has_start[l])
        {
            Lic_SeedRoute(l);
        }
    }
    return 1;
}

stock Lic_OnPlayerConnect(playerid)
{
    static const empty[E_LIC_TEST];
    g_LicTest[playerid] = empty;
    g_LicTest[playerid][ltVehicle] = INVALID_VEHICLE_ID;
}

stock Lic_ShowList(playerid)
{
    new body[640], line[96];
    strcopy(body, "Dozvola\tCena\tUslov\n");
    for (new E_LICENSE:l; l < E_LICENSE; l++)
    {
        if (Lic_Has(playerid, l))
        {
            format(line, sizeof(line), "%s\t" C_GREEN "imate" C_WHITE "\t-\n", g_LicInfo[l][liName]);
        }
        else if (g_LicInfo[l][liRequires] != LIC_NONE)
        {
            format(line, sizeof(line), "%s\t$%d\tnivo %d, %s\n", g_LicInfo[l][liName], g_LicInfo[l][liPrice], g_LicInfo[l][liLevel], g_LicInfo[g_LicInfo[l][liRequires]][liName]);
        }
        else
        {
            format(line, sizeof(line), "%s\t$%d\tnivo %d%s\n", g_LicInfo[l][liName], g_LicInfo[l][liPrice], g_LicInfo[l][liLevel], Lic_HasTest(l) ? (", test i voznja") : (", bez testa"));
        }
        strcat(body, line);
    }
    ShowPlayerDialog(playerid, DIALOG_LIC_LIST, DIALOG_STYLE_TABLIST_HEADERS, "Auto skola - dozvole", body, "Izaberi", "Zatvori");
}

// Told the reason when it cannot be taken; checked again after the confirmation, since money or level may change.
static stock bool:Lic_CanTake(playerid, E_LICENSE:license)
{
    if (Lic_Has(playerid, license))
    {
        Msg_Info(playerid, "Vec imate dozvolu: %s.", g_LicInfo[license][liName]);
    }
    else if (g_LicTest[playerid][ltStage] != LIC_STAGE_NONE)
    {
        Msg_Error(playerid, "Vec polazete ispit.");
    }
    else if (g_Player[playerid][pLevel] < g_LicInfo[license][liLevel])
    {
        Msg_Error(playerid, "Za ovu dozvolu treba nivo %d.", g_LicInfo[license][liLevel]);
    }
    else if (g_LicInfo[license][liRequires] != LIC_NONE && !Lic_Has(playerid, g_LicInfo[license][liRequires]))
    {
        Msg_Error(playerid, "Prvo vam treba dozvola: %s.", g_LicInfo[g_LicInfo[license][liRequires]][liName]);
    }
    else if (Lic_HasTest(license) && g_LicRouteCount[license] < 2)
    {
        Msg_Error(playerid, "Ispit za ovu dozvolu trenutno nije moguc (ruta nije postavljena). Javite adminu.");
    }
    else if (g_Player[playerid][pCash] < g_LicInfo[license][liPrice])
    {
        Msg_Error(playerid, "Nemate dovoljno novca (potrebno je $%d).", g_LicInfo[license][liPrice]);
    }
    else
    {
        return true;
    }
    return false;
}

static stock Lic_Confirm(playerid, E_LICENSE:license)
{
    if (!Lic_CanTake(playerid, license))
    {
        return;
    }
    g_LicChosen[playerid] = license;
    new text[512];
    if (!Lic_HasTest(license))
    {
        format(text, sizeof(text), "Dozvola: %s\nCena: $%d\n\nDozvola se izdaje odmah i vazi trajno.", g_LicInfo[license][liName], g_LicInfo[license][liPrice]);
        ShowPlayerDialog(playerid, DIALOG_LIC_CONFIRM, DIALOG_STYLE_MSGBOX, "Auto skola", text, "Plati", "Nazad");
        return;
    }
    new speed[48];
    if (g_LicInfo[license][liMaxSpeed] > 0)
    {
        format(speed, sizeof(speed), "\n- ne smete voziti brze od %d km/h", g_LicInfo[license][liMaxSpeed]);
    }
    format(text, sizeof(text),
        "Dozvola: %s\nCena: $%d (placa se odmah; posle pada se placa ponovo)\n\n" \
        "1. Teorija: %d pitanja, dozvoljena je %d greska.\n2. Voznja po oznacenim tackama, za %d minuta:\n" \
        "- vozilo ne sme da se osteti\n- ne smete izaci iz vozila duze od %d s%s\n\nDozvola vazi trajno (moze je oduzeti policija).",
        g_LicInfo[license][liName], g_LicInfo[license][liPrice], LIC_QUESTIONS, LIC_MAX_ERRORS, g_LicInfo[license][liTimeLimit] / 60, LIC_OUT_LIMIT_S, speed);
    ShowPlayerDialog(playerid, DIALOG_LIC_CONFIRM, DIALOG_STYLE_MSGBOX, "Auto skola", text, "Plati i pocni", "Nazad");
}

// Random questions from the license's set, in a random order.
static stock Lic_PickQuestions(playerid, E_LIC_QUESTIONS:set)
{
    new pool[sizeof(g_LicQuestions)], count;
    for (new i = 0; i < sizeof(g_LicQuestions); i++)
    {
        if (g_LicQuestions[i][lqSet] == set)
        {
            pool[count++] = i;
        }
    }
    for (new i = count - 1; i > 0; i--)
    {
        new j = random(i + 1), swap = pool[i];
        pool[i] = pool[j];
        pool[j] = swap;
    }
    g_LicTest[playerid][ltCount] = min(count, LIC_QUESTIONS);
    for (new i = 0; i < g_LicTest[playerid][ltCount]; i++)
    {
        g_LicTest[playerid][ltOrder][i] = pool[i];
    }
}

static stock Lic_ShowQuestion(playerid)
{
    new q = g_LicTest[playerid][ltOrder][g_LicTest[playerid][ltQuestion]], body[256], title[48];
    format(body, sizeof(body), "%s\n%s\n%s\n%s", g_LicQuestions[q][lqText], g_LicQuestions[q][lqA], g_LicQuestions[q][lqB], g_LicQuestions[q][lqC]);
    format(title, sizeof(title), "Teorija: pitanje %d od %d", g_LicTest[playerid][ltQuestion] + 1, g_LicTest[playerid][ltCount]);
    ShowPlayerDialog(playerid, DIALOG_LIC_QUESTION, DIALOG_STYLE_TABLIST_HEADERS, title, body, "Odgovori", "Odustani");
}

static stock Lic_ShowPoint(playerid)
{
    new E_LICENSE:license = g_LicTest[playerid][ltLicense], point = g_LicTest[playerid][ltPoint];
    new count = g_LicRouteCount[license], bool:last = point == count - 1, next = last ? point : point + 1;
    new type = g_LicInfo[license][liAir] ? (last ? 4 : 3) : (last ? 1 : 0);
    SetPlayerRaceCheckpoint(playerid, CP_TYPE:type, g_LicRoute[license][point][0], g_LicRoute[license][point][1], g_LicRoute[license][point][2],
        g_LicRoute[license][next][0], g_LicRoute[license][next][1], g_LicRoute[license][next][2], g_LicRoute[license][point][3]);
}

static stock Lic_StartDrive(playerid)
{
    new E_LICENSE:license = g_LicTest[playerid][ltLicense];
    new vehicleid = CreateVehicle(g_LicInfo[license][liModel], g_LicStart[license][0], g_LicStart[license][1], g_LicStart[license][2], g_LicStart[license][3], 1, 1, -1);
    if (vehicleid == INVALID_VEHICLE_ID)
    {
        Msg_Error(playerid, "Vozilo za ispit trenutno ne moze da se napravi; novac je vracen.");
        Money_GiveCash(playerid, g_LicInfo[license][liPrice], "auto skola: povracaj");
        g_LicTest[playerid][ltStage] = LIC_STAGE_NONE;
        return;
    }
    GetPlayerPos(playerid, g_LicTest[playerid][ltReturnX], g_LicTest[playerid][ltReturnY], g_LicTest[playerid][ltReturnZ]);
    GetPlayerFacingAngle(playerid, g_LicTest[playerid][ltReturnA]);
    g_LicTest[playerid][ltReturnInterior] = GetPlayerInterior(playerid);
    g_LicTest[playerid][ltReturnWorld] = GetPlayerVirtualWorld(playerid);

    g_TestVehicleOwner[vehicleid] = playerid;
    Fuel_Set(vehicleid, float(Fuel_TankSize(GetVehicleModel(vehicleid))));
    new label[48];
    format(label, sizeof(label), "Auto skola\n" C_WHITE "Ispit: %s", g_LicInfo[license][liName]);
    g_LicTest[playerid][ltLabel] = CreateDynamic3DTextLabel(label, COLOR_SERVER, 0.0, 0.0, 1.2, LIC_LABEL_DISTANCE, INVALID_PLAYER_ID, vehicleid, 1);
    g_LicTest[playerid][ltSeated] = false;
    // The school lends a helmet for the motorbike test; /kaciga still puts on the player's own first.
    Safety_LendHelmet(playerid, license == LIC_MOTO);
    g_LicTest[playerid][ltStage] = LIC_STAGE_DRIVE;
    g_LicTest[playerid][ltVehicle] = vehicleid;
    g_LicTest[playerid][ltPoint] = 0;
    g_LicTest[playerid][ltStartedAt] = gettime();
    g_LicTest[playerid][ltOutSeconds] = 0;
    g_LicTest[playerid][ltFastSeconds] = 0;

    // The GPS marker would be a second target next to the test's checkpoint.
    Gps_Stop(playerid);
    // A vehicle the client has not streamed in cannot be entered, so the player is moved next to it first.
    SetPlayerInterior(playerid, 0);
    SetPlayerVirtualWorld(playerid, 0);
    AC_SetPlayerPos(playerid, g_LicStart[license][0], g_LicStart[license][1], g_LicStart[license][2] + 1.0);
    SetTimerEx("Lic_SeatPlayer", LIC_SEAT_RETRY_MS, false, "ddd", playerid, vehicleid, 1);
    Lic_ShowPoint(playerid);
    Msg_Info(playerid, "Instruktor: vozite kroz oznacene tacke. Imate %d minuta; ne ostecujte vozilo.", g_LicInfo[license][liTimeLimit] / 60);
}

forward Lic_SeatPlayer(playerid, vehicleid, tries);
public Lic_SeatPlayer(playerid, vehicleid, tries)
{
    if (g_LicTest[playerid][ltStage] != LIC_STAGE_DRIVE || g_LicTest[playerid][ltVehicle] != vehicleid || g_LicTest[playerid][ltSeated])
    {
        return 1;
    }
    if (IsVehicleStreamedIn(vehicleid, playerid) && GetPlayerVehicleID(playerid) != vehicleid)
    {
        new E_LICENSE:license = g_LicTest[playerid][ltLicense];
        AC_ExpectTeleport(playerid, g_LicStart[license][0], g_LicStart[license][1], g_LicStart[license][2]);
        PutPlayerInVehicle(playerid, vehicleid, 0);
    }
    if (tries < LIC_SEAT_TRIES)
    {
        SetTimerEx("Lic_SeatPlayer", LIC_SEAT_RETRY_MS, false, "ddd", playerid, vehicleid, tries + 1);
    }
    else
    {
        // The player is next to the vehicle and can get in by hand; from here the time out of it counts.
        g_LicTest[playerid][ltSeated] = true;
    }
    return 1;
}

// The engine starts only when the player sits in, so a helicopter rotor never spins next to them.
static stock Lic_OnSeated(playerid)
{
    g_LicTest[playerid][ltSeated] = true;
    Veh_SetParam(g_LicTest[playerid][ltVehicle], VEH_PARAM_ENGINE, true);
}

// bring_back: false on death and disconnect, where the player is not moved.
static stock Lic_EndTest(playerid, bool:bring_back)
{
    new vehicleid = g_LicTest[playerid][ltVehicle];
    g_LicTest[playerid][ltStage] = LIC_STAGE_NONE;
    g_LicTest[playerid][ltVehicle] = INVALID_VEHICLE_ID;
    Safety_LendHelmet(playerid, false);
    if (vehicleid == INVALID_VEHICLE_ID)
    {
        return;
    }
    DisablePlayerRaceCheckpoint(playerid);
    DestroyDynamic3DTextLabel(g_LicTest[playerid][ltLabel]);
    g_TestVehicleOwner[vehicleid] = INVALID_PLAYER_ID;
    DestroyVehicle(vehicleid);
    if (bring_back)
    {
        Utils_MoveInto(playerid, g_LicTest[playerid][ltReturnX], g_LicTest[playerid][ltReturnY], g_LicTest[playerid][ltReturnZ], g_LicTest[playerid][ltReturnA],
            g_LicTest[playerid][ltReturnInterior], g_LicTest[playerid][ltReturnWorld], !g_Staff[playerid][sFrozen]);
    }
}

static stock Lic_Fail(playerid, const reason[])
{
    Msg_Error(playerid, "Pali ste ispit: %s. Ispit mozete polagati ponovo na salteru (/skola).", reason);
    Lic_EndTest(playerid, true);
}

static stock Lic_Grant(playerid, E_LICENSE:license)
{
    Lic_Set(playerid, license, true);
    Msg_Success(playerid, "Dobili ste dozvolu: %s. Vase dozvole: /dozvole.", g_LicInfo[license][liName]);
}

forward Lic_Tick();
public Lic_Tick()
{
    new now = gettime();
    for (new playerid = 0; playerid < MAX_PLAYERS; playerid++)
    {
        if (g_LicTest[playerid][ltStage] != LIC_STAGE_DRIVE || !IsPlayerConnected(playerid))
        {
            continue;
        }
        new E_LICENSE:license = g_LicTest[playerid][ltLicense], vehicleid = g_LicTest[playerid][ltVehicle], Float:health;
        if (!IsValidVehicle(vehicleid))
        {
            Lic_Fail(playerid, "vozilo je unisteno");
            continue;
        }
        if (now - g_LicTest[playerid][ltStartedAt] > g_LicInfo[license][liTimeLimit])
        {
            Lic_Fail(playerid, "isteklo je vreme");
            continue;
        }
        if (g_Player[playerid][pJailSeconds] > 0)
        {
            Lic_Fail(playerid, "zatvoreni ste");
            continue;
        }
        GetVehicleHealth(vehicleid, health);
        if (health < LIC_MIN_HEALTH)
        {
            Lic_Fail(playerid, "ostetili ste vozilo");
            continue;
        }
        if (GetPlayerVehicleID(playerid) != vehicleid)
        {
            if (!g_LicTest[playerid][ltSeated])
            {
                continue;
            }
            new out = ++g_LicTest[playerid][ltOutSeconds];
            if (out > LIC_OUT_LIMIT_S)
            {
                Lic_Fail(playerid, "predugo ste bili van vozila");
            }
            else if (out == 1)
            {
                Msg_Info(playerid, "Vratite se u vozilo za ispit za %d sekundi.", LIC_OUT_LIMIT_S);
            }
            continue;
        }
        g_LicTest[playerid][ltOutSeconds] = 0;
        if (g_LicInfo[license][liMaxSpeed] > 0 && Veh_SpeedKmh(vehicleid) > float(g_LicInfo[license][liMaxSpeed]))
        {
            new fast = ++g_LicTest[playerid][ltFastSeconds];
            if (fast >= LIC_FAST_LIMIT_S)
            {
                Lic_Fail(playerid, "vozili ste prebrzo");
            }
            else if (fast == 1)
            {
                Msg_Info(playerid, "Instruktor: usporite, ogranicenje je %d km/h!", g_LicInfo[license][liMaxSpeed]);
            }
            continue;
        }
        g_LicTest[playerid][ltFastSeconds] = 0;
    }
    return 1;
}

stock Lic_OnPlayerEnterRaceCP(playerid)
{
    if (g_LicTest[playerid][ltStage] != LIC_STAGE_DRIVE || GetPlayerVehicleID(playerid) != g_LicTest[playerid][ltVehicle]
        || GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
    {
        return;
    }
    new E_LICENSE:license = g_LicTest[playerid][ltLicense];
    // Nobody reminds the player: on the road tests the belt or helmet is part of what is being tested.
    if (g_LicInfo[license][liQuestions] == LIC_Q_ROAD && !Safety_IsProtected(playerid, g_LicTest[playerid][ltVehicle]))
    {
        Lic_Fail(playerid, license == LIC_MOTO ? ("vozili ste bez kacige") : ("vozili ste bez pojasa"));
        return;
    }
    PlayerPlaySound(playerid, 1138, 0.0, 0.0, 0.0);
    if (++g_LicTest[playerid][ltPoint] >= g_LicRouteCount[license])
    {
        Lic_EndTest(playerid, true);
        Lic_Grant(playerid, license);
        return;
    }
    Lic_ShowPoint(playerid);
}

// Nobody else may use a test vehicle; a player without the license gets a warning, as is usual on RP servers.
stock Lic_OnPlayerStateChange(playerid, PLAYER_STATE:newstate)
{
    if (newstate != PLAYER_STATE_DRIVER && newstate != PLAYER_STATE_PASSENGER)
    {
        return;
    }
    new vehicleid = GetPlayerVehicleID(playerid), owner = g_TestVehicleOwner[vehicleid];
    if (owner != INVALID_PLAYER_ID && owner != playerid)
    {
        RemovePlayerFromVehicle(playerid);
        Msg_Error(playerid, "Ovo vozilo je za vozacki ispit.");
        return;
    }
    if (owner == playerid && newstate == PLAYER_STATE_DRIVER)
    {
        Lic_OnSeated(playerid);
    }
    if (newstate != PLAYER_STATE_DRIVER || owner == playerid || g_IsAdminVehicle[vehicleid])
    {
        return;
    }
    new E_LICENSE:needed = Lic_ForModel(GetVehicleModel(vehicleid));
    if (needed != LIC_NONE && !Lic_Has(playerid, needed))
    {
        Msg_Info(playerid, "Nemate dozvolu za ovo vozilo (%s). Policija moze da vas kazni; dozvole se polazu u auto skoli (/gps).", g_LicInfo[needed][liName]);
    }
}

stock Lic_OnPlayerDeath(playerid)
{
    if (g_LicTest[playerid][ltStage] == LIC_STAGE_DRIVE)
    {
        Msg_Error(playerid, "Pali ste ispit: poginuli ste.");
    }
    Lic_EndTest(playerid, false);
}

stock Lic_OnPlayerDisconnect(playerid)
{
    Lic_EndTest(playerid, false);
}

stock bool:Lic_OnDialogResponse(playerid, dialogid, response, listitem)
{
    switch (dialogid)
    {
        case DIALOG_LIC_LIST:
        {
            if (response && listitem >= 0 && listitem < _:E_LICENSE && Point_Require(playerid, POINT_SCHOOL))
            {
                Lic_Confirm(playerid, E_LICENSE:listitem);
            }
        }
        case DIALOG_LIC_CONFIRM:
        {
            new E_LICENSE:license = g_LicChosen[playerid];
            if (!response)
            {
                Lic_ShowList(playerid);
                return true;
            }
            if (!Point_Require(playerid, POINT_SCHOOL) || !Lic_CanTake(playerid, license))
            {
                return true;
            }
            new reason[40];
            format(reason, sizeof(reason), "auto skola: %s", g_LicInfo[license][liName]);
            if (!Money_Take(playerid, g_LicInfo[license][liPrice], reason))
            {
                return true;
            }
            if (!Lic_HasTest(license))
            {
                Lic_Grant(playerid, license);
                return true;
            }
            g_LicTest[playerid][ltStage] = LIC_STAGE_THEORY;
            g_LicTest[playerid][ltLicense] = license;
            g_LicTest[playerid][ltQuestion] = 0;
            g_LicTest[playerid][ltErrors] = 0;
            Lic_PickQuestions(playerid, g_LicInfo[license][liQuestions]);
            Lic_ShowQuestion(playerid);
        }
        case DIALOG_LIC_QUESTION:
        {
            if (g_LicTest[playerid][ltStage] != LIC_STAGE_THEORY)
            {
                return true;
            }
            if (!response)
            {
                g_LicTest[playerid][ltStage] = LIC_STAGE_NONE;
                Msg_Error(playerid, "Odustali ste od ispita.");
                return true;
            }
            new q = g_LicTest[playerid][ltOrder][g_LicTest[playerid][ltQuestion]];
            if (listitem != g_LicQuestions[q][lqRight])
            {
                g_LicTest[playerid][ltErrors]++;
            }
            if (++g_LicTest[playerid][ltQuestion] < g_LicTest[playerid][ltCount])
            {
                Lic_ShowQuestion(playerid);
                return true;
            }
            new errors = g_LicTest[playerid][ltErrors];
            if (errors > LIC_MAX_ERRORS)
            {
                g_LicTest[playerid][ltStage] = LIC_STAGE_NONE;
                Msg_Error(playerid, "Pali ste teoriju: %d gresaka (dozvoljena je %d). Ispit mozete polagati ponovo (/skola).", errors, LIC_MAX_ERRORS);
                return true;
            }
            Msg_Success(playerid, "Polozili ste teoriju (%d od %d tacno). Sledi voznja.", g_LicTest[playerid][ltCount] - errors, g_LicTest[playerid][ltCount]);
            Lic_StartDrive(playerid);
        }
        default:
        {
            return false;
        }
    }
    return true;
}

cmd:skola(playerid, params[])
{
    if (Point_Require(playerid, POINT_SCHOOL))
    {
        Lic_ShowList(playerid);
    }
    return 1;
}

cmd:dozvole(playerid, params[])
{
    new list[128];
    Lic_Summary(playerid, list);
    Msg_Info(playerid, "Vase dozvole: %s.", list);
    return 1;
}
flags:dozvole(CMD_FLAG_JAIL_OK)

cmd:pokazidozvole(playerid, params[])
{
    new targetid = Staff_ParseTarget(playerid, params, "/pokazidozvole [id ili ime] (igracu pored vas)", "Svoje dozvole vidite sa /dozvole.");
    if (targetid == INVALID_PLAYER_ID || !Chat_CheckBeside(playerid, targetid, INV_GIVE_RADIUS))
    {
        return 1;
    }
    new list[128];
    Lic_Summary(playerid, list);
    Msg_Info(targetid, "%s vam pokazuje dozvole: %s.", Account_Name(playerid), list);
    Chat_Action(playerid, "pokazuje svoje dozvole.");
    return 1;
}

cmd:setdozvola(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }
    new targetid, key[12], value;
    if (sscanf(params, "us[12]d", targetid, key, value) || (value != 0 && value != 1))
    {
        Msg_Usage(playerid, "/setdozvola [id ili ime] [auto / motor / kamion / brod / letenje / ribolov] [1 daje, 0 oduzima]");
        return 1;
    }
    new E_LICENSE:license = Lic_FromKey(key);
    if (license == LIC_NONE)
    {
        Msg_Error(playerid, "Vrste: auto, motor, kamion, brod, letenje, ribolov.");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }
    Lic_Set(targetid, license, value == 1);
    new details[48];
    format(details, sizeof(details), "%s: %s", g_LicInfo[license][liKey], value == 1 ? ("daje") : ("oduzima"));
    Staff_Log(playerid, "setlicense", targetid, details);
    Msg_Admin(playerid, "Igrac %s: dozvola %s %s.", Account_Name(targetid), g_LicInfo[license][liName], value == 1 ? ("data") : ("oduzeta"));
    if (targetid != playerid)
    {
        Msg_Info(targetid, "Admin %s vam je %s dozvolu: %s.", Account_Name(playerid), value == 1 ? ("dao") : ("oduzeo"), g_LicInfo[license][liName]);
    }
    return 1;
}

static const g_LicRouteMenu[][E_MENU_ITEM] =
{
    { ADMIN_LEVEL_HEAD, "Start: vozilo se pojavljuje ovde", "rutaskole %s start", "" },
    { ADMIN_LEVEL_HEAD, "Dodaj tacku ovde", "rutaskole %s dodaj", "" },
    { ADMIN_LEVEL_HEAD, "Dodaj usku tacku (poligon)", "rutaskole %s usko", "" },
    { ADMIN_LEVEL_HEAD, "Obrisi poslednju tacku", "rutaskole %s obrisi", "" },
    { ADMIN_LEVEL_HEAD, "Obrisi sve tacke", "rutaskole %s ocisti", "Upisite DA za potvrdu:" },
    { ADMIN_LEVEL_HEAD, "Vrati podrazumevanu rutu", "rutaskole %s vrati", "Upisite DA za potvrdu:" }
};

// The admin stands (or sits in a vehicle) where the point goes; a test vehicle starts where the admin's vehicle faces.
cmd:rutaskole(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }
    new key[12], option[12], value[8];
    if (sscanf(params, "s[12]S()[12]S()[8]", key, option, value))
    {
        Msg_Usage(playerid, "/rutaskole [auto / motor / kamion / brod / letenje] (meni za rutu ispita)");
        return 1;
    }
    new E_LICENSE:license = Lic_FromKey(key);
    if (license == LIC_NONE || !Lic_HasTest(license))
    {
        Msg_Error(playerid, "Rute postoje za: auto, motor, kamion, brod, letenje.");
        return 1;
    }
    if (isnull(option))
    {
        new title[64];
        format(title, sizeof(title), "Ruta: %s (%d tacaka)", g_LicInfo[license][liName], g_LicRouteCount[license]);
        Menu_Show(playerid, title, g_LicRouteMenu, sizeof(g_LicRouteMenu), g_LicInfo[license][liKey]);
        return 1;
    }
    new Float:x, Float:y, Float:z, Float:angle, vehicleid = GetPlayerVehicleID(playerid);
    if (vehicleid != 0)
    {
        GetVehiclePos(vehicleid, x, y, z);
        GetVehicleZAngle(vehicleid, angle);
    }
    else
    {
        GetPlayerPos(playerid, x, y, z);
        GetPlayerFacingAngle(playerid, angle);
    }
    if (!strcmp(option, "start", true))
    {
        g_LicStart[license][0] = x;
        g_LicStart[license][1] = y;
        g_LicStart[license][2] = z;
        g_LicStart[license][3] = angle;
        Lic_SaveRoutePoint(license, 0);
    }
    else if (!strcmp(option, "dodaj", true) || !strcmp(option, "usko", true))
    {
        new Float:size = LIC_SIZE_ROAD;
        if (!strcmp(option, "usko", true))
        {
            size = LIC_SIZE_NARROW;
        }
        else if (license == LIC_BOAT)
        {
            size = LIC_SIZE_BOAT;
        }
        else if (g_LicInfo[license][liAir])
        {
            size = LIC_SIZE_AIR;
        }
        if (!Lic_AddPoint(license, x, y, z, size))
        {
            Msg_Error(playerid, "Ruta moze imati najvise %d tacaka.", LIC_ROUTE_MAX);
            return 1;
        }
    }
    else if (!strcmp(option, "obrisi", true))
    {
        if (g_LicRouteCount[license] == 0)
        {
            Msg_Error(playerid, "Ruta nema tacaka.");
            return 1;
        }
        new query[96];
        mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `license_routes` WHERE `license` = %d AND `idx` = %d", _:license, g_LicRouteCount[license]);
        mysql_tquery(g_SQL, query);
        g_LicRouteCount[license]--;
    }
    else if (!strcmp(option, "ocisti", true) || !strcmp(option, "vrati", true))
    {
        if (isnull(value) || strcmp(value, "DA", true))
        {
            Msg_Error(playerid, "Nije potvrdjeno (upisite DA).");
            return 1;
        }
        if (!strcmp(option, "vrati", true))
        {
            Lic_SeedRoute(license);
        }
        else
        {
            new query[96];
            mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `license_routes` WHERE `license` = %d AND `idx` > 0", _:license);
            mysql_tquery(g_SQL, query);
            g_LicRouteCount[license] = 0;
        }
    }
    else
    {
        Msg_Error(playerid, "Opcije: start, dodaj, usko, obrisi, ocisti, vrati.");
        return 1;
    }
    new details[48];
    format(details, sizeof(details), "%s %s (%d tacaka)", g_LicInfo[license][liKey], option, g_LicRouteCount[license]);
    Staff_Log(playerid, "licroute", INVALID_PLAYER_ID, details);
    Msg_Admin(playerid, "Ruta %s: %s, sada %d tacaka.", g_LicInfo[license][liName], option, g_LicRouteCount[license]);
    return 1;
}
