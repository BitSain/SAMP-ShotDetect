/*
    ShotDetect (open.mp first, SA-MP compatible)
    Version: 2.2.0
    Updated: 21/08/2026
    Primary target: open.mp v1.5.8.3079
    Compatibility target: SA-MP 0.3.7
    Author: BitSain (https://github.com/BitSain/)

    Detects on-foot fire intent: KEY_FIRE held with a firearm equipped.

    Default validation policy:
    - Event-driven state changes through OnPlayerKeyStateChange.
    - Weapon validation on the active-player hot path.
    - KEY_FIRE resynchronization every SD_KEY_SYNC_EVERY updates.

    Configuration before #include <ShotDetect>:
        #define SD_KEY_SYNC_EVERY (3) // balanced default
        #define SD_KEY_SYNC_EVERY (1) // strict synchronization
        #define SD_KEY_SYNC_EVERY (0) // fastest event-driven mode
        #define SD_NO_Y_HOOKS            // force ALS
        #define SD_ENABLE_STATE_CALLBACK  // enable OnPlayerShootingStateChange
        #define SD_DEBUG_MODE             // server-console diagnostics
*/

#if defined _INC_ShotDetect
    #endinput
#endif
#define _INC_ShotDetect

// -----------------------------------------------------------------------------
// Base include and tag compatibility
// open.mp is intentionally attempted first. The legacy SA-MP header remains a
// supported fallback for projects that compile against the traditional stdlib.
// -----------------------------------------------------------------------------

#if !defined _INC_open_mp && !defined _INC_a_samp
    #tryinclude <open.mp>
#endif
#if !defined _INC_open_mp && !defined _INC_a_samp
    #tryinclude <a_samp>
#endif
#if !defined _INC_open_mp && !defined _INC_a_samp
    #error "Please include <open.mp> or <a_samp> before <ShotDetect>."
#endif

#if !defined _INC_open_mp
    #if defined KEY
        #undef KEY
    #endif
    #define KEY: _:

    #if defined PLAYER_STATE
        #undef PLAYER_STATE
    #endif
    #define PLAYER_STATE: _:

    #if defined WEAPON
        #undef WEAPON
    #endif
    #define WEAPON: _:
#endif

#define SD_VERSION "2.2.0"

// -----------------------------------------------------------------------------
// Configuration
// -----------------------------------------------------------------------------

#if !defined SD_KEY_SYNC_EVERY
    #define SD_KEY_SYNC_EVERY (3)
#endif

#if SD_KEY_SYNC_EVERY < 0
    #error "SD_KEY_SYNC_EVERY cannot be negative. Use 0 for event-driven mode."
#endif

#define SD_WEAPON_FIRST_FIREARM (22)
#define SD_WEAPON_LAST_FIREARM  (34)
#define SD_WEAPON_MINIGUN       (38)

#define SD_WEAPON_CLASS_NONE     (0)
#define SD_WEAPON_CLASS_HANDGUN  (1)
#define SD_WEAPON_CLASS_SHOTGUN  (2)
#define SD_WEAPON_CLASS_SMG      (3)
#define SD_WEAPON_CLASS_RIFLE    (4)
#define SD_WEAPON_CLASS_HEAVY    (5)

// -----------------------------------------------------------------------------
// Callback composition
// -----------------------------------------------------------------------------

#if !defined SD_NO_Y_HOOKS && defined _INC_y_hooks
    #define SD_USING_Y_HOOKS
    #define SD_CALLBACK hook
#else
    #define SD_CALLBACK public
#endif

// -----------------------------------------------------------------------------
// Per-player state and callback-chain cache
// -----------------------------------------------------------------------------

static bool:SD_PlayerShooting[MAX_PLAYERS] = {false, ...};
static SD_PlayerShootingWeapon[MAX_PLAYERS] = {0, ...};
static SD_PlayerSyncCounter[MAX_PLAYERS] = {0, ...};

#if !defined SD_USING_Y_HOOKS
    static bool:SD_CallbacksChecked = false;
    static bool:SD_HasOnPlayerConnect = false;
    static bool:SD_HasOnPlayerDisconnect = false;
    static bool:SD_HasOnPlayerSpawn = false;
    static bool:SD_HasOnPlayerDeath = false;
    static bool:SD_HasOnPlayerStateChange = false;
    static bool:SD_HasOnPlayerKeyStateChange = false;
    static bool:SD_HasOnPlayerUpdate = false;
