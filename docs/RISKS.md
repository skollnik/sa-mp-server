# Rizici i zloupotrebe / Risks and exploits

## 🇷🇸 Srpski

Spisak svega što bi igrač mogao da zloupotrebi ili što bi moglo da se pokvari kad se sistemi ukrste, i kako je rešeno. Dopunjuje se posle svakog novog sistema.

**Status:** ✅ rešeno · 👁 prati se (rešeno delimično ili samo upozorava) · 📋 planirano

### Pravila za buduće sisteme
Ova pravila sprečavaju greške kao što je „GPS pomeri marker posla, pa igrač završi posao stojeći“:

1. **Jedan vlasnik checkpoint-a.** Igrač ima samo jedan crveni checkpoint u igri. Svi sistemi (GPS, poslovi, auto škola, misije) ga traže preko zajedničkog upravljača, koji pamti ko ga je postavio. Posao prihvata ulazak u checkpoint **samo ako je on njegov vlasnik**. Ako je GPS zauzet poslom, GPS koristi ikonicu na mapi umesto checkpoint-a.
2. **Server je jedini izvor istine.** Novac, oružje, armor, HP, pozicija posla i vreme kazne se vode na serveru. Klijentu se nikad ne veruje.
3. **Provera na kraju, ne samo na početku.** Posao proverava i da je igrač stvarno prošao rutu (vreme između tačaka, udaljenost, isto vozilo), a ne samo da je stigao do poslednje tačke.
4. **Asinhroni upiti proveravaju da je igrač još isti** (`g_AccountRace`), jer isti ID može dobiti novi igrač dok baza odgovara.
5. **Novac se prenosi u jednom koraku.** Kod trgovine se oduzima i dodaje odjednom, posle provere da obe strane i dalje imaju šta prodaju, da ne bi moglo da se duplira.
6. **Svaka komanda ima pauzu ili limit** ako može da se spamuje (report, ask, newbie chat, komande uopšte).
7. **Teleport, oružje i armor samo preko AC funkcija**, inače ih anti-cheat prijavljuje ili, još gore, ne primećuje cheat.

### Nalog i login
| Rizik | Status | Kako je rešeno |
|-------|--------|----------------|
| Pogađanje lozinke | ✅ | 3 pokušaja po ulasku, a posle 5 pogrešnih u 15 min zaključava se prijava **sa te IP adrese** |
| Neko namerno pogreši lozinku 5 puta da zaključa vlasnika (ili bilo koga) | ✅ | Zaključavanje važi samo za IP sa koga se pogađa, ne za ceo nalog |
| Lažni odgovor na dijalog (preskakanje koraka registracije ili promene lozinke) | ✅ | open.mp odbija odgovor na dijalog koji nije prikazan; promena lozinke dodatno traži potvrđenu staru lozinku |
| Odgovor baze stigne kad je na istom ID-u već drugi igrač | ✅ | Svaki upit nosi `g_AccountRace` i proverava ga |
| Odloženi kick pogodi novog igrača na istom ID-u | ✅ | Kick nosi broj konekcije i ne izvršava se ako se promenio |
| Ime vlasnika (`skollnik`) | 👁 | Nalog već postoji i zaštićen je lozinkom. Ako se baza ikad obriše, vlasnik mora prvi da se registruje |
| Lozinka u memoriji servera | ✅ | Čuva se samo između dva dijaloga i briše odmah posle heširanja |

### Chat
| Rizik | Status | Kako je rešeno |
|-------|--------|----------------|
| Znak `%` u poruci ruši server | ✅ | Tekst igrača uvek ide kao `%s` argument |
| Kodovi boja `{FF0000}` za lažne admin poruke | ✅ | Brišu se iz svega što igrač napiše |
| Spam | ✅ | Kazneni poeni po brzini slanja; 3 upozorenja = automatski mute 2 min |
| Mute se izbegne preko druge chat komande | ✅ | Sve chat komande imaju oznaku i blokirane su pod mute-om |
| `~` u `/ann` ruši klijente | ✅ | Znak `~` se menja u `-` |

### Tim (admini i helperi)
| Rizik | Status | Kako je rešeno |
|-------|--------|----------------|
| Admin kazni ili preimenuje višeg admina | ✅ | Provera nivoa pre svake akcije nad drugim adminom |
| Admin u zatvoru pobegne preko `/goto` ili `/spec` | ✅ | U zatvoru rade samo chat komande i nekoliko bezbednih (report, ask, pomoć) |
| Zatvor se izbegne izlaskom sa servera | ✅ | Preostalo vreme se čuva u bazi |
| Admin vozila kao besplatni automobili | ✅ | Vozi ih samo admin, a brišu se kad admin izađe |
| Broj reporta se ponovo iskoristi, pa admin prihvati pogrešan report | 👁 | Retko; brojevi se ponovo koriste tek kad se report zatvori |
| Ban IP-a zaključa sve na istoj mreži | 👁 | Namerno; `127.0.0.1` se nikad ne banuje. `/unban` skida ceo ban |
| Ban po serijskom broju pogodi sve naloge na istom računaru | 👁 | Namerno (sprečava zaobilaženje bana). Vlasnik je izuzet |

