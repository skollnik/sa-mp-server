cmd:hduty(playerid, params[])
{
    if (!Staff_IsHelper(playerid))
    {
        Msg_Error(playerid, "Nemate dozvolu za ovu komandu.");
        return 1;
    }

    g_Staff[playerid][sHelperDuty] = !g_Staff[playerid][sHelperDuty];
    Staff_UpdateNameColor(playerid);

    new name[MAX_PLAYER_NAME + 1], message[128];
    Account_GetDisplayName(playerid, name);
    format(message, sizeof(message), "[H] %s je %s helper duznosti.", name, g_Staff[playerid][sHelperDuty] ? ("sada na") : ("napustio"));
    Staff_SendToHelpers(message);
    return 1;
}

cmd:hc(playerid, params[])
{
    if (!Staff_IsStaff(playerid))
    {
        Msg_Error(playerid, "Nemate dozvolu za ovu komandu.");
        return 1;
    }
    if (isnull(params))
    {
        Msg_Usage(playerid, "/hc [tekst] (helper chat)");
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1], rank[24], message[192];
    Account_GetDisplayName(playerid, name);
    if (Staff_IsAdmin(playerid))
    {
        Staff_GetAdminRank(playerid, rank);
    }
    else
    {
        Staff_GetHelperRank(playerid, rank);
    }

    format(message, sizeof(message), "[H] %s %s: %s", rank, name, params);
    Staff_SendToHelpers(message);
    return 1;
}

cmd:odg(playerid, params[])
{
    if (!Staff_CheckHelper(playerid, HELPER_LEVEL_HELPER))
    {
        return 1;
    }

    new targetid, text[128];
    if (sscanf(params, "us[128]", targetid, text))
    {
        Msg_Usage(playerid, "/odg [id ili ime] [odgovor]");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }

    new name[MAX_PLAYER_NAME + 1], target_name[MAX_PLAYER_NAME + 1], message[192];
    Account_GetDisplayName(playerid, name);
    Account_GetDisplayName(targetid, target_name);

    format(message, sizeof(message), "[ODGOVOR] %s: %s", name, text);
    Chat_Send(targetid, COLOR_HELPER, message);

    // The rest of the team sees the answer so nobody answers the same question twice.
    format(message, sizeof(message), "[H] %s je odgovorio igracu %s [%d]: %s", name, target_name, targetid, text);
    Staff_SendToHelpers(message);

    g_Staff[targetid][sHasOpenQuestion] = false;
    return 1;
}

cmd:goto(playerid, params[])
{
    if (!Staff_IsAdmin(playerid) && !Staff_CheckHelper(playerid, HELPER_LEVEL_SENIOR))
    {
        return 1;
    }
    if (Staff_IsAdmin(playerid) && !Staff_CheckAdmin(playerid, ADMIN_LEVEL_TRIAL))
    {
        return 1;
    }

    new targetid;
    if (sscanf(params, "u", targetid))
    {
        Msg_Usage(playerid, "/goto [id ili ime]");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }

    new Float:x, Float:y, Float:z;
    GetPlayerPos(targetid, x, y, z);
    Staff_TeleportTo(playerid, x + 1.0, y, z, GetPlayerInterior(targetid), GetPlayerVirtualWorld(targetid));

    new target_name[MAX_PLAYER_NAME + 1];
    Account_GetDisplayName(targetid, target_name);
    if (Staff_IsAdmin(playerid))
    {
        Msg_Admin(playerid, "Teleportovali ste se do igraca %s.", target_name);
    }
    else
    {
        Msg_Helper(playerid, "Teleportovali ste se do igraca %s.", target_name);
    }
    return 1;
}
