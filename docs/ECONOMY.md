# Ekonomija / Economy

## 🇷🇸 Srpski

### Principi
- **Cene su vezane za stvarni život** (američke cene u $). Igrač tako intuitivno zna šta je jeftino, a šta skupo.
- **Svaka cena se meri satima rada.** Osnova je prosečna zarada od ~$1.000–1.500 po satu aktivnog rada.
- **Vozila su retka.** Nema stalnog salona. Admin povremeno otvara auto pijacu sa **novim** vozilima u ograničenom broju (npr. 1–2 komada od svakog modela). Ko kupi, kupio je. Posle toga vozila menjaju vlasnika samo **trgovinom između igrača**, po ceni oko koje se dogovore.
- **Novac mora i da izlazi iz igre** (porezi, gorivo, popravke, kazne, takse), inače dolazi do inflacije.
- Ovo je početni predlog. Svi iznosi će u kodu stajati na jednom mestu, a balansiraćemo ih tokom testiranja. Ako ukupan novac na serveru naglo raste, dodajemo troškove.

### Zarada
| Izvor | Iznos |
|-------|-------|
| Jednostavni poslovi (dostava, smetlar, ribar...) | ~$800–1.200 po satu |
| Stručni poslovi (mehaničar, kamiondžija...) | ~$1.200–2.000 po satu |
| Plata u legalnoj frakciji (payday) | $1.000 (najniži rank) → $3.000 (lider) |
| Socijalna pomoć, nezaposleni (payday) | $100 |
| Kamata na banku (payday) | 0.05% stanja, najviše $250 |
| Payday | Na svaki pun sat, ako je igrač od prošle plate bio aktivan bar 30 min; uplata ide na račun |
| Transfer na drugi račun | Od nivoa 2, provizija 1%; bankomat isplaćuje najviše $10.000 odjednom |
| Opklada (kockice, novčić) | Ulog $10–$5.000 po igraču, od nivoa 2; pobednik uzima oba uloga, nerešeno vraća uloge |
| Prošnja | Do nivoa 4; jedno davanje $1–$100, jedan davalac najviše $100 dnevno istom prosjaku, prosjak najviše $1.000 dnevno |
| Lutrija | Tiket $50, broj 1–50, jedan tiket po igraču; izvlačenje svakog sata; 70% ide u fond, 30% nestaje iz igre (kasnije u državnu kasu) |

### Startni paket
- **$2.000 keš + $3.000 koji čekaju na šalteru banke = $5.000.** Novi igrač nema račun: $3.000 podiže na šalteru ili mu prelaze na račun kad ga otvori ($100, uz ličnu kartu). To pokriva vozačku, telefon i hranu, ali ne i vozilo.

### Svakodnevne stvari
| Stvar | Cena |
|-------|------|
| Voda / sok / pivo (traka gladi +5) | $2 / $3 / $5 |
| Hamburger / pica (traka gladi +30 / +40) | $10 / $15 |
| Cigarete (kutija, 20 komada) / upaljač | $12 / $3 |
| Gorivo | $3 po litru (pun rezervoar ~$150–200) |
| Kanister (prazan) | $50 |
| Telefon (aparat) | $500 |
| SIM kartica (nasumičan broj) | $50 |
| Izbor svog broja (besplatno za Silver i Gold VIP i admine) | +$2.000 |
| Kredit: SMS / minut poziva / pretraga javnog imenika | $1 / $2 / $2 |
| Dopuna kredita (24/7 gotovinom ili sa računa preko telefona; najviše $500 na kartici) | $10, $20, $50, $100 |
| Prva pomoć (leči do 75 HP) | $150 |
| Otvaranje bankovnog računa (uz ličnu kartu, dobija se kartica i PIN) / novi PIN | $100 / $50 |
| Kockice / novčić (za opklade, ne troše se) | $20 / $5 |
| Naočare (3 vrste, prodavnica odeće) | $150 / $200 / $250 |
| Zimska kapa / kačket / panama šešir / polucilindar | $150 / $200 / $300 / $350 |
| Torba / ranac | $400 / $600 |
| Lična karta (opština) | $50 |
| Radio | $700 |
| Alat (kit za popravku) | $300 |
| Uže | $50 |
| Maska (crno tržište) | $2.000 |
| Bolnica (lečenje posle smrti) | $500 |
| Otkup vozila sa pauka | $750 |
| Kirija za sobu (payday) | ~$50 (postavlja vlasnik) |

