// Shown when the job is taken and again with /posaoinfo.
static const g_JobIntro[E_JOB][] =
{
    "",
    "Mehanicar popravlja i sredjuje vozila.\n\n" \
    "- Opremu i sleper uzimate u radionici u Willowfield-u (/oprema), uz dozvolu za auto.\n" \
    "- /popravi [id] [cena]: na terenu motor (do 60%) i gume, u radionici sve do 100%.\n" \
    "- /tjuning [id]: delovi i farbanje, samo u radionici.\n" \
    "- Pozivi stizu na broj 555; prihvatate ih sa /prihvatipoziv [id].\n" \
    "- /vuci: kacenje vozila na sleper.\n\n" \
    "Zaradu za popravku dogovarate sa vozacem; od tjuninga dobijate deo cene."
};

stock Job_Init()
{
    mysql_tquery(g_SQL, "ALTER TABLE `accounts` ADD COLUMN IF NOT EXISTS `job` TINYINT UNSIGNED NOT NULL DEFAULT 0");
}

stock Job_OnPlayerConnect(playerid)
{
    g_JobDuty[playerid] = false;
}

stock E_JOB:Job_Of(playerid)
{
    new job = g_Player[playerid][pJob];
    if (job <= _:JOB_NONE || job >= _:E_JOB)
    {
        return JOB_NONE;
    }
    return E_JOB:job;
}

stock bool:Job_IsOnDuty(playerid, E_JOB:job)
{
    return g_JobDuty[playerid] && Job_Of(playerid) == job;
}

// The work outfit while on duty, or -1; Staff_UpdateLook puts it on.
stock Job_Skin(playerid)
{
    if (!g_JobDuty[playerid])
    {
        return -1;
    }
    new E_JOB:job = Job_Of(playerid);
    return g_Player[playerid][pGender] == GENDER_FEMALE ? g_JobInfo[job][jbSkinFemale] : g_JobInfo[job][jbSkinMale];
}

static stock Job_Set(playerid, E_JOB:job)
{
    g_Player[playerid][pJob] = _:job;
    new query[80];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `accounts` SET `job` = %d WHERE `id` = %d", _:job, g_Player[playerid][pDbId]);
    mysql_tquery(g_SQL, query);
}

stock Job_SetDuty(playerid, bool:on)
{
    g_JobDuty[playerid] = on;
    // Off duty the job's vehicle is no longer theirs to drive.
    if (!on && GetPlayerState(playerid) == PLAYER_STATE_DRIVER && g_JobVehicle[GetPlayerVehicleID(playerid)] != JOB_NONE)
    {
        RemovePlayerFromVehicle(playerid);
    }
    Staff_UpdateLook(playerid);
}

// Death puts the player back in their own clothes on spawn, so the shift ends with it.
stock Job_OnPlayerDeath(playerid)
{
    g_JobDuty[playerid] = false;
}

// A job that drives needs the license for it; told when it is missing, as when the police took it away.
stock bool:Job_HasLicense(playerid, E_JOB:job)
{
    new E_LICENSE:license = g_JobInfo[job][jbLicense];
    if (license != LIC_NONE && !Lic_Has(playerid, license))
    {
        Msg_Error(playerid, "Za posao %s treba dozvola: %s (auto skola, /gps).", g_JobInfo[job][jbName], g_LicInfo[license][liName]);
        return false;
    }
    return true;
}

stock Job_ShowList(playerid)
{
    new body[512], line[96];
    strcopy(body, "Posao\tUslov\n");
    for (new E_JOB:job = JOB_MECHANIC; job < E_JOB; job++)
    {
        new E_LICENSE:license = g_JobInfo[job][jbLicense], need[40];
        if (license != LIC_NONE)
        {
            format(need, sizeof(need), ", dozvola %s", g_LicInfo[license][liName]);
        }
        format(line, sizeof(line), "%s\tnivo %d%s\n", g_JobInfo[job][jbName], g_JobInfo[job][jbLevel], need);
        strcat(body, line);
    }
    ShowPlayerDialog(playerid, DIALOG_JOB_LIST, DIALOG_STYLE_TABLIST_HEADERS, "Opstina - poslovi", body, "Zaposli se", "Zatvori");
}

static stock Job_ShowIntro(playerid, E_JOB:job)
{
    ShowPlayerDialog(playerid, DIALOG_JOB_INTRO, DIALOG_STYLE_MSGBOX, g_JobInfo[job][jbName], g_JobIntro[job], "U redu", "");
}

