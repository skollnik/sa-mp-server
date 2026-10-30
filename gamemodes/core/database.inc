new MySQL:g_SQL = MYSQL_INVALID_HANDLE;

stock bool:Database_Connect()
{
    mysql_log(ERROR | WARNING);

    g_SQL = mysql_connect_file("mysql.ini");
    if (g_SQL == MYSQL_INVALID_HANDLE)
    {
        print("[DB] mysql.ini is missing or invalid (see mysql.ini.example).");
        return false;
    }

    new errno = mysql_errno(g_SQL);
    if (errno != 0)
    {
        printf("[DB] Connection failed (error %d). Check the password in mysql.ini and that MySQL is running in XAMPP.", errno);
        return false;
    }

    mysql_set_charset("utf8mb4", g_SQL);
    print("[DB] Connected.");
    return true;
}

stock Database_Disconnect()
{
    if (g_SQL != MYSQL_INVALID_HANDLE)
    {
        mysql_close(g_SQL);
        g_SQL = MYSQL_INVALID_HANDLE;
    }
}

// The lowest free `id` of a table, for an INSERT ... SELECT, so the IDs of deleted rows are given out again.
// LAST_INSERT_ID(expr) makes cache_insert_id() return it, since the row is not given an id by AUTO_INCREMENT.
#define DB_FREE_ID(%0) "LAST_INSERT_ID(IF(NOT EXISTS (SELECT 1 FROM `" %0 "` WHERE `id` = 1), 1, " \
    "(SELECT MIN(a.`id`) + 1 FROM `" %0 "` a LEFT JOIN `" %0 "` b ON b.`id` = a.`id` + 1 WHERE b.`id` IS NULL)))"