### Dozvole
| Dozvola | Cena |
|---------|------|
| Ribolov | $250 |
| Motor | $1.000 |
| Auto | $1.500 |
| Kamion | $5.000 |
| Brod | $7.500 |
| Oružje | $15.000 |
| Letenje | $50.000 |

### Oružje (legalno, uz dozvolu)
| Oružje | Cena |
|--------|------|
| Palica | $100 |
| Nož | $300 |
| Pištolj 9mm | $3.000 |
| Desert Eagle | $8.000 |
| Pumparica | $10.000 |
| Municija (pakovanje) | $100–400 |

### Vozila na auto pijaci (nova, ograničen broj komada)
| Kategorija | Primeri | Cena |
|------------|---------|------|
| Bicikli | BMX, Bike, Mountain Bike | $300–1.500 |
| Skuter | Faggio | $3.500 |
| Jeftini automobili | Perennial, Clover, Manana, Previon, Tampa, Glendale | $12.000–20.000 |
| Srednja klasa | Premier, Sentinel, Stratum, Elegant, Washington, Merit | $25.000–45.000 |
| SUV i pickup | Huntley, Landstalker, Rancher, Sadler, Yosemite | $35.000–70.000 |
| Motori | Freeway $18.000, PCJ-600 $25.000, FCR-900 $30.000, NRG-500 $90.000 | $18.000–90.000 |
| Sportski | Elegy, Jester, ZR-350, Buffalo, Banshee, Comet, Super GT | $80.000–200.000 |
| **Top 5** | Sultan $350.000, Cheetah $450.000, Bullet $550.000, Turismo $700.000, Infernus $900.000 | $350.000–900.000 |

**Trgovina između igrača:** prodaja ide preko komande (prodavac zadaje cenu, kupac prihvata), pa je razmena novca i vozila sigurna. Država uzima **taksu od 5%** na prenos vlasništva.

### Nekretnine
| Tip | Cena |
|-----|------|
| Stan / prikolica | $60.000–90.000 |
| Mala kuća | $120.000–200.000 |
| Srednja kuća | $250.000–450.000 |
| Velika kuća | $600.000–1.000.000 |
| Vila | $1.500.000–3.000.000 |
| Mala firma (24/7, bar) | $250.000–500.000 |
| Velika firma (benzinska, klub) | $750.000–1.500.000 |

### Porezi i troškovi (payday)
- Kuća: 0.02% vrednosti (kuća od $150.000 plaća $30).
- Vozilo: 0.01% kupovne cene (Sultan plaća $35).
- Firma: 0.05% vrednosti.

### Šta to znači u praksi (uz ~$1.200 po satu)
| Cilj | Potrebno vreme rada |
|------|---------------------|
| Jeftin auto ($12.000–20.000) | ~10–17 h |
| Stan ($60.000–90.000) | ~50–75 h |
| Mala kuća ($120.000–200.000) | ~100–170 h |
| Sportski auto ($80.000–200.000) | ~65–170 h |
| Sultan ($350.000) | ~290 h |
| Infernus ($900.000) | ~750 h |

### Nivoi
- 1 respect poen se dobija na svaki payday (svaki sat igre).
- Za prelaz sa nivoa N na N+1 potrebno je **(N+1) × 4** poena, kao na većini servera: 1→2 za 8 h, 2→3 za 12 h, 3→4 za 16 h, 4→5 za 20 h (ukupno 56 h do nivoa 5).
- Happy hour (`/happyhour [broj plata]`, menadžment): naredne plate (1–12) donose duplu platu i dupli respect.

