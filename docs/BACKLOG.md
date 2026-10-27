# Podsetnik / Backlog

## 🇷🇸 Srpski

Ovde je **sve**: šta je urađeno od početka projekta, šta dolazi po planu i šta čeka neki drugi sistem. Svaki zahtev vlasnika koji nije odmah urađen upisuje se ovde, a kad se uradi, prelazi u „Urađeno“ sa datumom. Detaljan redosled faza je u [PLAN.md](PLAN.md).

### Sledeće po redu (plan)
| Faza | Šta dolazi |
|------|-----------|
| 2 | Ulazi i izlazi u enterijere (pickup, `/udji`, `/izadji`, admin alati za pravljenje) |
| 2 | Novac i banka sa logovima, payday svakog sata, nivoi i respect, statistike (`/stats`) |
| 2 | Opština (posao, lična karta), GPS lokacije, HUD |
| 2 | Inventar i predmeti, telefon (pozivi, SMS, 911) |
| 3 | Lična vozila (ključevi, gorivo, parkiranje, gepek), auto pijaca kao admin event, auto škola i dozvole, mehaničar |
| 4 | Kuće, iznajmljivanje, sef, nameštaj, garaže; firme sa zalihama; poslovi i kamiondžija; porezi |
| 5 | Frakcije (rankovi, sedišta, plate); povrede i smrt; policija, saobraćajna, SWAT; hitna i vatrogasci; vlada; novinari; taksi; vojska |
| 6 | Maska; bande i teritorije; mafije, materijali i oružje; droga; pljačke, otmice, krađa vozila, crno tržište |
| 7 | Ostali civilni poslovi, napredni anti-cheat, kapije i mape, kompletni logovi |
| 8 | Web panel (UCP), hosting |

### Čeka drugi sistem
**Kad vlasnik doda mape** (vlasnik ih pravi, a ja ih ubacujem u `gamemodes/maps/`)
| Stavka |
|--------|
| Baza za admine i helpere (`/abaza`, `/hbaza`) i admin zatvor u njoj |
| Baze bandi, mafija i frakcija |
| Uređenje spawna na aerodromu (prva verzija iz koda je obrisana) |
| VIP klub (enterijer za Silver i Gold VIP) |

**Posle opštine, banke i ostalih mesta (Faza 2)**
| Stavka |
|--------|
| Teleport lokacije (`/tp`): generičke lokacije se zamenjuju pravim |
| Tutorijal posle registracije |
| Sve cene i plate iz `ECONOMY.md` u jedan config fajl u kodu |
| VIP: bonus na platu (+5/10/15%), +1 respect na payday za Gold |
| VIP: dodatni slotovi u inventaru (+2/4/6), biranje broja telefona (Silver+) |

**Posle sistema vozila (Faza 3)**
| Stavka |
|--------|
| `/respawnveh`, chat kroz prozor vozila (`/cw`) |
| Anti-cheat za vozila: HP vozila (repair hack), ulazak u zaključano vozilo |
| VIP: mesto za još jedno vozilo (Silver+), sopstvena tablica i neon (Gold) |
| VIP: `/vcolor` i `/vfix` samo za sopstvena vozila (sada rade na svakom vozilu koje VIP vozi) |

**Posle kuća (Faza 4) i frakcija (Faza 5)**
| Stavka |
|--------|
| Izbor spawna pri loginu: poslednja pozicija, kuća, sedište frakcije ili javni spawn |
| Radio, frakcijski chat, megafon |
| VIP: popust na oglase (10/20/30%) |

**Posle povreda, smrti i policije (Faza 5)**
| Stavka |
|--------|
| Spawn posle smrti u bolnici; admin komanda za podizanje ranjenog igrača |
| Oružje se daje samo preko `AC_GivePlayerWeapon` |
| VIP: `/vheal` ne sme da radi dok je igrač ranjen; `/vgoto` i `/vfix` ne smeju da rade u potjeri ili sa potragom |

