// Bit positions are stored in the database; append new settings at the end, never reorder.
enum E_SETTING
{
    SETTING_HUD,
    SETTING_HUD_CLOCK,
    SETTING_HUD_ZONE,
    SETTING_HUD_LOGO
}

forward bool:Setting_IsOn(playerid, E_SETTING:setting);

new const g_SettingNames[E_SETTING][] =
{
    "HUD (sat, datum, kvart i logo)",
    "Sat i datum",
    "Kvart",
    "Logo, vas ID i broj igraca"
};

// The settings dialog lists every E_SETTING first, then these rows.
enum
{
    SETTINGS_ROW_CHANNELS = _:E_SETTING
}
