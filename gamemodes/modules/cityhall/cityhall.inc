cmd:opstina(playerid, params[])
{
    if (!Point_Require(playerid, POINT_CITYHALL))
    {
        return 1;
    }

    new list[160], price[24];
    if (g_Player[playerid][pHasIdCard]) strcopy(price, C_GREEN "imate je");
    else format(price, sizeof(price), "$%d", ID_CARD_PRICE);
    format(list, sizeof(list), "Licna karta\t%s\nPrijava na posao\t" C_GREY "uskoro", price);
    ShowPlayerDialog(playerid, DIALOG_CITYHALL, DIALOG_STYLE_TABLIST, SERVER_NAME " - Opstina Los Santos", list, "Izaberi", "Zatvori");
    return 1;
}

stock bool:CityHall_OnDialogResponse(playerid, dialogid, response, listitem)
{
    if (dialogid != DIALOG_CITYHALL)
    {
        return false;
    }
    if (!response || !Point_Require(playerid, POINT_CITYHALL))
    {
        return true;
    }

    if (listitem == 0)
    {
        if (g_Player[playerid][pHasIdCard])
        {
            Msg_Info(playerid, "Vec imate licnu kartu. Pokazujete je sa /licna [id].");
            return true;
        }
        if (!Money_Take(playerid, ID_CARD_PRICE, "licna karta"))
        {
            return true;
        }
        g_Player[playerid][pHasIdCard] = true;
        Account_Save(playerid);
        Msg_Success(playerid, "Izvadili ste licnu kartu za $%d. Pokazujete je sa /licna [id].", ID_CARD_PRICE);
    }
    else
    {
        Msg_Info(playerid, "Prijava na posao stize zajedno sa poslovima.");
    }
    return true;
}

cmd:licna(playerid, params[])
{
    if (!g_Player[playerid][pHasIdCard])
    {
        Msg_Error(playerid, "Nemate licnu kartu. Izvadite je u opstini.");
        return 1;
    }

    new targetid = playerid;
    if (!isnull(params) && sscanf(params, "u", targetid))
    {
        Msg_Usage(playerid, "/licna [id ili ime] (bez parametra gledate svoju)");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }
    if (targetid != playerid && !Chat_CheckBeside(playerid, targetid, GIVE_CASH_RADIUS))
    {
        return 1;
    }

    new body[256];
    format(body, sizeof(body),
        C_GREY "Broj licne karte: " C_WHITE "%06d\n\n" \
        C_GREY "Ime i prezime: " C_WHITE "%s\n" \
        C_GREY "Pol: " C_WHITE "%s\n" \
        C_GREY "Godine: " C_WHITE "%d\n" \
        C_GREY "Prebivaliste: " C_WHITE "Los Santos, San Andreas",
        g_Player[playerid][pDbId], Account_Name(playerid), g_Player[playerid][pGender] == GENDER_FEMALE ? ("zenski") : ("muski"), g_Player[playerid][pAge]);
    ShowPlayerDialog(targetid, DIALOG_ADMIN_INFO, DIALOG_STYLE_MSGBOX, "Licna karta", body, "Vrati", "");

    if (targetid != playerid)
    {
        new action[96];
        format(action, sizeof(action), "vadi licnu kartu i pokazuje je osobi %s.", Account_Name(targetid));
        Chat_Action(playerid, action);
    }
    return 1;
}

cmd:setlicna(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }
    new targetid, value;
    if (sscanf(params, "ud", targetid, value) || (value != 0 && value != 1))
    {
        Msg_Usage(playerid, "/setlicna [id ili ime] [1 daje, 0 oduzima licnu kartu]");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid))
    {
        return 1;
    }
    g_Player[targetid][pHasIdCard] = value == 1;
    Account_Save(targetid);
    Staff_NotifyTarget(playerid, targetid, value == 1 ? ("vam je dao licnu kartu.") : ("vam je oduzeo licnu kartu."));
    Staff_Log(playerid, "setidcard", targetid, value == 1 ? ("daje") : ("oduzima"));
    Msg_Admin(playerid, "Igrac %s %s licnu kartu.", Account_Name(targetid), value == 1 ? ("sada ima") : ("vise nema"));
    return 1;
}
