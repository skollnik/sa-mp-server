# Katalog sistema / Feature catalog

Spisak sistema koje naš server treba da ima.
Svaki sistem ima oznaku (npr. `ACC-1`), a `PLAN.md` određuje kojim redom se rade. Cene i iznosi novca su u [ECONOMY.md](ECONOMY.md).

A list of the systems our server should have.
Every system has an ID (e.g. `ACC-1`), and `PLAN.md` sets the order in which they get built. Prices and money amounts are in [ECONOMY.md](ECONOMY.md).

---

## 🇷🇸 Srpski

### ACC — Nalog i karakter
- **ACC-1 Registracija / login:** lozinka hešovana bcrypt-om, ograničen broj pokušaja, zaključavanje posle X promašaja.
- **ACC-2 Provera imena:** jedan nalog = jedan karakter, a ime je obavezno u formatu `Ime_Prezime` (samo slova, svaki deo počinje velikim slovom, zabranjena poznata imena). Izuzetak: vlasnik i igrači kojima vlasnik dodeli izuzetak mogu imati kratko ime (npr. `skollnik`).
- **ACC-3 Pravila servera:** prikazuju se pri registraciji i preko `/pravila`, bez kviza.
- **ACC-4 Kreiranje karaktera:** pol, godine, poreklo, izbor skina, kratak opis izgleda (`/examine`).
- **ACC-5 Tutorijal / prvi spawn:** kratak obilazak (opština, banka, poslovi) i startni novac.
- **ACC-6 Čuvanje podataka:** pri izlasku i periodično (autosave). Čuva se pozicija, enterijer, virtuelni svet, HP i armor.
- **ACC-7 Payday:** svakih sat vremena provedenog u igri isplaćuje platu, kamatu i naplaćuje poreze/račune.
- **ACC-8 Statistike / lična karta** (`/stats`): ime, godine, novac, banka, posao, organizacija, sati igre.
- **ACC-9 Podešavanja igrača:** `/podesavanja` sa delovima HUD-a (sat i datum, kvart, traka gladi), melodijom telefona i kanalima; sve se pamti posle izlaska
- **ACC-10 Nivoi:** respect poeni se dobijaju na payday, a sa njima se prelazi na sledeći nivo. Nivo otključava imovinu, vozila i dozvole (vidi `ECONOMY.md`).

### CHAT — Komunikacija
- **CHAT-1 IC chat po udaljenosti:** što je igrač dalje, tekst je bleđi.
- **CHAT-2 RP komande:** `/me`, `/do`, `/ame` (tekst iznad glave), `/s` (vika), `/l` (tiho), `/w` (šapat), `/cw` (u vozilu).
- **CHAT-3 OOC:** `/b` (lokalni OOC), `/o` (globalni OOC, admini ga mogu isključiti), `/pm`.
- **CHAT-4 Frakcijski chatovi:** `/f` (OOC frakcija), `/r` (radio), `/d` (među službama), `/m` (megafon).
- **CHAT-5 Oglasi:** `/ad` (plaćeni, preko novinara ili oglasne službe).
- **CHAT-6 Akcenti** (`/accent`), koji se dodaju ispred IC poruka.

### ADM — Administracija i podrška
- **ADM-1 Staff nivoi:** Helper, Admin 1–5, Vlasnik. Postoji admin duty (`/aduty`).
- **ADM-2 Kanali za pomoć:** `/report` (prijava igrača) i `/ask` (pitanje za helpere).
- **ADM-3 Kazne:** kick, ban, privremeni ban, ban po IP-u, warn sistem, admin zatvor (`/ajail`), mute.
- **ADM-4 Alati:** `/spec`, `/goto`, `/gethere`, freeze, slap, postavljanje statistika, spawn vozila, teleport lokacije.
- **ADM-5 Alati za kreiranje u igri:** kuće, firme, ulazi/izlazi, frakcijska vozila, kapije i pickup-i se prave iz igre i čuvaju u bazi.
- **ADM-7 Imena van formata:** samo vlasnik može da dodeli izuzetak od `Ime_Prezime` formata.
- **ADM-8 Auto pijaca (event):** admin povremeno otvara auto pijacu na nekoj lokaciji, postavlja vozila sa cenom i količinom, a igrači kupuju dok je pijaca otvorena. Stalni auto salon **ne postoji**, da vozila ostanu retka.
- **ADM-6 Logovi:** sve admin akcije se upisuju u bazu.

