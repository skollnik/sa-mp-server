cmd:vip(playerid, params[])
{
    new body[1536], status[96];
    if (Vip_IsVip(playerid))
    {
        format(status, sizeof(status), C_WHITE "Vas status: %s%s" C_WHITE ", jos %d dana.\n\n",
            g_VipColorTags[Vip_GetLevel(playerid)], g_VipNames[Vip_GetLevel(playerid)], Vip_DaysLeft(playerid));
    }
    else
    {
        strcopy(status, C_WHITE "Trenutno nemate VIP status.\n\n");
    }

    format(body, sizeof(body),
        "%s" \
        "{CD7F32}Bronze VIP\n" C_WHITE "- oznaka [VIP] iznad glave (/vtag sakriva) i ime u boji u OOC chatovima\n" \
        "- VIP chat (/vc), spisak VIP igraca (/vips)\n- /vskin (VIP skinovi), /vtime i /vweather (licno vreme i vremenske prilike)\n" \
        "- 25%% kraca pauza za /report, /ask i /n, rezervisano mesto kad je server pun\n\n" \
        "{C0C0C0}Silver VIP\n" C_WHITE "- sve iz Bronze VIP\n- /vfix (popravka vozila, na 10 min), /vcolor (farbanje vozila)\n- 50%% kraca pauza za /report, /ask i /n\n\n" \
        "{FFD700}Gold VIP\n" C_WHITE "- sve iz Silver VIP\n- /vgoto (do igraca, na 5 min), /vheal (HP na 100, na 15 min)\n\n" \
        C_GREY "/vgoto, /vfix i /vheal ne rade minut posle primljene stete i ne rade u zatvoru.\n" \
        "Nove pogodnosti stizu sa novim sistemima. VIP ne daje admin ovlascenja.\nZa VIP se javite vlasniku servera.",
        status);
    ShowPlayerDialog(playerid, DIALOG_ADMIN_INFO, DIALOG_STYLE_MSGBOX, SERVER_NAME " - VIP", body, "Zatvori", "");
    return 1;
}

cmd:vc(playerid, params[])
{
    if (!Vip_IsVip(playerid) && !Staff_IsAdmin(playerid))
    {
        Msg_Error(playerid, "Nemate dozvolu za ovu komandu.");
        return 1;
    }
    if (!Channel_IsOn(playerid, CHANNEL_VIP))
    {
        Msg_Error(playerid, "Iskljucili ste VIP chat. Ukljucite ga u /kanali.");
        return 1;
    }
    if (isnull(params))
    {
        Msg_Usage(playerid, "/vc [tekst] (VIP chat)");
        return 1;
    }

    new message[192];
    format(message, sizeof(message), "[VIP] %s %s: %s",
        Vip_IsVip(playerid) ? g_VipNames[Vip_GetLevel(playerid)] : ("Admin"), Account_Name(playerid), params);
    Vip_SendToVips(message);
    return 1;
}

cmd:vips(playerid, params[])
{
    new body[1024], line[96], count;
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (!Account_IsLoggedIn(i) || !Vip_IsVip(i))
        {
            continue;
        }

        format(line, sizeof(line), "%s%s " C_WHITE "%s [%d]\n", g_VipColorTags[Vip_GetLevel(i)], g_VipNames[Vip_GetLevel(i)], Account_Name(i), i);
        strcat(body, line);
        count++;
    }

    if (count == 0)
    {
        strcopy(body, C_WHITE "Trenutno nema VIP igraca na serveru.");
    }
    ShowPlayerDialog(playerid, DIALOG_ADMIN_INFO, DIALOG_STYLE_MSGBOX, SERVER_NAME " - VIP igraci online", body, "Zatvori", "");
    return 1;
}