| Nivo | Otključava |
|------|------------|
| 1 | Posao, vozačka dozvola, telefon |
| 2 | Lično vozilo, ulazak u legalne frakcije |
| 3 | Kuća, ulazak u ilegalne organizacije |
| 5 | Dozvola za oružje |
| 6 | Firma |

### Ograničenja
- Najviše **2 lična vozila** po igraču.
- Najviše **1 kuća** i **1 firma** po igraču.

---

## 🇬🇧 English

### Principles
- **Prices are anchored to real life** (US prices in $). Players intuitively know what is cheap and what is expensive.
- **Every price is measured in hours of work.** The baseline is an average income of ~$1,000–1,500 per hour of active work.
- **Vehicles are rare.** There is no permanent dealership. An admin occasionally opens a car market with **new** vehicles in limited stock (e.g. 1–2 units of each model). First come, first served. After that, vehicles only change hands through **player-to-player trades**, at whatever price the players agree on.
- **Money must also leave the game** (taxes, fuel, repairs, fines, fees), otherwise inflation follows.
- This is an initial proposal. All amounts will live in one place in the code, and we will balance them during testing. If the total money on the server grows sharply, we add more costs.

### Income
| Source | Amount |
|--------|--------|
| Simple jobs (delivery, garbage, fishing...) | ~$800–1,200 per hour |
| Skilled jobs (mechanic, trucker...) | ~$1,200–2,000 per hour |
| Legal faction salary (payday) | $1,000 (lowest rank) → $3,000 (leader) |
| Welfare, unemployed (payday) | $100 |
| Bank interest (payday) | 0.05% of balance, max $250 |
| Payday | Every full hour, if the player was active for at least 30 min since the last one; paid into the bank |
| Transfer to another account | From level 2, 1% fee; an ATM pays out at most $10,000 at once |
| Bet (dice, coin) | Stake $10–$5,000 per player, from level 2; the winner takes both stakes, a tie returns them |
| Begging | Up to level 4; one gift $1–$100, one giver at most $100 a day to the same beggar, a beggar at most $1,000 a day |
| Lottery | Ticket $50, number 1–50, one ticket per player; a draw every hour; 70% goes into the fund, 30% leaves the game (later the state treasury) |

### Starter pack
- **$2,000 cash + $3,000 waiting at the bank counter = $5,000.** A new player has no account: the $3,000 is picked up at the counter or moves into the account once it is opened ($100, with an ID card). That covers a driving license, a phone and food, but not a vehicle.

### Everyday items
| Item | Price |
|------|-------|
| Water / juice / beer (hunger bar +5) | $2 / $3 / $5 |
| Burger / pizza (hunger bar +30 / +40) | $10 / $15 |
| Cigarettes (pack of 20) / lighter | $12 / $3 |
| Fuel | $3 per liter (full tank ~$150–200) |
| Jerry can (empty) | $50 |
| Phone (device) | $500 |
| SIM card (random number) | $50 |
| Picking your own number (free for Silver and Gold VIP and admins) | +$2,000 |
| Credit: SMS / call minute / public directory search | $1 / $2 / $2 |
| Credit top-up (24/7 in cash or from the bank account through the phone; at most $500 on the card) | $10, $20, $50, $100 |
| First aid kit (heals up to 75 HP) | $150 |
| Opening a bank account (with an ID card; comes with a card and a PIN) / a new PIN | $100 / $50 |
| Dice / coin (for bets, not used up) | $20 / $5 |
| Glasses (3 kinds, clothing shop) | $150 / $200 / $250 |
| Knit hat / cap / panama hat / bowler | $150 / $200 / $300 / $350 |
| Bag / backpack | $400 / $600 |
| ID card (city hall) | $50 |
| Radio | $700 |
| Toolkit (repair kit) | $300 |
| Rope | $50 |
| Mask (black market) | $2,000 |
| Hospital (treatment after death) | $500 |
| Impound release fee | $750 |
| Room rent (payday) | ~$50 (set by the owner) |

