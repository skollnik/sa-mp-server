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
| Uređenje spawna ispred aerodroma |
| VIP klub (enterijer za Silver i Gold VIP) |
| Bolnica, auto škola, mehaničar, novinari: još nema izabrane mape |

**Posle opštine, banke i ostalih mesta (Faza 2)**
| Stavka |
|--------|
| Teleport lokacije (`/tp`): generičke lokacije se zamenjuju pravim |
| Ulazi u ubačene enterijere: opština, policija, zatvor, banka. Svi su u enterijeru 20, koordinate su u fajlovima u `gamemodes/maps/interiors/` |
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
| Vrata ćelija u zatvorskom enterijeru da se otvaraju i zatvaraju (sada su nepomična i zatvorena) |
| VIP: `/vheal` ne sme da radi dok je igrač ranjen; `/vgoto` i `/vfix` ne smeju da rade u potjeri ili sa potragom |

**Napredni anti-cheat (Faza 7)**
| Stavka |
|--------|
| Lažna smrt (fake kill), spam konekcija i botovi |
| Automatske kazne za teleport i brzinu kad se pragovi provere u praksi |
| C-bug, drive-by, spam ulaska u vozilo i promene stanja, tuning van mehaničara |

### Odobreni sistemi (izbor vlasnika 28.10.2026.)
Brojevi mapa (#) su iz atlasa mapa. Svaka stavka se pre izrade predlaže vlasniku.

**Faza 2: novac, banka, payday, telefon, inventar**
| Stavka | Kako radi i sa čim se vezuje | Mapa |
|--------|------------------------------|------|
| AFK detekcija | Ko stoji AFK duže od X minuta, tom vremenu se ne broji payday | — |
| Happy hours | Admin pokrene period sa duplom platom ili respectom; vezano za payday i poslove | — |
| Bankarski kredit | Iznos zavisi od nivoa, rata se skida na payday; ako se ne plaća, dug raste i blokira kupovine, a posle X dana se oduzima imovina | — |
| PIN za bankomat i karticu | Bez PIN-a nema isplate na bankomatu; posle 3 pogrešna PIN-a kartica se blokira na neko vreme | — |
| Lutrija | Tiket se kupuje do izvlačenja u zadato vreme; deo uplata ide u državnu kasu | — |
| Državna kasa sa donacijama | Igrači uplaćuju u budžet države (ECO-3, GOV-1), lista najvećih donatora | — |
| Prošnja (do nivoa 4) | Sitan novac od drugih igrača, ne radi kod spawna; dnevni limit po davaocu i log, da se novac ne prebacuje sa drugih naloga | — |
| Novčić i kockice uz opkladu | Oba igrača potvrde ulog, novac drži server dok traje bacanje, najveći ulog je ograničen, sve ide u log | — |
| Meni za ponude između igrača | Jedna komanda za prodaju vozila, kuće i firme: oba igrača blizu, potvrda dijalogom, log protiv prevare | — |
| Telefon: dopuna, promena broja, melodija | Dodatak na PHONE-1; kredit se troši na pozive i SMS | — |
| Dodaci na telu (naočare, kape, torbe) | Predmeti iz prodavnice u inventaru, nose se na telu, igrač sam podešava položaj | — |
| Muzika: radio u vozilu, boombox, muzika sa linka | Radio stanice u vozilu, boombox kao predmet koji svira ljudima oko njega | — |

**Faza 3 i 4: vozila, imovina, firme, poslovi**
| Stavka | Kako radi i sa čim se vezuje | Mapa |
|--------|------------------------------|------|
| Limiter brzine (`/limit`) | Vozač zada najveću brzinu | — |
| Tehnički pregled | Registracija važi X dana, produžava se na tehničkom pregledu; policija proverava | — |
| Pay'n'spray i perionica | Plaća se popravka i farbanje ili pranje vozila | Pay'n'spray postoji u GTA; perionica nema mapu |
| Drive-thru | Naručivanje hrane iz vozila na prolazu restorana brze hrane | Prolazi postoje u GTA |
| Iznajmljivanje bicikala i skutera | Punkt sa vozilima, plaća se po satu, vozilo nestaje kad istekne | Mapa sa 7 malih punktova (LS: Unity stanica, kod opštine, kod bolnice, Downtown, Conference Center; Dillimore) |
| Stanovi i vikendice | Novi tipovi imovine pored kuće (PROP-1) | Stanovi #1161; vikend naselje #0318–#0328 |
| Imanje | Kupi se parcela, sadi se voće i drveće, bere i prodaje firmama (PROP-8) | Farme #1248–#1257, #1451–#1456, #0897–#0899 |
| Nadogradnje kuće: alarm, sef, ormar | Alarm javlja vlasniku i policiji kad neko provaljuje; sef (PROP-3); ormar čuva odeću | — |
| Garaže sa enterijerom i nadogradnjom | Garaža na više mesta, sa nadogradnjom broja mesta (PROP-5) | #1118–#1125, #1580, #1876, #0841 |
| Prodavnica nameštaja sa pregledom | Pre kupovine kamera pokaže objekat (PROP-4) | Nema posebne mape |
| Kamp prikolica | Prikolica se vuče autom, a parkirana ima enterijer u koji se ulazi | Enterijeri #1367–#1369, salon prikolica #1370 |
| Hotel | Soba po danu za igrače bez kuće; može da bude mesto spawna | #1218–#1225, #1526 |
| Kripto farma | Video kartice u kući ili podrumu rudare valutu koja se menja za novac; broj kartica je ograničen, a kurs pada što je više farmi | #1348, #2106, #1835, #1650 |
| Zlatara | Kupovina i prodaja zlata po promenljivoj ceni; zlato donose ronilac i rudar | Enterijer #0407 |
| Kazino i kladionica kao biznis | Igre: blackjack, poker, rulet, kockice; kladionica na evente. Krupije kao posao samo ako se pokaže potreba, igre rade i bez njega | Kazino #0790, #1828–#1829, #1878; kladionica #1234–#1235 |
| DJ u klubu | Funkcija kluba, ne poseban posao: vlasnik ili DJ pušta muziku sa linka svima u klubu | — |
| Rudnik | Posao rudara za sve igrače; ako je rudnik firma, vlasnik dobija procenat od svake ture | #1287–#1288 |
| Poslovi: ronilac, košenje trave, lovac, poštar, utovarivač u luci, vozač autobusa po linijama | Ronilac vadi zlato za zlataru; lovac prodaje meso restoranima; poštar nosi pošiljke do kuća | Luka #0678–#0679 |
| Poslovi: građevinar, mašinovođa, prevoz starog gvožđa | Građevinar nosi materijal po gradilištu; mašinovođa vozi voz po stanicama; staro gvožđe ide na otpad | Gradilište #2128–#2130 |
| Uvodni tekst kad igrač prvi put uzme posao | Kratak prozor: šta je posao, gde se počinje, komande i plata; posle se otvara komandom (npr. `/posaoinfo`) | — |
| Kućica za čamac | Imovina na vodi, kao garaža za lični čamac: čamac stoji u njoj i tu se pojavljuje; traži dozvolu za brod (LIC-2) | Pristanište #2107–#2112, enterijer #1805 |

**Faza 5: frakcije, država, policija, hitna**
| Stavka | Kako radi i sa čim se vezuje | Mapa |
|--------|------------------------------|------|
| Medicinska karta | Izdaje je doktor iz bolnice (rang koji lider ovlasti) posle RP pregleda; statusi Zdrav, Ograničeno sposoban, Nesposoban; važi 3 meseca; uslov za dozvolu za oružje (LIC-3), ulazak u državne frakcije i neke poslove; zavisnik od droge dobija „Nesposoban“ dok se ne leči. Kad istekne, ništa se ne oduzima: karta se traži samo za novu dozvolu, frakciju ili posao. Dok bolnica nema igrače, admin sebi daje ulogu bolničara | — |
| Brak | `/propose`, venčanje sa svedokom, `/divorce`; supružnici dele ključeve kuće; prezime se ne menja, u `/stats` piše „u braku sa X“ | — |
| Izbori za gradonačelnika | Glasa se u opštini, od nivoa 3 naviše | Enterijer opštine (već ubačen) |
| Političke stranke | Stranke kandiduju ljude na izborima | — |
| Savezi između organizacija | Dve organizacije postanu saveznici (zajednički chat, bez napada na teritorije) | — |
| Zatvorska služba | Čuvari, otvaranje i zatvaranje ćelija (vezano za vrata ćelija iz Faze 5) | Zatvor (već ubačen) |
| Crna lista frakcija | Ko izađe ili bude izbačen iz državne frakcije, X dana (npr. 30) ne može da uđe u državne frakcije | — |
| Zakazani razgovori za posao | Lideri objave kad i gde frakcija prima nove članove, a igrači vide listu | — |
| Narko test | Policija testira igrača; pozitivan ako je skoro koristio drogu (CRIME-3) | — |
| Pojas i kaciga | Policija proverava pojas i kacigu i piše kaznu (LAW-5) | — |
| Policija: kazneni poeni, blokada točka, razbijanje vrata, štit, `/backup` sa markerom, praćenje poslednjeg poziva | Dodaci na LAW-2 do LAW-8 | — |
| Hitna: nosila, lečenje zavisnosti, kreveti u bolnici | Dodaci na EMS-1; lečenje zavisnosti je vezano za drogu (CRIME-3) | — |
| Lanac oružja preko državnog skladišta | Vojska vozi materijal do skladišta policije i bandi (ARMY-1, CRIME-4) | Vojne baze #0995–#0997, #1661–#1663 |
| Agencija za plaćene ubice | Ugovori se primaju preko telefona; ubistvo nije CK bez odobrenja admina | — |

**Faza 6: kriminal**
| Stavka | Kako radi i sa čim se vezuje | Mapa |
|--------|------------------------------|------|
| Pljačka banke u timu | Dogovor, poziv ekipe, C4, utovar para; minimum policajaca na dužnosti (CRIME-5) | Banka (ubačena) |
| Pljačka prodavnice sa zastrašivanjem | Iznos raste dok igrač drži prodavca na nišanu | — |
| Pljačka zlatare, kladionice i opštine | Isti sistem kao pljačka prodavnice, veći plen | Zlatara #0407, kladionica #1234 |
| Rat za biznise | Mafije se bore za „zaštitu“ firmi i dobijaju deo prihoda (CRIME-2) | — |
| Grafiti na teritorijama bandi | Banda farba grafite na svojoj teritoriji (CRIME-1) | — |
| Zakazan okršaj bandi | Napad na teritoriju se najavi, a borba počinje u zadato vreme | — |
| Prodaja teritorije drugoj bandi | Lider proda teritoriju drugoj bandi, uz potvrdu obe strane | — |
| Narko laboratorija | Pravljenje droge u laboratoriji (CRIME-3) | #0117–#0118, #0854 |
| Skrivena pošiljka droge | Kupac dobije lokaciju na kojoj je droga skrivena | — |
| Krađa vozila po porudžbini | „Donesi auto modela X“, sa markerom na radaru (CRIME-8) | — |

**Faza 7: eventi i ostalo**
| Stavka | Kako radi i sa čim se vezuje | Mapa |
|--------|------------------------------|------|
| Event sistem | Admin pokrene event, igrači se prijave sa `/join` i dobiju nagradu: trke sa checkpoint-ima, drift, derbi, dueli 1 na 1 sa statistikom, DM arena, boks meč | Trke #0459, #0490; arene #0846, #0903, #0904, #0914, #0934, #0946, #0953 |
| Teretana | Učenje stila borbe za novac | #0427, #0145, #0011, enterijer #0973 |
| Streljana i veštine oružja | Veština svakog oružja raste vežbom na streljani | #0130, #0805 |
| Sigurne zone | Oko spawna, bolnice i opštine nema štete ni izvlačenja oružja; policija na dužnosti je izuzeta | — |
| Bilbordi | Igrač plati da mu se tekst neko vreme prikazuje na bilbordu | Mapa sa 7 praznih ekrana (LS: Pershing Square i Market) |
| Tezge sa hranom i automati za piće | Kupovina hrane i pića na ulici (HEALTH-5) | GTA objekti, postavljaju se po gradu |
| Kućni detalji | Kucanje na vrata (`/knock`), interfon, lift u zgradama | — |
| Granični prelaz između gradova | Kontrola na izlazu iz grada, može da je drži policija ili vojska | #0402, #0641, #1082–#1083, #1884–#1885 |

### Potencijalno (možda, možda ne)
| Stavka |
|--------|
| Dostignuća sa nagradama (npr. 100 sati igre, 50 vožnji taksija) |
| Nedeljna nagrada za aktivnost |
| Oružje vidljivo na leđima kad nije u ruci (koristi ga dosta servera) |
| Skok padobranom kao plaćena atrakcija |
| Avio linija između gradova: pilot leti, putnici kupuju kartu |
| Aukcija kuća: kuća oduzeta zbog neplaćenog poreza ili kredita ide na licitaciju |
| Tezge na pijaci: igrač iznajmi tezgu i prodaje svoje predmete (mapa centralne pijace #1868–#1871) |
| Podrum u kući kao nadogradnja (mape #1348, #2106) |
| Inkasator: oklopnim vozilom vozi novac do banaka i bankomata, može da bude opljačkan |
| Rad u zatvoru skraćuje kaznu |
| Fudbal (mapa #1397–#1398), ankete (`/poll`), NPC pratilac, tajni admini, VIP kozmetika |
| Prijava prebivališta kao uslov za neke poslove ili kredit |
| „Najbolji radnici“ (retko se koristi) i dugovi među igračima (retko se koristi kao sistem, obično je RP) |
| Praznični eventi (novogodišnje mape i zadaci) |
| Recept od doktora za lekove (traži doktora stalno na dužnosti; ima smisla tek kad postoje droga i hitna) |

### Čeka odluku vlasnika
| Stavka |
|--------|
| `/vskin`: umesto ID-ja skina prikazati slike skinova ili nazive „VIP skin 1, 2, 3...“ (predlog poslat 27.10.2026.) |
| Faza 2: detaljan predlog poslat 27.10.2026., čeka odobrenje i odgovore na pitanja |

### Može bilo kada
| Stavka |
|--------|
| Posle `/unjail` ili isteka kazne vratiti igrača na mesto gde je bio pre zatvora (sada ide na aerodrom) |
| Opis izgleda karaktera (`/examine`), akcenti (`/accent`) |
| `/try` (nasumično „uspeo“ ili „nije uspeo“) i `/todo` (radnja i govor u jednoj poruci) |
| Izbor skina pri registraciji iz ponuđene liste po polu, umesto nasumičnog |
| `/aka`: svi nalozi sa iste IP adrese ili serijskog broja, uz istoriju promene imena |
| Aktivnost tima: odgovoreni reportovi, kazne i sati na dužnosti po adminu, sa pregledom za vlasnika |
| Selfie kamera |
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
| 28.10.2026. | Prve mape: opština spolja i parking ispred nje, ograđena auto pijaca, enterijeri opštine, policije i zatvora |
| 28.10.2026. | Izbor sistema iz liste 1–118: odobreno upisano u „Odobreni sistemi“, a „možda“ u „Potencijalno“ |
| 29.10.2026. | Banka spolja i enterijer banke, natpis BANKA na pozorištu pored banke, mape se vide i iz daljine |

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
| Decorating the spawn in front of the airport |
| VIP club (interior for Silver and Gold VIP) |
| Hospital, driving school, mechanic, news: no map picked yet |

**After city hall, bank and other places (Phase 2)**
| Item |
|------|
| Teleport locations (`/tp`): generic spots replaced by real ones |
| Entrances to the added interiors: city hall, police, prison, bank. All are in interior 20; coordinates are in the files in `gamemodes/maps/interiors/` |
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
| Cell doors in the prison interior open and close (they are now fixed and closed) |
| VIP: `/vheal` must not work while wounded; `/vgoto` and `/vfix` must not work during a chase or while wanted |

**Advanced anti-cheat (Phase 7)**
| Item |
|------|
| Fake kill, connection spam and bots |
| Automatic punishment for teleport and speed once the thresholds are proven in practice |
| C-bug, drive-by, vehicle entry and state change spam, tuning outside a mechanic |

### Approved systems (owner's pick, 2026-10-28)
Map numbers (#) are from the map atlas. Every item is proposed to the owner before it is built.

**Phase 2: money, bank, payday, phone, inventory**
| Item | How it works and what it ties into | Map |
|------|------------------------------------|-----|
| AFK detection | Time spent AFK longer than X minutes does not count toward payday | — |
| Happy hours | An admin starts a period of double pay or respect; tied to payday and jobs | — |
| Bank loan | Amount depends on level, an instalment is taken on payday; unpaid debt grows and blocks purchases, after X days property is seized | — |
| ATM and card PIN | No ATM withdrawal without the PIN; 3 wrong PINs block the card for a while | — |
| Lottery | Tickets are sold until the draw at a set time; part of the money goes to the state treasury | — |
| State treasury donations | Players pay into the state budget (ECO-3, GOV-1), with a top donors list | — |
| Begging (up to level 4) | Small money from other players, not near the spawn; daily limit per giver and a log, so money is not moved from alt accounts | — |
| Coin flip and dice bets | Both players confirm the stake, the server holds the money during the roll, the max stake is capped, everything is logged | — |
| Offer menu between players | One command to sell a vehicle, house or business: both players nearby, dialog confirmation, a log against scams | — |
| Phone: top-up, number change, ringtone | Extends PHONE-1; credit is spent on calls and SMS | — |
| Body accessories (glasses, hats, bags) | Shop items in the inventory, worn on the body, the player adjusts the position | — |
| Music: car radio, boombox, music from a link | Radio stations in vehicles, a boombox item that plays to people around it | — |

**Phases 3 and 4: vehicles, property, businesses, jobs**
| Item | How it works and what it ties into | Map |
|------|------------------------------------|-----|
| Speed limiter (`/limit`) | The driver sets a top speed | — |
| Vehicle inspection | Registration is valid for X days and renewed at the inspection; police check it | — |
| Pay'n'spray and car wash | Paid repair and respray, or a car wash | Pay'n'spray exists in GTA; no car wash map |
| Drive-thru | Ordering food from a vehicle at a fast food drive-thru | Drive-thru lanes exist in GTA |
| Bike and scooter rental | A rental point with vehicles, paid per hour, the vehicle disappears when time runs out | Map with 7 small points (LS: Unity Station, city hall, hospital, Downtown, Conference Center; Dillimore) |
| Apartments and cottages | New property types next to houses (PROP-1) | Apartments #1161; weekend settlement #0318–#0328 |
| Farm land | Buy a plot, plant fruit and trees, harvest and sell to businesses (PROP-8) | Farms #1248–#1257, #1451–#1456, #0897–#0899 |
| House upgrades: alarm, safe, wardrobe | The alarm alerts the owner and police on a break-in; safe (PROP-3); the wardrobe keeps clothes | — |
| Garages with interiors and upgrades | Garages in several places, upgradable number of slots (PROP-5) | #1118–#1125, #1580, #1876, #0841 |
| Furniture store with preview | The camera shows an object before buying (PROP-4) | No dedicated map |
| Camper trailer | The trailer is towed by a car; when parked it has an interior to enter | Interiors #1367–#1369, trailer dealer #1370 |
| Hotel | A room per day for players without a house; can be a spawn point | #1218–#1225, #1526 |
| Crypto farm | Video cards in a house or basement mine a currency exchanged for money; card count is capped, the rate drops as farms grow | #1348, #2106, #1835, #1650 |
| Jewellery store | Buying and selling gold at a changing price; gold comes from divers and miners | Interior #0407 |
| Casino and betting shop as businesses | Games: blackjack, poker, roulette, dice; bets on events. A croupier job only if it turns out to be needed, the games work without one | Casino #0790, #1828–#1829, #1878; betting #1234–#1235 |
| Club DJ | A club feature, not a separate job: the owner or DJ plays music from a link to everyone in the club | — |
| Mine | Miner job for every player; if the mine is a business, the owner gets a share of each run | #1287–#1288 |
| Jobs: diver, lawn mowing, hunter, mail carrier, port loader, bus driver on lines | The diver finds gold for the jewellery store; the hunter sells meat to restaurants; the mail carrier delivers to houses | Port #0678–#0679 |
| Jobs: construction worker, train driver, scrap metal hauler | Construction carries material on a site; the train driver runs between stations; scrap goes to the junkyard | Construction site #2128–#2130 |
| Intro text when a player takes a job for the first time | A short window: what the job is, where to start, commands and pay; reopened later with a command (e.g. `/posaoinfo`) | — |
| Boathouse | Property on the water, a garage for a personal boat: the boat is stored and spawns there; requires a boat license (LIC-2) | Pier #2107–#2112, interior #1805 |

**Phase 5: factions, state, police, EMS**
| Item | How it works and what it ties into | Map |
|------|------------------------------------|-----|
| Medical card | Issued by a hospital doctor (a rank the leader allows) after an RP exam; statuses Healthy, Limited, Unfit; valid for 3 months; required for the weapon license (LIC-3), state factions and some jobs; a drug addict gets "Unfit" until treated. On expiry nothing is taken away: the card is only checked for a new license, faction or job. While the hospital has no players, an admin gives themselves the medic role | — |
| Marriage | `/propose`, a wedding with a witness, `/divorce`; spouses share house keys; the surname does not change, `/stats` shows "married to X" | — |
| Mayor elections | Voting at city hall, from level 3 up | City hall interior (already added) |
| Political parties | Parties field candidates in elections | — |
| Alliances between organisations | Two organisations become allies (shared chat, no attacks on each other's turf) | — |
| Prison service | Guards, opening and closing cells (tied to the Phase 5 cell doors) | Prison (already added) |
| Faction blacklist | Whoever leaves or is kicked from a state faction cannot join state factions for X days (e.g. 30) | — |
| Scheduled faction interviews | Leaders post when and where the faction recruits; players see the list | — |
| Drug test | Police test a player; positive after recent drug use (CRIME-3) | — |
| Seatbelt and helmet | Police check them and write a ticket (LAW-5) | — |
| Police: license penalty points, wheel clamp, door breaching, shield, `/backup` with a marker, tracing the last call | Extends LAW-2 to LAW-8 | — |
| EMS: stretcher, addiction treatment, hospital beds | Extends EMS-1; addiction treatment ties into drugs (CRIME-3) | — |
| Weapon chain through the state warehouse | The army hauls material to police and gang warehouses (ARMY-1, CRIME-4) | Army bases #0995–#0997, #1661–#1663 |
| Hitman agency | Contracts come in by phone; a kill is not a CK without admin approval | — |

**Phase 6: crime**
| Item | How it works and what it ties into | Map |
|------|------------------------------------|-----|
| Team bank robbery | Planning, calling the crew, C4, loading the money; minimum police on duty (CRIME-5) | Bank (added) |
| Store robbery with intimidation | The amount grows while the player keeps the clerk at gunpoint | — |
| Robbing the jewellery store, betting shop and city hall | Same system as the store robbery, bigger loot | Jewellery #0407, betting #1234 |
| Business war | Mafias fight over "protection" of businesses and get a share of income (CRIME-2) | — |
| Gang graffiti on turfs | A gang sprays graffiti on its turf (CRIME-1) | — |
| Scheduled gang fight | A turf attack is announced and the fight starts at a set time | — |
| Selling a turf to another gang | A leader sells a turf to another gang, confirmed by both sides | — |
| Drug lab | Making drugs in a lab (CRIME-3) | #0117–#0118, #0854 |
| Hidden drug shipment | The buyer gets the location where the drugs are hidden | — |
| Car theft on order | "Bring a car of model X", with a marker on the radar (CRIME-8) | — |

**Phase 7: events and the rest**
| Item | How it works and what it ties into | Map |
|------|------------------------------------|-----|
| Event system | An admin starts an event, players sign up with `/join` and get rewards: checkpoint races, drift, derby, 1v1 duels with stats, DM arena, boxing match | Races #0459, #0490; arenas #0846, #0903, #0904, #0914, #0934, #0946, #0953 |
| Gym | Learning fighting styles for money | #0427, #0145, #0011, interior #0973 |
| Shooting range and weapon skills | Each weapon skill grows by practice at the range | #0130, #0805 |
| Safe zones | No damage and no drawing weapons around the spawn, hospital and city hall; police on duty are exempt | — |
| Billboards | A player pays to show text on a billboard for a while | Map with 7 blank screens (LS: Pershing Square and Market) |
| Food stands and drink machines | Buying food and drinks on the street (HEALTH-5) | GTA objects placed around the city |
| House details | Knocking (`/knock`), intercom, elevators in buildings | — |
| Border crossing between cities | A checkpoint at the city exit, can be manned by police or army | #0402, #0641, #1082–#1083, #1884–#1885 |

### Potential (maybe, maybe not)
| Item |
|------|
| Achievements with rewards (e.g. 100 hours played, 50 taxi rides) |
| Weekly activity reward |
| Weapons visible on the back when not in hand (many servers use it) |
| Paid skydiving attraction |
| Airline between cities: a pilot flies, passengers buy tickets |
| House auction: a house seized for unpaid tax or loan goes to auction |
| Market stalls: a player rents a stall and sells their items (central market map #1868–#1871) |
| Basement as a house upgrade (maps #1348, #2106) |
| Cash-in-transit: an armoured vehicle carries money to banks and ATMs and can be robbed |
| Prison work shortens the sentence |
| Football (map #1397–#1398), polls (`/poll`), NPC companion, hidden admins, VIP cosmetics |
| Residence registration as a requirement for some jobs or a loan |
| "Best workers" (rarely used) and debts between players (rarely a system, usually RP) |
| Holiday events (New Year maps and tasks) |
| Doctor's prescription for medicine (needs a doctor always on duty; only makes sense once drugs and EMS exist) |

### Waiting for the owner's decision
| Item |
|------|
| `/vskin`: show skin pictures or "VIP skin 1, 2, 3..." instead of skin IDs (proposal sent 2026-10-27) |
| Phase 2: detailed proposal sent 2026-10-27, waiting for approval and answers |

### Any time
| Item |
|------|
| After `/unjail` or when the sentence ends, return the player to where they were before jail (now the airport) |
| Character appearance description (`/examine`), accents (`/accent`) |
| `/try` (randomly "succeeded" or "failed") and `/todo` (action and speech in one message) |
| Picking a skin at registration from a set list per gender instead of a random one |
| `/aka`: every account from the same IP address or serial, with name change history |
| Staff activity: answered reports, punishments and duty hours per admin, with an overview for the owner |
| Selfie camera |
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
| 2026-10-28 | First maps: city hall exterior and the parking in front of it, fenced car market, interiors of the city hall, police and prison |
| 2026-10-28 | Pick from the 1–118 list: approved items written under "Approved systems", "maybe" items under "Potential" |
| 2026-10-29 | Bank exterior and interior, BANKA on the theatre next to the bank, maps visible from a distance |