// Owner only: VIP is tied to payments, so nobody else hands it out.
cmd:setvip(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_OWNER))
    {
        return 1;
    }

    new targetid, level, days;
    if (sscanf(params, "udD(30)", targetid, level, days) || level < 0 || level > VIP_MAX_LEVEL || days < 1 || days > 365)
    {
        Msg_Usage(playerid, "/setvip [id ili ime] [nivo 0-3] [dana, podrazumevano 30] (1 Bronze, 2 Silver, 3 Gold, 0 skida VIP)");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }

    Vip_Grant(playerid, targetid, level, days);
    Staff_UpdateLook(targetid);

    new details[96];
    if (level == 0)
    {
        Msg_Admin(playerid, "Skinuli ste VIP status igracu %s.", Account_Name(targetid));
        Staff_NotifyTarget(playerid, targetid, "vam je skinuo VIP status.");
        strcopy(details, "skinut");
    }
    else
    {
        Msg_Admin(playerid, "Igrac %s sada ima %s jos %d dana.", Account_Name(targetid), g_VipNames[level], Vip_DaysLeft(targetid));
        format(details, sizeof(details), "vam je dodelio %s (%d dana). Pogodnosti: /vip", g_VipNames[level], Vip_DaysLeft(targetid));
        Staff_NotifyTarget(playerid, targetid, details);
        format(details, sizeof(details), "%s, %d dana", g_VipNames[level], days);
    }
    Staff_Log(playerid, "setvip", targetid, details);
    return 1;
}

cmd:vgoto(playerid, params[])
{
    if (!Vip_HasLevel(playerid, VIP_GOLD) || !Vip_CheckCooldown(playerid, g_Vip[playerid][vLastGoto], VIP_GOTO_COOLDOWN_S))
    {
        return 1;
    }

    new targetid = Staff_ParseTarget(playerid, params, "/vgoto [id ili ime]", "Ne mozete se teleportovati do samog sebe.");
    if (targetid == INVALID_PLAYER_ID)
    {
        return 1;
    }
    if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT || Vip_IsInCombat(playerid))
    {
        Msg_Error(playerid, "/vgoto radi samo peske i ne minut posle primljene stete.");
        return 1;
    }
    // Only to players outside: no shortcuts into interiors, other virtual worlds or to staff at work.
    new PLAYER_STATE:target_state = GetPlayerState(targetid);
    if (GetPlayerInterior(targetid) != 0 || GetPlayerVirtualWorld(targetid) != 0 || GetPlayerVirtualWorld(playerid) != 0
        || g_Staff[targetid][sAdminDuty] || g_Staff[targetid][sHelperDuty] || g_Staff[targetid][sSpectating]
        || (target_state != PLAYER_STATE_ONFOOT && target_state != PLAYER_STATE_DRIVER && target_state != PLAYER_STATE_PASSENGER))
    {
        Msg_Error(playerid, "Do tog igraca trenutno ne mozete da se teleportujete.");
        return 1;
    }

    new Float:x, Float:y, Float:z;
    GetPlayerPos(targetid, x, y, z);
    Staff_TeleportTo(playerid, x + 1.0, y, z, 0, 0);
    g_Vip[playerid][vLastGoto] = gettime();

    Msg_Info(playerid, "Teleportovali ste se do igraca %s.", Account_Name(targetid));
    Msg_Info(targetid, "VIP igrac %s se teleportovao do vas.", Account_Name(playerid));
    return 1;
}

cmd:vfix(playerid, params[])
{
    if (!Vip_HasLevel(playerid, VIP_SILVER) || !Vip_CheckCooldown(playerid, g_Vip[playerid][vLastFix], VIP_FIX_COOLDOWN_S))
    {
        return 1;
    }
    if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER || Vip_IsInCombat(playerid))
    {
        Msg_Error(playerid, "/vfix radi samo dok vozite i ne minut posle primljene stete.");
        return 1;
    }

    RepairVehicle(GetPlayerVehicleID(playerid));
    g_Vip[playerid][vLastFix] = gettime();
    Msg_Info(playerid, "Vozilo je popravljeno.");
    return 1;
}

