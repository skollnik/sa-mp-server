cmd:aduty(playerid, params[])
{
    if (!Staff_IsAdmin(playerid))
    {
        Msg_Error(playerid, "Nemate dozvolu za ovu komandu.");
        return 1;
    }

    g_Staff[playerid][sAdminDuty] = !g_Staff[playerid][sAdminDuty];
    Staff_UpdateDutyLook(playerid);

    new name[MAX_PLAYER_NAME + 1], rank[24], message[128];
    Account_GetDisplayName(playerid, name);
    Staff_GetAdminRank(playerid, rank);
    format(message, sizeof(message), "[A] %s %s je %s admin duznosti.", rank, name, g_Staff[playerid][sAdminDuty] ? ("sada na") : ("napustio"));
    Staff_SendToAdmins(message);
    Staff_Log(playerid, g_Staff[playerid][sAdminDuty] ? ("duty_on") : ("duty_off"), INVALID_PLAYER_ID, "");
    return 1;
}

cmd:a(playerid, params[])
{
    if (!Staff_IsAdmin(playerid))
    {
        Msg_Error(playerid, "Nemate dozvolu za ovu komandu.");
        return 1;
    }
    if (isnull(params))
    {
        Msg_Usage(playerid, "/a [tekst] (admin chat)");
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1], rank[24], message[192];
    Account_GetDisplayName(playerid, name);
    Staff_GetAdminRank(playerid, rank);
    format(message, sizeof(message), "[A] %s %s: %s", rank, name, params);
    Staff_SendToAdmins(message);
    return 1;
}

cmd:reports(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }

    new body[2048], line[192], name[MAX_PLAYER_NAME + 1], target_name[MAX_PLAYER_NAME + 1], count;
    for (new i = 0; i < MAX_REPORTS; i++)
    {
        if (!g_Reports[i][rUsed])
        {
            continue;
        }

        Account_GetDisplayName(g_Reports[i][rReporter], name);
        Account_GetDisplayName(g_Reports[i][rTarget], target_name);
        format(line, sizeof(line), C_ADMIN "#%d " C_WHITE "%s [%d] -> %s [%d]: %s " C_GREY "(pre %d min)\n",
            i + 1, name, g_Reports[i][rReporter], target_name, g_Reports[i][rTarget], g_Reports[i][rReason],
            (gettime() - g_Reports[i][rCreatedAt]) / 60);
        strcat(body, line);
        count++;
    }

    if (count == 0)
    {
        strcopy(body, C_WHITE "Nema otvorenih reportova.");
    }
    else
    {
        strcat(body, "\n" C_GREY "Prihvati: /ar [broj]    Odbij: /rr [broj] [razlog]");
    }
    ShowPlayerDialog(playerid, DIALOG_ADMIN_REPORTS, DIALOG_STYLE_MSGBOX, SERVER_NAME " - Otvoreni reportovi", body, "Zatvori", "");
    return 1;
}

cmd:ar(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }

    new number;
    if (sscanf(params, "d", number))
    {
        Msg_Usage(playerid, "/ar [broj reporta]");
        return 1;
    }

    new slot = number - 1;
    if (slot < 0 || slot >= MAX_REPORTS || !g_Reports[slot][rUsed])
    {
        Msg_Error(playerid, "Report #%d ne postoji.", number);
        return 1;
    }

    new reporter = g_Reports[slot][rReporter];
    g_Reports[slot][rUsed] = false;

    new name[MAX_PLAYER_NAME + 1], rank[24], message[160];
    Account_GetDisplayName(playerid, name);
    Staff_GetAdminRank(playerid, rank);

    Msg_Admin(reporter, "%s %s je preuzeo vas report i resava ga.", rank, name);
    format(message, sizeof(message), "[A] %s je preuzeo report #%d.", name, number);
    Staff_SendToAdmins(message);
    Staff_Log(playerid, "report_accept", reporter, g_Reports[slot][rReason]);
    return 1;
}

cmd:rr(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }

    new number, reason[96];
    if (sscanf(params, "dS(Bez razloga)[96]", number, reason))
    {
        Msg_Usage(playerid, "/rr [broj reporta] [razlog]");
        return 1;
    }

    new slot = number - 1;
    if (slot < 0 || slot >= MAX_REPORTS || !g_Reports[slot][rUsed])
    {
        Msg_Error(playerid, "Report #%d ne postoji.", number);
        return 1;
    }

    new reporter = g_Reports[slot][rReporter];
    g_Reports[slot][rUsed] = false;

    new name[MAX_PLAYER_NAME + 1], message[192];
    Account_GetDisplayName(playerid, name);

    Msg_Admin(reporter, "Vas report je odbijen. Razlog: %s", reason);
    format(message, sizeof(message), "[A] %s je odbio report #%d. Razlog: %s", name, number, reason);
    Staff_SendToAdmins(message);
    Staff_Log(playerid, "report_reject", reporter, reason);
    return 1;
}

