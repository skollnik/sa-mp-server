cmd:enterijerifirmi(playerid, params[])
{
    if (!Staff_CheckAdmin(playerid, ADMIN_LEVEL_HEAD))
    {
        return 1;
    }
    new list[sizeof(g_BizInteriors) * 44];
    for (new i = 0; i < sizeof(g_BizInteriors); i++)
    {
        strcat(list, g_BizInteriors[i][biName]);
        strcat(list, "\n");
    }
    ShowPlayerDialog(playerid, DIALOG_BIZ_INTERIORS, DIALOG_STYLE_LIST, "Enterijeri firmi", list, "Udji", "Zatvori");
    return 1;
}

stock bool:Biz_OnDialogResponse(playerid, dialogid, response, listitem)
{
    if (dialogid != DIALOG_BIZ_INTERIORS)
    {
        return false;
    }
    if (!response || listitem < 0 || listitem >= sizeof(g_BizInteriors) || !Staff_IsAdminOnDuty(playerid))
    {
        return true;
    }
    Staff_TeleportTo(playerid, g_BizInteriors[listitem][biX], g_BizInteriors[listitem][biY], g_BizInteriors[listitem][biZ],
        g_BizInteriors[listitem][biInterior], 0);
    SetPlayerFacingAngle(playerid, g_BizInteriors[listitem][biA]);
    SetCameraBehindPlayer(playerid);
    Msg_Admin(playerid, "Enterijer: %s. Izlaz: /enterijerifirmi pa drugi enterijer, ili /tp.", g_BizInteriors[listitem][biName]);
    return true;
}
