enum E_FURN_CATEGORY
{
    fcName[24],
    fcItem[16],
    fcPrice,
    // Lamps, pictures and shelves may hang on a wall or from the ceiling; the rest has to stand on something.
    bool:fcHangs
}

// One price per category keeps the shop readable; the pieces are picked by their pictures, so they need no names of their own.
static const g_FurnCategories[][E_FURN_CATEGORY] =
{
    { "Kreveti",           "Krevet",      800, false },
    { "Kaucevi i fotelje", "Sedenje",     600, false },
    { "Stolice",           "Stolica",     200, false },
    { "Stolovi",           "Sto",         300, false },
    { "Elektronika",       "Elektronika", 500, false },
    { "Kuhinja",           "Kuhinja",     400, false },
    { "Kupatilo",          "Kupatilo",    450, false },
    { "Ormari i police",   "Ormaric",     350, true },
    { "Svetla",            "Lampa",       150, true },
    { "Dekoracija",        "Dekoracija",  100, true }
};

// GTA's own furniture objects, so every client (0.3.7 and 0.3.DL) has them; { category, model }.
static const g_FurnCatalog[][2] =
{
    {0, 1700}, {0, 1701}, {0, 1745}, {0, 1771}, {0, 1793}, {0, 1794}, {0, 1795}, {0, 1796}, {0, 1797}, {0, 1798}, {0, 1799},
    {0, 1800}, {0, 1801}, {0, 1802}, {0, 1803}, {0, 1812}, {0, 2090}, {0, 2298}, {0, 2299}, {0, 2300}, {0, 2301}, {0, 2302},
    {1, 1702}, {1, 1703}, {1, 1704}, {1, 1705}, {1, 1706}, {1, 1707}, {1, 1708}, {1, 1709}, {1, 1710}, {1, 1711}, {1, 1712},
    {1, 1713}, {1, 1723}, {1, 1724}, {1, 1726}, {1, 1727}, {1, 1728}, {1, 1729}, {1, 1735}, {1, 1753}, {1, 1754}, {1, 1755},
    {1, 1756}, {1, 1757}, {1, 1758}, {1, 1759}, {1, 1760}, {1, 1761}, {1, 1762}, {1, 1763}, {1, 1764}, {1, 1765}, {1, 1766},
    {1, 1767}, {1, 1768}, {1, 1769}, {1, 2290}, {1, 2291},
    {2, 1663}, {2, 1671}, {2, 1714}, {2, 1715}, {2, 1716}, {2, 1720}, {2, 1721}, {2, 1806}, {2, 1810}, {2, 1811}, {2, 2079},
    {2, 2120}, {2, 2121}, {2, 2122}, {2, 2123}, {2, 2124}, {2, 2125}, {2, 2309}, {2, 2310}, {2, 2356},
    {3, 1433}, {3, 1516}, {3, 1737}, {3, 1813}, {3, 1814}, {3, 1815}, {3, 1817}, {3, 1818}, {3, 1819}, {3, 1820}, {3, 1822},
    {3, 1823}, {3, 2029}, {3, 2030}, {3, 2031}, {3, 2032}, {3, 2080}, {3, 2081}, {3, 2082}, {3, 2085}, {3, 2086}, {3, 2109},
    {3, 2110}, {3, 2111}, {3, 2115}, {3, 2116}, {3, 2117}, {3, 2118}, {3, 2119}, {3, 2311}, {3, 2313}, {3, 2314}, {3, 2315},
    {3, 2319}, {3, 2357},
    {4, 1429}, {4, 1518}, {4, 1717}, {4, 1748}, {4, 1749}, {4, 1750}, {4, 1751}, {4, 1752}, {4, 1781}, {4, 1786}, {4, 1791},
    {4, 1792}, {4, 2296}, {4, 2297}, {4, 2312}, {4, 2316}, {4, 2317}, {4, 2318}, {4, 2320}, {4, 2322}, {4, 1719}, {4, 1782},
    {4, 1783}, {4, 1785}, {4, 1787}, {4, 1788}, {4, 1790}, {4, 1809}, {4, 1839}, {4, 1840}, {4, 2028}, {4, 2099}, {4, 2100},
    {4, 2101}, {4, 2102}, {4, 2103}, {4, 2104}, {4, 2190}, {4, 2229}, {4, 2230}, {4, 2231}, {4, 2232}, {4, 2233},
    {5, 2013}, {5, 2014}, {5, 2015}, {5, 2016}, {5, 2017}, {5, 2018}, {5, 2019}, {5, 2022}, {5, 2127}, {5, 2128}, {5, 2129},
    {5, 2130}, {5, 2131}, {5, 2132}, {5, 2133}, {5, 2134}, {5, 2135}, {5, 2136}, {5, 2137}, {5, 2138}, {5, 2139}, {5, 2140},
    {5, 2141}, {5, 2142}, {5, 2143}, {5, 2144}, {5, 2145}, {5, 2147}, {5, 2149}, {5, 2338}, {5, 2339}, {5, 2340}, {5, 2341},
    {6, 2514}, {6, 2515}, {6, 2516}, {6, 2517}, {6, 2518}, {6, 2519}, {6, 2520}, {6, 2521}, {6, 2522}, {6, 2523}, {6, 2524},
    {6, 2525}, {6, 2526}, {6, 2527}, {6, 2528}, {6, 2738}, {6, 2739},
    {7, 1416}, {7, 1417}, {7, 1730}, {7, 1740}, {7, 1741}, {7, 1742}, {7, 1743}, {7, 1744}, {7, 2020}, {7, 2021}, {7, 2025},
    {7, 2063}, {7, 2087}, {7, 2088}, {7, 2089}, {7, 2091}, {7, 2095}, {7, 2161}, {7, 2162}, {7, 2163}, {7, 2164}, {7, 2167},
    {7, 2197}, {7, 2199}, {7, 2200}, {7, 2204}, {7, 2306}, {7, 2307}, {7, 2323}, {7, 2328}, {7, 2329}, {7, 2330}, {7, 2569},
    {7, 2570}, {7, 2571}, {7, 2573}, {7, 2574}, {7, 2576},
    {8, 2026}, {8, 2069}, {8, 2070}, {8, 2071}, {8, 2072}, {8, 2073}, {8, 2074}, {8, 2075}, {8, 2076}, {8, 2105}, {8, 2108},
    {8, 2196}, {8, 2238},
    {9, 948}, {9, 949}, {9, 950}, {9, 2001}, {9, 2010}, {9, 2011}, {9, 2194}, {9, 2195}, {9, 2203}, {9, 2240}, {9, 2241},
    {9, 2245}, {9, 2251}, {9, 2252}, {9, 2253}, {9, 2255}, {9, 2256}, {9, 2257}, {9, 2258}, {9, 2259}, {9, 2260}, {9, 2261},
    {9, 2262}, {9, 2263}, {9, 2264}, {9, 2265}, {9, 2266}, {9, 2267}, {9, 2268}, {9, 2269}, {9, 2270}, {9, 2811}, {9, 2815},
    {9, 2817}, {9, 2818}, {9, 2833}, {9, 2834}, {9, 2835}, {9, 2836}, {9, 2841}, {9, 2842}, {9, 2847}, {9, 1736}, {9, 2828}
};

