// GTA's mod shops and Pay'n'Spray give free tuning, paint and repairs, so they are closed: a vehicle heading for the door
// is pushed back the way it came. The "outside" is where the player entered the zone, since the doors face every direction.
#define GARAGE_ZONE_RADIUS      (8.0)
#define GARAGE_PUSH_SPEED       (0.25)

static const Float:g_ClosedGarages[][3] =
{
    { 1041.35, -1025.93, 32.67 },   // Transfender, Temple
    { 1024.98, -1029.35, 33.20 },   // Pay'n'Spray, Temple
    { 2071.48, -1831.42, 14.56 },   // Pay'n'Spray, Idlewood
    { 488.28, -1734.70, 12.39 },    // Pay'n'Spray, Santa Maria Beach
    { 2644.86, -2039.23, 14.04 },   // Loco Low Co, Willowfield
    { 720.02, -462.52, 16.86 },     // Pay'n'Spray, Dillimore
    { -2716.35, 217.48, 5.38 },     // Wheel Arch Angels
    { -1935.86, 239.53, 35.35 },    // Transfender, Doherty
    { -1904.53, 277.90, 42.95 },    // Pay'n'Spray, Downtown San Fierro
    { -1786.81, 1209.42, 25.84 },   // Pay'n'Spray, Downtown San Fierro (north)
    { -2425.73, 1027.99, 52.28 },   // Pay'n'Spray, Juniper Hollow
    { 2386.66, 1043.60, 11.59 },    // Transfender, Come-A-Lot
    { 1968.74, 2162.49, 12.09 },    // Pay'n'Spray, Redsands East
    { -1420.55, 2591.16, 57.74 },   // Pay'n'Spray, El Quebrados
    { -100.00, 1111.41, 21.64 }     // Pay'n'Spray, Fort Carson
};

new STREAMER_TAG_AREA:g_GarageArea[sizeof(g_ClosedGarages)];
new g_GarageInside[MAX_PLAYERS] = { -1, ... };
new Float:g_GarageEntry[MAX_PLAYERS][2];
new bool:g_GarageWarned[MAX_PLAYERS];

stock Garage_Init()
{
    for (new i = 0; i < sizeof(g_ClosedGarages); i++)
    {
        g_GarageArea[i] = CreateDynamicCylinder(g_ClosedGarages[i][0], g_ClosedGarages[i][1], g_ClosedGarages[i][2] - 4.0,
            g_ClosedGarages[i][2] + 6.0, GARAGE_ZONE_RADIUS, 0, 0);
    }
}

static stock Garage_FindByArea(STREAMER_TAG_AREA:areaid)
{
    for (new i = 0; i < sizeof(g_ClosedGarages); i++)
    {
        if (g_GarageArea[i] == areaid)
        {
            return i;
        }
    }
    return -1;
}

// Pushes only a vehicle on the entry side that moves towards the door, so a vehicle that somehow ended up inside can still leave.
static stock Garage_Check(playerid)
{
    new garage = g_GarageInside[playerid];
    if (garage == -1 || GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
    {
        return;
    }
    new Float:out_x = g_GarageEntry[playerid][0] - g_ClosedGarages[garage][0];
    new Float:out_y = g_GarageEntry[playerid][1] - g_ClosedGarages[garage][1];
    new Float:length = floatsqroot(out_x * out_x + out_y * out_y);
    if (length < 1.0)
    {
        return;
    }
    out_x /= length;
    out_y /= length;

    new vehicleid = GetPlayerVehicleID(playerid), Float:x, Float:y, Float:z, Float:vx, Float:vy, Float:vz;
    GetVehiclePos(vehicleid, x, y, z);
    GetVehicleVelocity(vehicleid, vx, vy, vz);
    new Float:side = (x - g_ClosedGarages[garage][0]) * out_x + (y - g_ClosedGarages[garage][1]) * out_y;
    if (side < -1.0 || vx * out_x + vy * out_y >= 0.0)
    {
        return;
    }
    SetVehicleVelocity(vehicleid, out_x * GARAGE_PUSH_SPEED, out_y * GARAGE_PUSH_SPEED, 0.02);
    if (!g_GarageWarned[playerid])
    {
        g_GarageWarned[playerid] = true;
        Msg_Error(playerid, "Ova radionica je zatvorena. Popravke, farbanje i tuning rade mehanicari.");
    }
}

stock Garage_OnPlayerEnterArea(playerid, STREAMER_TAG_AREA:areaid)
{
    new garage = Garage_FindByArea(areaid);
    if (garage == -1)
    {
        return;
    }
    new Float:z;
    GetPlayerPos(playerid, g_GarageEntry[playerid][0], g_GarageEntry[playerid][1], z);
    g_GarageInside[playerid] = garage;
    g_GarageWarned[playerid] = false;
    Garage_Check(playerid);
}

stock Garage_OnPlayerLeaveArea(playerid, STREAMER_TAG_AREA:areaid)
{
    new garage = Garage_FindByArea(areaid);
    if (garage != -1 && g_GarageInside[playerid] == garage)
    {
        g_GarageInside[playerid] = -1;
    }
}

stock Garage_OnPlayerConnect(playerid)
{
    g_GarageInside[playerid] = -1;
}

stock Garage_Tick()
{
    for (new playerid = 0; playerid < MAX_PLAYERS; playerid++)
    {
        if (g_GarageInside[playerid] != -1 && IsPlayerConnected(playerid))
        {
            Garage_Check(playerid);
        }
    }
}
