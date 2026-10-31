# Tim: admini i helperi / Staff: admins and helpers

## 🇷🇸 Srpski

Kompletan spisak ko šta može. Svaki novi sistem dodaje svoje komande ovde.
Viši nivo uvek ima sve komande nižeg nivoa.

### Pravila
- Postoje dve odvojene lestvice: **Helper 1–3** i **Admin 1–5 + Vlasnik (6)**. Igrač je na jednoj od njih, ne na obe.
- **Admin komande rade samo na dužnosti** (`/aduty`), osim admin chata `/a`. Vlasnik je izuzet.
- **Helper komande rade na helper dužnosti** (`/hduty`), osim helper chata `/hc`. Svaki admin može da koristi helper komande.
- **Helperi ne kažnjavaju.** Odgovaraju na pitanja, a prekršaje prijavljuju adminima.
- Igrač je **ili admin ili helper**, ne oba. Kad se helper postavi za admina, helper status se automatski skida. Admina ne možete postaviti za helpera dok mu ne skinete admin status.
- Admin vozila (`/veh`) može da vozi samo admin, ostali mogu da se voze kao putnici. Brišu se kad admin koji ih je napravio izađe sa servera.
- Bez dozvole za komandu igrač dobija poruku „Nemate dozvolu za ovu komandu.“ Admin bez dužnosti dobija poruku da uključi `/aduty`.
- `/ahelp` prikazuje komande do vašeg nivoa.
- Admin ne može da kazni, zamrzne ili preimenuje admina **istog ili višeg** nivoa.
- Imena igrača su uvek bela. Na dužnosti iznad imena stoji oznaka: crno **[ADMIN]** (vlasnik **[VLASNIK]**), a žuto **[HELPER]**. Igrač ne vidi oznaku iznad svoje glave, vide je drugi.
- Admin na dužnosti nosi skin osoblja (217 muški, 211 ženski). Kad izađe sa dužnosti, vraća mu se njegov skin. Helperi zadržavaju svoj skin.
- Svaka admin akcija se upisuje u tabelu `admin_logs` (ko, šta, kome, detalji, vreme).

### Komande za sve igrače
| Komanda | Opis |
|---------|------|
| `/report [id] [razlog]` | prijava igrača adminima (jednom u 60 s) |
| `/ask [pitanje]` | pitanje helperima (jednom u 30 s). Ako nema helpera na dužnosti, vidi ga ceo tim |
| `/n [tekst]` | newbie chat, za igrače do nivoa 3 i za tim. Igrači pišu jednom u 30 s |
| `/tognewbie` | isključi/uključi newbie chat za sebe |
| `/admins`, `/helpers` | ko je od tima online i ko je na dužnosti |

### Helperi
| Nivo | Naziv | Komande |
|------|-------|---------|
| 1 | Helper | `/hduty`, `/hc`, `/odg [id] [odgovor]`, `/hhelp` |
| 2 | Iskusni helper | `/goto`, `/nmute [id] [min]` i `/unnmute` (zabrana pisanja u newbie chat) |
| 3 | Glavni helper | `/makehelper` (postavlja i skida helpere nivoa 1–2) |

