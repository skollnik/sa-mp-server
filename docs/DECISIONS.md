# Odluke / Decisions

## 🇷🇸 Srpski

Ovde se beleži svaka važna odluka, da se ne bi ponovo raspravljalo o njoj. Novije odluke idu na dno tabele.

| # | Datum | Odluka | Razlog |
|---|-------|--------|--------|
| 1 | 2026-10-21 | Platforma: open.mp 1.6.7 (win-x86), Pawn | Moderna zamena za SA-MP server, kompatibilna sa 0.3.7/0.3DL klijentima |
| 2 | 2026-10-21 | Tekst u igri na srpskom, **bez dijakritika** (c, s, z, dj) | GTA SA font ne prikazuje č ć š ž đ |
| 3 | 2026-10-21 | Kod, komentari i imena fajlova na engleskom | Standard |
| 4 | 2026-10-21 | `.md` dokumentacija dvojezična: srpski, pa engleski ispod | Zahtev vlasnika |
| 5 | 2026-10-21 | Baza: **MySQL (MariaDB preko XAMPP-a)** + plugin **mysql R41-4** | R41-4 je industrijski standard za SA-MP i pouzdan je. MariaDB izbegava problem sa MySQL 8 autentifikacijom (`caching_sha2_password`) koji R41-4 ima |
| 6 | 2026-10-21 | Biblioteke: `sscanf 2.13.8`, `Pawn.CMD 3.4.0-omp`, `streamer 2.9.6`, `samp-bcrypt 0.4.1`, `crashdetect 4.22` (za debug) | Najčešće korišćene i proverene biblioteke, sve rade na open.mp |
| 7 | 2026-10-21 | Bez YSI biblioteke. Moduli se povezuju ručnim pozivima iz glavnih callback-ova | Jednostavniji i čitljiviji kod, brže kompajliranje, nema "magije" |
| 8 | 2026-10-21 | Editor: VS Code (qawno editor obrisan, kompajler `pawncc` ostaje) | Vlasnik koristi VS Code |
| 9 | 2026-10-21 | Server je za sada lokalni (Windows) | Hosting se rešava kasnije |
| 10 | 2026-10-21 | Ime servera: `SA-MP Test Server`, u porukama "Test Server", 50 slotova | Privremeno ime, dok se ne smisli pravo |
| 11 | 2026-10-21 | Radnja se odvija uglavnom u Los Santosu | Kao na većini RP servera |
| 12 | 2026-10-21 | Heavy (pravi) RP, sa punim spektrom frakcija | Zahtev vlasnika |
| 13 | 2026-10-21 | Git: prate se izvorni kod, include-ovi, kompajler, server binarni fajlovi i pluginovi. Ignorišu se `.amx`, logovi i lozinke za bazu | Kada se repo klonira, server se odmah pokreće. Tajne podatke ne guramo na GitHub |
| 14 | 2026-10-21 | Pristupni podaci za bazu idu u `mysql.ini` (ignorisan u gitu). U repou je samo `mysql.ini.example` | Bezbednost |
| 15 | 2026-10-21 | Server sam pravi tabele pri pokretanju (`CREATE TABLE IF NOT EXISTS`), svaki modul svoje | Vlasnik ne mora ništa ručno da importuje |
| 16 | 2026-10-21 | Jedan nalog = jedan karakter, ime obavezno `Ime_Prezime`. Kratko ime ima samo vlasnik (`skollnik`) i oni kojima ga on dodeli | Zahtev vlasnika |
| 17 | 2026-10-21 | Nema kviza pri registraciji, pravila se samo prikazuju | Zahtev vlasnika |
| 18 | 2026-10-21 | Postoje nivoi (respect poeni na payday) | Zahtev vlasnika |
| 19 | 2026-10-21 | Nema stalnog auto salona. Vozila se kupuju samo na povremenim auto pijacama (admin event) ili od igrača | Vozila treba da budu retka |
| 20 | 2026-10-21 | Realna ekonomija: start $5.000 ($2.000 keš + $3.000 banka), cene vezane za stvarni život. Cene su u `ECONOMY.md` | Novac i vozila treba da budu retki i vredni |
| 21 | 2026-10-21 | Novi igrači dobijaju nasumičan skin (3 muška: 7, 47, 60; 3 ženska: 40, 56, 93) i pojavljuju se ispred aerodroma u Los Santosu | Zahtev vlasnika |
| 22 | 2026-10-21 | Email se za sada ne traži pri registraciji | Nije potreban dok ne postoji web panel |
| 23 | 2026-10-21 | Dve odvojene lestvice tima: Helper 1–3 i Admin 1–5 + Vlasnik (6). Viši nivo ima sve komande nižeg | Usvojen predlog |
| 24 | 2026-10-21 | Admin komande rade samo na dužnosti (`/aduty`), osim admin chata. Vlasnik je izuzet | Na dužnosti je jasno ko je admin, a van nje admin igra kao igrač |
| 25 | 2026-10-21 | Helperi ne kažnjavaju (nema kick/ban). Odgovaraju na pitanja, a prekršaje prijavljuju adminima. Od nivoa 2 mogu da daju mute u newbie chatu | Uobičajena uloga helpera |
| 26 | 2026-10-21 | 3 upozorenja (`/warn`) = automatski ban 3 dana | Usvojen predlog |
| 27 | 2026-10-21 | Admini od nivoa 4 mogu da prate PM-ove (`/pmspy`) | Usvojen predlog |