cmd:vheal(playerid, params[])
{
    if (!Vip_HasLevel(playerid, VIP_GOLD) || !Vip_CheckCooldown(playerid, g_Vip[playerid][vLastHeal], VIP_HEAL_COOLDOWN_S))
    {
        return 1;
    }
    if (Vip_IsInCombat(playerid))
    {
        Msg_Error(playerid, "/vheal ne radi minut posle primljene stete.");
        return 1;
    }

    SetPlayerHealth(playerid, 100.0);
    g_Vip[playerid][vLastHeal] = gettime();
    Msg_Info(playerid, "HP je vracen na 100.");
    return 1;
}

cmd:vskin(playerid, params[])
{
    if (!Vip_HasLevel(playerid, VIP_BRONZE))
    {
        return 1;
    }

    if (g_Player[playerid][pGender] == GENDER_FEMALE)
    {
        Preview_Show(playerid, PREVIEW_VIP_SKIN, PREVIEW_KIND_SKIN, "VIP skinovi", g_VipSkinsFemale, sizeof(g_VipSkinsFemale));
    }
    else
    {
        Preview_Show(playerid, PREVIEW_VIP_SKIN, PREVIEW_KIND_SKIN, "VIP skinovi", g_VipSkinsMale, sizeof(g_VipSkinsMale));
    }
    return 1;
}

cmd:vtime(playerid, params[])
{
    if (!Vip_HasLevel(playerid, VIP_BRONZE))
    {
        return 1;
    }

    new hour;
    if (sscanf(params, "d", hour) || hour < -1 || hour > 23)
    {
        Msg_Usage(playerid, "/vtime [sat 0-23, ili -1 za serversko vreme] (vreme koje vidite samo vi)");
        return 1;
    }

    g_Vip[playerid][vPersonalTime] = hour != -1;
    if (hour == -1)
    {
        Time_Apply(playerid);
        Msg_Info(playerid, "Ponovo vidite serversko vreme.");
        return 1;
    }
    SetPlayerTime(playerid, hour, 0);
    Msg_Info(playerid, "Vase licno vreme je %02d:00. Serversko vreme: /vtime -1 ili ponovni ulazak.", hour);
    return 1;
}

cmd:vweather(playerid, params[])
{
    if (!Vip_HasLevel(playerid, VIP_BRONZE))
    {
        return 1;
    }

    new weather;
    if (sscanf(params, "d", weather) || weather < 0 || weather > 45)
    {
        Msg_Usage(playerid, "/vweather [id 0-45] (vremenske prilike koje vidite samo vi)");
        return 1;
    }

    SetPlayerWeather(playerid, weather);
    Msg_Info(playerid, "Vase licne vremenske prilike su postavljene na %d.", weather);
    return 1;
}

cmd:vcolor(playerid, params[])
{
    if (!Vip_HasLevel(playerid, VIP_SILVER))
    {
        return 1;
    }

    new color1, color2;
    if (sscanf(params, "dd", color1, color2) || color1 < 0 || color1 > 255 || color2 < 0 || color2 > 255)
    {
        Msg_Usage(playerid, "/vcolor [boja 1] [boja 2] (0-255, dok vozite)");
        return 1;
    }
    if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
    {
        Msg_Error(playerid, "Morate voziti vozilo.");
        return 1;
    }

    ChangeVehicleColours(GetPlayerVehicleID(playerid), color1, color2);
    Msg_Info(playerid, "Vozilo je ofarbano.");
    return 1;
}

cmd:vtag(playerid, params[])
{
    if (!Vip_HasLevel(playerid, VIP_BRONZE))
    {
        return 1;
    }

    g_Vip[playerid][vTagHidden] = !g_Vip[playerid][vTagHidden];
    Staff_UpdateLook(playerid);
    Msg_Info(playerid, "Oznaka [VIP] iznad glave je %s.", g_Vip[playerid][vTagHidden] ? ("sakrivena") : ("ponovo vidljiva"));
    return 1;
}

flags:vc(CMD_FLAG_CHAT)
flags:vip(CMD_FLAG_JAIL_OK)
flags:vips(CMD_FLAG_JAIL_OK)
flags:vtag(CMD_FLAG_JAIL_OK)
flags:vtime(CMD_FLAG_JAIL_OK)
flags:vweather(CMD_FLAG_JAIL_OK)
