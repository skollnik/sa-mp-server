// One window per topic: the whole text in one dialog ran past both the dialog limit and the compiler's line limit.
static const g_HelpTopics[][] =
{
    "Razgovor i RP", "OOC i podesavanja", "Svet i novac", "Inventar i telefon", "Vozila", "Kuca", "Posao", "Pomoc, VIP i nalog"
};

static stock Help_ShowTopics(playerid)
{
    new body[256];
    for (new i = 0; i < sizeof(g_HelpTopics); i++)
    {
        strcat(body, g_HelpTopics[i]);
        strcat(body, "\n");
    }
    ShowPlayerDialog(playerid, DIALOG_HELP, DIALOG_STYLE_LIST, SERVER_NAME " - Pomoc", body, "Otvori", "Zatvori");
}

static stock Help_ShowTopic(playerid, topic)
{
    new body[2048];
    switch (topic)
    {
        case 0:
        {
            strcat(body, C_WHITE "Obican tekst - govor ljudima oko vas\n");
            strcat(body, "/s - vika    /l - tihi govor    /w - sapat jednom igracu    /cw - sapat u vozilu\n\n");
            strcat(body, "/me - radnja vaseg karaktera    /do - opis okoline ili stanja\n");
            strcat(body, "/ame i /ado - isto, ali se vidi iznad glave");
        }
        case 1:
        {
            strcat(body, C_WHITE "/b - lokalni OOC    /pm - privatna poruka\n");
            strcat(body, "/o - globalni OOC (radi samo kad ga admin ukljuci)\n");
            strcat(body, "/kanali - ukljucivanje i iskljucivanje chatova (ili /togooc, /togpm, /tognewbie)\n");
            strcat(body, "/podesavanja - HUD i kanali (pamti se i posle izlaska); melodija se bira u /telefon");
        }
        case 2:
        {
            strcat(body, C_WHITE "F ili /udji, /izadji - vrata zgrada    /gps - navigacija    /hud - sat i kvart\n");
            strcat(body, "/stats - statistika    /daj - novac igracu pored vas    /licna - licna karta\n");
            strcat(body, "/banka, /bankomat, /opstina, /kupi - na salterima i u prodavnicama (oznaka iznad mesta)\n");
            strcat(body, "Racun i kartica se otvaraju na salteru banke (/banka, uz licnu kartu), tamo dobijate i PIN za bankomat\n");
            strcat(body, "/kockice, /novcic - opklada sa igracem pored vas (od nivoa 2, kockice ili novcic iz 24/7)\n");
            strcat(body, "/loto - lutrija, izvlacenje svakog sata\n");
            strcat(body, "/prosim - prosnja (do nivoa 4)    /udeli - novac igracu koji prosi");
        }
        case 3:
        {
            strcat(body, C_WHITE "/inv - inventar    /podigni - podizanje bacenog predmeta    F - baca cigaretu ili flasu iz ruke\n");
            strcat(body, "/dodaci - stavljanje i skidanje naocara, kapa, ranca i torbe (kupuju se u prodavnici odece, /kupi)\n");
            strcat(body, "Traka gladi je ispod novca: jedite u prodavnici 24/7, na nuli gubite HP\n\n");
            strcat(body, "/telefon - meni telefona (broj, kredit, imenik)    /pozovi /javi /prekini /sms /kontakti\n");
            strcat(body, "Brojevi 911 (hitne sluzbe) i 555 (mehanicar) su uvek u imeniku\n");
            strcat(body, "SIM kartica i dopuna kredita se kupuju u prodavnici 24/7 (/kupi)");
        }
        case 4:
        {
            strcat(body, C_WHITE "N - motor (pali se uz kljuc; upaljeno vozilo moze da vozi svako)    Y - zakljucavanje najblizeg vaseg vozila\n");
            strcat(body, "/v - vasa vozila (GPS, registracija, oduzimanje kopije kljuca)    /v parkiraj - mesto gde se vozilo vraca\n");
            strcat(body, "/v [motor / zakljucaj / svetla / hauba / gepek / neon]    /v kljuc - kopija kljuca igracu pored vas\n");
            strcat(body, "/gorivo [litara] - tocenje na pumpi (za volanom, ugasen motor; peske puni kanistere)\n");
            strcat(body, "/registruj - tehnicki pregled (vazi 60 dana)    /tablice - tablice u policiji    /v tablice - postavljanje\n");
            strcat(body, "/gepek - stvari u gepeku (stanite iza vozila; otvoren sa /v gepek)    Kanister: /inv pored vozila\n");
            strcat(body, "/pijaca - auto pijaca    /kupivozilo - kupovina na pijaci    /ponudi - prodaja ili zamena vozila\n");
            strcat(body, "/kazne - kazne sa kamera (placaju se u opstini ili policiji)\n");
            strcat(body, "/pojas - pojas u vozilu    /kaciga - kaciga na motoru (kupuje se u prodavnici odece)\n");
            strcat(body, "/skola - auto skola    /dozvole - vase dozvole    /pokazidozvole - igracu pored vas\n");
            strcat(body, "/rent [sati] - u vozilu sa natpisom [RENT]    /vratirent - vracanje (iz vozila)\n");
            strcat(body, "/pozovi 555 [opis] - mehanicar    /servis - popravka do 100% u servisu kad nema mehanicara\n");
            strcat(body, "Letelice se servisiraju u servisu letelica, brodovi u servisu brodova; vozilo posla radnik servisira besplatno");
        }
        case 5:
        {
            strcat(body, C_WHITE "/kupikucu - ispred kuce na prodaju (zelena kucica na mapi)\n");
            strcat(body, "/kuca - meni kuce (informacije, zakljucavanje, GPS, pojavljivanje u kuci, prodaja drzavi)\n");
            strcat(body, "F ili /udji, /izadji - ulaz i izlaz    /kucaj - kucanje na vrata (cuju oni u kuci)\n");
            strcat(body, "/garaza - ispred garaze kuce: za volanom ostavlja vozilo, peske ga vadi\n");
            strcat(body, "/v parkiraj ispred garaze svoje kuce - parking u garazi (vozilo se tu vraca posle restarta i unistenja)\n");
            strcat(body, "/ponudi - prodaja kuce igracu pored vas (stojite kod kuce)");
        }
        case 6:
        {
            strcat(body, C_WHITE "/posao - poslovi u opstini    /posaoinfo - uputstvo    /oprema - pocetak i kraj smene    /otkaz\n");
            strcat(body, "Mehanicar: /popravi [id] [do %]    /tune [id]    /prihvatipoziv [id]    /vuci (iz slepera)");
        }
        default:
        {
            strcat(body, C_WHITE "/n - newbie chat (pitanja novih igraca)    /ask - pitanje helperima\n");
            strcat(body, "/report - prijava igraca adminima    /admins, /helpers - tim online    /ahelp, /hhelp - komande tima\n\n");
            strcat(body, "/vip - VIP nivoi i pogodnosti    /vips - VIP igraci online    /vc - VIP chat\n\n");
            strcat(body, "/promenilozinku - promena lozinke");
        }
    }
    ShowPlayerDialog(playerid, DIALOG_HELP_TOPIC, DIALOG_STYLE_MSGBOX, g_HelpTopics[topic], body, "Nazad", "Zatvori");
}

cmd:pomoc(playerid, params[])
{
    Help_ShowTopics(playerid);
    return 1;
}

alias:pomoc("help", "komande")
flags:pomoc(CMD_FLAG_JAIL_OK)

stock bool:Help_OnDialogResponse(playerid, dialogid, response, listitem)
{
    if (dialogid == DIALOG_HELP)
    {
        if (response && listitem >= 0 && listitem < sizeof(g_HelpTopics))
        {
            Help_ShowTopic(playerid, listitem);
        }
        return true;
    }
    if (dialogid == DIALOG_HELP_TOPIC)
    {
        if (response)
        {
            Help_ShowTopics(playerid);
        }
        return true;
    }
    return false;
}
