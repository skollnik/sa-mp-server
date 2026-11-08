// Every price, wage and limit from docs/ECONOMY.md lives here, so balancing never means hunting through modules.

#define PAYDAY_MIN_MINUTES          (30)
#define PAYDAY_SOCIAL_WAGE          (100)
#define PAYDAY_INTEREST_RATE        (0.0005)
#define PAYDAY_INTEREST_MAX         (250)
#define PAYDAY_RESPECT              (1)
// The next level costs (next level x this) respect, so level 2 takes 8 paydays and level 3 another 12.
#define RESPECT_PER_LEVEL           (4)
#define HAPPY_HOUR_MAX_PAYDAYS      (12)

// A player counts as away (no payday minutes) when the game is paused or they have not moved or typed for a while.
#define AFK_PAUSE_MS                (10000)
#define AFK_IDLE_MINUTES            (5)

#define TRANSFER_MIN_LEVEL          (2)
#define TRANSFER_FEE_PERCENT        (1)
#define ATM_MAX_WITHDRAW            (10000)
#define GIVE_CASH_RADIUS            (3.0)
// Opening an account is free but needs the ID card; the PIN is random and a new one is the only way back from a forgotten PIN.
#define BANK_PIN_RESET_PRICE        (50)

// Dice and coin bets: what each side puts in; from level 2 so fresh alt accounts cannot pass money around.
#define BET_MIN_STAKE               (10)
#define BET_MAX_STAKE               (5000)
#define BET_MIN_LEVEL               (2)

// Begging: only for new players; the daily caps keep it from becoming a way to move money between accounts.
#define BEG_MAX_LEVEL               (4)
#define BEG_GIFT_MAX                (100)
#define BEG_GIVER_DAILY_MAX         (100)
#define BEG_DAILY_MAX               (1000)

// Lottery: only part of each ticket goes into the prize fund, the rest leaves the game (later: the state treasury).
#define LOTTERY_TICKET_PRICE        (50)
#define LOTTERY_POOL_PERCENT        (70)
#define LOTTERY_MAX_NUMBER          (50)

#define ID_CARD_PRICE               (50)

#define VEH_MAX_PER_PLAYER          (2)
#define FUEL_PRICE_PER_LITRE        (3)
// Registration at the technical inspection, about an hour of work for a car; valid VEH_REGISTRATION_DAYS.
#define REG_PRICE_MOTORBIKE         (500)
#define REG_PRICE_CAR               (1000)
#define REG_PRICE_LARGE             (2000)
// Plates come from the police counter; a custom plate is a luxury, and Gold VIP pays half.
#define PLATE_FIRST_PRICE           (300)
#define PLATE_NEW_PRICE             (1500)
#define PLATE_CUSTOM_PRICE          (60000)
// Camera fines go to the vehicle owner and leave the game when paid (later: the state treasury).
#define FINE_UNREGISTERED           (500)
#define FINE_SPEED_LOW              (150)
#define FINE_SPEED_MID              (400)
#define FINE_SPEED_HIGH             (800)
#define FINE_NO_BELT                (200)
#define FINE_NO_HELMET              (200)

#define PHONE_PRICE                 (500)
#define PHONE_SIM_PRICE             (50)
// Silver and Gold VIP and admins pick a number for free.
#define PHONE_PICK_NUMBER_PRICE     (2000)
#define PHONE_CREDIT_MAX            (500)
#define PHONE_DIRECTORY_PRICE       (2)
#define PHONE_SMS_PRICE             (1)
#define PHONE_CALL_MINUTE_PRICE     (2)
// A number whose owner has not logged in for this long is freed for others.
#define PHONE_INACTIVE_DAYS         (180)

// Credit top-up amounts, the same at a 24/7 and from the bank account.
new const g_PhoneTopups[] = { 10, 20, 50, 100 };

// Percentage added to the payday per VIP level.
new const g_VipPaydayBonus[] = { 0, 5, 10, 15 };
// Inventory slots added per VIP level.
new const g_VipInventorySlots[] = { 0, 2, 4, 6 };

// Licenses (driving school); the test is paid again after a failed attempt.
#define LIC_PRICE_CAR               (1500)
#define LIC_PRICE_MOTO              (1000)
#define LIC_PRICE_TRUCK             (5000)
#define LIC_PRICE_BOAT              (7500)
#define LIC_PRICE_FLY               (50000)
#define LIC_PRICE_FISH              (250)

// Mechanic repairs have a fixed price per repaired percent, so two friends cannot repair for pennies.
// The field costs more than the workshop, since the mechanic comes to the vehicle.
#define MECH_PRICE_FIELD_PERCENT    (20)
#define MECH_PRICE_SHOP_PERCENT     (15)
#define MECH_PRICE_TYRE             (50)
// The mechanic keeps this share; the rest are parts and leave the game.
#define MECH_SHARE                  (60)
// The workshop's own repair when no mechanic is at work, per percent up to 100%; all of it leaves the game.
#define MECH_SERVICE_PERCENT        (20)
// Tuning and paint at the mechanic: the customer pays the part, the mechanic keeps this share and the rest leaves the game.
#define TUNE_MECHANIC_SHARE         (30)
#define TUNE_PRICE_PAINT            (300)
#define TUNE_PRICE_PAINTJOB         (1500)
// Gold VIP pays half, like a custom plate.
#define TUNE_PRICE_NEON             (5000)
#define TUNE_PRICE_NITRO_X2         (1500)
#define TUNE_PRICE_NITRO_X5         (3000)
#define TUNE_PRICE_NITRO_X10        (5000)

// Rent-a-car, per hour.
#define RENT_PRICE_BIKE             (20)
#define RENT_PRICE_MOTO             (80)
#define RENT_PRICE_CAR              (200)
