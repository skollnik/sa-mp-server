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
| Admin postavi respect veći od potrebnog, pa igrač na sledećem payday-u preskoči 20 nivoa | ✅ | `/setrespect` prima samo manje od potrebnog za sledeći nivo; `/setlevel` nuluje respect ako je preko potrebnog za novi nivo |

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
| Igrač propadne kroz pod enterijera bolnice (prva izabrana mapa je imala pod samo ispod dela plafona) | ✅ | Ta mapa je izbačena; nove mape se pre ubacivanja proveravaju da li pod postoji ispod nameštaja, po stvarnim dimenzijama modela; izabrani enterijer je proveren u igri |

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
| Previše textdraw-ova po igraču | ✅ | HUD koristi 10 po igraču (logo, sat i datum su zajednički) (granica je 256), sat i datum su zajednički za sve |
| Natpis „Prosi“ ostaje dok igrač vozi ili hoda (ušao u vozilo sa F, mimo komande) | ✅ | Prošnja se prekida pri promeni stanja (vozilo) i kad se igrač pomeri više od 1 m ili ustane; provera svake sekunde |
| Animacija sedenja ne krene prvi put (biblioteka nije učitana), pa se prošnja odmah prekine | ✅ | Biblioteke animacija se učitavaju pri svakom spawnu; poza se proverava tek posle 3 s |
| Opklada bez kockica ili novčića (dao ih je nekome dok druga strana čeka) | ✅ | Predmet se proverava pri ponudi i ponovo pri prihvatanju |
| Dodatak ostaje na telu kad ga igrač da ili baci | ✅ | Kad poslednji komad napusti inventar (bilo kojim putem), skida se sa tela i to se pamti |
| Ogroman ili daleko pomeren dodatak zaklanja ekran drugima ili lebdi | ✅ | Vlasnik je u testu odvukao ranac pola metra iza tela, pa je pomeranje ukinuto: položaji su fiksni u kodu |
| Dodatak nosi igrač koji ga više nema (stari bit u nalogu) | ✅ | Pri učitavanju inventara stavlja se samo ako je predmet tu |
| Igrač bez računa ostane bez plate ili dobitka | ✅ | Novac čeka na šalteru (`held_pay`), i za igrača koji nije online; pri otvaranju računa sam prelazi na račun |
| Transfer na igrača bez računa | ✅ | Online: odbija se; offline: upit menja samo nalog sa računom, inače se novac vraća |
| Plaćen novi PIN ili račun, a igrač izađe pre nego što se PIN sačuva | ✅ | Naplata ide tek kad je PIN spreman; ko izađe ranije ne plaća ništa |
| Neko vidi PIN u chatu | ✅ | PIN se prikazuje samo u dijalogu vlasniku, jednom; u bazi je samo šifrovan |
| Podizanje tuđeg novca sa šaltera | ✅ | Novac sa šaltera podiže samo vlasnik naloga, uz ličnu kartu |
| Dodaci zauzmu slot koji koristi drugi sistem (telefon u ruci, oružje na leđima...) | ✅ | Slotovi 0–2 su samo za dodatke; novi sistemi koriste 3 i više (odluka 80) |
| Dodaci nestanu posle smrti ili spawna | ✅ | Stavljaju se ponovo pri svakom spawnu |
| Prodavnica 24/7 pokaže predmete koji se tu ne prodaju (dodaci imaju cenu 0) | ✅ | Lista i izbor u dijalogu idu kroz istu funkciju koja preskače predmete bez cene |
| Posle restarta servera igrač vidi ranac na leđima, a `/dodaci` kaže da je u inventaru | ✅ | Klijent pamti stare objekte posle restarta; pri spawnu se prazni delovi tela sada i čiste |
| Upit za čuvanje naloga ima više vrednosti nego mesta u tekstu, pa se nalog uopšte ne čuva (pozicija, nivo, račun, dodaci) | ✅ | Od dodavanja bankovnog računa do 30.10.2026. nalog se nije čuvao; dopisane su kolone `acc_worn`, `bank_account`, `held_pay`, a računu kome je ostao PIN vraćen je račun. Pravilo: nova kolona ide i u tekst upita i u listu vrednosti, pa se u bazi proveri da se stvarno upisala |
| HUD se duplira posle smrti i pokazuje stare vrednosti | ✅ | Pri svakom spawnu stari delovi HUD-a se brišu pre nego što se naprave novi |
| Obrisan ID GPS lokacije, ulaza ili mesta se ne koristi ponovo | ✅ | Slobodan ID se traži pre nego što se novo mesto označi kao zauzeto (ranije je novo mesto nosilo ID obrisanog i „zauzimalo“ ga); vozila dobijaju najmanji slobodan ID u bazi. Nalozi, logovi, banovi i upozorenja zadržavaju svoje ID-jeve, jer se istorija veže za njih |
| Oznaka gladi pomerena desno ispod novca može da se preklopi sa zvezdicama potrage | 👁 | Potraga još ne postoji; kad dođe policija (Faza 5), proveriti položaj |