static g_FurnShopCat[MAX_PLAYERS];
static g_FurnShopModel[MAX_PLAYERS];
static g_FurnBuyHouses[MAX_PLAYERS][HOUSE_LIST_SIZE];
// Pool indexes behind the pictures of the last list, and the piece picked for selling; both checked again when used.
static g_FurnListed[MAX_PLAYERS][FURN_LIST_SIZE];
static g_FurnPick[MAX_PLAYERS];

stock Furn_Init()
{
    mysql_tquery(g_SQL,
        "CREATE TABLE IF NOT EXISTS `house_furniture` (" \
        "`id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY," \
        "`house_id` INT UNSIGNED NOT NULL," \
        "`model` INT NOT NULL," \
        "`price` INT NOT NULL," \
        "`placed` TINYINT(1) NOT NULL DEFAULT 0," \
        "`x` FLOAT NOT NULL DEFAULT 0, `y` FLOAT NOT NULL DEFAULT 0, `z` FLOAT NOT NULL DEFAULT 0," \
        "`rx` FLOAT NOT NULL DEFAULT 0, `ry` FLOAT NOT NULL DEFAULT 0, `rz` FLOAT NOT NULL DEFAULT 0," \
        "INDEX `house_furniture_house` (`house_id`)" \
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
}

// After the houses, since an object needs its house's world and interior.
stock Furn_LoadAll()
{
    mysql_tquery(g_SQL, "SELECT * FROM `house_furniture`", "OnFurnitureLoaded");
}

static stock Furn_FreeIndex()
{
    for (new i = 0; i < MAX_FURNITURE; i++)
    {
        if (!g_Furn[i][fUsed])
        {
            return i;
        }
    }
    return -1;
}

static stock Furn_Hide(index)
{
    if (IsValidDynamicObject(g_Furn[index][fObject]))
    {
        DestroyDynamicObject(g_Furn[index][fObject]);
    }
    g_Furn[index][fObject] = STREAMER_TAG_OBJECT:INVALID_STREAMER_ID;
}

static stock Furn_MakeObject(index, slot)
{
    Furn_Hide(index);
    g_Furn[index][fObject] = CreateDynamicObject(g_Furn[index][fModel], g_Furn[index][fX], g_Furn[index][fY], g_Furn[index][fZ],
        g_Furn[index][fRX], g_Furn[index][fRY], g_Furn[index][fRZ], House_World(slot), g_HouseInteriors[g_Houses[slot][hInterior]][hiInterior],
        -1, FURN_STREAM_DISTANCE, FURN_STREAM_DISTANCE);
}

forward OnFurnitureLoaded();
public OnFurnitureLoaded()
{
    new rows = cache_num_rows(), loaded, house_dbid, placed;
    for (new row = 0; row < rows; row++)
    {
        new index = Furn_FreeIndex();
        cache_get_value_name_int(row, "house_id", house_dbid);
        new slot = House_FindSlotByDbId(house_dbid);
        if (index == -1 || slot == -1)
        {
            continue;
        }
        g_Furn[index][fUsed] = true;
        g_Furn[index][fHouseDbId] = house_dbid;
        cache_get_value_name_int(row, "id", g_Furn[index][fDbId]);
        cache_get_value_name_int(row, "model", g_Furn[index][fModel]);
        cache_get_value_name_int(row, "price", g_Furn[index][fPrice]);
        cache_get_value_name_int(row, "placed", placed);
        g_Furn[index][fPlaced] = placed != 0;
        cache_get_value_name_float(row, "x", g_Furn[index][fX]);
        cache_get_value_name_float(row, "y", g_Furn[index][fY]);
        cache_get_value_name_float(row, "z", g_Furn[index][fZ]);
        cache_get_value_name_float(row, "rx", g_Furn[index][fRX]);
        cache_get_value_name_float(row, "ry", g_Furn[index][fRY]);
        cache_get_value_name_float(row, "rz", g_Furn[index][fRZ]);
        g_Furn[index][fObject] = STREAMER_TAG_OBJECT:INVALID_STREAMER_ID;
        if (g_Furn[index][fPlaced])
        {
            Furn_MakeObject(index, slot);
        }
        loaded++;
    }
    printf("[HOUSES] %d furniture pieces loaded.", loaded);
    return 1;
}

static stock Furn_Save(index)
{
    if (g_Furn[index][fDbId] == 0)
    {
        return;
    }
    new query[256];
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `house_furniture` SET `placed` = %d, `x` = %f, `y` = %f, `z` = %f, `rx` = %f, `ry` = %f, `rz` = %f WHERE `id` = %d",
        g_Furn[index][fPlaced] ? 1 : 0, g_Furn[index][fX], g_Furn[index][fY], g_Furn[index][fZ], g_Furn[index][fRX], g_Furn[index][fRY], g_Furn[index][fRZ],
        g_Furn[index][fDbId]);
    mysql_tquery(g_SQL, query);
}

