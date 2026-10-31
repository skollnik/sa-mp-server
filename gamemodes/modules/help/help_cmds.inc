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
        "/kanali - ukljucivanje i iskljucivanje chatova (ili /togooc, /togpm, /tognewbie)\n" \
        "/podesavanja - HUD i kanali (pamti se i posle izlaska); melodija se bira u /telefon\n\n" \
        C_SERVER "Svet i novac\n" \
        C_WHITE "F ili /udji, /izadji - vrata zgrada    /gps - navigacija    /hud - sat i kvart\n" \
        "/stats - statistika    /daj - novac igracu pored vas    /licna - licna karta\n" \
        "/banka, /bankomat, /opstina, /kupi - na salterima i u prodavnicama (oznaka iznad mesta)\n" \
        "Racun i kartica se otvaraju na salteru banke (/banka, uz licnu kartu), tamo dobijate i PIN za bankomat\n" \
        "/kockice, /novcic - opklada sa igracem pored vas (od nivoa 2, kockice ili novcic iz 24/7)    /loto - lutrija, izvlacenje svakog sata\n" \
        "/prosim - prosnja (do nivoa 4)    /udeli - novac igracu koji prosi\n\n" \
        C_SERVER "Inventar i telefon\n" \
        C_WHITE "/inv - inventar    /podigni - podizanje bacenog predmeta    F - baca cigaretu ili flasu iz ruke\n" \
        "/dodaci - stavljanje i skidanje naocara, kapa, ranca i torbe (kupuju se u prodavnici odece, /kupi)\n" \
        "Traka gladi je ispod novca: jedite u prodavnici 24/7, na nuli gubite HP\n" \
        "/telefon - meni telefona (broj, kredit, imenik)    /pozovi /javi /prekini /sms /kontakti (911 - hitne sluzbe)\n" \
        "SIM kartica i dopuna kredita se kupuju u prodavnici 24/7 (/kupi)\n\n" \
        C_SERVER "Vozila\n" \
        C_WHITE "N - motor (pali se uz kljuc; upaljeno vozilo moze da vozi svako)    Y - zakljucavanje najblizeg vaseg vozila\n" \
        "/v - vasa vozila (GPS, registracija, oduzimanje kopije kljuca)    /v parkiraj - mesto gde se vozilo vraca\n" \
        "/v [motor / zakljucaj / svetla / hauba / gepek]    /v kljuc - kopija kljuca igracu pored vas\n" \
        "/gorivo - tocenje na benzinskoj pumpi (ugasen motor)    /registruj - tehnicki pregled (vazi 60 dana)\n" \
        "/pijaca - auto pijaca (kad je otvorena)    /kupivozilo - kupovina na pijaci    /ponudi - prodaja ili zamena vozila igracu pored vas\n" \
        "/tablice - tablice na salteru u policiji    /v tablice - postavljanje tablica    /kazne - kazne sa kamera (placaju se u opstini ili policiji)\n\n" \
        C_SERVER "Pomoc i podrska\n" \
        C_WHITE "/n - newbie chat (pitanja novih igraca)    /ask - pitanje helperima\n" \
        "/report - prijava igraca adminima    /admins, /helpers - tim online    /ahelp, /hhelp - komande tima\n\n" \
        C_SERVER "VIP\n" \
        C_WHITE "/vip - VIP nivoi i pogodnosti    /vips - VIP igraci online    /vc - VIP chat\n\n" \
        C_SERVER "Nalog\n" \
        C_WHITE "/promenilozinku - promena lozinke",
        "Zatvori", "");
    return 1;
}

alias:pomoc("help", "komande")
flags:pomoc(CMD_FLAG_JAIL_OK)
