cmd:report(playerid, params[])
{
    new targetid, reason[96];
    if (sscanf(params, "us[96]", targetid, reason))
    {
        Msg_Usage(playerid, "/report [id ili ime] [razlog]");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid, "Ne mozete prijaviti samog sebe."))
    {
        return 1;
    }

    new wait = Staff_CooldownLeft(playerid, g_Staff[playerid][sLastReport], REPORT_COOLDOWN_S);
    if (wait > 0)
    {
        Msg_Error(playerid, "Mozete poslati novi report za %d sekundi.", wait);
        return 1;
    }

    new slot = Staff_FindFreeReport();
    if (slot == -1)
    {
        Msg_Error(playerid, "Trenutno ima previse otvorenih reportova, pokusajte malo kasnije.");
        return 1;
    }

    g_Reports[slot][rUsed] = true;
    g_Reports[slot][rReporter] = playerid;
    g_Reports[slot][rTarget] = targetid;
    g_Reports[slot][rCreatedAt] = gettime();
    strcopy(g_Reports[slot][rReason], reason, 96);
    g_Staff[playerid][sLastReport] = gettime();

    new message[192];
    format(message, sizeof(message), "[REPORT #%d] %s [%d] prijavljuje %s [%d]: %s", slot + 1, Account_Name(playerid), playerid, Account_Name(targetid), targetid, reason);
    Staff_SendToAdmins(message, CHANNEL_REPORTS);
    format(message, sizeof(message), "[REPORT #%d] Prihvati: /ar %d    Odbij: /rr %d [razlog]", slot + 1, slot + 1, slot + 1);
    Staff_SendToAdmins(message, CHANNEL_REPORTS);

    Msg_Success(playerid, "Vas report je poslat administraciji.");
    return 1;
}

cmd:ask(playerid, params[])
{
    if (isnull(params))
    {
        Msg_Usage(playerid, "/ask [pitanje]");
        return 1;
    }

    new wait = Staff_CooldownLeft(playerid, g_Staff[playerid][sLastAsk], ASK_COOLDOWN_S);
    if (wait > 0)
    {
        Msg_Error(playerid, "Mozete postaviti novo pitanje za %d sekundi.", wait);
        return 1;
    }
    g_Staff[playerid][sLastAsk] = gettime();
    g_Staff[playerid][sHasOpenQuestion] = true;

    new message[192];
    format(message, sizeof(message), "[PITANJE] %s [%d]: %s", Account_Name(playerid), playerid, params);

    // With no helper on duty the question still reaches someone: every helper and admin gets it.
    Staff_SendToHelpers(message, CHANNEL_QUESTIONS);
    format(message, sizeof(message), "[PITANJE] Odgovor: /odg %d [tekst]", playerid);
    Staff_SendToHelpers(message, CHANNEL_QUESTIONS);

    if (Staff_CountOnDutyHelpers() == 0)
    {
        Msg_Success(playerid, "Pitanje je poslato. Trenutno nema helpera na duznosti, odgovorice prvi slobodan clan tima.");
    }
    else
    {
        Msg_Success(playerid, "Pitanje je poslato helperima.");
    }
    return 1;
}

cmd:n(playerid, params[])
{
    if (g_Player[playerid][pLevel] > NEWBIE_MAX_PLAYER_LEVEL && !Staff_IsStaff(playerid))
    {
        Msg_Error(playerid, "Newbie chat je samo za nove igrace (do nivoa %d) i clanove tima.", NEWBIE_MAX_PLAYER_LEVEL);
        return 1;
    }
    if (isnull(params))
    {
        Msg_Usage(playerid, "/n [pitanje ili odgovor] (newbie chat)");
        return 1;
    }
    if (!Channel_IsOn(playerid, CHANNEL_NEWBIE))
    {
        Msg_Error(playerid, "Iskljucili ste newbie chat. Ukljucite ga sa /tognewbie.");
        return 1;
    }
    if (g_Player[playerid][pNewbieMuteUntil] > gettime())
    {
        Msg_Error(playerid, "Zabranjeno vam je pisanje u newbie chat jos %d min.", Punish_MinutesLeft(g_Player[playerid][pNewbieMuteUntil]));
        return 1;
    }

    if (!Staff_IsStaff(playerid))
    {
        new wait = Staff_CooldownLeft(playerid, g_Staff[playerid][sLastNewbie], NEWBIE_COOLDOWN_S);
        if (wait > 0)
        {
            Msg_Error(playerid, "Mozete pisati u newbie chat za %d sekundi.", wait);
            return 1;
        }
        g_Staff[playerid][sLastNewbie] = gettime();
    }

    new rank[24], message[192];

    if (Staff_IsAdmin(playerid))
    {
        strcopy(rank, "Admin");
    }
    else if (Staff_IsHelper(playerid))
    {
        strcopy(rank, "Helper");
    }
    else
    {
        strcopy(rank, "Igrac");
    }

    format(message, sizeof(message), "[N] %s %s [%d]: %s", rank, Chat_ColoredName(playerid, C_NEWBIE), playerid, Chat_CleanText(params));
    Channel_Send(CHANNEL_NEWBIE, COLOR_NEWBIE, message, true);
    return 1;
}

cmd:admins(playerid, params[])
{
    new body[1024], line[96], count;

    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (!Account_IsLoggedIn(i) || !Staff_IsAdmin(i))
        {
            continue;
        }

        format(line, sizeof(line), C_ADMIN "%s " C_WHITE "%s [%d]%s\n",
            g_AdminRankNames[g_Player[i][pAdminLevel]], Account_Name(i), i, g_Staff[i][sAdminDuty] ? (C_GREEN " - na duznosti") : (C_GREY " - nije na duznosti"));
        strcat(body, line);
        count++;
    }

    if (count == 0)
    {
        strcopy(body, C_WHITE "Trenutno nema admina na serveru.");
    }
    ShowPlayerDialog(playerid, DIALOG_ADMIN_INFO, DIALOG_STYLE_MSGBOX, SERVER_NAME " - Admini online", body, "Zatvori", "");
    return 1;
}

cmd:helpers(playerid, params[])
{
    new body[1024], line[96], count;

    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (!Account_IsLoggedIn(i) || !Staff_IsHelper(i))
        {
            continue;
        }

        format(line, sizeof(line), C_HELPER "%s " C_WHITE "%s [%d]%s\n",
            g_HelperRankNames[g_Player[i][pHelperLevel]], Account_Name(i), i, g_Staff[i][sHelperDuty] ? (C_GREEN " - na duznosti") : (C_GREY " - nije na duznosti"));
        strcat(body, line);
        count++;
    }

    if (count == 0)
    {
        strcopy(body, C_WHITE "Trenutno nema helpera na serveru. Pitanja mozete postaviti preko /ask.");
    }
    ShowPlayerDialog(playerid, DIALOG_ADMIN_INFO, DIALOG_STYLE_MSGBOX, SERVER_NAME " - Helperi online", body, "Zatvori", "");
    return 1;
}
flags:n(CMD_FLAG_CHAT)
flags:report(CMD_FLAG_JAIL_OK)
flags:ask(CMD_FLAG_JAIL_OK)
flags:admins(CMD_FLAG_JAIL_OK)
flags:helpers(CMD_FLAG_JAIL_OK)
