# Odluke / Decisions

## 🇷🇸 Srpski

Ovde se beleži svaka važna odluka, da se ne bi ponovo raspravljalo o njoj. Novije odluke idu na dno tabele.

| # | Datum | Odluka | Razlog |
|---|-------|--------|--------|
| 1 | 2026-10-21 | Platforma: open.mp 1.6.7 (win-x86), Pawn | Moderna zamena za SA-MP server, kompatibilna sa 0.3.7/0.3DL klijentima |
| 2 | 2026-10-21 | Tekst u igri na srpskom, **bez dijakritika** (c, s, z, dj) | GTA SA font ne prikazuje č ć š ž đ |
| 3 | 2026-10-21 | Kod, komentari i imena fajlova na engleskom | Standard |
| 4 | 2026-10-21 | `.md` dokumentacija dvojezična: srpski, pa engleski ispod | Zahtev vlasnika |
| 5 | 2026-10-21 | Baza: **MySQL (MariaDB preko XAMPP-a)** + plugin **mysql R41-4** | R41-4 je industrijski standard za SA-MP i pouzdan je. MariaDB izbegava problem sa MySQL 8 autentifikacijom (`caching_sha2_password`) koji R41-4 ima |
| 6 | 2026-10-21 | Biblioteke: `sscanf 2.13.8`, `Pawn.CMD 3.4.0-omp`, `streamer 2.9.6`, `samp-bcrypt 0.4.1`, `crashdetect 4.22` (za debug) | Najčešće korišćene i proverene biblioteke, sve rade na open.mp |
| 7 | 2026-10-21 | Bez YSI biblioteke. Moduli se povezuju ručnim pozivima iz glavnih callback-ova | Jednostavniji i čitljiviji kod, brže kompajliranje, nema "magije" |
| 8 | 2026-10-21 | Editor: VS Code (qawno editor obrisan, kompajler `pawncc` ostaje) | Vlasnik koristi VS Code |
| 9 | 2026-10-21 | Server je za sada lokalni (Windows) | Hosting se rešava kasnije |
| 10 | 2026-10-21 | Ime servera: `SA-MP Test Server`, u porukama "Test Server", 50 slotova | Privremeno ime, dok se ne smisli pravo |
| 11 | 2026-10-21 | Radnja se odvija uglavnom u Los Santosu | Kao na većini RP servera |
| 12 | 2026-10-21 | Heavy (pravi) RP, sa punim spektrom frakcija | Zahtev vlasnika |
| 13 | 2026-10-21 | Git: prate se izvorni kod, include-ovi, kompajler, server binarni fajlovi i pluginovi. Ignorišu se `.amx`, logovi i lozinke za bazu | Kada se repo klonira, server se odmah pokreće. Tajne podatke ne guramo na GitHub |
| 14 | 2026-10-21 | Pristupni podaci za bazu idu u `mysql.ini` (ignorisan u gitu). U repou je samo `mysql.ini.example` | Bezbednost |
| 15 | 2026-10-21 | Server sam pravi tabele pri pokretanju (`CREATE TABLE IF NOT EXISTS`), svaki modul svoje | Vlasnik ne mora ništa ručno da importuje |
| 16 | 2026-10-21 | Jedan nalog = jedan karakter, ime obavezno `Ime_Prezime`. Kratko ime ima samo vlasnik (`skollnik`) i oni kojima ga on dodeli | Zahtev vlasnika |
| 17 | 2026-10-21 | Nema kviza pri registraciji, pravila se samo prikazuju | Zahtev vlasnika |
| 18 | 2026-10-21 | Postoje nivoi (respect poeni na payday) | Zahtev vlasnika |
| 19 | 2026-10-21 | Nema stalnog auto salona. Vozila se kupuju samo na povremenim auto pijacama (admin event) ili od igrača | Vozila treba da budu retka |
| 20 | 2026-10-21 | Realna ekonomija: start $5.000 ($2.000 keš + $3.000 banka), cene vezane za stvarni život. Cene su u `ECONOMY.md` | Novac i vozila treba da budu retki i vredni |
| 21 | 2026-10-21 | Novi igrači dobijaju nasumičan skin (3 muška: 7, 47, 60; 3 ženska: 40, 56, 93) i pojavljuju se ispred aerodroma u Los Santosu | Zahtev vlasnika |
| 22 | 2026-10-21 | Email se za sada ne traži pri registraciji | Nije potreban dok ne postoji web panel |
| 23 | 2026-10-21 | Dve odvojene lestvice tima: Helper 1–3 i Admin 1–5 + Vlasnik (6). Viši nivo ima sve komande nižeg | Usvojen predlog |
| 24 | 2026-10-21 | Admin komande rade samo na dužnosti (`/aduty`), osim admin chata. Vlasnik je izuzet | Na dužnosti je jasno ko je admin, a van nje admin igra kao igrač |
| 25 | 2026-10-21 | Helperi ne kažnjavaju (nema kick/ban). Odgovaraju na pitanja, a prekršaje prijavljuju adminima. Od nivoa 2 mogu da daju mute u newbie chatu | Uobičajena uloga helpera |
| 26 | 2026-10-21 | 3 upozorenja (`/warn`) = automatski ban 3 dana | Usvojen predlog |
| 27 | 2026-10-21 | Admini od nivoa 4 mogu da prate PM-ove (`/pmspy`) | Usvojen predlog |
| 28 | 2026-10-23 | Imena su uvek bela. Tim na dužnosti ima oznaku iznad imena: crno [ADMIN]/[VLASNIK], žuto [HELPER]. Admin na dužnosti nosi skin osoblja (217/211) | Zahtev vlasnika |
| 29 | 2026-10-23 | Igrač je ili admin ili helper, ne oba | Admini već imaju sve helper komande |
| 30 | 2026-10-23 | `/ahelp` prikazuje samo komande do sopstvenog nivoa | Uobičajeno: igrač vidi samo ono što može da koristi |
| 31 | 2026-10-24 | Newbie chat čitaju svi, a pišu samo igrači do nivoa 3 i tim | Iskusni igrači mogu da pozovu helpera |
| 32 | 2026-10-24 | Anti-cheat: automatski kick za oružje i jetpack; za teleport, brzinu, HP i armor samo upozorenje adminima | Prvo su sigurni cheatovi, a drugo ima lažnih uzbuna (lag, padovi) |
| 33 | 2026-10-24 | Flood: blokada i upozorenje, a posle 3 upozorenja u minuti automatski mute od 2 minuta | Uobičajeno na RP serverima |
| 34 | 2026-10-27 | Flood se meri u milisekundama (kazneni poeni), a ponovljena poruka se računa kao upozorenje | Stara provera u sekundama nije hvatala ručni spam |
| 35 | 2026-10-27 | Zaključavanje posle pogrešnih lozinki važi po nalogu i IP adresi | Da niko ne može namerno da zaključa tuđi nalog |
| 36 | 2026-10-27 | U admin zatvoru rade samo chat komande i komande označene kao bezbedne | Da se zatvor ne izbegne preko `/goto` ili `/spec` |
| 37 | 2026-10-27 | VIP: Bronze, Silver, Gold, mesečno, dodeljuje samo vlasnik, bez prednosti u igri i bez admin moći | Uobičajeno na ozbiljnim RP serverima |
| 38 | 2026-10-27 | VIP cene: Bronze 3 €, Silver 5 €, Gold 10 € mesečno; sve predložene pogodnosti su odobrene | Odluka vlasnika |
| 39 | 2026-10-27 | Iznad glave se vidi jedna oznaka: admin na dužnosti, pa helper na dužnosti, pa VIP | Dve oznake bi zatrpale ekran |
| 40 | 2026-10-27 | Kanali: svaki chat i obaveštenje može da se isključi preko `/kanali`, osim reportova, AC upozorenja i pitanja dok si na dužnosti | Tim ne sme da propusti ono zbog čega je na dužnosti |
| 41 | 2026-10-28 | Mape se pišu u našem obliku: jedan fajl po mapi u `maps/exteriors` ili `maps/interiors`, objekti preko `Map_Exterior` i `Map_Interior`, a uklonjeni GTA objekti u funkciji `Map_X_Remove(playerid)` | Isti izgled koda, lako se dodaje i briše mapa |
| 42 | 2026-10-28 | Spoljne mape su u svetu 0 i enterijeru 0. Custom enterijeri su visoko iznad grada, u enterijeru 20 i svim virtuelnim svetovima | Ne vide se iz grada, grad se ne vidi kroz njihove zidove, a vrata (WORLD-1) kasnije samo postave enterijer 20 |
| 43 | 2026-10-28 | Igrač vidi do 1000 objekata odjednom (streamer) | Najveći enterijer ima skoro 1000 objekata, a sa podrazumevanih 500 bi mu falili zidovi |
| 44 | 2026-10-28 | GTA zgrada kojoj treba promeniti izgled (npr. natpisi na pozorištu pored banke) uklanja se i pravi ponovo kao objekat, sa zamenjenim materijalom | GTA zgradama se materijal ne može menjati direktno |
| 45 | 2026-10-28 | Medicinska karta važi 3 meseca i izdaju je doktori iz bolnice | Da igrači ne obnavljaju kartu prečesto |
| 46 | 2026-10-28 | Brak ne menja prezime; u `/stats` piše „u braku sa X“ | Ime naloga je vezano za logove i banove |
| 47 | 2026-10-29 | Glad bez žeđi; hrana daje malo HP-a i ne može da se jede kad je igrač sit; pun HP vraćaju samo bolnica i hitna | Hrana ima smisla, a HP se ne vraća prejedanjem |
| 48 | 2026-10-29 | Objekti spoljnih mapa se vide do 300 m, a velike ploče tla i ponovo napravljene GTA zgrade do 800 m (`Map_Exterior`) | Te ploče i zgrade zamenjuju uklonjene GTA modele za daljinu, pa bi bez toga izdaleka ostala rupa |
| 49 | 2026-10-29 | `/vskin` i ostali izbori modela (odeća, vozila) idu kroz mrežu sa slikama (`Preview_Show`), klik bira, ESC zatvara | Igrač vidi šta bira; isti meni služi za sve |
| 50 | 2026-10-29 | Vrata: taster F i komande `/udji` i `/izadji`; admin ih pravi u igri; svaka vrata imaju svoj virtuelni svet (1000 + ID) | Oba načina su uobičajena; ulazi se dodaju bez menjanja koda |
| 51 | 2026-10-29 | Šalteri, bankomati i prodavnice su „mesta“ koja admin postavlja u igri (`/napravimesto`); komanda radi samo pored mesta | Isti sistem za sve usluge, bez koordinata u kodu |
| 52 | 2026-10-29 | Vreme u igri prati stvarno vreme; admin može da ga zadrži (`/settime`), VIP ima lično (`/vtime`) | Odluka vlasnika („generalno kao na serverima“) |
| 53 | 2026-10-29 | Payday je na svaki pun sat za sve koji su od prošle plate bili aktivni bar 30 min; kraće vreme se prenosi; novac ide na račun | Uobičajen način; AFK i ponovni ulazak ne donose platu |
| 54 | 2026-10-29 | Nivo raste automatski kad se skupi N × 4 respecta; višak respecta ostaje | Odluka vlasnika |
| 55 | 2026-10-29 | Inventar sa slotovima: 10 mesta + VIP (2/4/6), predmeti se slažu do najveće količine | Jednostavnije za igrače od težine; odluka prepuštena meni |
| 56 | 2026-10-29 | Do firmi (Faza 4) postoje državne prodavnice 24/7 sa cenama iz `core/economy.inc` | Igrači moraju negde da kupe telefon i hranu |
| 57 | 2026-10-29 | Sve cene, plate i granice su u `core/economy.inc` | Balans se menja na jednom mestu |
| 58 | 2026-10-29 | Novac se menja samo kroz `Money_GiveCash` i `Money_GiveBank`, uz zapis u `money_logs` | Svaka promena novca može da se prati |
| 59 | 2026-10-29 | Transfer na račun: od nivoa 2, provizija 1%; bankomat do $10.000 odjednom; lična karta $50 | Otežava prebacivanje sa lažnih naloga |
| 60 | 2026-10-29 | Telefon: nasumičan broj od 6 cifara (Silver i Gold biraju broj), SMS $1, minut poziva $2 koje plaća pozivalac; 911 ide adminima dok ne dođu službe | Novac izlazi iz igre, a hitni pozivi ipak stižu do nekoga |
| 61 | 2026-10-29 | ID obrisanog ulaza, mesta ili GPS lokacije dobija sledeće napravljeno (uvek najmanji slobodan); brisanje ulaza izvodi sve iz njegovog sveta i briše mesta u njemu | Zahtev vlasnika; bez čišćenja bi novi ulaz sa istim ID-jem nasledio stari svet |
| 62 | 2026-10-29 | Ista poruka je spam samo ako se ponovi u roku od 10 s; `/pozovi` nije chat komanda (911 tekst ipak prolazi proveru za mute i spam) | Vlasnik je dobio mute za tri obična poziva |
| 63 | 2026-10-29 | F baca cigaretu ili flašu iz ruke, pre vrata; lična karta se pokazuje u dijalogu | Zahtev vlasnika |
| 64 | 2026-10-29 | Admin koji posmatra prati enterijer i svet igrača i učitava objekte oko njega; teleport tokom posmatranja prekida posmatranje | Custom enterijeri su objekti; bez toga je admin video prazninu |
| 65 | 2026-10-29 | Glad: traka 0–100 ispod novca, pada 1 na 3 minuta igre (ne dok je igrač AFK, u zatvoru ili na admin dužnosti); na 0 se gubi 2 HP na minut, ali ne ispod 20 HP; hrana +30/+40, piće +5; iznad 90 se ne jede; posle smrti bar 50 | Odobren predlog vlasnika; dok nema bolnice niko ne umire od gladi |
| 66 | 2026-10-29 | Telefon i SIM kartica se kupuju odvojeno; broj i kredit su na nalogu, pa davanje telefona ne daje broj; nova SIM kartica menja broj, kredit ostaje | Odobren predlog vlasnika |
| 67 | 2026-10-29 | Izbor broja: Silver i Gold VIP i admini besplatno, ostali $2.000; SMS, pozivi i javni imenik se plaćaju kreditom (dopuna u 24/7 ili sa računa, najviše $500); javni imenik prikazuje samo one koji su se upisali; broj se gasi posle 180 dana neaktivnosti | Odobren predlog vlasnika; privatnost i odliv novca iz igre |
| 68 | 2026-10-29 | Respect za sledeći nivo: (sledeći nivo) × 4; happy hour se zadaje u broju plata (1–12) | Uobičajena formula; happy hour u minutima je mogao da ne pogodi nijednu platu |
| 69 | 2026-10-29 | Strelica na vratima se bira iz menija sa slikama (14 modela) i važi za ulaz i izlaz; otvoreno piće se ne vraća u inventar | Odobren predlog vlasnika |
| 70 | 2026-10-29 | `/podesavanja` je jedno mesto za izbore igrača: delovi HUD-a (sat i datum, kvart) i kanali; traka gladi ne može da se sakrije; melodija se bira u meniju telefona; sve se čuva u bazi (`setting_bits`, `ringtone`), a `/hud` se sada pamti | Odobren predlog vlasnika (ACC-9); novi sistemi samo dodaju svoj red |
| 71 | 2026-10-29 | Vozilo se prodaje samo drugom igraču, ne i državi | Odluka vlasnika; novac ne nastaje iz vazduha prodajom državi |
| 72 | 2026-10-30 | HUD i svi budući prozori se crtaju samo teksturama iz igre (pločice, zaobljene ivice, ikone sa radara), bez custom modela; HUD ima datum na pločici, traku gladi u stilu GTA traka sa ikonom, kvart na pločici sa ikonom i logo sa ID-jem i brojem igrača | Vlasnik želi da svi igrači vide isto, i na 0.3.7 i na 0.3.DL |
| 73 | 2026-10-30 | PIN kartice: 4 cifre, postavlja se na šalteru banke i čuva šifrovano (bcrypt); bankomat bez PIN-a ne isplaćuje; posle 3 pogrešna PIN-a kartica je blokirana 30 min, a na šalteru se odmah odblokira novim PIN-om; na šalteru PIN ne treba (lična karta) | Odobren predlog vlasnika |

