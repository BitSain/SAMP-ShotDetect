# Migration Guide

## From the previous version to 2.1.0

The main call remains compatible:

```pawn
if (IsPlayerShooting(playerid))
{
    // The player is in shooting-intent state.
}
```

The most important structural change is the public file path. The library should now be installed through sampctl and included as `#include <ShotDetect>`. Do not copy the file into `pawno/include` or use `#include "src/ShotDetect.inc"`.

## Install through sampctl

In the consuming project, remove an old manual copy of `ShotDetect.inc` to avoid filename conflicts and run:

```bash
sampctl install BitSain/SAMP-ShotDetect
```

Then update the source to:

```pawn
#include <ShotDetect>
```

The managed package should be the only source of the library. When two dependencies provide the same `.inc` filename, the compiler may select the wrong file depending on include-path order.

## New functions

The current version adds:

```pawn
SD_GetPlayerShootingWeapon(playerid);
SD_IsFirearm(weaponid);
SD_GetWeaponClass(weaponid);
```

These functions allow the gamemode to distinguish weapons and weapon classes without duplicating the validation table internally.

## New optional callback

Projects that need to react to state transitions can enable:

```pawn
#define SD_ENABLE_STATE_CALLBACK
#include <ShotDetect>
```

Then implement `OnPlayerShootingStateChange`. The callback is not required for basic usage and remains disabled by default.

## Performance

The recommended configuration is `SD_KEY_SYNC_EVERY (3)`. Projects that require more aggressive reconciliation can use `1`; projects that prioritize the lowest number of native calls can use `0`, accepting stronger reliance on key events.

```pawn
#define SD_KEY_SYNC_EVERY (3)
#include <ShotDetect>
```

Do not create custom timers to query `IsPlayerShooting`. The include already maintains state through events and provides configurable reconciliation.

## Lifecycle changes

State is now cleared on spawn, death, disconnect, and state changes. When returning to on-foot state, the library performs a one-time input capture to handle `KEY_FIRE` already being held. The gamemode does not need to call a manual reset function.

## YSI and ALS

If the project already uses YSI, include `y_hooks` before ShotDetect:

```pawn
#include <YSI_Coding/y_hooks>
#include <ShotDetect>
```

Otherwise, the library uses ALS automatically. To force the no-YSI path:

```pawn
#define SD_NO_Y_HOOKS
#include <ShotDetect>
```
