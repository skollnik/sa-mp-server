// Everything is drawn from textures already inside GTA (LD_SPAC, LD_BEAT, hud), so every client version sees the same HUD.
#define HUD_SPRITE_BOX          "LD_SPAC:white"
#define HUD_SPRITE_ROUND        "LD_BEAT:chit"
#define HUD_PANEL_COLOR         (0x000000A0)

#define HUD_HUNGER_X            (546.5)
#define HUD_HUNGER_Y            (102.0)
#define HUD_HUNGER_WIDTH        (60.0)
#define HUD_HUNGER_HEIGHT       (8.0)
#define HUD_HUNGER_BORDER       (1.5)
#define HUD_HUNGER_COLOR        (0xE0892AFF)
#define HUD_HUNGER_LOW_COLOR    (0xB4191DFF)
#define HUD_HUNGER_TRACK_COLOR  (0x5C3A12FF)

// Parts in a row (a pill's three pieces, a group shown together) must stay next to each other.
enum E_HUD_PART
{
    PlayerText:hpDateLeft,
    PlayerText:hpDateMid,
    PlayerText:hpDateRight,
    PlayerText:hpHungerIcon,
    PlayerText:hpHungerBorder,
    PlayerText:hpHungerTrack,
    PlayerText:hpHungerFill,
    PlayerText:hpZoneLeft,
    PlayerText:hpZoneMid,
    PlayerText:hpZoneRight,
    PlayerText:hpZoneIcon,
    PlayerText:hpZoneText,
    PlayerText:hpLogoLeft,
    PlayerText:hpLogoMid,
    PlayerText:hpLogoRight,
    PlayerText:hpLogoText,
    PlayerText:hpInfo
}

