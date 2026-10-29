new g_PaydayTimer;
new g_LastPaydayHour;
// How many of the coming paydays are doubled.
new g_HappyHourPaydays;

stock Payday_Init()
{
    new minute;
    gettime(g_LastPaydayHour, minute);
    g_PaydayTimer = SetTimer("Payday_MinuteTick", 60 * 1000, true);
}

stock Payday_Exit()
{
    KillTimer(g_PaydayTimer);
}

stock Payday_OnPlayerUpdate(playerid)
{
    g_Player[playerid][pLastUpdateTick] = GetTickCount();
}

// Typing in chat or using a command counts as activity even when standing still.
stock Payday_MarkActive(playerid)
{
    g_Player[playerid][pIdleMinutes] = 0;
}

stock bool:Payday_IsAfk(playerid)
{
    return GetTickCount() - g_Player[playerid][pLastUpdateTick] > AFK_PAUSE_MS || g_Player[playerid][pIdleMinutes] >= AFK_IDLE_MINUTES;
}

stock bool:Payday_IsHappyHour()
{
    return g_HappyHourPaydays > 0;
}

stock Payday_RespectNeeded(playerid)
{
    return (g_Player[playerid][pLevel] + 1) * RESPECT_PER_LEVEL;
}

stock Payday_CheckLevelUp(playerid)
{
    while (g_Player[playerid][pRespect] >= Payday_RespectNeeded(playerid))
    {
        g_Player[playerid][pRespect] -= Payday_RespectNeeded(playerid);
        g_Player[playerid][pLevel]++;
        SetPlayerScore(playerid, g_Player[playerid][pLevel]);
        GameTextForPlayer(playerid, "~g~Nivo %d", 4000, 1, g_Player[playerid][pLevel]);
        PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
        Msg_Success(playerid, "Presli ste na nivo %d!", g_Player[playerid][pLevel]);
    }
}

forward Payday_MinuteTick();
public Payday_MinuteTick()
{
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (!Account_IsLoggedIn(i))
        {
            continue;
        }

        new Float:x, Float:y, Float:z;
        GetPlayerPos(i, x, y, z);
        if (VectorSize(x - g_Player[i][pIdleX], y - g_Player[i][pIdleY], z - g_Player[i][pIdleZ]) > 1.0)
        {
            g_Player[i][pIdleMinutes] = 0;
            g_Player[i][pIdleX] = x;
            g_Player[i][pIdleY] = y;
            g_Player[i][pIdleZ] = z;
        }
        else
        {
            g_Player[i][pIdleMinutes]++;
        }

        // Time in admin jail is a punishment and does not count toward the payday.
        if (!Payday_IsAfk(i) && g_Player[i][pJailSeconds] == 0 && g_Player[i][pPaydayMinutes] < 60)
        {
            g_Player[i][pPaydayMinutes]++;
        }
    }

    new hour, minute;
    gettime(hour, minute);
    if (hour != g_LastPaydayHour)
    {
        g_LastPaydayHour = hour;
        for (new i = 0; i < MAX_PLAYERS; i++)
        {
            if (Account_IsLoggedIn(i))
            {
                Payday_Give(i);
            }
        }
        Payday_CountHappyHour();
    }
    return 1;
}

// Payday comes at every full hour of real time to everyone with enough active minutes since the last one.
// Short of the minimum, the minutes carry over to the next hour.
stock Payday_Give(playerid)
{
    if (g_Player[playerid][pPaydayMinutes] < PAYDAY_MIN_MINUTES)
    {
        Msg_Info(playerid, "Payday: niste bili dovoljno aktivni (%d/%d min). Minuti se prenose na sledeci sat.", g_Player[playerid][pPaydayMinutes], PAYDAY_MIN_MINUTES);
        return;
    }
    g_Player[playerid][pPaydayMinutes] = 0;

    new vip = Vip_GetLevel(playerid), bool:happy = Payday_IsHappyHour();
    new wage = PAYDAY_SOCIAL_WAGE * (happy ? 2 : 1);
    new interest = min(floatround(float(g_Player[playerid][pBank]) * PAYDAY_INTEREST_RATE, floatround_floor), PAYDAY_INTEREST_MAX);
    new bonus = (wage + interest) * g_VipPaydayBonus[vip] / 100;
    new respect = (PAYDAY_RESPECT + (vip == VIP_GOLD ? 1 : 0)) * (happy ? 2 : 1);
    new total = wage + interest + bonus;

    Money_GiveBank(playerid, total, "payday");
    g_Player[playerid][pRespect] += respect;

    SendClientMessage(playerid, COLOR_SERVER, "_______________ PLATNI LISTIC _______________");
    SendClientMessage(playerid, COLOR_WHITE, "Plata (socijalna pomoc): $%d%s | Kamata: $%d | VIP bonus: $%d", wage, happy ? (" (happy hour x2)") : (""), interest, bonus);
    SendClientMessage(playerid, COLOR_WHITE, "Ukupno uplaceno na racun: " C_GREEN "$%d" C_WHITE " | Novo stanje: $%d", total, g_Player[playerid][pBank]);
    SendClientMessage(playerid, COLOR_WHITE, "Respect: +%d (%d/%d do nivoa %d)", respect, g_Player[playerid][pRespect], Payday_RespectNeeded(playerid), g_Player[playerid][pLevel] + 1);
    GameTextForPlayer(playerid, "~y~Payday", 3000, 1);

    Payday_CheckLevelUp(playerid);
    Account_Save(playerid);
}

stock Payday_AnnounceHappyHour()
{
    new message[128];
    if (g_HappyHourPaydays > 0)
    {
        format(message, sizeof(message), "[HAPPY HOUR] Sledecih %d payday-a donosi duplu platu i respect!", g_HappyHourPaydays);
    }
    else
    {
        strcopy(message, "[HAPPY HOUR] Happy hour je zavrsen.");
    }
    Chat_SendToAll(COLOR_YELLOW, message);
}

// Called once per full-hour payday, after everyone got paid.
stock Payday_CountHappyHour()
{
    if (g_HappyHourPaydays > 0)
    {
        g_HappyHourPaydays--;
        Payday_AnnounceHappyHour();
    }
}

cmd:happyhour(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_MANAGEMENT))
    {
        return 1;
    }

    new paydays;
    if (sscanf(params, "d", paydays) || paydays < 0 || paydays > HAPPY_HOUR_MAX_PAYDAYS)
    {
        Msg_Usage(playerid, "/happyhour [broj plata 1-%d, 0 prekida] (naredne plate donose duplu platu i respect)", HAPPY_HOUR_MAX_PAYDAYS);
        return 1;
    }

    g_HappyHourPaydays = paydays;
    Payday_AnnounceHappyHour();

    new details[16];
    format(details, sizeof(details), "%d plata", paydays);
    Staff_Log(playerid, "happyhour", INVALID_PLAYER_ID, details);
    return 1;
}
