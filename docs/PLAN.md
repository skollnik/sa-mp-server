# Plan projekta / Project plan

## 🇷🇸 Srpski

**Trenutni status:** Faza 2 sa dopunama (glad, telefon sa SIM karticom i kreditom, strelice, nivo, happy hour) napisana, čeka test vlasnika u igri (ažurirano 2026-10-29)

Oznake sistema (npr. `ACC-1`) objašnjene su u [FEATURES.md](FEATURES.md), odluke su u [DECISIONS.md](DECISIONS.md), a cene u [ECONOMY.md](ECONOMY.md).
Redosled je postavljen tako da svaka faza stoji na prethodnoj. Ništa ne preskačemo dok prethodno ne radi i nije testirano u igri.

### Faza 0: Priprema
- [x] Čišćenje open.mp paketa
- [x] Osnovne odluke (baza, biblioteke, jezik, editor)
- [x] Git + GitHub repo, `.gitignore`
- [x] Instalacija XAMPP-a (MariaDB + phpMyAdmin), vidi [SETUP_MYSQL.md](SETUP_MYSQL.md)
- [x] Preuzimanje pluginova (mysql, sscanf, Pawn.CMD, streamer, bcrypt, crashdetect) i podešavanje `config.json`
- [x] VS Code: build task (Ctrl+Shift+B kompajlira gamemode) i ekstenzija za Pawn
- [x] Kostur gamemode-a (`roleplay.pwn` + `core/`) koji se kompajlira, pokreće i povezuje na bazu

### Faza 1: Jezgro
Cilj: igrač može da se registruje, napravi karakter, igra i izađe, a sve se čuva.
- [x] Sloj za bazu (tabele se prave automatski)
- [x] ACC-1, ACC-2: registracija i login
- [x] ACC-3: pravila servera (bez kviza)
- [x] ADM-7: izuzeci od Ime_Prezime formata (samo vlasnik)
- [x] ACC-4, ACC-5: kreiranje karaktera (pol, godine, nasumičan skin), prvi spawn na aerodromu
- [x] ACC-6: čuvanje podataka (izlazak + na 10 min)
- [x] CHAT-1, CHAT-2, CHAT-3: osnovni RP chat i `/pomoc`
- [x] Dopune logina: poslednja prijava, zaključavanje posle 5 pogrešnih lozinki, `/promenilozinku`, kamera na loginu
- [x] ADM-1, ADM-2, ADM-4, ADM-6 (deo 1): tim (Helper 1–3, Admin 1–6), duty, report, ask, newbie chat, alati, logovi. Vidi [ADMIN.md](ADMIN.md)
- [x] ADM-3 (deo 2): kazne (kick, ban, warn, mute, admin zatvor), provera bana, `/pmspy`
- [x] VIP sistem sa svim komandama i oznakom, vidi [VIP.md](VIP.md)
- [x] Kanali (`/kanali`) za uključivanje i isključivanje chatova i obaveštenja
- [x] AC-1, AC-3, AC-4: serverski novac/HP/armor/oružje, zaštita dijaloga (open.mp), flood zaštita, osnovna detekcija teleporta i brzine

### Faza 2: Svet i ekonomija
- [x] WORLD-6 (deo 1): prve mape, i to opština spolja i parking ispred nje, ograđena auto pijaca, banka spolja i enterijeri opštine, policije, zatvora i banke. Ulazi u enterijere se postavljaju u igri (`/napraviulaz`)
- [x] WORLD-1: ulazi i izlazi (F, `/udji`, `/izadji`), admin ih pravi u igri; „mesta“ za šaltere, bankomate i prodavnice
- [x] WORLD-2, WORLD-3, WORLD-7: stvarno vreme, GPS sa upravljačem checkpoint-a, HUD (sat, datum, kvart)
- [x] ECO-1, ECO-2, ECO-4: novac sa logovima, banka, bankomat, transfer, `/daj`
- [x] ACC-7, ACC-8, ACC-10: payday (sa AFK proverom i happy hour), `/stats`, nivoi
- [ ] ACC-9: podešavanja (npr. da se `/hud` pamti posle izlaska)
- [x] WORLD-4: opština (lična karta); prijava na posao čeka poslove
- [x] INV-1, INV-2: inventar sa slotovima, predmeti, prodavnice 24/7, `/vskin` sa slikama
- [x] PHONE-1, PHONE-2: telefon (pozivi, SMS, kontakti, 911 adminima)
- [x] Ispravke posle testa vlasnika (HUD, lična karta, ID-jevi, bankomat, cigareta, spam, spec)
- [x] HEALTH-5: glad sa trakom
- [x] PHONE-1 (nadogradnja): telefon i SIM kartica odvojeno, kredit i dopuna, meni `/telefon`, javni imenik
- [x] Strelice na vratima, nivo po uobičajenoj formuli, happy hour po broju plata

