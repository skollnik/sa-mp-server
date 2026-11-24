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
| Admin ostavi jetpack ili god mode upaljen van dužnosti, ili ga anti-cheat prijavi | ✅ | Obe moći rade samo na dužnosti (vlasnik i van nje) i gase se sa dužnošću ili spuštanjem nivoa ispod 5; anti-cheat nikad ne izbacuje admina nivoa 5+ zbog jetpacka (u vazduhu klijent kasni), već mu jetpack samo skine ako je moć ugašena; HP 99999 se ne pamti pri odjavi |
| Jetpack skinut na F ostaje na zemlji (ostavlja ga sama igra, server ne može da ga obriše), a admin ga posle podigne; `/jetpack` u padu kaže „dobili ste“, a jetpack ne stigne | ✅ | F gasi moć, pa se podignut jetpack odmah skida; skidanje sa `/jetpack` briše animaciju i ne ostavlja ništa na zemlji; 1,5 s posle davanja server proverava da li je jetpack stigao i, ako nije, gasi moć i kaže adminu da proba na zemlji |

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
| Igrač koji uđe na server u kući ne može da izađe 30 s: dok se enterijer učitava padne malo, a igra to javi kao štetu | ✅ | Šteta od pada i davljenja (bez napadača) se ne računa kao borba; i dalje se računa šteta od drugog igrača, vozila i eksplozije |
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
| Neko odveze tuđe otključano vozilo | ✅ | Ugašeno vozilo bez ključa ne može da se upali; upaljeno vozilo može da odveze svako (ključ je u bravi), ali kad ugasi motor, bez ključa ga ne pali ponovo. Vlasnik zaključava (Y) i nalazi vozilo preko `/v` → GPS |
| Vozilo blokira put dok vlasnik nije online | 👁 | Vozila su stalno u svetu; admin ga sklanja sa `/respawnveh` ili `/resetvozila` (zabranjena mesta su u BACKLOG-u) |
| Vozila neaktivnih igrača popune granicu od 2000 vozila | ✅ | Najviše 1000 ličnih vozila, a vozila naloga koji 14 dana nisu ulazili se ne pojavljuju pri pokretanju servera; dok server radi bez restarta, ostaju u svetu |
| Igrač sa kopijom ključa vozi tuđe vozilo dok vlasnik nije online | ✅ | Dozvoljeno po odluci vlasnika; vlasnik kopiju oduzima bilo kad u `/v` |
| Spam tastera N dok se motor pali, ili izlazak iz vozila tokom paljenja | ✅ | Dok traje paljenje, N ne radi; posle 1,5 s proverava se da je igrač i dalje vozač istog vozila |
| Registracija bez plaćanja ili dvaput za istu cenu | ✅ | Novac se uzima tek na kraju pregleda, posle ponovne provere (vozač, svoje vozilo, mesto, ugašen motor, HP); drugi `/registruj` dok pregled traje ne radi; produžava se tek kad ostane manje od 15 dana |
| Dve iste tablice pri registraciji | ✅ | Tablica se proverava u bazi pre upisa; vozila bez tablica imaju NULL, pa se ne sudaraju |
| Tablice za neregistrovano vozilo ili dve tablice odjednom | ✅ | Policija izdaje tablice samo registrovanom vozilu, i nove tek kad se izvađene postave; provera se ponavlja posle izbora u dijalogu |
| Sopstvena tablica sa uvredljivim tekstom ili bojama | ✅ | Samo slova bez naših, brojevi, razmak i crtica (bez `{}` i `~`); admin je menja sa `/resettablice` |
| Ista tablica izdata dvaput | ✅ | Proverava se i među postavljenim i među izvađenim tablicama, a obe kolone su jedinstvene u bazi |
| Kamera kazni isto vozilo više puta u prolazu | ✅ | Ista kamera isto vozilo najviše jednom u 10 minuta; tolerancija 5 km/h |
| Kazna za vožnju tuđeg vozila ide vlasniku, a ne vozaču | 👁 | Namerno, kao u stvarnosti; vlasnik ne daje ključ ni upaljeno vozilo nepoznatima |
| Kazne se ignorišu | ✅ | Neplaćene kazne blokiraju registraciju; policija ih kasnije vidi (BACKLOG) |
| Plaćanje kazni koje su stigle posle otvaranja liste | ✅ | Plaća se tačno ono što je lista pokazala (do poslednjeg ID-ja sa liste) |
| Kamera stoji na kolovozu i smeta vožnji | 👁 | Početne kamere su postavljene unapred; vlasnik proverava u igri i pomera ih (`/obrisikameru`, `/napravikameru`) |
| Kupljena vozila se gomilaju na izlazu pijace i eksplodiraju posle restarta | ✅ | Kupovina ne radi dok je izlaz zauzet; parking kupljenog vozila postaje prvo mesto gde ga vlasnik ostavi van pijace, a na izlazu se ne može parkirati |
| Neko odveze ili uništi izložbeno vozilo | ✅ | Izložbena vozila su zaključana, vozač se izbacuje, a uništeno vozilo se vraća novo posle 5 s; guranje je u BACKLOG-u |
| Plaćanje pijace delom gotovinom, delom sa računa, ili kupovina rasprodatog vozila | ✅ | Cela cena iz jednog izvora; komadi i blizina se proveravaju ponovo pri plaćanju |
| Prevara pri prodaji („dao sam pare, nije mi dao auto“) | ✅ | Novac i vozilo se menjaju u istom trenutku, samo preko `/ponudi`; kupac vidi model, tablicu, registraciju, gorivo, stanje i cenu |
| Vozilo se prebaci na drugi nalog „za $1“ | ✅ | Najmanja cena $1.000 i taksa 5%; sve ide u `vehicle_trades` i novčani log |
| Ponuda prihvaćena kad se nešto promenilo (vozilo pomereno, igrač otišao, nema novca) | ✅ | Sve se proverava ponovo pri prihvatanju; ponuda važi 60 s |
| Prodaja vozila sa kopijom ključa ili sa izvađenim tablicama | ✅ | Kopija se poništava; dok izvađene tablice nisu postavljene, vozilo se ne prodaje |
| VIP kupi dodatno vozilo pa VIP istekne | 👁 | Vozilo ostaje (odluka vlasnika); novo ne može dok je na limitu |
| Meni pokreće komandu kojoj admin nema pravo | ✅ | Meni prikazuje samo opcije do nivoa admina, a pokrenuta komanda ionako proverava nivo |
| Admin poklanja predmete, novac ili popravke | 👁 | Sve je po nivoima (predmeti i izmene vozila nivo 4, novac vlasnik) i ide u admin log |
| Pregled oštećenog vozila dok mehaničar ne postoji | 👁 | Uslov je 650 HP (VIP `/vfix` i admin `/fixveh` popravljaju); kad dođe mehaničar, uslov raste (BACKLOG) |
| Nove tablice se ne vide dok se vozilo ponovo ne napravi | ✅ | Vozilo se pravi ponovo na istom mestu, sa istim stanjem, a putnici se vraćaju na svoja mesta (vlasnik proverava u igri) |
| Admin besplatno tunira vozila prijateljima | ✅ | Admin nivoa 1 i 2 tunira samo admin vozila; lična vozila tek nivo 3+, uz log i poruku vlasniku |
| Besplatan tuning, farbanje i popravka u GTA radionicama (Transfender, Pay'n'spray) | ✅ | Radionice su zatvorene kapijom; ako neko ipak uđe (npr. lag ili teleport), lično vozilo se vraća na sačuvani izgled, a drugi ne vide promenu |
| Dva igrača uzmu isti predmet iz gepeka u isto vreme (dupliranje) | ✅ | Svaka radnja ponovo proverava predmet i količinu u trenutku izvršenja; drugi dobija poruku da predmeta više nema |
| Krađa iz gepeka | 👁 | Moguća samo dok je gepek otvoren i vozilo otključano (RP); zaključavanje štiti; svaka radnja je u chatu i u `trunk_log` |
| Prevara pri prodaji vozila (prodavac isprazni gepek posle ponude) | ✅ | Kupac vidi sadržaj u ponudi; svaka promena gepeka posle slanja poništava ponudu |
| Predmet koji je na igraču ode u gepek, a ostane na telu | ✅ | Naočare, kapa, ranac i torba moraju prvo da se skinu |
| Besplatno gorivo kanisterom | ✅ | Puni se samo na pumpi po istoj ceni po litru; sipanje traje 5 s i proverava se ponovo (vozilo blizu, motor ugašen, kanister još kod igrača) |
| Kanister u vozilu bez goriva (admin vozila, bicikli) | ✅ | Odbija se: sipa se samo u vozilo koje koristi gorivo |
| Dva delimično puna kanistera se spoje u jedan (gubi se kanister) | ✅ | Kanister sa gorivom se nikad ne spaja; daje se, baca i stavlja u gepek uvek ceo |
| Igrač uđe u vozilo dok sipa, pa ga posle nekoliko sekundi izbaci | ✅ | Bilo koji taster odmah prekida sipanje; animacija se briše samo dok je igrač peške |
| Gepek zaključanog vozila otvoren i ispražnjen | ✅ | Zaključavanje zatvara haubu i gepek, a dok je vozilo zaključano ne mogu da se otvore |
| Drugi igrač uzme vozilo za ispit | ✅ | Ko nije na ispitu biva izbačen iz vozila; vozilo se briše na kraju ispita |
| Vozilo za ispit kao besplatno vozilo (izlazak sa servera, smrt, zatvor) | ✅ | Ispit pada i vozilo se briše; igrač se vraća na šalter |
| Ispit preskočen teleportom ili prečicom | ✅ | Checkpoint se računa samo za vozača u vozilu za ispit, redom; vreme je ograničeno |
| GPS i ispit koriste isti marker | ✅ | Ispit koristi trkački checkpoint, a GPS se gasi na početku vožnje |
| Vozilo za ispit se stvori u objektu na parkingu | ✅ | Na parkingu nema objekata |
| Igrač ne sedne u vozilo za ispit posle dalekog teleporta i padne ispit zbog vremena van vozila | ✅ | Stavlja se u vozilo tek kad ga klijent učita; vreme van vozila se broji tek kad sedne (ili posle 8 s, kad može sam da uđe) |
| Propeler helikoptera za ispit udari u objekat ili igrača pored | ✅ | Start na otvorenoj pisti bez objekata u krugu od 22 m; motor se pali tek kad igrač sedne |
| Ruta za brod vodi preko kopna ili kroz mol | ✅ | Svaka tačka i deo puta provereni kroz GTA geometriju; ruta ide južno od kraja mola |
| Poruka u chatu duža od 144 znaka se ne prikaže | ✅ | Kamera šalje svaku kaznu u posebnom redu; nove poruke sa promenljivim delovima proveravati na najdužu verziju |
| Pucanje u vozilo ili struganje povređuje putnike bez pojasa | ✅ | Udes se računa samo za gubitak od 50+ HP odjednom pri brzini od 30+ km/h |
| Igrač izbačen kroz šoferšajbnu zaglavi u zidu ili ga anti-cheat prijavi | ✅ | Postavlja se iznad krova vozila i leti napred; teleport i kretanje su najavljeni anti-cheatu |
| Pojas ostane vezan posle izlaska, smrti ili prelaska u drugo vozilo | ✅ | Pojas se odvezuje pri svakoj promeni stanja igrača |
| Pozajmljena kaciga sa ispita ostane igraču ili uđe u inventar | ✅ | Nije predmet; skida se na kraju ispita (prolaz, pad, smrt, izlazak) i vraća se ono što je nosio na glavi |
| Kaciga i šešir na glavi u isto vreme | ✅ | Kaciga je dodatak za glavu, pa zamenjuje šešir ili kapu |
| Kaciga se menja u vozilu i tako zaobiđe proveru kamere ili ispita | ✅ | Kamera i ispit gledaju stanje u trenutku prolaska; stavljanje kacige u vožnji je dozvoljeno i nije prekršaj |
| Mehaničar odvuče šlepom tuđe ispravno vozilo | ✅ | Kače se samo vozila ispod 20% ili bez goriva, i samo kad niko ne sedi za volanom |
| Mehaničar naplati popravku koja nije urađena | ✅ | Novac se uzima tek kad se popravka završi, a mehaničar je i dalje pored vozila |
| Prijatelji jedni drugima popravljaju za $50 i niko ne ide u servis | ✅ | Cena je fiksna po procentu ($20 na terenu, $15 u radionici, $50 po gumi); mehaničar dobija 60%, a 40% su delovi koji nestaju iz igre |
| Ponuda za popravku pa udes pre prihvatanja (jeftina popravka velike štete) | ✅ | Pri prihvatanju se proverava da je stanje i broj puklih guma isti kao u ponudi; ako nije, traži se nova ponuda |
| Mehaničar prekine animaciju ili ode usred popravke | ✅ | Zamrznut je dok popravka traje; prekida se ako bude povređen, završi smenu, vozač ode ili se vozilo pomeri više od 1,5 m; plaća se tek na kraju |
| Mehaničar ostane zamrznut posle popravke | ✅ | Odmrzava se na kraju i pri svakom prekidu, osim ako ga je admin zamrznuo (`/freeze`) |
| Teren se koristi umesto radionice | ✅ | Na terenu najviše 60% i bez karoserije; sve preko toga samo u radionici (letelice i brodovi u servisu letelica i brodova) |
| Tjuning na ulici pored radionice | ✅ | Samo kad vozilo stoji na mestu `tjuning` (6 m) |
| Popravka u radionici na putu pored nje | ✅ | Servis je samo 8 m oko tačke `servis` ili `pregledservis`; tačka `radionica` je samo za `/oprema` |
| Repair hack (cheat popravi vozilo bez mehaničara) | ✅ | HP i pukle gume se vraćaju na poslednje poznato stanje, adminima stiže upozorenje i upis u AC log. Udubljenja, vrata i svetla se ne proveravaju: klijent ih posle popravke javlja na svoj način (lažne prijave), a samo su izgled |
| Anti-cheat lažno prijavi serversku popravku | ✅ | Sve serverske popravke i nova vozila idu kroz `AC_*` funkcije, a klijent ima 3 s da prikaže promenu; još 30 s se prihvata kad se vozilo, posle zakasnele prijave, vrati do stanja koje je server postavio |
| Parkiranje vozila ispred ulaza, šaltera ili mesta za rent da posle restarta smeta | ✅ | `/v parkiraj` odbija mesta bliže od 40 m (10 m od tuđe kuće); na putu i dalje rešava admin, kasnije policija |
| Prodaja kuće dok su vozila u garaži (kupac ih ne može izvaditi) | ✅ | Prodaja državi i preko `/ponudi` traži praznu garažu; kad kuću oduzme admin ili neaktivnost, vozila izlaze na svoje parking mesto |
| Vozilo iz garaže prodato ili zamenjeno sa `/ponudi` | ✅ | Vozilo u garaži nije u svetu, pa ne može u ponudu |
| Gomilanje vozila u garaži preko limita | ✅ | Vozilo u garaži se i dalje računa u limit vozila |
| Skrivanje u praznoj kući (razgledanje kuće na prodaju) | ✅ | Razgledanje traje najviše 5 min, posle toga ista kuća čeka 5 min; ulaz ima ista pravila kao vrata (borba, zatvor, zamrzavanje); kad je neko kupi, posetioci izlaze odmah; ko je bio unutra kad je kuća vraćena na prodaju dobija istih 5 min |
| Kuća kao bekstvo iz borbe ili zatvora | ✅ | Ista pravila kao vrata: posle štete 30 s nema ulaza, zatvor i zamrzavanje sprečavaju ulaz |
| Kupovina više kuća preko VIP-a pa istek | ✅ | Kuće ostaju, nova ne može dok je igrač na limitu |
| Neaktivan vlasnik drži kuću zauvek | ✅ | Posle 30 dana kuća ide na prodaju uz 50% na račun |
| Pojavljivanje u kući kao bekstvo iz zatvora | ✅ | Zatvor ima prednost; kuća mora i dalje biti igračeva |
| Izlazak i ulazak na server kao teleport kući | ✅ | Kuća važi pri ulasku tek posle 10 min van servera |
| Smrt kao teleport kući | ✅ | Posle smrti uvek bolnica |
| Ikonica tuđe kuće ostane igraču koji dobije isti playerid | ✅ | Ikonica vlasnika se briše čim izađe sa servera, a pravi se tek posle prijave (ne vidi je neko ko samo ukuca ime vlasnika) |
| ID vozila u garaži dobije drugo vozilo | ✅ | U svetu ID uvek pripada vozilu koje ga sada ima; od dva vozila u garaži ID pamti samo poslednje |
| Obrisan ID ponovo dat, a neko je ostao u svetu obrisane kuće | ✅ | Pri brisanju se svi iz te kuće izbacuju ispred vrata |
| Parking u garaži preko broja mesta | ✅ | Parking u garaži može imati najviše onoliko vozila koliko garaža prima; ako je garaža puna, vozilo staje ispred nje |
| Prodaja kuće dok neko vozilo ima parking u garaži | ✅ | Prodaja se odbija dok se parking ne promeni |
| Sef: dva igrača ili posetilac uzmu iste stvari, ili sef ostane otvoren posle izlaska | ✅ | Sef otvara samo vlasnik, peške, do 1,5 m od sefa; svaki odgovor u meniju ponovo proverava mesto, vlasnika i da je predmet još tu |
| Sef: dupliranje novca ili predmeta (pad servera između dva upisa) | ✅ | Prvo se uzima iz jednog mesta, pa daje drugom, i oba se odmah upisuju; svaki pomak ide u `house_safe_log` |
| Sef „prodat“ sa kućom ili izgubljen bez znanja vlasnika | ✅ | Kuća se ne prodaje (državi ni igraču) dok sef nije prazan; kad kuću oduzme admin ili neaktivnost, novac ide vlasniku na račun (i van servera), predmeti propadaju; admin `/izmenikucu` vidi i prazni sef (log) |
| F kod sefa otvori sef umesto vrata | ✅ | Izlaz ima prednost; `/udji` i `/izadji` nikad ne otvaraju sef |
| Kirija: soba besplatno do prve plate, ili kirija promenjena dok igrač potvrđuje | ✅ | Prva kirija se plaća odmah; ako se kirija promeni između pitanja i potvrde, traži se nova potvrda |
| Kirija: igrač izbegava plaćanje (prazan račun, AFK) | ✅ | Kirija ide sa plate koju stanar dobije: račun, pa džep; bez novca se iseljava; bez plate (AFK) nema ni kirije, ali ni zarade |
| Stanar zloupotrebi kuću (sef, garaža, zaključavanje, prodaja) | ✅ | Stanar samo ulazi (i kad je zaključano), pojavljuje se u kući i koristi svoj ormar; sve ostalo traži vlasnika |
| Stanar ostane posle promene vlasnika ili kad prestane da igra | ✅ | Svaka promena vlasnika iseljava sve stanare (i van servera, u bazi); 30 dana bez ulaska na server iseljava; pojavljivanje u sobi se briše zajedno sa sobom |
| Vlasnik iseli stanara odmah posle plate i zadrži kiriju | ✅ | Pri iseljenju vlasnik vraća poslednju plaćenu kiriju; bez novca iseljenje se odbija; ako je stanar u međuvremenu sam otišao, ništa se ne plaća |
| Igrač ima kuću i sobu (dva besplatna spawna) | ✅ | Soba se ne iznajmljuje ako igrač ima kuću; kupovina ili prijem kuće iseljava iz sobe |
| Odeća: dupliranje kese ili VIP skin kao kesa za prodaju | ✅ | Obučena odeća ide u istu kesu ili isto mesto u ormaru; VIP skinovi, uniforme, admin i posebni skinovi nikad ne postaju kesa (bacaju se uz poruku) |
| Odeća: presvlačenje ispod uniforme ili admin skina, ili tuđeg pola | ✅ | Presvlači se samo bez smene i admin dužnosti, peške; oblači se samo odeća za pol igrača |
| Ormar kao besplatno skladište bez kuće | ✅ | Ormar radi samo u kući ili sobi gde igrač živi i svaka kuća ima svoj; kad igrač izgubi dom, garderoba ide u magacin, koji se otvara samo iz ormara u nekom domu; kapacitet 3/5/8 |
| Dve kuće dele ormar, ili stvari prethodnog vlasnika i stanara ostanu novom vlasniku | ✅ | Ormar je vezan za kuću i stanovnika; kuća se prodaje tek kad je ormar vlasnika prazan; ormari onih koji više ne žive u kući idu u magacin |
| Duplo uzimanje iz magacina (dupli klik, dva prozora) | ✅ | Red se prvo briše u bazi, a predmet se daje tek kad je brisanje uspelo; ako je inventar u međuvremenu pun, red se vraća |
| Ormar ostane učitan posle iseljenja, pa se stara garderoba uzme još jednom | ✅ | Posle svakog premeštanja u magacin svi učitani ormari se brišu iz memorije i čitaju ponovo iz baze |
| Nameštaj ili sef kao put van enterijera ili kao zid u kući | ✅ | Postavlja samo vlasnik u svojoj kući; predmet mora biti do 35 m od izlaza, u visini enterijera i do 10 m od igrača; sve se proverava ponovo pri čuvanju |
| Kuća prodata ili oduzeta dok vlasnik pomera predmet | ✅ | Pri čuvanju se proveravaju vlasnik, kuća i da li je objekat isti; ako nije, ništa se ne čuva, a novi komad se vraća među nepostavljene |
| Previše objekata u kući (klijent ima limit) | ✅ | Limit 30/50/80 + admin do 50 (najviše 130); objekti se šalju samo do 60 m |
| Novac za nameštaj dva puta (prodaja pa oduzimanje kuće) | ✅ | Prodat komad se briše odmah; pri oduzimanju se otkupljuje samo ono što je još u kući |
| Admin promeni enterijer, a nameštaj i sef ostanu u vazduhu | ✅ | Nameštaj se skida i čeka ponovno postavljanje, sef ide pored izlaza |
| Server sa starim kodom radi dok nova verzija menja bazu (ormar, visina sefa) | ⚠️ | Posle ažuriranja server se odmah restartuje; sef koji ostane metar iznad poda vlasnik postavi ponovo („Postavi ispred sebe“) |
| Vlasnik menja cenu dok kupac ima otvoren meni | ✅ | Meni pamti cenu koju je pokazao; ako se promeni, kupovina se odbija i meni se otvara ponovo |
| Vlasnik dere kupce ili prebacuje novac preko firme | ✅ | Jedna cena za celu firmu, od 50% do 200% državne; svaka prodaja troši robu koja košta 35% |
| Firme bez robe ostave grad bez hrane ili goriva | ✅ | Firma bez vlasnika je državna i uvek ima robu; pumpe van benzinske su državne; vlasnik dobija poruku kad roba padne na 10% |
| Igrač ostane zaključan u firmi | ✅ | Zaključavanje sprečava samo ulaz; izlaz uvek radi, a zatvorena firma ne prodaje nikome osim vlasniku |
| Dve benzinske dele iste pumpe | ⚠️ | Pumpa pripada prvoj benzinskoj do 50 m; admin ne pravi dve benzinske bliže od 100 m (`/finfo` pokazuje broj pumpi) |
| Firma se briše ili joj se menja tip dok su igrači unutra | ✅ | Svi iz njenog sveta se izbacuju napolje pre brisanja i promene tipa, jer se ID ponovo koristi |
| Prodaja firme sa novcem u kasi ili sa dugom | ✅ | Kasa mora biti prazna i firma bez duga (kao sef kod kuće); pri oduzimanju novac iz kase ide vlasniku na račun |
| Vlasnik izbegava porez | ✅ | Porez ide na svaku platu vlasnika (kasa, pa račun, pa dug); dug preko 1% vrednosti ili 30 dana neaktivnosti vraća firmu državi |
| Ulaznica u klub naplaćena, a igrač nije ušao | ✅ | Plaća se tek posle potvrde, i to samo ako igrač još stoji na ulazu i klub je otvoren; vlasnik i admin na dužnosti ulaze besplatno |
| Virtuelni svet firme se poklopi sa kućom ili zatvorom | ✅ | Firme su 6000 + ID (do 200), kuće 5000 + ID (do 500), zatvor 9999 |
| Ime firme sa bojama ili znakom % | ✅ | Dozvoljeni su samo obični znakovi, bez { } % i ~, 3-32 znaka; admin može da promeni ime (`/izmenifirmu`) |
| Igrač jede ili pije u restoranu u vozilu, zatvoru ili zamrznut | ✅ | Samo peške i kad nije zauzet; sit igrač ne može da jede (kao hrana iz inventara) |
| Spoljne mape firmi preklapaju postojeće mape ili pumpe | ✅ | Market kod auto pijace, market kod bolnice i picerija kod banke su blizu postojećih mapa, ali nijedan objekat nije bliže od 8 m; nova benzinska u Idlewood-u dodaje aparate pored postojećih pumpi, a mesta `pumpa` ostaju slobodna |
| Jetpack admina se prijavljuje kao brzina pešaka | ✅ | Brzina pešaka se ne proverava dok igrač ima jetpack koji mu je server dao; jetpack koji server nije dao i dalje izbacuje sa servera |
| Mapa stoji preko GTA zgrade koju je referentni mod uklanjao (treperenje, dupli objekti) | ✅ | Za svaku mapu proverena su uklanjanja iz referentnog moda i preneta u `_Remove` funkcije (kafić, Lidl, picerija) |
| Benzinska stoji preko GTA zgrade (dvostruki objekti, treperenje) | ✅ | OMV u Temple-u uklanja GTA nadstrešnicu (5898, 5853) koju zamenjuje; pumpe stanice ostaju GTA, a mesta `pumpa` za nju pravi admin |
| Admin promeni enterijer firme dok su igrači unutra | ✅ | Svi iz sveta firme se izbacuju napolje pre promene, kao kod promene tipa |
| Enterijer SECONDHAND stoji na tlu aerodroma u LV (z 10-14) | ✅ | Tako je i na referentnom modu (njihov enterijer 30); kod nas je u enterijeru 20 i ne preklapa se ni sa jednom drugom mapom |
| McDonald's uklanja ceo GTA Burger Shot u Vinewood-u (zgradu, LOD i vrata) | ✅ | Mapa gradi novu zgradu na istom mestu, a uklanjanja su preneta tačno kao u referentnom modu; ako mapa ne prođe izbor, briše se zajedno sa uklanjanjima |
| Klijent pamti uklonjene GTA zgrade dok se igra ne ugasi | ⚠️ | Posle brisanja mape koja je uklanjala zgradu, rupa ostaje dok se igra potpuno ne zatvori; ponovno povezivanje na server je ne vraća |
| Novi enterijeri u enterijeru 20 (aksesoari, Balkan Caffe) | ✅ | Proverene granice svih mapa u enterijeru 20: nijedan objekat nije bliži od 8 m drugoj mapi |
| Prodaja artikla koga firma nema | ✅ | Svaki artikal ima svoje stanje; meni pokazuje "ima N" ili "nema", a kupovina bez stanja se odbija i vlasnik dobija poruku |
| Igrač dobije više firmi nego što sme | ✅ | Kupovina i `/ponudi` proveravaju broj firmi prema 1 + dodatna mesta od admina; oduzimanje mesta ne oduzima firme |
| Igrač koristi pregled enterijera firmi | ✅ | `/enterijerifirmi` radi samo glavnom adminu na dužnosti; posmatranje i vozilo rešava isti teleport kao `/tp` |
| Sef na plafonu ili u zidu | ✅ | Sef stoji korak ispred vlasnika, uvek na površini ispod sebe (kolizije iz igre), ne sme iza zida ni u zid; postavlja se samo peške i u kući |
| Nameštaj kroz spoljni zid ili iza zida | ✅ | Zrak od igrača do sredine predmeta ne sme udariti u zid; zraci od sredine ka stranama ne smeju udariti zid pre 15 cm od ivice |
| Nameštaj lebdi ili pravi stepenice | ✅ | Predmeti za pod uvek uspravni i padaju na pod ili nameštaj ispod; viseći (police, svetla, dekoracija) samo uz zid ili plafon |
| Predmet ostane da lebdi kad se pomeri ili proda sto ispod njega | ⚠️ | Za sada se ne spušta sam; vlasnik ga pomeri ponovo (u BACKLOG-u) |
| Nema fajla sa kolizijama (`ColAndreas.cadb`) | ⚠️ | Server piše upozorenje u log, a nameštaj i sef rade bez provera zida i poda. Ime mora biti tačno `ColAndreas.cadb` (sa pogrešnim imenom provere nisu radile), a fajl se čita samo pri paljenju celog servera: posle dodavanja fajla ne pomaže restart gamemode-a. Upozorenje u logu to kaže |
| Tuđ nameštaj (isti enterijer, drugi svet) utiče na proveru | ✅ | Nameštaj se ne dodaje u ColAndreas; računa se samo nameštaj iste kuće |
| Provera ne vidi ugrađen nameštaj enterijera kao smetnju pa odbije mesto | ⚠️ | Moguće kad je pult ili zid između igrača i predmeta; poruka kaže da se pomeri ili pridje |
| Nameštaj ili sef zatvori izlaz iz kuće | ✅ | Ništa se ne čuva bliže od 2,5 m izlazu; posle promene enterijera sef ide nekoliko koraka od izlaza |
| Besplatan sef za sledećeg kupca kuće od države | ✅ | Država otkupljuje sef i nameštaj za 50% prethodnom vlasniku, a kuća se prodaje bez njih |
| Probna boja ili paintjob kod mehaničara ostane na vozilu bez plaćanja | ✅ | Posle odbijanja ili izlaska iz menija vraćaju se zajedno paintjob i boja iz baze, jer klijent inače zadrži probu |
| `/resetvozila` vrati tuđ iznajmljen rent | ✅ | Iznajmljen rent se preskače; vraćaju se samo prazni i slobodni |
| Vozilo neaktivnog vlasnika: novac za vozilo dobijeno od admina ili duplo | ✅ | Vraća se 50% cene koja je stvarno plaćena (pijaca, `/ponudi`); vozilo od admina $0; red se briše odmah, uz zapis u `vehicle_trades` |
| Vozilo obrisano dok je još u svetu (server radi duže od 30 dana) | ✅ | Pre brisanja iz baze vozilo se skida sa mape |
| Neon cevi ostanu da vise posle brisanja vozila | ✅ | Neon se skida pre svakog brisanja i ponovnog pravljenja ličnog vozila |
| Besplatan neon probom u tjuningu | ✅ | Proba se vraća kao i ostali delovi; plaća se drugim izborom |
| `/cw` kao skriveni chat van vozila | ✅ | Radi samo u vozilu i čuju ga samo oni u istom vozilu; važe mute i spam pravila |
| Natpis "popravka..." ostane na ekranu posle prekida | ✅ | Pri prekidu se natpis skida vozaču |
| Servis slučajno pokrenut ili skuplji nego što je igrač mislio | ✅ | Dijalog sa cenom, stanjem i trajanjem; plaća se tek posle potvrde i završenog servisa, i proverava se sve ponovo |
| Vozilo posla besplatno servisira neko ko ne radi taj posao | ✅ | Besplatno samo za radnika tog posla na smeni (isti ključ kao za paljenje) |
| Mehaničar prebačen u zid kad ga popravka namesti ispred haube | ✅ | Mora sam da stoji do 1,5 m od haube; pomera se samo za taj korak |
| Letelica se servisira u luci ili brod na aerodromu | ✅ | Svaka vrsta ima svoje mesto i svoju poruku gde se servisira |
| Deo za probu ostane na vozilu bez plaćanja | ✅ | Proba se vraća pri izlasku iz liste, izboru drugog dela, zatvaranju menija, izlasku iz vozila, smrti i izlasku sa servera; isto za paintjob i boju |
| Paljenje motora brzim pritiskanjem N | ✅ | Posle neuspelog paljenja motor se 3 s ne može paliti; šansa zavisi od stanja (10–40%) |
| Letelica ili brod se ugasi u pokretu | ✅ | Motor letelica i brodova se nikad ne gasi u pokretu; teško pali samo dok stoje |
| Vozilo posla prođe kroz kameru bez kazne | ✅ | Kazna stiže vozaču |
| `/vratirent` kao besplatan teleport vozila nazad | ✅ | Radi samo iz iznajmljenog vozila; vozilo se vraća na svoje mesto, a igrač ostaje gde je |
| Mehaničar popravlja sam sebi | ✅ | `/popravi` i `/tjuning` ne mogu na sebe; servis radi samo kad nema mehaničara na smeni |
| Neko vozi vozilo posla bez posla | ✅ | Vozača bez smene izbaci iz vozila; ključ (motor) ima samo radnik na smeni; posle smene ili otkaza izlazi iz vozila |
| Rent vozilo kao besplatno vozilo zauvek | ✅ | Najam najviše 3 sata unapred; vraća se kad istekne, na izlazak, uništenje ili `/vratirent`; slobodno vozilo odgurano sa mesta vraća se posle 10 minuta |
| Rent vozilo da se prođe kamera bez kazne | ✅ | Kazna stiže onome ko je iznajmio |
| Rent vozilo u gepek, prodaju ili tjuning | ✅ | Ti sistemi rade samo sa ličnim vozilima |
| Tjuning uz mehaničara bez plaćanja | ✅ | Deo se prvo proba, a plaća se drugim izborom pre nego što ostane na vozilu; skidanje je besplatno; meni se zatvara kad mehaničar ode ili završi smenu |
| Natpis vozila ostane da visi posle brisanja vozila | ✅ | Natpis se briše pre svakog brisanja admin, rent i vozila za ispit |
| Igrač zaglavi vozilo na 0% i ne može da ga pomeri | ✅ | Šlep mehaničara ili `/respawnveh`; lično vozilo se čuva sa najmanje 300 HP |
| Poziv 555 preduži red u chatu | ✅ | Opis je najviše 64 slova, pa red sa imenom i kvartom staje u 144 znaka |
| Admin vozilo ili vozilo za ispit kažnjeno kamerom | ✅ | Kamere snimaju lična, rent i vozila poslova |
| Dozvola bez ispita | 👁 | Samo ribolov se kupuje; ostalo samo posle teorije i vožnje ili od admina nivoa 4 (log) |
| Admin daje dodatna mesta za vozila prijateljima | 👁 | Samo nivo 4, najviše 5 mesta, svaka promena ide u admin log |
| Admin ostane bez kontrola ili sa zaglavljenom kamerom posle `/atune` | ✅ | Kamera i kontrole se vraćaju pri zatvaranju menija, izlasku iz vozila, smrti i izlasku sa servera; admin koga je drugi admin zamrznuo (`/freeze`) ostaje zamrznut |
| Admin sopstvenom tablicom uzme tuđu ili uvredljivu tablicu | ✅ | Ista pravila i provera zauzetosti kao za igrača (i sačuvane tablice); svaka promena ide u admin log |
| `/resetvozila` izbaci nekoga ili obriše admin vozilo u upotrebi | ✅ | Pre reseta svi dobijaju poruku i vreme da sednu u vozilo; vozila u kojima neko sedi se ne diraju |
| Admin premesti vozilo u enterijer ili drugi svet (`/getveh`) | ✅ | Parking se ne menja; `/respawnveh` i reset vraćaju vozilo napolje, u svet 0 |
| Kopije ključa se dele na sve strane | ✅ | Postoji samo jedna kopija; daje je samo vlasnik, igraču pored sebe |
| Besplatna popravka uništenog vozila | ✅ | Vraća se na parking sa 500 HP; `/resetvozila` pomera vozilo bez popravke |
| Vozilo sačuvano kao zapaljeno, pa eksplodira odmah po pojavljivanju | ✅ | HP se čuva najmanje 300 |
| Kratak reconnect vrati vozilo na parking kao besplatan teleport | ✅ | Vozilo koje je još u svetu ostaje gde je |
| Gorivo bez plaćanja: relog ili restart | ✅ | Gorivo se čuva pri odjavi, nestanku vozila, točenju, zaključavanju i restartu |
| Točenje goriva iz daljine | ✅ | Mora se sedeti za volanom do 6 m od pumpe, uz ugašen motor |
| Rezervoar se nikad ne napuni do kraja (piše 54/55, a „Rezervoar je pun“) | ✅ | Toči se do punog, a započet litar se plaća kao ceo |
| Parkiranje vozila u enterijeru ili ispred vrata da smeta drugima | ✅ | U enterijeru ne može; ispred vrata, šaltera i ostalih mesta `/v parkiraj` odbija (25 m); na putu admin može `/respawnveh`, `/resetvozila` ili `/obrisivozilo` |
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
| An admin leaves jetpack or god mode on off duty, or anti-cheat flags it | ✅ | Both powers work only on duty (the owner also off duty) and end with duty or when the level drops below 5; anti-cheat never kicks a level 5+ admin for a jetpack (in the air the client is late), it only takes the jetpack when the power is off; 99999 HP is never saved on logout |
| A jetpack taken off with F stays on the ground (the game itself leaves it, the server cannot delete it) and the admin picks it up later; `/jetpack` while falling says "you got it" but no jetpack arrives | ✅ | F ends the power, so a picked-up jetpack is taken at once; taking it off with `/jetpack` clears the animation and leaves nothing on the ground; 1.5 s after giving it the server checks that the jetpack arrived and, if not, ends the power and tells the admin to try on the ground |

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
| A player who joins inside a house cannot leave for 30 s: while the interior loads they drop a little and the game reports damage | ✅ | Fall and drowning damage with no attacker does not count as a fight; damage from another player, a vehicle or an explosion still does |
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
| Someone drives off in another player's unlocked vehicle | ✅ | A switched-off vehicle cannot be started without a key; a running one can be driven by anyone (the key is in the ignition), but once switched off it cannot be restarted without a key. The owner locks it (Y) and finds it through `/v` → GPS |
| A vehicle blocks a road while its owner is offline | 👁 | Vehicles are always in the world; an admin moves it with `/respawnveh` or `/resetvozila` (no-parking spots are in the backlog) |
| Vehicles of inactive players fill the 2000-vehicle limit | ✅ | At most 1000 personal vehicles, and vehicles of accounts that have not logged in for 14 days are not spawned on startup; while the server runs without a restart they stay |
| A key-copy holder drives the vehicle while the owner is offline | ✅ | Allowed by the owner's decision; the owner takes the copy back any time in `/v` |
| Spamming N while the engine starts, or leaving the vehicle during the start | ✅ | N does nothing while starting; after 1.5 s the player must still be the driver of the same vehicle |
| Registering without paying, or twice for one price | ✅ | Money is taken only at the end of the inspection, after checking again (driver, own vehicle, place, engine off, health); a second `/registruj` during the inspection does nothing; renewal only once fewer than 15 days are left |
| Two identical plates at registration | ✅ | The plate is checked in the DB before it is written; vehicles without plates hold NULL, so they never clash |
| Plates for an unregistered vehicle, or two plates at once | ✅ | The police issue plates only to a registered vehicle, and new ones only once the issued ones are mounted; the check is repeated after the dialog choice |
| A custom plate with offensive text or colours | ✅ | Only letters without Serbian ones, digits, space and dash (no `{}` or `~`); an admin replaces it with `/resettablice` |
| The same plate issued twice | ✅ | Checked among mounted and issued plates, and both columns are unique in the DB |
| A camera fines the same vehicle several times while passing | ✅ | One camera fines one vehicle at most once in 10 minutes; 5 km/h tolerance |
| A fine for driving someone else's vehicle goes to the owner, not the driver | 👁 | On purpose, as in real life; the owner does not hand out keys or running vehicles to strangers |
| Fines are ignored | ✅ | Unpaid fines block registration; police see them later (backlog) |
| Paying fines that arrived after the list was opened | ✅ | Exactly what the list showed is paid (up to the last ID on it) |
| A camera stands on the road and gets in the way | 👁 | The starting cameras are placed in advance; the owner checks in game and moves them (`/obrisikameru`, `/napravikameru`) |
| Bought vehicles pile up at the market exit and explode after a restart | ✅ | Buying does not work while the exit is taken; a bought vehicle's parking spot becomes the first place the owner leaves it away from the market, and parking at the exit is not allowed |
| Someone drives off with or destroys a display vehicle | ✅ | Display vehicles are locked, a driver is thrown out, and a destroyed one comes back new after 5 s; pushing is in the backlog |
| Paying the market partly in cash, partly from the account, or buying a sold-out vehicle | ✅ | The whole price from one source; pieces and distance are checked again when paying |
| A sale scam ("I paid but got no car") | ✅ | Money and vehicle change hands at the same moment, only through `/ponudi`; the buyer sees the model, plate, registration, fuel, condition and price |
| A vehicle moved to an alt account "for $1" | ✅ | At least $1,000 and a 5% tax; everything goes to `vehicle_trades` and the money log |
| An offer accepted after something changed (vehicle moved, player left, no money) | ✅ | Everything is checked again on accept; an offer lasts 60 s |
| Selling a vehicle with a key copy or with issued plates not mounted | ✅ | The copy is cancelled; until issued plates are mounted the vehicle cannot be sold |
| A VIP buys an extra vehicle and VIP ends | 👁 | The vehicle stays (owner's decision); no new one while at the limit |
| A menu runs a command the admin has no right to | ✅ | The menu shows only options up to the admin's level, and the command checks the level anyway |
| An admin gives away items, money or repairs | 👁 | Everything is by level (items and vehicle edits level 4, money the owner) and logged |
| Inspecting a damaged vehicle while there is no mechanic | 👁 | The requirement is 650 HP (VIP `/vfix` and admin `/fixveh` repair); it goes up once the mechanic exists (backlog) |
| New plates do not show until the vehicle is created again | ✅ | The vehicle is created again on the same spot with the same state, and passengers are put back in their seats (the owner checks in game) |
| An admin tunes friends' vehicles for free | ✅ | Admins of level 1 and 2 tune only admin vehicles; personal vehicles need level 3+, logged and the owner is told |
| Free tuning, paint and repair in GTA shops (Transfender, Pay'n'spray) | ✅ | The shops are closed with a gate; if someone gets in anyway (lag or a teleport), a personal vehicle goes back to its saved look and nobody else sees the change |
| Two players take the same trunk item at once (duplication) | ✅ | Every action checks the item and amount again when it happens; the other player is told it is gone |
| Theft from a trunk | 👁 | Only possible while the trunk is open and the vehicle unlocked (RP); locking protects it; every action shows in chat and goes to `trunk_log` |
| A sale scam (the seller empties the trunk after the offer) | ✅ | The buyer sees the contents in the offer; any trunk change after sending cancels the offer |
| A worn item goes into the trunk but stays on the body | ✅ | Glasses, hats, backpacks and bags must be taken off first |
| Free fuel with a jerry can | ✅ | Filled only at a pump at the same price per litre; pouring takes 5 s and is checked again (vehicle close, engine off, can still held) |
| A jerry can on a vehicle without fuel (admin vehicles, bicycles) | ✅ | Refused: it pours only into a vehicle that uses fuel |
| Two part-full cans merge into one (a can is lost) | ✅ | A can with fuel never merges; it is always given, dropped and put in a trunk as a whole |
| A player gets in a vehicle while pouring and is thrown out a few seconds later | ✅ | Any key stops the pouring right away; the animation is cleared only while on foot |
| A locked vehicle's trunk is opened and emptied | ✅ | Locking shuts the bonnet and trunk, and they cannot be opened while it is locked |
| Another player takes the test vehicle | ✅ | Anyone not on the test is removed from it; the vehicle is destroyed when the test ends |
| The test vehicle as a free vehicle (disconnect, death, jail) | ✅ | The test fails and the vehicle is destroyed; the player goes back to the counter |
| Skipping the test by teleport or shortcut | ✅ | A checkpoint counts only for the driver of the test vehicle, in order; time is limited |
| GPS and the test share a marker | ✅ | The test uses a race checkpoint, and the GPS is stopped when the drive starts |
| The test vehicle spawns inside an object in the lot | ✅ | There are no objects in the lot |
| The player is not seated after a far teleport and fails for time out of the vehicle | ✅ | They are put in only once the client has loaded the vehicle; time out of it counts only after they sit in (or after 8 s, when they can get in by hand) |
| The test helicopter's rotor hits an object or a player next to it | ✅ | It starts on open tarmac with no object within 22 m; the engine starts only when the player sits in |
| The boat route leads over land or through the pier | ✅ | Every point and leg checked against the GTA geometry; the route passes south of the pier's end |
| A chat message longer than 144 characters is not shown | ✅ | A camera sends each fine on its own line; new messages with variable parts are checked at their longest |
| Shooting at a vehicle or scraping it hurts passengers without a belt | ✅ | A crash counts only for a loss of 50+ HP at once at 30+ km/h |
| A player thrown through the windscreen gets stuck in a wall or flagged by anti-cheat | ✅ | They are set above the vehicle's roof and fly forward; the teleport and movement are announced to the anti-cheat |
| The belt stays on after leaving, dying or moving to another vehicle | ✅ | The belt comes off on every change of the player's state |
| The helmet lent for the test stays with the player or goes into the inventory | ✅ | It is not an item; it comes off when the test ends (pass, fail, death, disconnect) and whatever was on the head goes back |
| A helmet and a hat on the head at once | ✅ | The helmet is a head accessory, so it replaces a hat or cap |
| The helmet is changed in a vehicle to get around a camera or test check | ✅ | The camera and the test look at the moment the player passes; putting a helmet on while riding is allowed and is no offence |
| A mechanic tows someone's working vehicle away | ✅ | Only vehicles below 20% or out of fuel are towed, and only with nobody at the wheel |
| A mechanic charges for a repair that was not done | ✅ | The money is taken only when the repair is finished and the mechanic is still next to the vehicle |
| Friends repair each other for $50 and nobody uses the service | ✅ | The price is fixed per percent ($20 in the field, $15 in the workshop, $50 per tyre); the mechanic gets 60%, and 40% are parts that leave the game |
| An offer, then a crash before accepting (a cheap repair of heavy damage) | ✅ | On accepting, the condition and the number of popped tyres must match the offer; otherwise a new offer is needed |
| The mechanic breaks the animation or walks off mid-repair | ✅ | Frozen while the repair runs; it is broken off if the mechanic gets hurt, ends the shift, the driver leaves or the vehicle moves more than 1.5 m; payment only at the end |
| The mechanic stays frozen after a repair | ✅ | Unfrozen at the end and on every break-off, unless an admin froze them (`/freeze`) |
| The field used instead of the workshop | ✅ | The field goes to 60% at most and no body work; everything above only in the workshop (aircraft and boats at the aircraft and boat service) |
| Tuning on the street next to the workshop | ✅ | Only while the vehicle stands on the `tjuning` spot (6 m) |
| A workshop repair on the road next to it | ✅ | The service is only 8 m around a `servis` or `pregledservis` point; a `radionica` point is only for `/oprema` |
| Repair hack (a cheat repairs the vehicle without a mechanic) | ✅ | Health and popped tyres go back to the last known state, admins get an alert and an AC log entry. Dents, doors and lights are not checked: after a repair the client reports them its own way (false alerts), and they are only looks |
| Anti-cheat flags a server repair | ✅ | Every server repair and new vehicle goes through the `AC_*` functions, and the client gets 3 s to show the change; for 30 s more, a vehicle that comes back up to what the server set after a late report is accepted |
| A vehicle parked in front of an entrance, counter or rental spot gets in the way after restarts | ✅ | `/v parkiraj` refuses spots closer than 40 m (10 m from someone else's house); on roads admins handle it, later the police |
| A house sold while vehicles are in its garage (the buyer could not take them out) | ✅ | Selling to the state and through `/ponudi` needs an empty garage; when an admin or inactivity takes the house, the vehicles go out to their parking spots |
| A vehicle in a garage sold or traded with `/ponudi` | ✅ | A stored vehicle is not in the world, so it cannot be offered |
| Piling vehicles into a garage beyond the limit | ✅ | A stored vehicle still counts towards the vehicle limit |
| Hiding in an empty house (viewing a house for sale) | ✅ | A view lasts at most 5 min, then the same house waits 5 min; entry follows the door rules (combat, jail, freeze); when someone buys it, visitors leave at once; whoever was inside when the house went back on sale gets the same 5 min |
| A house as an escape from a fight or jail | ✅ | Same rules as doors: no entry for 30 s after damage, jail and freezing block it |
| Buying more houses with VIP, then VIP ends | ✅ | The houses stay, no new one while at the limit |
| An inactive owner keeps a house forever | ✅ | After 30 days it goes on sale with 50% to the bank |
| Spawning at home as an escape from jail | ✅ | Jail comes first; the house must still be the player's |
| Leaving and rejoining as a teleport home | ✅ | The house applies on login only after 10 min away |
| Death as a teleport home | ✅ | After death always the hospital |
| The icon of someone's house stays for the next player with the same playerid | ✅ | The owner's icon is removed the moment they leave and made only after the login (someone who only types the owner's name never sees it) |
| A garage vehicle's ID is given to another vehicle | ✅ | In the world an ID always belongs to the vehicle that has it now; of two garage vehicles only the latest keeps it |
| A deleted id given out again while someone is still in the deleted house's world | ✅ | Deleting moves everyone inside out in front of the door |
| Garage parking beyond the number of places | ✅ | A garage can be parking for at most as many vehicles as it holds; when it is full the vehicle stands in front of it |
| Selling a house while a vehicle parks in its garage | ✅ | The sale is refused until the parking is changed |
| Safe: two players or a visitor take the same things, or the safe stays open after leaving | ✅ | Only the owner opens it, on foot, within 1.5 m; every menu answer checks the place, the owner and that the item is still there |
| Safe: copying money or items (a crash between two writes) | ✅ | Taken from one side before the other gets it, and both are written at once; every move goes to `house_safe_log` |
| A safe "sold" with the house or lost without the owner knowing | ✅ | The house is not sold (to the state or a player) while the safe is not empty; when an admin or inactivity takes the house, the money goes to the owner's bank (offline too) and the items are lost; admins see and empty the safe in `/izmenikucu` (logged) |
| F at the safe opens it instead of the door | ✅ | The exit comes first; `/udji` and `/izadji` never open the safe |
| Rent: a room free until the first payday, or the rent changed while the player confirms | ✅ | The first rent is paid at once; if the rent changes between the question and the answer, a new confirmation is asked |
| Rent: a tenant avoids paying (empty account, AFK) | ✅ | Rent comes with the payday the tenant gets: bank, then cash; without money the tenant moves out; no payday (AFK) means no rent, but no pay either |
| A tenant abuses the house (safe, garage, lock, sale) | ✅ | A tenant only enters (also when locked), spawns there and uses their own wardrobe; everything else needs the owner |
| A tenant stays after a change of owner or after quitting | ✅ | Every change of owner evicts all tenants (offline ones in the database); 30 days without logging in evicts; the room spawn goes with the room |
| The owner evicts a tenant right after payday and keeps the rent | ✅ | On eviction the owner gives back the last rent paid; without the money the eviction is refused; if the tenant already left, nothing is paid |
| A player holds a house and a room (two free spawns) | ✅ | A room is not rented to a player with a house; buying or receiving a house ends the room |
| Clothes: copying a bag or a VIP skin as a bag to sell | ✅ | The clothes taken off go into the same bag or the same wardrobe place; VIP skins, uniforms, admin and special skins never become a bag (dropped with a message) |
| Clothes: changing under a uniform or the admin skin, or the other gender's clothes | ✅ | Only without a shift and admin duty, on foot; only clothes for the player's gender can be worn |
| The wardrobe as free storage without a house | ✅ | It works only in a house or room where the player lives, and each house has its own; when the player loses a home the outfits go to the stash, which opens only from the wardrobe of some home; capacity 3/5/8 |
| Two houses share a wardrobe, or the previous owner's and tenants' things stay for the new owner | ✅ | The wardrobe belongs to the house and the resident; a house is sold only with the owner's wardrobe empty; wardrobes of people who no longer live there go to the stash |
| Taking from the stash twice (double click, two windows) | ✅ | The row is deleted in the database first and the item is given only when the delete succeeded; if the inventory filled up meanwhile, the row is put back |
| A wardrobe stays loaded after moving out, so old outfits are taken again | ✅ | After every move to the stash all loaded wardrobes are dropped from memory and read again from the database |
| Furniture or the safe as a way out of the interior or as a wall in the house | ✅ | Only the owner places, in their own house; an object must stay within 35 m of the exit, at the interior's height and within 10 m of the player; everything is checked again on saving |
| The house is sold or taken while the owner is moving an object | ✅ | Saving checks the owner, the house and that the object is the same; otherwise nothing is saved and a new piece goes back to the unplaced ones |
| Too many objects in a house (the client has a limit) | ✅ | Limit 30/50/80 + admin up to 50 (130 at most); objects are sent only within 60 m |
| Furniture money twice (sold, then the house taken) | ✅ | A sold piece is deleted at once; taking the house buys back only what is still in it |
| An admin changes the interior and furniture and the safe hang in the air | ✅ | Furniture is taken down and waits to be placed again, the safe goes next to the exit |
| A server with the old code runs while the new version changes the database (wardrobe, safe height) | ⚠️ | Restart the server right after updating; a safe left a metre above the floor is placed again by the owner ("Postavi ispred sebe") |
| The owner changes the price while a customer has the menu open | ✅ | The menu remembers the price it showed; if it changes, the purchase is refused and the menu is opened again |
| An owner robs customers or moves money through a business | ✅ | One price for the whole business, from 50% to 200% of the state's; every sale uses stock that costs 35% |
| Businesses without stock leave the city without food or fuel | ✅ | A business without an owner is the state's and always has stock; pumps outside a station are the state's; the owner is told when stock falls to 10% |
| A player is locked inside a business | ✅ | Locking only stops entering; the exit always works, and a closed business sells to nobody but its owner |
| Two gas stations share the same pumps | ⚠️ | A pump belongs to the first station within 50 m; admins do not place two stations closer than 100 m (`/finfo` shows the pump count) |
| A business is deleted or its type changed while players are inside | ✅ | Everyone in its world is put outside before the delete and the type change, since the ID is reused |
| Selling a business with money in the till or a debt | ✅ | The till must be empty and the business without debt (like the safe for a house); when it is taken, the till goes to the owner's bank |
| The owner dodges the tax | ✅ | The tax comes on every payday of the owner (till, then bank, then debt); a debt over 1% of the value or 30 days away returns it to the state |
| The club fee is charged but the player does not get in | ✅ | It is paid only after the confirmation, and only if the player still stands at the entrance and the club is open; the owner and admins on duty get in free |
| A business's virtual world clashes with a house or the jail | ✅ | Businesses are 6000 + ID (up to 200), houses 5000 + ID (up to 500), the jail 9999 |
| A business name with colours or a % sign | ✅ | Only plain characters, no { } % or ~, 3-32 characters; an admin can change the name (`/izmenifirmu`) |
| A player eats or drinks in a restaurant in a vehicle, in jail or frozen | ✅ | Only on foot and when not busy; a full player cannot eat (like food from the inventory) |
| Business exterior maps overlap existing maps or pumps | ✅ | The market by the car market, the market by the hospital and the pizzeria by the bank are close to existing maps, but no object is within 8 m; the new Idlewood gas station adds pumps next to the existing ones, and the `pumpa` points stay clear |
| An admin's jetpack is reported as running speed | ✅ | Running speed is not checked while the player has a jetpack the server gave; a jetpack the server did not give still kicks |
| A map stands over a GTA building the reference mod removed (flickering, doubled objects) | ✅ | Every map's removals from the reference mod were checked and moved into `_Remove` functions (cafe, Lidl, pizzeria) |
| A gas station stands over a GTA building (doubled objects, flickering) | ✅ | The OMV in Temple removes the GTA canopy (5898, 5853) it replaces; the station's pumps stay the game's, and an admin makes its `pumpa` points |
| An admin changes a business's interior while players are inside | ✅ | Everyone in the business's world is put outside before the change, as when the type changes |
| The SECONDHAND interior stands on the LV airport ground (z 10-14) | ✅ | The same as in the reference mod (their interior 30); here it is in interior 20 and overlaps no other map |
| The McDonald's removes the whole GTA Burger Shot in Vinewood (building, LOD and door) | ✅ | The map builds a new building in the same place and the removals are ported exactly as in the reference mod; if the map is not picked, it is deleted together with its removals |
| The client keeps removed GTA buildings until the game is closed | ⚠️ | After a map that removed a building is deleted, the hole stays until the game is fully closed; reconnecting to the server does not bring it back |
| New interiors in interior 20 (accessories, Balkan Caffe) | ✅ | The bounds of every interior-20 map are checked: no object is within 8 m of another map |
| Selling an item the business does not have | ✅ | Each item has its own stock; the menu shows "ima N" or "nema", a purchase without stock is refused and the owner is told |
| A player gets more businesses than allowed | ✅ | Buying and `/ponudi` check the count against 1 + the admin's extra places; taking places back takes no business away |
| A player uses the business interior preview | ✅ | `/enterijerifirmi` works only for a head admin on duty; spectating and vehicles are handled by the same teleport as `/tp` |
| The safe on the ceiling or in a wall | ✅ | The safe stands a step in front of the owner, always on the surface below it (the game's collision), never behind or in a wall; placed only on foot and inside the house |
| Furniture through the outer wall or behind a wall | ✅ | A ray from the player to the piece's middle must not hit a wall; rays from the middle to the sides must not hit one before 15 cm from the edge |
| Furniture floats or builds stairs | ✅ | Standing pieces stay upright and drop onto the floor or a piece below; hanging ones (shelves, lights, decoration) only against a wall or the ceiling |
| A piece is left floating when the table under it is moved or sold | ⚠️ | Not dropped by itself yet; the owner moves it again (in BACKLOG) |
| No collision file (`ColAndreas.cadb`) | ⚠️ | The server logs a warning, and furniture and the safe work without wall and floor checks. The name must be exactly `ColAndreas.cadb` (with a wrong name the checks did not run), and the file is read only when the whole server starts: a gamemode restart does not pick up a file added later. The log warning says so |
| Other houses' furniture (same interior, other world) affects the check | ✅ | Furniture is not added to ColAndreas; only the same house's furniture counts |
| The interior's built-in furniture blocks the check and a spot is refused | ⚠️ | Possible when a counter or wall stands between the player and the piece; the message says to move or come closer |
| Furniture or the safe closes the house exit | ✅ | Nothing is saved closer than 2.5 m to the exit; after an interior change the safe goes a few steps from the exit |
| A free safe for the next buyer of a house from the state | ✅ | The state buys back the safe and furniture for 50% from the previous owner, and the house is sold without them |
| A trial colour or paintjob at a mechanic stays on the vehicle unpaid | ✅ | After declining or leaving the menu the paintjob and colour from the database are put back together, since the client otherwise keeps the trial |
| `/resetvozila` returns someone's rented rental | ✅ | A rented rental is skipped; only empty and free ones go back |
| Inactive owner's vehicle: money for an admin-given vehicle, or paid twice | ✅ | 50% of the price actually paid comes back (market, `/ponudi`); an admin-given vehicle $0; the row is deleted at once, with a `vehicle_trades` entry |
| A vehicle deleted while still in the world (the server ran longer than 30 days) | ✅ | It is taken off the map before it leaves the database |
| Neon tubes left hanging after a vehicle is removed | ✅ | The neon is taken off before every removal and rebuild of a personal vehicle |
| Free neon through the tuning trial | ✅ | The trial is undone like other parts; it is paid with the second pick |
| `/cw` as a hidden chat outside vehicles | ✅ | Works only in a vehicle and only those in it hear it; mute and spam rules apply |
| The "popravka..." text stays on screen after a break-off | ✅ | It is removed from the driver's screen when the repair breaks off |
| The service started by accident or dearer than the player thought | ✅ | A dialog with the price, condition and duration; payment only after confirming and a finished service, and everything is checked again |
| Someone outside the job services a job vehicle for free | ✅ | Free only for that job's worker on duty (the same key as for the engine) |
| The mechanic moved into a wall when centred at the bonnet | ✅ | They must stand within 1.5 m of the bonnet themselves; the move is only that step |
| An aircraft serviced at the dock or a boat at the airport | ✅ | Each kind has its own place and its own message saying where |
| A part being tried on stays without payment | ✅ | The trial is undone on leaving the list, picking another part, closing the menu, leaving the vehicle, death and disconnect; the same for paintjobs and colours |
| Starting the engine by hammering N | ✅ | After a failed start the engine cannot be started for 3 s; the chance depends on the condition (10–40%) |
| An aircraft or boat stalls while moving | ✅ | Aircraft and boat engines never stall while moving; they start badly only while standing |
| A job vehicle passes a camera without a fine | ✅ | The fine goes to the driver |
| `/vratirent` as a free teleport back | ✅ | Works only from inside the rental; the vehicle goes back to its spot and the player stays where they are |
| A mechanic repairs their own vehicle | ✅ | `/popravi` and `/tjuning` cannot target oneself; the service works only when no mechanic is on a shift |
| Someone drives a job vehicle without the job | ✅ | A driver without a shift is thrown out; only a worker on a shift has the key (engine); after the shift or quitting they leave the vehicle |
| A rental as a free vehicle forever | ✅ | At most 3 hours ahead; it goes back when the time runs out, on disconnect, when destroyed or with `/vratirent`; a free one pushed off its spot returns after 10 minutes |
| A rental used to pass a camera without a fine | ✅ | The fine goes to whoever rented it |
| A rental in the trunk, sale or tuning systems | ✅ | Those systems work with personal vehicles only |
| Tuning at the mechanic without paying | ✅ | A part is first tried on and paid with the second pick before it stays on the vehicle; removing is free; the menu closes when the mechanic leaves or ends the shift |
| A vehicle label left hanging after the vehicle is deleted | ✅ | The label is removed before every admin, rental and test vehicle is destroyed |
| A player stuck with a vehicle at 0% that cannot move | ✅ | The mechanic's tow truck or `/respawnveh`; a personal vehicle is saved with at least 300 HP |
| A 555 call too long for the chat line | ✅ | The description is at most 64 characters, so the line with name and district fits in 144 |
| An admin or test vehicle fined by a camera | ✅ | Cameras photograph personal, rental and job vehicles |
| A license without a test | 👁 | Only fishing is bought; the rest only after theory and the drive, or from a level 4 admin (logged) |
| An admin gives friends extra vehicle slots | 👁 | Level 4 only, at most 5 slots, every change goes to the admin log |
| An admin is left without controls or with a stuck camera after `/atune` | ✅ | The camera and controls come back when the menu closes, on leaving the vehicle, on death and on disconnect; an admin frozen by another admin (`/freeze`) stays frozen |
| An admin takes someone else's plate or an offensive one with a custom plate | ✅ | The same rules and taken-plate check as for players (stored plates too); every change goes to the admin log |
| `/resetvozila` throws someone out or removes an admin vehicle in use | ✅ | Everyone gets a message and time to sit in a vehicle first; vehicles with someone inside are left alone |
| An admin moves a vehicle into an interior or another world (`/getveh`) | ✅ | The parking spot does not change; `/respawnveh` and the reset bring it back outside, into world 0 |
| Key copies passed around everywhere | ✅ | There is only one copy; only the owner gives it, to a player standing next to them |
| A free repair of a destroyed vehicle | ✅ | It comes back to its parking spot with 500 HP; `/resetvozila` moves vehicles without repairing them |
| A vehicle saved while burning explodes right after spawning | ✅ | Health is saved as at least 300 |
| A short reconnect sends the vehicle back to its spot as a free teleport | ✅ | A vehicle still in the world stays where it is |
| Free fuel through a relog or restart | ✅ | Fuel is saved on logout, despawn, refuelling, locking and restart |
| Refuelling from a distance | ✅ | The player must sit at the wheel within 6 m of a pump, with the engine off |
| The tank never fills up (shows 54/55, yet "Rezervoar je pun") | ✅ | A top-up fills to the brim, and a started litre is paid as a whole one |
| Parking in an interior or in front of a door to annoy others | ✅ | Interiors are refused; `/v parkiraj` refuses spots within 25 m of doors, counters and the other spots; on roads admins can use `/respawnveh`, `/resetvozila` or `/obrisivozilo` |
| Two identical plates | ✅ | The plate is checked in the DB before the insert, and the column is unique |
| VIPs paint or repair other people's vehicles | ✅ | `/vcolor` only on their own vehicle, `/vfix` on their own or one they hold a key for |
| The Y or N key clashes with other systems | ✅ | Y only acts when a vehicle with a key is nearby; N only for the driver of a vehicle with an engine; doors and the inventory use F |