### ECO — Ekonomija
- **ECO-1 Novac:** keš u džepu i račun u banci. Novac je samo serverski, pa anti-cheat ne može da se zaobiđe.
- **ECO-2 Banka:** filijale i bankomati, uplata, isplata, transfer i štedni račun.
- **ECO-3 Porezi** na imovinu i vozila. Budžet države puni vlada.
- **ECO-4 Logovi transakcija**, radi borbe protiv duplikacije novca.

### WORLD — Svet
- **WORLD-1 Dinamički ulazi/izlazi** (enterijeri) sa pickup-om i 3D labelom, plus enter/exit komande.
- **WORLD-2 Vreme i vremenske prilike:** sinhronizovano sa stvarnim vremenom ili ubrzano.
- **WORLD-3 GPS / lokacije** (`/gps`): opština, banka, bolnica, policija, poslovi.
- **WORLD-4 Opština (City Hall):** uzimanje posla, lična karta, plaćanje kazni.
- **WORLD-5 Dinamičke kapije i vrata** (frakcijske, privatne).
- **WORLD-6 Mapiranja i custom enterijeri** (streamer objekti).
- **WORLD-7 HUD i UI:** brzinomer, prikaz goriva, notifikacije, dijalozi, 3D labele.

### LIC — Dozvole
- **LIC-1 Vozačke dozvole:** auto škola sa teorijskim testom i praktičnom vožnjom po checkpoint-ima. Postoje kategorije auto, motor, kamion.
- **LIC-2 Ostale dozvole:** za letenje, brod, ribolov.
- **LIC-3 Dozvola za oružje:** izdaje je policija.

### VEH — Vozila
- **VEH-1 Lična vozila:** kupuju se samo na auto pijaci (ADM-8) ili od drugog igrača. Postoji maksimalan broj po igraču, a vozila se čuvaju u bazi.
- **VEH-2 Tablice / registracija:** tablica može da se proveri u MDC-u.
- **VEH-3 Ključevi:** zaključavanje/otključavanje, davanje ključeva drugima.
- **VEH-4 Ručno upravljanje:** motor, svetla, hauba i gepek.
- **VEH-5 Gorivo:** potrošnja zavisi od tipa vozila. Toči se na benzinskim pumpama, a postoji i kanister.
- **VEH-6 Oštećenja i popravka:** preko mehaničara. Kilometraža.
- **VEH-7 Parkiranje** (`/park`): vozilo se spawnuje na mestu gde je parkirano.
- **VEH-8 Gepek / pregradak:** inventar u vozilu.
- **VEH-9 Rent-a-car i javna vozila** za poslove.
- **VEH-10 Frakcijska vozila:** dostupna samo članovima, sa sirenama.
- **VEH-11 Impound (pauk služba):** odneseno vozilo se otkupljuje.
- **VEH-12 Alarm i krađa:** obijanje brave, vruća žica, prodaja na otpadu (chop shop).

### PROP — Imovina
- **PROP-1 Kuće:** kupovina, prodaja državi ili igraču, zaključavanje, enterijeri različite veličine.
- **PROP-2 Iznajmljivanje:** vlasnik izdaje sobe, a stanar plaća kiriju na payday.
- **PROP-3 Kućni sef i ormar:** novac, oružje, droga, predmeti.
- **PROP-4 Nameštaj:** postavljanje objekata u kući.
- **PROP-5 Garaže** vezane za kuću.
- **PROP-6 Firme (biznisi):** 24/7, prodavnica oružja, benzinska, restoran/brza hrana, bar/klub, prodavnica odeće, mehaničarska radionica. Salona vozila nema (vidi ADM-8).
- **PROP-7 Ekonomija firmi:** zalihe/produkti, cene koje postavlja vlasnik, kasa firme, zaposleni.
- **PROP-8 Dostava produkata** firmama (posao kamiondžije), čime se pravi lanac ekonomije.

