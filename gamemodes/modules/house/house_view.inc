// A house for sale may be looked at from inside before buying; the time limit keeps it from becoming a free hideout.
// Returns false, and says why, when the player has to wait before looking at this house again.
stock bool:HouseView_Start(playerid, slot)
{
    new dbid = g_Houses[slot][hDbId], wait = g_HouseViewBlockedUntil[playerid] - gettime();
    if (g_HouseViewBlocked[playerid] == dbid && wait > 0)
    {
        Msg_Error(playerid, "Ovu kucu ste upravo razgledali. Ponovo mozete za %d min.", (wait + 59) / 60);
        return false;
    }
    g_HouseViewing[playerid] = dbid;
    g_HouseViewEnd[playerid] = gettime() + HOUSE_VIEW_SECONDS;
    Msg_Info(playerid, "Razgledate kucu na prodaju ($%d): imate %d minuta. Kupuje se sa /kupikucu ispred vrata.", g_Houses[slot][hPrice], HOUSE_VIEW_SECONDS / 60);
    return true;
}

// Whoever is inside when a house goes back on sale gets the same time to leave as a visitor.
stock HouseView_StartAllInside(slot)
{
    new world = House_World(slot);
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (Account_IsLoggedIn(i) && GetPlayerVirtualWorld(i) == world && !g_Staff[i][sAdminDuty])
        {
            g_HouseViewing[i] = g_Houses[slot][hDbId];
            g_HouseViewEnd[i] = gettime() + HOUSE_VIEW_SECONDS;
            Msg_Info(i, "Kuca je sada na prodaju; imate %d minuta da izadjete.", HOUSE_VIEW_SECONDS / 60);
        }
    }
}

static stock HouseView_PutOut(playerid, slot, const message[])
{
    g_HouseViewing[playerid] = 0;
    House_PutOutside(playerid, slot);
    Msg_Info(playerid, "%s", message);
}

// Called when the house gets an owner: visitors may not stay in someone's home.
stock HouseView_EndAll(slot)
{
    new dbid = g_Houses[slot][hDbId], world = House_World(slot);
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (g_HouseViewing[i] == dbid && IsPlayerConnected(i) && GetPlayerVirtualWorld(i) == world)
        {
            HouseView_PutOut(i, slot, "Kuca je upravo prodata, pa izlazite napolje.");
        }
    }
}

forward HouseView_Tick();
public HouseView_Tick()
{
    new now = gettime();
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (g_HouseViewing[i] == 0)
        {
            continue;
        }
        new slot = House_FindSlotByDbId(g_HouseViewing[i]);
        // Left by the door, died, was jailed or moved by an admin: the visit is over.
        if (!IsPlayerConnected(i) || slot == -1 || GetPlayerVirtualWorld(i) != House_World(slot))
        {
            g_HouseViewing[i] = 0;
            continue;
        }
        if (now >= g_HouseViewEnd[i])
        {
            g_HouseViewBlocked[i] = g_HouseViewing[i];
            g_HouseViewBlockedUntil[i] = now + HOUSE_VIEW_COOLDOWN_S;
            HouseView_PutOut(i, slot, "Vreme za razgledanje kuce je isteklo.");
        }
    }
    return 1;
}