### Faza 3: vozila
| Rizik | Status | Kako je rešeno |
|-------|--------|----------------|
| Neko odveze tuđe otključano vozilo | ✅ | Ugašeno vozilo bez ključa ne može da se upali; upaljeno vozilo može da odveze svako (ključ je u bravi, kao u referentnim gamemode-ovima), ali kad ugasi motor, bez ključa ga ne pali ponovo. Vlasnik zaključava (Y) i nalazi vozilo preko `/v` → GPS |
| Vozilo blokira put dok vlasnik nije online | 👁 | Vozila su stalno u svetu; admin ga sklanja sa `/respawnveh` ili `/resetvozila` (zabranjena mesta su u BACKLOG-u) |
| Vozila neaktivnih igrača popune granicu od 2000 vozila | ✅ | Najviše 1000 ličnih vozila, a vozila naloga koji 14 dana nisu ulazili se ne pojavljuju pri pokretanju servera; dok server radi bez restarta, ostaju u svetu |
| Igrač sa kopijom ključa vozi tuđe vozilo dok vlasnik nije online | ✅ | Dozvoljeno po odluci vlasnika; vlasnik kopiju oduzima bilo kad u `/v` |
| Spam tastera N dok se motor pali, ili izlazak iz vozila tokom paljenja | ✅ | Dok traje paljenje, N ne radi; posle 1,5 s proverava se da je igrač i dalje vozač istog vozila |
| Registracija bez plaćanja ili dvaput za istu cenu | ✅ | Novac se uzima tek na kraju pregleda, posle ponovne provere (vozač, svoje vozilo, mesto, ugašen motor, HP); drugi `/registruj` dok pregled traje ne radi; produžava se tek kad ostane manje od 15 dana |
| Dve iste tablice pri registraciji | ✅ | Tablica se proverava u bazi pre upisa; vozila bez tablica imaju NULL, pa se ne sudaraju |
| Pregled oštećenog vozila dok mehaničar ne postoji | 👁 | Uslov je 650 HP (VIP `/vfix` i admin `/fixveh` popravljaju); kad dođe mehaničar, uslov raste (BACKLOG) |
| Nove tablice se ne vide dok se vozilo ponovo ne napravi | ✅ | Vozilo se pravi ponovo na istom mestu, sa istim stanjem, a putnici se vraćaju na svoja mesta (vlasnik proverava u igri) |
| Admin besplatno tunira vozila prijateljima | ✅ | Admin nivoa 1 i 2 tunira samo admin vozila; lična vozila tek nivo 3+, uz log i poruku vlasniku |
| Besplatan tuning u GTA radionicama (Transfender) | 👁 | Ne pamti se (pamti se samo `/atune`); zatvaranje radionica je u BACKLOG-u |
| `/resetvozila` izbaci nekoga ili obriše admin vozilo u upotrebi | ✅ | Pre reseta svi dobijaju poruku i vreme da sednu u vozilo; vozila u kojima neko sedi se ne diraju |
| Admin premesti vozilo u enterijer ili drugi svet (`/getveh`) | ✅ | Parking se ne menja; `/respawnveh` i reset vraćaju vozilo napolje, u svet 0 |
| Kopije ključa se dele na sve strane | ✅ | Postoji samo jedna kopija; daje je samo vlasnik, igraču pored sebe |
| Besplatna popravka uništenog vozila | ✅ | Vraća se na parking sa 500 HP; `/resetvozila` pomera vozilo bez popravke |
| Vozilo sačuvano kao zapaljeno, pa eksplodira odmah po pojavljivanju | ✅ | HP se čuva najmanje 300 |
| Kratak reconnect vrati vozilo na parking kao besplatan teleport | ✅ | Vozilo koje je još u svetu ostaje gde je |
| Gorivo bez plaćanja: relog ili restart | ✅ | Gorivo se čuva pri odjavi, nestanku vozila, točenju, zaključavanju i restartu |
| Točenje goriva iz daljine | ✅ | Mora se sedeti za volanom do 6 m od pumpe, uz ugašen motor |
| Rezervoar se nikad ne napuni do kraja (piše 54/55, a „Rezervoar je pun“) | ✅ | Toči se do punog, a započet litar se plaća kao ceo |
| Parkiranje vozila u enterijeru ili ispred vrata da smeta drugima | 👁 | U enterijeru ne može; ispred vrata za sada ne štiti ništa, admin može `/respawnveh`, `/resetvozila` ili `/obrisivozilo` (zabranjena mesta su u BACKLOG-u) |
| Dve iste tablice | ✅ | Tablica se proverava u bazi pre upisa, a kolona je jedinstvena |
| VIP farba ili popravlja tuđa vozila | ✅ | `/vcolor` samo na svom vozilu, `/vfix` na svom ili vozilu za koje ima ključ |
| Taster Y ili N smeta drugim sistemima | ✅ | Y radi samo kad je u blizini vozilo sa ključem; N samo za vozača vozila sa motorom; vrata i inventar koriste F |

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
| An admin sets more respect than needed, so the player skips 20 levels on the next payday | ✅ | `/setrespect` only takes less than the next level needs; `/setlevel` clears respect when it is over the new level's need |

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
| A player falls through the floor of the hospital interior (the first chosen map had floor only under part of the ceiling) | ✅ | That map is dropped; new maps are checked before they go in for a floor under the furniture, using the real model sizes; the chosen interior was checked in game |

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
| Too many textdraws per player | ✅ | The HUD uses 10 per player (logo, clock and date are shared) (the limit is 256); clock and date are shared by everyone |
| The "Prosi" label stays while the player drives or walks (entered a vehicle with F, bypassing the command) | ✅ | Begging ends on a state change (vehicle) and when the player moves more than 1 m or stands up; checked every second |
| The sitting animation does not start the first time (library not loaded), so begging ends at once | ✅ | Animation libraries are loaded on every spawn; the pose is checked only after 3 s |
| A bet without dice or a coin (given away while the other side waits) | ✅ | The item is checked at the offer and again at acceptance |
| An accessory stays on the body after the player gives or drops it | ✅ | When the last piece leaves the inventory (any way), it comes off the body and that is saved |
| A huge or far-moved accessory covers other players' screens or floats | ✅ | In the test the owner dragged a backpack half a metre behind the body, so moving was removed: places are fixed in the code |
| An accessory worn by a player who no longer has it (an old bit in the account) | ✅ | When the inventory loads it is put on only if the item is there |
| A player without an account loses pay or a prize | ✅ | The money waits at the counter (`held_pay`), also for offline players; it moves into the account once one is opened |
| A transfer to a player without an account | ✅ | Online: refused; offline: the query only changes an account that has one, otherwise the money comes back |
| A new PIN or account paid, but the player leaves before the PIN is saved | ✅ | The fee is charged only once the PIN is ready; someone who leaves earlier pays nothing |
| Someone sees the PIN in chat | ✅ | The PIN is shown only in a dialog to its owner, once; the DB only keeps a hash |
| Taking someone else's money from the counter | ✅ | Counter money is only paid to the account owner, with an ID card |
| Accessories take a slot another system uses (phone in hand, weapon on the back...) | ✅ | Slots 0–2 are only for accessories; new systems use 3 and up (decision 80) |
| Accessories vanish after death or spawn | ✅ | They are put back on every spawn |
| The 24/7 shop lists items it does not sell (accessories have price 0) | ✅ | The list and the dialog choice go through the same function that skips items without a price |
| After a server restart the player sees a backpack on their back while `/dodaci` says it is in the inventory | ✅ | The client keeps old objects after a restart; empty body parts are now cleared on spawn too |
| The account save query has more values than placeholders, so the account is not saved at all (position, level, bank account, accessories) | ✅ | From the bank account step until 2026-10-30 accounts were not saved; the `acc_worn`, `bank_account`, `held_pay` columns were added, and the account that still had a PIN got its bank account back. Rule: a new column goes into both the query text and the value list, then check in the DB that it was really written |
| The HUD doubles after death and shows stale values | ✅ | On every spawn the old HUD parts are destroyed before new ones are made |
| The ID of a deleted GPS location, door or point is not given out again | ✅ | The free ID is picked before the new slot is marked used (before, the new slot carried the deleted entry's ID and "took" it); vehicles get the lowest free ID in the DB. Accounts, logs, bans and warnings keep their IDs, since history is tied to them |
| The hunger badge moved right under the money can overlap the wanted stars | 👁 | Wanted levels do not exist yet; check the position once police arrive (Phase 5) |

### Phase 3: vehicles
| Risk | Status | How it is handled |
|------|--------|-------------------|
| Someone drives off in another player's unlocked vehicle | ✅ | A switched-off vehicle cannot be started without a key; a running one can be driven by anyone (the key is in the ignition, as in the reference gamemodes), but once switched off it cannot be restarted without a key. The owner locks it (Y) and finds it through `/v` → GPS |
| A vehicle blocks a road while its owner is offline | 👁 | Vehicles are always in the world; an admin moves it with `/respawnveh` or `/resetvozila` (no-parking spots are in the backlog) |
| Vehicles of inactive players fill the 2000-vehicle limit | ✅ | At most 1000 personal vehicles, and vehicles of accounts that have not logged in for 14 days are not spawned on startup; while the server runs without a restart they stay |
| A key-copy holder drives the vehicle while the owner is offline | ✅ | Allowed by the owner's decision; the owner takes the copy back any time in `/v` |
| Spamming N while the engine starts, or leaving the vehicle during the start | ✅ | N does nothing while starting; after 1.5 s the player must still be the driver of the same vehicle |
| Registering without paying, or twice for one price | ✅ | Money is taken only at the end of the inspection, after checking again (driver, own vehicle, place, engine off, health); a second `/registruj` during the inspection does nothing; renewal only once fewer than 15 days are left |
| Two identical plates at registration | ✅ | The plate is checked in the DB before it is written; vehicles without plates hold NULL, so they never clash |
| Inspecting a damaged vehicle while there is no mechanic | 👁 | The requirement is 650 HP (VIP `/vfix` and admin `/fixveh` repair); it goes up once the mechanic exists (backlog) |
| New plates do not show until the vehicle is created again | ✅ | The vehicle is created again on the same spot with the same state, and passengers are put back in their seats (the owner checks in game) |
| An admin tunes friends' vehicles for free | ✅ | Admins of level 1 and 2 tune only admin vehicles; personal vehicles need level 3+, logged and the owner is told |
| Free tuning in GTA shops (Transfender) | 👁 | Not kept (only `/atune` is saved); closing the shops is in the backlog |
| `/resetvozila` throws someone out or removes an admin vehicle in use | ✅ | Everyone gets a message and time to sit in a vehicle first; vehicles with someone inside are left alone |
| An admin moves a vehicle into an interior or another world (`/getveh`) | ✅ | The parking spot does not change; `/respawnveh` and the reset bring it back outside, into world 0 |
| Key copies passed around everywhere | ✅ | There is only one copy; only the owner gives it, to a player standing next to them |
| A free repair of a destroyed vehicle | ✅ | It comes back to its parking spot with 500 HP; `/resetvozila` moves vehicles without repairing them |
| A vehicle saved while burning explodes right after spawning | ✅ | Health is saved as at least 300 |
| A short reconnect sends the vehicle back to its spot as a free teleport | ✅ | A vehicle still in the world stays where it is |
| Free fuel through a relog or restart | ✅ | Fuel is saved on logout, despawn, refuelling, locking and restart |
| Refuelling from a distance | ✅ | The player must sit at the wheel within 6 m of a pump, with the engine off |
| The tank never fills up (shows 54/55, yet "Rezervoar je pun") | ✅ | A top-up fills to the brim, and a started litre is paid as a whole one |
| Parking in an interior or in front of a door to annoy others | 👁 | Interiors are refused; in front of doors nothing protects yet, admins can use `/resetvozila` or `/obrisivozilo` (no-parking spots are in the BACKLOG) |
| Two identical plates | ✅ | The plate is checked in the DB before the insert, and the column is unique |
| VIPs paint or repair other people's vehicles | ✅ | `/vcolor` only on their own vehicle, `/vfix` on their own or one they hold a key for |
| The Y or N key clashes with other systems | ✅ | Y only acts when a vehicle with a key is nearby; N only for the driver of a vehicle with an engine; doors and the inventory use F |