### INV — Inventar i predmeti
- **INV-1 Inventar igrača:** slotovi i količine, `/inv`, davanje, bacanje i podizanje predmeta sa zemlje.
- **INV-2 Predmeti:** telefon, radio, hrana, piće, cigarete, alat, kanister, maska, uže, povez za oči, droga, materijali.
- **INV-3 Oružje kao predmet:** ima serijski broj i municiju, a može da se stavi u futrolu ili gepek.
- **INV-4 Inventar kontejnera:** kuća, vozilo, firma i frakcija.

### PHONE — Telefon
- **PHONE-1 Broj telefona:** telefon i SIM kartica se kupuju odvojeno, broj je na nalogu; pozivi (`/pozovi`, `/javi`, `/prekini`), SMS i javni imenik se plaćaju kreditom; lični kontakti; meni `/telefon`; broj se gasi posle 6 meseci neaktivnosti.
- **PHONE-2 Hitni brojevi:** 911 (policija/hitna/vatrogasci), taksi, mehaničar.
- **PHONE-3 Govornice.**
- **PHONE-4 Prisluškivanje / praćenje lokacije** (za policiju, sa nalogom).

### HEALTH — Zdravlje, povrede i smrt
- **HEALTH-1 Stanje ranjenosti (brutally wounded):** umesto smrti igrač pada u animaciju i čeka hitnu pomoć. Može da prihvati smrt (`/acceptdeath`).
- **HEALTH-2 Log povreda** (`/damages`): koje oružje i koji deo tela.
- **HEALTH-3 Bolnica:** respawn posle smrti uz naplatu lečenja.
- **HEALTH-4 PK / CK pravila:** zaboravljanje događaja (PK) i trajna smrt karaktera (CK) uz odobrenje admina.
- **HEALTH-5 Glad:** traka gladi polako pada, a na nuli igrač polako gubi HP. Hrana puni glad i vraća malo HP-a, ali ne može da se jede kad je igrač sit, pa se hranom ne vraća pun HP. Prva pomoć iz 24/7 vraća deo HP-a (ne u borbi ni dok je igrač ranjen); pun HP vraćaju bolnica i hitna. Žeđi nema.
- **HEALTH-6 Droga i alkohol** imaju vizuelne efekte (vremenske prilike, kretanje kamere).

### FAC — Frakcije (okvir)
- **FAC-1 Dinamičke frakcije:** pravi ih admin iz igre. Tip određuje mogućnosti (policija, medicina, vlada, mediji, taksi, vojska, banda, mafija...).
- **FAC-2 Rankovi:** do 15, sa imenima i dozvolama po ranku.
- **FAC-3 Liderske komande:** invite, uninvite, giverank, upravljanje budžetom.
- **FAC-4 Sedište (HQ):** ulaz, ormarić za opremu, duty, skinovi po ranku.
- **FAC-5 Frakcijska vozila i plata po ranku** (na payday).
- **FAC-6 Sef i budžet frakcije.**

### LAW — Policija (LSPD), saobraćajna, SWAT
- **LAW-1 Duty i oprema:** uniforma, oružje, pancir, radio.
- **LAW-2 Hapšenje:** lisice (`/cuff`), vođenje (`/drag`), stavljanje u vozilo, hapšenje (`/arrest`), zatvor sa vremenom kazne i ćelijama.
- **LAW-3 Potrage:** `/su` (dodavanje zločina), wanted lista, BOLO.
- **LAW-4 MDC kompjuter:** dosije osobe, krivična istorija, provera tablica, serijski brojevi oružja, nalozi.
- **LAW-5 Kazne:** `/ticket`, saobraćajne kazne, oduzimanje dozvola.
- **LAW-6 Pretres** (`/frisk`) i oduzimanje predmeta.
- **LAW-7 Saobraćajna policija:** radari, merenje brzine, alkotest, pauk služba.
- **LAW-8 Oprema na terenu:** barikade, ekseri (spike strips), čunjevi, sirene, svetla.
- **LAW-9 Dojava pucnjave:** dispečer dobija obaveštenje sa zakašnjenjem.
- **LAW-10 SWAT / specijalna jedinica:** teže naoružanje, spuštanje iz helikoptera užetom.

