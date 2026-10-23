// Welcome area in front of the airport terminal, where new characters spawn.
// Objects sit in one row along the pavement on both sides of the spawn point. Heights are estimated:
// if something floats or sinks, stand next to it, use /mypos and correct AIRPORT_GROUND_Z or the object.
#define AIRPORT_GROUND_Z    (12.55)
#define AIRPORT_ROW_Y       (NEW_SPAWN_Y)

stock Map_Airport()
{
    new Float:cx = NEW_SPAWN_X, Float:y = AIRPORT_ROW_Y, Float:z = AIRPORT_GROUND_Z;

    // Benches
    CreateDynamicObject(1280, cx - 3.5, y, z + 0.45, 0.0, 0.0, 0.0, 0, 0);
    CreateDynamicObject(1280, cx + 3.5, y, z + 0.45, 0.0, 0.0, 0.0, 0, 0);

    // Street lamps
    CreateDynamicObject(1226, cx - 6.0, y, z, 0.0, 0.0, 0.0, 0, 0);
    CreateDynamicObject(1226, cx + 6.0, y, z, 0.0, 0.0, 180.0, 0, 0);

    // Planters with bushes
    CreateDynamicObject(640, cx - 8.5, y, z, 0.0, 0.0, 0.0, 0, 0);
    CreateDynamicObject(640, cx + 8.5, y, z, 0.0, 0.0, 0.0, 0, 0);

    // Bins
    CreateDynamicObject(1300, cx - 10.5, y, z, 0.0, 0.0, 0.0, 0, 0);
    CreateDynamicObject(1300, cx + 10.5, y, z, 0.0, 0.0, 0.0, 0, 0);

    // Food carts
    CreateDynamicObject(1340, cx + 13.5, y, z + 0.5, 0.0, 0.0, 0.0, 0, 0);
    CreateDynamicObject(1342, cx - 13.5, y, z + 0.5, 0.0, 0.0, 0.0, 0, 0);
}