### Arhitektura koda (predlog)
```
gamemodes/
  roleplay.pwn            ← ulazna tačka: samo include-ovi i glavni callback-ovi
  core/                   ← podešavanja, boje, makroi, util funkcije, baza
  modules/<modul>/        ← jedan sistem = jedan folder (account, character, admin, chat, vehicle...)
      <modul>_data.inc    ← enumi, nizovi, konstante
      <modul>_impl.inc    ← logika i funkcije
      <modul>_cmds.inc    ← komande
                          (tabele pravi svaki modul u svom Module_Init)
```
- Imenovanje: `Module_Action()` (npr. `Account_Load`, `Vehicle_SetFuel`), globalne promenljive `g_`, konstante `UPPER_CASE`.
- Svaki modul ima funkcije `Module_OnPlayerConnect`, `Module_OnPlayerDisconnect` itd. i one se pozivaju iz `roleplay.pwn`.
- Svi upiti ka bazi su asinhroni (`mysql_tquery`) i prolaze kroz `mysql_format` sa `%e`, zbog SQL injection-a.

### Otvorena pitanja
Vidi `PLAN.md`, sekcija "Otvorena pitanja".

---

## 🇬🇧 English

This file records every important decision so it does not get re-discussed. Newer decisions go at the bottom of the table.

