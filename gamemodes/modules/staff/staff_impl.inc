stock Staff_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `admin_logs` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`admin_id` INT UNSIGNED NOT NULL," \
        "`admin_name` VARCHAR(24) NOT NULL," \
        "`action` VARCHAR(32) NOT NULL," \
        "`target_id` INT UNSIGNED NULL," \
        "`target_name` VARCHAR(24) NULL," \
        "`details` VARCHAR(255) NOT NULL DEFAULT ''," \
        "`created_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP," \
        "INDEX `admin_time` (`admin_id`, `created_at`)," \
        "INDEX `target_time` (`target_id`, `created_at`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
}

stock Staff_OnPlayerConnect(playerid)
{
    static const empty[E_STAFF_STATE];
    g_Staff[playerid] = empty;
}

stock Staff_OnPlayerDisconnect(playerid)
{
    if (IsValidDynamic3DTextLabel(g_Staff[playerid][sDutyTag]))
    {
        DestroyDynamic3DTextLabel(g_Staff[playerid][sDutyTag]);
    }

    // Admin vehicles are temporary and leave together with the admin who spawned them.
    for (new v = 1; v < MAX_VEHICLES; v++)
    {
        if (g_IsAdminVehicle[v] && g_AdminVehicleOwner[v] == playerid)
        {
            DestroyVehicle(v);
            g_IsAdminVehicle[v] = false;
        }
    }

    for (new i = 0; i < MAX_REPORTS; i++)
    {
        if (g_Reports[i][rUsed] && (g_Reports[i][rReporter] == playerid || g_Reports[i][rTarget] == playerid))
        {
            g_Reports[i][rUsed] = false;
        }
    }

    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (g_Staff[i][sSpectating] && g_Staff[i][sSpecTarget] == playerid)
        {
            Msg_Admin(i, "Igrac kojeg ste posmatrali je napustio server.");
            Staff_StopSpectating(i);
        }
    }
}

stock Staff_OnPlayerSpawn(playerid)
{
    if (g_Staff[playerid][sRestoreAfterSpec])
    {
        g_Staff[playerid][sRestoreAfterSpec] = false;
        Utils_MoveInto(playerid, g_Staff[playerid][sSpecX], g_Staff[playerid][sSpecY], g_Staff[playerid][sSpecZ], g_Staff[playerid][sSpecA],
            g_Staff[playerid][sSpecInterior], g_Staff[playerid][sSpecWorld]);
    }
    Staff_UpdateLook(playerid);
}

stock Staff_OnPlayerStateChange(playerid, PLAYER_STATE:newstate)
{
    // Admin vehicles are tools: anyone may ride along, only admins may drive.
    if (newstate == PLAYER_STATE_DRIVER && g_IsAdminVehicle[GetPlayerVehicleID(playerid)] && !Staff_IsAdmin(playerid))
    {
        RemovePlayerFromVehicle(playerid);
        Msg_Error(playerid, "Ovo je admin vozilo. Mozete se voziti kao putnik, ali ga ne mozete voziti.");
    }

    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (!g_Staff[i][sSpectating] || g_Staff[i][sSpecTarget] != playerid)
        {
            continue;
        }

        if (newstate == PLAYER_STATE_DRIVER || newstate == PLAYER_STATE_PASSENGER)
        {
            PlayerSpectateVehicle(i, GetPlayerVehicleID(playerid));
        }
        else if (newstate == PLAYER_STATE_ONFOOT)
        {
            PlayerSpectatePlayer(i, playerid);
        }
    }
}

stock Staff_OnPlayerInteriorChange(playerid, newinteriorid)
{
    #pragma unused newinteriorid
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (g_Staff[i][sSpectating] && g_Staff[i][sSpecTarget] == playerid)
        {
            Staff_SpecFollow(i);
        }
    }
}

// Custom interiors are streamer objects, so they are loaded around the target rather than the spot the admin left;
// this also follows world changes, which have no callback (one door to another).
stock Staff_SpecFollow(playerid)
{
    new targetid = g_Staff[playerid][sSpecTarget], interior = GetPlayerInterior(targetid), world = GetPlayerVirtualWorld(targetid);
    if (GetPlayerInterior(playerid) != interior)
    {
        SetPlayerInterior(playerid, interior);
    }
    if (GetPlayerVirtualWorld(playerid) != world)
    {
        SetPlayerVirtualWorld(playerid, world);
    }

    new Float:x, Float:y, Float:z;
    GetPlayerPos(targetid, x, y, z);
    Streamer_UpdateEx(playerid, x, y, z, world, interior, STREAMER_TYPE_OBJECT, -1, 0);
}

stock bool:Staff_IsAdmin(playerid)
{
    return g_Player[playerid][pAdminLevel] > 0;
}

stock bool:Staff_IsHelper(playerid)
{
    return g_Player[playerid][pHelperLevel] > 0;
}

stock bool:Staff_IsStaff(playerid)
{
    return Staff_IsAdmin(playerid) || Staff_IsHelper(playerid);
}

stock bool:Staff_IsAdminOnDuty(playerid)
{
    return g_Staff[playerid][sAdminDuty] || g_Player[playerid][pAdminLevel] >= ADMIN_LEVEL_OWNER;
}

// Admin commands need the level and, except for the owner, admin duty.
stock bool:Staff_CheckAdmin(playerid, level)
{
    if (g_Player[playerid][pAdminLevel] < level)
    {
        Msg_Error(playerid, "Nemate dozvolu za ovu komandu.");
        return false;
    }
    if (!Staff_IsAdminOnDuty(playerid))
    {
        Msg_Error(playerid, "Admin komande rade samo na duznosti. Ukljucite je sa /aduty.");
        return false;
    }
    return true;
}

// Helper commands are open to helpers on duty with the level, and to every admin.
stock bool:Staff_CheckHelper(playerid, level)
{
    if (Staff_IsAdmin(playerid))
    {
        return true;
    }

    if (g_Player[playerid][pHelperLevel] < level)
    {
        Msg_Error(playerid, "Nemate dozvolu za ovu komandu.");
        return false;
    }
    if (!g_Staff[playerid][sHelperDuty])
    {
        Msg_Error(playerid, "Helper komande rade samo na duznosti. Ukljucite je sa /hduty.");
        return false;
    }
    return true;
}

// self_error is the message shown when the command targets the caller; empty means targeting yourself is allowed.
stock bool:Staff_CheckTarget(playerid, targetid, const self_error[] = "")
{
    if (targetid == INVALID_PLAYER_ID || !Account_IsLoggedIn(targetid))
    {
        Msg_Error(playerid, "Taj igrac nije pronadjen.");
        return false;
    }
    if (self_error[0] && targetid == playerid)
    {
        Msg_Error(playerid, "%s", self_error);
        return false;
    }
    return true;
}

// Reads a single "[id or name]" parameter and validates it. Prints the usage or error itself;
// returns INVALID_PLAYER_ID when the command should stop.
stock Staff_ParseTarget(playerid, const params[], const usage[], const self_error[] = "")
{
    new targetid;
    if (sscanf(params, "u", targetid))
    {
        Msg_Usage(playerid, "%s", usage);
        return INVALID_PLAYER_ID;
    }
    if (!Staff_CheckTarget(playerid, targetid, self_error))
    {
        return INVALID_PLAYER_ID;
    }
    return targetid;
}

// Lower staff can never act on someone of equal or higher admin level.
stock bool:Staff_CheckOutranks(playerid, targetid)
{
    if (targetid != playerid && g_Player[targetid][pAdminLevel] >= g_Player[playerid][pAdminLevel])
    {
        Msg_Error(playerid, "Ne mozete ovo uraditi adminu istog ili viseg nivoa.");
        return false;
    }
    return true;
}

stock Staff_GetAdminRank(playerid, dest[], size = sizeof(dest))
{
    strcopy(dest, g_AdminRankNames[g_Player[playerid][pAdminLevel]], size);
}

stock Staff_GetHelperRank(playerid, dest[], size = sizeof(dest))
{
    strcopy(dest, g_HelperRankNames[g_Player[playerid][pHelperLevel]], size);
}

// Names stay white; staff on duty are recognised by a tag floating above the name instead.
// Admins on duty also wear the staff outfit; the character's own skin (pSkin) is never overwritten.
stock Staff_UpdateLook(playerid)
{
    SetPlayerColor(playerid, COLOR_PLAYER_NAME);

    if (g_Staff[playerid][sAdminDuty])
    {
        SetPlayerSkin(playerid, (g_Player[playerid][pGender] == GENDER_FEMALE) ? ADMIN_DUTY_SKIN_FEMALE : ADMIN_DUTY_SKIN_MALE);
    }
    else if (GetPlayerSkin(playerid) != g_Player[playerid][pSkin])
    {
        SetPlayerSkin(playerid, g_Player[playerid][pSkin]);
    }

    // Only one tag is shown: staff on duty first, then VIP (unless the VIP hid it with /vtag).
    if (g_Staff[playerid][sAdminDuty])
    {
        Staff_SetTag(playerid, (g_Player[playerid][pAdminLevel] >= ADMIN_LEVEL_OWNER) ? ("[VLASNIK]") : ("[ADMIN]"), COLOR_ADMIN_TAG);
    }
    else if (g_Staff[playerid][sHelperDuty])
    {
        Staff_SetTag(playerid, "[HELPER]", COLOR_HELPER_TAG);
    }
    else if (Vip_IsVip(playerid) && !g_Vip[playerid][vTagHidden])
    {
        Staff_SetTag(playerid, "[VIP]", g_VipColors[Vip_GetLevel(playerid)]);
    }
    else
    {
        Staff_SetTag(playerid, "", 0);
    }
}

// An empty text removes the tag.
stock Staff_SetTag(playerid, const text[], color)
{
    if (IsValidDynamic3DTextLabel(g_Staff[playerid][sDutyTag]))
    {
        DestroyDynamic3DTextLabel(g_Staff[playerid][sDutyTag]);
    }
    g_Staff[playerid][sDutyTag] = STREAMER_TAG_3D_TEXT_LABEL:INVALID_STREAMER_ID;

    if (text[0])
    {
        g_Staff[playerid][sDutyTag] = CreateDynamic3DTextLabel(text, color, 0.0, 0.0, STAFF_TAG_OFFSET_Z, STAFF_TAG_DRAW_DISTANCE, playerid, INVALID_VEHICLE_ID, 1);
    }
}

stock Staff_NotifyTarget(adminid, targetid, const action[])
{
    if (adminid == targetid)
    {
        return;
    }

    Msg_Admin(targetid, "Admin %s %s", Account_Name(adminid), action);
}

// The tag follows whoever acted: an admin's action reads [ADMIN], a helper's reads [HELPER].
stock Staff_MessageAs(actorid, targetid, const message[])
{
    if (Staff_IsAdmin(actorid))
    {
        Msg_Admin(targetid, "%s", message);
    }
    else
    {
        Msg_Helper(targetid, "%s", message);
    }
}

stock Staff_SendToAdmins(const message[], E_CHANNEL:channel = CHANNEL_ADMIN)
{
    Channel_Send(channel, COLOR_ADMIN, message);
}

// Helpers and admins together; used for questions and the helper chat.
stock Staff_SendToHelpers(const message[], E_CHANNEL:channel = CHANNEL_HELPER)
{
    Channel_Send(channel, COLOR_HELPER, message);
}

stock Staff_CountOnDutyHelpers()
{
    new count;
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (Account_IsLoggedIn(i) && g_Staff[i][sHelperDuty])
        {
            count++;
        }
    }
    return count;
}

stock Staff_Log(adminid, const action[], targetid, const details[])
{
    new query[512];
    if (targetid == INVALID_PLAYER_ID)
    {
        mysql_format(g_SQL, query, sizeof(query),
            "INSERT INTO `admin_logs` (`admin_id`, `admin_name`, `action`, `details`) VALUES (%d, '%e', '%e', '%e')",
            g_Player[adminid][pDbId], g_Player[adminid][pName], action, details);
    }
    else
    {
        mysql_format(g_SQL, query, sizeof(query),
            "INSERT INTO `admin_logs` (`admin_id`, `admin_name`, `action`, `target_id`, `target_name`, `details`) " \
            "VALUES (%d, '%e', '%e', %d, '%e', '%e')",
            g_Player[adminid][pDbId], g_Player[adminid][pName], action,
            g_Player[targetid][pDbId], g_Player[targetid][pName], details);
    }
    mysql_tquery(g_SQL, query);
}

stock Staff_TeleportTo(playerid, Float:x, Float:y, Float:z, interior, world)
{
    // A spectator has no body to move: the camera would stay on the target in the wrong interior.
    // The teleport becomes the spot they come back to, and spectating ends.
    if (g_Staff[playerid][sSpectating])
    {
        g_Staff[playerid][sSpecX] = x;
        g_Staff[playerid][sSpecY] = y;
        g_Staff[playerid][sSpecZ] = z;
        g_Staff[playerid][sSpecInterior] = interior;
        g_Staff[playerid][sSpecWorld] = world;
        Staff_StopSpectating(playerid);
        return;
    }

    if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
    {
        new vehicleid = GetPlayerVehicleID(playerid);
        AC_ExpectTeleport(playerid, x, y, z);
        SetVehiclePos(vehicleid, x, y, z);
        LinkVehicleToInterior(vehicleid, interior);
        SetVehicleVirtualWorld(vehicleid, world);
        SetPlayerInterior(playerid, interior);
        SetPlayerVirtualWorld(playerid, world);
        return;
    }

    new Float:angle;
    GetPlayerFacingAngle(playerid, angle);
    Utils_MoveInto(playerid, x, y, z, angle, interior, world, !g_Staff[playerid][sFrozen]);
}

stock Staff_StartSpectating(playerid, targetid)
{
    if (!g_Staff[playerid][sSpectating])
    {
        GetPlayerPos(playerid, g_Staff[playerid][sSpecX], g_Staff[playerid][sSpecY], g_Staff[playerid][sSpecZ]);
        GetPlayerFacingAngle(playerid, g_Staff[playerid][sSpecA]);
        g_Staff[playerid][sSpecInterior] = GetPlayerInterior(playerid);
        g_Staff[playerid][sSpecWorld] = GetPlayerVirtualWorld(playerid);

        // While spectating the account save skips the position, so make sure the spot we return to is what gets saved.
        g_Player[playerid][pPosX] = g_Staff[playerid][sSpecX];
        g_Player[playerid][pPosY] = g_Staff[playerid][sSpecY];
        g_Player[playerid][pPosZ] = g_Staff[playerid][sSpecZ];
        g_Player[playerid][pPosA] = g_Staff[playerid][sSpecA];
        g_Player[playerid][pInterior] = g_Staff[playerid][sSpecInterior];
        g_Player[playerid][pVirtualWorld] = g_Staff[playerid][sSpecWorld];
    }

    g_Staff[playerid][sSpectating] = true;
    g_Staff[playerid][sSpecTarget] = targetid;

    TogglePlayerSpectating(playerid, true);
    AC_ShowSpecInfo(playerid);
    Staff_SpecFollow(playerid);

    if (IsPlayerInAnyVehicle(targetid))
    {
        PlayerSpectateVehicle(playerid, GetPlayerVehicleID(targetid));
    }
    else
    {
        PlayerSpectatePlayer(playerid, targetid);
    }
}

stock Staff_StopSpectating(playerid)
{
    g_Staff[playerid][sSpectating] = false;
    g_Staff[playerid][sRestoreAfterSpec] = true;

    SetSpawnInfo(playerid, NO_TEAM, g_Player[playerid][pSkin],
        g_Staff[playerid][sSpecX], g_Staff[playerid][sSpecY], g_Staff[playerid][sSpecZ], g_Staff[playerid][sSpecA]);
    AC_HideSpecInfo(playerid);
    TogglePlayerSpectating(playerid, false);
}

stock Staff_FindFreeReport()
{
    for (new i = 0; i < MAX_REPORTS; i++)
    {
        if (!g_Reports[i][rUsed])
        {
            return i;
        }
    }
    return -1;
}

stock bool:Staff_OnDialogResponse(playerid, dialogid, response, listitem)
{
    switch (dialogid)
    {
        case DIALOG_ADMIN_TELEPORT:
        {
            if (!response || listitem < 0 || listitem >= sizeof(g_AdminTeleports) || !Staff_IsAdminOnDuty(playerid))
            {
                return true;
            }

            Staff_TeleportTo(playerid, g_AdminTeleports[listitem][tX], g_AdminTeleports[listitem][tY], g_AdminTeleports[listitem][tZ],
                g_AdminTeleports[listitem][tInterior], 0);
            Msg_Admin(playerid, "Teleportovani ste: %s", g_AdminTeleports[listitem][tName]);
        }
        case DIALOG_ADMIN_TELEPORT_GPS:
        {
            new slot = Gps_GetListed(playerid, listitem);
            if (!response || slot == -1 || !Staff_IsAdminOnDuty(playerid))
            {
                return true;
            }

            Staff_TeleportTo(playerid, g_Gps[slot][gX], g_Gps[slot][gY], g_Gps[slot][gZ], 0, 0);
            Msg_Admin(playerid, "Teleportovani ste: %s", g_Gps[slot][gName]);
        }
        case DIALOG_ADMIN_REPORTS, DIALOG_ADMIN_INFO:
        {
        }
        default:
        {
            return false;
        }
    }
    return true;
}

// Seconds left before a support command can be used again; VIP status shortens the wait.
stock Staff_CooldownLeft(playerid, last_used, base_seconds)
{
    return last_used + Vip_Cooldown(playerid, base_seconds) - gettime();
}