### Admini
| Nivo | Naziv | Komande |
|------|-------|---------|
| 1 | Admin pocetnik | `/aduty`, `/a`, `/ahelp`, `/reports`, `/ar`, `/rr`, `/goto`, `/gethere`, `/spec`, `/specoff`, `/check`, `/freeze`, `/unfreeze`, `/slap`, `/kick`, `/mute`, `/unmute`, `/ajail` (do 30 min), `/unjail`, `/warns`, `/aclog`, `/kanali`, vozila: `/vinfo [id]`, `/gotoveh [id]`, `/atune` (admin vozila), `/aigrac [igrac]` (meni sa svim opcijama za igraca, po nivou), `/izmenivozilo [id]` (meni, izmene od nivoa 4) |
| 2 | Admin | `/sethp`, `/setarmour`, `/tp` (GPS lokacije, a dok ih nema opšte lokacije), `/gotoulaz`, `/ooc` (globalni OOC), `/cc` (briše chat svima), `/ao` (admin obaveštenje u chatu), `/fixveh`, `/flip`, `/disarm`, `/ban` (do 7 dana), `/warn`, `/ajail` (do 24 h), vozila: `/getveh [id]`, `/respawnveh [id]`, `/resettablice [id]` (uvredljiva tablica postaje nasumična, besplatno), igrac: `/proveriinv`, `/kazne [igrac]`, `/odblokirajpin` |
| 3 | Visi admin | `/setskin`, `/setint` (enterijer), `/setvw` (virtuelni svet), `/veh [model]`, `/dveh`, `/dveh sva`, `/ann` (tekst na sredini ekrana), `/settime` (`-1` vraća stvarno vreme), `/setweather`, `/gotopos`, `/mypos`, `/ban` (i trajni), `/oban`, `/banip`, `/unban`, `/unwarn`, `/resetvozila [sekunde]` (odbrojavanje, pa sva prazna vozila na parking), `/setgorivo [id] [litara]`, `/atune` i na ličnim vozilima |
| 4 | Glavni admin | `/makehelper` (svi nivoi), `/setlevel`, `/setrespect` (manje od potrebnog za sledeći nivo), `/setglad` (traka gladi 0-100), `/setname`, `/pmspy`; vrata: `/napraviulaz`, `/izmeniulaz` (ime, ulaz, izlaz, ikona, strelica sa slikama, zakljucaj, svet), `/obrisiulaz`, `/ulazi`; mesta: `/napravimesto` (banka, bankomat, opstina, prodavnica, odeca, pumpa, pregled, policija; bez parametra lista), `/obrisimesto`, `/mesta`; GPS: `/dodajgps`, `/obrisigps`; vozila: `/dajvozilo [igrac] [model]` (do koraka 3.2, za testiranje), `/obrisivozilo`; kamere: `/napravikameru [ogranicenje]`, `/obrisikameru`, `/kamere`; auto pijaca: `/pijaca` (meni); izmene: `/izmenivozilo`, `/izmenimesto`, `/izmenigps`, `/izmenikameru`; igrac: `/dajpredmet`, `/uzmipredmet`, `/setlicna`, `/ponistikazne`; `/izmeniulaz [id]` i `/dodajgps` bez ostalih parametara otvaraju meni |
| 5 | Menadzment | `/makeadmin` (do nivoa 4), `/gmx` (restart servera), `/happyhour [broj plata 1-12]` (naredne plate donose duplu platu i respect) |
| 6 | Vlasnik | `/makeadmin` (do nivoa 5), `/givemoney`, `/dozvoliime`, `/zabraniime`, `/setvip` (vidi VIP.md), `/givebank` (novac na racun), `/setvip [igrac]` bez ostalog otvara meni |

### Kanali (`/kanali`)
- Svako bira koje chatove i obaveštenja prati: globalni OOC, PM, newbie, VIP chat, helper chat, pitanja, admin chat, reportovi, anti-cheat upozorenja, ista IP adresa i PM spy (samo admin 4+, podrazumevano isključen).
- Spisak u `/kanali` pokazuje šta je uključeno, a klik menja stanje. Podešavanje se čuva i posle izlaska.
- **Na dužnosti** admin ne može da isključi reportove i anti-cheat upozorenja, a helper i admin ne mogu da isključe pitanja.
- Ko je isključio kanal, ne može ni da piše u njega dok ga ne uključi.

### Kazne
- Svaka kazna (kick, ban, warn, mute, admin zatvor) se objavljuje svim igračima crvenom porukom i upisuje u `admin_logs`.
- **Ban** pamti ime, IP adresu i serijski broj klijenta. Pri ulasku igrač je blokiran ako se poklapa bilo šta od toga. Vlasnik se ne može banovati. Lokalna adresa `127.0.0.1` se ne pamti i ne proverava, jer je na test računaru zajednička svim klijentima.
- **Ban bez igrača:** `/oban` banuje nalog koji nije online, a `/banip` samo IP adresu. `/unban` prima ime naloga ili IP adresu.
- **Upozorenja:** 3 aktivna upozorenja = automatski ban 3 dana, a upozorenja se tada brišu. Spisak upozorenja daje `/warns`.
- **Mute, newbie mute i admin zatvor** se čuvaju u bazi, pa se ne mogu izbeći izlaskom sa servera. Mute blokira govor i sve chat komande. Newbie mute blokira samo `/n`.
- **Admin zatvor** je ćelija u LSPD enterijeru, u posebnom virtuelnom svetu. Vreme teče samo dok je igrač u igri. Ko pobegne, vraća se nazad. Posle kazne igrač se pojavljuje na aerodromu.
- Pri ulasku sa iste IP adrese kao neko ko je već na serveru, admini dobijaju upozorenje.
- `/pmspy` (admin 4+) prikazuje kopije svih privatnih poruka.