### Arhitektura koda (predlog)
```
gamemodes/
  roleplay.pwn            ← ulazna tačka: samo include-ovi i glavni callback-ovi
  core/                   ← podešavanja, boje, makroi, util funkcije, baza
  modules/<modul>/        ← jedan sistem = jedan folder (account, character, admin, chat, vehicle...)
      <modul>_data.inc    ← enumi, nizovi, konstante
      <modul>_impl.inc    ← logika i funkcije
      <modul>_cmds.inc    ← komande
                          (tabele pravi svaki modul u svom Module_Init)
```
- Imenovanje: `Module_Action()` (npr. `Account_Load`, `Vehicle_SetFuel`), globalne promenljive `g_`, konstante `UPPER_CASE`.
- Svaki modul ima funkcije `Module_OnPlayerConnect`, `Module_OnPlayerDisconnect` itd. i one se pozivaju iz `roleplay.pwn`.
- Svi upiti ka bazi su asinhroni (`mysql_tquery`) i prolaze kroz `mysql_format` sa `%e`, zbog SQL injection-a.

### Otvorena pitanja
Vidi `PLAN.md`, sekcija "Otvorena pitanja".

---

## 🇬🇧 English

This file records every important decision so it does not get re-discussed. Newer decisions go at the bottom of the table.

