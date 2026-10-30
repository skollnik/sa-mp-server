#define BEG_SPAWN_DISTANCE      (100.0)
#define BEG_TOGGLE_COOLDOWN     (10)
// Lower than the staff duty tag so both fit above one head.
#define BEG_TAG_OFFSET_Z        (0.2)
// Leaving the spot or the sitting pose ends begging, so the label never follows a player who walks or drives off.
#define BEG_MAX_DRIFT           (1.0)
#define BEG_POSE_GRACE_S        (3)
#define BEG_SIT_LIBRARY         "BEACH"
#define BEG_SIT_ANIMATION       "ParkSit_M_loop"

new bool:g_Begging[MAX_PLAYERS];
new STREAMER_TAG_3D_TEXT_LABEL:g_BegTag[MAX_PLAYERS];
new g_BegLastToggle[MAX_PLAYERS];
new bool:g_BegPending[MAX_PLAYERS];
new Float:g_BegPos[MAX_PLAYERS][3];
new g_BegStartedAt[MAX_PLAYERS];
new g_BegTimer;

stock Beg_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `beg_gifts` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`giver_id` INT UNSIGNED NOT NULL," \
        "`beggar_id` INT UNSIGNED NOT NULL," \
        "`amount` INT NOT NULL," \
        "`created_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP," \
        "INDEX `beg_beggar_time` (`beggar_id`, `created_at`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
    g_BegTimer = SetTimer("Beg_Tick", 1000, true);
}

stock Beg_Exit()
{
    KillTimer(g_BegTimer);
}

stock Beg_OnPlayerConnect(playerid)
{
    g_Begging[playerid] = false;
    g_BegTag[playerid] = STREAMER_TAG_3D_TEXT_LABEL:INVALID_STREAMER_ID;
    g_BegLastToggle[playerid] = 0;
    g_BegPending[playerid] = false;
}

stock Beg_Stop(playerid)
{
    if (!g_Begging[playerid])
    {
        return;
    }
    g_Begging[playerid] = false;
    // The label is attached to the player ID, so it must go before the ID is reused.
    if (IsValidDynamic3DTextLabel(g_BegTag[playerid]))
    {
        DestroyDynamic3DTextLabel(g_BegTag[playerid]);
    }
    g_BegTag[playerid] = STREAMER_TAG_3D_TEXT_LABEL:INVALID_STREAMER_ID;
}

stock Beg_OnPlayerDisconnect(playerid)
{
    Beg_Stop(playerid);
}

stock Beg_OnPlayerDeath(playerid)
{
    Beg_Stop(playerid);
}

static stock Beg_Interrupt(playerid)
{
    Beg_Stop(playerid);
    Msg_Info(playerid, "Ustali ste, vise ne prosite.");
}

// Entering a vehicle with F skips the command entirely, so the state change ends begging right away.
stock Beg_OnPlayerStateChange(playerid, PLAYER_STATE:newstate)
{
    if (g_Begging[playerid] && newstate != PLAYER_STATE_ONFOOT)
    {
        Beg_Interrupt(playerid);
    }
}

static stock bool:Beg_IsSitting(playerid)
{
    new library[32], animation[32];
    GetAnimationName(GetPlayerAnimationIndex(playerid), library, sizeof(library), animation, sizeof(animation));
    return !strcmp(library, BEG_SIT_LIBRARY, true) && !strcmp(animation, BEG_SIT_ANIMATION, true);
}

forward Beg_Tick();
public Beg_Tick()
{
    new now = gettime();
    for (new playerid = 0; playerid < MAX_PLAYERS; playerid++)
    {
        if (!g_Begging[playerid])
        {
            continue;
        }
        // The pose needs a moment to start after the command, so it is checked only after a short grace.
        if (!IsPlayerInRangeOfPoint(playerid, BEG_MAX_DRIFT, g_BegPos[playerid][0], g_BegPos[playerid][1], g_BegPos[playerid][2])
            || (now - g_BegStartedAt[playerid] >= BEG_POSE_GRACE_S && !Beg_IsSitting(playerid)))
        {
            Beg_Interrupt(playerid);
        }
    }
    return 1;
}

static stock bool:Beg_SameIp(playerid, targetid)
{
    new ip[16], target_ip[16];
    GetPlayerIp(playerid, ip, sizeof(ip));
    GetPlayerIp(targetid, target_ip, sizeof(target_ip));
    return !strcmp(ip, target_ip);
}

cmd:prosim(playerid, params[])
{
    new now = gettime();
    if (now - g_BegLastToggle[playerid] < BEG_TOGGLE_COOLDOWN)
    {
        Msg_Error(playerid, "Sacekajte malo.");
        return 1;
    }
    if (g_Begging[playerid])
    {
        g_BegLastToggle[playerid] = now;
        Beg_Stop(playerid);
        ClearAnimations(playerid, SYNC_ALL);
        Msg_Info(playerid, "Vise ne prosite.");
        return 1;
    }
    if (g_Player[playerid][pLevel] > BEG_MAX_LEVEL)
    {
        Msg_Error(playerid, "Prosnja je samo do nivoa %d.", BEG_MAX_LEVEL);
        return 1;
    }
    if (IsPlayerInAnyVehicle(playerid) || GetPlayerState(playerid) != PLAYER_STATE_ONFOOT)
    {
        Msg_Error(playerid, "Prosi se peske.");
        return 1;
    }
    if (GetPlayerDistanceFromPoint(playerid, NEW_SPAWN_X, NEW_SPAWN_Y, NEW_SPAWN_Z) < BEG_SPAWN_DISTANCE)
    {
        Msg_Error(playerid, "Ne mozete prositi kod mesta gde se pojavljuju novi igraci.");
        return 1;
    }

    g_BegLastToggle[playerid] = now;
    g_BegStartedAt[playerid] = now;
    GetPlayerPos(playerid, g_BegPos[playerid][0], g_BegPos[playerid][1], g_BegPos[playerid][2]);
    g_Begging[playerid] = true;
    g_BegTag[playerid] = CreateDynamic3DTextLabel("Prosi\n(/udeli)", 0xC8C8C8FF, 0.0, 0.0, BEG_TAG_OFFSET_Z, 15.0, playerid, INVALID_VEHICLE_ID, 1);
    ApplyAnimation(playerid, BEG_SIT_LIBRARY, BEG_SIT_ANIMATION, 4.1, true, false, false, false, 0, SYNC_ALL);
    Chat_Action(playerid, "seda na zemlju i prosi.");
    Msg_Info(playerid, "Prosite. Drugi vam daju novac sa /udeli (najvise $%d dnevno). Prekid: /prosim", BEG_DAILY_MAX);
    return 1;
}

cmd:udeli(playerid, params[])
{
    new targetid, amount;
    if (sscanf(params, "ud", targetid, amount))
    {
        Msg_Usage(playerid, "/udeli [id ili ime] [iznos $1-%d] (igracu koji prosi pored vas)", BEG_GIFT_MAX);
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid, "Ne mozete udeliti sami sebi."))
    {
        return 1;
    }
    if (!g_Begging[targetid])
    {
        Msg_Error(playerid, "Taj igrac ne prosi. Novac se daje sa /daj.");
        return 1;
    }
    if (amount < 1 || amount > BEG_GIFT_MAX)
    {
        Msg_Error(playerid, "Udeljuje se od $1 do $%d.", BEG_GIFT_MAX);
        return 1;
    }
    if (g_Player[playerid][pCash] < amount)
    {
        Msg_Error(playerid, "Nemate toliko novca kod sebe.");
        return 1;
    }
    if (!Chat_CheckBeside(playerid, targetid, GIVE_CASH_RADIUS))
    {
        return 1;
    }
    if (Beg_SameIp(playerid, targetid))
    {
        Msg_Error(playerid, "Ne mozete udeliti igracu sa iste mreze (IP adrese).");
        return 1;
    }
    if (g_BegPending[playerid])
    {
        return 1;
    }

    // Today's gifts live in the database, so the daily caps survive relogs and restarts.
    g_BegPending[playerid] = true;
    new query[256];
    mysql_format(g_SQL, query, sizeof(query),
        "SELECT COALESCE(SUM(CASE WHEN `giver_id` = %d THEN `amount` END), 0) AS `from_giver`, COALESCE(SUM(`amount`), 0) AS `total` " \
        "FROM `beg_gifts` WHERE `beggar_id` = %d AND `created_at` >= CURDATE()",
        g_Player[playerid][pDbId], g_Player[targetid][pDbId]);
    mysql_tquery(g_SQL, query, "OnBegLimits", "ddddd", playerid, g_AccountRace[playerid], targetid, g_AccountRace[targetid], amount);
    return 1;
}

forward OnBegLimits(playerid, race, targetid, target_race, amount);
public OnBegLimits(playerid, race, targetid, target_race, amount)
{
    if (race != g_AccountRace[playerid])
    {
        return 1;
    }
    g_BegPending[playerid] = false;
    if (target_race != g_AccountRace[targetid] || !g_Begging[targetid])
    {
        Msg_Error(playerid, "Taj igrac vise ne prosi.");
        return 1;
    }
    // The player could have moved or spent the money while the database answered.
    if (g_Player[playerid][pCash] < amount || !Chat_CheckBeside(playerid, targetid, GIVE_CASH_RADIUS))
    {
        return 1;
    }

    new from_giver, total;
    cache_get_value_name_int(0, "from_giver", from_giver);
    cache_get_value_name_int(0, "total", total);
    if (from_giver + amount > BEG_GIVER_DAILY_MAX)
    {
        Msg_Error(playerid, "Ovom igracu danas mozete udeliti jos $%d.", max(0, BEG_GIVER_DAILY_MAX - from_giver));
        return 1;
    }
    if (total + amount > BEG_DAILY_MAX)
    {
        Msg_Error(playerid, "Taj igrac je danas vec dobio skoro najvise od prosnje (jos $%d).", max(0, BEG_DAILY_MAX - total));
        return 1;
    }

    Money_GiveCash(playerid, -amount, "udeljeno prosjaku", g_Player[targetid][pDbId]);
    Money_GiveCash(targetid, amount, "prosnja", g_Player[playerid][pDbId]);
    Account_Save(playerid);
    Account_Save(targetid);

    new query[160], action[96];
    mysql_format(g_SQL, query, sizeof(query), "INSERT INTO `beg_gifts` (`giver_id`, `beggar_id`, `amount`) VALUES (%d, %d, %d)",
        g_Player[playerid][pDbId], g_Player[targetid][pDbId], amount);
    mysql_tquery(g_SQL, query);

    format(action, sizeof(action), "udeljuje nesto novca osobi %s.", Account_Name(targetid));
    Chat_Action(playerid, action);
    Msg_Info(playerid, "Udelili ste $%d.", amount);
    Msg_Success(targetid, "%s vam je udelio $%d.", Account_Name(playerid), amount);
    return 1;
}
