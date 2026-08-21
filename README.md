# SAMP-ShotDetect

[![CI](https://github.com/BitSain/SAMP-ShotDetect/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/BitSain/SAMP-ShotDetect/actions/workflows/ci.yml)
[![sampctl](https://img.shields.io/badge/sampctl-compatible-2f2f2f.svg?logo=github)](https://github.com/Southclaws/sampctl)
[![Version](https://img.shields.io/badge/version-2.1.0-2563eb.svg)](CHANGELOG.md)
[![GitHub release](https://img.shields.io/github/v/release/BitSain/SAMP-ShotDetect?display_name=tag&sort=semver)](https://github.com/BitSain/SAMP-ShotDetect/releases)
[![GitHub stars](https://img.shields.io/github/stars/BitSain/SAMP-ShotDetect?style=flat)](https://github.com/BitSain/SAMP-ShotDetect/stargazers)
[![GitHub issues](https://img.shields.io/github/issues/BitSain/SAMP-ShotDetect?style=flat)](https://github.com/BitSain/SAMP-ShotDetect/issues)
[![Last commit](https://img.shields.io/github/last-commit/BitSain/SAMP-ShotDetect?style=flat)](https://github.com/BitSain/SAMP-ShotDetect/commits/main)

A small, focused Pawn include for detecting **shooting intent** in SA-MP and open.mp. The library combines `KEY_FIRE`, on-foot state, and a recognized firearm while keeping the hot path lightweight and per-player state consistent.

> **Important definition:** ShotDetect detects shooting intent. It does not assert that a projectile was created, damage was dealt, or the client is legitimate. For physical shot confirmation, use an additional layer based on the weapon events available in your server environment.

## Contents

- [Installation](#installation)
- [Quick start](#quick-start)
- [Public API](#public-api)
- [Performance configuration](#performance-configuration)
- [ALS and YSI integration](#als-and-ysi-integration)
- [Compatibility](#compatibility)
- [Project structure](#project-structure)
- [Testing](#testing)
- [Documentation](#documentation)
- [Contributing](#contributing)

## Installation

Install the dependency directly in your Pawn project:

```bash
sampctl install BitSain/SAMP-ShotDetect
```

Then include the public library name:

```pawn
#include <ShotDetect>
```

The public `ShotDetect.inc` file is kept at the package root. This is the recommended convention for sampctl include-only libraries: the dependency root is added to the include path, so consumers do not need to configure `include_path` manually.

If your project does not use sampctl yet, read the [official installation guide](https://github.com/Southclaws/sampctl/blob/master/docs/install.md). For public libraries, the recommended layout is described in the [Library Creator Guide](https://github.com/Southclaws/sampctl/blob/master/docs/library-creator-guide.md).

## Quick start

```pawn
#include <ShotDetect>

public OnPlayerUpdate(playerid)
{
    if (IsPlayerShooting(playerid))
    {
        new weaponid = SD_GetPlayerShootingWeapon(playerid);
        printf("[ShotDetect] playerid=%d weaponid=%d", playerid, weaponid);
    }
    return 1;
}
```

The include owns its internal state. Do not create a custom timer to poll the function or maintain a second weapon table in your gamemode.

## Public API

| Function or constant | Purpose |
| --- | --- |
| `IsPlayerShooting(playerid)` | Checks whether the player is in the active state. |
| `SD_GetPlayerShootingWeapon(playerid)` | Returns the weapon registered in the active state or `0`. |
| `SD_IsFirearm(weaponid)` | Validates a weapon ID. |
| `SD_GetWeaponClass(weaponid)` | Converts a weapon ID into a ShotDetect class. |
| `SD_WEAPON_CLASS_HANDGUN` | Handgun class, IDs 22–24. |
| `SD_WEAPON_CLASS_SHOTGUN` | Shotgun class, IDs 25–27. |
| `SD_WEAPON_CLASS_SMG` | Submachine-gun class, IDs 28–32. |
| `SD_WEAPON_CLASS_RIFLE` | Rifle class, IDs 33–34. |
| `SD_WEAPON_CLASS_HEAVY` | Heavy class, including minigun 38. |

The complete reference is available in [`docs/API.md`](docs/API.md).

## Performance configuration

The default value of `SD_KEY_SYNC_EVERY` is `3`. Primary detection is event-driven; periodic synchronization is used as reconciliation only for players already marked as active.

```pawn
#define SD_KEY_SYNC_EVERY (3)
#include <ShotDetect>
```

| Value | Behavior | Recommended use |
| ---: | --- | --- |
| `0` | Fully event-driven key handling. | Lowest possible cost; relies on the key callbacks. |
| `1` | Reconciles weapon, state, and keys on every active update. | Strictest reconciliation. |
| `3` | Reconciles every three active updates. | Balanced default. |

The implementation does not call `GetPlayerKeys` for inactive players. Review the [open.mp `OnPlayerUpdate` documentation](https://open.mp/docs/scripting/callbacks/OnPlayerUpdate) when evaluating logic executed in that callback.

## ALS and YSI integration

Without additional configuration, the library uses ALS and forwards existing callbacks. Projects that already use YSI can load `y_hooks` before ShotDetect:

```pawn
#include <YSI_Coding/y_hooks>
#include <ShotDetect>
```

To force ALS even when YSI is available:

```pawn
#define SD_NO_Y_HOOKS
#include <ShotDetect>
```

To receive a start/stop transition without polling, enable the optional callback:

```pawn
#define SD_ENABLE_STATE_CALLBACK
#include <ShotDetect>

public OnPlayerShootingStateChange(playerid, bool:shooting, weaponid)
{
    if (shooting)
    {
        // State start; weaponid is the registered weapon.
    }
    else
    {
        // State stop; weaponid is the last registered weapon.
    }
    return 1;
}
```

This callback is disabled by default to keep the public surface and runtime cost minimal.

## Compatibility

| Environment | Base include | Composition | Status |
| --- | --- | --- | --- |
| SA-MP | `<a_samp>` | ALS or optional YSI | Supported. |
| open.mp | `<open.mp>` | ALS or optional YSI | Supported. |
| sampctl project | `#include <ShotDetect>` | GitHub dependency | Supported. |

The include uses bitmasks for `newkeys` and `oldkeys`, preserving combinations such as fire + crouch or fire + aim. `OnPlayerStateChange` invalidates the state when the player changes context, and a one-time capture when returning to `PLAYER_STATE_ONFOOT` avoids relying on a new physical key press.

## Project structure

```text
.
├── ShotDetect.inc                 # public include installed by sampctl
├── pawn.json                      # package manifest and smoke-test entry point
├── tests/
│   └── smoke.pwn                  # package installation and compilation check
├── examples/
│   └── basic.pwn                  # minimal API integration example
├── docs/
│   ├── API.md                     # functions and constants reference
│   ├── ARCHITECTURE.md            # state machine and hot path
│   ├── MIGRATION.md               # migration from previous versions
│   └── RELEASING.md               # release process
├── .github/
│   ├── ISSUE_TEMPLATE/            # issue templates
│   ├── pull_request_template.md   # pull-request checklist
│   └── workflows/ci.yml           # reproducible sampctl build
├── CHANGELOG.md
├── CONTRIBUTING.md
└── README.md
```

Dependencies, lockfiles, runtimes, binaries, logs, and compiler output are local artifacts and remain outside commits through `.gitignore`.

## Testing

To validate the package locally:

```bash
sampctl build --bare --no-lock
```

The smoke test must compile without warnings or errors, and the public include must resolve exactly as `#include <ShotDetect>`. The same process runs automatically in the [CI workflow](.github/workflows/ci.yml).

To run the example in a real gamemode, copy the content of [`examples/basic.pwn`](examples/basic.pwn) into your callback flow. The smoke test does not replace validation with a connected client; it verifies layout, manifest, dependencies, include guards, and compilation.

## Documentation

- [Public API](docs/API.md)
- [Architecture and performance](docs/ARCHITECTURE.md)
- [Migration guide](docs/MIGRATION.md)
- [Release process](docs/RELEASING.md)
- [Changelog](CHANGELOG.md)
- [Contribution guide](CONTRIBUTING.md)
- [`OnPlayerKeyStateChange` documentation](https://open.mp/docs/scripting/callbacks/OnPlayerKeyStateChange)
- [`GetPlayerKeys` documentation](https://open.mp/docs/scripting/functions/GetPlayerKeys)
- [`OnPlayerWeaponShot` limitations](https://open.mp/docs/scripting/callbacks/OnPlayerWeaponShot)

## Contributing

Issues and pull requests are welcome. Before proposing a change, read [`CONTRIBUTING.md`](CONTRIBUTING.md), describe the hot-path impact, and run the smoke test. The existing API should remain compatible; breaking changes must be documented as a major version.

The project was created and is maintained by [BitSain](https://github.com/BitSain/). See the history for authorship and implementation evolution.
