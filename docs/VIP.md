# VIP sistem / VIP system

## 🇷🇸 Srpski

### Principi
- Tri nivoa: **Bronze**, **Silver** i **Gold VIP**. Plaćaju se mesečno, a dodeljuje ih samo vlasnik (`/setvip`).
- **Bez prednosti u igri (no pay-to-win).** VIP daje udobnost, izgled i sitne pogodnosti, ali ne novac, oružje ili moć koju igrač bez VIP-a ne može da dobije igranjem. Tako rade ozbiljni RP serveri, jer inače igrači koji ne plaćaju odlaze.
- **VIP nikad ne daje admin ili helper ovlašćenja.**
- Kad VIP istekne, pogodnosti prestaju odmah, i u igri. Karakter, novac i imovina ostaju.

### Šta je urađeno
| Pogodnost | Bronze | Silver | Gold |
|-----------|:------:|:------:|:----:|
| VIP chat `/vc` i spisak VIP igrača `/vips` | ✅ | ✅ | ✅ |
| Kraća pauza za `/report`, `/ask` i `/n` | 25% | 50% | 50% |
| Status u `/vip` i poruka pri loginu (broj preostalih dana, upozorenje 3 dana pre isteka) | ✅ | ✅ | ✅ |

**Za vlasnika:**
- `/setvip [id] [nivo 0-3] [dana]` dodeljuje, produžava ili skida VIP. Podrazumevano je 30 dana.
- Ponovna dodela **istog** nivoa produžava preostalo vreme. Dodela **drugog** nivoa kreće od danas.
- Svaka dodela se upisuje u tabelu `vip_history` (ko, koji nivo, koliko dana, do kada, ko je dodelio), da bi plaćeni meseci mogli da se provere.
- Admini vide VIP status igrača u `/check` i mogu da čitaju i pišu u VIP chat.

### Predlog pogodnosti kad dođu ostali sistemi (čeka tvoju odluku)
| Pogodnost | Bronze | Silver | Gold | Sistem |
|-----------|:------:|:------:|:----:|--------|
| Bonus na platu (payday) | +5% | +10% | +15% | Faza 2 (payday) |
| Dodatni respect poen na payday | – | – | +1 | Faza 2 (nivoi) |
| Mesto za lično vozilo više | – | +1 | +1 | Faza 3 (vozila) |
| Dodatni slotovi u inventaru | +2 | +4 | +6 | Faza 2 (inventar) |
| Biranje broja telefona | – | ✅ | ✅ | Faza 2 (telefon) |
| Sopstvena registarska tablica | – | – | ✅ | Faza 3 (vozila) |
| Besplatna promena imena jednom mesečno | – | – | ✅ | bilo kada |
| Rezervisano mesto kad je server pun | ✅ | ✅ | ✅ | bilo kada |
| Popust na oglase (`/ad`) | 10% | 20% | 30% | Faza 5 (novinari) |

Bonus na platu, dodatni respect i dodatno vozilo daju malu prednost. Na većini servera to postoji, ali na heavy RP-u se često izbacuje. To je tvoja odluka.

### Predlog cena (mesečno)
| Nivo | Cena |
|------|------|
| Bronze VIP | 3 € |
| Silver VIP | 5 € |
| Gold VIP | 10 € |

Plaćanje za sada ide van igre (dogovor sa tobom), a ti dodeljuješ VIP komandom. Automatska naplata preko sajta može kasnije, uz web panel.

---

## 🇬🇧 English

### Principles
- Three tiers: **Bronze**, **Silver** and **Gold VIP**. They are paid monthly and only the owner grants them (`/setvip`).
- **No pay-to-win.** VIP gives convenience, looks and small perks, but not money, weapons or power a non-VIP player cannot earn by playing. Serious RP servers work this way, because otherwise non-paying players leave.
- **VIP never grants admin or helper powers.**
- When VIP expires, perks stop immediately, even in-game. Character, money and property stay.

### Done
| Perk | Bronze | Silver | Gold |
|------|:------:|:------:|:----:|
| VIP chat `/vc` and online VIP list `/vips` | ✅ | ✅ | ✅ |
| Shorter cooldown for `/report`, `/ask` and `/n` | 25% | 50% | 50% |
| Status in `/vip` and a login message (days left, warning 3 days before expiry) | ✅ | ✅ | ✅ |

**For the owner:**
- `/setvip [id] [level 0-3] [days]` grants, extends or removes VIP. Default is 30 days.
- Granting the **same** level again extends the remaining time. Granting a **different** level starts from today.
- Every grant is stored in the `vip_history` table (who, which level, how many days, until when, granted by), so paid months can be checked.
- Admins see a player's VIP status in `/check` and can read and write in VIP chat.

### Proposed perks once other systems exist (waiting for your decision)
| Perk | Bronze | Silver | Gold | System |
|------|:------:|:------:|:----:|--------|
| Payday bonus | +5% | +10% | +15% | Phase 2 (payday) |
| Extra respect point on payday | – | – | +1 | Phase 2 (levels) |
| One more personal vehicle slot | – | +1 | +1 | Phase 3 (vehicles) |
| Extra inventory slots | +2 | +4 | +6 | Phase 2 (inventory) |
| Choose your phone number | – | ✅ | ✅ | Phase 2 (phone) |
| Custom number plate | – | – | ✅ | Phase 3 (vehicles) |
| Free name change once a month | – | – | ✅ | any time |
| Reserved slot when the server is full | ✅ | ✅ | ✅ | any time |
| Discount on ads (`/ad`) | 10% | 20% | 30% | Phase 5 (news) |

Payday bonus, extra respect and an extra vehicle give a small advantage. Most servers have them, but heavy RP servers often leave them out. That is your call.

### Proposed prices (monthly)
| Tier | Price |
|------|-------|
| Bronze VIP | €3 |
| Silver VIP | €5 |
| Gold VIP | €10 |

For now payment happens outside the game (arranged with you), and you grant VIP with the command. Automatic payment through a website can come later with the web panel.