cmd:gethere(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }

    new targetid;
    if (sscanf(params, "u", targetid))
    {
        Msg_Usage(playerid, "/gethere [id ili ime]");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid, "Ne mozete teleportovati samog sebe."))
    {
        return 1;
    }

    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    Staff_TeleportTo(targetid, x + 1.0, y, z, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid));

    new name[MAX_PLAYER_NAME + 1], target_name[MAX_PLAYER_NAME + 1];
    Account_GetDisplayName(playerid, name);
    Account_GetDisplayName(targetid, target_name);
    Msg_Admin(targetid, "Admin %s vas je teleportovao do sebe.", name);
    Msg_Admin(playerid, "Teleportovali ste igraca %s do sebe.", target_name);
    Staff_Log(playerid, "gethere", targetid, "");
    return 1;
}

cmd:spec(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }

    new targetid;
    if (sscanf(params, "u", targetid))
    {
        Msg_Usage(playerid, "/spec [id ili ime]    Prekid: /specoff");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid, "Ne mozete posmatrati samog sebe."))
    {
        return 1;
    }
    if (g_Staff[targetid][sSpectating])
    {
        Msg_Error(playerid, "Taj igrac trenutno i sam posmatra nekoga.");
        return 1;
    }

    Staff_StartSpectating(playerid, targetid);

    new target_name[MAX_PLAYER_NAME + 1];
    Account_GetDisplayName(targetid, target_name);
    Msg_Admin(playerid, "Posmatrate igraca %s [%d]. Prekid: /specoff", target_name, targetid);
    return 1;
}

cmd:specoff(playerid, params[])
{
    if (!g_Staff[playerid][sSpectating])
    {
        Msg_Error(playerid, "Trenutno ne posmatrate nikoga.");
        return 1;
    }

    Staff_StopSpectating(playerid);
    Msg_Admin(playerid, "Prestali ste da posmatrate.");
    return 1;
}

cmd:check(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }

    new targetid;
    if (sscanf(params, "u", targetid))
    {
        Msg_Usage(playerid, "/check [id ili ime]");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }

    new Float:health, Float:armour, ip[16];
    GetPlayerHealth(targetid, health);
    GetPlayerArmour(targetid, armour);
    GetPlayerIp(targetid, ip, sizeof(ip));

    new played = g_Player[targetid][pPlayedSeconds] + gettime() - g_Player[targetid][pSessionStart];

    new body[1024], title[64];
    format(body, sizeof(body),
        C_SERVER "Podatak\t" C_SERVER "Vrednost\n" \
        C_GREY "Ime\t" C_WHITE "%s\n" \
        C_GREY "ID na serveru / broj naloga\t" C_WHITE "%d / #%d\n" \
        C_GREY "Pol i godine\t" C_WHITE "%s, %d god.\n" \
        C_GREY "Skin\t" C_WHITE "%d\n" \
        C_GREY "Nivo / respect\t" C_WHITE "%d / %d\n" \
        C_GREY "Sati igre\t" C_WHITE "%d h\n" \
        C_GREY "Novac / banka\t" C_GREEN "$%d " C_WHITE "/ " C_GREEN "$%d\n" \
        C_GREY "HP / armor\t" C_WHITE "%.0f / %.0f\n" \
        C_GREY "Enterijer / virtuelni svet\t" C_WHITE "%d / %d\n" \
        C_GREY "Admin\t" C_ADMIN "%s\n" \
        C_GREY "Helper\t" C_HELPER "%s\n" \
        C_GREY "IP adresa\t" C_WHITE "%s",
        g_Player[targetid][pName],
        targetid, g_Player[targetid][pDbId],
        g_Player[targetid][pGender] == GENDER_FEMALE ? ("Zensko") : ("Musko"), g_Player[targetid][pAge],
        GetPlayerSkin(targetid),
        g_Player[targetid][pLevel], g_Player[targetid][pRespect],
        played / 3600,
        g_Player[targetid][pCash], g_Player[targetid][pBank],
        health, armour,
        GetPlayerInterior(targetid), GetPlayerVirtualWorld(targetid),
        g_AdminRankNames[g_Player[targetid][pAdminLevel]],
        g_HelperRankNames[g_Player[targetid][pHelperLevel]],
        ip);
    format(title, sizeof(title), SERVER_NAME " - Provera: %s", g_Player[targetid][pName]);
    ShowPlayerDialog(playerid, DIALOG_ADMIN_INFO, DIALOG_STYLE_TABLIST_HEADERS, title, body, "Zatvori", "");
    return 1;
}