**Napredni anti-cheat (Faza 7)**
| Stavka |
|--------|
| Lažna smrt (fake kill), spam konekcija i botovi |
| Automatske kazne za teleport i brzinu kad se pragovi provere u praksi |

### Može bilo kada
| Stavka |
|--------|
| Posle `/unjail` ili isteka kazne vratiti igrača na mesto gde je bio pre zatvora (sada ide na aerodrom) |
| Opis izgleda karaktera (`/examine`), akcenti (`/accent`) |
| Report bez igrača, npr. `/report zaglavio sam` (čeka odgovor vlasnika) |
| VIP: `/setvip` za igrača koji nije online; prelazak na viši nivo bez gubitka preostalih dana |
| VIP: besplatna promena imena jednom mesečno (Gold) |
| Email i povratak lozinke (uz web panel) |

### Urađeno
| Datum | Stavka |
|-------|--------|
| 21.10.2026. | Čišćenje open.mp paketa, plan projekta, katalog sistema, odluke, ekonomija |
| 21.10.2026. | Git i GitHub repo (jedini autor skollnik) |
| 21.10.2026. | MySQL (XAMPP) i uputstvo, pluginovi (mysql, sscanf, Pawn.CMD, streamer, bcrypt, crashdetect) |
| 21.10.2026. | VS Code: kompajliranje (Ctrl+Shift+B) i pokretanje servera |
| 21.10.2026. | Kostur gamemode-a sa vezom na bazu; ime servera na jednom mestu u kodu |
| 21.10.2026. | Nalog: registracija i login (bcrypt), Ime_Prezime format, izuzeci imena (`/dozvoliime`, `/zabraniime`), pol, godine, nasumičan skin, spawn na aerodromu, čuvanje podataka |
| 21.10.2026. | Dopune logina: poslednja prijava, zaključavanje posle pogrešnih lozinki, `/promenilozinku`, kamera |
| 21.10.2026. | RP chat (`/s`, `/l`, `/w`, `/me`, `/do`, `/ame`, `/b`, `/o`, `/pm`) i `/pomoc` |
| 23.10.2026. | Jedinstven izgled sistemskih poruka; zaštita od `%` u tekstu igrača |
| 23.10.2026. | Admin i helper sistem, deo 1 (nivoi, duty, report, ask, newbie chat, alati, logovi) |
| 23.10.2026. | `/ooc`, `/ado`; igrač sam isključuje chatove |
| 23.10.2026. | Oznaka iznad imena za tim na dužnosti, admin skin na dužnosti |
| 23.10.2026. | Dodatne admin komande: `/cc`, `/ao`, `/fixveh`, `/flip`, `/disarm`, `/ann`, `/settime`, `/setweather`, `/gotopos`, `/mypos` |
| 23.10.2026. | Sistem mapa (`gamemodes/maps/`); prva verzija uređenja aerodroma obrisana na zahtev vlasnika |
| 23.10.2026. | Admin sistem, deo 2: kick, ban (privremeni, trajni, offline, IP), warn, mute, newbie mute, admin zatvor, PM spy, provera bana, upozorenje za istu IP adresu |
| 23.10.2026. | Uputstvo za mapiranje (obrisano 27.10.2026. na zahtev vlasnika) |
| 24.10.2026. | Osnovni anti-cheat: oružje, jetpack, HP, armor, novac, teleport, brzina, flood, boje u chatu, `/aclog` |
| 24.10.2026. | Prikaz HP-a, armora, oružja, brzine i pinga dok admin posmatra igrača |
| 24.10.2026. | Oznaka poruke prati ko je uradio akciju ([ADMIN] ili [HELPER]) |
| 27.10.2026. | Ispravke: flood zaštita, zaključavanje po IP-u, bezbedan odloženi kick, ograničene komande u zatvoru, brisanje oružja pri smrti |
| 27.10.2026. | Prerada koda (`Account_Name`, `Staff_ParseTarget`, `Staff_CooldownLeft`), čišćenje komentara |
| 27.10.2026. | Pregled rizika i postupak provere (`docs/RISKS.md`) |
| 27.10.2026. | VIP sistem: Bronze, Silver, Gold, `/vip`, `/vc`, `/vips`, `/setvip`, kraće pauze, istorija dodela; cene 3/5/10 € odobrene |
| 27.10.2026. | Ispravke anti-cheat-a: lažne uzbune za teleport i armor, pogrešan kick za oružje; `/specoff` poruka |
| 27.10.2026. | Kanali (`/kanali`): uključivanje i isključivanje svih chatova i obaveštenja, zaključani na dužnosti |
| 27.10.2026. | VIP: oznaka [VIP] iznad glave, ime u boji u OOC chatovima, `/vtag`, `/vskin`, `/vtime`, `/vweather`, `/vcolor`, `/vfix`, `/vgoto`, `/vheal`, rezervisana mesta |

