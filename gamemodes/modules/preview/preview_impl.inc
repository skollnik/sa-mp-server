stock Preview_OnPlayerConnect(playerid)
{
    static const empty[E_PREVIEW_STATE];
    g_Preview[playerid] = empty;
}

stock bool:Preview_IsOpen(playerid)
{
    return g_Preview[playerid][pvMenu] != PREVIEW_NONE;
}

// A grid of 3D model pictures the player clicks on; ESC closes it. The chosen index goes back to the module that opened it.
stock Preview_Show(playerid, E_PREVIEW_MENU:menu, E_PREVIEW_KIND:kind, const title[], const models[], count)
{
    Preview_Hide(playerid);

    count = min(count, PREVIEW_MAX_ITEMS);
    for (new i = 0; i < count; i++)
    {
        g_Preview[playerid][pvModels][i] = models[i];
    }
    g_Preview[playerid][pvMenu] = menu;
    g_Preview[playerid][pvKind] = kind;
    g_Preview[playerid][pvCount] = count;
    g_Preview[playerid][pvPage] = 0;

    new PlayerText:td = CreatePlayerTextDraw(playerid, 115.0, 105.0, "_");
    PlayerTextDrawLetterSize(playerid, td, 0.0, 28.0);
    PlayerTextDrawTextSize(playerid, td, 525.0, 0.0);
    PlayerTextDrawUseBox(playerid, td, true);
    PlayerTextDrawBoxColour(playerid, td, 0x000000CC);
    g_Preview[playerid][pvBackground] = td;

    td = CreatePlayerTextDraw(playerid, 320.0, 110.0, "%s", title);
    PlayerTextDrawAlignment(playerid, td, TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawFont(playerid, td, TEXT_DRAW_FONT_2);
    PlayerTextDrawLetterSize(playerid, td, 0.3, 1.3);
    PlayerTextDrawColour(playerid, td, 0xFFFFFFFF);
    PlayerTextDrawSetOutline(playerid, td, 1);
    g_Preview[playerid][pvTitle] = td;

    for (new slot = 0; slot < PREVIEW_PER_PAGE; slot++)
    {
        new Float:x = 130.0 + float(slot % PREVIEW_COLUMNS) * 77.0,
            Float:y = 135.0 + float(slot / PREVIEW_COLUMNS) * 100.0;

        td = CreatePlayerTextDraw(playerid, x, y, "_");
        PlayerTextDrawFont(playerid, td, TEXT_DRAW_FONT_MODEL_PREVIEW);
        PlayerTextDrawTextSize(playerid, td, 70.0, 95.0);
        PlayerTextDrawUseBox(playerid, td, true);
        PlayerTextDrawBoxColour(playerid, td, PREVIEW_SLOT_COLOR);
        PlayerTextDrawBackgroundColour(playerid, td, PREVIEW_SLOT_COLOR);
        PlayerTextDrawSetSelectable(playerid, td, true);
        g_Preview[playerid][pvSlots][slot] = td;
    }

    g_Preview[playerid][pvPrev] = Preview_CreateButton(playerid, 135.0, "<< Nazad");
    g_Preview[playerid][pvNext] = Preview_CreateButton(playerid, 380.0, "Dalje >>");
    g_Preview[playerid][pvClose] = Preview_CreateButton(playerid, 460.0, "Zatvori");

    td = CreatePlayerTextDraw(playerid, 290.0, 340.0, "_");
    PlayerTextDrawFont(playerid, td, TEXT_DRAW_FONT_1);
    PlayerTextDrawLetterSize(playerid, td, 0.25, 1.1);
    PlayerTextDrawSetOutline(playerid, td, 1);
    g_Preview[playerid][pvPageText] = td;

    PlayerTextDrawShow(playerid, g_Preview[playerid][pvBackground]);
    PlayerTextDrawShow(playerid, g_Preview[playerid][pvTitle]);
    PlayerTextDrawShow(playerid, g_Preview[playerid][pvClose]);
    PlayerTextDrawShow(playerid, g_Preview[playerid][pvPageText]);
    Preview_ShowPage(playerid);

    SelectTextDraw(playerid, PREVIEW_HOVER_COLOR);
}

stock PlayerText:Preview_CreateButton(playerid, Float:x, const text[])
{
    new PlayerText:td = CreatePlayerTextDraw(playerid, x, 340.0, "%s", text);
    PlayerTextDrawFont(playerid, td, TEXT_DRAW_FONT_1);
    PlayerTextDrawLetterSize(playerid, td, 0.25, 1.1);
    PlayerTextDrawSetOutline(playerid, td, 1);
    PlayerTextDrawTextSize(playerid, td, x + 55.0, 10.0);
    PlayerTextDrawSetSelectable(playerid, td, true);
    return td;
}

static stock Preview_PageCount(playerid)
{
    return (g_Preview[playerid][pvCount] + PREVIEW_PER_PAGE - 1) / PREVIEW_PER_PAGE;
}

static stock Preview_ShowPage(playerid)
{
    new first = g_Preview[playerid][pvPage] * PREVIEW_PER_PAGE;
    for (new slot = 0; slot < PREVIEW_PER_PAGE; slot++)
    {
        new PlayerText:td = g_Preview[playerid][pvSlots][slot], index = first + slot;
        if (index >= g_Preview[playerid][pvCount])
        {
            PlayerTextDrawHide(playerid, td);
            continue;
        }

        PlayerTextDrawSetPreviewModel(playerid, td, g_Preview[playerid][pvModels][index]);
        switch (g_Preview[playerid][pvKind])
        {
            case PREVIEW_KIND_SKIN: PlayerTextDrawSetPreviewRot(playerid, td, -16.0, 0.0, -25.0, 1.0);
            case PREVIEW_KIND_VEHICLE:
            {
                PlayerTextDrawSetPreviewRot(playerid, td, -10.0, 0.0, -40.0, 1.0);
                PlayerTextDrawSetPreviewVehicleColours(playerid, td, 1, 1);
            }
            default: PlayerTextDrawSetPreviewRot(playerid, td, 0.0, 0.0, 0.0, 1.0);
        }
        PlayerTextDrawShow(playerid, td);
    }

    new pages = Preview_PageCount(playerid), page = g_Preview[playerid][pvPage];
    PlayerTextDrawSetString(playerid, g_Preview[playerid][pvPageText], "Strana %d/%d", page + 1, pages);

    if (page > 0) PlayerTextDrawShow(playerid, g_Preview[playerid][pvPrev]);
    else PlayerTextDrawHide(playerid, g_Preview[playerid][pvPrev]);

    if (page < pages - 1) PlayerTextDrawShow(playerid, g_Preview[playerid][pvNext]);
    else PlayerTextDrawHide(playerid, g_Preview[playerid][pvNext]);
}

stock Preview_Hide(playerid)
{
    if (!Preview_IsOpen(playerid))
    {
        return;
    }

    // Cleared first: cancelling the selection fires the ESC callback, which must find the menu already closed.
    g_Preview[playerid][pvMenu] = PREVIEW_NONE;

    PlayerTextDrawDestroy(playerid, g_Preview[playerid][pvBackground]);
    PlayerTextDrawDestroy(playerid, g_Preview[playerid][pvTitle]);
    PlayerTextDrawDestroy(playerid, g_Preview[playerid][pvPrev]);
    PlayerTextDrawDestroy(playerid, g_Preview[playerid][pvNext]);
    PlayerTextDrawDestroy(playerid, g_Preview[playerid][pvPageText]);
    PlayerTextDrawDestroy(playerid, g_Preview[playerid][pvClose]);
    for (new slot = 0; slot < PREVIEW_PER_PAGE; slot++)
    {
        PlayerTextDrawDestroy(playerid, g_Preview[playerid][pvSlots][slot]);
    }
    CancelSelectTextDraw(playerid);
}

// ESC while selecting reports an invalid textdraw.
stock Preview_OnClickTextDraw(playerid, Text:clickedid)
{
    if (clickedid == INVALID_TEXT_DRAW)
    {
        Preview_Hide(playerid);
    }
}

stock bool:Preview_OnClickPlayerTextDraw(playerid, PlayerText:td)
{
    if (!Preview_IsOpen(playerid))
    {
        return false;
    }

    if (td == g_Preview[playerid][pvClose])
    {
        Preview_Hide(playerid);
        return true;
    }
    if (td == g_Preview[playerid][pvPrev] && g_Preview[playerid][pvPage] > 0)
    {
        g_Preview[playerid][pvPage]--;
        Preview_ShowPage(playerid);
        return true;
    }
    if (td == g_Preview[playerid][pvNext] && g_Preview[playerid][pvPage] < Preview_PageCount(playerid) - 1)
    {
        g_Preview[playerid][pvPage]++;
        Preview_ShowPage(playerid);
        return true;
    }

    for (new slot = 0; slot < PREVIEW_PER_PAGE; slot++)
    {
        if (td != g_Preview[playerid][pvSlots][slot])
        {
            continue;
        }

        new index = g_Preview[playerid][pvPage] * PREVIEW_PER_PAGE + slot;
        new E_PREVIEW_MENU:menu = g_Preview[playerid][pvMenu];
        if (index >= g_Preview[playerid][pvCount])
        {
            return true;
        }

        Preview_Hide(playerid);
        switch (menu)
        {
            case PREVIEW_VIP_SKIN: Vip_OnSkinSelected(playerid, index);
            case PREVIEW_DOOR_ARROW: Door_OnArrowSelected(playerid, index);
            default: {}
        }
        return true;
    }
    return true;
}