#endif
#if defined SD_ENABLE_STATE_CALLBACK
    static bool:SD_HasOnShootStateChange = false;
    static bool:SD_StateCbChecked = false;
#endif

// -----------------------------------------------------------------------------
// Weapon and state core
// -----------------------------------------------------------------------------

stock bool:SD_IsFirearm(weaponid)
{
    return (weaponid >= SD_WEAPON_FIRST_FIREARM && weaponid <= SD_WEAPON_LAST_FIREARM) || weaponid == SD_WEAPON_MINIGUN;
}

stock SD_GetWeaponClass(weaponid)
{
    switch (weaponid)
    {
        case 22 .. 24: return SD_WEAPON_CLASS_HANDGUN;
        case 25 .. 27: return SD_WEAPON_CLASS_SHOTGUN;
        case 28 .. 32: return SD_WEAPON_CLASS_SMG;
        case 33 .. 34: return SD_WEAPON_CLASS_RIFLE;
        case 38: return SD_WEAPON_CLASS_HEAVY;
    }
    return SD_WEAPON_CLASS_NONE;
}

static SD_ClearPlayer(playerid)
{
    SD_PlayerShooting[playerid] = false;
    SD_PlayerShootingWeapon[playerid] = 0;
    SD_PlayerSyncCounter[playerid] = 0;
}

static SD_SetShooting(playerid, bool:shooting, weaponid)
{
    new bool:changed = (SD_PlayerShooting[playerid] != shooting);
    #if defined SD_ENABLE_STATE_CALLBACK || defined SD_DEBUG_MODE
        new previous_weapon = SD_PlayerShootingWeapon[playerid];
    #endif

    SD_PlayerShooting[playerid] = shooting;
    SD_PlayerShootingWeapon[playerid] = shooting ? weaponid : 0;
    SD_PlayerSyncCounter[playerid] = 0;

    if (changed)
    {
        #if defined SD_DEBUG_MODE
            printf("[ShotDetect] player %d: %s weapon=%d", playerid, shooting ? "START" : "STOP", shooting ? weaponid : previous_weapon);
        #endif

        #if defined SD_ENABLE_STATE_CALLBACK
            if (SD_HasOnShootStateChange)
            {
                CallLocalFunction("OnPlayerShootingStateChange", "ddd", playerid, shooting, shooting ? weaponid : previous_weapon);
            }
        #endif
    }
}

static SD_ResetPlayer(playerid)
{
    SD_SetShooting(playerid, false, 0);
}

static SD_StartPlayerShooting(playerid, weaponid)
{
    SD_SetShooting(playerid, true, weaponid);
}

static SD_CaptureCurrentInput(playerid, bool:already_onfoot)
{
    if (!already_onfoot && GetPlayerState(playerid) != PLAYER_STATE_ONFOOT)
    {
        return;
    }

    new weaponid = GetPlayerWeapon(playerid);
    if (!SD_IsFirearm(weaponid))
    {
        return;
    }

    new KEY:keys, updownkeys, leftrightkeys;
    GetPlayerKeys(playerid, keys, updownkeys, leftrightkeys);
    if (keys & KEY_FIRE)
    {
        SD_StartPlayerShooting(playerid, weaponid);
    }
}

static SD_SyncPlayer(playerid)
{
    new weaponid = GetPlayerWeapon(playerid);
    if (!SD_IsFirearm(weaponid))
    {
        SD_ResetPlayer(playerid);
        return;
    }

    // A weapon swap while KEY_FIRE remains held is still a valid active state.
    SD_PlayerShootingWeapon[playerid] = weaponid;

    #if SD_KEY_SYNC_EVERY > 0
        if (++SD_PlayerSyncCounter[playerid] < SD_KEY_SYNC_EVERY)
        {
            return;
        }
        SD_PlayerSyncCounter[playerid] = 0;

        if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT)
        {
            SD_ResetPlayer(playerid);
            return;
        }

        new KEY:keys, updownkeys, leftrightkeys;
        GetPlayerKeys(playerid, keys, updownkeys, leftrightkeys);
        if (!(keys & KEY_FIRE))
        {
            SD_ResetPlayer(playerid);
        }
    #endif
}

