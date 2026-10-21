# Instalacija baze / Database setup

## 🇷🇸 Srpski

Koristimo **XAMPP**. On sadrži **MariaDB** (verzija MySQL-a koja radi sa našim pluginom) i **phpMyAdmin** (web interfejs za pregled baze).

### 1. Preuzimanje
1. Otvori https://www.apachefriends.org i preuzmi **XAMPP for Windows** (najnovija 8.x verzija).
2. Pokreni instalaciju. Ako se pojavi upozorenje o UAC-u, samo klikni OK.

### 2. Instalacija
1. Na ekranu **Select Components** ostavi štiklirano samo:
   - `Apache` (potreban za phpMyAdmin)
   - `MySQL`
   - `PHP`
   - `phpMyAdmin`
   
   Sve ostalo (FileZilla, Mercury, Tomcat, Perl, Webalizer, Fake Sendmail) odštikliraj.
2. Folder za instalaciju: `C:\xampp` (ostavi podrazumevano).
3. Odštikliraj "Learn more about Bitnami".
4. Završi instalaciju.

### 3. Pokretanje
1. Otvori **XAMPP Control Panel**.
2. Klikni **Start** pored `Apache` i pored `MySQL`. Oba treba da pozelene.
   - Ako Apache ne može da se pokrene, port 80 je zauzet (Skype, IIS...). Javi mi pa rešavamo.
   - Ako MySQL ne može da se pokrene, port 3306 je zauzet. Javi mi.
3. Opciono: klikni na **Config → Autostart of modules** i štikliraj Apache i MySQL. Tako se pale sami kad otvoriš Control Panel.

### 4. Pravljenje baze i korisnika
1. U browseru otvori http://localhost/phpmyadmin
2. Gore klikni na tab **SQL**.
3. Nalepi sledeće. Umesto `OVDE_TVOJA_LOZINKA` upiši lozinku koju sam izmisliš:
   ```sql
   CREATE DATABASE samp_rp CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
   CREATE USER 'samp'@'localhost' IDENTIFIED BY 'OVDE_TVOJA_LOZINKA';
   GRANT ALL PRIVILEGES ON samp_rp.* TO 'samp'@'localhost';
   FLUSH PRIVILEGES;
   ```
4. Klikni **Go**. Levo treba da se pojavi baza `samp_rp`.

### 5. Lozinka za server
1. Otvori fajl `scriptfiles/mysql.ini`. Ako ne postoji, kopiraj `scriptfiles/mysql.ini.example` i preimenuj kopiju u `mysql.ini`.
2. U redu `password = OVDE_TVOJA_LOZINKA` upiši lozinku iz koraka 4 i sačuvaj fajl.
3. Ovaj fajl se **nikad ne šalje na GitHub** (nalazi se u `.gitignore`), a lozinku ne moraš nikome da šalješ.

---

## 🇬🇧 English

We use **XAMPP**. It ships **MariaDB** (a MySQL variant that works with our plugin) and **phpMyAdmin** (a web UI for browsing the database).

### 1. Download
1. Open https://www.apachefriends.org and download **XAMPP for Windows** (latest 8.x).
2. Run the installer. If a UAC warning shows up, just click OK.

### 2. Install
1. On the **Select Components** screen keep only these checked:
   - `Apache` (needed for phpMyAdmin)
   - `MySQL`
   - `PHP`
   - `phpMyAdmin`
   
   Uncheck everything else (FileZilla, Mercury, Tomcat, Perl, Webalizer, Fake Sendmail).
2. Install folder: `C:\xampp` (keep the default).
3. Uncheck "Learn more about Bitnami".
4. Finish the install.

### 3. Start
1. Open **XAMPP Control Panel**.
2. Click **Start** next to `Apache` and next to `MySQL`. Both should turn green.
   - If Apache won't start, port 80 is taken (Skype, IIS...). Tell me and we'll fix it.
   - If MySQL won't start, port 3306 is taken. Tell me.
3. Optional: click **Config → Autostart of modules** and check Apache and MySQL. They will then start whenever the Control Panel opens.

### 4. Create the database and user
1. Open http://localhost/phpmyadmin in a browser.
2. Click the **SQL** tab at the top.
3. Paste the following. Replace `OVDE_TVOJA_LOZINKA` with a password you make up:
   ```sql
   CREATE DATABASE samp_rp CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
   CREATE USER 'samp'@'localhost' IDENTIFIED BY 'OVDE_TVOJA_LOZINKA';
   GRANT ALL PRIVILEGES ON samp_rp.* TO 'samp'@'localhost';
   FLUSH PRIVILEGES;
   ```
4. Click **Go**. The `samp_rp` database should appear on the left.

### 5. Password for the server
1. Open `scriptfiles/mysql.ini`. If it doesn't exist, copy `scriptfiles/mysql.ini.example` and rename the copy to `mysql.ini`.
2. On the line `password = OVDE_TVOJA_LOZINKA` put the password from step 4 and save the file.
3. This file **never goes to GitHub** (it is in `.gitignore`), and you don't have to send the password to anyone.