cmd:freeze(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }

    new targetid;
    if (sscanf(params, "u", targetid))
    {
        Msg_Usage(playerid, "/freeze [id ili ime]    Odmrzavanje: /unfreeze");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid, "Ne mozete zamrznuti samog sebe.") || !Staff_CheckOutranks(playerid, targetid))
    {
        return 1;
    }

    TogglePlayerControllable(targetid, false);
    g_Staff[targetid][sFrozen] = true;

    new name[MAX_PLAYER_NAME + 1], target_name[MAX_PLAYER_NAME + 1];
    Account_GetDisplayName(playerid, name);
    Account_GetDisplayName(targetid, target_name);
    Msg_Admin(targetid, "Admin %s vas je zamrznuo.", name);
    Msg_Admin(playerid, "Zamrzli ste igraca %s.", target_name);
    Staff_Log(playerid, "freeze", targetid, "");
    return 1;
}

cmd:unfreeze(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }

    new targetid;
    if (sscanf(params, "u", targetid))
    {
        Msg_Usage(playerid, "/unfreeze [id ili ime]");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }

    TogglePlayerControllable(targetid, true);
    g_Staff[targetid][sFrozen] = false;

    new name[MAX_PLAYER_NAME + 1], target_name[MAX_PLAYER_NAME + 1];
    Account_GetDisplayName(playerid, name);
    Account_GetDisplayName(targetid, target_name);
    Msg_Admin(targetid, "Admin %s vas je odmrznuo.", name);
    Msg_Admin(playerid, "Odmrzli ste igraca %s.", target_name);
    Staff_Log(playerid, "unfreeze", targetid, "");
    return 1;
}

cmd:slap(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }

    new targetid;
    if (sscanf(params, "u", targetid))
    {
        Msg_Usage(playerid, "/slap [id ili ime]");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid) || !Staff_CheckOutranks(playerid, targetid))
    {
        return 1;
    }

    new Float:x, Float:y, Float:z;
    GetPlayerPos(targetid, x, y, z);
    SetPlayerPos(targetid, x, y, z + 5.0);
    PlayerPlaySound(targetid, 1190, x, y, z);

    new name[MAX_PLAYER_NAME + 1], target_name[MAX_PLAYER_NAME + 1];
    Account_GetDisplayName(playerid, name);
    Account_GetDisplayName(targetid, target_name);
    Msg_Admin(targetid, "Admin %s vas je osamario.", name);
    Msg_Admin(playerid, "Osamarili ste igraca %s.", target_name);
    Staff_Log(playerid, "slap", targetid, "");
    return 1;
}

cmd:sethp(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_ADMIN))
    {
        return 1;
    }

    new targetid, Float:amount;
    if (sscanf(params, "uf", targetid, amount) || amount < 0.0 || amount > 100.0)
    {
        Msg_Usage(playerid, "/sethp [id ili ime] [0-100]");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }

    SetPlayerHealth(targetid, amount);

    new target_name[MAX_PLAYER_NAME + 1], details[48];
    Account_GetDisplayName(targetid, target_name);
    Msg_Admin(playerid, "Postavili ste HP igraca %s na %.0f.", target_name, amount);
    format(details, sizeof(details), "vam je postavio HP na %.0f.", amount);
    Staff_NotifyTarget(playerid, targetid, details);
    format(details, sizeof(details), "%.0f", amount);
    Staff_Log(playerid, "sethp", targetid, details);
    return 1;
}

cmd:setarmour(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_ADMIN))
    {
        return 1;
    }

    new targetid, Float:amount;
    if (sscanf(params, "uf", targetid, amount) || amount < 0.0 || amount > 100.0)
    {
        Msg_Usage(playerid, "/setarmour [id ili ime] [0-100]");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }

    SetPlayerArmour(targetid, amount);

    new target_name[MAX_PLAYER_NAME + 1], details[48];
    Account_GetDisplayName(targetid, target_name);
    Msg_Admin(playerid, "Postavili ste armor igraca %s na %.0f.", target_name, amount);
    format(details, sizeof(details), "vam je postavio armor na %.0f.", amount);
    Staff_NotifyTarget(playerid, targetid, details);
    format(details, sizeof(details), "%.0f", amount);
    Staff_Log(playerid, "setarmour", targetid, details);
    return 1;
}

cmd:tp(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_ADMIN))
    {
        return 1;
    }

    new list[512];
    for (new i = 0; i < sizeof(g_AdminTeleports); i++)
    {
        strcat(list, g_AdminTeleports[i][tName]);
        strcat(list, "\n");
    }
    ShowPlayerDialog(playerid, DIALOG_ADMIN_TELEPORT, DIALOG_STYLE_LIST, SERVER_NAME " - Teleport", list, "Teleport", "Zatvori");
    return 1;
}

cmd:ooc(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_ADMIN))
    {
        return 1;
    }

    g_GlobalOocEnabled = !g_GlobalOocEnabled;

    new name[MAX_PLAYER_NAME + 1];
    Account_GetDisplayName(playerid, name);
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (Account_IsLoggedIn(i))
        {
            Msg_Server(i, "Admin %s je %s globalni OOC chat (/o).", name, g_GlobalOocEnabled ? ("ukljucio") : ("iskljucio"));
        }
    }
    Staff_Log(playerid, g_GlobalOocEnabled ? ("ooc_on") : ("ooc_off"), INVALID_PLAYER_ID, "");
    return 1;
}