---

## 🇬🇧 English

**Everything** is here: what has been done since the project started, what comes next by plan, and what waits for another system. Every owner request not done right away is written here, and when done it moves to "Done" with the date. The detailed phase order is in [PLAN.md](PLAN.md).

### Next up (plan)
| Phase | What comes |
|-------|-----------|
| 2 | Interior entrances and exits (pickup, `/udji`, `/izadji`, admin creation tools) |
| 2 | Money and bank with logs, hourly payday, levels and respect, stats (`/stats`) |
| 2 | City hall (job, ID card), GPS locations, HUD |
| 2 | Inventory and items, phone (calls, SMS, 911) |
| 3 | Personal vehicles (keys, fuel, parking, trunk), car market as an admin event, driving school and licenses, mechanic |
| 4 | Houses, renting, safe, furniture, garages; businesses with stock; jobs and trucker; taxes |
| 5 | Factions (ranks, HQs, salaries); injuries and death; police, traffic police, SWAT; EMS and fire; government; news; taxi; army |
| 6 | Mask; gangs and turfs; mafias, materials and weapons; drugs; robberies, kidnapping, car theft, black market |
| 7 | Other civilian jobs, advanced anti-cheat, gates and maps, complete logs |
| 8 | Web panel (UCP), hosting |

### Waiting for another system
**Once the owner adds maps** (the owner makes them, I put them into `gamemodes/maps/`)
| Item |
|------|
| Staff base for admins and helpers (`/abaza`, `/hbaza`) with admin jail inside |
| Bases for gangs, mafias and factions |
| Decorate the airport spawn (the first code-only version was deleted) |
| VIP club (interior for Silver and Gold VIP) |

**After city hall, bank and other places (Phase 2)**
| Item |
|------|
| Teleport locations (`/tp`): generic spots replaced by real ones |
| Tutorial after registration |
| All prices and wages from `ECONOMY.md` in one config file in the code |
| VIP: payday bonus (+5/10/15%), +1 respect per payday for Gold |
| VIP: extra inventory slots (+2/4/6), choosing a phone number (Silver+) |

**After the vehicle system (Phase 3)**
| Item |
|------|
| `/respawnveh`, talking through a vehicle window (`/cw`) |
| Vehicle anti-cheat: vehicle HP (repair hack), entering a locked vehicle |
| VIP: one more vehicle slot (Silver+), custom plate and neon (Gold) |
| VIP: `/vcolor` and `/vfix` only for own vehicles (they now work on any vehicle the VIP drives) |

**After houses (Phase 4) and factions (Phase 5)**
| Item |
|------|
| Spawn choice on login: last position, house, faction HQ or public spawn |
| Radio, faction chat, megaphone |
| VIP: discount on ads (10/20/30%) |

**After injuries, death and police (Phase 5)**
| Item |
|------|
| Respawn at the hospital after death; admin command to revive a wounded player |
| Weapons are only given through `AC_GivePlayerWeapon` |
| VIP: `/vheal` must not work while wounded; `/vgoto` and `/vfix` must not work during a chase or while wanted |