### Anti-cheat
- **Automatski kick:** oružje koje server nije dao i jetpack.
- **Upozorenje adminima** („[AC] Ime [id]: sumnja na …“): HP iznad 100, armor koji server nije dao, teleport, prevelika brzina peške ili u vozilu. Admin proverava sa `/spec`, gde vidi HP, armor, oružje, brzinu i ping igrača.
- **Novac** je samo na serveru: ako ga cheat promeni na ekranu, server ga vraća bez upozorenja.
- **Flood:** više od 3 poruke u 3 sekunde se blokira uz upozorenje, a posle 3 upozorenja u minuti dolazi automatski mute od 2 minuta. Ista poruka više od 2 puta zaredom se ne šalje. Komande imaju limit od 5 u 3 sekunde.
- Kodovi boja (`{FF0000}`) se uklanjaju iz teksta igrača.
- Sva upozorenja i kickovi se čuvaju u tabeli `ac_logs`, a `/aclog [id]` prikazuje poslednjih 15.

---

## 🇬🇧 English

The full list of who can do what. Every new system adds its commands here.
A higher level always has every command of the lower levels.

### Rules
- Two separate ladders: **Helper 1–3** and **Admin 1–5 + Owner (6)**. A player is on one of them, not both.
- **Admin commands work only on duty** (`/aduty`), except admin chat `/a`. The owner is exempt.
- **Helper commands work on helper duty** (`/hduty`), except helper chat `/hc`. Every admin can use helper commands.
- **Helpers do not punish.** They answer questions and report violations to admins.
- A player is **either an admin or a helper**, not both. Promoting a helper to admin removes the helper rank automatically. An admin cannot be made a helper until the admin rank is removed.
- Admin vehicles (`/veh`) can only be driven by admins, others may ride as passengers. They are deleted when the admin who spawned them leaves.
- Without permission a player gets "Nemate dozvolu za ovu komandu." An admin off duty is told to turn on `/aduty`.
- `/ahelp` shows the commands up to your own level.
- An admin cannot punish, freeze or rename an admin of the **same or higher** level.
- Player names are always white. On duty a tag floats above the name: black **[ADMIN]** (owner **[VLASNIK]**), yellow **[HELPER]**. Players do not see the tag above their own head, others do.
- An admin on duty wears the staff skin (217 male, 211 female). Going off duty restores their own skin. Helpers keep their own skin.
- Every admin action is written to the `admin_logs` table (who, what, to whom, details, time).

### Commands for every player
| Command | Description |
|---------|-------------|
| `/report [id] [reason]` | report a player to admins (once per 60 s) |
| `/ask [question]` | question for helpers (once per 30 s). With no helper on duty the whole team sees it |
| `/n [text]` | newbie chat, for players up to level 3 and the team. Players post once per 30 s |
| `/tognewbie` | turn newbie chat off/on for yourself |
| `/admins`, `/helpers` | who from the team is online and who is on duty |

### Helpers
| Level | Name | Commands |
|-------|------|----------|
| 1 | Helper | `/hduty`, `/hc`, `/odg [id] [answer]`, `/hhelp` |
| 2 | Senior helper | `/goto`, `/nmute [id] [min]` and `/unnmute` (newbie chat ban) |
| 3 | Head helper | `/makehelper` (sets and removes helper levels 1–2) |