cmd:cc(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_ADMIN))
    {
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1];
    Account_GetDisplayName(playerid, name);

    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (!Account_IsLoggedIn(i))
        {
            continue;
        }
        for (new line = 0; line < 50; line++)
        {
            SendClientMessage(i, COLOR_WHITE, " ");
        }
        Msg_Admin(i, "Admin %s je obrisao chat.", name);
    }
    Staff_Log(playerid, "clear_chat", INVALID_PLAYER_ID, "");
    return 1;
}

cmd:ao(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_ADMIN))
    {
        return 1;
    }
    if (isnull(params))
    {
        Msg_Usage(playerid, "/ao [tekst] (admin obavestenje svima u chatu)");
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1], message[192];
    Account_GetDisplayName(playerid, name);
    format(message, sizeof(message), "[ADMIN] %s: %s", name, params);
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (Account_IsLoggedIn(i))
        {
            Chat_Send(i, COLOR_ADMIN, message);
        }
    }
    Staff_Log(playerid, "announce_chat", INVALID_PLAYER_ID, params);
    return 1;
}

cmd:fixveh(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_ADMIN))
    {
        return 1;
    }

    new vehicleid = GetPlayerVehicleID(playerid);
    if (vehicleid == 0)
    {
        Msg_Error(playerid, "Morate biti u vozilu.");
        return 1;
    }

    RepairVehicle(vehicleid);
    Msg_Admin(playerid, "Vozilo je popravljeno.");
    Staff_Log(playerid, "fixveh", INVALID_PLAYER_ID, "");
    return 1;
}

cmd:flip(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_ADMIN))
    {
        return 1;
    }

    new vehicleid = GetPlayerVehicleID(playerid);
    if (vehicleid == 0)
    {
        Msg_Error(playerid, "Morate biti u vozilu.");
        return 1;
    }

    // Setting the Z angle also resets the X/Y rotation, which puts the vehicle back on its wheels.
    new Float:angle;
    GetVehicleZAngle(vehicleid, angle);
    SetVehicleZAngle(vehicleid, angle);
    Msg_Admin(playerid, "Vozilo je ispravljeno.");
    return 1;
}

cmd:disarm(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_ADMIN))
    {
        return 1;
    }

    new targetid;
    if (sscanf(params, "u", targetid))
    {
        Msg_Usage(playerid, "/disarm [id ili ime] (oduzima svo oruzje)");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid) || !Staff_CheckOutranks(playerid, targetid))
    {
        return 1;
    }

    ResetPlayerWeapons(targetid);

    new target_name[MAX_PLAYER_NAME + 1];
    Account_GetDisplayName(targetid, target_name);
    Msg_Admin(playerid, "Oduzeli ste svo oruzje igracu %s.", target_name);
    Staff_NotifyTarget(playerid, targetid, "vam je oduzeo svo oruzje.");
    Staff_Log(playerid, "disarm", targetid, "");
    return 1;
}

cmd:ann(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_SENIOR))
    {
        return 1;
    }
    if (isnull(params) || strlen(params) > 100)
    {
        Msg_Usage(playerid, "/ann [tekst do 100 karaktera] (veliko obavestenje na sredini ekrana)");
        return 1;
    }

    // A stray ~ is a broken game text colour code and can crash clients, so the admin's text never carries any.
    new text[101];
    strcopy(text, params);
    for (new i = 0; text[i] != '\0'; i++)
    {
        if (text[i] == '~')
        {
            text[i] = '-';
        }
    }

    GameTextForAll("~w~%s", 6000, 3, text);
    Staff_Log(playerid, "announce_screen", INVALID_PLAYER_ID, params);
    return 1;
}

cmd:settime(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_SENIOR))
    {
        return 1;
    }

    new hour;
    if (sscanf(params, "d", hour) || hour < 0 || hour > 23)
    {
        Msg_Usage(playerid, "/settime [sat 0-23]");
        return 1;
    }

    SetWorldTime(hour);
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (Account_IsLoggedIn(i))
        {
            SetPlayerTime(i, hour, 0);
        }
    }

    new details[16];
    format(details, sizeof(details), "%d", hour);
    Msg_Admin(playerid, "Vreme na serveru je postavljeno na %02d:00.", hour);
    Staff_Log(playerid, "settime", INVALID_PLAYER_ID, details);
    return 1;
}

cmd:setweather(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_SENIOR))
    {
        return 1;
    }

    new weather;
    if (sscanf(params, "d", weather) || weather < 0 || weather > 45)
    {
        Msg_Usage(playerid, "/setweather [id 0-45] (npr. 1 suncano, 8 kisa, 9 magla, 16 oluja)");
        return 1;
    }

    SetWeather(weather);

    new details[16];
    format(details, sizeof(details), "%d", weather);
    Msg_Admin(playerid, "Vremenske prilike su postavljene na %d.", weather);
    Staff_Log(playerid, "setweather", INVALID_PLAYER_ID, details);
    return 1;
}

