cmd:daj(playerid, params[])
{
    new targetid, amount;
    if (sscanf(params, "ud", targetid, amount) || amount <= 0)
    {
        Msg_Usage(playerid, "/daj [id ili ime] [iznos] (predaja novca igracu pored vas)");
        return 1;
    }
    if (!Staff_CheckTarget(playerid, targetid, "Ne mozete dati novac samom sebi."))
    {
        return 1;
    }
    if (!Chat_CheckBeside(playerid, targetid, GIVE_CASH_RADIUS))
    {
        return 1;
    }
    if (amount > g_Player[playerid][pCash])
    {
        Msg_Error(playerid, "Nemate toliko novca kod sebe (imate $%d).", g_Player[playerid][pCash]);
        return 1;
    }

    Money_GiveCash(playerid, -amount, "/daj", g_Player[targetid][pDbId]);
    Money_GiveCash(targetid, amount, "/daj", g_Player[playerid][pDbId]);
    Account_Save(playerid);
    Account_Save(targetid);

    new action[96];
    format(action, sizeof(action), "vadi novac i daje ga osobi %s.", Account_Name(targetid));
    Chat_Action(playerid, action);
    Msg_Info(playerid, "Dali ste $%d igracu %s.", amount, Account_Name(targetid));
    Msg_Info(targetid, "%s vam je dao $%d.", Account_Name(playerid), amount);
    return 1;
}

cmd:banka(playerid, params[])
{
    if (Point_Require(playerid, POINT_BANK))
    {
        Money_ShowBank(playerid, false);
    }
    return 1;
}

cmd:bankomat(playerid, params[])
{
    if (Point_Require(playerid, POINT_ATM))
    {
        BankPin_OpenAtm(playerid);
    }
    return 1;
}
