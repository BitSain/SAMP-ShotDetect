# Changelog

All notable ShotDetect changes are documented in this file.

The project follows [Semantic Versioning](https://semver.org/).

## [2.2.0] — 2026-08-21

### Added

- Open.mp-first package configuration using the `openmp` preset.
- Official open.mp dependency chain: `openmultiplayer/omp-stdlib`, `pawn-lang/samp-stdlib@open.mp`, and `pawn-lang/pawn-stdlib@open.mp`.
- Explicit open.mp smoke test and separate SA-MP compatibility smoke test.
- CI validation for both open.mp and traditional SA-MP stdlib environments.

### Changed

- Updated the include metadata and public documentation to target open.mp `v1.5.8.3079` as the primary environment.
- Preserved `<a_samp>` fallback support and neutral tag aliases for traditional SA-MP headers.
- Updated installation, migration, architecture, and release guidance for dual-environment support.

## [2.1.0] — 2026-08-20

### Added

- Weapon identification through `SD_GetPlayerShootingWeapon`.
- Weapon classification through `SD_GetWeaponClass` and class constants.
- Optional `OnPlayerShootingStateChange` callback.
- Configurable `SD_KEY_SYNC_EVERY` reconciliation policy.
- Package smoke test in `tests/smoke.pwn`.
- Integration example in `examples/basic.pwn`.
- Separate API, architecture, migration, and release documentation.
- GitHub Actions workflow for sampctl package validation.

### Changed

- Detection now uses key bitmasks and `KEY_FIRE` edge transitions.
- Per-player state is centralized in an explicit state machine.
- State is cleared on spawn, death, disconnect, and state changes.
- The public include was moved to the repository root for direct sampctl installation.
- The manifest was simplified for an include-only library package.
- Automatic `sampctl_build_file.inc` generation was disabled.

### Removed

- The previous weapon-presence table.
- Timers and ammunition counters from the previous implementation.
- The duplicate public include under `src/`.

## [1.1.2]

Previous version available in the repository history.