### EMS — Hitna pomoć i vatrogasci (LSFD)
- **EMS-1 Lečenje i oživljavanje** ranjenih igrača, prevoz u bolnicu.
- **EMS-2 Pozivi preko 911** i markeri na mapi.
- **EMS-3 Požari** (dinamički, gase se crevom/vozilom), opciono.

### GOV — Vlada
- **GOV-1 Gradonačelnik i vlada:** postavljanje poreza, budžet, dozvole.
- **GOV-2 Sud / advokati** (RP, bez posebnog skripta ili sa osnovnim komandama).

### NEWS — Novinari (SAN News)
- **NEWS-1** `/news` (vesti za ceo server), intervjui uživo (`/live`), oglasi.

### TAXI — Taksi
- **TAXI-1** Taksimetar (`/fare`) sa automatskom naplatom, pozivi preko telefona, dispečer.

### ARMY — Vojska
- **ARMY-1** Vojna baza, kontrola pristupa, transport materijala/oružja (konvoj), podrška policiji u vanrednom stanju.

### JOB — Civilni poslovi
- **JOB-1 Okvir za poslove:** posao se uzima u opštini ili na lokaciji. Postoje checkpoint misije, plata po turi i skill nivo posla.
- **JOB-2 Kamiondžija:** dostava produkata firmama.
- **JOB-3 Mehaničar:** popravka, farbanje, tuning.
- **JOB-4 Ostali poslovi:** dostavljač pice, smetlar, autobuski vozač, čistač ulica, ribar, farmer, drvoseča, rudar, kurir.
- **JOB-5 Taksi** (kao posao ili frakcija, vidi otvorena pitanja).

### CRIME — Kriminal
- **CRIME-1 Bande:** teritorije (gang zone), zauzimanje teritorija, prihod sa teritorija, grafiti.
- **CRIME-2 Mafije:** materijali, izrada i prodaja oružja, šverc, reketiranje firmi.
- **CRIME-3 Droga:** uzgoj (marihuana), kuvanje, prodaja, efekti i zavisnost.
- **CRIME-4 Oružje:** materijali → izrada (crafting) → prodaja, ilegalni dileri.
- **CRIME-5 Pljačke:** prodavnice, kuće (provala), banka (veliki event), uz minimalan broj policajaca na dužnosti.
- **CRIME-6 Otmice:** vezivanje (`/tie`), povez preko očiju, stavljanje u gepek.
- **CRIME-7 Maska:** sakriva ime ("Stranac_1234").
- **CRIME-8 Krađa vozila** i chop shop.
- **CRIME-9 Crno tržište.**

### AC — Bezbednost i anti-cheat
- **AC-1** Novac, oružje, HP i armor su isključivo serverski. Ono što klijent prijavi se ignoriše.
- **AC-2 Detekcija:** teleport, speedhack, fly/airbreak, spawn vozila, jetpack.
- **AC-3 Zaštita dijaloga:** provera ID-a dijaloga.
- **AC-4 Ograničenje komandi i chata** (flood zaštita).

### LOG — Logovanje
- **LOG-1** Novac, imovina, admin akcije, konekcije (IP, serijski) i smrti, u bazi.

### WEB — Web / UCP (kasnije)
- **WEB-1** Web panel: registracija, pregled karaktera, aplikacije za frakcije, forum.

---

## 🇬🇧 English

