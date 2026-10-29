new Text:g_HudClock = INVALID_TEXT_DRAW;
new Text:g_HudDate = INVALID_TEXT_DRAW;
new PlayerText:g_HudZone[MAX_PLAYERS] = { INVALID_PLAYER_TEXT_DRAW, ... };
// The hunger bar sits under the money, the same width as the GTA health bar above it.
#define HUD_HUNGER_X            (546.5)
#define HUD_HUNGER_Y            (101.0)
#define HUD_HUNGER_WIDTH        (60.0)
#define HUD_HUNGER_COLOR        (0xE0892AFF)
#define HUD_HUNGER_LOW_COLOR    (0xB4191DFF)

enum E_HUD_HUNGER
{
    PlayerText:hhLabel,
    PlayerText:hhBack,
    PlayerText:hhFill
}
new PlayerText:g_HudHunger[MAX_PLAYERS][E_HUD_HUNGER];
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

    // The date sits above the clock: right under it is where GTA draws the armour bar.
    g_HudDate = TextDrawCreate(577.0, 10.0, "01.01.2026.");
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
    for (new E_HUD_HUNGER:part; part < E_HUD_HUNGER; part++)
    {
        g_HudHunger[playerid][part] = INVALID_PLAYER_TEXT_DRAW;
    }
}

stock Hud_UpdateClock()
{
    new hour, minute, year, month, day;
    Time_Get(hour, minute);
    getdate(year, month, day);
    TextDrawSetString(g_HudClock, "%02d:%02d", hour, minute);
    TextDrawSetString(g_HudDate, "%02d.%02d.%d.", day, month, year);
}

static stock PlayerText:Hud_CreateBox(playerid, Float:x, Float:y, Float:width, Float:height, colour)
{
    new PlayerText:td = CreatePlayerTextDraw(playerid, x, y, "_");
    PlayerTextDrawLetterSize(playerid, td, 0.0, height);
    PlayerTextDrawTextSize(playerid, td, x + width, 0.0);
    PlayerTextDrawUseBox(playerid, td, true);
    PlayerTextDrawBoxColour(playerid, td, colour);
    return td;
}

static stock Hud_ShowHunger(playerid)
{
    if (g_HudHunger[playerid][hhBack] == INVALID_PLAYER_TEXT_DRAW)
    {
        new PlayerText:label = CreatePlayerTextDraw(playerid, HUD_HUNGER_X - 3.0, HUD_HUNGER_Y - 2.5, "Glad");
        PlayerTextDrawFont(playerid, label, TEXT_DRAW_FONT_1);
        PlayerTextDrawLetterSize(playerid, label, 0.2, 0.9);
        PlayerTextDrawAlignment(playerid, label, TEXT_DRAW_ALIGN_RIGHT);
        PlayerTextDrawSetOutline(playerid, label, 1);
        g_HudHunger[playerid][hhLabel] = label;
        g_HudHunger[playerid][hhBack] = Hud_CreateBox(playerid, HUD_HUNGER_X, HUD_HUNGER_Y, HUD_HUNGER_WIDTH, 0.55, 0x000000AA);
        g_HudHunger[playerid][hhFill] = Hud_CreateBox(playerid, HUD_HUNGER_X + 1.5, HUD_HUNGER_Y + 1.5, HUD_HUNGER_WIDTH - 3.0, 0.25, HUD_HUNGER_COLOR);
    }
    PlayerTextDrawShow(playerid, g_HudHunger[playerid][hhLabel]);
    PlayerTextDrawShow(playerid, g_HudHunger[playerid][hhBack]);
    Hud_UpdateHunger(playerid);
}

// A box only takes a new size or colour when it is shown again.
stock Hud_UpdateHunger(playerid)
{
    new PlayerText:fill = g_HudHunger[playerid][hhFill];
    if (fill == INVALID_PLAYER_TEXT_DRAW)
    {
        return;
    }

    new hunger = g_Player[playerid][pHunger];
    if (hunger == 0)
    {
        PlayerTextDrawHide(playerid, fill);
        return;
    }
    PlayerTextDrawTextSize(playerid, fill, HUD_HUNGER_X + 1.5 + (HUD_HUNGER_WIDTH - 3.0) * float(hunger) / float(HUNGER_MAX), 0.0);
    PlayerTextDrawBoxColour(playerid, fill, hunger <= HUNGER_WARNING ? HUD_HUNGER_LOW_COLOR : HUD_HUNGER_COLOR);
    PlayerTextDrawShow(playerid, fill);
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
    Hud_ShowHunger(playerid);
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
    for (new E_HUD_HUNGER:part; part < E_HUD_HUNGER; part++)
    {
        if (g_HudHunger[playerid][part] != INVALID_PLAYER_TEXT_DRAW)
        {
            PlayerTextDrawDestroy(playerid, g_HudHunger[playerid][part]);
            g_HudHunger[playerid][part] = INVALID_PLAYER_TEXT_DRAW;
        }
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
        Msg_Info(playerid, "HUD (sat, datum, kvart i glad) je sakriven. Ponovo ga ukljucite sa /hud.");
    }
    else
    {
        Hud_Show(playerid);
        Msg_Info(playerid, "HUD je ponovo prikazan.");
    }
    return 1;
}
flags:hud(CMD_FLAG_JAIL_OK)
