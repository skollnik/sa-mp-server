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

### Kako se proverava svaka izmena
Za svaki novi sistem ili izmenu prolazi se kroz ova pitanja, za sve postojeće sisteme, a ne samo za jedan primer:
1. **Zajednički podaci:** ko još čita ili menja isti podatak (novac, poziciju, checkpoint, oružje, tajmer, kolonu u bazi)? Može li jedan sistem da pregazi drugi?
2. **Stanja igrača:** šta ako igrač u tom trenutku izađe, umre, bude u zatvoru, ima mute, posmatra ili je posmatran, sedi u vozilu, u enterijeru ili drugom virtuelnom svetu?
3. **Kašnjenje klijenta:** šta ako klijent javi promenu sekundu kasnije (lag, prozor u pozadini)? Ne sme da nastane lažna uzbuna ni pogrešan kick.
4. **Ponavljanje i brzina:** šta ako igrač komandu kuca 100 puta ili dva igrača rade isto u istoj sekundi?
5. **Ko sme:** da li je provera nivoa i dužnosti na pravom mestu, i da li poruka otkriva previše?
6. **Baza:** da li asinhroni odgovor proverava da je igrač isti i da li se išta gubi pri restartu?

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
| Igrač bez dozvole ukuca `/specoff` i dobije poruku namenjenu adminu | ✅ | Prvo se proverava dozvola |
| Broj reporta se ponovo iskoristi, pa admin prihvati pogrešan report | 👁 | Retko; brojevi se ponovo koriste tek kad se report zatvori |
| Ban IP-a zaključa sve na istoj mreži | 👁 | Namerno; `127.0.0.1` se nikad ne banuje. `/unban` skida ceo ban |
| Ban po serijskom broju pogodi sve naloge na istom računaru | 👁 | Namerno (sprečava zaobilaženje bana). Vlasnik je izuzet |

### VIP
| Rizik | Status | Kako je rešeno |
|-------|--------|----------------|
| VIP dodeli neko ko nije vlasnik | ✅ | `/setvip` radi samo za vlasnika |
| Autosave prepiše tek dodeljen VIP | ✅ | VIP polja menja samo VIP modul, odmah u bazi |
| VIP istekne dok je igrač online, a pogodnosti i dalje rade | ✅ | Rok se proverava pri svakoj upotrebi |
| Spor oko plaćenih meseci | ✅ | Svaka dodela je u tabeli `vip_history` |
| Spam u VIP chatu | ✅ | `/vc` je chat komanda: važe mute i flood zaštita |
| `/vgoto` za bežanje iz borbe ili ulazak u tuđi enterijer | ✅ | Pauza 5 min, ne radi minut posle primljene štete, samo peške, samo do igrača napolju (enterijer 0, svet 0), ne do tima na dužnosti ni do igrača u spec-u ili mrtvog; cilj dobija obaveštenje |
| `/vheal` i `/vfix` usred borbe | ✅ | Ne rade minut posle primljene štete i imaju pauzu |
| `/vheal` dok je igrač ranjen, `/vgoto` i `/vfix` u potjeri | 📋 | Kad dođu povrede i policija |
| `/vcolor` i `/vfix` na tuđem vozilu | 📋 | Kad dođu lična vozila: samo sopstvena |
| `/vskin` uniforma frakcije | ✅ | Lista sadrži samo civilne skinove |
| Igrač odgovori na `/vskin` dijalog posle isteka VIP-a | ✅ | Status se proverava i pri odgovoru |
| Rezervisano mesto zaobiđe neko ko nije VIP | ✅ | Provera posle čitanja naloga iz baze, pre logina |

### Kanali
| Rizik | Status | Kako je rešeno |
|-------|--------|----------------|
| Admin isključi reportove ili AC upozorenja, pa ih niko ne vidi | ✅ | Na dužnosti su zaključani |
| Niko ne vidi pitanje kad nema helpera | ✅ | Pitanja su zaključana za helpere i admine na dužnosti |
| Igrač piše u kanal koji je sam isključio | ✅ | Ne može dok ga ne uključi |
| Klik u `/kanali` promeni pogrešan kanal | ✅ | Pamti se koji je red koji kanal u poslednjem prikazanom spisku |