cmd:gotopos(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_SENIOR))
    {
        return 1;
    }

    new Float:x, Float:y, Float:z, interior;
    if (sscanf(params, "fffD(0)", x, y, z, interior))
    {
        Msg_Usage(playerid, "/gotopos [x] [y] [z] [enterijer, opciono]");
        return 1;
    }

    Staff_TeleportTo(playerid, x, y, z, interior, GetPlayerVirtualWorld(playerid));
    Msg_Admin(playerid, "Teleportovani ste na %.2f, %.2f, %.2f (enterijer %d).", x, y, z, interior);
    return 1;
}

cmd:mypos(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_SENIOR))
    {
        return 1;
    }

    new Float:x, Float:y, Float:z, Float:angle;
    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, angle);
    Msg_Admin(playerid, "Pozicija: %.2f, %.2f, %.2f  ugao: %.1f  enterijer: %d  svet: %d", x, y, z, angle, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid));
    return 1;
}

cmd:setskin(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_SENIOR))
    {
        return 1;
    }

    new targetid, skin;
    if (sscanf(params, "ud", targetid, skin) || skin < 0 || skin > 311 || skin == 74)
    {
        Msg_Usage(playerid, "/setskin [id ili ime] [skin 0-311]");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }

    g_Player[targetid][pSkin] = skin;
    SetPlayerSkin(targetid, skin);

    new target_name[MAX_PLAYER_NAME + 1], details[48];
    Account_GetDisplayName(targetid, target_name);
    Msg_Admin(playerid, "Postavili ste skin %d igracu %s.", skin, target_name);
    format(details, sizeof(details), "vam je postavio skin %d.", skin);
    Staff_NotifyTarget(playerid, targetid, details);
    format(details, sizeof(details), "%d", skin);
    Staff_Log(playerid, "setskin", targetid, details);
    return 1;
}

cmd:setint(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_SENIOR))
    {
        return 1;
    }

    new targetid, interior;
    if (sscanf(params, "ud", targetid, interior) || interior < 0 || interior > 255)
    {
        Msg_Usage(playerid, "/setint [id ili ime] [enterijer 0-255]");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }

    SetPlayerInterior(targetid, interior);

    new target_name[MAX_PLAYER_NAME + 1], details[48];
    Account_GetDisplayName(targetid, target_name);
    Msg_Admin(playerid, "Postavili ste enterijer %d igracu %s.", interior, target_name);
    format(details, sizeof(details), "vam je postavio enterijer %d.", interior);
    Staff_NotifyTarget(playerid, targetid, details);
    format(details, sizeof(details), "%d", interior);
    Staff_Log(playerid, "setint", targetid, details);
    return 1;
}

cmd:setvw(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_SENIOR))
    {
        return 1;
    }

    new targetid, world;
    if (sscanf(params, "ud", targetid, world) || world < 0)
    {
        Msg_Usage(playerid, "/setvw [id ili ime] [virtuelni svet]");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }

    SetPlayerVirtualWorld(targetid, world);

    new target_name[MAX_PLAYER_NAME + 1], details[48];
    Account_GetDisplayName(targetid, target_name);
    Msg_Admin(playerid, "Postavili ste virtuelni svet %d igracu %s.", world, target_name);
    format(details, sizeof(details), "vam je postavio virtuelni svet %d.", world);
    Staff_NotifyTarget(playerid, targetid, details);
    format(details, sizeof(details), "%d", world);
    Staff_Log(playerid, "setvw", targetid, details);
    return 1;
}

cmd:veh(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_SENIOR))
    {
        return 1;
    }

    new model;
    if (sscanf(params, "d", model) || model < 400 || model > 611)
    {
        Msg_Usage(playerid, "/veh [model 400-611]    Brisanje: /dveh");
        return 1;
    }

    new Float:x, Float:y, Float:z, Float:angle;
    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, angle);

    new vehicleid = CreateVehicle(model, x, y, z + 0.5, angle, -1, -1, -1);
    if (vehicleid == INVALID_VEHICLE_ID)
    {
        Msg_Error(playerid, "Vozilo nije moguce napraviti.");
        return 1;
    }

    g_IsAdminVehicle[vehicleid] = true;
    g_AdminVehicleOwner[vehicleid] = playerid;
    LinkVehicleToInterior(vehicleid, GetPlayerInterior(playerid));
    SetVehicleVirtualWorld(vehicleid, GetPlayerVirtualWorld(playerid));
    SetVehicleParamsEx(vehicleid, 1, 1, 0, 0, 0, 0, 0);
    PutPlayerInVehicle(playerid, vehicleid, 0);

    new details[16];
    format(details, sizeof(details), "model %d", model);
    Msg_Admin(playerid, "Napravili ste admin vozilo (model %d). Brisanje: /dveh", model);
    Staff_Log(playerid, "veh", INVALID_PLAYER_ID, details);
    return 1;
}