static stock Furn_Free(index)
{
    Furn_Hide(index);
    g_Furn[index][fUsed] = false;
}

stock Furn_Count(house_dbid)
{
    new count;
    for (new i = 0; i < MAX_FURNITURE; i++)
    {
        if (g_Furn[i][fUsed] && g_Furn[i][fHouseDbId] == house_dbid)
        {
            count++;
        }
    }
    return count;
}

stock Furn_Value(house_dbid)
{
    new value;
    for (new i = 0; i < MAX_FURNITURE; i++)
    {
        if (g_Furn[i][fUsed] && g_Furn[i][fHouseDbId] == house_dbid)
        {
            value += g_Furn[i][fPrice];
        }
    }
    return value;
}

// The house size sets the base and the owner's admin slots (/setmesta namestaj) add to it.
stock Furn_Limit(playerid, slot)
{
    static const limit[E_HOUSE_SIZE] = { 30, 50, 80 };
    return limit[House_Size(slot)] + g_Player[playerid][pExtraFurnSlots];
}

static stock Furn_ItemName(model, dest[], size = sizeof(dest))
{
    dest[0] = EOS;
    for (new i = 0; i < sizeof(g_FurnCatalog); i++)
    {
        if (g_FurnCatalog[i][1] == model)
        {
            strcopy(dest, g_FurnCategories[g_FurnCatalog[i][0]][fcItem], size);
            return;
        }
    }
    strcopy(dest, "Namestaj", size);
}

