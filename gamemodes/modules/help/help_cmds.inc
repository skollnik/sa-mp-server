cmd:pomoc(playerid, params[])
{
    ShowPlayerDialog(playerid, DIALOG_HELP, DIALOG_STYLE_MSGBOX, SERVER_NAME " - Pomoc",
        C_SERVER "Razgovor\n" \
        C_WHITE "Obican tekst - govor ljudima oko vas\n" \
        "/s - vika    /l - tihi govor    /w - sapat jednom igracu\n\n" \
        C_SERVER "Radnje (RP)\n" \
        C_WHITE "/me - radnja vaseg karaktera\n" \
        "/do - opis okoline ili stanja\n" \
        "/ame - radnja koja se vidi iznad glave\n\n" \
        C_SERVER "OOC (van uloge)\n" \
        C_WHITE "/b - lokalni OOC    /o - globalni OOC    /pm - privatna poruka\n\n" \
        C_SERVER "Nalog\n" \
        C_WHITE "/promenilozinku - promena lozinke",
        "Zatvori", "");
    return 1;
}

alias:pomoc("help", "komande")