### ACC — Account and character
- **ACC-1 Register / login:** password hashed with bcrypt, limited attempts, lockout after X failures.
- **ACC-2 Name validation:** one account = one character, and the name must be `Firstname_Lastname` (letters only, each part capitalized, famous names forbidden). Exception: the owner and players the owner grants an exemption can have a short name (e.g. `skollnik`).
- **ACC-3 Server rules:** shown at registration and via `/pravila`, no quiz.
- **ACC-4 Character creation:** gender, age, origin, skin selection, short appearance description (`/examine`).
- **ACC-5 Tutorial / first spawn:** short tour (city hall, bank, jobs) and starting money.
- **ACC-6 Data saving:** on quit and periodically (autosave). Saves position, interior, virtual world, HP and armour.
- **ACC-7 Payday:** every hour of playtime it pays salary and interest and charges taxes/bills.
- **ACC-8 Stats / ID card** (`/stats`): name, age, money, bank, job, organization, hours played.
- **ACC-9 Player settings:** `/podesavanja` with HUD parts (clock and date, district, hunger bar), the phone ringtone and channels; everything is remembered after leaving
- **ACC-10 Levels:** respect points are earned on payday and used to reach the next level. Level unlocks property, vehicles and licenses (see `ECONOMY.md`).

### CHAT — Communication
- **CHAT-1 Proximity IC chat:** the farther the player, the fainter the text.
- **CHAT-2 RP commands:** `/me`, `/do`, `/ame` (text above head), `/s` (shout), `/l` (low), `/w` (whisper), `/cw` (in vehicle).
- **CHAT-3 OOC:** `/b` (local OOC), `/o` (global OOC, admins can toggle it), `/pm`.
- **CHAT-4 Faction chats:** `/f` (faction OOC), `/r` (radio), `/d` (inter-department), `/m` (megaphone).
- **CHAT-5 Ads:** `/ad` (paid, via news faction or ad service).
- **CHAT-6 Accents** (`/accent`), prefixed to IC messages.

### ADM — Administration and support
- **ADM-1 Staff levels:** Helper, Admin 1–5, Owner. There is admin duty (`/aduty`).
- **ADM-2 Help channels:** `/report` (report a player) and `/ask` (question for helpers).
- **ADM-3 Punishments:** kick, ban, temp ban, IP ban, warn system, admin jail (`/ajail`), mute.
- **ADM-4 Tools:** `/spec`, `/goto`, `/gethere`, freeze, slap, set stats, vehicle spawn, teleport locations.
- **ADM-5 In-game creation tools:** houses, businesses, entrances/exits, faction vehicles, gates and pickups are created in-game and saved to the DB.
- **ADM-7 Non-format names:** only the owner can grant an exemption from the `Firstname_Lastname` format.
- **ADM-8 Car market (event):** an admin occasionally opens a car market at some location, lists vehicles with price and quantity, and players buy while the market is open. There is **no** permanent dealership, so vehicles stay rare.
- **ADM-6 Logs:** every admin action is written to the DB.

### ECO — Economy
- **ECO-1 Money:** cash in pocket and bank account. Money is server-side only, so anti-cheat cannot be bypassed.
- **ECO-2 Bank:** branches and ATMs, deposit, withdraw, transfer and savings.
- **ECO-3 Taxes** on property and vehicles. The government fills the state budget.
- **ECO-4 Transaction logs**, to fight money duplication.

### WORLD — World
- **WORLD-1 Dynamic entrances/exits** (interiors) with a pickup and 3D label, plus enter/exit commands.
- **WORLD-2 Time and weather:** synced to real time or accelerated.
- **WORLD-3 GPS / locations** (`/gps`): city hall, bank, hospital, police, jobs.
- **WORLD-4 City Hall:** take a job, ID card, pay fines.
- **WORLD-5 Dynamic gates and doors** (faction, private).
- **WORLD-6 Mappings and custom interiors** (streamer objects).
- **WORLD-7 HUD and UI:** speedometer, fuel display, notifications, dialogs, 3D labels.

