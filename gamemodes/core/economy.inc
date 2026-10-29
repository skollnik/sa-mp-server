// Every price, wage and limit from docs/ECONOMY.md lives here, so balancing never means hunting through modules.

#define PAYDAY_MIN_MINUTES          (30)
#define PAYDAY_SOCIAL_WAGE          (100)
#define PAYDAY_INTEREST_RATE        (0.0005)
#define PAYDAY_INTEREST_MAX         (250)
#define PAYDAY_RESPECT              (1)
#define RESPECT_PER_LEVEL           (4)

// A player counts as away (no payday minutes) when the game is paused or they have not moved or typed for a while.
#define AFK_PAUSE_MS                (10000)
#define AFK_IDLE_MINUTES            (5)

#define TRANSFER_MIN_LEVEL          (2)
#define TRANSFER_FEE_PERCENT        (1)
#define ATM_MAX_WITHDRAW            (10000)
#define GIVE_CASH_RADIUS            (3.0)

#define ID_CARD_PRICE               (50)

#define PHONE_PRICE                 (500)
#define PHONE_SMS_PRICE             (1)
#define PHONE_CALL_MINUTE_PRICE     (2)

// Percentage added to the payday per VIP level.
new const g_VipPaydayBonus[] = { 0, 5, 10, 15 };
// Inventory slots added per VIP level.
new const g_VipInventorySlots[] = { 0, 2, 4, 6 };
