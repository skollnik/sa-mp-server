// Everything is drawn from textures already inside GTA (LD_SPAC, LD_BEAT, particle, hud), so every client version sees the same HUD.
#define HUD_SPRITE_BOX          "LD_SPAC:white"
#define HUD_SPRITE_ROUND        "LD_BEAT:chit"
#define HUD_SPRITE_GLOW         "particle:lamp_shad_64"

#define HUD_HUNGER_COLOR        (0x16A085FF)
#define HUD_HUNGER_LOW_COLOR    (0xC0392BFF)
#define HUD_GLOW_COLOR          (0x2980B9FF)
#define HUD_GLOW_LIGHT_COLOR    (0x7FB3D5FF)
#define HUD_TEXT_COLOR          (0xFFFFFFFF)
#define HUD_INFO_COLOR          (0xC8C8C8FF)

// The hunger badge sits under the money: a round badge with a burger drawn from shapes and the level in percent.
// Parts shown together must stay next to each other.
enum E_HUD_PART
{
    PlayerText:hpHungerCircle,
    PlayerText:hpBunTop,
    PlayerText:hpFilling,
    PlayerText:hpLineTop,
    PlayerText:hpLineBottom,
    PlayerText:hpBunBottom,
    PlayerText:hpHungerText,
    PlayerText:hpZoneIcon,
    PlayerText:hpZoneText,
    PlayerText:hpInfo
}

new Text:g_HudLogo[6];
new Text:g_HudClock = INVALID_TEXT_DRAW;
new Text:g_HudDate = INVALID_TEXT_DRAW;
new PlayerText:g_HudParts[MAX_PLAYERS][E_HUD_PART];
new g_HudZoneIndex[MAX_PLAYERS];
new g_HudOnline;
new bool:g_HudBlink;
new g_HudTimer;

static stock Text:Hud_GlobalSprite(Float:x, Float:y, Float:width, Float:height, const sprite[], colour)
{
    new Text:td = TextDrawCreate(x, y, sprite);
    TextDrawFont(td, TEXT_DRAW_FONT_SPRITE_DRAW);
    TextDrawTextSize(td, width, height);
    TextDrawColour(td, colour);
    return td;
}

static stock Text:Hud_GlobalText(Float:x, Float:y, const text[], TEXT_DRAW_FONT:font, Float:width, Float:height, TEXT_DRAW_ALIGN:align, colour)
{
    new Text:td = TextDrawCreate(x, y, text);
    TextDrawFont(td, font);
    TextDrawLetterSize(td, width, height);
    TextDrawAlignment(td, align);
    TextDrawColour(td, colour);
    TextDrawSetShadow(td, 0);
    TextDrawSetProportional(td, true);
    return td;
}

// The server name in two lines, both centred on one axis, over a soft glow.
static stock Hud_CreateLogo()
{
    g_HudLogo[0] = Hud_GlobalSprite(578.997, 27.222, 15.0, 8.0, HUD_SPRITE_GLOW, HUD_GLOW_COLOR);
    g_HudLogo[1] = Hud_GlobalSprite(579.198, 43.918, 15.0, -9.0, HUD_SPRITE_GLOW, HUD_GLOW_COLOR);
    g_HudLogo[2] = Hud_GlobalSprite(582.098, 40.527, 9.0, -5.0, HUD_SPRITE_GLOW, HUD_GLOW_LIGHT_COLOR);
    g_HudLogo[3] = Hud_GlobalSprite(581.997, 30.771, 9.0, 5.0, HUD_SPRITE_GLOW, HUD_GLOW_LIGHT_COLOR);
    g_HudLogo[4] = Hud_GlobalText(587.398, 17.004, SERVER_LOGO_TOP, TEXT_DRAW_FONT_2, 0.2231, 1.0325, TEXT_DRAW_ALIGN_CENTER, HUD_TEXT_COLOR);
    g_HudLogo[5] = Hud_GlobalText(587.398, 31.260, SERVER_LOGO_BOTTOM, TEXT_DRAW_FONT_2, 0.1439, 0.8981, TEXT_DRAW_ALIGN_CENTER, HUD_TEXT_COLOR);
}

stock Hud_Init()
{
    Hud_CreateLogo();

    // Bottom right is free of GTA's own HUD, so the clock and date live there in the logo's lettering.
    g_HudClock = Hud_GlobalText(632.0, 396.0, "00:00", TEXT_DRAW_FONT_3, 0.45, 1.8, TEXT_DRAW_ALIGN_RIGHT, HUD_TEXT_COLOR);
    TextDrawSetOutline(g_HudClock, 1);
    g_HudDate = Hud_GlobalText(632.0, 414.0, "01.01.2026.", TEXT_DRAW_FONT_2, 0.17, 0.85, TEXT_DRAW_ALIGN_RIGHT, HUD_TEXT_COLOR);
    TextDrawSetOutline(g_HudDate, 1);

    Hud_UpdateClock();
    g_HudTimer = SetTimer("Hud_Tick", 1000, true);
}