**Advanced anti-cheat (Phase 7)**
| Item |
|------|
| Fake kill, connection spam and bots |
| Automatic punishment for teleport and speed once the thresholds are proven in practice |

### Any time
| Item |
|------|
| After `/unjail` or when the sentence ends, return the player to where they were before jail (now the airport) |
| Character appearance description (`/examine`), accents (`/accent`) |
| Report without a player, e.g. `/report I'm stuck` (waiting for the owner's answer) |
| VIP: `/setvip` for an offline player; upgrading without losing remaining days |
| VIP: free name change once a month (Gold) |
| Email and password recovery (with the web panel) |

### Done
| Date | Item |
|------|------|
| 2026-10-21 | open.mp package cleanup, project plan, feature catalog, decisions, economy |
| 2026-10-21 | Git and GitHub repo (sole author skollnik) |
| 2026-10-21 | MySQL (XAMPP) and its guide, plugins (mysql, sscanf, Pawn.CMD, streamer, bcrypt, crashdetect) |
| 2026-10-21 | VS Code: compiling (Ctrl+Shift+B) and starting the server |
| 2026-10-21 | Gamemode skeleton with a database connection; server name in one place in the code |
| 2026-10-21 | Account: register and login (bcrypt), Firstname_Lastname format, name exemptions (`/dozvoliime`, `/zabraniime`), gender, age, random skin, airport spawn, data saving |
| 2026-10-21 | Login additions: last login, lockout after wrong passwords, `/promenilozinku`, camera |
| 2026-10-21 | RP chat (`/s`, `/l`, `/w`, `/me`, `/do`, `/ame`, `/b`, `/o`, `/pm`) and `/pomoc` |
| 2026-10-23 | Unified system message style; protection against `%` in player text |
| 2026-10-23 | Admin and helper system, part 1 (ranks, duty, reports, ask, newbie chat, tools, logs) |
| 2026-10-23 | `/ooc`, `/ado`; players toggle chats themselves |
| 2026-10-23 | Tag above the name for staff on duty, admin duty skin |
| 2026-10-23 | Extra admin commands: `/cc`, `/ao`, `/fixveh`, `/flip`, `/disarm`, `/ann`, `/settime`, `/setweather`, `/gotopos`, `/mypos` |
| 2026-10-23 | Map system (`gamemodes/maps/`); first airport decoration deleted at the owner's request |
| 2026-10-23 | Admin system part 2: kick, ban (temporary, permanent, offline, IP), warn, mute, newbie mute, admin jail, PM spy, ban check, shared-IP alert |
| 2026-10-23 | Mapping guide (deleted 2026-10-27 at the owner's request) |
| 2026-10-24 | Basic anti-cheat: weapons, jetpack, HP, armour, money, teleport, speed, flood, chat colours, `/aclog` |
| 2026-10-24 | HP, armour, weapon, speed and ping shown while an admin spectates |
| 2026-10-24 | Message tag follows who acted ([ADMIN] or [HELPER]) |
| 2026-10-27 | Fixes: flood protection, per-IP lockout, safe delayed kick, limited commands in jail, weapons cleared on death |
| 2026-10-27 | Code refactor (`Account_Name`, `Staff_ParseTarget`, `Staff_CooldownLeft`), comment cleanup |
| 2026-10-27 | Risk review and change checklist (`docs/RISKS.md`) |
| 2026-10-27 | VIP system: Bronze, Silver, Gold, `/vip`, `/vc`, `/vips`, `/setvip`, shorter cooldowns, grant history; prices €3/5/10 approved |
| 2026-10-27 | Anti-cheat fixes: false teleport and armour alerts, wrong weapon kick; `/specoff` message |
| 2026-10-27 | Channels (`/kanali`): toggling every chat and notification, locked while on duty |
| 2026-10-27 | VIP: [VIP] tag above the head, coloured name in OOC chats, `/vtag`, `/vskin`, `/vtime`, `/vweather`, `/vcolor`, `/vfix`, `/vgoto`, `/vheal`, reserved slots |
