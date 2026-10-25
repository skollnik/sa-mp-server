cmd:pomoc(playerid, params[])
{
    ShowPlayerDialog(playerid, DIALOG_HELP, DIALOG_STYLE_MSGBOX, SERVER_NAME " - Pomoc",
        C_SERVER "Razgovor\n" \
        C_WHITE "Obican tekst - govor ljudima oko vas\n" \
        "/s - vika    /l - tihi govor    /w - sapat jednom igracu\n\n" \
        C_SERVER "Radnje (RP)\n" \
        C_WHITE "/me - radnja vaseg karaktera    /do - opis okoline ili stanja\n" \
        "/ame i /ado - isto, ali se vidi iznad glave\n\n" \
        C_SERVER "OOC (van uloge)\n" \
        C_WHITE "/b - lokalni OOC    /pm - privatna poruka\n" \
        "/o - globalni OOC (radi samo kad ga admin ukljuci)\n" \
        "/togooc, /togpm, /tognewbie - iskljucivanje chatova za sebe\n\n" \
        C_SERVER "Pomoc i podrska\n" \
        C_WHITE "/n - newbie chat (pitanja novih igraca)    /ask - pitanje helperima\n" \
        "/report - prijava igraca adminima    /admins, /helpers - tim online\n\n" \
        C_SERVER "Nalog\n" \
        C_WHITE "/promenilozinku - promena lozinke",
        "Zatvori", "");
    return 1;
}

alias:pomoc("help", "komande")
flags:pomoc(CMD_FLAG_JAIL_OK)
