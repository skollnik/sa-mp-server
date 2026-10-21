// Global server settings. Gameplay numbers (prices, wages) will get their own file.

#define SERVER_NAME         "Test Server"
#define SERVER_MODE_TEXT    "Roleplay"

// The owner always gets the highest admin level and may use a non Firstname_Lastname name.
#define OWNER_NAME          "skollnik"
#define ADMIN_LEVEL_OWNER   (6)

// Spawn for new characters: Los Santos International Airport, in front of the terminal.
#define NEW_SPAWN_X         (1642.30)
#define NEW_SPAWN_Y         (-2333.63)
#define NEW_SPAWN_Z         (13.55)
#define NEW_SPAWN_A         (180.0)

// Camera shown while the player is on the login/register screen (view over Los Santos).
#define LOGIN_CAMERA_X      (1468.88)
#define LOGIN_CAMERA_Y      (-919.25)
#define LOGIN_CAMERA_Z      (100.15)
#define LOGIN_LOOKAT_X      (1468.39)
#define LOGIN_LOOKAT_Y      (-918.42)
#define LOGIN_LOOKAT_Z      (99.88)

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
#define LOGIN_TIMEOUT_MS    (60 * 1000)
#define REGISTER_TIMEOUT_MS (5 * 60 * 1000)
#define AUTOSAVE_INTERVAL_MS (10 * 60 * 1000)
