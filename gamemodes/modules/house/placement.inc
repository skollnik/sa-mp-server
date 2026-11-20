// Moving furniture inside a house with the game's own mouse arrows; furniture only says what happens when a piece is put down.
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

stock bool:Place_BlocksExit(slot, Float:x, Float:y)
{
    new Float:ex, Float:ey, Float:ez, Float:ea;
    House_ExitPos(slot, ex, ey, ez, ea);
    return VectorSize(x - ex, y - ey, 0.0) < FURN_EXIT_CLEAR;
}

stock bool:Place_IsEditing(playerid)
{
    return g_PlaceKind[playerid] != PLACE_NONE;
}

// The height at which the model's bottom rests on the surface under the point.
stock bool:Place_FloorZ(model, Float:x, Float:y, Float:from, &Float:z)
{
    new Float:box[6], Float:floorz;
    if (!Collision_ModelBox(model, box) || !Collision_FloorBelow(x, y, from, 3.0, floorz))
    {
        return false;
    }
    z = floorz - box[2];
    return true;
}

// The spot a little in front of the player, on the floor, turned to face them.
stock Place_InFront(playerid, model, &Float:x, &Float:y, &Float:z, &Float:rz)
{
    new Float:angle, Float:pz;
    GetPlayerPos(playerid, x, y, pz);
    GetPlayerFacingAngle(playerid, angle);
    x += 1.2 * floatsin(-angle, degrees);
    y += 1.2 * floatcos(-angle, degrees);
    rz = angle + 180.0;
    // Without the collision data the origin goes where the floor is under a standing player's middle.
    if (!Place_FloorZ(model, x, y, pz, z))
    {
        z = pz - 1.0;
    }
}

static stock Place_BoxCenter(const Float:box[6], Float:x, Float:y, Float:z, Float:rz, &Float:cx, &Float:cy, &Float:cz)
{
    Collision_LocalToWorld(x, y, rz, (box[0] + box[3]) / 2.0, (box[1] + box[4]) / 2.0, cx, cy);
    cz = z + (box[2] + box[5]) / 2.0;
}

// Rays from the middle of the box out to each side, ending reach past it (a negative reach stops short of the side).
static stock bool:Place_SideHit(const Float:box[6], Float:x, Float:y, Float:z, Float:rz, Float:reach)
{
    new Float:cx, Float:cy, Float:cz, Float:mx = (box[0] + box[3]) / 2.0, Float:my = (box[1] + box[4]) / 2.0, Float:ends[4][2], Float:wx, Float:wy;
    Place_BoxCenter(box, x, y, z, rz, cx, cy, cz);
    ends[0][0] = box[0] - reach;
    ends[0][1] = my;
    ends[1][0] = box[3] + reach;
    ends[1][1] = my;
    ends[2][0] = mx;
    ends[2][1] = box[1] - reach;
    ends[3][0] = mx;
    ends[3][1] = box[4] + reach;
    for (new i = 0; i < 4; i++)
    {
        // Thinner than the allowance on this side (a picture): stopping short would cross the middle.
        new Float:half = i < 2 ? (box[3] - box[0]) / 2.0 : (box[4] - box[1]) / 2.0;
        if (reach < 0.0 && half <= -reach)
        {
            continue;
        }
        Collision_LocalToWorld(x, y, rz, ends[i][0], ends[i][1], wx, wy);
        if (Collision_Blocked(cx, cy, cz, wx, wy, cz))
        {
            return true;
        }
    }
    return false;
}

static stock bool:Place_Hangs(const Float:box[6], Float:x, Float:y, Float:z, Float:rz)
{
    new Float:cx, Float:cy, Float:cz;
    Place_BoxCenter(box, x, y, z, rz, cx, cy, cz);
    return Place_SideHit(box, x, y, z, rz, PLACE_TOUCH) || Collision_Blocked(cx, cy, cz, cx, cy, z + box[5] + PLACE_TOUCH);
}

static stock bool:Place_SupportBelow(slot, ignore, Float:x, Float:y, Float:from, &Float:support)
{
    new Float:top, bool:found = Collision_FloorBelow(x, y, from, 20.0, support);
    if (Furn_TopBelow(g_Houses[slot][hDbId], ignore, x, y, from, top) && (!found || top > support))
    {
        support = top;
        found = true;
    }
    return found;
}

