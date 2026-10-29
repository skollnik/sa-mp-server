// Bit positions are stored in the database; append new settings at the end, never reorder.
enum E_SETTING
{
    SETTING_HUD,
    SETTING_HUD_CLOCK,
    SETTING_HUD_ZONE
}

forward bool:Setting_IsOn(playerid, E_SETTING:setting);

new const g_SettingNames[E_SETTING][] =
{
    "HUD (sat, datum i kvart)",
    "Sat i datum",
    "Kvart"
};

// The settings dialog lists every E_SETTING first, then these rows.
enum
{
    SETTINGS_ROW_CHANNELS = _:E_SETTING
}
