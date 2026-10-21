# SA-MP Roleplay server (open.mp)

## Pravila projekta / Project rules
- Server: open.mp 1.6.7 (win-x86), Pawn gamemode, pisan od nule / written from scratch.
- Sav tekst koji igrač vidi je na **srpskom, bez dijakritika** (c, s, z, dj), jer ih GTA SA font ne prikazuje. / All player-facing text is **Serbian without diacritics**.
- Kod (imena, fajlovi, komentari) je na **engleskom**. / Code (names, files, comments) is in **English**.
- `.md` dokumentacija je dvojezična: prvo srpski, pa engleski ispod. / `.md` docs are bilingual: Serbian first, English below.
- Sve što nije jasno pitati vlasnika, ne pretpostavljati. / Ask the owner about anything unclear, don't assume.
- Posle svakog koraka ažurirati `docs/PLAN.md`, a nove odluke upisati u `docs/DECISIONS.md`. / Update `docs/PLAN.md` after each step, record new decisions in `docs/DECISIONS.md`.

## Pravila koda / Code rules
- Sistemske poruke idu kroz `Msg_Usage/Msg_Error/Msg_Success/Msg_Server/Msg_Info` iz `core/messages.inc`. / System messages go through the `Msg_*` helpers in `core/messages.inc`.
- Tekst koji je uneo igrač nikad ne ide kao format string (`SendClientMessage`, `SetPlayerChatBubble`...), uvek kao `"%s"` argument, jer `%` može da sruši server. / Player-entered text is never a format string, always a `"%s"` argument, since a stray `%` can crash the server.
- Svi SQL upiti kroz `mysql_format` sa `%e` za stringove. / All SQL goes through `mysql_format` with `%e` for strings.

## Dokumentacija / Docs
- `docs/PLAN.md`: faze, status, otvorena pitanja / phases, status, open questions
- `docs/FEATURES.md`: katalog sistema sa oznakama (ACC-1, VEH-3...) / feature catalog with IDs
- `docs/DECISIONS.md`: odluke i arhitektura koda / decisions and code architecture
- `docs/ECONOMY.md`: cene, plate, nivoi / prices, wages, levels
- `docs/BACKLOG.md`: podsetnik za stvari koje čekaju drugi sistem / reminders for things waiting on another system
- `docs/SETUP_MYSQL.md`: instalacija baze / database setup

## Struktura / Layout
- `omp-server.exe`, `config.json`: server i konfiguracija / server and config
- `components/`: open.mp komponente / components
- `qawno/`: `pawncc.exe` kompajler i include-ovi / compiler and includes
- `gamemodes/`, `filterscripts/`, `scriptfiles/`, `models/`