### LIC — Licenses
- **LIC-1 Driving licenses:** driving school with a theory test and a practical checkpoint drive. Categories are car, motorbike and truck.
- **LIC-2 Other licenses:** flying, boat, fishing.
- **LIC-3 Weapon license:** issued by the police.

### VEH — Vehicles
- **VEH-1 Personal vehicles:** bought only at a car market (ADM-8) or from another player. There is a per-player cap, and vehicles are saved to the DB.
- **VEH-2 Plates / registration:** a plate can be checked in the MDC.
- **VEH-3 Keys:** lock/unlock, share keys with others.
- **VEH-4 Manual controls:** engine, lights, hood and trunk.
- **VEH-5 Fuel:** consumption depends on vehicle type. Refuel at gas stations, or use a jerry can.
- **VEH-6 Damage and repair:** via mechanics. Mileage.
- **VEH-7 Parking** (`/park`): the vehicle spawns where it was parked.
- **VEH-8 Trunk / glovebox:** in-vehicle inventory.
- **VEH-9 Rentals and public job vehicles.**
- **VEH-10 Faction vehicles:** members only, with sirens.
- **VEH-11 Impound (tow service):** an impounded vehicle has to be bought back.
- **VEH-12 Alarm and theft:** lockpicking, hotwiring, selling at a chop shop.

### PROP — Property
- **PROP-1 Houses:** buy, sell to the state or a player, lock, interiors of different sizes.
- **PROP-2 Renting:** the owner rents out rooms, and the tenant pays rent on payday.
- **PROP-3 House safe and wardrobe:** money, weapons, drugs, items.
- **PROP-4 Furniture:** placing objects inside the house.
- **PROP-5 Garages** tied to a house.
- **PROP-6 Businesses:** 24/7, gun shop, gas station, restaurant/fast food, bar/club, clothing store, mechanic shop. There is no dealership (see ADM-8).
- **PROP-7 Business economy:** stock/products, owner-set prices, business till, employees.
- **PROP-8 Product delivery** to businesses (trucker job), which creates an economy chain.

### INV — Inventory and items
- **INV-1 Player inventory:** slots and quantities, `/inv`, give, drop and pick up items from the ground.
- **INV-2 Items:** phone, radio, food, drinks, cigarettes, tools, jerry can, mask, rope, blindfold, drugs, materials.
- **INV-3 Weapons as items:** they have a serial number and ammo, and can be holstered or put in a trunk.
- **INV-4 Container inventories:** house, vehicle, business and faction.

### PHONE — Phone
- **PHONE-1 Phone number:** the phone and the SIM card are bought separately, the number belongs to the account; calls (`/pozovi`, `/javi`, `/prekini`), SMS and the public directory are paid with credit; personal contacts; a `/telefon` menu; the number is freed after 6 months of inactivity.
- **PHONE-2 Emergency numbers:** 911 (police/EMS/fire), taxi, mechanic.
- **PHONE-3 Payphones.**
- **PHONE-4 Wiretap / location tracking** (police, with a warrant).

### HEALTH — Health, injuries and death
- **HEALTH-1 Brutally wounded state:** instead of dying the player falls into an animation and waits for EMS. They can accept death (`/acceptdeath`).
- **HEALTH-2 Damage log** (`/damages`): which weapon and which body part.
- **HEALTH-3 Hospital:** respawn after death with a treatment fee.
- **HEALTH-4 PK / CK rules:** forgetting the event (PK) and permanent character death (CK) with admin approval.
- **HEALTH-5 Hunger:** a hunger bar slowly drops, and at zero the player slowly loses HP. Food fills hunger and gives a little HP, but cannot be eaten when full, so food never restores full HP. A first aid kit from a 24/7 restores part of the HP (not in combat or while wounded); full HP comes from the hospital and EMS. There is no thirst.
- **HEALTH-6 Drugs and alcohol** have visual effects (weather, camera sway).