new Text:g_HudClock = INVALID_TEXT_DRAW;
new Text:g_HudDate = INVALID_TEXT_DRAW;
new PlayerText:g_HudParts[MAX_PLAYERS][E_HUD_PART];
new g_HudZoneIndex[MAX_PLAYERS];
new g_HudOnline;
new bool:g_HudBlink;
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
    TextDrawSetShadow(g_HudDate, 0);

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
    g_HudZoneIndex[playerid] = -1;
    for (new E_HUD_PART:part; part < E_HUD_PART; part++)
    {
        g_HudParts[playerid][part] = INVALID_PLAYER_TEXT_DRAW;
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

static stock Hud_Sprite(playerid, E_HUD_PART:part, Float:x, Float:y, Float:width, Float:height, const sprite[], colour)
{
    new PlayerText:td = CreatePlayerTextDraw(playerid, x, y, sprite);
    PlayerTextDrawFont(playerid, td, TEXT_DRAW_FONT_SPRITE_DRAW);
    PlayerTextDrawTextSize(playerid, td, width, height);
    PlayerTextDrawColour(playerid, td, colour);
    g_HudParts[playerid][part] = td;
}

// A box with round ends: two circles of the box's height and a plain box between them.
static stock Hud_Pill(playerid, E_HUD_PART:first, Float:x, Float:y, Float:width, Float:height, colour)
{
    new Float:radius = height / 2.0;
    Hud_Sprite(playerid, first, x, y, height, height, HUD_SPRITE_ROUND, colour);
    Hud_Sprite(playerid, first + E_HUD_PART:1, x + radius, y, width - height, height, HUD_SPRITE_BOX, colour);
    Hud_Sprite(playerid, first + E_HUD_PART:2, x + width - height, y, height, height, HUD_SPRITE_ROUND, colour);
}

static stock Hud_Text(playerid, E_HUD_PART:part, Float:x, Float:y, const text[], Float:size, TEXT_DRAW_ALIGN:align, colour)
{
    new PlayerText:td = CreatePlayerTextDraw(playerid, x, y, text);
    PlayerTextDrawFont(playerid, td, TEXT_DRAW_FONT_1);
    PlayerTextDrawLetterSize(playerid, td, size, size * 4.5);
    PlayerTextDrawAlignment(playerid, td, align);
    PlayerTextDrawSetShadow(playerid, td, 0);
    PlayerTextDrawColour(playerid, td, colour);
    g_HudParts[playerid][part] = td;
}

static stock Hud_ShowParts(playerid, E_HUD_PART:first, E_HUD_PART:last, bool:show)
{
    for (new E_HUD_PART:part = first; part <= last; part++)
    {
        if (g_HudParts[playerid][part] == INVALID_PLAYER_TEXT_DRAW)
        {
            continue;
        }
        if (show)
        {
            PlayerTextDrawShow(playerid, g_HudParts[playerid][part]);
        }
        else
        {
            PlayerTextDrawHide(playerid, g_HudParts[playerid][part]);
        }
    }
}

static stock Hud_ShowClock(playerid)
{
    Hud_Pill(playerid, hpDateLeft, 545.0, 9.0, 64.0, 11.0, HUD_PANEL_COLOR);
    Hud_ShowParts(playerid, hpDateLeft, hpDateRight, true);
    TextDrawShowForPlayer(playerid, g_HudDate);
    TextDrawShowForPlayer(playerid, g_HudClock);
}

// Built like the GTA health bar above it: a black border, a dark track and the coloured fill.
static stock Hud_ShowHunger(playerid)
{
    Hud_Sprite(playerid, hpHungerIcon, HUD_HUNGER_X - 14.5, HUD_HUNGER_Y - 1.5, 11.0, 11.0, "hud:radar_burgerShot", 0xFFFFFFFF);
    Hud_Sprite(playerid, hpHungerBorder, HUD_HUNGER_X, HUD_HUNGER_Y, HUD_HUNGER_WIDTH, HUD_HUNGER_HEIGHT, HUD_SPRITE_BOX, 0x000000FF);
    Hud_Sprite(playerid, hpHungerTrack, HUD_HUNGER_X + HUD_HUNGER_BORDER, HUD_HUNGER_Y + HUD_HUNGER_BORDER,
        HUD_HUNGER_WIDTH - 2.0 * HUD_HUNGER_BORDER, HUD_HUNGER_HEIGHT - 2.0 * HUD_HUNGER_BORDER, HUD_SPRITE_BOX, HUD_HUNGER_TRACK_COLOR);
    Hud_Sprite(playerid, hpHungerFill, HUD_HUNGER_X + HUD_HUNGER_BORDER, HUD_HUNGER_Y + HUD_HUNGER_BORDER,
        HUD_HUNGER_WIDTH - 2.0 * HUD_HUNGER_BORDER, HUD_HUNGER_HEIGHT - 2.0 * HUD_HUNGER_BORDER, HUD_SPRITE_BOX, HUD_HUNGER_COLOR);
    Hud_ShowParts(playerid, hpHungerIcon, hpHungerTrack, true);
    Hud_UpdateHunger(playerid);
}

// A sprite only takes a new size or colour when it is shown again.
stock Hud_UpdateHunger(playerid)
{
    new PlayerText:fill = g_HudParts[playerid][hpHungerFill];
    if (fill == INVALID_PLAYER_TEXT_DRAW)
    {
        return;
    }

    new hunger = g_Player[playerid][pHunger];
    PlayerTextDrawShow(playerid, g_HudParts[playerid][hpHungerIcon]);
    if (hunger == 0)
    {
        PlayerTextDrawHide(playerid, fill);
        return;
    }
    PlayerTextDrawTextSize(playerid, fill, (HUD_HUNGER_WIDTH - 2.0 * HUD_HUNGER_BORDER) * float(hunger) / float(HUNGER_MAX), HUD_HUNGER_HEIGHT - 2.0 * HUD_HUNGER_BORDER);
    PlayerTextDrawColour(playerid, fill, hunger <= HUNGER_WARNING ? HUD_HUNGER_LOW_COLOR : HUD_HUNGER_COLOR);
    PlayerTextDrawShow(playerid, fill);
}

// Shown once the first district is known; Hud_UpdateZone hides it again inside interiors.
static stock Hud_ShowZone(playerid)
{
    Hud_Pill(playerid, hpZoneLeft, 20.0, 314.0, 140.0, 13.0, HUD_PANEL_COLOR);
    Hud_Sprite(playerid, hpZoneIcon, 24.0, 316.0, 9.0, 9.0, "hud:radar_waypoint", 0xFFFFFFFF);
    Hud_Text(playerid, hpZoneText, 95.0, 315.6, "_", 0.19, TEXT_DRAW_ALIGN_CENTER, 0xFFFFFFFF);
    g_HudZoneIndex[playerid] = -2;
}

static stock Hud_ShowLogo(playerid)
{
    Hud_Pill(playerid, hpLogoLeft, 552.0, 428.0, 82.0, 13.0, HUD_PANEL_COLOR);
    Hud_Text(playerid, hpLogoText, 593.0, 429.6, SERVER_NAME, 0.2, TEXT_DRAW_ALIGN_CENTER, COLOR_SERVER);
    Hud_Text(playerid, hpInfo, 632.0, 417.0, "_", 0.17, TEXT_DRAW_ALIGN_RIGHT, 0xC8C8C8FF);
    PlayerTextDrawSetOutline(playerid, g_HudParts[playerid][hpInfo], 1);
    Hud_UpdateInfo(playerid);
    Hud_ShowParts(playerid, hpLogoLeft, hpInfo, true);
}

static stock Hud_UpdateInfo(playerid)
{
    if (g_HudParts[playerid][hpInfo] != INVALID_PLAYER_TEXT_DRAW)
    {
        PlayerTextDrawSetString(playerid, g_HudParts[playerid][hpInfo], "ID %d  |  Online %d", playerid, g_HudOnline);
    }
}

stock Hud_Show(playerid)
{
    // The hunger bar is game state, not decoration, so no setting hides it.
    Hud_ShowHunger(playerid);
    if (!Setting_IsOn(playerid, SETTING_HUD))
    {
        return;
    }
    if (Setting_IsOn(playerid, SETTING_HUD_CLOCK))
    {
        Hud_ShowClock(playerid);
    }
    if (Setting_IsOn(playerid, SETTING_HUD_ZONE))
    {
        Hud_ShowZone(playerid);
    }
    if (Setting_IsOn(playerid, SETTING_HUD_LOGO))
    {
        Hud_ShowLogo(playerid);
    }
}

// Rebuilt from scratch after a setting changes, so only the parts still switched on come back.
stock Hud_Refresh(playerid)
{
    Hud_Hide(playerid);
    Hud_Show(playerid);
}

stock Hud_Hide(playerid)
{
    TextDrawHideForPlayer(playerid, g_HudClock);
    TextDrawHideForPlayer(playerid, g_HudDate);
    for (new E_HUD_PART:part; part < E_HUD_PART; part++)
    {
        if (g_HudParts[playerid][part] != INVALID_PLAYER_TEXT_DRAW)
        {
            PlayerTextDrawDestroy(playerid, g_HudParts[playerid][part]);
            g_HudParts[playerid][part] = INVALID_PLAYER_TEXT_DRAW;
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
    if (g_HudParts[playerid][hpZoneText] == INVALID_PLAYER_TEXT_DRAW)
    {
        return;
    }

    // Interiors have no district; the whole plate is hidden inside.
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
        Hud_ShowParts(playerid, hpZoneLeft, hpZoneText, false);
        return;
    }
    PlayerTextDrawSetString(playerid, g_HudParts[playerid][hpZoneText], "%s", g_Zones[zone][zName]);
    Hud_ShowParts(playerid, hpZoneLeft, hpZoneText, true);
}

static stock Hud_CountOnline()
{
    new count;
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (IsPlayerConnected(i) && !IsPlayerNPC(i))
        {
            count++;
        }
    }
    return count;
}

forward Hud_Tick();
public Hud_Tick()
{
    g_HudBlink = !g_HudBlink;
    new online = Hud_CountOnline();
    new bool:online_changed = (online != g_HudOnline);
    g_HudOnline = online;

    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (!Account_IsLoggedIn(i))
        {
            continue;
        }
        Hud_UpdateZone(i);
        Gps_Tick(i);
        if (online_changed)
        {
            Hud_UpdateInfo(i);
        }
        // A blinking burger is easier to notice than a red bar when the player is starving.
        if (g_Player[i][pHunger] <= HUNGER_WARNING && g_HudParts[i][hpHungerIcon] != INVALID_PLAYER_TEXT_DRAW)
        {
            Hud_ShowParts(i, hpHungerIcon, hpHungerIcon, g_HudBlink);
        }
    }
    return 1;
}

cmd:hud(playerid, params[])
{
    Setting_Toggle(playerid, SETTING_HUD);
    if (Setting_IsOn(playerid, SETTING_HUD))
    {
        Msg_Info(playerid, "HUD je ponovo prikazan. Delovi HUD-a se biraju u /podesavanja.");
    }
    else
    {
        Msg_Info(playerid, "HUD (sat, datum, kvart i logo) je sakriven i ostaje sakriven i posle izlaska. Traka gladi se uvek vidi. Ponovo: /hud");
    }
    return 1;
}
flags:hud(CMD_FLAG_JAIL_OK)