stock Furn_ShowShop(playerid)
{
    new body[512], line[64];
    strcopy(body, "Kategorija\tCena po komadu\n");
    for (new i = 0; i < sizeof(g_FurnCategories); i++)
    {
        format(line, sizeof(line), "%s\t$%d\n", g_FurnCategories[i][fcName], g_FurnCategories[i][fcPrice]);
        strcat(body, line);
    }
    ShowPlayerDialog(playerid, DIALOG_FURN_SHOP, DIALOG_STYLE_TABLIST_HEADERS, "Prodavnica namestaja", body, "Izaberi", "Zatvori");
}

static stock Furn_ShowCategory(playerid, category)
{
    new models[PREVIEW_MAX_ITEMS], count, title[48];
    for (new i = 0; i < sizeof(g_FurnCatalog) && count < sizeof(models); i++)
    {
        if (g_FurnCatalog[i][0] == category)
        {
            models[count++] = g_FurnCatalog[i][1];
        }
    }
    g_FurnShopCat[playerid] = category;
    format(title, sizeof(title), "%s ($%d)", g_FurnCategories[category][fcName], g_FurnCategories[category][fcPrice]);
    Preview_Show(playerid, PREVIEW_FURN_SHOP, PREVIEW_KIND_FURNITURE, title, models, count);
}

