#define FINE_LIST_LIMIT         (30)

// Unpaid fines of an online player, kept in memory so registration can check them without a query.
new g_FineCount[MAX_PLAYERS];
new g_FineTotal[MAX_PLAYERS];
// What the open /kazne list showed, so paying never covers a fine that arrived after the list was opened.
new g_FineShownMaxId[MAX_PLAYERS];
new g_FineShownTotal[MAX_PLAYERS];

stock Fine_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `fines` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`account_id` INT UNSIGNED NOT NULL," \
        "`vehicle_id` INT UNSIGNED NOT NULL DEFAULT 0," \
        "`plate` VARCHAR(12) NOT NULL DEFAULT ''," \
        "`reason` VARCHAR(64) NOT NULL," \
        "`amount` INT NOT NULL," \
        "`created_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP," \
        "`paid_at` DATETIME NULL," \
        "INDEX `fine_account` (`account_id`, `paid_at`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
}

stock Fine_OnPlayerConnect(playerid)
{
    g_FineCount[playerid] = 0;
    g_FineTotal[playerid] = 0;
}

stock Fine_Count(playerid)
{
    return g_FineCount[playerid];
}

stock Fine_Total(playerid)
{
    return g_FineTotal[playerid];
}

stock Fine_OnLogin(playerid)
{
    new query[160];
    mysql_format(g_SQL, query, sizeof(query),
        "SELECT COUNT(*) AS `count`, IFNULL(SUM(`amount`), 0) AS `total` FROM `fines` WHERE `account_id` = %d AND `paid_at` IS NULL",
        g_Player[playerid][pDbId]);
    mysql_tquery(g_SQL, query, "OnFinesCounted", "dd", playerid, g_AccountRace[playerid]);
}

forward OnFinesCounted(playerid, race);
public OnFinesCounted(playerid, race)
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }
    cache_get_value_name_int(0, "count", g_FineCount[playerid]);
    cache_get_value_name_int(0, "total", g_FineTotal[playerid]);
    if (g_FineCount[playerid] > 0)
    {
        Msg_Info(playerid, "Imate %d neplacenih kazni ($%d). Placaju se u opstini ili policiji (/kazne); dok nisu placene, vozilo se ne registruje.", g_FineCount[playerid], g_FineTotal[playerid]);
    }
    return 1;
}

// The owner pays, as with a real camera fine, even when someone else was driving; an offline owner sees it on login.
stock Fine_Add(owner_dbid, vehicle_dbid, const plate[], const reason[], amount)
{
    new query[256];
    mysql_format(g_SQL, query, sizeof(query),
        "INSERT INTO `fines` (`account_id`, `vehicle_id`, `plate`, `reason`, `amount`) VALUES (%d, %d, '%e', '%e', %d)",
        owner_dbid, vehicle_dbid, plate, reason, amount);
    mysql_tquery(g_SQL, query);

    new ownerid = Account_FindOnlineByDbId(owner_dbid);
    if (ownerid != INVALID_PLAYER_ID)
    {
        g_FineCount[ownerid]++;
        g_FineTotal[ownerid] += amount;
    }
    return ownerid;
}

cmd:kazne(playerid, params[])
{
    new query[256];
    mysql_format(g_SQL, query, sizeof(query),
        "SELECT `id`, `plate`, `reason`, `amount`, DATE_FORMAT(`created_at`, '%%d.%%m. %%H:%%i') AS `time` FROM `fines` " \
        "WHERE `account_id` = %d AND `paid_at` IS NULL ORDER BY `id` LIMIT %d",
        g_Player[playerid][pDbId], FINE_LIST_LIMIT);
    mysql_tquery(g_SQL, query, "OnFinesListed", "dd", playerid, g_AccountRace[playerid]);
    return 1;
}
alias:kazne("fines")

forward OnFinesListed(playerid, race);
public OnFinesListed(playerid, race)
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }
    new rows = cache_num_rows();
    if (rows == 0)
    {
        Msg_Info(playerid, "Nemate neplacenih kazni.");
        return 1;
    }

    new body[2048], line[128], id, plate[VEH_PLATE_LENGTH], reason[64], amount, time[16], total, max_id;
    strcopy(body, "Vreme\tTablica\tPrekrsaj\tIznos\n");
    for (new row = 0; row < rows; row++)
    {
        cache_get_value_name_int(row, "id", id);
        cache_get_value_name(row, "plate", plate);
        cache_get_value_name(row, "reason", reason);
        cache_get_value_name_int(row, "amount", amount);
        cache_get_value_name(row, "time", time);
        if (!plate[0])
        {
            strcopy(plate, "-");
        }
        format(line, sizeof(line), "%s\t%s\t%s\t$%d\n", time, plate, reason, amount);
        strcat(body, line);
        total += amount;
        max_id = id;
    }
    g_FineShownMaxId[playerid] = max_id;
    g_FineShownTotal[playerid] = total;

    new bool:can_pay = Point_IsNear(playerid, POINT_CITYHALL) || Point_IsNear(playerid, POINT_POLICE), title[64];
    format(title, sizeof(title), "Neplacene kazne: $%d%s", total, can_pay ? ("") : (" (placaju se u opstini ili policiji)"));
    ShowPlayerDialog(playerid, DIALOG_FINES, DIALOG_STYLE_TABLIST_HEADERS, title, body, can_pay ? ("Plati sve") : ("Zatvori"), can_pay ? ("Zatvori") : (""));
    return 1;
}

stock bool:Fine_OnDialogResponse(playerid, dialogid, response)
{
    if (dialogid != DIALOG_FINES)
    {
        return false;
    }
    if (!response || g_FineShownTotal[playerid] <= 0 || !(Point_IsNear(playerid, POINT_CITYHALL) || Point_IsNear(playerid, POINT_POLICE)))
    {
        return true;
    }
    new total = g_FineShownTotal[playerid];
    if (!Money_Take(playerid, total, "kazne sa kamera"))
    {
        return true;
    }
    new query[192];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `fines` SET `paid_at` = NOW() WHERE `account_id` = %d AND `paid_at` IS NULL AND `id` <= %d",
        g_Player[playerid][pDbId], g_FineShownMaxId[playerid]);
    mysql_tquery(g_SQL, query);
    g_FineShownTotal[playerid] = 0;
    // Recounted from the database, since more fines than the list shows may still be open.
    Fine_OnLogin(playerid);
    Msg_Success(playerid, "Platili ste kazne: $%d.", total);
    return true;
}
