# Contributing

Thank you for contributing to ShotDetect. This project is a Pawn include for SA-MP and open.mp, so small changes to signatures, tags, or callbacks can affect many gamemodes.

## Before opening a change

Read the [README](README.md), [API reference](docs/API.md), [architecture guide](docs/ARCHITECTURE.md), and [migration guide](docs/MIGRATION.md). Every change must keep `#include <ShotDetect>` working in a clean sampctl project.

## Expected structure

The public include remains at `ShotDetect.inc`. Compilation tests belong in `tests/`, examples belong in `examples/`, and detailed documentation belongs in `docs/`. `pawn.json` must continue to point to `tests/smoke.pwn` as the package entry point.

## Local validation

Run:

```bash
sampctl build --bare --no-lock
```

The smoke test must finish without warnings or errors. When reviewing layout, confirm that the package contains `ShotDetect.inc` at the root and that the README uses `#include <ShotDetect>`.

## Compatibility rules

The existing `IsPlayerShooting(playerid)` API must not be removed without a major-version change. New functions should use the `SD_` prefix, constants should use `SD_`, and optional callbacks must be protected by a configuration macro. The ALS path must continue to work without YSI; YSI hooks are optional integration.

Changes to the state machine should include at least one smoke-test or documented validation scenario. Hot-path changes must explain the expected native-call cost and the reason for the design choice.

## Commits and releases

Use clear commit messages, preferably following Conventional Commits. Public releases should use semantic tags such as `1.2.3` or `v1.2.3` and update `CHANGELOG.md`.

Pull requests should explain the problem, previous behavior, solution, SA-MP/open.mp compatibility, and tests executed.
