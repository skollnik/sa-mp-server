cmd:hduty(playerid, params[])
{
    if (!Staff_IsHelper(playerid))
    {
        Msg_Error(playerid, "Nemate dozvolu za ovu komandu.");
        return 1;
    }

    g_Staff[playerid][sHelperDuty] = !g_Staff[playerid][sHelperDuty];
    Staff_UpdateLook(playerid);

    new message[128];
    format(message, sizeof(message), "[H] %s je %s helper duznosti.", Account_Name(playerid), g_Staff[playerid][sHelperDuty] ? ("sada na") : ("napustio"));
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
    if (!Channel_IsOn(playerid, CHANNEL_HELPER))
    {
        Msg_Error(playerid, "Iskljucili ste helper chat. Ukljucite ga u /kanali.");
        return 1;
    }
    if (isnull(params))
    {
        Msg_Usage(playerid, "/hc [tekst] (helper chat)");
        return 1;
    }

    new rank[24], message[192];
    if (Staff_IsAdmin(playerid))
    {
        Staff_GetAdminRank(playerid, rank);
    }
    else
    {
        Staff_GetHelperRank(playerid, rank);
    }

    format(message, sizeof(message), "[H] %s %s: %s", rank, Account_Name(playerid), params);
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
    if (!Staff_CheckTarget(playerid, targetid, "Ne mozete odgovoriti samom sebi."))
    {
        return 1;
    }

    new message[192];

    format(message, sizeof(message), "[ODGOVOR] %s: %s", Account_Name(playerid), text);
    Chat_Send(targetid, COLOR_HELPER, message);

    // The rest of the team sees the answer so nobody answers the same question twice.
    format(message, sizeof(message), "[H] %s je odgovorio igracu %s [%d]: %s", Account_Name(playerid), Account_Name(targetid), targetid, text);
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

    new targetid = Staff_ParseTarget(playerid, params, "/goto [id ili ime]", "Ne mozete se teleportovati do samog sebe.");
    if (targetid == INVALID_PLAYER_ID)
    {
        return 1;
    }

    new Float:x, Float:y, Float:z;
    GetPlayerPos(targetid, x, y, z);
    Staff_TeleportTo(playerid, x + 1.0, y, z, GetPlayerInterior(targetid), GetPlayerVirtualWorld(targetid));

    if (Staff_IsAdmin(playerid))
    {
        Msg_Admin(playerid, "Teleportovali ste se do igraca %s.", Account_Name(targetid));
    }
    else
    {
        Msg_Helper(playerid, "Teleportovali ste se do igraca %s.", Account_Name(targetid));
    }
    return 1;
}
