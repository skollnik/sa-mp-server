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

### Faza 2: vrata, novac, inventar, telefon
| Rizik | Status | Kako je rešeno |
|-------|--------|----------------|
| GPS i posao koriste isti checkpoint, pa se posao završi bez vožnje | ✅ | Upravljač checkpoint-a: reaguje samo sistem koji je postavio checkpoint, a GPS nikad ne uzima tuđi (koristi ikonicu na mapi) |
| Bežanje iz borbe kroz vrata | ✅ | Vrata ne rade 30 s posle primljene štete, samo peške, uz pauzu od 2 s |
| Bekstvo iz admin zatvora ili zamrzavanja kroz vrata (učitavanje enterijera odmrzava igrača) | ✅ | Vrata, inventar i telefon ne rade u zatvoru ni dok je igrač zamrznut; admin teleport ne odmrzava zamrznutog |
| Propadanje kroz pod custom enterijera pre nego što se objekti učitaju (vrata, login u enterijeru, admin teleport, kraj posmatranja) | ✅ | Objekti se učitaju pre pomeranja i igrač stoji 1,5 s (`Utils_MoveInto`) |
| Lažni AC teleport pri prolasku kroz vrata | ✅ | Svako pomeranje ide preko `AC_ExpectTeleport` |
| Ulaz obrisan dok je igrač unutra (ostane zaglavljen) | ✅ | Igrači unutra se prebacuju ispred ulaza, a sačuvan položaj igrača van igre se menja u bazi; adminu koji posmatra menja se mesto povratka |
| Ponovo korišćen ID ulaza: novi ulaz dobije svet obrisanog, pa se u njemu pojave stara mesta ili igrači | ✅ | Brisanje ulaza briše i mesta u njegovom svetu i izvodi sve iz njega; svet se ne dira ako ga deli još neki ulaz |
| Igrač se prijavi baš dok se briše ulaz (učitan je stari položaj u obrisanom svetu) | 👁 | Retko; admin ga izvlači sa `/gethere` |
| F za bacanje cigarete ili flaše istovremeno otvori vrata | ✅ | Isti pritisak samo baca ono što je u ruci |
| Admin posmatra igrača u custom enterijeru i vidi prazninu; teleport tokom posmatranja ostavi pogrešan enterijer | ✅ | Admin svake sekunde prati enterijer i svet igrača i objekti se učitavaju oko igrača; kad se igrač teleportuje (skok veći od 100 m) ili promeni enterijer, kamera se ponovo kači za njega; teleport tokom posmatranja ga prekida i postaje mesto povratka (vlasnik proverava u igri) |
| Lična karta u dijalogu zatvori igraču dijalog koji je imao otvoren (npr. banku) | 👁 | Samo na 3 m, uz zaštitu od spama komandama |
| Igrač umre od gladi dok bolnica ne postoji | ✅ | Na nuli HP pada samo do 20 |
| Glad pada AFK igraču, u zatvoru ili adminu na dužnosti | ✅ | Ne pada dok je igrač AFK, u admin zatvoru, na admin dužnosti ili nije u svetu (spec, mrtav) |
| Lečenje hranom do punog HP-a | ✅ | Hrana daje malo HP-a i ne može da se jede iznad 90 na traci gladi |
| Petlja smrti od gladi | ✅ | Posle smrti traka gladi je bar 50 |
| Dva igrača dobiju isti broj (istovremena kupovina) | ✅ | Odgovori iz baze stižu jedan po jedan; drugi vidi da je broj već dodeljen igraču online |
| Plaćanje za broj koji je u međuvremenu zauzet, ili dupla naplata | ✅ | Novac se uzima tek kad provera pokaže da je broj slobodan; dok provera traje, nova se ne prihvata |
| Broj sa nulom na početku (000001) | ✅ | Prihvata se samo tačno 6 cifara od 100000 do 999999, uz jasnu poruku |
| Davanje telefona drugome daje i broj | ✅ | Broj i kredit su na nalogu, ne na aparatu |
| Kredit u minusu ili preko granice | ✅ | Plaća se samo ako ima dovoljno; dopuna najviše do $500 |
| Pretraga javnog imenika za tuđe brojeve | ✅ | Prikazuje samo one koji su se sami upisali, a svaka pretraga košta $2 kredita |
| Ugašen broj (180 dana) ostao u tuđim kontaktima pa ga dobije neko drugi | 👁 | Kao u stvarnosti; kontakt prikazuje ime dok ga vlasnik ne obriše |
| Dopuna gotovinom bez prodavnice (otvoren dijalog pa odlazak) | ✅ | Mesto se proverava i kad igrač izabere iznos |
| Happy hour u minutima ne pogodi nijednu platu | ✅ | Zadaje se u broju plata |
| Happy hour se izgubi posle restarta | 👁 | Upisano u BACKLOG |
| Dupliranje novca preko transfera ili `/daj` | ✅ | Novac se skida pa dodaje, oba igrača se odmah čuvaju, sve ide u `money_logs` |
| Prebacivanje novca sa lažnih naloga | ✅ | Transfer od nivoa 2 uz proviziju 1%, svaki prenos u logu sa oba naloga |
| Transfer igraču koji se upravo prijavljuje (njegovo staro stanje prepiše uplatu) | ✅ | Transfer se odbija dok se primalac prijavljuje |
| Negativan ili ogroman iznos u dijalogu banke | ✅ | Prihvata se samo pozitivan ceo broj do stanja |
| Payday za AFK igrače ili dvaput isti sat (izlazak i ulazak) | ✅ | Minuti se broje samo dok igrač nije pauzirao i pomera se ili piše; čuvaju se u bazi; payday je jednom po punom satu |
| Payday u admin zatvoru | ✅ | Vreme u zatvoru se ne broji |
| Dupliranje predmeta (davanje, bacanje, podizanje) | ✅ | Predmet se uvek prvo skida, pa dodaje; bačen predmet nestaje sa zemlje pre nego što uđe u inventar |
| Zatrpavanje mape bačenim predmetima | ✅ | Najviše 200 bačenih predmeta, svaki nestaje posle 15 min |
| Bačeni predmeti nestanu posle restarta servera | 👁 | Namerno, ne čuvaju se u bazi |
| Lečenje hranom ili prvom pomoći u borbi | ✅ | Hrana daje malo HP-a i ne jede se iznad 90 na traci gladi; prva pomoć ne radi minut posle štete i leči samo do 75 HP |
| Telefonski razgovor nastavi posle izlaska, smrti, zatvora ili kad igrač ostane bez telefona | ✅ | Poziv se prekida u svim tim slučajevima |
| Spam SMS-om ili pozivima | ✅ | `/sms` je chat komanda (mute i flood zaštita), uz cenu po poruci; `/pozovi` ima zaštitu od spama komandama, a tekst za 911 prolazi proveru za mute i spam |
| Mute zbog ponovljene komande koja nije spam (npr. dva puta isti `/pozovi`) | ✅ | `/pozovi` više nije chat komanda; ista poruka se računa kao ponavljanje samo ako stigne u roku od 10 s |
| Dva igrača sa istim brojem telefona | ✅ | Broj se proverava u bazi i među igračima online pre dodele i naplate |
| VIP lično vreme ostane posle isteka VIP-a | ✅ | Serversko vreme se primenjuje čim VIP istekne |
| Melodija telefona sa nevažećim brojem iz baze | ✅ | Broj melodije se proverava pre upotrebe, nevažeći postaje prva melodija |
| Zvonjava kao spam okolini | ✅ | Zvoni najviše 30 s po pozivu, čuje se samo do 15 m, a pozivi se plaćaju kreditom |
| Sakriven deo HUD-a ostane prikazan ili se pojavi dupli | ✅ | Posle svake promene HUD se ceo sklanja i ponovo crta samo sa uključenim delovima |
| HUD sa custom teksturama koji igrač na 0.3.7 ne vidi | ✅ | Koriste se samo teksture iz igre |
| Pogađanje tuđeg PIN-a ili izbegavanje blokade izlaskom iz igre | ✅ | Bankomat isplaćuje samo vlasniku računa; pogrešni pokušaji i blokada se odmah čuvaju u bazi; PIN je šifrovan |
| Zaobilaženje PIN-a preko menija bankomata | ✅ | Meni bankomata se otvara tek posle tačnog PIN-a; open.mp prihvata odgovor samo za prikazani dijalog |
| Prebacivanje novca sa lažnih naloga kroz opklade ili prošnju | ✅ | Opklade od nivoa 2 i do $5.000; prošnja sa dnevnim limitima u bazi i bez davanja sa iste IP adrese; sve ide u log novca |
| Opklada kad jedan igrač u međuvremenu potroši novac ili ode | ✅ | Pri prihvatanju se sve proverava ponovo (novac, blizina, nivo); oba uloga se uzimaju pre bacanja |
| Spam ponudama za opkladu (dijalog drugom igraču) | ✅ | Pauza 10 s između ponuda i samo jedna ponuda po igraču odjednom; ponuda ističe za 30 s |
| Natpis „Prosi“ ostane iznad tuđe glave posle izlaska | ✅ | Natpis se briše pri izlasku, smrti i ulasku u zatvor |
| Dva tiketa u istom izvlačenju | ✅ | Tiket je jedan red po nalogu (primarni ključ); drugi pokušaj vraća novac |
| Kupovina tiketa tokom izvlačenja | ✅ | Prodaja je zatvorena od upita za izvlačenje do brisanja tiketa |
| Dobitnik na lutriji nije online (njegovo staro stanje prepiše dobitak) | ✅ | Igraču online se dodaje u memoriji, a igraču van igre u bazi |
| Fond lutrije se izgubi posle restarta | ✅ | Fond je u tabeli `server_vars`; tiketi ostaju u bazi do izvlačenja |
| Previše textdraw-ova po igraču | ✅ | HUD koristi 17 po igraču (granica je 256), sat i datum su zajednički za sve |

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

