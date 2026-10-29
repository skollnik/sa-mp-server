stock Hunger_Init()
{
    g_HungerTimer = SetTimer("Hunger_Tick", 60 * 1000, true);
}

stock Hunger_Exit()
{
    KillTimer(g_HungerTimer);
}

stock Hunger_OnPlayerConnect(playerid)
{
    g_HungerMinutes[playerid] = 0;
}

// Prints the warning when hunger crosses the warning line or reaches zero.
stock Hunger_Set(playerid, value)
{
    new old = g_Player[playerid][pHunger];
    value = clamp(value, 0, HUNGER_MAX);
    g_Player[playerid][pHunger] = value;
    Hud_UpdateHunger(playerid);

    if (value == 0 && old > 0)
    {
        Msg_Error(playerid, "Umirete od gladi: gubite HP dok nesto ne pojedete (prodavnica 24/7).");
    }
    else if (value <= HUNGER_WARNING && old > HUNGER_WARNING)
    {
        Msg_Info(playerid, "Gladni ste. Pojedite nesto (prodavnica 24/7, /inv).");
    }
}

stock Hunger_Add(playerid, amount)
{
    Hunger_Set(playerid, g_Player[playerid][pHunger] + amount);
}

stock bool:Hunger_IsFull(playerid)
{
    return g_Player[playerid][pHunger] > HUNGER_FULL;
}

// Hunger stands still while the player is away, in admin jail, on admin duty or not in the world.
static stock bool:Hunger_IsRunning(playerid)
{
    new PLAYER_STATE:player_state = GetPlayerState(playerid);
    if (player_state != PLAYER_STATE_ONFOOT && player_state != PLAYER_STATE_DRIVER && player_state != PLAYER_STATE_PASSENGER)
    {
        return false;
    }
    return !Payday_IsAfk(playerid) && g_Player[playerid][pJailSeconds] == 0 && !g_Staff[playerid][sAdminDuty];
}

static stock Hunger_Starve(playerid)
{
    new Float:health;
    GetPlayerHealth(playerid, health);
    if (health > HUNGER_MIN_HP)
    {
        SetPlayerHealth(playerid, health - HUNGER_HP_LOSS < HUNGER_MIN_HP ? HUNGER_MIN_HP : health - HUNGER_HP_LOSS);
    }
}

forward Hunger_Tick();
public Hunger_Tick()
{
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (!Account_IsLoggedIn(i) || !Hunger_IsRunning(i))
        {
            continue;
        }
        if (g_Player[i][pHunger] == 0)
        {
            Hunger_Starve(i);
        }
        else if (++g_HungerMinutes[i] >= HUNGER_DROP_MINUTES)
        {
            g_HungerMinutes[i] = 0;
            Hunger_Add(i, -1);
        }
    }
    return 1;
}

// A death must not leave the player starving the moment they come back.
stock Hunger_OnPlayerDeath(playerid)
{
    if (g_Player[playerid][pHunger] < HUNGER_AFTER_DEATH)
    {
        Hunger_Set(playerid, HUNGER_AFTER_DEATH);
    }
}

cmd:setglad(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }

    new targetid, value;
    if (sscanf(params, "ud", targetid, value) || value < 0 || value > HUNGER_MAX)
    {
        Msg_Usage(playerid, "/setglad [id ili ime] [0-%d] (%d je potpuno sit)", HUNGER_MAX, HUNGER_MAX);
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }

    Hunger_Set(targetid, value);
    Account_Save(targetid);

    new details[48];
    Msg_Admin(playerid, "Postavili ste glad %d/%d igracu %s.", value, HUNGER_MAX, Account_Name(targetid));
    format(details, sizeof(details), "vam je postavio glad na %d/%d.", value, HUNGER_MAX);
    Staff_NotifyTarget(playerid, targetid, details);
    format(details, sizeof(details), "%d", value);
    Staff_Log(playerid, "setglad", targetid, details);
    return 1;
}