// The piece goes to one of the buyer's houses, unplaced, and is put in place from inside that house (/namestaj).
static stock Furn_Buy(playerid, slot)
{
    new category = g_FurnShopCat[playerid], price = g_FurnCategories[category][fcPrice], house_dbid = g_Houses[slot][hDbId];
    if (!Point_Require(playerid, POINT_FURNITURE) || !House_IsOwner(playerid, slot))
    {
        return;
    }
    if (Furn_Count(house_dbid) >= Furn_Limit(playerid, slot))
    {
        Msg_Error(playerid, "Ta kuca je puna namestaja (%d/%d). Prodajte nesto u kuci (/namestaj).", Furn_Count(house_dbid), Furn_Limit(playerid, slot));
        return;
    }
    new index = Furn_FreeIndex();
    if (index == -1)
    {
        Msg_Error(playerid, "Server je dostigao najvise namestaja; javite adminu.");
        return;
    }
    new reason[48];
    format(reason, sizeof(reason), "namestaj za kucu %d", house_dbid);
    if (!Money_Take(playerid, price, reason))
    {
        return;
    }
    Account_Save(playerid);
    static const empty[E_FURN];
    g_Furn[index] = empty;
    g_Furn[index][fUsed] = true;
    g_Furn[index][fHouseDbId] = house_dbid;
    g_Furn[index][fModel] = g_FurnShopModel[playerid];
    g_Furn[index][fPrice] = price;
    g_Furn[index][fObject] = STREAMER_TAG_OBJECT:INVALID_STREAMER_ID;
    new query[160];
    mysql_format(g_SQL, query, sizeof(query), "INSERT INTO `house_furniture` (`house_id`, `model`, `price`) VALUES (%d, %d, %d)", house_dbid, g_Furn[index][fModel], price);
    mysql_tquery(g_SQL, query, "OnFurnitureInserted", "dd", index, house_dbid);

    new name[16], address[48];
    Furn_ItemName(g_Furn[index][fModel], name);
    House_Address(slot, address);
    Msg_Success(playerid, "Kupili ste %s za $%d za kucu %s. Postavlja se u kuci: /namestaj.", name, price, address);
}

forward OnFurnitureInserted(index, house_dbid);
public OnFurnitureInserted(index, house_dbid)
{
    if (!g_Furn[index][fUsed] || g_Furn[index][fHouseDbId] != house_dbid || g_Furn[index][fDbId] != 0)
    {
        return 1;
    }
    g_Furn[index][fDbId] = cache_insert_id();
    // Placed before the id came back: the position is written now.
    if (g_Furn[index][fPlaced])
    {
        Furn_Save(index);
    }
    return 1;
}

stock Furn_OnShopSelected(playerid, index)
{
    new category = g_FurnShopCat[playerid], seen;
    g_FurnShopModel[playerid] = 0;
    for (new i = 0; i < sizeof(g_FurnCatalog); i++)
    {
        if (g_FurnCatalog[i][0] == category && seen++ == index)
        {
            g_FurnShopModel[playerid] = g_FurnCatalog[i][1];
            break;
        }
    }
    if (g_FurnShopModel[playerid] == 0 || !Point_Require(playerid, POINT_FURNITURE))
    {
        return;
    }
    new body[512], line[64], address[48], count;
    for (new i = 0; i < MAX_HOUSES && count < HOUSE_LIST_SIZE; i++)
    {
        if (House_IsOwner(playerid, i))
        {
            House_Address(i, address);
            format(line, sizeof(line), "%s\t%d/%d\n", address, Furn_Count(g_Houses[i][hDbId]), Furn_Limit(playerid, i));
            strcat(body, line);
            g_FurnBuyHouses[playerid][count++] = g_Houses[i][hDbId];
        }
    }
    if (count == 0)
    {
        Msg_Error(playerid, "Namestaj se kupuje za kucu, a vi nemate svoju kucu.");
        return;
    }
    if (count == 1)
    {
        Furn_Buy(playerid, House_FindSlotByDbId(g_FurnBuyHouses[playerid][0]));
        return;
    }
    new title[48];
    format(title, sizeof(title), "Za koju kucu? ($%d)", g_FurnCategories[category][fcPrice]);
    ShowPlayerDialog(playerid, DIALOG_FURN_HOUSE, DIALOG_STYLE_TABLIST, title, body, "Kupi", "Odustani");
}

