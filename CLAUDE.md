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
- `Msg_*` su makroi: poziv mora biti u jednom redu (bez preloma unutar zagrada). / `Msg_*` are macros: the call must stay on one line.
- Sve što se ponavlja ide u funkciju modula (`Chat_SendToAll`, `Chat_Clear`, `Staff_MessageAs`, `Staff_NotifyTarget`...), ne kopira se kod. / Anything repeated goes into a module function, never copy-pasted code.
- Teleport, oružje i armor uvek preko `AC_SetPlayerPos`, `AC_GivePlayerWeapon`, `AC_ResetPlayerWeapons`, `AC_SetPlayerArmour`, inače ih anti-cheat prijavljuje. / Teleports, weapons and armour always go through the `AC_*` wrappers, otherwise anti-cheat flags them.
- Komande nad igračem imaju posebnu poruku za nepostojećeg igrača i posebnu za sebe (`Staff_CheckTarget` sa porukom za sebe). / Commands on a player have separate messages for a missing player and for targeting yourself.
- Svi SQL upiti kroz `mysql_format` sa `%e` za stringove. / All SQL goes through `mysql_format` with `%e` for strings.

## Dokumentacija / Docs
- `docs/PLAN.md`: faze, status, otvorena pitanja / phases, status, open questions
- `docs/FEATURES.md`: katalog sistema sa oznakama (ACC-1, VEH-3...) / feature catalog with IDs
- `docs/DECISIONS.md`: odluke i arhitektura koda / decisions and code architecture
- `docs/ECONOMY.md`: cene, plate, nivoi / prices, wages, levels
- `docs/ADMIN.md`: tim, nivoi i komande / staff, levels and commands
- `docs/MAPPING.md`: Texture Studio i gotove mape / Texture Studio and ready-made maps
- `docs/BACKLOG.md`: podsetnik za stvari koje čekaju drugi sistem / reminders for things waiting on another system
- `docs/SETUP_MYSQL.md`: instalacija baze / database setup

## Struktura / Layout
- `omp-server.exe`, `config.json`: server i konfiguracija / server and config
- `components/`: open.mp komponente / components
- `qawno/`: `pawncc.exe` kompajler i include-ovi / compiler and includes
- `gamemodes/`, `filterscripts/`, `scriptfiles/`, `models/`
