forward bool:Vip_IsVip(playerid);
forward bool:Vip_HasLevel(playerid, level);
forward bool:Vip_IsInCombat(playerid);
forward bool:Vip_CheckCooldown(playerid, last_used, cooldown_s);
forward bool:Vip_HasFreeSlot(playerid);
forward bool:Vip_OnDialogResponse(playerid, dialogid, response, listitem);

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

#define VIP_RESERVED_SLOTS      (2)
#define VIP_EXPIRY_CHECK_MS     (60 * 1000)

// Teleport, repair and heal are convenience, not combat tools: cooldowns plus no use right after taking damage.
#define VIP_GOTO_COOLDOWN_S     (5 * 60)
#define VIP_FIX_COOLDOWN_S      (10 * 60)
#define VIP_HEAL_COOLDOWN_S     (15 * 60)
#define VIP_COMBAT_LOCK_S       (60)

// Civilian skins only; faction uniforms (police, medics, army) are never on these lists.
new const g_VipSkinsMale[] = { 17, 46, 59, 98, 101, 170, 185, 188, 240, 250 };
new const g_VipSkinsFemale[] = { 12, 55, 69, 91, 141, 150, 169, 192, 216, 233 };

enum E_VIP_STATE
{
    vLastGoto,
    vLastFix,
    vLastHeal,
    vLastDamage,
    bool:vTagHidden
}

new g_Vip[MAX_PLAYERS][E_VIP_STATE];
new g_VipExpiryTimer;