// The house the owner stands in, or -1 after saying why not.
static stock Furn_HouseHere(playerid)
{
    new slot = House_FindInsideAnywhere(playerid);
    if (slot == -1 || !House_IsOwner(playerid, slot))
    {
        Msg_Error(playerid, "Namestaj se namesta u vasoj kuci; udjite unutra.");
        return -1;
    }
    if (Place_IsEditing(playerid))
    {
        Msg_Error(playerid, "Prvo zavrsite postavljanje (disketa ili ESC).");
        return -1;
    }
    return slot;
}

stock Furn_ShowMenu(playerid)
{
    new slot = Furn_HouseHere(playerid);
    if (slot == -1)
    {
        return;
    }
    new house_dbid = g_Houses[slot][hDbId], placed, waiting;
    for (new i = 0; i < MAX_FURNITURE; i++)
    {
        if (g_Furn[i][fUsed] && g_Furn[i][fHouseDbId] == house_dbid)
        {
            if (g_Furn[i][fPlaced]) placed++;
            else waiting++;
        }
    }
    new body[192], title[48];
    format(body, sizeof(body), "Postavi kupljen namestaj (%d)\nPomeri postavljen namestaj (%d)\nProdaj namestaj (vraca se %d%% cene)", waiting, placed, FURN_SELL_PERCENT);
    format(title, sizeof(title), "Namestaj (%d/%d komada)", placed + waiting, Furn_Limit(playerid, slot));
    ShowPlayerDialog(playerid, DIALOG_FURN_MENU, DIALOG_STYLE_LIST, title, body, "Izaberi", "Zatvori");
}

// menu says which pieces are listed: unplaced ones to place, placed ones to move, or all of them to sell.
static stock Furn_ShowList(playerid, E_PREVIEW_MENU:menu)
{
    new slot = Furn_HouseHere(playerid);
    if (slot == -1)
    {
        return;
    }
    new models[FURN_LIST_SIZE], count, house_dbid = g_Houses[slot][hDbId];
    for (new i = 0; i < MAX_FURNITURE && count < FURN_LIST_SIZE; i++)
    {
        if (!g_Furn[i][fUsed] || g_Furn[i][fHouseDbId] != house_dbid
            || (menu == PREVIEW_FURN_PLACE && g_Furn[i][fPlaced]) || (menu == PREVIEW_FURN_MOVE && !g_Furn[i][fPlaced]))
        {
            continue;
        }
        models[count] = g_Furn[i][fModel];
        g_FurnListed[playerid][count++] = i;
    }
    if (count == 0)
    {
        if (menu == PREVIEW_FURN_PLACE)
        {
            Msg_Info(playerid, "Nema kupljenog namestaja koji ceka. Kupuje se u prodavnici namestaja (/kupi).");
        }
        else
        {
            Msg_Info(playerid, "U kuci nema postavljenog namestaja.");
        }
        Furn_ShowMenu(playerid);
        return;
    }
    new title[32];
    if (menu == PREVIEW_FURN_PLACE)
    {
        strcopy(title, "Postavi namestaj");
    }
    else if (menu == PREVIEW_FURN_MOVE)
    {
        strcopy(title, "Pomeri namestaj");
    }
    else
    {
        strcopy(title, "Prodaj namestaj");
    }
    Preview_Show(playerid, menu, PREVIEW_KIND_FURNITURE, title, models, count);
}

// A listed piece is used only while it is still in the house the player stands in; the list may be old by the click.
static stock bool:Furn_IsHere(index, slot)
{
    return index >= 0 && index < MAX_FURNITURE && g_Furn[index][fUsed] && g_Furn[index][fHouseDbId] == g_Houses[slot][hDbId];
}

