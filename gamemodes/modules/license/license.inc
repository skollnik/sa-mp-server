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
// The car and motorbike tests start with the cone course on the polygon and go on through the city.
static const Float:g_LicDefaultPolygon[][3] =
{
    { 1439.51, -2363.62, 13.37 },
    { 1448.40, -2360.80, 13.60 },
    { 1460.05, -2364.59, 13.37 },
    { 1460.10, -2345.00, 13.37 },
    { 1453.24, -2357.80, 13.36 },
    { 1440.22, -2352.15, 13.37 },
    { 1452.19, -2345.33, 13.37 }
};

static const Float:g_LicDefaultCity[][3] =
{
    { 1466.68, -2358.89, 13.24 },
    { 1466.38, -2375.67, 13.15 },
    { 1332.79, -2375.77, 21.47 },
    { 1264.02, -2334.52, 16.01 },
    { 1328.98, -2335.39, 13.15 },
    { 1318.70, -2400.50, 13.14 },
    { 1318.37, -2435.24, 8.09 },
    { 1306.94, -2447.06, 7.43 },
    { 1142.43, -2385.41, 10.96 },
    { 1050.06, -2083.43, 12.72 },
    { 1059.78, -1832.94, 13.30 },
    { 1015.91, -1792.02, 13.61 },
    { 726.62, -1754.84, 14.15 },
    { 168.68, -1573.92, 12.34 },
    { 166.27, -1550.48, 11.48 },
    { 196.88, -1502.50, 12.43 },
    { 507.03, -1283.98, 15.55 },
    { 712.50, -1126.64, 17.17 },
    { 843.03, -1037.82, 25.02 },
    { 988.78, -972.22, 39.77 },
    { 1203.67, -948.14, 42.48 },
    { 1348.90, -942.97, 34.41 },
    { 1359.30, -963.45, 33.35 },
    { 1352.44, -1021.02, 26.31 },
    { 1381.64, -1038.05, 25.95 },
    { 1502.16, -1037.79, 23.39 },
    { 1575.25, -1147.81, 23.59 },
    { 1594.35, -1163.21, 23.67 },
    { 1832.21, -1183.27, 23.40 },
    { 1845.38, -1195.64, 22.88 },
    { 1865.56, -1262.96, 13.15 },
    { 2054.60, -1263.30, 23.58 },
    { 2064.66, -1277.31, 23.59 },
    { 2064.55, -1358.33, 23.59 },
    { 2084.72, -1386.76, 23.59 },
    { 2140.87, -1386.20, 23.60 },
    { 2303.65, -1385.78, 23.56 },
    { 2388.61, -1401.97, 23.61 },
    { 2405.68, -1446.09, 23.61 },
    { 2565.66, -1447.25, 34.36 },
    { 2639.97, -1457.54, 30.05 },
    { 2640.64, -1635.13, 10.49 },
    { 2640.81, -1759.80, 10.48 },
    { 2809.92, -1893.66, 10.70 },
    { 2819.86, -1904.08, 10.70 },
    { 2667.87, -2153.40, 10.68 },
    { 2273.91, -2276.83, 13.14 },
    { 2157.31, -2482.73, 13.14 },
    { 1847.91, -2667.70, 5.64 },
    { 1465.92, -2668.33, 11.98 },
    { 1347.89, -2492.38, 13.15 },
    { 1356.72, -2302.00, 13.15 },
    { 1389.05, -2288.78, 13.11 },
    { 1427.12, -2301.09, 13.15 },
    { 1470.34, -2334.57, 13.15 },
    { 1465.28, -2352.55, 13.30 }
};

static const Float:g_LicDefaultBoat[][3] =
{
    { 80.18, -1854.06, 0.0 },
    { 93.56, -1864.49, 0.0 },
    { 107.24, -1861.70, 0.0 },
    { 115.71, -1863.39, 0.0 },
    { 121.49, -1850.95, 0.0 },
    { 117.62, -1841.70, 0.0 },
    { 112.06, -1837.43, 0.0 }
};

static const Float:g_LicDefaultAir[][3] =
{
    { 1520.0, -2430.0, 45.0 },
    { 1800.0, -2500.0, 70.0 },
    { 2050.0, -2400.0, 80.0 },
    { 1900.0, -2250.0, 80.0 },
    { 1600.0, -2250.0, 70.0 },
    { 1450.0, -2400.0, 40.0 },
    { 1520.0, -2430.0, 15.0 }
};

// x, y, z and facing angle where the test vehicle appears.
static const Float:g_LicDefaultStart[E_LICENSE][4] =
{
    { 1428.39, -2356.28, 13.48, 270.0 },
    { 1428.39, -2356.28, 13.48, 270.0 },
    { 1428.39, -2356.28, 13.48, 270.0 },
    { 112.06, -1837.43, 0.0, 94.0 },
    { 1520.0, -2430.0, 13.6, 90.0 },
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
        case LIC_CAR, LIC_MOTO:
        {
            Lic_AddDefaults(license, g_LicDefaultPolygon, sizeof(g_LicDefaultPolygon), LIC_SIZE_NARROW);
            Lic_AddDefaults(license, g_LicDefaultCity, sizeof(g_LicDefaultCity), LIC_SIZE_ROAD);
        }
        case LIC_TRUCK: Lic_AddDefaults(license, g_LicDefaultCity, sizeof(g_LicDefaultCity), LIC_SIZE_ROAD);
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
    Veh_SetParam(vehicleid, VEH_PARAM_ENGINE, true);
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
    SetTimerEx("Lic_SeatPlayer", 700, false, "dd", playerid, vehicleid);
    Lic_ShowPoint(playerid);
    Msg_Info(playerid, "Instruktor: vozite kroz oznacene tacke. Imate %d minuta; ne ostecujte vozilo.", g_LicInfo[license][liTimeLimit] / 60);
}

forward Lic_SeatPlayer(playerid, vehicleid);
public Lic_SeatPlayer(playerid, vehicleid)
{
    if (g_LicTest[playerid][ltStage] == LIC_STAGE_DRIVE && g_LicTest[playerid][ltVehicle] == vehicleid)
    {
        AC_ExpectTeleport(playerid, g_LicStart[g_LicTest[playerid][ltLicense]][0], g_LicStart[g_LicTest[playerid][ltLicense]][1], g_LicStart[g_LicTest[playerid][ltLicense]][2]);
        PutPlayerInVehicle(playerid, vehicleid, 0);
    }
    return 1;
}

// bring_back: false on death and disconnect, where the player is not moved.
static stock Lic_EndTest(playerid, bool:bring_back)
{
    new vehicleid = g_LicTest[playerid][ltVehicle];
    g_LicTest[playerid][ltStage] = LIC_STAGE_NONE;
    g_LicTest[playerid][ltVehicle] = INVALID_VEHICLE_ID;
    if (vehicleid == INVALID_VEHICLE_ID)
    {
        return;
    }
    DisablePlayerRaceCheckpoint(playerid);
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
