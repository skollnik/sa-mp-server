enum E_ACCOUNT_STATE
{
    ACCOUNT_STATE_NONE,         // just connected, waiting for the DB lookup
    ACCOUNT_STATE_LOGIN,        // account exists, waiting for the password
    ACCOUNT_STATE_REGISTER,     // new account, going through the register dialogs
    ACCOUNT_STATE_LOGGED_IN
}

enum E_PLAYER_DATA
{
    pDbId,
    pName[MAX_PLAYER_NAME + 1],
    pPasswordHash[BCRYPT_HASH_LENGTH],
    pAdminLevel,
    pHelperLevel,
    pChannelBits,
    pMuteUntil,
    pNewbieMuteUntil,
    pJailSeconds,
    pJailReason[96],
    pVipLevel,
    pVipUntil,
    pGender,
    pAge,
    pSkin,
    pLevel,
    pRespect,
    pCash,
    pBank,
    Float:pPosX,
    Float:pPosY,
    Float:pPosZ,
    Float:pPosA,
    pInterior,
    pVirtualWorld,
    Float:pHealth,
    Float:pArmour,
    pPlayedSeconds,

    // Runtime only, never saved
    pLastLoginText[24],
    pLastIp[16],
    E_ACCOUNT_STATE:pState,
    bool:pAwaitingBcrypt,
    bool:pOldPasswordVerified,
    bool:pFirstSpawn,
    pLoginAttempts,
    pLoginTimer,
    pSessionStart,
    pLastDamage
}

new g_Player[MAX_PLAYERS][E_PLAYER_DATA];

// Bumped on every connect/disconnect so async DB callbacks for a previous player in the same slot are ignored.
// Kept outside g_Player because that record gets wiped on connect.
new g_AccountRace[MAX_PLAYERS];

// Plain password kept only between the "password" and "confirm" dialogs, wiped right after hashing.
new g_PendingPassword[MAX_PLAYERS][PASSWORD_MAX_LENGTH + 1];

#define GENDER_MALE     (0)
#define GENDER_FEMALE   (1)

new g_AutosaveTimer;