### Phase 2: doors, money, inventory, phone
| Risk | Status | How it is handled |
|------|--------|-------------------|
| GPS and a job share the checkpoint, so the job finishes without driving | ✅ | Checkpoint manager: only the system that set the checkpoint reacts, and GPS never takes someone else's (it uses a map icon) |
| Escaping a fight through a door | ✅ | Doors do not work for 30 s after taking damage, only on foot, with a 2 s pause |
| Escaping admin jail or a freeze through a door (loading an interior unfreezes the player) | ✅ | Doors, inventory and phone do not work in jail or while frozen; admin teleports do not unfreeze a frozen player |
| Falling through a custom interior floor before its objects load (doors, login inside, admin teleport, end of spectating) | ✅ | Objects are loaded before the move and the player is held for 1.5 s (`Utils_MoveInto`) |
| False AC teleport alert when using a door | ✅ | Every move goes through `AC_ExpectTeleport` |
| A door deleted while a player is inside (stuck) | ✅ | Players inside are moved in front of the door and the saved position of offline players is changed in the DB; a spectating admin's return spot is moved |
| A reused door ID: the new door gets the deleted door's world, and old points or players show up in it | ✅ | Deleting a door also deletes the points in its world and moves everyone out; the world is left alone if another door shares it |
| A player logs in exactly while a door is deleted (old position in the deleted world is loaded) | 👁 | Rare; an admin pulls them out with `/gethere` |
| F to throw away a cigarette or bottle also opens a door | ✅ | The same press only throws away what is in hand |
| An admin spectating a player in a custom interior sees nothing; teleporting while spectating leaves the wrong interior | ✅ | The admin follows the player's interior and world every second and objects are loaded around the player; when the player teleports (a jump over 100 m) or changes interior, the camera is attached to them again; teleporting while spectating ends it and becomes the return spot (the owner checks in game) |
| The ID card dialog closes a dialog the player had open (e.g. the bank) | 👁 | Only within 3 m, with command spam protection |
| A player starves to death while there is no hospital | ✅ | At zero, HP only drops to 20 |
| Hunger drops for AFK players, in jail or for admins on duty | ✅ | It does not drop while the player is AFK, in admin jail, on admin duty or not in the world (spectating, dead) |
| Healing to full HP with food | ✅ | Food gives a little HP and cannot be eaten above 90 on the hunger bar |
| A hunger death loop | ✅ | After death the hunger bar is at least 50 |
| Two players get the same number (buying at the same time) | ✅ | DB answers arrive one by one; the second sees the number is already given to an online player |
| Paying for a number taken in the meantime, or paying twice | ✅ | Money is taken only once the check shows the number is free; no new check is accepted while one runs |
| A number with a leading zero (000001) | ✅ | Only exactly 6 digits from 100000 to 999999 are accepted, with a clear message |
| Giving the phone away also gives the number | ✅ | Number and credit belong to the account, not the device |
| Negative credit or credit over the limit | ✅ | It is charged only when there is enough; top-ups go up to $500 at most |
| Searching the public directory for other people's numbers | ✅ | Only those who listed themselves are shown, and each search costs $2 of credit |
| A freed number (180 days) stays in someone's contacts and goes to someone else | 👁 | As in real life; the contact shows the name until its owner deletes it |
| Cash top-up away from a shop (dialog left open, then walking away) | ✅ | The place is checked again when the amount is picked |
| Happy hour in minutes misses every payday | ✅ | It is set in paydays |
| Happy hour is lost after a restart | 👁 | Written in BACKLOG |
| Money duplication through transfers or `/daj` | ✅ | Money is taken before it is given, both players are saved right away, everything goes into `money_logs` |
| Moving money from alt accounts | ✅ | Transfers from level 2 with a 1% fee, every transfer logged with both accounts |
| Transfer to a player who is logging in (their old balance overwrites the payment) | ✅ | The transfer is refused while the receiver is logging in |
| Negative or huge amount in the bank dialog | ✅ | Only a positive whole number up to the balance is accepted |
| Payday for AFK players or twice in the same hour (relogging) | ✅ | Minutes only count while the game is not paused and the player moves or types; they are saved in the DB; payday happens once per full hour |
| Payday in admin jail | ✅ | Time in jail does not count |
| Item duplication (give, drop, pick up) | ✅ | The item is always taken first, then given; a dropped item leaves the ground before it enters the inventory |
| The map filling up with dropped items | ✅ | At most 200 dropped items, each disappears after 15 min |
| Dropped items vanish after a server restart | 👁 | On purpose, they are not stored in the DB |
| Healing with food or first aid during a fight | ✅ | Food gives a little HP and cannot be eaten above 90 on the hunger bar; first aid does not work for a minute after damage and heals only up to 75 HP |
| A phone call goes on after leaving, dying, jail or losing the phone | ✅ | The call ends in all those cases |
| SMS or call spam | ✅ | `/sms` is a chat command (mute and flood protection) with a price per message; `/pozovi` has command spam protection, and 911 text goes through the mute and spam check |
| A mute for a repeated command that is not spam (e.g. the same `/pozovi` twice) | ✅ | `/pozovi` is no longer a chat command; the same message only counts as a repeat if it comes within 10 s |
| Two players with the same phone number | ✅ | The number is checked in the DB and among online players before it is assigned |
| VIP personal time stays after VIP expires | ✅ | Server time applies as soon as VIP expires |
| A phone ringtone with an invalid number from the DB | ✅ | The ringtone number is checked before use; an invalid one becomes the first ringtone |
| Ringing as spam for people around | ✅ | It rings at most 30 s per call, is heard only within 15 m, and calls cost credit |
| A hidden HUD part stays on screen or shows up twice | ✅ | After every change the whole HUD is removed and drawn again with only the parts that are on |
| A custom-texture HUD that a 0.3.7 player cannot see | ✅ | Only textures from the game are used |
| Guessing a PIN or dodging the block by leaving | ✅ | An ATM only pays the account owner; wrong tries and the block are saved to the DB at once; the PIN is hashed |
| Skipping the PIN through the ATM menu | ✅ | The ATM menu opens only after a correct PIN; open.mp accepts a response only for the dialog that is shown |
| Moving money from alt accounts through bets or begging | ✅ | Bets from level 2 and up to $5,000; begging with daily caps in the DB and no giving from the same IP; everything goes into the money log |
| A bet where one player spends the money or leaves meanwhile | ✅ | Everything is checked again on accept (money, distance, level); both stakes are taken before the roll |
| Spamming bet offers (a dialog for the other player) | ✅ | 10 s between offers and only one offer per player at a time; an offer expires after 30 s |
| The "Prosi" label stays over someone else's head after leaving | ✅ | The label is removed on leaving, death and jail |
| Two tickets in the same draw | ✅ | A ticket is one row per account (primary key); a second try returns the money |
| Buying a ticket during a draw | ✅ | Sales are closed from the draw query until the tickets are deleted |
| A lottery winner is offline (their old balance overwrites the prize) | ✅ | An online player gets it in memory, an offline one in the DB |
| The lottery fund is lost after a restart | ✅ | The fund is in the `server_vars` table; tickets stay in the DB until the draw |
| Too many textdraws per player | ✅ | The HUD uses 17 per player (the limit is 256); clock and date are shared by everyone |