| # | Date | Decision | Reason |
|---|------|----------|--------|
| 1 | 2026-10-21 | Platform: open.mp 1.6.7 (win-x86), Pawn | Modern SA-MP server replacement, compatible with 0.3.7/0.3DL clients |
| 2 | 2026-10-21 | In-game text in Serbian, **no diacritics** (c, s, z, dj) | GTA SA font cannot render č ć š ž đ |
| 3 | 2026-10-21 | Code, comments and file names in English | Standard |
| 4 | 2026-10-21 | `.md` docs bilingual: Serbian first, English below | Owner's request |
| 5 | 2026-10-21 | Database: **MySQL (MariaDB via XAMPP)** + **mysql R41-4** plugin | R41-4 is the de-facto SA-MP standard and is reliable. MariaDB avoids R41-4's MySQL 8 auth problem (`caching_sha2_password`) |
| 6 | 2026-10-21 | Libraries: `sscanf 2.13.8`, `Pawn.CMD 3.4.0-omp`, `streamer 2.9.6`, `samp-bcrypt 0.4.1`, `crashdetect 4.22` (debug) | Most widely used and proven libraries, all work on open.mp |
| 7 | 2026-10-21 | No YSI. Modules are wired up by explicit calls from the main callbacks | Simpler, readable code, faster compiles, no "magic" |
| 8 | 2026-10-21 | Editor: VS Code (qawno editor removed, `pawncc` compiler kept) | Owner uses VS Code |
| 9 | 2026-10-21 | Server is local (Windows) for now | Hosting decided later |
| 10 | 2026-10-21 | Server name `SA-MP Test Server`, "Test Server" in messages, 50 slots | Placeholder name until the real one is chosen |
| 11 | 2026-10-21 | Setting is mostly Los Santos | Like most RP servers |
| 12 | 2026-10-21 | Heavy (serious) RP with a full range of factions | Owner's request |
| 13 | 2026-10-21 | Git tracks source, includes, compiler, server binaries and plugins. It ignores `.amx`, logs and DB credentials | A clone runs immediately. Secrets stay off GitHub |
| 14 | 2026-10-21 | DB credentials live in `mysql.ini` (git-ignored). The repo only has `mysql.ini.example` | Security |
| 15 | 2026-10-21 | The server creates its tables on startup (`CREATE TABLE IF NOT EXISTS`), each module its own | The owner never has to import anything by hand |
| 16 | 2026-10-21 | One account = one character, name must be `Firstname_Lastname`. Only the owner (`skollnik`) and those they grant it to get a short name | Owner's request |
| 17 | 2026-10-21 | No quiz at registration, rules are only shown | Owner's request |
| 18 | 2026-10-21 | Levels exist (respect points on payday) | Owner's request |
| 19 | 2026-10-21 | No permanent dealership. Vehicles are bought only at occasional car markets (admin event) or from players | Vehicles should be rare |
| 20 | 2026-10-21 | Realistic economy: start with $5,000 ($2,000 cash + $3,000 bank), prices anchored to real life. Prices are in `ECONOMY.md` | Money and vehicles should be scarce and valuable |
| 21 | 2026-10-21 | New players get a random skin (3 male: 7, 47, 60; 3 female: 40, 56, 93) and spawn in front of Los Santos airport | Owner's request |
| 22 | 2026-10-21 | No email asked at registration for now | Not needed until there is a web panel |
| 23 | 2026-10-21 | Two separate staff ladders: Helper 1–3 and Admin 1–5 + Owner (6). A higher level has every command of the lower ones | Proposal accepted |
| 24 | 2026-10-21 | Admin commands work only on duty (`/aduty`), except admin chat. The owner is exempt | On duty it is clear who is an admin, off duty the admin plays as a player |
| 25 | 2026-10-21 | Helpers do not punish (no kick/ban). They answer questions and report violations to admins. From level 2 they can mute in newbie chat | Usual helper role |
| 26 | 2026-10-21 | 3 warnings (`/warn`) = automatic 3-day ban | Proposal accepted |
| 27 | 2026-10-21 | Admins level 4+ can watch PMs (`/pmspy`) | Proposal accepted |
| 28 | 2026-10-23 | Names are always white. Staff on duty get a tag above the name: black [ADMIN]/[VLASNIK], yellow [HELPER]. Admins on duty wear the staff skin (217/211) | Owner's request |
| 29 | 2026-10-23 | A player is either an admin or a helper, not both | Admins already have every helper command |
| 30 | 2026-10-23 | `/ahelp` shows only the commands up to your own level | Common practice: you only see what you can use |
| 31 | 2026-10-24 | Everyone reads newbie chat; only players up to level 3 and staff write | Experienced players can call a helper |
| 32 | 2026-10-24 | Anti-cheat: automatic kick for weapons and jetpack; teleport, speed, HP and armour only alert admins | The first are certain cheats, the second have false alarms (lag, falls) |
| 33 | 2026-10-24 | Flood: block and warn, then an automatic 2-minute mute after 3 warnings in a minute | Common on RP servers |
| 34 | 2026-10-27 | Flood is measured in milliseconds (strikes), and a repeated message counts as a warning | The old per-second check missed manual spam |
| 35 | 2026-10-27 | The wrong-password lockout applies per account and IP address | Nobody can lock someone else out on purpose |
| 36 | 2026-10-27 | In admin jail only chat commands and commands marked as safe work | Jail cannot be escaped via `/goto` or `/spec` |
| 37 | 2026-10-27 | VIP: Bronze, Silver, Gold, monthly, granted only by the owner, no in-game advantage, no admin powers | Common on serious RP servers |
| 38 | 2026-10-27 | VIP prices: Bronze €3, Silver €5, Gold €10 monthly; all proposed perks approved | Owner's decision |
| 39 | 2026-10-27 | One tag above the head: admin on duty, then helper on duty, then VIP | Two tags would clutter the screen |
| 40 | 2026-10-27 | Channels: every chat and notification can be switched off via `/kanali`, except reports, AC alerts and questions while on duty | Staff must not miss what they are on duty for |
| 41 | 2026-10-28 | Maps are written in our format: one file per map in `maps/exteriors` or `maps/interiors`, objects via `Map_Exterior` and `Map_Interior`, removed GTA objects in a `Map_X_Remove(playerid)` function | Uniform code, maps are easy to add and remove |
| 42 | 2026-10-28 | Exterior maps are in world 0 and interior 0. Custom interiors sit high above the city, in interior 20 and every virtual world | They are not visible from the city, the city does not show through their walls, and doors (WORLD-1) later just set interior 20 |
| 43 | 2026-10-28 | A player sees up to 1000 objects at once (streamer) | The biggest interior has almost 1000 objects; with the default 500 its walls would be missing |
| 44 | 2026-10-28 | A GTA building whose look must change (e.g. the banners on the theatre next to the bank) is removed and recreated as an object with the material replaced | Materials of GTA buildings cannot be changed directly |
| 45 | 2026-10-28 | The medical card is valid for 3 months and is issued by hospital doctors | So players do not renew it too often |
| 46 | 2026-10-28 | Marriage does not change the surname; `/stats` shows "married to X" | The account name is tied to logs and bans |
| 47 | 2026-10-29 | Hunger without thirst; food gives a little HP and cannot be eaten when full; only the hospital and EMS restore full HP | Food matters, and HP cannot be restored by overeating |
| 48 | 2026-10-29 | Exterior map objects are visible up to 300 m, large ground planes and rebuilt GTA buildings up to 800 m (`Map_Exterior`) | They stand in for removed GTA LOD models, so without it a hole would show from a distance |
| 49 | 2026-10-29 | `/vskin` and other model choices (clothes, vehicles) use a picture grid (`Preview_Show`); click picks, ESC closes | Players see what they pick; one menu serves everything |
| 50 | 2026-10-29 | Doors: the F key and `/udji`, `/izadji`; admins create them in game; each door has its own virtual world (1000 + ID) | Both ways are common; doors are added without code changes |
| 51 | 2026-10-29 | Counters, ATMs and shops are "points" admins place in game (`/napravimesto`); the command only works next to one | One system for every service, no coordinates in code |
| 52 | 2026-10-29 | In-game time follows real time; admins can pin it (`/settime`), VIPs have a personal one (`/vtime`) | Owner's decision ("as servers generally do") |
| 53 | 2026-10-29 | Payday comes every full hour to everyone active for at least 30 min since the last one; shorter time carries over; the money goes to the bank | The usual way; AFK and relogging earn nothing |
| 54 | 2026-10-29 | The level rises automatically once N × 4 respect is collected; leftover respect stays | Owner's decision |
| 55 | 2026-10-29 | Slot inventory: 10 slots + VIP (2/4/6), items stack up to a maximum | Simpler for players than weight; the choice was left to me |
| 56 | 2026-10-29 | Until businesses (Phase 4), state 24/7 shops sell at prices from `core/economy.inc` | Players need somewhere to buy a phone and food |
| 57 | 2026-10-29 | Every price, wage and limit is in `core/economy.inc` | Balancing happens in one place |
| 58 | 2026-10-29 | Money only changes through `Money_GiveCash` and `Money_GiveBank`, logged in `money_logs` | Every money change can be traced |
| 59 | 2026-10-29 | Bank transfers: from level 2, 1% fee; ATM up to $10,000 at once; ID card $50 | Makes moving money from alt accounts harder |
| 60 | 2026-10-29 | Phone: random 6-digit number (Silver and Gold pick one), SMS $1, $2 per call minute paid by the caller; 911 goes to admins until the services exist | Money leaves the game, and emergency calls still reach someone |
| 61 | 2026-10-29 | The ID of a deleted door, point or GPS location goes to the next one created (always the lowest free one); deleting a door moves everyone out of its world and deletes the points inside | Owner's request; without the cleanup a new door with the same ID would inherit the old world |
| 62 | 2026-10-29 | The same message is spam only if repeated within 10 s; `/pozovi` is not a chat command (911 text still goes through the mute and spam check) | The owner got muted for three ordinary calls |
| 63 | 2026-10-29 | F throws away a cigarette or bottle in hand, before doors; the ID card is shown in a dialog | Owner's request |
| 64 | 2026-10-29 | A spectating admin follows the player's interior and world and loads objects around them; teleporting while spectating ends it | Custom interiors are objects; without this the admin saw nothing |
| 65 | 2026-10-29 | Hunger: a 0–100 bar under the money, drops 1 per 3 minutes played (not while AFK, jailed or on admin duty); at 0 the player loses 2 HP a minute, but not below 20 HP; food +30/+40, drinks +5; no eating above 90; at least 50 after death | Owner approved the proposal; until the hospital exists nobody dies of hunger |
| 66 | 2026-10-29 | The phone and the SIM card are bought separately; number and credit belong to the account, so giving the phone away does not give the number; a new SIM card changes the number, the credit stays | Owner approved the proposal |
| 67 | 2026-10-29 | Picking a number: free for Silver and Gold VIP and admins, $2,000 for others; SMS, calls and the public directory are paid with credit (top-up at a 24/7 or from the bank, at most $500); the directory only shows those who listed themselves; a number is freed after 180 days of inactivity | Owner approved the proposal; privacy and money leaving the game |
| 68 | 2026-10-29 | Respect for the next level: (next level) × 4; happy hour is set in paydays (1–12) | The usual formula; happy hour in minutes could miss every payday |
| 69 | 2026-10-29 | A door's arrow is picked from a picture menu (14 models) and applies to both sides; an opened drink does not go back to the inventory | Owner approved the proposal |
| 70 | 2026-10-29 | `/podesavanja` is one place for player choices: HUD parts (clock and date, district) and channels; the hunger bar cannot be hidden; the ringtone is picked in the phone menu; everything is stored in the DB (`setting_bits`, `ringtone`), and `/hud` is now remembered | Owner approved the proposal (ACC-9); new systems just add their row |
| 71 | 2026-10-29 | A vehicle can only be sold to another player, not to the state | Owner's decision; selling to the state would create money out of thin air |
| 72 | 2026-10-30 | The HUD and every future screen are drawn only with textures from the game (plates, round ends, radar icons), no custom models; the HUD has the date on a plate, a GTA-style hunger bar with an icon, the district on a plate with an icon and a logo with the ID and player count | The owner wants every player to see the same, on 0.3.7 and 0.3.DL |
| 73 | 2026-10-30 | Card PIN: 4 digits, set at the bank counter and stored hashed (bcrypt); an ATM pays nothing without it; after 3 wrong PINs the card is blocked for 30 min, and a new PIN at the counter unblocks it right away; the counter needs no PIN (ID card) | Owner approved the proposal |

### Code architecture (proposal)
```
gamemodes/
  roleplay.pwn            ← entry point: only includes and main callbacks
  core/                   ← settings, colors, macros, utils, database
  modules/<module>/       ← one system = one folder (account, character, admin, chat, vehicle...)
      <module>_data.inc   ← enums, arrays, constants
      <module>_impl.inc   ← logic and functions
      <module>_cmds.inc   ← commands
                          (each module creates its tables in Module_Init)
```
- Naming: `Module_Action()` (e.g. `Account_Load`, `Vehicle_SetFuel`), globals prefixed `g_`, constants `UPPER_CASE`.
- Each module exposes `Module_OnPlayerConnect`, `Module_OnPlayerDisconnect` etc., which are called from `roleplay.pwn`.
- All DB queries are async (`mysql_tquery`) and go through `mysql_format` with `%e` to prevent SQL injection.

### Open questions
See `PLAN.md`, section "Open questions".
