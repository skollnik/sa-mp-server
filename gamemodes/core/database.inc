// MySQL connection handling. Credentials are read from scriptfiles/mysql.ini (never committed).

new MySQL:g_SQL = MYSQL_INVALID_HANDLE;

stock bool:Database_Connect()
{
    mysql_log(ERROR | WARNING);

    g_SQL = mysql_connect_file("mysql.ini");
    if (g_SQL == MYSQL_INVALID_HANDLE)
    {
        print("[DB] scriptfiles/mysql.ini is missing or invalid (see scriptfiles/mysql.ini.example).");
        return false;
    }

    new errno = mysql_errno(g_SQL);
    if (errno != 0)
    {
        printf("[DB] Connection failed (error %d). Check the password in scriptfiles/mysql.ini and that MySQL is running in XAMPP.", errno);
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
