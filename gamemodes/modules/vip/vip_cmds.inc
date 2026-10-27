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
        "{CD7F32}Bronze VIP\n" C_WHITE "- VIP chat (/vc) i spisak VIP igraca (/vips)\n- 25%% kraca pauza za /report, /ask i /n\n\n" \
        "{C0C0C0}Silver VIP\n" C_WHITE "- sve iz Bronze VIP\n- 50%% kraca pauza za /report, /ask i /n\n\n" \
        "{FFD700}Gold VIP\n" C_WHITE "- sve iz Silver VIP\n- dodatne pogodnosti stizu sa novim sistemima\n\n" \
        C_GREY "VIP traje mesecno i ne daje prednost u igri ni admin ovlascenja.\nZa VIP se javite vlasniku servera.",
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

flags:vc(CMD_FLAG_CHAT)
flags:vip(CMD_FLAG_JAIL_OK)
flags:vips(CMD_FLAG_JAIL_OK)