### Anti-cheat
| Rizik | Status | Kako je rešeno |
|-------|--------|----------------|
| Cheat novca | ✅ | Novac je na serveru, a ekran se vraća svake sekunde |
| Cheat oružja | ✅ | Kick za oružje koje server nije dao; spisak se briše pri smrti |
| Cheat municije (beskonačni meci za dato oružje) | 📋 | Prati se kad dođe sistem oružja |
| Jetpack | ✅ | Kick |
| HP i armor | 👁 | Vraćaju se na pravu vrednost i adminima stiže upozorenje |
| Teleport i speedhack | 👁 | Samo upozorenje adminima, jer lag i padovi daju lažne uzbune |
| Lažna uzbuna za teleport posle izlaska iz zatvora (klijent kasni sa novom pozicijom) | ✅ | Posle teleporta servera provera čeka dok igrač stvarno ne stigne na cilj, najviše 10 s |
| Lažna uzbuna za armor odmah posle `/setarmour` | ✅ | Provera čeka da klijent potvrdi novu vrednost |
| Pogrešan kick za oružje odmah posle `/disarm` ili ulaska u zatvor | ✅ | Posle oduzimanja oružja provera oružja čeka 3 s |
| Vozila: popravka cheat-om, ulazak u zaključano vozilo | 📋 | Kad dođe sistem vozila |
| Lažna smrt, spam konekcija, botovi | 📋 | Napredni anti-cheat (Faza 7) |

### Mape
| Rizik | Status | Kako je rešeno |
|-------|--------|----------------|
| Skriven kod u gotovim skriptama sa interneta (npr. komanda koja određenoj IP adresi daje admin nivo) | ✅ | Ne ubacujemo gotove skripte ni filterskripte, sav kod pišemo sami |
| Igrač avionom doleti do enterijera koji visi iznad grada i uđe u njega bez vrata | ✅ | Enterijeri su u enterijeru 20, pa se u otvorenom svetu ne vide i nemaju koliziju |
| Previše objekata na jednom mestu, pa klijentu fale zidovi | ✅ | Streamer šalje do 1000 objekata, a najveći enterijer ih ima 986 |
| Igrač ili admin se pojavi zaglavljen u objektu nove mape (npr. `/tp` na Pershing Square, gde je sada parking) | ✅ | Provereno u igri |
| Uklonjeni GTA objekti (zgrada opštine, lampe i drveće na trgu, pozorište pored banke) ostanu kod igrača koji je već bio na serveru | ✅ | Uklanjaju se pri svakom ulasku na server, pre spawna |
| Nova zgrada opštine i parking ispred nje se dodiruju, pa tlo može da treperi ili da se objekti preklope | ✅ | Provereno u igri |
| Izdaleka nestanu objekti nove mape, a GTA modeli za daljinu su uklonjeni, pa ostane rupa (tlo opštine i trga, pozorište pored banke) | ✅ | Objekti mapa se vide do 300 m, a velike ploče tla i ponovo napravljeno pozorište do 800 m |
| Natpis „BANKA“ se ne uklapa u banere pozorišta (jedan materijal pokriva dva banera) | ✅ | Svaki baner dobija reč odozgo nadole; provereno u igri |
| Staklene ploče ispred banke se razbiju kod jednog igrača (GTA staklo puca od metka), a kod drugih ostanu cele | 👁 | Samo izgled, ne utiče na igru; proverava se u igri, a staklo se menja drugim modelom ako smeta |

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

### How every change is checked
Every new system or change goes through these questions, against all existing systems, not just one example:
1. **Shared data:** who else reads or writes the same data (money, position, checkpoint, weapons, timer, database column)? Can one system overwrite another?
2. **Player states:** what if the player leaves, dies, is jailed, muted, spectating or spectated, in a vehicle, an interior or another virtual world at that moment?
3. **Client delay:** what if the client reports the change a second later (lag, window in the background)? No false alarm or wrong kick may follow.
4. **Repetition and speed:** what if the player types the command 100 times, or two players do the same thing in the same second?
5. **Permissions:** is the level and duty check in the right place, and does the message reveal too much?
6. **Database:** does the async answer check the player is still the same, and is anything lost on restart?

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
| A player without permission types `/specoff` and gets an admin-only message | ✅ | Permission is checked first |
| A report number is reused, so an admin accepts the wrong report | 👁 | Rare; numbers are reused only after a report is closed |
| An IP ban locks everyone on the same network | 👁 | By design; `127.0.0.1` is never banned. `/unban` lifts the whole ban |
| A serial ban hits every account on the same computer | 👁 | By design (stops ban evasion). The owner is exempt |

