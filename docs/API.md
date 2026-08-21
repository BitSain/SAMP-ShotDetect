# ShotDetect API

ShotDetect exposes a small, stable Pawn API for checking a player's shooting-intent state. Load the public include as follows:

```pawn
#include <ShotDetect>
```

## Public functions

| Function | Return value | Description |
| --- | --- | --- |
| `IsPlayerShooting(playerid)` | `bool:` | Reports whether the player is marked as shooting. Invalid player IDs return `false`. |
| `SD_GetPlayerShootingWeapon(playerid)` | `weaponid` | Returns the weapon registered in the active state or `0` when no shooting state exists. |
| `SD_IsFirearm(weaponid)` | `bool:` | Checks whether the weapon ID belongs to the recognized firearm family. |
| `SD_GetWeaponClass(weaponid)` | `classid` | Converts a weapon ID into a ShotDetect class. |

## Weapon classes

The implementation uses constants and ranges instead of a global presence table.

| Constant | Value | Weapons |
| --- | ---: | --- |
| `SD_WEAPON_CLASS_NONE` | `0` | Unrecognized ID. |
| `SD_WEAPON_CLASS_HANDGUN` | `1` | 22–24. |
| `SD_WEAPON_CLASS_SHOTGUN` | `2` | 25–27. |
| `SD_WEAPON_CLASS_SMG` | `3` | 28–32. |
| `SD_WEAPON_CLASS_RIFLE` | `4` | 33–34. |
| `SD_WEAPON_CLASS_HEAVY` | `5` | 38, minigun. |

## Example

```pawn
#include <ShotDetect>

public OnPlayerUpdate(playerid)
{
    if (IsPlayerShooting(playerid))
    {
        new weaponid = SD_GetPlayerShootingWeapon(playerid);
        new classid = SD_GetWeaponClass(weaponid);

        if (classid == SD_WEAPON_CLASS_RIFLE)
        {
            // Apply a gamemode-specific rule.
        }
    }
    return 1;
}
```

## Optional state-transition callback

The additional callback is not compiled by default. Enable it before including ShotDetect:

```pawn
#define SD_ENABLE_STATE_CALLBACK
#include <ShotDetect>

public OnPlayerShootingStateChange(playerid, bool:shooting, weaponid)
{
    if (shooting)
    {
        // State start; weaponid is the weapon that started detection.
    }
    else
    {
        // State stop; weaponid is the last registered weapon.
    }
    return 1;
}
```

The event fires only when the state flag changes. The library does not use this callback as a polling mechanism.

## Synchronization configuration

Define the policy before including the library:

```pawn
#define SD_KEY_SYNC_EVERY (3)
#include <ShotDetect>
```

The default value `3` periodically reconciles active players. `1` validates every active update. `0` relies exclusively on key events and removes periodic `GetPlayerKeys` reads.

## Semantic limits

ShotDetect identifies shooting intent using `KEY_FIRE`, a firearm, and on-foot state. It does not prove that a projectile was created, that damage was dealt, or that the client is legitimate. For physical confirmation, consider an additional layer based on the weapon events available in the server environment, while respecting the limitations documented by [open.mp](https://open.mp/docs/scripting/callbacks/OnPlayerWeaponShot).