### Anti-cheat
| Rizik | Status | Kako je rešeno |
|-------|--------|----------------|
| Cheat novca | ✅ | Novac je na serveru, a ekran se vraća svake sekunde |
| Cheat oružja | ✅ | Kick za oružje koje server nije dao; spisak se briše pri smrti |
| Cheat municije (beskonačni meci za dato oružje) | 📋 | Prati se kad dođe sistem oružja |
| Jetpack | ✅ | Kick |
| HP i armor | 👁 | Vraćaju se na pravu vrednost i adminima stiže upozorenje |
| Teleport i speedhack | 👁 | Samo upozorenje adminima, jer lag i padovi daju lažne uzbune |
| Vozila: popravka cheat-om, ulazak u zaključano vozilo | 📋 | Kad dođe sistem vozila |
| Lažna smrt, spam konekcija, botovi | 📋 | Napredni anti-cheat (Faza 7) |

---

## 🇬🇧 English

A list of everything a player could abuse or that could break when systems interact, and how it is handled. Extended after every new system.

**Status:** ✅ handled · 👁 watched (partly handled or alert only) · 📋 planned

### Rules for future systems
These rules prevent bugs like "the GPS moves the job marker, so the player finishes the job standing still":

1. **One checkpoint owner.** A player has a single red checkpoint in the game. Every system (GPS, jobs, driving school, missions) requests it through a shared manager that remembers who set it. A job accepts entering the checkpoint **only if it owns it**. If a job holds the checkpoint, the GPS uses a map icon instead.
2. **The server is the only source of truth.** Money, weapons, armour, HP, job progress and punishment time live on the server. The client is never trusted.
3. **Validate the whole run, not just the end.** A job also checks that the player really drove the route (time between points, distance, same vehicle), not only that they reached the last point.
4. **Async queries check that the player is still the same one** (`g_AccountRace`), since a new player can take the ID while the database answers.
5. **Money moves in one step.** In trades it is removed and added at once, after checking both sides still have what they sell, so nothing can be duplicated.
6. **Every spammable command has a cooldown or limit** (report, ask, newbie chat, commands in general).
7. **Teleports, weapons and armour only through the AC functions**, otherwise anti-cheat flags them or, worse, misses a cheat.

### Account and login
| Risk | Status | How it is handled |
|------|--------|-------------------|
| Password guessing | ✅ | 3 tries per connection; after 5 wrong in 15 min, logins **from that IP** are locked |
| Someone fails 5 times on purpose to lock out the owner (or anyone) | ✅ | The lock applies only to the guessing IP, not the whole account |
| Spoofed dialog response (skipping register or password-change steps) | ✅ | open.mp rejects responses to dialogs not shown; password change also requires a verified old password |
| Database answer arrives after another player took the same ID | ✅ | Every query carries `g_AccountRace` and checks it |
| A delayed kick hits a new player on the same ID | ✅ | The kick carries a connection number and is skipped if it changed |
| The owner's name (`skollnik`) | 👁 | The account exists and is password-protected. If the database is ever wiped, the owner must register first |
| Password kept in server memory | ✅ | Only between two dialogs, wiped right after hashing |

### Chat
| Risk | Status | How it is handled |
|------|--------|-------------------|
| A `%` in a message crashes the server | ✅ | Player text always goes in as a `%s` argument |
| `{FF0000}` colour codes for fake admin messages | ✅ | Stripped from everything a player writes |
| Spam | ✅ | Strikes based on sending speed; 3 warnings = automatic 2-minute mute |
| Bypassing mute through another chat command | ✅ | Every chat command is flagged and blocked while muted |
| `~` in `/ann` crashes clients | ✅ | `~` is replaced with `-` |

### Staff (admins and helpers)
| Risk | Status | How it is handled |
|------|--------|-------------------|
| An admin punishes or renames a higher admin | ✅ | Level check before every action on another admin |
| A jailed admin escapes via `/goto` or `/spec` | ✅ | In jail only chat commands and a few safe ones (report, ask, help) work |
| Avoiding jail by leaving the server | ✅ | Remaining time is stored in the database |
| Admin vehicles as free cars | ✅ | Only admins drive them, and they are deleted when the admin leaves |
| A report number is reused, so an admin accepts the wrong report | 👁 | Rare; numbers are reused only after a report is closed |
| An IP ban locks everyone on the same network | 👁 | By design; `127.0.0.1` is never banned. `/unban` lifts the whole ban |
| A serial ban hits every account on the same computer | 👁 | By design (stops ban evasion). The owner is exempt |

### Anti-cheat
| Risk | Status | How it is handled |
|------|--------|-------------------|
| Money cheat | ✅ | Money is server-side and the display is reset every second |
| Weapon cheat | ✅ | Kick for weapons the server never gave; the list is cleared on death |
| Ammo cheat (infinite ammo for a given weapon) | 📋 | Tracked once the weapon system exists |
| Jetpack | ✅ | Kick |
| HP and armour | 👁 | Reset to the real value and admins are alerted |
| Teleport and speedhack | 👁 | Admin alert only, since lag and falls cause false alarms |
| Vehicles: cheat repair, entering a locked vehicle | 📋 | With the vehicle system |
| Fake kill, connection spam, bots | 📋 | Advanced anti-cheat (Phase 7) |
