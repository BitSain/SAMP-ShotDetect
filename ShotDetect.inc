/*
    Include name: ShotDetect (SAMP/open.mp)
    Version: 2.0.0
    Last update: 20/08/2026
    Created by BitSain (https://github.com/BitSain/)

    Detects whether an on-foot player is currently holding KEY_FIRE
    while a firearm is equipped.

    Compatibility:
    - open.mp
    - SA-MP
    - YSI y_hooks (when included before ShotDetect)
    - ALS fallback (define SD_NO_Y_HOOKS for explicit fallback)

    Usage:
        if (IsPlayerShooting(playerid))
        {
            // The player is holding the fire key with a firearm equipped.
        }

    Optional debug:
        #define SD_DEBUG_MODE
        #include <ShotDetect>
*/

#if defined _INC_ShotDetect
    #endinput
#endif
#define _INC_ShotDetect

// -----------------------------------------------------------------------------
// Base include compatibility
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
#endif

#define SD_VERSION "2.0.0"

// -----------------------------------------------------------------------------
// Optional callback composition
// -----------------------------------------------------------------------------

#if !defined SD_NO_Y_HOOKS && defined _INC_y_hooks
    #define SD_USING_Y_HOOKS
    #define SD_CALLBACK hook
#else
    #define SD_CALLBACK public
#endif

// -----------------------------------------------------------------------------
// Configuration and weapon rules
// -----------------------------------------------------------------------------

#define SD_WEAPON_FIRST_FIREARM (22)
#define SD_WEAPON_LAST_FIREARM  (34)
#define SD_WEAPON_MINIGUN       (38)

// -----------------------------------------------------------------------------
// Internal state
// -----------------------------------------------------------------------------

static bool:SD_PlayerShooting[MAX_PLAYERS] = {false, ...};
static SD_PlayerShootingWeapon[MAX_PLAYERS] = {0, ...};
static bool:SD_CallbacksChecked = false;
static bool:SD_HasOnPlayerConnect = false;
static bool:SD_HasOnPlayerDisconnect = false;
static bool:SD_HasOnPlayerKeyStateChange = false;
static bool:SD_HasOnPlayerUpdate = false;

// -----------------------------------------------------------------------------
// Internal helpers
// -----------------------------------------------------------------------------

static stock bool:SD_IsFirearm(weaponid)
{
    return (weaponid >= SD_WEAPON_FIRST_FIREARM && weaponid <= SD_WEAPON_LAST_FIREARM) || weaponid == SD_WEAPON_MINIGUN;
}

static SD_ResetPlayer(playerid)
{
    SD_PlayerShooting[playerid] = false;
    SD_PlayerShootingWeapon[playerid] = 0;
}

static SD_SyncPlayer(playerid)
{
    if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT)
    {
        SD_ResetPlayer(playerid);
        return;
    }

    new weaponid = GetPlayerWeapon(playerid);
    if (!SD_IsFirearm(weaponid))
    {
        SD_ResetPlayer(playerid);
        return;
    }

    SD_PlayerShootingWeapon[playerid] = weaponid;

    new KEY:keys, updownkeys, leftrightkeys;
    GetPlayerKeys(playerid, keys, updownkeys, leftrightkeys);

    if (!(keys & KEY_FIRE))
    {
        SD_ResetPlayer(playerid);
    }
}

static SD_ResolveCallbacks()
{
    #if !defined SD_USING_Y_HOOKS
        if (!SD_CallbacksChecked)
        {
            SD_HasOnPlayerConnect = (funcidx("SD_OnPlayerConnect") != -1);
            SD_HasOnPlayerDisconnect = (funcidx("SD_OnPlayerDisconnect") != -1);
            SD_HasOnPlayerKeyStateChange = (funcidx("SD_OnPlayerKeyStateChange") != -1);
            SD_HasOnPlayerUpdate = (funcidx("SD_OnPlayerUpdate") != -1);
            SD_CallbacksChecked = true;
        }
    #endif
}

static SD_HandleKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys)
{
    if (GetPlayerState(playerid) != PLAYER_STATE_ONFOOT)
    {
        if (SD_PlayerShooting[playerid])
        {
            SD_ResetPlayer(playerid);
        }
        return;
    }

    new fire_now = (_:(newkeys & KEY_FIRE) != 0);
    new fire_before = (_:(oldkeys & KEY_FIRE) != 0);

    if (fire_now && !fire_before)
    {
        new weaponid = GetPlayerWeapon(playerid);

        if (SD_IsFirearm(weaponid))
        {
            SD_PlayerShooting[playerid] = true;
            SD_PlayerShootingWeapon[playerid] = weaponid;

            #if defined SD_DEBUG_MODE
                printf("[ShotDetect] player %d started shooting with weapon %d.", playerid, weaponid);
            #endif
        }
        else
        {
            SD_ResetPlayer(playerid);
        }
    }
    else if (!fire_now && fire_before)
    {
        if (SD_PlayerShooting[playerid])
        {
            SD_ResetPlayer(playerid);

            #if defined SD_DEBUG_MODE
                printf("[ShotDetect] player %d stopped shooting.", playerid);
            #endif
        }
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

// -----------------------------------------------------------------------------
// Callbacks
// -----------------------------------------------------------------------------

SD_CALLBACK OnPlayerConnect(playerid)
{
    SD_ResetPlayer(playerid);

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
    SD_ResetPlayer(playerid);

    #if !defined SD_USING_Y_HOOKS
        if (SD_HasOnPlayerDisconnect)
        {
            return CallLocalFunction("SD_OnPlayerDisconnect", "dd", playerid, reason);
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
    // The expensive safety path runs only while this player is marked active.
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
// ALS fallback for servers without YSI y_hooks
// -----------------------------------------------------------------------------

#if !defined SD_USING_Y_HOOKS

    forward OnPlayerConnect(playerid);
    forward OnPlayerDisconnect(playerid, reason);
    forward OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys);
    forward OnPlayerUpdate(playerid);

    forward SD_OnPlayerConnect(playerid);
    forward SD_OnPlayerDisconnect(playerid, reason);
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
