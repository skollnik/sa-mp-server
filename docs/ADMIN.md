# Tim: admini i helperi / Staff: admins and helpers

## 🇷🇸 Srpski

Kompletan spisak ko šta može. Svaki novi sistem dodaje svoje komande ovde.
Viši nivo uvek ima sve komande nižeg nivoa.

### Pravila
- Postoje dve odvojene lestvice: **Helper 1–3** i **Admin 1–5 + Vlasnik (6)**. Ista osoba može imati obe.
- **Admin komande rade samo na dužnosti** (`/aduty`), osim admin chata `/a`. Vlasnik je izuzet.
- **Helper komande rade na helper dužnosti** (`/hduty`), osim helper chata `/hc`. Svaki admin može da koristi helper komande.
- **Helperi ne kažnjavaju.** Odgovaraju na pitanja, a prekršaje prijavljuju adminima.
- Admin ne može da kazni, zamrzne ili preimenuje admina **istog ili višeg** nivoa.
- Na dužnosti je ime admina narandžasto, a helpera tirkizno (u TAB listi).
- Svaka admin akcija se upisuje u tabelu `admin_logs` (ko, šta, kome, detalji, vreme).

### Komande za sve igrače
| Komanda | Opis |
|---------|------|
| `/report [id] [razlog]` | prijava igrača adminima (jednom u 60 s) |
| `/ask [pitanje]` | pitanje helperima (jednom u 30 s). Ako nema helpera na dužnosti, vidi ga ceo tim |
| `/n [tekst]` | newbie chat, za igrače do nivoa 3 i za tim. Igrači pišu jednom u 30 s |
| `/tognewbie` | isključi/uključi newbie chat za sebe |
| `/admins`, `/helpers` | ko je od tima online i ko je na dužnosti |

### Helperi
| Nivo | Naziv | Komande |
|------|-------|---------|
| 1 | Helper | `/hduty`, `/hc`, `/odg [id] [odgovor]`, `/hhelp` |
| 2 | Iskusni helper | `/goto` |
| 3 | Glavni helper | `/makehelper` (postavlja i skida helpere nivoa 1–2) |

### Admini
| Nivo | Naziv | Komande |
|------|-------|---------|
| 1 | Admin pocetnik | `/aduty`, `/a`, `/ahelp`, `/reports`, `/ar`, `/rr`, `/goto`, `/gethere`, `/spec`, `/specoff`, `/check`, `/freeze`, `/unfreeze`, `/slap` |
| 2 | Admin | `/sethp`, `/setarmour`, `/tp` (teleport lokacije), `/ooc` (uključuje/isključuje globalni OOC) |
| 3 | Visi admin | `/setskin`, `/setint`, `/setvw`, `/veh [model]`, `/dveh`, `/dveh sva` |
| 4 | Glavni admin | `/makehelper` (svi nivoi), `/setlevel`, `/setrespect`, `/setname` |
| 5 | Menadzment | `/makeadmin` (do nivoa 4), `/gmx` (restart servera) |
| 6 | Vlasnik | `/makeadmin` (do nivoa 5), `/givemoney`, `/dozvoliime`, `/zabraniime` |

### Uskoro (deo 2: kazne)
`/kick`, `/ban`, `/unban`, ban po IP-u i serijskom broju, ban igrača koji nije online, `/warn` (3 upozorenja = ban 3 dana), `/mute`, `/nmute` (newbie mute, helperi od nivoa 2), `/ajail`, `/pmspy` (admin 4+), provera bana pri konekciji.

---

## 🇬🇧 English

The full list of who can do what. Every new system adds its commands here.
A higher level always has every command of the lower levels.

### Rules
- Two separate ladders: **Helper 1–3** and **Admin 1–5 + Owner (6)**. One person can hold both.
- **Admin commands work only on duty** (`/aduty`), except admin chat `/a`. The owner is exempt.
- **Helper commands work on helper duty** (`/hduty`), except helper chat `/hc`. Every admin can use helper commands.
- **Helpers do not punish.** They answer questions and report violations to admins.
- An admin cannot punish, freeze or rename an admin of the **same or higher** level.
- On duty, an admin's name is orange and a helper's is teal (in the TAB list).
- Every admin action is written to the `admin_logs` table (who, what, to whom, details, time).

### Commands for every player
| Command | Description |
|---------|-------------|
| `/report [id] [reason]` | report a player to admins (once per 60 s) |
| `/ask [question]` | question for helpers (once per 30 s). With no helper on duty the whole team sees it |
| `/n [text]` | newbie chat, for players up to level 3 and the team. Players post once per 30 s |
| `/tognewbie` | turn newbie chat off/on for yourself |
| `/admins`, `/helpers` | who from the team is online and who is on duty |

### Helpers
| Level | Name | Commands |
|-------|------|----------|
| 1 | Helper | `/hduty`, `/hc`, `/odg [id] [answer]`, `/hhelp` |
| 2 | Senior helper | `/goto` |
| 3 | Head helper | `/makehelper` (sets and removes helper levels 1–2) |

### Admins
| Level | Name | Commands |
|-------|------|----------|
| 1 | Trial admin | `/aduty`, `/a`, `/ahelp`, `/reports`, `/ar`, `/rr`, `/goto`, `/gethere`, `/spec`, `/specoff`, `/check`, `/freeze`, `/unfreeze`, `/slap` |
| 2 | Admin | `/sethp`, `/setarmour`, `/tp` (teleport locations), `/ooc` (toggles global OOC) |
| 3 | Senior admin | `/setskin`, `/setint`, `/setvw`, `/veh [model]`, `/dveh`, `/dveh sva` |
| 4 | Head admin | `/makehelper` (all levels), `/setlevel`, `/setrespect`, `/setname` |
| 5 | Management | `/makeadmin` (up to level 4), `/gmx` (server restart) |
| 6 | Owner | `/makeadmin` (up to level 5), `/givemoney`, `/dozvoliime`, `/zabraniime` |

### Coming next (part 2: punishments)
`/kick`, `/ban`, `/unban`, IP and client-serial bans, offline ban, `/warn` (3 warnings = 3-day ban), `/mute`, `/nmute` (newbie mute, helpers level 2+), `/ajail`, `/pmspy` (admin 4+), ban check on connect.
