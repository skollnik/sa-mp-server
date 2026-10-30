// Sits above the clock, bottom right, in the HUD's lettering: speed, a fuel bar, engine and lock.
#define SPEEDO_FUEL_X           (562.0)
#define SPEEDO_FUEL_Y           (377.0)
#define SPEEDO_FUEL_WIDTH       (70.0)
#define SPEEDO_FUEL_HEIGHT      (3.5)
#define SPEEDO_FUEL_COLOR       (0xF5B041FF)
#define SPEEDO_FUEL_LOW_COLOR   (0xC0392BFF)
#define SPEEDO_BACK_COLOR       (0x00000099)
#define SPEEDO_UPDATE_MS        (250)

enum E_SPEEDO_PART
{
    PlayerText:spSpeed,
    PlayerText:spUnit,
    PlayerText:spFuelBack,
    PlayerText:spFuelBar,
    PlayerText:spStatus
}

new PlayerText:g_Speedo[MAX_PLAYERS][E_SPEEDO_PART];
new bool:g_SpeedoShown[MAX_PLAYERS];
// Last values sent, so the textdraws are only rewritten when something changed.
new g_SpeedoLastSpeed[MAX_PLAYERS];
new g_SpeedoLastFuel[MAX_PLAYERS];
new g_SpeedoLastStatus[MAX_PLAYERS];
new g_SpeedoTimer;

stock Speedo_Init()
{
    g_SpeedoTimer = SetTimer("Speedo_Tick", SPEEDO_UPDATE_MS, true);
}

stock Speedo_Exit()
{
    KillTimer(g_SpeedoTimer);
}

stock Speedo_OnPlayerConnect(playerid)
{
    g_SpeedoShown[playerid] = false;
}

static stock PlayerText:Speedo_Text(playerid, Float:x, Float:y, const text[], TEXT_DRAW_FONT:font, Float:width, Float:height)
{
    new PlayerText:td = CreatePlayerTextDraw(playerid, x, y, text);
    PlayerTextDrawFont(playerid, td, font);
    PlayerTextDrawLetterSize(playerid, td, width, height);
    PlayerTextDrawAlignment(playerid, td, TEXT_DRAW_ALIGN_RIGHT);
    PlayerTextDrawColour(playerid, td, HUD_TEXT_COLOR);
    PlayerTextDrawSetShadow(playerid, td, 0);
    PlayerTextDrawSetOutline(playerid, td, 1);
    PlayerTextDrawSetProportional(playerid, td, true);
    return td;
}

static stock PlayerText:Speedo_Box(playerid, Float:width, colour)
{
    new PlayerText:td = CreatePlayerTextDraw(playerid, SPEEDO_FUEL_X, SPEEDO_FUEL_Y, HUD_SPRITE_BOX);
    PlayerTextDrawFont(playerid, td, TEXT_DRAW_FONT_SPRITE_DRAW);
    PlayerTextDrawTextSize(playerid, td, width, SPEEDO_FUEL_HEIGHT);
    PlayerTextDrawColour(playerid, td, colour);
    return td;
}

stock Speedo_Show(playerid)
{
    if (g_SpeedoShown[playerid] || !Setting_IsOn(playerid, SETTING_HUD) || !Setting_IsOn(playerid, SETTING_HUD_SPEEDO))
    {
        return;
    }
    new vehicleid = GetPlayerVehicleID(playerid);
    if (vehicleid == 0)
    {
        return;
    }

    g_Speedo[playerid][spSpeed] = Speedo_Text(playerid, 606.0, 350.0, "0", TEXT_DRAW_FONT_3, 0.5, 2.0);
    g_Speedo[playerid][spUnit] = Speedo_Text(playerid, 632.0, 361.0, "KM/H", TEXT_DRAW_FONT_2, 0.16, 0.8);
    g_Speedo[playerid][spStatus] = Speedo_Text(playerid, 632.0, 383.0, "_", TEXT_DRAW_FONT_2, 0.15, 0.75);
    g_Speedo[playerid][spFuelBack] = Speedo_Box(playerid, SPEEDO_FUEL_WIDTH, SPEEDO_BACK_COLOR);
    g_Speedo[playerid][spFuelBar] = Speedo_Box(playerid, SPEEDO_FUEL_WIDTH, SPEEDO_FUEL_COLOR);

    g_SpeedoShown[playerid] = true;
    g_SpeedoLastSpeed[playerid] = -1;
    g_SpeedoLastFuel[playerid] = -1;
    g_SpeedoLastStatus[playerid] = -1;
    PlayerTextDrawShow(playerid, g_Speedo[playerid][spSpeed]);
    PlayerTextDrawShow(playerid, g_Speedo[playerid][spUnit]);
    Speedo_Update(playerid);
}

