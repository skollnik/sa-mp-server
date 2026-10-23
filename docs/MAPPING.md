# Mapiranje / Mapping

## 🇷🇸 Srpski

### Čime se pravi mapa
- **Texture Studio**, najkorišćeniji editor za SA-MP i open.mp. Radi **u igri**: šetaš, postavljaš objekte, pomeraš ih i bojiš teksturama, a na kraju izvezeš kod. Ovo preporučujem.
- Alternative: **MTA:SA Map Editor** (editor iz drugog multiplayera, izvoz se konvertuje u SA-MP kod preko konvertera) i **Fusez Map Editor**. Rade, ali se manje koriste.

### Instalacija Texture Studio-a
Texture Studio pokrećemo kao **poseban server za mapiranje**, odvojen od našeg, da se ništa ne pomeša.

1. Napravi folder `C:\Users\Skolnik\Desktop\open.mp-win-x86\MapEditor` (pored foldera `Server`).
2. Preuzmi Texture Studio: otvori https://github.com/Pottus/Texture-Studio → zeleno dugme **Code** → **Download ZIP**. Raspakuj **sadržaj** ZIP-a u `MapEditor` (tako da u `MapEditor` budu folderi `filterscripts`, `plugins`, `scriptfiles` i fajl `server.cfg`).
3. Iz našeg `Server` foldera **kopiraj** u `MapEditor`:
   - `omp-server.exe`
   - ceo folder `components`
4. Otvori `MapEditor\server.cfg` u Notepad-u i promeni:
   - `port 7777` u `port 7780`, da može da radi istovremeno sa našim serverom
   - `rcon_password nopass` u neku svoju lozinku
5. Pokreni `MapEditor\omp-server.exe`. U konzoli treba da piše da su učitani filterscript `tstudio` i pluginovi `streamer`, `sscanf` i `filemanager`.
6. U SA-MP klijentu dodaj server `127.0.0.1:7780` i poveži se (ime nije bitno).

Ako se pojavi greška, pošalji mi `MapEditor\log.txt`. Ovu kombinaciju (Texture Studio na open.mp serveru) nisam sam probao u igri, pa je moguće da treba neka sitna ispravka.

### Osnovne komande
| Komanda | Šta radi |
|---------|----------|
| `/newmap` | nova mapa (upiše se kratko ime) |
| `/loadmap` | otvara ranije sačuvanu mapu |
| `/cobject [model]` | postavlja objekat ispred tebe |
| `/osearch [ime]` | traži objekat po imenu (npr. `bench`, `lamp`, `tree`) |
| `/sel [broj]` | bira objekat |
| `/editobject` | pomeranje i rotacija mišem (Levi ALT pravi kopiju) |
| `/ox`, `/oy`, `/oz` | precizno pomeranje po osi |
| `/rx`, `/ry`, `/rz` | precizna rotacija |
| `/gadd`, `/editgroup`, `/gclone`, `/gdelete` | rad sa grupom objekata |
| `/mtextures`, `/mtset`, `/mtcolor` | teksture i boje |
| `/remobject` | uklanja originalnu GTA zgradu ili objekat |
| `/undo` | vraća poslednju izmenu |
| `/thelp` | spisak svih komandi |

Mapa se automatski čuva dok radiš.

