forward bool:Vip_IsVip(playerid);

#define VIP_BRONZE              (1)
#define VIP_SILVER              (2)
#define VIP_GOLD                (3)
#define VIP_MAX_LEVEL           (VIP_GOLD)

#define VIP_EXPIRY_WARNING_DAYS (3)
#define SECONDS_PER_DAY         (86400)

new const g_VipNames[][] =
{
    "Nema", "Bronze VIP", "Silver VIP", "Gold VIP"
};

new const g_VipColors[] =
{
    0xFFFFFFFF, 0xCD7F32FF, 0xC0C0C0FF, 0xFFD700FF
};

new const g_VipColorTags[][] =
{
    "{FFFFFF}", "{CD7F32}", "{C0C0C0}", "{FFD700}"
};

// Percentage cut from support cooldowns (/report, /ask, /n) per VIP level.
new const g_VipCooldownCut[] =
{
    0, 25, 50, 50
};