cmd:dveh(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_SENIOR))
    {
        return 1;
    }

    if (!isnull(params) && !strcmp(params, "sva", true))
    {
        new count;
        for (new i = 1; i < MAX_VEHICLES; i++)
        {
            if (g_IsAdminVehicle[i])
            {
                DestroyVehicle(i);
                g_IsAdminVehicle[i] = false;
                count++;
            }
        }
        Msg_Admin(playerid, "Obrisano admin vozila: %d.", count);
        return 1;
    }

    new vehicleid = GetPlayerVehicleID(playerid);
    if (vehicleid == 0 || !g_IsAdminVehicle[vehicleid])
    {
        Msg_Usage(playerid, "/dveh (dok sedite u admin vozilu) ili /dveh sva");
        return 1;
    }

    DestroyVehicle(vehicleid);
    g_IsAdminVehicle[vehicleid] = false;
    Msg_Admin(playerid, "Admin vozilo je obrisano.");
    return 1;
}

// Glavni admin can set any helper level; Glavni helper runs the helper ranks below their own (levels 0-2).
cmd:makehelper(playerid, params[])
{
    new bool:as_head_helper = !Staff_IsAdmin(playerid) && g_Player[playerid][pHelperLevel] >= HELPER_LEVEL_HEAD;

    if (as_head_helper)
    {
        if (!Staff_CheckHelper(playerid, HELPER_LEVEL_HEAD))
        {
            return 1;
        }
    }
    else if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }

    new max_level = as_head_helper ? HELPER_LEVEL_SENIOR : HELPER_LEVEL_HEAD;

    new targetid, level;
    if (sscanf(params, "ud", targetid, level) || level < 0 || level > max_level)
    {
        Msg_Usage(playerid, "/makehelper [id ili ime] [0-%d] (0 = skidanje)", max_level);
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid, as_head_helper ? ("Ne mozete menjati sopstveni helper nivo.") : ("")))
    {
        return 1;
    }
    if (as_head_helper && (Staff_IsAdmin(targetid) || g_Player[targetid][pHelperLevel] >= HELPER_LEVEL_HEAD))
    {
        Msg_Error(playerid, "Ne mozete menjati status ovom clanu tima.");
        return 1;
    }
    // A player is either an admin or a helper; admins already have every helper command.
    if (level > 0 && Staff_IsAdmin(targetid))
    {
        Msg_Error(playerid, "Taj igrac je %s i vec ima sve helper komande. Da bi bio helper, prvo mu skinite admin (/makeadmin %d 0).", g_AdminRankNames[g_Player[targetid][pAdminLevel]], targetid);
        return 1;
    }

    g_Player[targetid][pHelperLevel] = level;
    if (level == 0)
    {
        g_Staff[targetid][sHelperDuty] = false;
        Staff_UpdateDutyLook(targetid);
    }
    Account_Save(targetid);

    new name[MAX_PLAYER_NAME + 1], target_name[MAX_PLAYER_NAME + 1], message[160], details[16];
    Account_GetDisplayName(playerid, name);
    Account_GetDisplayName(targetid, target_name);

    if (level == 0)
    {
        Msg_Helper(targetid, "%s vam je oduzeo helper status.", name);
        format(message, sizeof(message), "[H] %s je oduzeo helper status igracu %s.", name, target_name);
    }
    else
    {
        Msg_Helper(targetid, "%s vas je postavio za: %s. Komande: /hhelp", name, g_HelperRankNames[level]);
        format(message, sizeof(message), "[H] %s je postavio %s za: %s.", name, target_name, g_HelperRankNames[level]);
    }
    Staff_SendToHelpers(message);
    format(details, sizeof(details), "%d", level);
    Staff_Log(playerid, "makehelper", targetid, details);
    return 1;
}

cmd:setlevel(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }

    new targetid, level;
    if (sscanf(params, "ud", targetid, level) || level < 1 || level > 100)
    {
        Msg_Usage(playerid, "/setlevel [id ili ime] [nivo 1-100]");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }

    g_Player[targetid][pLevel] = level;
    SetPlayerScore(targetid, level);
    Account_Save(targetid);

    new name[MAX_PLAYER_NAME + 1], target_name[MAX_PLAYER_NAME + 1], details[16];
    Account_GetDisplayName(playerid, name);
    Account_GetDisplayName(targetid, target_name);
    Msg_Admin(targetid, "Admin %s vam je postavio nivo na %d.", name, level);
    Msg_Admin(playerid, "Postavili ste nivo %d igracu %s.", level, target_name);
    format(details, sizeof(details), "%d", level);
    Staff_Log(playerid, "setlevel", targetid, details);
    return 1;
}

