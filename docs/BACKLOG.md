# Podsetnik / Backlog

## 🇷🇸 Srpski

Šta treba dodati **posle** nekog sistema ili **kad dođe** neki drugi sistem. Kad se stavka uradi, prebacuje se u „Urađeno“ sa datumom.

### Čeka

**Kad vlasnik doda mape** (vlasnik ih pravi sam, a ja ih ubacujem u `gamemodes/maps/`)
| Stavka |
|--------|
| Baza za admine i helpere (`/abaza`, `/hbaza`) |
| Baze bandi, mafija i frakcija |
| Uređenje spawna na aerodromu (prva verzija iz koda je obrisana) |
| Prebaciti admin zatvor u bazu tima, kad baza bude napravljena |

**Posle sistema vozila (Faza 3)**
| Stavka |
|--------|
| `/respawnveh` – vraća napuštena vozila na mesto |
| Chat kroz prozor vozila (`/cw`) |
| Anti-cheat za vozila: HP vozila (repair hack), ulazak u zaključano vozilo |

**Posle opštine, banke i ostalih mesta (Faza 2 i dalje)**
| Stavka |
|--------|
| Teleport lokacije (`/tp`): generičke lokacije se zamenjuju pravim (opština, policija, banka, bolnica...) |
| Tutorijal posle registracije (opština, banka, posao, GPS) |
| Sve cene i plate iz `ECONOMY.md` prebaciti u jedan config fajl u kodu |

**Posle kuća (Faza 4) i frakcija (Faza 5)**
| Stavka |
|--------|
| Izbor spawna pri loginu: poslednja pozicija, kuća, sedište frakcije ili javni spawn. Za sada uvek poslednja pozicija |
| Radio, frakcijski chat, megafon |

**Posle povreda i smrti (Faza 5)**
| Stavka |
|--------|
| Spawn posle smrti u bolnici umesto na poslednjoj poziciji |
| Admin komanda za podizanje ranjenog igrača |
| Oružje se daje samo preko `AC_GivePlayerWeapon`, inače ga anti-cheat smatra cheatom |

**Napredni anti-cheat (Faza 7)**
| Stavka |
|--------|
| Lažna smrt (fake kill), spam konekcija i botovi |
| Automatske kazne za teleport i brzinu kad se pragovi provere u praksi (sada samo upozorenje) |

**Može bilo kada**
| Stavka |
|--------|
| Posle `/unjail` ili isteka kazne vratiti igrača na mesto gde je bio pre zatvora (sada ide na aerodrom). Treba sačuvati poziciju pre zatvora |
| Opis izgleda karaktera (`/examine`) |
| Akcenti (`/accent`) |
| Report bez igrača, npr. `/report zaglavio sam` (čeka odgovor vlasnika) |

**VIP (čeka odluku vlasnika i druge sisteme, vidi VIP.md)**
| Stavka |
|--------|
| Odluka o pogodnostima koje daju malu prednost (bonus na platu, dodatni respect, dodatno vozilo) i o cenama |
| `/setvip` za igrača koji nije online |
| Prelazak na viši nivo bez gubitka preostalih dana (sada novi nivo kreće od danas) |
| Rezervisano mesto za VIP kad je server pun |
| Pogodnosti vezane za payday, inventar, telefon, vozila i oglase, kad ti sistemi budu napravljeni |

**Kasnije**
| Stavka |
|--------|
| Email i povratak lozinke (web panel, Faza 8) |

### Urađeno
| Datum | Stavka |
|-------|--------|
| 21.10.2026. | Dopune logina: poslednja prijava, zaključavanje naloga, `/promenilozinku`, kamera |
| 21.10.2026. | RP chat i `/pomoc` |
| 23.10.2026. | Admin i helper sistem, deo 1 (nivoi, duty, report, ask, newbie chat, alati, logovi) |
| 23.10.2026. | Admin komanda za uključivanje globalnog OOC-a (`/ooc`) |
| 23.10.2026. | Igrač sam isključuje chatove: `/togooc`, `/togpm`, `/tognewbie` |
| 23.10.2026. | Oznaka iznad imena za tim na dužnosti i admin skin na dužnosti |
| 23.10.2026. | Dodatne admin komande: `/cc`, `/ao`, `/fixveh`, `/flip`, `/disarm`, `/ann`, `/settime`, `/setweather`, `/gotopos`, `/mypos` |
| 23.10.2026. | Sistem mapa (`gamemodes/maps/`). Prva verzija uređenja aerodroma je obrisana na zahtev vlasnika |
| 23.10.2026. | Admin sistem, deo 2: kick, ban (privremeni, trajni, offline, IP), warn, mute, newbie mute, admin zatvor, `/pmspy` |
| 23.10.2026. | Provera bana pri ulasku i upozorenje adminima za istu IP adresu |
| 23.10.2026. | Uputstvo za mapiranje (obrisano 27.10.2026. na zahtev vlasnika) |
| 24.10.2026. | Osnovni anti-cheat: oružje, jetpack, HP, armor, novac, teleport, brzina, flood, boje u chatu, `/aclog` |
| 24.10.2026. | Prikaz HP-a, armora, oružja, brzine i pinga dok admin posmatra igrača (`/spec`) |
| 24.10.2026. | Oznaka poruke prati ko je uradio akciju ([ADMIN] ili [HELPER]) |
| 27.10.2026. | Ispravljena flood zaštita (milisekunde, ponovljene poruke), zaključavanje po IP-u, bezbedan odloženi kick, ograničene komande u zatvoru, brisanje oružja pri smrti |
| 27.10.2026. | Prerada koda: `Account_Name`, `Staff_ParseTarget`, `Staff_CooldownLeft` umesto ponovljenog koda |
| 27.10.2026. | Pregled rizika i zloupotreba (`docs/RISKS.md`) |
| 27.10.2026. | VIP sistem: Bronze, Silver, Gold, `/vip`, `/vc`, `/vips`, `/setvip`, kraće pauze, istorija dodela |

