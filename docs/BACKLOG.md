# Podsetnik / Backlog

## 🇷🇸 Srpski

Šta treba dodati **posle** nekog sistema ili **kad dođe** neki drugi sistem. Kad se stavka uradi, prebacuje se u „Urađeno“ sa datumom.

### Čeka

**Posle odluke o mapama** (Texture Studio ili gotove mape)
| Stavka |
|--------|
| Baza za admine i helpere (`/abaza`, `/hbaza`) |
| Baze bandi, mafija i frakcija |
| Uređenje spawna na aerodromu (prva verzija iz koda je obrisana, radi se ponovo u Texture Studio-u) |
| Prebaciti admin zatvor u bazu tima, kad baza bude napravljena |

**Posle sistema vozila (Faza 3)**
| Stavka |
|--------|
| `/respawnveh` – vraća napuštena vozila na mesto |
| Chat kroz prozor vozila (`/cw`) |

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

**Posle anti-cheata**
| Stavka |
|--------|
| Prikaz HP-a, armora, oružja i brzine dok admin posmatra igrača (`/spec`) |

**Može bilo kada**
| Stavka |
|--------|
| Opis izgleda karaktera (`/examine`) |
| Akcenti (`/accent`) |
| Report bez igrača, npr. `/report zaglavio sam` (čeka odgovor vlasnika) |

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
| 23.10.2026. | Uputstvo za mapiranje (`docs/MAPPING.md`) |

---

## 🇬🇧 English

What must be added **after** a system or **once** another system exists. When an item is done, it moves to "Done" with the date.

### Waiting

**After the decision on maps** (Texture Studio or ready-made maps)
| Item |
|------|
| Staff base for admins and helpers (`/abaza`, `/hbaza`) |
| Bases for gangs, mafias and factions |
| Decorate the airport spawn (the first code-only version was deleted, redo it in Texture Studio) |
| Move admin jail into the staff base once the base exists |

**After the vehicle system (Phase 3)**
| Item |
|------|
| `/respawnveh` – returns abandoned vehicles to their spot |
| Talking through a vehicle window (`/cw`) |

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

**After anti-cheat**
| Item |
|------|
| Show HP, armour, weapon and speed while an admin spectates a player (`/spec`) |

**Any time**
| Item |
|------|
| Character appearance description (`/examine`) |
| Accents (`/accent`) |
| Report without a player, e.g. `/report I'm stuck` (waiting for the owner's answer) |

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
| 2026-10-23 | Mapping guide (`docs/MAPPING.md`) |