stock Furn_OnListSelected(playerid, E_PREVIEW_MENU:menu, row)
{
    new slot = Furn_HouseHere(playerid);
    if (slot == -1 || row < 0 || row >= FURN_LIST_SIZE)
    {
        return;
    }
    new index = g_FurnListed[playerid][row];
    if (!Furn_IsHere(index, slot))
    {
        return;
    }
    if (menu == PREVIEW_FURN_SELL)
    {
        new text[160], name[16];
        Furn_ItemName(g_Furn[index][fModel], name);
        g_FurnPick[playerid] = index;
        format(text, sizeof(text), "Prodajete %s za $%d (%d%% od $%d).", name, g_Furn[index][fPrice] * FURN_SELL_PERCENT / 100, FURN_SELL_PERCENT, g_Furn[index][fPrice]);
        ShowPlayerDialog(playerid, DIALOG_FURN_SELL, DIALOG_STYLE_MSGBOX, "Prodaja namestaja", text, "Prodaj", "Odustani");
        return;
    }
    if (g_Furn[index][fPlaced] != (menu == PREVIEW_FURN_MOVE))
    {
        return;
    }
    if (menu == PREVIEW_FURN_PLACE)
    {
        Place_InFront(playerid, g_Furn[index][fModel], g_Furn[index][fX], g_Furn[index][fY], g_Furn[index][fZ], g_Furn[index][fRZ]);
        g_Furn[index][fRX] = 0.0;
        g_Furn[index][fRY] = 0.0;
        Furn_MakeObject(index, slot);
    }
    Place_Start(playerid, PLACE_FURNITURE, index, slot, g_Furn[index][fObject], menu == PREVIEW_FURN_PLACE);
}

stock Furn_OnPlaced(playerid, index, slot, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz)
{
    if (!Furn_IsHere(index, slot))
    {
        return;
    }
    g_Furn[index][fPlaced] = true;
    g_Furn[index][fX] = x;
    g_Furn[index][fY] = y;
    g_Furn[index][fZ] = z;
    g_Furn[index][fRX] = rx;
    g_Furn[index][fRY] = ry;
    g_Furn[index][fRZ] = rz;
    Furn_Save(index);
    Msg_Success(playerid, "Namestaj je postavljen. Jos opcija: /namestaj");
}

// A piece just taken out for placing goes back to the unplaced ones.
stock Furn_OnPlaceCancelled(index, house_dbid)
{
    if (index >= 0 && index < MAX_FURNITURE && g_Furn[index][fUsed] && g_Furn[index][fHouseDbId] == house_dbid && !g_Furn[index][fPlaced])
    {
        Furn_Hide(index);
    }
}

stock bool:Furn_CanHang(model)
{
    for (new i = 0; i < sizeof(g_FurnCatalog); i++)
    {
        if (g_FurnCatalog[i][1] == model)
        {
            return g_FurnCategories[g_FurnCatalog[i][0]][fcHangs];
        }
    }
    return false;
}

// The highest top of the house's pieces under the point, no higher than limit, so a TV can stand on a table.
stock bool:Furn_TopBelow(house_dbid, ignore, Float:x, Float:y, Float:limit, &Float:top)
{
    new bool:found, Float:box[6], Float:lx, Float:ly, Float:t;
    for (new i = 0; i < MAX_FURNITURE; i++)
    {
        if (i == ignore || !g_Furn[i][fUsed] || !g_Furn[i][fPlaced] || g_Furn[i][fHouseDbId] != house_dbid || !Collision_ModelBox(g_Furn[i][fModel], box))
        {
            continue;
        }
        Collision_WorldToLocal(g_Furn[i][fX], g_Furn[i][fY], g_Furn[i][fRZ], x, y, lx, ly);
        t = g_Furn[i][fZ] + box[5];
        if (lx < box[0] || lx > box[3] || ly < box[1] || ly > box[4] || t > limit || (found && t <= top))
        {
            continue;
        }
        top = t;
        found = true;
    }
    return found;
}

