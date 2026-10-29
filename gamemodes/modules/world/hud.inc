new Text:g_HudClock = INVALID_TEXT_DRAW;
new Text:g_HudDate = INVALID_TEXT_DRAW;
new PlayerText:g_HudZone[MAX_PLAYERS] = { INVALID_PLAYER_TEXT_DRAW, ... };
new g_HudZoneIndex[MAX_PLAYERS];
new bool:g_HudHidden[MAX_PLAYERS];
new g_HudTimer;

stock Hud_Init()
{
    g_HudClock = TextDrawCreate(577.0, 22.0, "00:00");
    TextDrawFont(g_HudClock, TEXT_DRAW_FONT_3);
    TextDrawLetterSize(g_HudClock, 0.55, 2.2);
    TextDrawAlignment(g_HudClock, TEXT_DRAW_ALIGN_CENTER);
    TextDrawSetOutline(g_HudClock, 2);
    TextDrawColour(g_HudClock, 0xC2C2C2FF);

    g_HudDate = TextDrawCreate(577.0, 45.0, "01.01.2026.");
    TextDrawFont(g_HudDate, TEXT_DRAW_FONT_1);
    TextDrawLetterSize(g_HudDate, 0.25, 1.0);
    TextDrawAlignment(g_HudDate, TEXT_DRAW_ALIGN_CENTER);
    TextDrawSetOutline(g_HudDate, 1);

    Hud_UpdateClock();
    g_HudTimer = SetTimer("Hud_Tick", 1000, true);
}

stock Hud_Exit()
{
    KillTimer(g_HudTimer);
    TextDrawDestroy(g_HudClock);
    TextDrawDestroy(g_HudDate);
}

stock Hud_OnPlayerConnect(playerid)
{
    g_HudZone[playerid] = INVALID_PLAYER_TEXT_DRAW;
    g_HudZoneIndex[playerid] = -1;
    g_HudHidden[playerid] = false;
}

stock Hud_UpdateClock()
{
    new hour, minute, year, month, day;
    Time_Get(hour, minute);
    getdate(year, month, day);
    TextDrawSetString(g_HudClock, "%02d:%02d", hour, minute);
    TextDrawSetString(g_HudDate, "%02d.%02d.%d.", day, month, year);
}

stock Hud_Show(playerid)
{
    if (g_HudHidden[playerid])
    {
        return;
    }
    TextDrawShowForPlayer(playerid, g_HudClock);
    TextDrawShowForPlayer(playerid, g_HudDate);

    if (g_HudZone[playerid] == INVALID_PLAYER_TEXT_DRAW)
    {
        new PlayerText:td = CreatePlayerTextDraw(playerid, 88.0, 318.0, "_");
        PlayerTextDrawFont(playerid, td, TEXT_DRAW_FONT_1);
        PlayerTextDrawLetterSize(playerid, td, 0.25, 1.1);
        PlayerTextDrawAlignment(playerid, td, TEXT_DRAW_ALIGN_CENTER);
        PlayerTextDrawSetOutline(playerid, td, 1);
        g_HudZone[playerid] = td;
    }
    g_HudZoneIndex[playerid] = -1;
    PlayerTextDrawShow(playerid, g_HudZone[playerid]);
}

stock Hud_Hide(playerid)
{
    TextDrawHideForPlayer(playerid, g_HudClock);
    TextDrawHideForPlayer(playerid, g_HudDate);
    if (g_HudZone[playerid] != INVALID_PLAYER_TEXT_DRAW)
    {
        PlayerTextDrawDestroy(playerid, g_HudZone[playerid]);
        g_HudZone[playerid] = INVALID_PLAYER_TEXT_DRAW;
    }
}

stock Hud_FindZone(Float:x, Float:y)
{
    for (new i = 0; i < sizeof(g_Zones); i++)
    {
        if (x >= g_Zones[i][zMinX] && x <= g_Zones[i][zMaxX] && y >= g_Zones[i][zMinY] && y <= g_Zones[i][zMaxY])
        {
            return i;
        }
    }
    return -1;
}

stock Hud_UpdateZone(playerid)
{
    if (g_HudZone[playerid] == INVALID_PLAYER_TEXT_DRAW)
    {
        return;
    }

    // Interiors have no district; the name is simply hidden inside.
    new zone = -1;
    if (GetPlayerInterior(playerid) == 0)
    {
        new Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);
        zone = Hud_FindZone(x, y);
    }
    if (zone == g_HudZoneIndex[playerid])
    {
        return;
    }

    g_HudZoneIndex[playerid] = zone;
    if (zone == -1)
    {
        PlayerTextDrawSetString(playerid, g_HudZone[playerid], "_");
    }
    else
    {
        PlayerTextDrawSetString(playerid, g_HudZone[playerid], "%s", g_Zones[zone][zName]);
    }
}

forward Hud_Tick();
public Hud_Tick()
{
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (!Account_IsLoggedIn(i))
        {
            continue;
        }
        Hud_UpdateZone(i);
        Gps_Tick(i);
    }
    return 1;
}

cmd:hud(playerid, params[])
{
    g_HudHidden[playerid] = !g_HudHidden[playerid];
    if (g_HudHidden[playerid])
    {
        Hud_Hide(playerid);
        Msg_Info(playerid, "HUD (sat, datum i kvart) je sakriven. Ponovo ga ukljucite sa /hud.");
    }
    else
    {
        Hud_Show(playerid);
        Msg_Info(playerid, "HUD je ponovo prikazan.");
    }
    return 1;
}
flags:hud(CMD_FLAG_JAIL_OK)
