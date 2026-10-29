# VIP sistem / VIP system

## 🇷🇸 Srpski

### Principi
- Tri nivoa: **Bronze** (3 €), **Silver** (5 €) i **Gold VIP** (10 €), mesečno. Dodeljuje ih samo vlasnik (`/setvip`).
- **VIP nikad ne daje admin ili helper ovlašćenja.**
- Komande koje pomažu u igri (`/vgoto`, `/vfix`, `/vheal`) imaju pauzu, ne rade minut posle primljene štete i ne rade u zatvoru, da ne bi služile za bežanje iz borbe.
- Kad VIP istekne, pogodnosti prestaju odmah, i u igri. Karakter, novac i imovina ostaju.

### Pogodnosti koje rade
| Pogodnost | Bronze | Silver | Gold |
|-----------|:------:|:------:|:----:|
| Oznaka **[VIP]** iznad glave u boji nivoa (`/vtag` je sakriva) | ✅ | ✅ | ✅ |
| Ime u boji nivoa u OOC chatovima (`/b`, `/o`, `/n`); u IC govoru ostaje belo | ✅ | ✅ | ✅ |
| VIP chat `/vc` i spisak VIP igrača `/vips` | ✅ | ✅ | ✅ |
| `/vskin`: izbor civilnih VIP skinova sa slika | ✅ | ✅ | ✅ |
| `/vtime`, `/vweather`: lično vreme i vremenske prilike (`/vtime -1` vraća serversko) | ✅ | ✅ | ✅ |
| Bonus na platu (payday) | +5% | +10% | +15% |
| Dodatni respect poen na payday | – | – | +1 |
| Dodatna mesta u inventaru | +2 | +4 | +6 |
| Biranje broja telefona (`/promenibroj`) | – | ✅ | ✅ |
| Kraća pauza za `/report`, `/ask` i `/n` | 25% | 50% | 50% |
| Rezervisano mesto kad je server pun (poslednja 2 mesta) | ✅ | ✅ | ✅ |
| `/vfix`: popravka vozila koje voziš (pauza 10 min) | – | ✅ | ✅ |
| `/vcolor`: farbanje vozila koje voziš | – | ✅ | ✅ |
| `/vgoto`: teleport do igrača napolju (pauza 5 min) | – | – | ✅ |
| `/vheal`: HP na 100 (pauza 15 min) | – | – | ✅ |

**Oznaka iznad glave:** prikazuje se samo jedna, po prioritetu: admin na dužnosti, pa helper na dužnosti, pa VIP. Kad admin koji je VIP izađe sa dužnosti, vidi se [VIP].

**`/vgoto` ne radi** do igrača u enterijeru ili drugom virtuelnom svetu, do tima na dužnosti, do igrača koji posmatra nekoga ili je mrtav, kao ni iz vozila. Igrač do koga se teleportuješ dobija obaveštenje.

### Pogodnosti koje dolaze sa drugim sistemima (odobrene)
| Pogodnost | Bronze | Silver | Gold | Sistem |
|-----------|:------:|:------:|:----:|--------|
| Mesto za još jedno vozilo | – | +1 | +1 | Faza 3 |
| Sopstvena registarska tablica i neon | – | – | ✅ | Faza 3 |
| Popust na oglase | 10% | 20% | 30% | Faza 5 |
| Besplatna promena imena jednom mesečno | – | – | ✅ | bilo kada |
| VIP klub (enterijer) | – | ✅ | ✅ | kad vlasnik napravi mapu |

### Za vlasnika
- `/setvip [id] [nivo 0-3] [dana]` dodeljuje, produžava ili skida VIP. Podrazumevano je 30 dana. Ponovna dodela istog nivoa produžava preostalo vreme.
- Svaka dodela se upisuje u tabelu `vip_history`.
- Admini vide VIP status u `/check` i mogu da pišu u VIP chat.

---

## 🇬🇧 English

### Principles
- Three tiers: **Bronze** (€3), **Silver** (€5) and **Gold VIP** (€10), monthly. Only the owner grants them (`/setvip`).
- **VIP never grants admin or helper powers.**
- Commands that help in-game (`/vgoto`, `/vfix`, `/vheal`) have a cooldown, do not work for a minute after taking damage and do not work in jail, so they cannot be used to escape a fight.
- When VIP expires, perks stop immediately, even in-game. Character, money and property stay.

### Working perks
| Perk | Bronze | Silver | Gold |
|------|:------:|:------:|:----:|
| **[VIP]** tag above the head in the tier colour (`/vtag` hides it) | ✅ | ✅ | ✅ |
| Name in the tier colour in OOC chats (`/b`, `/o`, `/n`); IC speech stays white | ✅ | ✅ | ✅ |
| VIP chat `/vc` and online VIP list `/vips` | ✅ | ✅ | ✅ |
| `/vskin`: pick civilian VIP skins from pictures | ✅ | ✅ | ✅ |
| `/vtime`, `/vweather`: personal time and weather (`/vtime -1` returns to server time) | ✅ | ✅ | ✅ |
| Payday bonus | +5% | +10% | +15% |
| Extra respect point on payday | – | – | +1 |
| Extra inventory slots | +2 | +4 | +6 |
| Choose your phone number (`/promenibroj`) | – | ✅ | ✅ |
| Shorter cooldown for `/report`, `/ask` and `/n` | 25% | 50% | 50% |
| Reserved slot when the server is full (last 2 slots) | ✅ | ✅ | ✅ |
| `/vfix`: repair the vehicle you drive (10 min cooldown) | – | ✅ | ✅ |
| `/vcolor`: paint the vehicle you drive | – | ✅ | ✅ |
| `/vgoto`: teleport to a player outdoors (5 min cooldown) | – | – | ✅ |
| `/vheal`: HP to 100 (15 min cooldown) | – | – | ✅ |

**Tag above the head:** only one is shown, by priority: admin on duty, then helper on duty, then VIP. When an admin who is VIP goes off duty, [VIP] shows.

**`/vgoto` does not work** to players in an interior or another virtual world, to staff on duty, to players who are spectating or dead, nor from a vehicle. The target is notified.

### Perks coming with other systems (approved)
| Perk | Bronze | Silver | Gold | System |
|------|:------:|:------:|:----:|--------|
| One more vehicle slot | – | +1 | +1 | Phase 3 |
| Custom number plate and neon | – | – | ✅ | Phase 3 |
| Discount on ads | 10% | 20% | 30% | Phase 5 |
| Free name change once a month | – | – | ✅ | any time |
| VIP club (interior) | – | ✅ | ✅ | once the owner makes the map |

### For the owner
- `/setvip [id] [level 0-3] [days]` grants, extends or removes VIP. Default is 30 days. Granting the same level again extends the remaining time.
- Every grant is stored in the `vip_history` table.
- Admins see VIP status in `/check` and can write in VIP chat.