stock bool:Furn_IsObject(index, house_dbid, STREAMER_TAG_OBJECT:objectid)
{
    return index >= 0 && index < MAX_FURNITURE && g_Furn[index][fUsed] && g_Furn[index][fHouseDbId] == house_dbid && g_Furn[index][fObject] == objectid;
}

static stock Furn_Sell(playerid)
{
    new slot = Furn_HouseHere(playerid), index = g_FurnPick[playerid];
    if (slot == -1 || !Furn_IsHere(index, slot))
    {
        return;
    }
    new refund = g_Furn[index][fPrice] * FURN_SELL_PERCENT / 100, query[96], reason[48];
    mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `house_furniture` WHERE `id` = %d", g_Furn[index][fDbId]);
    mysql_tquery(g_SQL, query);
    Furn_Free(index);
    format(reason, sizeof(reason), "prodaja namestaja iz kuce %d", g_Houses[slot][hDbId]);
    Money_GiveCash(playerid, refund, reason);
    Account_Save(playerid);
    Msg_Success(playerid, "Prodali ste namestaj za $%d.", refund);
    Furn_ShowMenu(playerid);
}

stock Furn_DeleteHouse(slot)
{
    new house_dbid = g_Houses[slot][hDbId], query[80];
    for (new i = 0; i < MAX_FURNITURE; i++)
    {
        if (g_Furn[i][fUsed] && g_Furn[i][fHouseDbId] == house_dbid)
        {
            Furn_Free(i);
        }
    }
    mysql_format(g_SQL, query, sizeof(query), "DELETE FROM `house_furniture` WHERE `house_id` = %d", house_dbid);
    mysql_tquery(g_SQL, query);
}

// A new interior has another layout, so everything placed waits to be put down again.
stock Furn_UnplaceAll(slot)
{
    new house_dbid = g_Houses[slot][hDbId], query[96];
    for (new i = 0; i < MAX_FURNITURE; i++)
    {
        if (g_Furn[i][fUsed] && g_Furn[i][fHouseDbId] == house_dbid)
        {
            Furn_Hide(i);
            g_Furn[i][fPlaced] = false;
        }
    }
    mysql_format(g_SQL, query, sizeof(query), "UPDATE `house_furniture` SET `placed` = 0 WHERE `house_id` = %d", house_dbid);
    mysql_tquery(g_SQL, query);
}

cmd:namestaj(playerid, params[])
{
    Furn_ShowMenu(playerid);
    return 1;
}
alias:namestaj("furniture")

stock bool:Furn_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[])
{
    #pragma unused inputtext
    switch (dialogid)
    {
        case DIALOG_FURN_SHOP:
        {
            if (response && listitem >= 0 && listitem < sizeof(g_FurnCategories) && Point_Require(playerid, POINT_FURNITURE))
            {
                Furn_ShowCategory(playerid, listitem);
            }
        }
        case DIALOG_FURN_HOUSE:
        {
            if (response && listitem >= 0 && listitem < HOUSE_LIST_SIZE)
            {
                new slot = House_FindSlotByDbId(g_FurnBuyHouses[playerid][listitem]);
                if (slot != -1)
                {
                    Furn_Buy(playerid, slot);
                }
            }
        }
        case DIALOG_FURN_MENU:
        {
            if (!response)
            {
                return true;
            }
            switch (listitem)
            {
                case 0: Furn_ShowList(playerid, PREVIEW_FURN_PLACE);
                case 1: Furn_ShowList(playerid, PREVIEW_FURN_MOVE);
                case 2: Furn_ShowList(playerid, PREVIEW_FURN_SELL);
            }
        }
        case DIALOG_FURN_SELL:
        {
            if (response)
            {
                Furn_Sell(playerid);
            }
            else
            {
                Furn_ShowMenu(playerid);
            }
        }
        default:
        {
            return false;
        }
    }
    return true;
}