cmd:setrespect(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }

    new targetid, respect;
    if (sscanf(params, "ud", targetid, respect) || respect < 0)
    {
        Msg_Usage(playerid, "/setrespect [id ili ime] [respect]");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }

    g_Player[targetid][pRespect] = respect;
    Account_Save(targetid);

    new target_name[MAX_PLAYER_NAME + 1], details[48];
    Account_GetDisplayName(targetid, target_name);
    Msg_Admin(playerid, "Postavili ste respect %d igracu %s.", respect, target_name);
    format(details, sizeof(details), "vam je postavio respect na %d.", respect);
    Staff_NotifyTarget(playerid, targetid, details);
    format(details, sizeof(details), "%d", respect);
    Staff_Log(playerid, "setrespect", targetid, details);
    return 1;
}

cmd:setname(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }

    new targetid, new_name[MAX_PLAYER_NAME + 1];
    if (sscanf(params, "us[25]", targetid, new_name))
    {
        Msg_Usage(playerid, "/setname [id ili ime] [Novo_Ime]");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid) || !Staff_CheckOutranks(playerid, targetid))
    {
        return 1;
    }
    if (!Account_IsValidRpName(new_name) && g_Player[playerid][pAdminLevel] < ADMIN_LEVEL_OWNER)
    {
        Msg_Error(playerid, "Novo ime mora biti u formatu Ime_Prezime.");
        return 1;
    }

    new query[128];
    mysql_format(g_SQL, query, sizeof(query), "SELECT 1 FROM `accounts` WHERE `name` = '%e' LIMIT 1", new_name);
    mysql_tquery(g_SQL, query, "OnStaffSetNameCheck", "ddds", playerid, targetid, g_AccountRace[targetid], new_name);
    return 1;
}

forward OnStaffSetNameCheck(playerid, targetid, target_race, const new_name[]);
public OnStaffSetNameCheck(playerid, targetid, target_race, const new_name[])
{
    if (target_race != g_AccountRace[targetid] || !Account_IsLoggedIn(playerid))
    {
        return 1;
    }
    if (cache_num_rows() > 0)
    {
        Msg_Error(playerid, "Ime %s je vec zauzeto.", new_name);
        return 1;
    }
    if (SetPlayerName(targetid, new_name) != 1)
    {
        Msg_Error(playerid, "Ime %s nije moguce postaviti (zauzeto ili neispravno).", new_name);
        return 1;
    }

    new old_name[MAX_PLAYER_NAME + 1];
    strcopy(old_name, g_Player[targetid][pName]);
    strcopy(g_Player[targetid][pName], new_name, MAX_PLAYER_NAME + 1);

    new query[160];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `accounts` SET `name` = '%e' WHERE `id` = %d", new_name, g_Player[targetid][pDbId]);
    mysql_tquery(g_SQL, query);

    new name[MAX_PLAYER_NAME + 1];
    Account_GetDisplayName(playerid, name);
    Msg_Admin(targetid, "Admin %s vam je promenio ime u %s. Od sada se prijavljujete sa tim imenom.", name, new_name);
    Msg_Admin(playerid, "Promenili ste ime %s u %s.", old_name, new_name);

    new details[64];
    format(details, sizeof(details), "%s -> %s", old_name, new_name);
    Staff_Log(playerid, "setname", targetid, details);
    return 1;
}

cmd:makeadmin(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_MANAGEMENT))
    {
        return 1;
    }

    // Management can hand out up to Glavni admin; only the owner can create Menadzment.
    new max_level = (g_Player[playerid][pAdminLevel] >= ADMIN_LEVEL_OWNER) ? ADMIN_LEVEL_MANAGEMENT : ADMIN_LEVEL_HEAD;

    new targetid, level;
    if (sscanf(params, "ud", targetid, level) || level < 0 || level > max_level)
    {
        Msg_Usage(playerid, "/makeadmin [id ili ime] [0-%d] (0 = skidanje)", max_level);
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid, "Ne mozete menjati sopstveni admin nivo.") || !Staff_CheckOutranks(playerid, targetid))
    {
        return 1;
    }

    // Promoting a helper to admin replaces the helper rank, since admins have every helper command.
    new bool:removed_helper = (level > 0 && g_Player[targetid][pHelperLevel] > 0);
    if (removed_helper)
    {
        g_Player[targetid][pHelperLevel] = 0;
        g_Staff[targetid][sHelperDuty] = false;
    }

    g_Player[targetid][pAdminLevel] = level;
    if (level == 0)
    {
        g_Staff[targetid][sAdminDuty] = false;
    }
    Staff_UpdateDutyLook(targetid);
    Account_Save(targetid);

    if (removed_helper)
    {
        Msg_Info(playerid, "Helper status igraca je uklonjen, jer admin vec ima sve helper komande.");
    }

    new name[MAX_PLAYER_NAME + 1], target_name[MAX_PLAYER_NAME + 1], message[160], details[16];
    Account_GetDisplayName(playerid, name);
    Account_GetDisplayName(targetid, target_name);

    if (level == 0)
    {
        Msg_Admin(targetid, "%s vam je oduzeo admin status.", name);
        format(message, sizeof(message), "[A] %s je oduzeo admin status igracu %s.", name, target_name);
    }
    else
    {
        Msg_Admin(targetid, "%s vas je postavio za: %s. Komande: /ahelp", name, g_AdminRankNames[level]);
        format(message, sizeof(message), "[A] %s je postavio %s za: %s.", name, target_name, g_AdminRankNames[level]);
    }
    Staff_SendToAdmins(message);
    format(details, sizeof(details), "%d", level);
    Staff_Log(playerid, "makeadmin", targetid, details);
    return 1;
}

