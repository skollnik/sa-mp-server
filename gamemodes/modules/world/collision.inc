// The game's own collision on the server (ColAndreas), so placed objects can be kept out of walls and off the air.
// Without the data file every check passes, so placing still works, only unchecked.
static bool:g_CollisionReady;

stock Collision_Init()
{
    g_CollisionReady = CA_Init() != 0;
    if (!g_CollisionReady)
    {
        // The component reads the file only when the whole server starts, so a gamemode restart never picks up a file added later.
        print("[COLLISION] No collision data: scriptfiles/ColAndreas/ColAndreas.cadb is missing or was added after the server started (restart the whole server). Placed objects are not checked against walls and floors.");
    }
}

stock bool:Collision_Ready()
{
    return g_CollisionReady;
}

// box: min x, y, z, then max x, y, z, around the model's origin.
stock bool:Collision_ModelBox(model, Float:box[6])
{
    return g_CollisionReady && CA_GetModelBoundingBox(model, box[0], box[1], box[2], box[3], box[4], box[5]) != 0;
}

stock bool:Collision_Blocked(Float:x1, Float:y1, Float:z1, Float:x2, Float:y2, Float:z2)
{
    new Float:hx, Float:hy, Float:hz;
    return g_CollisionReady && CA_RayCastLine(x1, y1, z1, x2, y2, z2, hx, hy, hz) != 0;
}

stock bool:Collision_FloorBelow(Float:x, Float:y, Float:z, Float:depth, &Float:floorz)
{
    new Float:hx, Float:hy;
    return g_CollisionReady && CA_RayCastLine(x, y, z, x, y, z - depth, hx, hy, floorz) != 0;
}

// A point given in an object's own axes, turned by its heading, in world coordinates.
stock Collision_LocalToWorld(Float:x, Float:y, Float:rz, Float:lx, Float:ly, &Float:wx, &Float:wy)
{
    wx = x + lx * floatcos(rz, degrees) - ly * floatsin(rz, degrees);
    wy = y + lx * floatsin(rz, degrees) + ly * floatcos(rz, degrees);
}

stock Collision_WorldToLocal(Float:x, Float:y, Float:rz, Float:wx, Float:wy, &Float:lx, &Float:ly)
{
    new Float:dx = wx - x, Float:dy = wy - y;
    lx = dx * floatcos(rz, degrees) + dy * floatsin(rz, degrees);
    ly = -dx * floatsin(rz, degrees) + dy * floatcos(rz, degrees);
}