static SD_HandleKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys)
{
    if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT)
    {
        SD_ResetPlayer(playerid);
        return;
    }

    new fire_now = (_:(newkeys & KEY_FIRE) != 0);
    new fire_before = (_:(oldkeys & KEY_FIRE) != 0);

    if (fire_now && !fire_before)
    {
        new weaponid = GetPlayerWeapon(playerid);
        if (SD_IsFirearm(weaponid))
        {
            SD_StartPlayerShooting(playerid, weaponid);
        }
        else
        {
            SD_ResetPlayer(playerid);
        }
    }
    else if (!fire_now && fire_before)
    {
        SD_ResetPlayer(playerid);

        #if defined SD_DEBUG_MODE
            printf("[ShotDetect] player %d: STOP", playerid);
        #endif
    }
}

// -----------------------------------------------------------------------------
// Public API
// -----------------------------------------------------------------------------

stock bool:IsPlayerShooting(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS)
    {
        return false;
    }
    return SD_PlayerShooting[playerid];
}

stock SD_GetPlayerShootingWeapon(playerid)
{
    if (playerid < 0 || playerid >= MAX_PLAYERS || !SD_PlayerShooting[playerid])
    {
        return 0;
    }
    return SD_PlayerShootingWeapon[playerid];
}

// -----------------------------------------------------------------------------
// Callback-chain discovery
// -----------------------------------------------------------------------------

static SD_ResolveCallbacks()
{
    #if defined SD_ENABLE_STATE_CALLBACK
        if (!SD_StateCbChecked)
        {
            SD_HasOnShootStateChange = (funcidx("OnPlayerShootingStateChange") != -1);
            SD_StateCbChecked = true;
        }
    #endif

    #if !defined SD_USING_Y_HOOKS
        if (!SD_CallbacksChecked)
        {
            SD_HasOnPlayerConnect = (funcidx("SD_OnPlayerConnect") != -1);
            SD_HasOnPlayerDisconnect = (funcidx("SD_OnPlayerDisconnect") != -1);
            SD_HasOnPlayerSpawn = (funcidx("SD_OnPlayerSpawn") != -1);
            SD_HasOnPlayerDeath = (funcidx("SD_OnPlayerDeath") != -1);
            SD_HasOnPlayerStateChange = (funcidx("SD_OnPlayerStateChange") != -1);
            SD_HasOnPlayerKeyStateChange = (funcidx("SD_OnPlayerKeyStateChange") != -1);
            SD_HasOnPlayerUpdate = (funcidx("SD_OnPlayerUpdate") != -1);
            SD_CallbacksChecked = true;
        }
    #endif
}

// -----------------------------------------------------------------------------
// Callbacks
// -----------------------------------------------------------------------------

#if defined SD_USING_Y_HOOKS
    forward OnPlayerConnect(playerid);
    forward OnPlayerDisconnect(playerid, reason);
    forward OnPlayerSpawn(playerid);
    forward OnPlayerDeath(playerid, killerid, WEAPON:reason);
    forward OnPlayerStateChange(playerid, PLAYER_STATE:newstate, PLAYER_STATE:oldstate);
    forward OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys);
    forward OnPlayerUpdate(playerid);
#endif

SD_CALLBACK OnPlayerConnect(playerid)
{
    SD_ClearPlayer(playerid);
    SD_ResolveCallbacks();

    #if !defined SD_USING_Y_HOOKS
        if (SD_HasOnPlayerConnect)
        {
            return CallLocalFunction("SD_OnPlayerConnect", "d", playerid);
        }
    #endif
    return 1;
}

SD_CALLBACK OnPlayerDisconnect(playerid, reason)
{
    SD_ClearPlayer(playerid);

    #if !defined SD_USING_Y_HOOKS
        if (SD_HasOnPlayerDisconnect)
        {
            return CallLocalFunction("SD_OnPlayerDisconnect", "dd", playerid, reason);
        }
    #endif
    return 1;
}

SD_CALLBACK OnPlayerSpawn(playerid)
{
    SD_ClearPlayer(playerid);
    SD_CaptureCurrentInput(playerid, false);

    #if !defined SD_USING_Y_HOOKS
        if (SD_HasOnPlayerSpawn)
        {
            return CallLocalFunction("SD_OnPlayerSpawn", "d", playerid);
        }
    #endif
    return 1;
}

SD_CALLBACK OnPlayerDeath(playerid, killerid, WEAPON:reason)
{
    SD_ResetPlayer(playerid);

    #if !defined SD_USING_Y_HOOKS
        if (SD_HasOnPlayerDeath)
        {
            return CallLocalFunction("SD_OnPlayerDeath", "ddd", playerid, killerid, reason);
        }
    #endif
    return 1;
}

