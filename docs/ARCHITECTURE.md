# ShotDetect Architecture

## Purpose

ShotDetect is a shooting-intent detector for Pawn. Its contract is to report whether an on-foot player is holding `KEY_FIRE` with a recognized firearm equipped. The project does not attempt to replace damage validation or model the entire client state.

## State machine

Each player owns only three internal values: an activity flag, the last recognized weapon, and a reconciliation counter. All transitions are centralized in `SD_SetShooting`, keeping those values synchronized.

| Input | Validation | Transition |
| --- | --- | --- |
| `KEY_FIRE` pressed | Player is on foot and the weapon is valid | `inactive -> active`, registering the weapon. |
| `KEY_FIRE` released | Release event | `active -> inactive`. |
| Invalid weapon | Checked on the active update path | Any active state is cleared. |
| Vehicle/death/disconnect transition | Lifecycle callback | State is cleared immediately. |
| Return to `ONFOOT` | One-time capture of current input | State becomes active if `KEY_FIRE` is already held. |
| Firearm swap | Current weapon read on the active path | State remains active and the weapon is updated. |

## Hot path

`OnPlayerUpdate` is the highest-volume callback. The library performs a cheap flag read and exits for inactive players. Only active players call `GetPlayerWeapon`; `GetPlayerKeys` and the additional state validation run according to `SD_KEY_SYNC_EVERY`.

| Mode | Inactive player | Active player |
| --- | --- | --- |
| `SD_KEY_SYNC_EVERY (0)` | Flag read and exit | Weapon read per update, no key reconciliation. |
| `SD_KEY_SYNC_EVERY (1)` | Flag read and exit | Weapon, state, and key validation per update. |
| `SD_KEY_SYNC_EVERY (3)` | Flag read and exit | Weapon per update; state and key validation every three updates. |

Primary detection is driven by `OnPlayerKeyStateChange`. Periodic reconciliation exists to correct missed events, context changes, or divergence between callbacks and current state. This design avoids fully scanning every player with expensive native calls.

## Callback integration

There are two composition modes. When `y_hooks` has already been loaded, the library uses `hook` and does not create ALS caches. Without YSI, it installs ALS for the supported callbacks and resolves the callback chain once on the first `OnPlayerConnect`. Return values from later callbacks are preserved by dynamic forwarding.

The library does not require YSI. This keeps dependencies and runtime cost minimal for both open.mp projects and traditional SA-MP projects without preventing hook composition in projects that already adopt YSI.

## Compatibility

The package is open.mp-first and targets open.mp `v1.5.8.3079` through the `openmp` sampctl preset and the official `openmultiplayer/omp-stdlib` dependency. The include accepts `<open.mp>` first and falls back to `<a_samp>` for traditional SA-MP projects. For older SA-MP headers, it provides neutral aliases for tags that may not exist. With open.mp, official tags are preserved. The public package keeps `ShotDetect.inc` at the root because sampctl adds a dependency root to the include path when `include_path` is not specified.

The open.mp `a_samp.inc` wrapper is not treated as the legacy compatibility test. CI uses `pawn-lang/samp-stdlib@master` in a separate `samp`-preset package so the fallback path is compiled against the traditional SA-MP header set.

## Testing

`tests/smoke.pwn` validates the open.mp package installation path with an explicit `<open.mp>` include and exactly `#include <ShotDetect>`. `tests/smoke_samp.pwn` validates the traditional `<a_samp>` path. The CI workflow recompiles both smoke tests in separate environments. These tests detect layout, manifest, dependency, include-guard, and compilation regressions; behavior with a real connected client must still be tested on a server.