---

## 🇬🇧 English

What must be added **after** a system or **once** another system exists. When an item is done, it moves to "Done" with the date.

### Waiting

**Once the owner adds maps** (the owner makes them, I put them into `gamemodes/maps/`)
| Item |
|------|
| Staff base for admins and helpers (`/abaza`, `/hbaza`) |
| Bases for gangs, mafias and factions |
| Decorate the airport spawn (the first code-only version was deleted) |
| Move admin jail into the staff base once the base exists |

**After the vehicle system (Phase 3)**
| Item |
|------|
| `/respawnveh` – returns abandoned vehicles to their spot |
| Talking through a vehicle window (`/cw`) |
| Vehicle anti-cheat: vehicle HP (repair hack), entering a locked vehicle |

**After city hall, bank and other places (Phase 2 onward)**
| Item |
|------|
| Teleport locations (`/tp`): generic spots get replaced by real ones (city hall, police, bank, hospital...) |
| Tutorial after registration (city hall, bank, job, GPS) |
| Move all prices and wages from `ECONOMY.md` into one config file in the code |

**After houses (Phase 4) and factions (Phase 5)**
| Item |
|------|
| Spawn choice on login: last position, house, faction HQ or public spawn. For now always the last position |
| Radio, faction chat, megaphone |

**After injuries and death (Phase 5)**
| Item |
|------|
| Respawn at the hospital after death instead of the last position |
| Admin command to revive a wounded player |
| Weapons are only given through `AC_GivePlayerWeapon`, otherwise anti-cheat treats them as cheats |

**Advanced anti-cheat (Phase 7)**
| Item |
|------|
| Fake kill, connection spam and bots |
| Automatic punishment for teleport and speed once the thresholds are proven in practice (alert only for now) |

**Any time**
| Item |
|------|
| After `/unjail` or when the sentence ends, return the player to where they were before jail (now they go to the airport). The pre-jail position must be saved |
| Character appearance description (`/examine`) |
| Accents (`/accent`) |
| Report without a player, e.g. `/report I'm stuck` (waiting for the owner's answer) |

**VIP (waiting for the owner's decision and other systems, see VIP.md)**
| Item |
|------|
| Decision on perks with a small advantage (payday bonus, extra respect, extra vehicle) and on prices |
| `/setvip` for an offline player |
| Upgrading to a higher tier without losing remaining days (now the new tier starts from today) |
| Reserved slot for VIP when the server is full |
| Perks tied to payday, inventory, phone, vehicles and ads, once those systems exist |

**Later**
| Item |
|------|
| Email and password recovery (web panel, Phase 8) |

### Done
| Date | Item |
|------|------|
| 2026-10-21 | Login additions: last login, account lockout, `/promenilozinku`, camera |
| 2026-10-21 | RP chat and `/pomoc` |
| 2026-10-23 | Admin and helper system, part 1 (ranks, duty, reports, ask, newbie chat, tools, logs) |
| 2026-10-23 | Admin command to toggle global OOC (`/ooc`) |
| 2026-10-23 | Players toggle chats themselves: `/togooc`, `/togpm`, `/tognewbie` |
| 2026-10-23 | Tag above the name for staff on duty and admin duty skin |
| 2026-10-23 | Extra admin commands: `/cc`, `/ao`, `/fixveh`, `/flip`, `/disarm`, `/ann`, `/settime`, `/setweather`, `/gotopos`, `/mypos` |
| 2026-10-23 | Map system (`gamemodes/maps/`). The first airport decoration was deleted at the owner's request |
| 2026-10-23 | Admin system part 2: kick, ban (temporary, permanent, offline, IP), warn, mute, newbie mute, admin jail, `/pmspy` |
| 2026-10-23 | Ban check on connect and admin alert for a shared IP |
| 2026-10-23 | Mapping guide (deleted 2026-10-27 at the owner's request) |
| 2026-10-24 | Basic anti-cheat: weapons, jetpack, HP, armour, money, teleport, speed, flood, chat colours, `/aclog` |
| 2026-10-24 | HP, armour, weapon, speed and ping shown while an admin spectates (`/spec`) |
| 2026-10-24 | Message tag follows who acted ([ADMIN] or [HELPER]) |
| 2026-10-27 | Fixed flood protection (milliseconds, repeated messages), per-IP lockout, safe delayed kick, limited commands in jail, weapons cleared on death |
| 2026-10-27 | Code refactor: `Account_Name`, `Staff_ParseTarget`, `Staff_CooldownLeft` instead of repeated code |
| 2026-10-27 | Risk and exploit review (`docs/RISKS.md`) |
| 2026-10-27 | VIP system: Bronze, Silver, Gold, `/vip`, `/vc`, `/vips`, `/setvip`, shorter cooldowns, grant history |