### FAC — Factions (framework)
- **FAC-1 Dynamic factions:** an admin creates them in-game. The type sets the capabilities (police, medical, government, media, taxi, army, gang, mafia...).
- **FAC-2 Ranks:** up to 15, with names and per-rank permissions.
- **FAC-3 Leader commands:** invite, uninvite, giverank, budget management.
- **FAC-4 Headquarters:** entrance, equipment locker, duty, skins per rank.
- **FAC-5 Faction vehicles and salary per rank** (on payday).
- **FAC-6 Faction safe and budget.**

### LAW — Police (LSPD), traffic police, SWAT
- **LAW-1 Duty and equipment:** uniform, weapons, armour, radio.
- **LAW-2 Arrests:** cuffs (`/cuff`), drag (`/drag`), place in vehicle, arrest (`/arrest`), jail with sentence time and cells.
- **LAW-3 Wanted:** `/su` (add charges), wanted list, BOLO.
- **LAW-4 MDC computer:** person record, criminal history, plate lookup, weapon serial numbers, warrants.
- **LAW-5 Fines:** `/ticket`, traffic fines, license revocation.
- **LAW-6 Frisk** (`/frisk`) and item confiscation.
- **LAW-7 Traffic police:** speed cameras, speed gun, breathalyzer, tow service.
- **LAW-8 Field equipment:** barricades, spike strips, cones, sirens, lights.
- **LAW-9 Shots-fired alerts:** dispatch gets a delayed notification.
- **LAW-10 SWAT / special unit:** heavier weapons, rappelling from a helicopter.

### EMS — Medical and fire department (LSFD)
- **EMS-1 Healing and reviving** wounded players, transport to hospital.
- **EMS-2 911 calls** with map markers.
- **EMS-3 Fires** (dynamic, extinguished with a hose or truck), optional.

### GOV — Government
- **GOV-1 Mayor and government:** sets taxes, budget, licenses.
- **GOV-2 Court / lawyers** (RP only, or with basic commands).

### NEWS — News (SAN News)
- **NEWS-1** `/news` (server-wide news), live interviews (`/live`), ads.

### TAXI — Taxi
- **TAXI-1** Taxi meter (`/fare`) with automatic billing, phone calls, dispatcher.

### ARMY — Army
- **ARMY-1** Military base, access control, material/weapon transport (convoy), police support in a state of emergency.

### JOB — Civilian jobs
- **JOB-1 Job framework:** a job is taken at City Hall or on location. There are checkpoint missions, pay per run and a job skill level.
- **JOB-2 Trucker:** delivers products to businesses.
- **JOB-3 Mechanic:** repair, paint, tuning.
- **JOB-4 Other jobs:** pizza delivery, garbage collector, bus driver, street sweeper, fisherman, farmer, lumberjack, miner, courier.
- **JOB-5 Taxi** (as a job or a faction, see open questions).

### CRIME — Crime
- **CRIME-1 Gangs:** turfs (gang zones), turf capture, turf income, graffiti.
- **CRIME-2 Mafias:** materials, weapon crafting and sales, smuggling, business extortion.
- **CRIME-3 Drugs:** growing (weed), cooking, selling, effects and addiction.
- **CRIME-4 Weapons:** materials → crafting → sale, illegal dealers.
- **CRIME-5 Robberies:** stores, houses (burglary), bank (big event), with a minimum number of cops on duty.
- **CRIME-6 Kidnapping:** tie up (`/tie`), blindfold, put in trunk.
- **CRIME-7 Mask:** hides the name ("Stranger_1234").
- **CRIME-8 Car theft** and chop shop.
- **CRIME-9 Black market.**

### AC — Security and anti-cheat
- **AC-1** Money, weapons, HP and armour are server-side only. What the client reports is ignored.
- **AC-2 Detection:** teleport, speedhack, fly/airbreak, vehicle spawn, jetpack.
- **AC-3 Dialog protection:** dialog ID validation.
- **AC-4 Command and chat rate limiting** (flood protection).

### LOG — Logging
- **LOG-1** Money, property, admin actions, connections (IP, serial) and deaths, in the DB.

### WEB — Web / UCP (later)
- **WEB-1** Web panel: registration, character overview, faction applications, forum.
