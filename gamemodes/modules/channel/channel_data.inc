forward bool:Channel_IsOn(playerid, E_CHANNEL:channel);

// Bit positions are stored in the database; append new channels at the end, never reorder.
enum E_CHANNEL
{
    CHANNEL_OOC,
    CHANNEL_PM,
    CHANNEL_NEWBIE,
    CHANNEL_VIP,
    CHANNEL_HELPER,
    CHANNEL_QUESTIONS,
    CHANNEL_ADMIN,
    CHANNEL_REPORTS,
    CHANNEL_ANTICHEAT,
    CHANNEL_SHARED_IP,
    CHANNEL_PM_SPY
}

new const g_ChannelNames[E_CHANNEL][] =
{
    "Globalni OOC (/o)",
    "Privatne poruke (/pm)",
    "Newbie chat (/n)",
    "VIP chat (/vc)",
    "Helper chat (/hc)",
    "Pitanja igraca (/ask)",
    "Admin chat (/a)",
    "Reportovi",
    "Anti-cheat upozorenja",
    "Ista IP adresa",
    "PM spy"
};

// PM spy is opt-in; every other channel starts switched on.
new const bool:g_ChannelDefaultOn[E_CHANNEL] =
{
    true, true, true, true, true, true, true, true, true, true, false
};

// The rows a player saw in their last /kanali dialog, so a click maps back to the right channel.
new E_CHANNEL:g_ChannelDialogRows[MAX_PLAYERS][E_CHANNEL];
new g_ChannelDialogRowCount[MAX_PLAYERS];
