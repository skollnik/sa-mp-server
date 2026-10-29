// Bit positions are stored in the database; append new settings at the end, never reorder.
enum E_SETTING
{
    SETTING_HUD,
    SETTING_HUD_CLOCK,
    SETTING_HUD_ZONE,
    SETTING_HUD_HUNGER
}

forward bool:Setting_IsOn(playerid, E_SETTING:setting);

new const g_SettingNames[E_SETTING][] =
{
    "HUD (sve ispod)",
    "Sat i datum",
    "Kvart",
    "Traka gladi"
};

// The settings dialog lists every E_SETTING first, then these rows.
enum
{
    SETTINGS_ROW_RINGTONE = _:E_SETTING,
    SETTINGS_ROW_CHANNELS
}