### Kako da mi pošalješ mapu
1. U igri ukucaj `/exportmap`, izaberi **CreateDynamicObject** i za draw distance stavi bar `300`.
2. Fajl se pojavi u `MapEditor\scriptfiles\ExportMaps\`.
3. Pošalji mi ime fajla. Ja ga ubacujem u `gamemodes/maps/<ime>.inc`, a uklonjene GTA zgrade (`RemoveBuildingForPlayer`) idu tamo gde se uklanjaju svakom igraču pri ulasku.

### Saveti
- Drži mape umerenim: stotine objekata su u redu, a hiljade na jednom mestu prave lag pri učitavanju.
- Za unutrašnjost (enterijer) koristi poseban virtuelni svet, da se ne preklapa sa drugim mapama.
- Pre nego što mapu ubacimo na server, pošalji screenshot (`F8` u igri), da znamo kako izgleda.

### Gotove besplatne mape
Na zvaničnom open.mp forumu, sekcija **Maps** (https://forum.open.mp/forumdisplay.php?fid=67), postoje gotove mape korisne za RP u Los Santosu:

| Mapa | Autor | Korisno za |
|------|-------|-----------|
| LS City Hall | Santino SL | opština |
| Modern LSPD HD Map | Ata | policija (LSPD) |
| Sheriff Los Santos | DarQ | šerif |
| City Hall, Jewelry, Hotel | pmemorex2016 | opština, zlatara, hotel |
| Gun shop | Twixxer | prodavnica oružja |
| Auto Repair | 8ocean.mr | mehaničarska radionica |
| Crime Base | 8ocean.mr | baza bande ili mafije |
| Map Pack #1–3 | AZZO | razne mape |

Pre korišćenja treba otvoriti temu, proveriti da li autor dozvoljava korišćenje i šta mapa tačno sadrži. Ako mi pošalješ link, ja proverim i ubacim je.

---

## 🇬🇧 English

### What maps are made with
- **Texture Studio**, the most used editor for SA-MP and open.mp. It works **in-game**: you walk around, place objects, move them and texture them, then export the code. This is the recommendation.
- Alternatives: **MTA:SA Map Editor** (an editor from another multiplayer; its export is converted to SA-MP code with a converter) and **Fusez Map Editor**. They work but are used less.

### Installing Texture Studio
Texture Studio runs as a **separate mapping server**, apart from ours, so nothing gets mixed up.

1. Create the folder `C:\Users\Skolnik\Desktop\open.mp-win-x86\MapEditor` (next to the `Server` folder).
2. Download Texture Studio: open https://github.com/Pottus/Texture-Studio → green **Code** button → **Download ZIP**. Extract the ZIP's **contents** into `MapEditor` (so `MapEditor` holds the `filterscripts`, `plugins`, `scriptfiles` folders and the `server.cfg` file).
3. From our `Server` folder **copy** into `MapEditor`:
   - `omp-server.exe`
   - the whole `components` folder
4. Open `MapEditor\server.cfg` in Notepad and change:
   - `port 7777` to `port 7780`, so it can run alongside our server
   - `rcon_password nopass` to a password of your own
5. Run `MapEditor\omp-server.exe`. The console should report the `tstudio` filterscript and the `streamer`, `sscanf` and `filemanager` plugins as loaded.
6. In the SA-MP client add the server `127.0.0.1:7780` and connect (any name).

If an error shows up, send me `MapEditor\log.txt`. This combination (Texture Studio on an open.mp server) has not been tried in-game yet, so a small fix may be needed.

### Basic commands
| Command | What it does |
|---------|--------------|
| `/newmap` | new map (type a short name) |
| `/loadmap` | opens a previously saved map |
| `/cobject [model]` | places an object in front of you |
| `/osearch [name]` | searches objects by name (e.g. `bench`, `lamp`, `tree`) |
| `/sel [index]` | selects an object |
| `/editobject` | move and rotate with the mouse (Left ALT clones) |
| `/ox`, `/oy`, `/oz` | precise movement along an axis |
| `/rx`, `/ry`, `/rz` | precise rotation |
| `/gadd`, `/editgroup`, `/gclone`, `/gdelete` | working with a group of objects |
| `/mtextures`, `/mtset`, `/mtcolor` | textures and colours |
| `/remobject` | removes an original GTA building or object |
| `/undo` | undoes the last change |
| `/thelp` | list of all commands |

The map is saved automatically while you work.

### How to send me a map
1. In-game type `/exportmap`, pick **CreateDynamicObject** and set the draw distance to at least `300`.
2. The file appears in `MapEditor\scriptfiles\ExportMaps\`.
3. Send me the file name. I put it into `gamemodes/maps/<name>.inc`, and removed GTA buildings (`RemoveBuildingForPlayer`) go where they are removed for every player on connect.

### Tips
- Keep maps moderate: hundreds of objects are fine, thousands in one spot cause lag while loading.
- Use a separate virtual world for interiors so they don't overlap with other maps.
- Before a map goes on the server, send a screenshot (`F8` in-game) so we know what it looks like.

### Ready-made free maps
The official open.mp forum, **Maps** section (https://forum.open.mp/forumdisplay.php?fid=67), has ready-made maps useful for RP in Los Santos:

| Map | Author | Useful for |
|-----|--------|-----------|
| LS City Hall | Santino SL | city hall |
| Modern LSPD HD Map | Ata | police (LSPD) |
| Sheriff Los Santos | DarQ | sheriff |
| City Hall, Jewelry, Hotel | pmemorex2016 | city hall, jewellery store, hotel |
| Gun shop | Twixxer | gun shop |
| Auto Repair | 8ocean.mr | mechanic shop |
| Crime Base | 8ocean.mr | gang or mafia base |
| Map Pack #1–3 | AZZO | various maps |

Before using one, open its thread and check that the author allows reuse and what exactly the map contains. Send me a link and I'll check and add it.
