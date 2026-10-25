// The only place the server name lives: in-game messages use SERVER_NAME,
// the server browser shows SERVER_HOSTNAME (set on startup, overriding config.json).
#define SERVER_NAME         "Test Server"
#define SERVER_HOSTNAME     "SA-MP Test Server"
#define SERVER_MODE_TEXT    "Roleplay"

// The owner always gets the highest admin level and may use a non Firstname_Lastname name.
#define OWNER_NAME          "skollnik"
#define ADMIN_LEVEL_OWNER   (6)

// Spawn for new characters: Los Santos International Airport, in front of the terminal.
#define NEW_SPAWN_X         (1642.30)
#define NEW_SPAWN_Y         (-2333.63)
#define NEW_SPAWN_Z         (13.55)
#define NEW_SPAWN_A         (180.0)

// Login screen camera: slow pan above the Vinewood hills, looking south over the city.
#define LOGIN_CAMERA_FROM_X (1100.0)
#define LOGIN_CAMERA_FROM_Y (-800.0)
#define LOGIN_CAMERA_TO_X   (1600.0)
#define LOGIN_CAMERA_TO_Y   (-800.0)
#define LOGIN_CAMERA_Z      (160.0)
#define LOGIN_LOOKAT_FROM_X (1300.0)
#define LOGIN_LOOKAT_TO_X   (1700.0)
#define LOGIN_LOOKAT_Y      (-1300.0)
#define LOGIN_LOOKAT_Z      (60.0)
#define LOGIN_CAMERA_TIME_MS (40 * 1000)

// Neutral civilian skins handed out at random on registration.
new const g_StarterSkinsMale[] = { 7, 47, 60 };
new const g_StarterSkinsFemale[] = { 40, 56, 93 };

#define STARTER_CASH        (2000)
#define STARTER_BANK        (3000)

#define PASSWORD_MIN_LENGTH (6)
#define PASSWORD_MAX_LENGTH (32)
#define CHARACTER_MIN_AGE   (16)
#define CHARACTER_MAX_AGE   (80)

#define MAX_LOGIN_ATTEMPTS  (3)

// Brute-force protection across reconnects: this many failures within the window locks the account.
#define LOCKOUT_MAX_FAILURES    (5)
#define LOCKOUT_WINDOW_MINUTES  (15)
#define LOCKOUT_MINUTES         (15)
#define LOGIN_TIMEOUT_MS    (60 * 1000)
#define REGISTER_TIMEOUT_MS (5 * 60 * 1000)
#define AUTOSAVE_INTERVAL_MS (10 * 60 * 1000)

// Pawn.CMD flags. Chat commands are blocked by mute; only chat and JAIL_OK commands work in admin jail.
#define CMD_FLAG_CHAT       (1)
#define CMD_FLAG_JAIL_OK    (2)