### Licenses
| License | Price |
|---------|-------|
| Fishing | $250 |
| Motorbike | $1,000 |
| Car | $1,500 |
| Truck | $5,000 |
| Boat | $7,500 |
| Weapon | $15,000 |
| Flying | $50,000 |

### Weapons (legal, with a license)
| Weapon | Price |
|--------|-------|
| Bat | $100 |
| Knife | $300 |
| 9mm pistol | $3,000 |
| Desert Eagle | $8,000 |
| Shotgun | $10,000 |
| Ammo (pack) | $100–400 |

### Vehicles at the car market (new, limited stock)
| Category | Examples | Price |
|----------|----------|-------|
| Bicycles | BMX, Bike, Mountain Bike | $300–1,500 |
| Scooter | Faggio | $3,500 |
| Cheap cars | Perennial, Clover, Manana, Previon, Tampa, Glendale | $12,000–20,000 |
| Mid-range | Premier, Sentinel, Stratum, Elegant, Washington, Merit | $25,000–45,000 |
| SUVs and pickups | Huntley, Landstalker, Rancher, Sadler, Yosemite | $35,000–70,000 |
| Motorbikes | Freeway $18,000, PCJ-600 $25,000, FCR-900 $30,000, NRG-500 $90,000 | $18,000–90,000 |
| Sports | Elegy, Jester, ZR-350, Buffalo, Banshee, Comet, Super GT | $80,000–200,000 |
| **Top 5** | Sultan $350,000, Cheetah $450,000, Bullet $550,000, Turismo $700,000, Infernus $900,000 | $350,000–900,000 |

**Player-to-player trades:** a sale goes through a command (the seller sets the price, the buyer accepts), so the money/vehicle exchange is safe. The state takes a **5% fee** on the ownership transfer.

### Property
| Type | Price |
|------|-------|
| Apartment / trailer | $60,000–90,000 |
| Small house | $120,000–200,000 |
| Medium house | $250,000–450,000 |
| Large house | $600,000–1,000,000 |
| Villa | $1,500,000–3,000,000 |
| Small business (24/7, bar) | $250,000–500,000 |
| Large business (gas station, club) | $750,000–1,500,000 |

### Taxes and upkeep (payday)
- House: 0.02% of value (a $150,000 house pays $30).
- Vehicle: 0.01% of purchase price (a Sultan pays $35).
- Business: 0.05% of value.

### What that means in practice (at ~$1,200 per hour)
| Goal | Work time needed |
|------|------------------|
| Cheap car ($12,000–20,000) | ~10–17 h |
| Apartment ($60,000–90,000) | ~50–75 h |
| Small house ($120,000–200,000) | ~100–170 h |
| Sports car ($80,000–200,000) | ~65–170 h |
| Sultan ($350,000) | ~290 h |
| Infernus ($900,000) | ~750 h |

### Levels
- 1 respect point is earned on every payday (every hour played).
- Going from level N to N+1 takes **(N+1) × 4** points, as on most servers: 1→2 in 8 h, 2→3 in 12 h, 3→4 in 16 h, 4→5 in 20 h (56 h total to reach level 5).
- Happy hour (`/happyhour [paydays]`, management): the next paydays (1–12) give double wage and double respect.

| Level | Unlocks |
|-------|---------|
| 1 | Job, driving license, phone |
| 2 | Personal vehicle, joining legal factions |
| 3 | House, joining illegal organizations |
| 5 | Weapon license |
| 6 | Business |

### Limits
- At most **2 personal vehicles** per player.
- At most **1 house** and **1 business** per player.
