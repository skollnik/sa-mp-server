// In-game time follows real time. An admin may pin an hour (/settime), and a VIP may keep a personal hour (/vtime).
new g_TimeOverride = -1;
new g_TimeTimer;

stock Time_Init()
{
    g_TimeTimer = SetTimer("Time_Tick", 60 * 1000, true);
}

stock Time_Exit()
{
    KillTimer(g_TimeTimer);
}

stock Time_Get(&hour, &minute)
{
    gettime(hour, minute);
    if (g_TimeOverride != -1)
    {
        hour = g_TimeOverride;
        minute = 0;
    }
}

stock Time_Apply(playerid)
{
    if (g_Vip[playerid][vPersonalTime] && Vip_IsVip(playerid))
    {
        return;
    }
    new hour, minute;
    Time_Get(hour, minute);
    SetPlayerTime(playerid, hour, minute);
}

stock Time_ApplyAll()
{
    new hour, minute;
    Time_Get(hour, minute);
    SetWorldTime(hour);
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (Account_IsLoggedIn(i))
        {
            Time_Apply(i);
        }
    }
    Hud_UpdateClock();
}

forward Time_Tick();
public Time_Tick()
{
    Time_ApplyAll();
    return 1;
}

// -1 goes back to real time.
stock Time_SetOverride(hour)
{
    g_TimeOverride = hour;
    Time_ApplyAll();
}
