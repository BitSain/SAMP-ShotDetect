# Release Process

ShotDetect follows semantic versioning. The version number must remain consistent across `ShotDetect.inc`, `README.md`, and `CHANGELOG.md`.

## Checklist

Update the version wherever it is exposed, record the changes in the changelog, run the local build, and confirm that CI passes on the main branch. The public include and `#include <ShotDetect>` must not change in a compatible release.

```bash
sampctl build --bare --no-lock
git diff --check
git status
```

Then create an annotated tag and publish the release:

```bash
git tag -a v2.1.0 -m "Release v2.1.0"
git push origin main --follow-tags
```

Tags must follow the `vMAJOR.MINOR.PATCH` format. Breaking changes to the public API, callbacks, or include name require a `MAJOR` increment. Compatible additions use `MINOR`; internal fixes without contract changes use `PATCH`.

## sampctl publishing

The published package must contain `ShotDetect.inc` at the root, `pawn.json`, documentation, examples, and the smoke test. Do not publish `dependencies/`, `pawn.lock`, binaries, runtimes, `.amx` files, or logs.

Before announcing a release, test it from a clean consumer project:

```bash
sampctl install BitSain/SAMP-ShotDetect:v2.1.0
```

Then confirm that the consumer compiles:

```pawn
#include <ShotDetect>
```
