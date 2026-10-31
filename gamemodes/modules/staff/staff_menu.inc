static const g_PlayerMenu[][E_MENU_ITEM] =
{
    { ADMIN_LEVEL_TRIAL,  "Informacije (/check)",            "check %s",        "" },
    { ADMIN_LEVEL_TRIAL,  "Teleport do igraca",              "goto %s",         "" },
    { ADMIN_LEVEL_TRIAL,  "Dovedi igraca",                   "gethere %s",      "" },
    { ADMIN_LEVEL_TRIAL,  "Posmatraj",                       "spec %s",         "" },
    { ADMIN_LEVEL_TRIAL,  "Zamrzni",                         "freeze %s",       "" },
    { ADMIN_LEVEL_TRIAL,  "Odmrzni",                         "unfreeze %s",     "" },
    { ADMIN_LEVEL_TRIAL,  "Osamari (slap)",                  "slap %s",         "" },
    { ADMIN_LEVEL_TRIAL,  "Izbaci (kick)",                   "kick %s",         "Razlog izbacivanja:" },
    { ADMIN_LEVEL_TRIAL,  "Zabrana pisanja (mute)",          "mute %s",         "Minuta i razlog, npr: 10 vredjanje" },
    { ADMIN_LEVEL_TRIAL,  "Skini zabranu pisanja",           "unmute %s",       "" },
    { ADMIN_LEVEL_TRIAL,  "Admin zatvor",                    "ajail %s",        "Minuta i razlog, npr: 15 DM" },
    { ADMIN_LEVEL_TRIAL,  "Pusti iz zatvora",                "unjail %s",       "" },
    { ADMIN_LEVEL_TRIAL,  "Upozorenja (lista)",              "warns %s",        "" },
    { ADMIN_LEVEL_ADMIN,  "Upozorenje (warn)",               "warn %s",         "Razlog upozorenja:" },
    { ADMIN_LEVEL_ADMIN,  "Ban",                             "ban %s",          "Dana i razlog, npr: 3 cheat" },
    { ADMIN_LEVEL_ADMIN,  "HP",                              "sethp %s",        "HP (0-100):" },
    { ADMIN_LEVEL_ADMIN,  "Armor",                           "setarmour %s",    "Armor (0-100):" },
    { ADMIN_LEVEL_ADMIN,  "Oduzmi oruzje",                   "disarm %s",       "" },
    { ADMIN_LEVEL_ADMIN,  "Inventar (pregled)",              "proveriinv %s",   "" },
    { ADMIN_LEVEL_ADMIN,  "Kazne sa kamera (pregled)",       "kazne %s",        "" },
    { ADMIN_LEVEL_ADMIN,  "Odblokiraj PIN kartice",          "odblokirajpin %s", "" },
    { ADMIN_LEVEL_SENIOR, "Skin",                            "setskin %s",      "Skin (0-311):" },
    { ADMIN_LEVEL_SENIOR, "Enterijer",                       "setint %s",       "Enterijer (0-255):" },
    { ADMIN_LEVEL_SENIOR, "Virtuelni svet",                  "setvw %s",        "Virtuelni svet:" },
    { ADMIN_LEVEL_SENIOR, "Skini poslednje upozorenje",      "unwarn %s",       "" },
    { ADMIN_LEVEL_HEAD,   "Nivo",                            "setlevel %s",     "Nivo (1-100):" },
    { ADMIN_LEVEL_HEAD,   "Respect",                         "setrespect %s",   "Respect (manje od potrebnog za sledeci nivo):" },
    { ADMIN_LEVEL_HEAD,   "Glad",                            "setglad %s",      "Glad (0-100):" },
    { ADMIN_LEVEL_HEAD,   "Promena imena",                   "setname %s",      "Novo ime (Ime_Prezime):" },
    { ADMIN_LEVEL_HEAD,   "Licna karta (daj / oduzmi)",      "setlicna %s",     "1 daje, 0 oduzima:" },
    { ADMIN_LEVEL_HEAD,   "Daj predmet",                     "dajpredmet %s",   "" },
    { ADMIN_LEVEL_HEAD,   "Uzmi predmet",                    "uzmipredmet %s",  "" },
    { ADMIN_LEVEL_HEAD,   "Ponisti kazne sa kamera",         "ponistikazne %s", "" },
    { ADMIN_LEVEL_HEAD,   "Daj vozilo",                      "dajvozilo %s",    "Model vozila (400-611):" },
    { ADMIN_LEVEL_HEAD,   "Helper nivo",                     "makehelper %s",   "Helper nivo (0 skida):" },
    { ADMIN_LEVEL_MANAGEMENT, "Admin nivo",                  "makeadmin %s",    "Admin nivo (0 skida):" },
    { ADMIN_LEVEL_OWNER,  "Novac (gotovina)",                "givemoney %s",    "Iznos (negativan oduzima):" },
    { ADMIN_LEVEL_OWNER,  "Novac (racun)",                   "givebank %s",     "Iznos (negativan oduzima):" },
    { ADMIN_LEVEL_OWNER,  "VIP",                             "setvip %s",       "" }
};

cmd:aigrac(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }
    new targetid = Staff_ParseTarget(playerid, params, "/aigrac [id ili ime] (sve admin opcije za igraca u jednom meniju)");
    if (targetid == INVALID_PLAYER_ID)
    {
        return 1;
    }
    new title[64], subject[8];
    format(title, sizeof(title), "Igrac: %s (%d)", Account_Name(targetid), targetid);
    valstr(subject, targetid);
    Menu_Show(playerid, title, g_PlayerMenu, sizeof(g_PlayerMenu), subject);
    return 1;
}
alias:aigrac("amenu")