| # | Date | Decision | Reason |
|---|------|----------|--------|
| 1 | 2026-10-21 | Platform: open.mp 1.6.7 (win-x86), Pawn | Modern SA-MP server replacement, compatible with 0.3.7/0.3DL clients |
| 2 | 2026-10-21 | In-game text in Serbian, **no diacritics** (c, s, z, dj) | GTA SA font cannot render č ć š ž đ |
| 3 | 2026-10-21 | Code, comments and file names in English | Standard |
| 4 | 2026-10-21 | `.md` docs bilingual: Serbian first, English below | Owner's request |
| 5 | 2026-10-21 | Database: **MySQL (MariaDB via XAMPP)** + **mysql R41-4** plugin | R41-4 is the de-facto SA-MP standard and is reliable. MariaDB avoids R41-4's MySQL 8 auth problem (`caching_sha2_password`) |
| 6 | 2026-10-21 | Libraries: `sscanf 2.13.8`, `Pawn.CMD 3.4.0-omp`, `streamer 2.9.6`, `samp-bcrypt 0.4.1`, `crashdetect 4.22` (debug) | Most widely used and proven libraries, all work on open.mp |
| 7 | 2026-10-21 | No YSI. Modules are wired up by explicit calls from the main callbacks | Simpler, readable code, faster compiles, no "magic" |
| 8 | 2026-10-21 | Editor: VS Code (qawno editor removed, `pawncc` compiler kept) | Owner uses VS Code |
| 9 | 2026-10-21 | Server is local (Windows) for now | Hosting decided later |
| 10 | 2026-10-21 | Server name `SA-MP Test Server`, "Test Server" in messages, 50 slots | Placeholder name until the real one is chosen |
| 11 | 2026-10-21 | Setting is mostly Los Santos | Like most RP servers |
| 12 | 2026-10-21 | Heavy (serious) RP with a full range of factions | Owner's request |
| 13 | 2026-10-21 | Git tracks source, includes, compiler, server binaries and plugins. It ignores `.amx`, logs and DB credentials | A clone runs immediately. Secrets stay off GitHub |
| 14 | 2026-10-21 | DB credentials live in `mysql.ini` (git-ignored). The repo only has `mysql.ini.example` | Security |
| 15 | 2026-10-21 | The server creates its tables on startup (`CREATE TABLE IF NOT EXISTS`), each module its own | The owner never has to import anything by hand |
| 16 | 2026-10-21 | One account = one character, name must be `Firstname_Lastname`. Only the owner (`skollnik`) and those they grant it to get a short name | Owner's request |
| 17 | 2026-10-21 | No quiz at registration, rules are only shown | Owner's request |
| 18 | 2026-10-21 | Levels exist (respect points on payday) | Owner's request |
| 19 | 2026-10-21 | No permanent dealership. Vehicles are bought only at occasional car markets (admin event) or from players | Vehicles should be rare |
| 20 | 2026-10-21 | Realistic economy: start with $5,000 ($2,000 cash + $3,000 bank), prices anchored to real life. Prices are in `ECONOMY.md` | Money and vehicles should be scarce and valuable |
| 21 | 2026-10-21 | New players get a random skin (3 male: 7, 47, 60; 3 female: 40, 56, 93) and spawn in front of Los Santos airport | Owner's request |
| 22 | 2026-10-21 | No email asked at registration for now | Not needed until there is a web panel |
| 23 | 2026-10-21 | Two separate staff ladders: Helper 1–3 and Admin 1–5 + Owner (6). A higher level has every command of the lower ones | Proposal accepted |
| 24 | 2026-10-21 | Admin commands work only on duty (`/aduty`), except admin chat. The owner is exempt | On duty it is clear who is an admin, off duty the admin plays as a player |
| 25 | 2026-10-21 | Helpers do not punish (no kick/ban). They answer questions and report violations to admins. From level 2 they can mute in newbie chat | Usual helper role |
| 26 | 2026-10-21 | 3 warnings (`/warn`) = automatic 3-day ban | Proposal accepted |
| 27 | 2026-10-21 | Admins level 4+ can watch PMs (`/pmspy`) | Proposal accepted |

### Code architecture (proposal)
```
gamemodes/
  roleplay.pwn            ← entry point: only includes and main callbacks
  core/                   ← settings, colors, macros, utils, database
  modules/<module>/       ← one system = one folder (account, character, admin, chat, vehicle...)
      <module>_data.inc   ← enums, arrays, constants
      <module>_impl.inc   ← logic and functions
      <module>_cmds.inc   ← commands
                          (each module creates its tables in Module_Init)
```
- Naming: `Module_Action()` (e.g. `Account_Load`, `Vehicle_SetFuel`), globals prefixed `g_`, constants `UPPER_CASE`.
- Each module exposes `Module_OnPlayerConnect`, `Module_OnPlayerDisconnect` etc., which are called from `roleplay.pwn`.
- All DB queries are async (`mysql_tquery`) and go through `mysql_format` with `%e` to prevent SQL injection.

### Open questions
See `PLAN.md`, section "Open questions".
