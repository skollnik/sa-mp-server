stock AC_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `ac_logs` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`account_id` INT UNSIGNED NOT NULL," \
        "`name` VARCHAR(24) NOT NULL," \
        "`type` VARCHAR(24) NOT NULL," \
        "`details` VARCHAR(128) NOT NULL DEFAULT ''," \
        "`action` VARCHAR(16) NOT NULL DEFAULT 'alert'," \
        "`created_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP," \
        "INDEX `ac_account_time` (`account_id`, `created_at`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");

    g_AcTimer = SetTimer("AC_Tick", AC_TICK_MS, true);
}

stock AC_Exit()
{
    KillTimer(g_AcTimer);
}

stock AC_OnPlayerConnect(playerid)
{
    static const empty[E_AC_STATE];
    g_Ac[playerid] = empty;
    g_Ac[playerid][acSpecText] = INVALID_PLAYER_TEXT_DRAW;
}

stock AC_OnPlayerSpawn(playerid)
{
    AC_GraceMovement(playerid);
}

stock AC_GraceMovement(playerid)
{
    g_Ac[playerid][acGraceUntil] = gettime() + AC_MOVE_GRACE_S;
    g_Ac[playerid][acHasLastPos] = false;
    g_Ac[playerid][acHasTarget] = false;
}

// After a server teleport, movement checks pause until the client reports being at the destination.
stock AC_ExpectTeleport(playerid, Float:x, Float:y, Float:z)
{
    g_Ac[playerid][acTargetX] = x;
    g_Ac[playerid][acTargetY] = y;
    g_Ac[playerid][acTargetZ] = z;
    g_Ac[playerid][acHasTarget] = true;
    g_Ac[playerid][acHasLastPos] = false;
    g_Ac[playerid][acGraceUntil] = gettime() + AC_TELEPORT_CONFIRM_S;
}

stock AC_SetPlayerPos(playerid, Float:x, Float:y, Float:z)
{
    AC_ExpectTeleport(playerid, x, y, z);
    SetPlayerPos(playerid, x, y, z);
}

// Weapons and armour are only valid when the server handed them out through these wrappers.
stock AC_GivePlayerWeapon(playerid, WEAPON:weapon, ammo)
{
    new WEAPON_SLOT:slot = GetWeaponSlot(weapon);
    if (_:slot >= 0 && _:slot < AC_WEAPON_SLOTS)
    {
        g_Ac[playerid][acWeapon][_:slot] = weapon;
    }
    GivePlayerWeapon(playerid, weapon, ammo);
}

stock AC_ResetPlayerWeapons(playerid)
{
    for (new i = 0; i < AC_WEAPON_SLOTS; i++)
    {
        g_Ac[playerid][acWeapon][i] = WEAPON_FIST;
    }
    g_Ac[playerid][acWeaponSyncUntil] = gettime() + AC_WEAPON_SYNC_S;
    ResetPlayerWeapons(playerid);
}

stock AC_SetPlayerArmour(playerid, Float:armour)
{
    g_Ac[playerid][acArmour] = armour;
    g_Ac[playerid][acArmourSyncUntil] = gettime() + AC_ARMOUR_SYNC_S;
    SetPlayerArmour(playerid, armour);
}

stock AC_Log(playerid, E_AC_TYPE:type, const details[], const action[])
{
    new query[384];
    mysql_format(g_SQL, query, sizeof(query),
        "INSERT INTO `ac_logs` (`account_id`, `name`, `type`, `details`, `action`) VALUES (%d, '%e', '%e', '%e', '%e')",
        g_Player[playerid][pDbId], g_Player[playerid][pName], g_AcTypeNames[type], details, action);
    mysql_tquery(g_SQL, query);
}

stock AC_Alert(playerid, E_AC_TYPE:type, const details[])
{
    new now = gettime();
    if (now - g_Ac[playerid][acLastAlert][type] < AC_ALERT_COOLDOWN_S)
    {
        return;
    }
    g_Ac[playerid][acLastAlert][type] = now;

    new message[160];
    format(message, sizeof(message), "[AC] %s [%d]: sumnja na %s (%s). Proverite sa /spec %d", g_Player[playerid][pName], playerid, g_AcTypeNames[type], details, playerid);
    Staff_SendToAdmins(message);
    AC_Log(playerid, type, details, "alert");
}

// Certain cheats (a weapon or jetpack the server never gave) are kicked straight away.
stock AC_KickForCheat(playerid, E_AC_TYPE:type, const details[])
{
    if (!Account_IsLoggedIn(playerid))
    {
        return;
    }

    new message[160];
    format(message, sizeof(message), "[AC] %s je izbacen sa servera: %s (%s).", g_Player[playerid][pName], g_AcTypeNames[type], details);
    Punish_Announce(message);
    AC_Log(playerid, type, details, "kick");

    // Saved and logged out first so the tick does not report the same player again before the kick lands.
    Account_Save(playerid);
    g_Player[playerid][pState] = ACCOUNT_STATE_NONE;
    KickWithDelay(playerid);
}

stock AC_CheckWeapons(playerid)
{
    // Right after the server takes weapons away the client still holds them for a moment.
    if (g_Ac[playerid][acWeaponSyncUntil] > gettime())
    {
        return;
    }

    new WEAPON:weapon, ammo;
    for (new slot = 0; slot < AC_WEAPON_SLOTS; slot++)
    {
        GetPlayerWeaponData(playerid, WEAPON_SLOT:slot, weapon, ammo);
        if (weapon == WEAPON_FIST || ammo <= 0 || weapon == WEAPON_PARACHUTE)
        {
            continue;
        }
        if (weapon != g_Ac[playerid][acWeapon][slot])
        {
            new weapon_name[32], details[64];
            GetWeaponName(weapon, weapon_name, sizeof(weapon_name));
            format(details, sizeof(details), "%s, %d metaka", weapon_name, ammo);
            AC_KickForCheat(playerid, AC_TYPE_WEAPON, details);
            return;
        }
    }
}

stock AC_CheckHealthArmour(playerid)
{
    new Float:health, Float:armour, details[48];
    GetPlayerHealth(playerid, health);
    GetPlayerArmour(playerid, armour);

    if (health > 100.5)
    {
        SetPlayerHealth(playerid, 100.0);
        format(details, sizeof(details), "HP %.0f", health);
        AC_Alert(playerid, AC_TYPE_HEALTH, details);
    }

    // Until the client shows the armour the server just set, its old value must not become the new allowance.
    if (g_Ac[playerid][acArmourSyncUntil] > gettime())
    {
        if (floatabs(armour - g_Ac[playerid][acArmour]) <= 1.0)
        {
            g_Ac[playerid][acArmourSyncUntil] = 0;
        }
        return;
    }

    if (armour > g_Ac[playerid][acArmour] + 1.0)
    {
        SetPlayerArmour(playerid, g_Ac[playerid][acArmour]);
        format(details, sizeof(details), "armor %.0f, dozvoljeno %.0f", armour, g_Ac[playerid][acArmour]);
        AC_Alert(playerid, AC_TYPE_ARMOUR, details);
    }
    else
    {
        // Armour only ever goes down on its own; remember the lower value so it cannot be raised back.
        g_Ac[playerid][acArmour] = armour;
    }
}

stock bool:AC_IsAircraft(modelid)
{
    switch (modelid)
    {
        case 417, 425, 447, 460, 469, 476, 487, 488, 497, 511, 512, 513, 519, 520, 548, 553, 563, 577, 592, 593:
        {
            return true;
        }
    }
    return false;
}

stock AC_CheckMovement(playerid, PLAYER_STATE:player_state)
{
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);

    if (g_Ac[playerid][acHasTarget])
    {
        new bool:arrived = VectorSize(x - g_Ac[playerid][acTargetX], y - g_Ac[playerid][acTargetY], z - g_Ac[playerid][acTargetZ]) <= AC_TELEPORT_ARRIVE_RADIUS;
        if (!arrived && gettime() < g_Ac[playerid][acGraceUntil])
        {
            return;
        }
        g_Ac[playerid][acHasTarget] = false;
        g_Ac[playerid][acGraceUntil] = 0;
    }

    if (gettime() < g_Ac[playerid][acGraceUntil] || !g_Ac[playerid][acHasLastPos])
    {
        g_Ac[playerid][acLastX] = x;
        g_Ac[playerid][acLastY] = y;
        g_Ac[playerid][acLastZ] = z;
        g_Ac[playerid][acHasLastPos] = true;
        return;
    }

    new Float:dx = x - g_Ac[playerid][acLastX],
        Float:dy = y - g_Ac[playerid][acLastY],
        Float:distance = VectorSize(dx, dy, 0.0),
        details[64];

    if (player_state == PLAYER_STATE_ONFOOT && GetPlayerSurfingVehicleID(playerid) == INVALID_VEHICLE_ID)
    {
        if (distance > AC_TELEPORT_DISTANCE)
        {
            format(details, sizeof(details), "%.0f m za 1 s", distance);
            AC_Alert(playerid, AC_TYPE_TELEPORT, details);
        }
        else if (distance > AC_MAX_FOOT_SPEED)
        {
            format(details, sizeof(details), "%.0f km/h peske", distance * 3.6);
            AC_Alert(playerid, AC_TYPE_FOOT_SPEED, details);
        }
    }
    else if (player_state == PLAYER_STATE_DRIVER)
    {
        new vehicleid = GetPlayerVehicleID(playerid);
        if (!AC_IsAircraft(GetVehicleModel(vehicleid)))
        {
            new Float:vx, Float:vy, Float:vz;
            GetVehicleVelocity(vehicleid, vx, vy, vz);
            new Float:kmh = VectorSize(vx, vy, vz) * 180.0;
            if (kmh > AC_MAX_VEHICLE_KMH)
            {
                format(details, sizeof(details), "%.0f km/h", kmh);
                AC_Alert(playerid, AC_TYPE_VEHICLE_SPEED, details);
            }
        }
    }

    g_Ac[playerid][acLastX] = x;
    g_Ac[playerid][acLastY] = y;
    g_Ac[playerid][acLastZ] = z;
}

forward AC_Tick();
public AC_Tick()
{
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (!Account_IsLoggedIn(i))
        {
            continue;
        }

        if (g_Staff[i][sSpectating])
        {
            AC_UpdateSpecInfo(i);
            continue;
        }

        new PLAYER_STATE:player_state = GetPlayerState(i);
        if (player_state != PLAYER_STATE_ONFOOT && player_state != PLAYER_STATE_DRIVER && player_state != PLAYER_STATE_PASSENGER)
        {
            continue;
        }

        // Money is kept on the server; a changed client value is just put back.
        if (GetPlayerMoney(i) != g_Player[i][pCash])
        {
            ResetPlayerMoney(i);
            GivePlayerMoney(i, g_Player[i][pCash]);
        }

        if (GetPlayerSpecialAction(i) == SPECIAL_ACTION_USEJETPACK)
        {
            SetPlayerSpecialAction(i, SPECIAL_ACTION_NONE);
            AC_KickForCheat(i, AC_TYPE_JETPACK, "jetpack");
            continue;
        }

        AC_CheckWeapons(i);
        if (!Account_IsLoggedIn(i))
        {
            continue;
        }

        AC_CheckHealthArmour(i);
        AC_CheckMovement(i, player_state);
    }
    return 1;
}

stock AC_UpdateStrikes(&last_tick, &strikes, min_gap_ms, calm_gap_ms)
{
    new now = GetTickCount(), gap = now - last_tick;
    last_tick = now;

    if (gap < min_gap_ms)
    {
        strikes++;
    }
    else if (gap >= calm_gap_ms)
    {
        strikes = 0;
    }
    else if (strikes > 0)
    {
        strikes--;
    }
    return strikes;
}

stock AC_FloodWarning(playerid)
{
    new now = gettime();
    if (now - g_Ac[playerid][acLastFloodWarning] >= AC_FLOOD_WARNING_RESET_S)
    {
        g_Ac[playerid][acFloodWarnings] = 0;
    }
    g_Ac[playerid][acLastFloodWarning] = now;

    if (++g_Ac[playerid][acFloodWarnings] < AC_FLOOD_WARNINGS_TO_MUTE)
    {
        Msg_Error(playerid, "Ne spamujte chat (upozorenje %d/%d, posle toga automatski mute).", g_Ac[playerid][acFloodWarnings], AC_FLOOD_WARNINGS_TO_MUTE);
        return;
    }

    g_Ac[playerid][acFloodWarnings] = 0;
    g_Ac[playerid][acChatStrikes] = 0;
    g_Player[playerid][pMuteUntil] = now + AC_FLOOD_MUTE_MINUTES * 60;
    Account_Save(playerid);

    new message[144];
    format(message, sizeof(message), "[AC] %s je automatski dobio mute na %d min zbog spama.", g_Player[playerid][pName], AC_FLOOD_MUTE_MINUTES);
    Punish_Announce(message);
    AC_Log(playerid, AC_TYPE_FLOOD, "automatski mute", "mute");
}

stock bool:AC_CheckChat(playerid, const text[])
{
    if (text[0] && !strcmp(text, g_Ac[playerid][acLastMessage], true))
    {
        if (++g_Ac[playerid][acRepeatCount] >= AC_REPEAT_LIMIT)
        {
            AC_FloodWarning(playerid);
            return false;
        }
    }
    else
    {
        g_Ac[playerid][acRepeatCount] = 0;
        strcopy(g_Ac[playerid][acLastMessage], text, 128);
    }

    if (AC_UpdateStrikes(g_Ac[playerid][acLastChatTick], g_Ac[playerid][acChatStrikes], AC_CHAT_MIN_GAP_MS, AC_CHAT_CALM_GAP_MS) >= AC_CHAT_STRIKES)
    {
        AC_FloodWarning(playerid);
        return false;
    }
    return true;
}

stock bool:AC_CheckCommand(playerid)
{
    if (AC_UpdateStrikes(g_Ac[playerid][acLastCmdTick], g_Ac[playerid][acCmdStrikes], AC_CMD_MIN_GAP_MS, AC_CHAT_CALM_GAP_MS) >= AC_CMD_STRIKES)
    {
        Msg_Error(playerid, "Previse komandi za kratko vreme, sacekajte malo.");
        return false;
    }
    return true;
}

stock AC_ShowSpecInfo(adminid)
{
    if (g_Ac[adminid][acSpecText] != INVALID_PLAYER_TEXT_DRAW)
    {
        return;
    }

    new PlayerText:text = CreatePlayerTextDraw(adminid, 320.0, 400.0, "_");
    PlayerTextDrawAlignment(adminid, text, TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawFont(adminid, text, TEXT_DRAW_FONT_1);
    PlayerTextDrawLetterSize(adminid, text, 0.25, 1.1);
    PlayerTextDrawSetOutline(adminid, text, 1);
    PlayerTextDrawShow(adminid, text);
    g_Ac[adminid][acSpecText] = text;
    AC_UpdateSpecInfo(adminid);
}

stock AC_HideSpecInfo(adminid)
{
    if (g_Ac[adminid][acSpecText] == INVALID_PLAYER_TEXT_DRAW)
    {
        return;
    }
    PlayerTextDrawDestroy(adminid, g_Ac[adminid][acSpecText]);
    g_Ac[adminid][acSpecText] = INVALID_PLAYER_TEXT_DRAW;
}

stock AC_UpdateSpecInfo(adminid)
{
    if (g_Ac[adminid][acSpecText] == INVALID_PLAYER_TEXT_DRAW)
    {
        return;
    }

    new targetid = g_Staff[adminid][sSpecTarget];
    new Float:health, Float:armour, Float:vx, Float:vy, Float:vz, weapon_name[32];
    GetPlayerHealth(targetid, health);
    GetPlayerArmour(targetid, armour);

    if (IsPlayerInAnyVehicle(targetid))
    {
        GetVehicleVelocity(GetPlayerVehicleID(targetid), vx, vy, vz);
    }
    else
    {
        GetPlayerVelocity(targetid, vx, vy, vz);
    }

    new WEAPON:weapon = GetPlayerWeapon(targetid);
    if (weapon == WEAPON_FIST)
    {
        strcopy(weapon_name, "nema");
    }
    else
    {
        GetWeaponName(weapon, weapon_name, sizeof(weapon_name));
    }

    PlayerTextDrawSetString(adminid, g_Ac[adminid][acSpecText],
        "~w~%s [%d]~n~HP ~r~%.0f ~w~Armor ~b~%.0f ~w~Oruzje ~y~%s (%d) ~w~Brzina ~g~%.0f km/h ~w~Ping %d",
        g_Player[targetid][pName], targetid, health, armour, weapon_name, GetPlayerAmmo(targetid),
        VectorSize(vx, vy, vz) * 180.0, GetPlayerPing(targetid));
}

// GTA strips weapons and armour on death, so the server-side allowance is cleared with them.
stock AC_OnPlayerDeath(playerid)
{
    for (new i = 0; i < AC_WEAPON_SLOTS; i++)
    {
        g_Ac[playerid][acWeapon][i] = WEAPON_FIST;
    }
    g_Ac[playerid][acArmour] = 0.0;
}