### Admins
| Level | Name | Commands |
|-------|------|----------|
| 1 | Trial admin | `/aduty`, `/a`, `/ahelp`, `/reports`, `/ar`, `/rr`, `/goto`, `/gethere`, `/spec`, `/specoff`, `/check`, `/freeze`, `/unfreeze`, `/slap`, `/kick`, `/mute`, `/unmute`, `/ajail` (up to 30 min), `/unjail`, `/warns`, `/aclog`, `/kanali`, vehicles: `/vinfo [id]`, `/gotoveh [id]`, `/atune` (admin vehicles), `/aigrac [player]` (a menu with every option for a player, by level), `/izmenivozilo [id]` (menu, changes from level 4) |
| 2 | Admin | `/sethp`, `/setarmour`, `/tp` (GPS locations, generic spots until there are any), `/gotoulaz`, `/ooc` (global OOC), `/cc` (clears everyone's chat), `/ao` (admin notice in chat), `/fixveh`, `/flip`, `/disarm`, `/ban` (up to 7 days), `/warn`, `/ajail` (up to 24 h), vehicles: `/getveh [id]`, `/respawnveh [id]`, `/resettablice [id]` (an offensive plate becomes a random one, free), player: `/proveriinv`, `/kazne [player]`, `/odblokirajpin` |
| 3 | Senior admin | `/setskin`, `/setint` (interior), `/setvw` (virtual world), `/veh [model]`, `/dveh`, `/dveh sva`, `/ann` (text in the middle of the screen), `/settime` (`-1` returns to real time), `/setweather`, `/gotopos`, `/mypos`, `/ban` (also permanent), `/oban`, `/banip`, `/unban`, `/unwarn`, `/resetvozila [seconds]` (a countdown, then every empty vehicle back to its parking spot), `/setgorivo [id] [litres]`, `/atune` also on personal vehicles |
| 4 | Head admin | `/makehelper` (all levels), `/setlevel`, `/setrespect` (below what the next level needs), `/setglad` (hunger bar 0-100), `/setname`, `/pmspy`; doors: `/napraviulaz`, `/izmeniulaz` (name, outside, inside, icon, arrow with pictures, lock, world), `/obrisiulaz`, `/ulazi`; points: `/napravimesto` (bank, ATM, city hall, shop, clothing shop, gas pump, inspection, police; a list without a parameter), `/obrisimesto`, `/mesta`; GPS: `/dodajgps`, `/obrisigps`; vehicles: `/dajvozilo [player] [model]` (until step 3.2, for testing), `/obrisivozilo`; cameras: `/napravikameru [limit]`, `/obrisikameru`, `/kamere`; car market: `/pijaca` (menu); edits: `/izmenivozilo`, `/izmenimesto`, `/izmenigps`, `/izmenikameru`; player: `/dajpredmet`, `/uzmipredmet`, `/setlicna`, `/ponistikazne`; `/izmeniulaz [id]` and `/dodajgps` without other parameters open a menu |
| 5 | Management | `/makeadmin` (up to level 4), `/gmx` (server restart), `/happyhour [paydays 1-12]` (the next paydays give double pay and respect) |
| 6 | Owner | `/makeadmin` (up to level 5), `/givemoney`, `/dozvoliime`, `/zabraniime`, `/setvip` (see VIP.md), `/givebank` (money to the account), `/setvip [player]` alone opens a menu |

### Channels (`/kanali`)
- Everyone picks which chats and notifications they follow: global OOC, PM, newbie, VIP chat, helper chat, questions, admin chat, reports, anti-cheat alerts, shared IP and PM spy (admin 4+ only, off by default).
- The `/kanali` list shows what is on, and a click toggles it. The setting is kept after leaving.
- **On duty** an admin cannot switch off reports and anti-cheat alerts, and helpers and admins cannot switch off questions.
- Whoever switched a channel off cannot write in it until they switch it back on.

### Punishments
- Every punishment (kick, ban, warn, mute, admin jail) is announced to all players in red and written to `admin_logs`.
- A **ban** stores the name, IP address and client serial. On connect the player is blocked if any of them match. The owner cannot be banned. The loopback address `127.0.0.1` is never stored or checked, since all clients on the test machine share it.
- **Bans without the player:** `/oban` bans an offline account, `/banip` only an IP address. `/unban` takes an account name or an IP address.
- **Warnings:** 3 active warnings = automatic 3-day ban, and the warnings are cleared. `/warns` lists them.
- **Mute, newbie mute and admin jail** are stored in the database, so leaving the server does not avoid them. Mute blocks talking and every chat command. Newbie mute blocks only `/n`.
- **Admin jail** is a cell in the LSPD interior, in its own virtual world. Time only runs while the player is in-game. Escapees are put back. After serving, the player appears at the airport.
- When someone connects from the same IP as a player already online, admins get a warning.
- `/pmspy` (admin 4+) shows copies of every private message.

### Anti-cheat
- **Automatic kick:** a weapon the server never gave, and jetpack.
- **Admin alert** ("[AC] Name [id]: suspected …"): HP above 100, armour the server never gave, teleport, too much speed on foot or in a vehicle. Admins check with `/spec`, which shows the player's HP, armour, weapon, speed and ping.
- **Money** lives on the server only: if a cheat changes the on-screen value, the server puts it back silently.
- **Flood:** more than 3 messages in 3 seconds is blocked with a warning, and 3 warnings within a minute give an automatic 2-minute mute. The same message more than twice in a row is not sent. Commands are limited to 5 per 3 seconds.
- Colour codes (`{FF0000}`) are stripped from player text.
- Every alert and kick is stored in `ac_logs`; `/aclog [id]` shows the last 15.
