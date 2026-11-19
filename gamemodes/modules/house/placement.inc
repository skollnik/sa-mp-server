// Moving an object inside a house with the game's own mouse arrows; the safe and furniture only say what happens when it is put down.
static E_PLACE_KIND:g_PlaceKind[MAX_PLAYERS];
static g_PlaceRef[MAX_PLAYERS];
static g_PlaceHouse[MAX_PLAYERS];
static STREAMER_TAG_OBJECT:g_PlaceObject[MAX_PLAYERS];
static Float:g_PlaceFrom[MAX_PLAYERS][6];
// A piece just taken out of the unplaced list: cancelling takes it back there instead of leaving it where it appeared.
static bool:g_PlaceFresh[MAX_PLAYERS];

stock Place_OnPlayerConnect(playerid)
{
    g_PlaceKind[playerid] = PLACE_NONE;
}

stock bool:Place_IsInsideHouse(slot, Float:x, Float:y, Float:z)
{
    new Float:ex, Float:ey, Float:ez, Float:ea;
    House_ExitPos(slot, ex, ey, ez, ea);
    return VectorSize(x - ex, y - ey, 0.0) <= FURN_PLACE_RADIUS && z > ez - 6.0 && z < ez + 8.0;
}

stock bool:Place_IsEditing(playerid)
{
    return g_PlaceKind[playerid] != PLACE_NONE;
}

// The spot a meter in front of the player, on the floor, turned to face them.
stock Place_InFront(playerid, &Float:x, &Float:y, &Float:z, &Float:rz)
{
    new Float:angle;
    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, angle);
    x += 1.2 * floatsin(-angle, degrees);
    y += 1.2 * floatcos(-angle, degrees);
    z -= 1.0;
    rz = angle + 180.0;
}

stock Place_Start(playerid, E_PLACE_KIND:kind, ref, slot, STREAMER_TAG_OBJECT:objectid, bool:fresh)
{
    Place_Stop(playerid);
    g_PlaceKind[playerid] = kind;
    g_PlaceRef[playerid] = ref;
    g_PlaceHouse[playerid] = g_Houses[slot][hDbId];
    g_PlaceObject[playerid] = objectid;
    g_PlaceFresh[playerid] = fresh;
    GetDynamicObjectPos(objectid, g_PlaceFrom[playerid][0], g_PlaceFrom[playerid][1], g_PlaceFrom[playerid][2]);
    GetDynamicObjectRot(objectid, g_PlaceFrom[playerid][3], g_PlaceFrom[playerid][4], g_PlaceFrom[playerid][5]);
    // A just-made object reaches the client only on the next streamer pass, and the arrows need it there.
    Streamer_Update(playerid, STREAMER_TYPE_OBJECT);
    EditDynamicObject(playerid, objectid);
    Msg_Info(playerid, "Pomerajte strelicama (mis). Disketa cuva, ESC vraca nazad. Drzite razmak da okrecete kameru.");
}

// The object may have been destroyed meanwhile (house sold, refreshed) and its id given to another object, which must not be moved.
static stock bool:Place_StillThere(playerid, E_PLACE_KIND:kind)
{
    if (kind == PLACE_SAFE)
    {
        new slot = House_FindSlotByDbId(g_PlaceHouse[playerid]);
        return slot != -1 && g_Houses[slot][hHasSafe] && g_Houses[slot][hSafeObject] == g_PlaceObject[playerid];
    }
    return Furn_IsObject(g_PlaceRef[playerid], g_PlaceHouse[playerid], g_PlaceObject[playerid]);
}

static stock Place_Undo(playerid)
{
    new E_PLACE_KIND:kind = g_PlaceKind[playerid];
    g_PlaceKind[playerid] = PLACE_NONE;
    if (g_PlaceFresh[playerid] && kind == PLACE_FURNITURE)
    {
        Furn_OnPlaceCancelled(g_PlaceRef[playerid], g_PlaceHouse[playerid]);
        return;
    }
    if (Place_StillThere(playerid, kind))
    {
        SetDynamicObjectPos(g_PlaceObject[playerid], g_PlaceFrom[playerid][0], g_PlaceFrom[playerid][1], g_PlaceFrom[playerid][2]);
        SetDynamicObjectRot(g_PlaceObject[playerid], g_PlaceFrom[playerid][3], g_PlaceFrom[playerid][4], g_PlaceFrom[playerid][5]);
    }
}

// Ends an edit left open (a new one started, the player left); nothing moved is kept.
stock Place_Stop(playerid)
{
    if (g_PlaceKind[playerid] == PLACE_NONE)
    {
        return;
    }
    if (IsPlayerConnected(playerid))
    {
        CancelEdit(playerid);
    }
    Place_Undo(playerid);
}

stock Place_OnEditObject(playerid, STREAMER_TAG_OBJECT:objectid, response, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz)
{
    if (g_PlaceKind[playerid] == PLACE_NONE || objectid != g_PlaceObject[playerid] || EDIT_RESPONSE:response == EDIT_RESPONSE_UPDATE)
    {
        return;
    }
    if (EDIT_RESPONSE:response != EDIT_RESPONSE_FINAL)
    {
        Place_Undo(playerid);
        Msg_Info(playerid, "Postavljanje je otkazano.");
        return;
    }
    // Checked again on saving: the house may have been sold, or the player may have left it while moving.
    new slot = House_FindSlotByDbId(g_PlaceHouse[playerid]);
    if (slot == -1 || !House_IsOwner(playerid, slot) || House_FindInsideAnywhere(playerid) != slot || !Place_StillThere(playerid, g_PlaceKind[playerid]))
    {
        Place_Undo(playerid);
        Msg_Error(playerid, "Postavljanje je prekinuto: nameste se samo u svojoj kuci, dok ste u njoj.");
        return;
    }
    if (!Place_IsInsideHouse(slot, x, y, z) || GetPlayerDistanceFromPoint(playerid, x, y, z) > 10.0)
    {
        Place_Undo(playerid);
        Msg_Error(playerid, "Predmet mora ostati u kuci, najvise 10 m od vas.");
        return;
    }
    new E_PLACE_KIND:kind = g_PlaceKind[playerid];
    g_PlaceKind[playerid] = PLACE_NONE;
    SetDynamicObjectPos(objectid, x, y, z);
    SetDynamicObjectRot(objectid, rx, ry, rz);
    if (kind == PLACE_SAFE)
    {
        Safe_OnPlaced(playerid, slot, x, y, z, rx, ry, rz);
    }
    else
    {
        Furn_OnPlaced(playerid, g_PlaceRef[playerid], slot, x, y, z, rx, ry, rz);
    }
}