SD_CALLBACK OnPlayerStateChange(playerid, PLAYER_STATE:newstate, PLAYER_STATE:oldstate)
{
    SD_ResetPlayer(playerid);
    if (newstate == PLAYER_STATE_ONFOOT)
    {
        SD_CaptureCurrentInput(playerid, true);
    }

    #if !defined SD_USING_Y_HOOKS
        if (SD_HasOnPlayerStateChange)
        {
            return CallLocalFunction("SD_OnPlayerStateChange", "ddd", playerid, newstate, oldstate);
        }
    #endif
    return 1;
}

SD_CALLBACK OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys)
{
    SD_HandleKeyStateChange(playerid, newkeys, oldkeys);

    #if !defined SD_USING_Y_HOOKS
        if (SD_HasOnPlayerKeyStateChange)
        {
            return CallLocalFunction("SD_OnPlayerKeyStateChange", "ddd", playerid, newkeys, oldkeys);
        }
    #endif
    return 1;
}

SD_CALLBACK OnPlayerUpdate(playerid)
{
    if (SD_PlayerShooting[playerid])
    {
        SD_SyncPlayer(playerid);
    }

    #if !defined SD_USING_Y_HOOKS
        if (SD_HasOnPlayerUpdate)
        {
            return CallLocalFunction("SD_OnPlayerUpdate", "d", playerid);
        }
    #endif
    return 1;
}

// -----------------------------------------------------------------------------
// ALS fallback
// -----------------------------------------------------------------------------

#if defined SD_ENABLE_STATE_CALLBACK
    forward OnPlayerShootingStateChange(playerid, bool:shooting, weaponid);
#endif

#if !defined SD_USING_Y_HOOKS

    forward OnPlayerConnect(playerid);
    forward OnPlayerDisconnect(playerid, reason);
    forward OnPlayerSpawn(playerid);
    forward OnPlayerDeath(playerid, killerid, WEAPON:reason);
    forward OnPlayerStateChange(playerid, PLAYER_STATE:newstate, PLAYER_STATE:oldstate);
    forward OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys);
    forward OnPlayerUpdate(playerid);

    forward SD_OnPlayerConnect(playerid);
    forward SD_OnPlayerDisconnect(playerid, reason);
    forward SD_OnPlayerSpawn(playerid);
    forward SD_OnPlayerDeath(playerid, killerid, WEAPON:reason);
    forward SD_OnPlayerStateChange(playerid, PLAYER_STATE:newstate, PLAYER_STATE:oldstate);
    forward SD_OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys);
    forward SD_OnPlayerUpdate(playerid);

    #if defined _ALS_OnPlayerConnect
        #undef OnPlayerConnect
    #else
        #define _ALS_OnPlayerConnect
    #endif
    #define OnPlayerConnect SD_OnPlayerConnect

    #if defined _ALS_OnPlayerDisconnect
        #undef OnPlayerDisconnect
    #else
        #define _ALS_OnPlayerDisconnect
    #endif
    #define OnPlayerDisconnect SD_OnPlayerDisconnect

    #if defined _ALS_OnPlayerSpawn
        #undef OnPlayerSpawn
    #else
        #define _ALS_OnPlayerSpawn
    #endif
    #define OnPlayerSpawn SD_OnPlayerSpawn

    #if defined _ALS_OnPlayerDeath
        #undef OnPlayerDeath
    #else
        #define _ALS_OnPlayerDeath
    #endif
    #define OnPlayerDeath SD_OnPlayerDeath

    #if defined _ALS_OnPlayerStateChange
        #undef OnPlayerStateChange
    #else
        #define _ALS_OnPlayerStateChange
    #endif
    #define OnPlayerStateChange SD_OnPlayerStateChange

    #if defined _ALS_OnPlayerKeyStateChange
        #undef OnPlayerKeyStateChange
    #else
        #define _ALS_OnPlayerKeyStateChange
    #endif
    #define OnPlayerKeyStateChange SD_OnPlayerKeyStateChange

    #if defined _ALS_OnPlayerUpdate
        #undef OnPlayerUpdate
    #else
        #define _ALS_OnPlayerUpdate
    #endif
    #define OnPlayerUpdate SD_OnPlayerUpdate

#endif

#undef SD_CALLBACK
#if defined SD_USING_Y_HOOKS
    #undef SD_USING_Y_HOOKS
#endif

// EOF.