// Puts the object down on the floor or a piece under it, or keeps a hanging one where it touches a wall, and refuses
// a spot behind or inside a wall. Without the collision data it accepts everything as it is.
stock bool:Place_Settle(playerid, slot, model, bool:hangs, ignore, &Float:x, &Float:y, &Float:z, &Float:rx, &Float:ry, &Float:rz)
{
    new Float:box[6];
    if (!Collision_ModelBox(model, box))
    {
        return true;
    }
    // Standing pieces stay upright, so none can be tipped over into a ramp or a wall.
    if (!hangs)
    {
        rx = 0.0;
        ry = 0.0;
    }
    new Float:cx, Float:cy, Float:cz, Float:support;
    Place_BoxCenter(box, x, y, z, rz, cx, cy, cz);
    if (!hangs || !Place_Hangs(box, x, y, z, rz))
    {
        if (!Place_SupportBelow(slot, ignore, cx, cy, z + box[2] + PLACE_LIFT, support))
        {
            Msg_Error(playerid, "Ispod predmeta nema poda; postavite ga u prostoriju.");
            return false;
        }
        z = support - box[2];
        cz = z + (box[2] + box[5]) / 2.0;
    }
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);
    if (Collision_Blocked(px, py, pz + 0.5, cx, cy, cz))
    {
        Msg_Error(playerid, "Izmedju vas i predmeta je zid; predmet mora biti u prostoriji u kojoj ste.");
        return false;
    }
    if (Place_SideHit(box, x, y, z, rz, -PLACE_WALL_MARGIN))
    {
        Msg_Error(playerid, "Predmet ulazi u zid; odmaknite ga malo.");
        return false;
    }
    return true;
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

// The object may have been destroyed meanwhile (house sold) and its id given to another object, which must not be moved.
static stock bool:Place_StillThere(playerid)
{
    return Furn_IsObject(g_PlaceRef[playerid], g_PlaceHouse[playerid], g_PlaceObject[playerid]);
}

static stock Place_Undo(playerid)
{
    g_PlaceKind[playerid] = PLACE_NONE;
    if (g_PlaceFresh[playerid])
    {
        Furn_OnPlaceCancelled(g_PlaceRef[playerid], g_PlaceHouse[playerid]);
        return;
    }
    if (Place_StillThere(playerid))
    {
        SetDynamicObjectPos(g_PlaceObject[playerid], g_PlaceFrom[playerid][0], g_PlaceFrom[playerid][1], g_PlaceFrom[playerid][2]);
        SetDynamicObjectRot(g_PlaceObject[playerid], g_PlaceFrom[playerid][3], g_PlaceFrom[playerid][4], g_PlaceFrom[playerid][5]);
    }
}

// A refused spot: the piece goes back to where the edit began and the arrows come back, so another spot can be tried.
static stock Place_Retry(playerid)
{
    SetDynamicObjectPos(g_PlaceObject[playerid], g_PlaceFrom[playerid][0], g_PlaceFrom[playerid][1], g_PlaceFrom[playerid][2]);
    SetDynamicObjectRot(g_PlaceObject[playerid], g_PlaceFrom[playerid][3], g_PlaceFrom[playerid][4], g_PlaceFrom[playerid][5]);
    EditDynamicObject(playerid, g_PlaceObject[playerid]);
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
    if (slot == -1 || !House_IsOwner(playerid, slot) || House_FindInsideAnywhere(playerid) != slot || !Place_StillThere(playerid))
    {
        Place_Undo(playerid);
        Msg_Error(playerid, "Postavljanje je prekinuto: namesta se samo u svojoj kuci, dok ste u njoj.");
        return;
    }
    if (!Place_IsInsideHouse(slot, x, y, z) || GetPlayerDistanceFromPoint(playerid, x, y, z) > 10.0)
    {
        Place_Retry(playerid);
        Msg_Error(playerid, "Predmet mora ostati u kuci, najvise 10 m od vas.");
        return;
    }
    if (Place_BlocksExit(slot, x, y))
    {
        Place_Retry(playerid);
        Msg_Error(playerid, "Predmet ne sme stajati na izlazu (najmanje %.1f m od njega).", FURN_EXIT_CLEAR);
        return;
    }
    new index = g_PlaceRef[playerid], model = g_Furn[index][fModel];
    if (!Place_Settle(playerid, slot, model, Furn_CanHang(model), index, x, y, z, rx, ry, rz))
    {
        Place_Retry(playerid);
        return;
    }
    g_PlaceKind[playerid] = PLACE_NONE;
    SetDynamicObjectPos(objectid, x, y, z);
    SetDynamicObjectRot(objectid, rx, ry, rz);
    Furn_OnPlaced(playerid, g_PlaceRef[playerid], slot, x, y, z, rx, ry, rz);
}
