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
    if (targetid != playerid && !Chat_IsInRange(playerid, targetid, GIVE_CASH_RADIUS))
    {
        Msg_Error(playerid, "Taj igrac nije pored vas.");
        return 1;
    }

    SendClientMessage(targetid, COLOR_SERVER, "_______ LICNA KARTA _______");
    SendClientMessage(targetid, COLOR_WHITE, "Ime i prezime: %s", Account_Name(playerid));
    SendClientMessage(targetid, COLOR_WHITE, "Pol: %s | Godine: %d", g_Player[playerid][pGender] == GENDER_FEMALE ? ("zenski") : ("muski"), g_Player[playerid][pAge]);
    SendClientMessage(targetid, COLOR_WHITE, "Prebivaliste: Los Santos, San Andreas");

    if (targetid != playerid)
    {
        new action[96];
        format(action, sizeof(action), "vadi licnu kartu i pokazuje je osobi %s.", Account_Name(targetid));
        Chat_Action(playerid, action);
    }
    return 1;
}