stock Speedo_Hide(playerid)
{
    if (!g_SpeedoShown[playerid])
    {
        return;
    }
    for (new E_SPEEDO_PART:part; part < E_SPEEDO_PART; part++)
    {
        PlayerTextDrawDestroy(playerid, g_Speedo[playerid][part]);
    }
    g_SpeedoShown[playerid] = false;
}

stock Speedo_Refresh(playerid)
{
    Speedo_Hide(playerid);
    Speedo_Show(playerid);
}

stock Speedo_OnPlayerStateChange(playerid, PLAYER_STATE:newstate)
{
    if (newstate == PLAYER_STATE_DRIVER || newstate == PLAYER_STATE_PASSENGER)
    {
        Speedo_Show(playerid);
    }
    else
    {
        Speedo_Hide(playerid);
    }
}

// Bicycles have no tank, engine or lock, so they only show the speed.
static stock Speedo_Update(playerid)
{
    new vehicleid = GetPlayerVehicleID(playerid);
    if (vehicleid == 0)
    {
        Speedo_Hide(playerid);
        return;
    }

    new speed = floatround(Veh_SpeedKmh(vehicleid));
    if (speed != g_SpeedoLastSpeed[playerid])
    {
        g_SpeedoLastSpeed[playerid] = speed;
        PlayerTextDrawSetString(playerid, g_Speedo[playerid][spSpeed], "%d", speed);
    }

    if (!Fuel_IsUsed(vehicleid))
    {
        if (g_SpeedoLastFuel[playerid] != -2)
        {
            g_SpeedoLastFuel[playerid] = -2;
            PlayerTextDrawHide(playerid, g_Speedo[playerid][spFuelBack]);
            PlayerTextDrawHide(playerid, g_Speedo[playerid][spFuelBar]);
            PlayerTextDrawHide(playerid, g_Speedo[playerid][spStatus]);
        }
        return;
    }

    new fuel = Fuel_Percent(vehicleid);
    if (fuel != g_SpeedoLastFuel[playerid])
    {
        g_SpeedoLastFuel[playerid] = fuel;
        new PlayerText:bar = g_Speedo[playerid][spFuelBar];
        PlayerTextDrawTextSize(playerid, bar, SPEEDO_FUEL_WIDTH * float(fuel) / 100.0, SPEEDO_FUEL_HEIGHT);
        PlayerTextDrawColour(playerid, bar, fuel < FUEL_RESERVE_PERCENT ? SPEEDO_FUEL_LOW_COLOR : SPEEDO_FUEL_COLOR);
        PlayerTextDrawShow(playerid, g_Speedo[playerid][spFuelBack]);
        if (fuel > 0)
        {
            PlayerTextDrawShow(playerid, bar);
        }
        else
        {
            PlayerTextDrawHide(playerid, bar);
        }
    }

    new slot = g_VehSlotOf[vehicleid], bool:engine = Veh_IsEngineOn(vehicleid);
    new bool:locked = slot != -1 && g_Veh[slot][vLocked];
    new status = (engine ? 1 : 0) | (locked ? 2 : 0) | (floatround(g_VehFuel[vehicleid]) << 2);
    if (status != g_SpeedoLastStatus[playerid])
    {
        g_SpeedoLastStatus[playerid] = status;
        PlayerTextDrawSetString(playerid, g_Speedo[playerid][spStatus], "~w~%.0f L   %sMOTOR   %s",
            g_VehFuel[vehicleid], engine ? ("~g~") : ("~r~"), locked ? ("~r~ZAKLJUCANO") : ("~w~OTKLJUCANO"));
        PlayerTextDrawShow(playerid, g_Speedo[playerid][spStatus]);
    }
}

forward Speedo_Tick();
public Speedo_Tick()
{
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (g_SpeedoShown[i])
        {
            Speedo_Update(i);
        }
    }
    return 1;
}