### VIP
| Risk | Status | How it is handled |
|------|--------|-------------------|
| VIP granted by someone other than the owner | ✅ | `/setvip` works only for the owner |
| Autosave overwrites a fresh VIP grant | ✅ | Only the VIP module writes the VIP columns, straight to the database |
| VIP expires while online but perks keep working | ✅ | Expiry is checked on every use |
| Dispute over paid months | ✅ | Every grant is stored in `vip_history` |
| VIP chat spam | ✅ | `/vc` is a chat command: mute and flood protection apply |
| `/vgoto` to escape a fight or enter someone's interior | ✅ | 5 min cooldown, not for a minute after taking damage, on foot only, only to players outdoors (interior 0, world 0), not to staff on duty or players spectating or dead; the target is notified |
| `/vheal` and `/vfix` mid-fight | ✅ | Blocked for a minute after taking damage, plus a cooldown |
| `/vheal` while wounded, `/vgoto` and `/vfix` during a chase | 📋 | Once injuries and police exist |
| `/vcolor` and `/vfix` on someone else's vehicle | 📋 | Once personal vehicles exist: own vehicles only |
| `/vskin` faction uniform | ✅ | The list holds civilian skins only |
| Answering the `/vskin` dialog after VIP expired | ✅ | Status is checked again on the answer |
| A non-VIP gets a reserved slot | ✅ | Checked after the account row is read, before login |

### Channels
| Risk | Status | How it is handled |
|------|--------|-------------------|
| An admin switches off reports or AC alerts, so nobody sees them | ✅ | Locked while on duty |
| Nobody sees a question when no helper is around | ✅ | Questions are locked for helpers and admins on duty |
| A player writes in a channel they switched off | ✅ | Not until they switch it back on |
| A click in `/kanali` toggles the wrong channel | ✅ | The row-to-channel mapping of the last shown list is remembered |

### Anti-cheat
| Risk | Status | How it is handled |
|------|--------|-------------------|
| Money cheat | ✅ | Money is server-side and the display is reset every second |
| Weapon cheat | ✅ | Kick for weapons the server never gave; the list is cleared on death |
| Ammo cheat (infinite ammo for a given weapon) | 📋 | Tracked once the weapon system exists |
| Jetpack | ✅ | Kick |
| HP and armour | 👁 | Reset to the real value and admins are alerted |
| Teleport and speedhack | 👁 | Admin alert only, since lag and falls cause false alarms |
| False teleport alert after leaving jail (client reports the new position late) | ✅ | After a server teleport the check waits until the player actually arrives, up to 10 s |
| False armour alert right after `/setarmour` | ✅ | The check waits for the client to confirm the new value |
| Wrong weapon kick right after `/disarm` or being jailed | ✅ | After weapons are taken away the weapon check waits 3 s |
| Vehicles: cheat repair, entering a locked vehicle | 📋 | With the vehicle system |
| Fake kill, connection spam, bots | 📋 | Advanced anti-cheat (Phase 7) |

### Maps
| Risk | Status | How it is handled |
|------|--------|-------------------|
| Hidden code in ready-made scripts from the internet (e.g. a command granting admin to a specific IP address) | ✅ | We never add ready-made scripts or filterscripts, all code is our own |
| A player flies a plane up to an interior hanging above the city and walks in without a door | ✅ | Interiors are in interior 20, so in the open world they are invisible and have no collision |
| Too many objects in one place, so the client misses walls | ✅ | The streamer sends up to 1000 objects, and the biggest interior has 986 |
| A player or admin ends up stuck in an object of a new map (e.g. `/tp` to Pershing Square, now a parking lot) | ✅ | Checked in game |
| Removed GTA objects (the city hall building, lamps and trees on the square, the theatre next to the bank) stay for a player who was on the server before | ✅ | They are removed on every connect, before spawn |
| The new city hall building and the parking in front of it touch, so the ground may flicker or objects may overlap | ✅ | Checked in game |
| Map objects vanish at a distance while the GTA LOD models are removed, leaving a hole (city hall and square ground, the theatre next to the bank) | ✅ | Map objects are visible up to 300 m, large ground planes and the recreated theatre up to 800 m |
| The "BANKA" text does not fit the theatre banners (one material covers two banners) | ✅ | Each banner gets the word top to bottom; checked in game |
| The glass panels in front of the bank break for one player (GTA glass shatters when shot) but stay whole for others | 👁 | Visual only, no gameplay effect; checked in game, the glass is swapped for another model if it matters |