stock Hud_Exit()
{
    KillTimer(g_HudTimer);
    for (new i = 0; i < sizeof(g_HudLogo); i++)
    {
        TextDrawDestroy(g_HudLogo[i]);
    }
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

static stock Hud_Text(playerid, E_HUD_PART:part, Float:x, Float:y, const text[], TEXT_DRAW_FONT:font, Float:width, Float:height, TEXT_DRAW_ALIGN:align, colour)
{
    new PlayerText:td = CreatePlayerTextDraw(playerid, x, y, text);
    PlayerTextDrawFont(playerid, td, font);
    PlayerTextDrawLetterSize(playerid, td, width, height);
    PlayerTextDrawAlignment(playerid, td, align);
    PlayerTextDrawSetShadow(playerid, td, 0);
    PlayerTextDrawSetProportional(playerid, td, true);
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

static stock Hud_ShowHunger(playerid)
{
    Hud_Sprite(playerid, hpHungerCircle, 503.2, 106.813, 21.0, 23.0, HUD_SPRITE_ROUND, HUD_HUNGER_COLOR);
    Hud_Sprite(playerid, hpBunTop, 508.3, 112.689, 10.0, 5.0, HUD_SPRITE_ROUND, HUD_TEXT_COLOR);
    Hud_Sprite(playerid, hpFilling, 509.8, 115.271, 7.0, 2.0, HUD_SPRITE_BOX, HUD_TEXT_COLOR);
    Hud_Sprite(playerid, hpLineTop, 509.8, 118.171, 7.0, 1.0, HUD_SPRITE_BOX, HUD_TEXT_COLOR);
    Hud_Sprite(playerid, hpLineBottom, 509.8, 120.371, 7.0, 1.0, HUD_SPRITE_BOX, HUD_TEXT_COLOR);
    Hud_Sprite(playerid, hpBunBottom, 508.3, 120.489, 10.0, 2.0, HUD_SPRITE_ROUND, HUD_TEXT_COLOR);
    Hud_Text(playerid, hpHungerText, 521.608, 114.338, "100%", TEXT_DRAW_FONT_1, 0.1771, 0.8433, TEXT_DRAW_ALIGN_LEFT, HUD_HUNGER_COLOR);
    Hud_ShowParts(playerid, hpBunTop, hpBunBottom, true);
    Hud_UpdateHunger(playerid);
}

// A sprite only takes a new colour when it is shown again; red means the player should eat soon.
stock Hud_UpdateHunger(playerid)
{
    new PlayerText:text = g_HudParts[playerid][hpHungerText];
    if (text == INVALID_PLAYER_TEXT_DRAW)
    {
        return;
    }
    new hunger = g_Player[playerid][pHunger], colour = hunger <= HUNGER_WARNING ? HUD_HUNGER_LOW_COLOR : HUD_HUNGER_COLOR;
    PlayerTextDrawColour(playerid, g_HudParts[playerid][hpHungerCircle], colour);
    PlayerTextDrawColour(playerid, text, colour);
    PlayerTextDrawSetString(playerid, text, "%d%%", hunger * 100 / HUNGER_MAX);
    PlayerTextDrawShow(playerid, g_HudParts[playerid][hpHungerCircle]);
    PlayerTextDrawShow(playerid, text);
    // The circle is drawn after the burger, so the burger has to come back on top of it.
    Hud_ShowParts(playerid, hpBunTop, hpBunBottom, true);
}

// Shown once the first district is known; Hud_UpdateZone hides it again inside interiors.
static stock Hud_ShowZone(playerid)
{
    Hud_Sprite(playerid, hpZoneIcon, 22.0, 316.0, 8.0, 8.0, "hud:radar_waypoint", HUD_TEXT_COLOR);
    Hud_Text(playerid, hpZoneText, 33.0, 315.5, "_", TEXT_DRAW_FONT_2, 0.19, 0.95, TEXT_DRAW_ALIGN_LEFT, HUD_TEXT_COLOR);
    PlayerTextDrawSetOutline(playerid, g_HudParts[playerid][hpZoneText], 1);
    g_HudZoneIndex[playerid] = -2;
}

static stock Hud_ShowLogo(playerid)
{
    for (new i = 0; i < sizeof(g_HudLogo); i++)
    {
        TextDrawShowForPlayer(playerid, g_HudLogo[i]);
    }
    Hud_Text(playerid, hpInfo, 632.0, 425.0, "_", TEXT_DRAW_FONT_2, 0.15, 0.75, TEXT_DRAW_ALIGN_RIGHT, HUD_INFO_COLOR);
    PlayerTextDrawSetOutline(playerid, g_HudParts[playerid][hpInfo], 1);
    Hud_UpdateInfo(playerid);
    PlayerTextDrawShow(playerid, g_HudParts[playerid][hpInfo]);
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
    // The hunger badge is game state, not decoration, so no setting hides it.
    Hud_ShowHunger(playerid);
    if (!Setting_IsOn(playerid, SETTING_HUD))
    {
        return;
    }
    if (Setting_IsOn(playerid, SETTING_HUD_CLOCK))
    {
        TextDrawShowForPlayer(playerid, g_HudClock);
        TextDrawShowForPlayer(playerid, g_HudDate);
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
    for (new i = 0; i < sizeof(g_HudLogo); i++)
    {
        TextDrawHideForPlayer(playerid, g_HudLogo[i]);
    }
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

    // Interiors have no district; the name and icon are hidden inside.
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
        Hud_ShowParts(playerid, hpZoneIcon, hpZoneText, false);
        return;
    }
    PlayerTextDrawSetString(playerid, g_HudParts[playerid][hpZoneText], "%s", g_Zones[zone][zName]);
    Hud_ShowParts(playerid, hpZoneIcon, hpZoneText, true);
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
        // A blinking badge is easier to notice than a colour change when the player is starving.
        if (g_Player[i][pHunger] <= HUNGER_WARNING && g_HudParts[i][hpHungerCircle] != INVALID_PLAYER_TEXT_DRAW)
        {
            Hud_ShowParts(i, hpHungerCircle, hpHungerCircle, g_HudBlink);
            if (g_HudBlink)
            {
                Hud_ShowParts(i, hpBunTop, hpBunBottom, true);
            }
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
        Msg_Info(playerid, "HUD (sat, datum, kvart i logo) je sakriven i ostaje sakriven i posle izlaska. Glad se uvek vidi. Ponovo: /hud");
    }
    return 1;
}
flags:hud(CMD_FLAG_JAIL_OK)