### Faza 3: Vozila i dozvole
- [ ] VEH-1 – VEH-9: lična vozila, ključevi, gorivo, parkiranje, gepek, rent
- [ ] ADM-8: auto pijaca (event)
- [ ] LIC-1, LIC-2: auto škola i dozvole
- [ ] JOB-3: mehaničar (VEH-6)

### Faza 4: Imovina
- [ ] PROP-1 – PROP-5: kuće, iznajmljivanje, sef, nameštaj, garaže
- [ ] PROP-6, PROP-7: firme
- [ ] JOB-1, JOB-2: okvir za poslove i kamiondžija (PROP-8)
- [ ] ADM-5: alati za kreiranje u igri
- [ ] ECO-3: porezi

### Faza 5: Frakcije, legalne
- [ ] FAC-1 – FAC-6: okvir za frakcije
- [ ] CHAT-4, CHAT-5: frakcijski chat i oglasi
- [ ] HEALTH-1 – HEALTH-4: povrede i smrt (potrebni su pre policije i hitne)
- [ ] LAW-1 – LAW-10: policija, saobraćajna, SWAT
- [ ] LIC-3: dozvola za oružje, INV-3: oružje kao predmet
- [ ] EMS-1 – EMS-3: hitna i vatrogasci
- [ ] VEH-10, VEH-11: frakcijska vozila, pauk
- [ ] GOV-1, GOV-2: vlada
- [ ] NEWS-1, TAXI-1, ARMY-1: novinari, taksi, vojska

### Faza 6: Kriminal
- [ ] CRIME-7: maska
- [ ] CRIME-1: bande i teritorije
- [ ] CRIME-2, CRIME-4: mafije, materijali, oružje
- [ ] CRIME-3: droga, HEALTH-6: efekti
- [ ] CRIME-5, CRIME-6, CRIME-8, CRIME-9, VEH-12: pljačke, otmice, krađa vozila, crno tržište
- [ ] PHONE-3, PHONE-4: govornice, prisluškivanje

### Faza 7: Ostali poslovi i dorade
- [ ] JOB-4: ostali civilni poslovi
- [ ] AC-2: napredni anti-cheat
- [ ] WORLD-5, WORLD-6: kapije, mapiranja
- [ ] CHAT-6 (ako ga odaberemo); HEALTH-5 je urađen u Fazi 2
- [ ] LOG-1: kompletiranje logova

### Faza 8: Kasnije
- [ ] WEB-1: web panel (UCP)
- [ ] Hosting / produkcija

### Otvorena pitanja (za vlasnika)
Pitanja koja moraju da se reše pre faze kojoj pripadaju:

| Faza | Pitanje |
|------|---------|
| 3 | Da li igrač može da proda vozilo državi (npr. za 50% cene) ili samo drugom igraču? |
| 5 | Taksi: frakcija ili posao koji svako može da radi? |

---

## 🇬🇧 English

**Current status:** Phase 2 with additions (hunger, SIM card phone with credit, arrows, levels, happy hour) written, waiting for the owner to test it in game (updated 2026-10-29)

System IDs (e.g. `ACC-1`) are explained in [FEATURES.md](FEATURES.md), decisions are in [DECISIONS.md](DECISIONS.md), and prices are in [ECONOMY.md](ECONOMY.md).
The order is set so that each phase builds on the previous one. Nothing gets skipped until the previous part works and is tested in-game.

### Phase 0: Preparation
- [x] Clean up the open.mp package
- [x] Core decisions (database, libraries, language, editor)
- [x] Git + GitHub repo, `.gitignore`
- [x] Install XAMPP (MariaDB + phpMyAdmin), see [SETUP_MYSQL.md](SETUP_MYSQL.md)
- [x] Download plugins (mysql, sscanf, Pawn.CMD, streamer, bcrypt, crashdetect) and configure `config.json`
- [x] VS Code: build task (Ctrl+Shift+B compiles the gamemode) and a Pawn extension
- [x] Gamemode skeleton (`roleplay.pwn` + `core/`) that compiles, runs and connects to the DB

