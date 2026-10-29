forward bool:Hunger_IsFull(playerid);

#define HUNGER_MAX              (100)
#define HUNGER_DROP_MINUTES     (3)
#define HUNGER_WARNING          (20)
// Above this the player is full and cannot eat; drinks still go down.
#define HUNGER_FULL             (90)
#define HUNGER_AFTER_DEATH      (50)
#define HUNGER_HP_LOSS          (2.0)
// Until the hospital exists nobody dies of hunger: HP stops falling here.
#define HUNGER_MIN_HP           (20.0)

new g_HungerMinutes[MAX_PLAYERS];
new g_HungerTimer;