static stock Job_Take(playerid, E_JOB:job)
{
    if (Job_Of(playerid) != JOB_NONE)
    {
        Msg_Error(playerid, "Vec imate posao (%s). Prvo dajte otkaz: /otkaz", g_JobInfo[Job_Of(playerid)][jbName]);
        return;
    }
    if (g_Player[playerid][pLevel] < g_JobInfo[job][jbLevel])
    {
        Msg_Error(playerid, "Za posao %s treba nivo %d.", g_JobInfo[job][jbName], g_JobInfo[job][jbLevel]);
        return;
    }
    if (!Job_HasLicense(playerid, job))
    {
        return;
    }
    Job_Set(playerid, job);
    Msg_Success(playerid, "Zaposlili ste se: %s. Uputstvo: /posaoinfo", g_JobInfo[job][jbName]);
    Job_ShowIntro(playerid, job);
}

stock bool:Job_OnDialogResponse(playerid, dialogid, response, listitem)
{
    switch (dialogid)
    {
        case DIALOG_JOB_LIST:
        {
            new E_JOB:job = E_JOB:(listitem + _:JOB_MECHANIC);
            if (response && job > JOB_NONE && job < E_JOB && Point_Require(playerid, POINT_CITYHALL))
            {
                Job_Take(playerid, job);
            }
        }
        case DIALOG_JOB_INTRO:
        {
        }
        case DIALOG_JOB_QUIT:
        {
            new E_JOB:job = Job_Of(playerid);
            if (!response || job == JOB_NONE)
            {
                return true;
            }
            if (g_JobDuty[playerid])
            {
                Job_SetDuty(playerid, false);
            }
            Job_Set(playerid, JOB_NONE);
            Msg_Success(playerid, "Dali ste otkaz (%s). Novi posao: opstina (/posao).", g_JobInfo[job][jbName]);
        }
        default:
        {
            return false;
        }
    }
    return true;
}

cmd:posao(playerid, params[])
{
    if (Point_Require(playerid, POINT_CITYHALL))
    {
        Job_ShowList(playerid);
    }
    return 1;
}

cmd:posaoinfo(playerid, params[])
{
    new E_JOB:job = Job_Of(playerid);
    if (job == JOB_NONE)
    {
        Msg_Error(playerid, "Nemate posao. Poslovi su u opstini (/posao).");
        return 1;
    }
    Job_ShowIntro(playerid, job);
    return 1;
}

cmd:otkaz(playerid, params[])
{
    new E_JOB:job = Job_Of(playerid);
    if (job == JOB_NONE)
    {
        Msg_Error(playerid, "Nemate posao.");
        return 1;
    }
    new text[96];
    format(text, sizeof(text), "Da li sigurno dajete otkaz na poslu %s?", g_JobInfo[job][jbName]);
    ShowPlayerDialog(playerid, DIALOG_JOB_QUIT, DIALOG_STYLE_MSGBOX, "Otkaz", text, "Otkaz", "Odustani");
    return 1;
}

// Taken at the job's workplace; it puts on the outfit and lets the player use the job's vehicles.
cmd:oprema(playerid, params[])
{
    new E_JOB:job = Job_Of(playerid);
    if (job == JOB_NONE)
    {
        Msg_Error(playerid, "Nemate posao. Poslovi su u opstini (/posao).");
        return 1;
    }
    if (!Point_Require(playerid, g_JobInfo[job][jbWorkplace]))
    {
        return 1;
    }
    if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT)
    {
        Msg_Error(playerid, "Opremu uzimate peske.");
        return 1;
    }
    if (g_JobDuty[playerid])
    {
        Job_SetDuty(playerid, false);
        Chat_Action(playerid, "ostavlja radnu opremu.");
        Msg_Info(playerid, "Zavrsili ste smenu.");
        return 1;
    }
    if (!Job_HasLicense(playerid, job))
    {
        return 1;
    }
    Job_SetDuty(playerid, true);
    Chat_Action(playerid, "oblaci radnu odecu i uzima opremu.");
    Msg_Success(playerid, "Na poslu ste (%s). Kraj smene: /oprema. Uputstvo: /posaoinfo", g_JobInfo[job][jbName]);
    return 1;
}

// Only workers on duty drive a job's vehicles; anyone may ride along.
stock Job_OnPlayerStateChange(playerid, PLAYER_STATE:newstate)
{
    if (newstate != PLAYER_STATE_DRIVER)
    {
        return;
    }
    new E_JOB:job = g_JobVehicle[GetPlayerVehicleID(playerid)];
    if (job != JOB_NONE && !Job_IsOnDuty(playerid, job))
    {
        RemovePlayerFromVehicle(playerid);
        Msg_Error(playerid, "Ovo vozilo voze samo radnici na poslu (%s, /oprema).", g_JobInfo[job][jbName]);
    }
}

// The game's respawn brings a job vehicle back with whatever fuel it had; it starts every shift full.
stock Job_OnVehicleSpawn(vehicleid)
{
    if (g_JobVehicle[vehicleid] != JOB_NONE)
    {
        Fuel_Set(vehicleid, float(Fuel_TankSize(GetVehicleModel(vehicleid))));
    }
}