cmd:gmx(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_MANAGEMENT))
    {
        return 1;
    }

    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (Account_IsLoggedIn(i))
        {
            Msg_Server(i, "Server se restartuje. Vasi podaci su sacuvani, ponovo se povezite za par sekundi.");
        }
    }
    Staff_Log(playerid, "gmx", INVALID_PLAYER_ID, "");
    SendRconCommand("gmx");
    return 1;
}

cmd:givemoney(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_OWNER))
    {
        return 1;
    }

    new targetid, amount;
    if (sscanf(params, "ud", targetid, amount) || amount == 0)
    {
        Msg_Usage(playerid, "/givemoney [id ili ime] [iznos] (negativan iznos oduzima)");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }

    Account_GiveCash(targetid, amount);
    Account_Save(targetid);

    new target_name[MAX_PLAYER_NAME + 1], details[48];
    Account_GetDisplayName(targetid, target_name);
    Msg_Admin(playerid, "Dali ste $%d igracu %s. Sada ima $%d.", amount, target_name, g_Player[targetid][pCash]);
    format(details, sizeof(details), "vam je %s $%d.", amount > 0 ? ("dao") : ("oduzeo"), amount > 0 ? amount : -amount);
    Staff_NotifyTarget(playerid, targetid, details);
    format(details, sizeof(details), "%d", amount);
    Staff_Log(playerid, "givemoney", targetid, details);
    return 1;
}

cmd:ahelp(playerid, params[])
{
    if (!Staff_IsAdmin(playerid))
    {
        Msg_Error(playerid, "Nemate dozvolu za ovu komandu.");
        return 1;
    }

    new level = g_Player[playerid][pAdminLevel], body[2048];

    strcat(body, C_ADMIN "1 - Admin pocetnik\n" C_WHITE "/aduty /a /reports /ar /rr /goto /gethere /spec /specoff /check /freeze /unfreeze /slap\n\n");
    if (level >= ADMIN_LEVEL_ADMIN)
    {
        strcat(body, C_ADMIN "2 - Admin\n" C_WHITE "/sethp /setarmour /tp /ooc /cc /ao /fixveh /flip /disarm\n\n");
    }
    if (level >= ADMIN_LEVEL_SENIOR)
    {
        strcat(body, C_ADMIN "3 - Visi admin\n" C_WHITE "/setskin /setint /setvw /veh /dveh /ann /settime /setweather /gotopos /mypos\n\n");
    }
    if (level >= ADMIN_LEVEL_HEAD)
    {
        strcat(body, C_ADMIN "4 - Glavni admin\n" C_WHITE "/makehelper /setlevel /setrespect /setname\n\n");
    }
    if (level >= ADMIN_LEVEL_MANAGEMENT)
    {
        strcat(body, C_ADMIN "5 - Menadzment\n" C_WHITE "/makeadmin /gmx\n\n");
    }
    if (level >= ADMIN_LEVEL_OWNER)
    {
        strcat(body, C_ADMIN "6 - Vlasnik\n" C_WHITE "/givemoney /dozvoliime /zabraniime\n\n");
    }
    if (level < ADMIN_LEVEL_OWNER)
    {
        strcat(body, C_GREY "Prikazane su komande do vaseg nivoa. Visi nivoi dobijaju jos komandi.\n");
    }
    strcat(body, C_GREY "Komande rade samo na admin duznosti (/aduty), osim /a.");

    ShowPlayerDialog(playerid, DIALOG_ADMIN_INFO, DIALOG_STYLE_MSGBOX, SERVER_NAME " - Admin komande", body, "Zatvori", "");
    return 1;
}

cmd:hhelp(playerid, params[])
{
    if (!Staff_IsStaff(playerid))
    {
        Msg_Error(playerid, "Nemate dozvolu za ovu komandu.");
        return 1;
    }

    ShowPlayerDialog(playerid, DIALOG_ADMIN_INFO, DIALOG_STYLE_MSGBOX, SERVER_NAME " - Helper komande",
        C_HELPER "1 - Helper\n" C_WHITE "/hduty /hc /odg\n\n" \
        C_HELPER "2 - Iskusni helper\n" C_WHITE "/goto\n\n" \
        C_HELPER "3 - Glavni helper\n" C_WHITE "/makehelper (postavlja helpere nivoa 1-2)\n\n" \
        C_GREY "Komande rade na helper duznosti (/hduty), osim /hc.\n" \
        "Helperi ne kaznjavaju: prekrsaje prijavite adminima preko /report.",
        "Zatvori", "");
    return 1;
}