### Phase 1: Core
Goal: a player can register, create a character, play and quit, and everything is saved.
- [x] Database layer (tables are created automatically)
- [x] ACC-1, ACC-2: register and login
- [x] ACC-3: server rules (no quiz)
- [x] ADM-7: exemptions from the Firstname_Lastname format (owner only)
- [x] ACC-4, ACC-5: character creation (gender, age, random skin), first spawn at the airport
- [x] ACC-6: data saving (on quit + every 10 min)
- [x] CHAT-1, CHAT-2, CHAT-3: basic RP chat and `/pomoc`
- [x] Login additions: last login, lockout after 5 wrong passwords, `/promenilozinku`, login camera
- [x] ADM-1, ADM-2, ADM-4, ADM-6 (part 1): staff (Helper 1–3, Admin 1–6), duty, reports, ask, newbie chat, tools, logs. See [ADMIN.md](ADMIN.md)
- [x] ADM-3 (part 2): punishments (kick, ban, warn, mute, admin jail), ban check, `/pmspy`
- [x] VIP system with all commands and the tag, see [VIP.md](VIP.md)
- [x] Channels (`/kanali`) for toggling chats and notifications
- [x] AC-1, AC-3, AC-4: server-side money/HP/armour/weapons, dialog protection (open.mp), flood protection, basic teleport and speed detection

### Phase 2: World and economy
- [x] WORLD-6 (part 1): first maps, namely the city hall exterior and the parking in front of it, the fenced car market, the bank exterior and the interiors of the city hall, the police, the prison and the bank. Doors into the interiors are set up in game (`/napraviulaz`)
- [x] WORLD-1: doors (F, `/udji`, `/izadji`), created by admins in game; "points" for counters, ATMs and shops
- [x] WORLD-2, WORLD-3, WORLD-7: real time, GPS with a checkpoint manager, HUD (clock, date, district)
- [x] ECO-1, ECO-2, ECO-4: money with logs, bank, ATM, transfers, `/daj`
- [x] ACC-7, ACC-8, ACC-10: payday (with AFK check and happy hour), `/stats`, levels
- [ ] ACC-9: settings (e.g. remembering `/hud` after leaving)
- [x] WORLD-4: city hall (ID card); job sign-up waits for jobs
- [x] INV-1, INV-2: slot inventory, items, 24/7 shops, `/vskin` with pictures
- [x] PHONE-1, PHONE-2: phone (calls, SMS, contacts, 911 to admins)
- [x] Fixes after the owner's test (HUD, ID card, IDs, ATM, cigarette, spam, spectating)
- [x] HEALTH-5: hunger with a bar
- [x] PHONE-1 (upgrade): phone and SIM card bought separately, credit and top-up, a `/telefon` menu, a public directory
- [x] Door arrows, levels by the usual formula, happy hour counted in paydays

### Phase 3: Vehicles and licenses
- [ ] VEH-1 – VEH-9: personal vehicles, keys, fuel, parking, trunk, rentals
- [ ] ADM-8: car market (event)
- [ ] LIC-1, LIC-2: driving school and licenses
- [ ] JOB-3: mechanic (VEH-6)

### Phase 4: Property
- [ ] PROP-1 – PROP-5: houses, renting, safe, furniture, garages
- [ ] PROP-6, PROP-7: businesses
- [ ] JOB-1, JOB-2: job framework and trucker (PROP-8)
- [ ] ADM-5: in-game creation tools
- [ ] ECO-3: taxes

### Phase 5: Legal factions
- [ ] FAC-1 – FAC-6: faction framework
- [ ] CHAT-4, CHAT-5: faction chat and ads
- [ ] HEALTH-1 – HEALTH-4: injuries and death (needed before police and EMS)
- [ ] LAW-1 – LAW-10: police, traffic police, SWAT
- [ ] LIC-3: weapon license, INV-3: weapons as items
- [ ] EMS-1 – EMS-3: medical and fire
- [ ] VEH-10, VEH-11: faction vehicles, impound
- [ ] GOV-1, GOV-2: government
- [ ] NEWS-1, TAXI-1, ARMY-1: news, taxi, army

### Phase 6: Crime
- [ ] CRIME-7: mask
- [ ] CRIME-1: gangs and turfs
- [ ] CRIME-2, CRIME-4: mafias, materials, weapons
- [ ] CRIME-3: drugs, HEALTH-6: effects
- [ ] CRIME-5, CRIME-6, CRIME-8, CRIME-9, VEH-12: robberies, kidnapping, car theft, black market
- [ ] PHONE-3, PHONE-4: payphones, wiretap

### Phase 7: Other jobs and polish
- [ ] JOB-4: other civilian jobs
- [ ] AC-2: advanced anti-cheat
- [ ] WORLD-5, WORLD-6: gates, mappings
- [ ] CHAT-6 (if chosen); HEALTH-5 was done in Phase 2
- [ ] LOG-1: complete logging

### Phase 8: Later
- [ ] WEB-1: web panel (UCP)
- [ ] Hosting / production

### Open questions (for the owner)
Questions that must be answered before the phase they belong to:

| Phase | Question |
|-------|----------|
| 3 | Can a player sell a vehicle to the state (e.g. for 50% of the price) or only to another player? |
| 5 | Taxi: a faction or a job anyone can do? |
