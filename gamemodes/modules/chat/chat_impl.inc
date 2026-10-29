#define CHAT_RADIUS_TALK    (20.0)
#define CHAT_RADIUS_SHOUT   (40.0)
#define CHAT_RADIUS_LOW     (5.0)
#define CHAT_RADIUS_WHISPER (3.0)
#define CHAT_RADIUS_ACTION  (20.0)
#define CHAT_RADIUS_OOC     (20.0)

// SA-MP drops client messages longer than 144 characters, so long lines are split in two.
#define CHAT_SPLIT_AT       (110)

// Off by default as on most heavy RP servers; admins turn it on for events and announcements.
new bool:g_GlobalOocEnabled = false;

stock Chat_FadeColor(Float:distance, Float:radius)
{
    new Float:ratio = distance / radius;

    if (ratio < 0.2)
    {
        return 0xE6E6E6FF;
    }
    if (ratio < 0.4)
    {
        return 0xC8C8C8FF;
    }
    if (ratio < 0.6)
    {
        return 0xAAAAAAFF;
    }
    if (ratio < 0.8)
    {
        return 0x8C8C8CFF;
    }
    return 0x6E6E6EFF;
}

// Player text always goes in as a "%s" argument: open.mp formats every message and a stray % can crash it.
// keep_colors is only for messages whose player text was already cleaned with Chat_CleanText.
stock Chat_Send(playerid, color, const message[], bool:keep_colors = false)
{
    new text[256];
    strcopy(text, message);
    if (!keep_colors)
    {
        Chat_StripColors(text);
    }

    new len = strlen(text);
    if (len <= CHAT_SPLIT_AT)
    {
        SendClientMessage(playerid, color, "%s", text);
        return;
    }

    new cut = CHAT_SPLIT_AT;
    for (new i = CHAT_SPLIT_AT; i > CHAT_SPLIT_AT - 40; i--)
    {
        if (text[i] == ' ')
        {
            cut = i;
            break;
        }
    }

    new part[160];
    strmid(part, text, 0, cut);
    strcat(part, " ...");
    SendClientMessage(playerid, color, "%s", part);

    strcopy(part, "... ");
    strcat(part, text[(text[cut] == ' ') ? cut + 1 : cut]);
    SendClientMessage(playerid, color, "%s", part);
}

stock Chat_SendNearby(playerid, Float:radius, color, const message[], bool:fade = false, bool:keep_colors = false)
{
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);

    new world = GetPlayerVirtualWorld(playerid),
        interior = GetPlayerInterior(playerid);

    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (!Account_IsLoggedIn(i) || GetPlayerVirtualWorld(i) != world || GetPlayerInterior(i) != interior)
        {
            continue;
        }

        new Float:distance = GetPlayerDistanceFromPoint(i, x, y, z);
        if (distance > radius)
        {
            continue;
        }

        Chat_Send(i, fade ? Chat_FadeColor(distance, radius) : color, message, keep_colors);
    }
}

stock Chat_Clear(playerid, lines = 50)
{
    for (new i = 0; i < lines; i++)
    {
        SendClientMessage(playerid, COLOR_WHITE, " ");
    }
}

// Removes {RRGGBB} colour codes so players cannot dress their text up as a system or admin message.
stock Chat_StripColors(text[])
{
    new read, write;
    while (text[read] != '\0')
    {
        if (text[read] == '{' && strlen(text[read]) >= 8 && text[read + 7] == '}')
        {
            new bool:is_code = true;
            for (new i = 1; i <= 6; i++)
            {
                new c = text[read + i];
                if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')))
                {
                    is_code = false;
                    break;
                }
            }
            if (is_code)
            {
                read += 8;
                continue;
            }
        }
        text[write++] = text[read++];
    }
    text[write] = '\0';
}

stock Chat_SendToAll(color, const message[])
{
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (Account_IsLoggedIn(i))
        {
            Chat_Send(i, color, message);
        }
    }
}

stock Chat_OnPlayerText(playerid, const text[])
{
    new message[192];
    format(message, sizeof(message), "%s kaze: %s", Account_Name(playerid), text);
    Chat_SendNearby(playerid, CHAT_RADIUS_TALK, COLOR_WHITE, message, true);
}

stock bool:Chat_IsInRange(playerid, targetid, Float:radius)
{
    if (GetPlayerVirtualWorld(playerid) != GetPlayerVirtualWorld(targetid) || GetPlayerInterior(playerid) != GetPlayerInterior(targetid))
    {
        return false;
    }

    new Float:x, Float:y, Float:z;
    GetPlayerPos(targetid, x, y, z);
    return GetPlayerDistanceFromPoint(playerid, x, y, z) <= radius;
}

stock Chat_ShowBubble(playerid, const message[])
{
    new text[192];
    strcopy(text, message);
    Chat_StripColors(text);
    SetPlayerChatBubble(playerid, "%s", COLOR_PURPLE, CHAT_RADIUS_ACTION, 8000, text);
}

// Player text without colour codes, for messages that carry intentional colours of their own.
stock Chat_CleanText(const text[])
{
    new clean[144];
    strcopy(clean, text);
    Chat_StripColors(clean);
    return clean;
}

// Name in the VIP colour for OOC chats, switching back to the chat's own colour afterwards.
stock Chat_ColoredName(playerid, const chat_color[])
{
    new text[64];
    if (Vip_IsVip(playerid))
    {
        format(text, sizeof(text), "%s%s%s", g_VipColorTags[Vip_GetLevel(playerid)], Account_Name(playerid), chat_color);
    }
    else
    {
        strcopy(text, Account_Name(playerid));
    }
    return text;
}

// "* Name does something" to everyone nearby, as /me prints it; also used by systems for automatic RP lines.
stock Chat_Action(playerid, const action[])
{
    new message[192];
    format(message, sizeof(message), "* %s %s", Account_Name(playerid), action);
    Chat_SendNearby(playerid, CHAT_RADIUS_ACTION, COLOR_PURPLE, message);
}
