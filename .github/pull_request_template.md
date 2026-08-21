## Summary

Describe what changed and why the change is needed.

## Change type

- [ ] Bug fix
- [ ] Compatible feature
- [ ] Performance change
- [ ] Documentation change
- [ ] Breaking API change

## Compatibility

Explain behavior in SA-MP, open.mp, ALS, and YSI where applicable.

## Performance

Describe the impact on `OnPlayerUpdate`, native calls, per-player memory, and dependencies.

## Validation

- [ ] `sampctl build --bare --no-lock`
- [ ] Smoke test updated when necessary
- [ ] Documentation updated
- [ ] `git diff --check`
- [ ] No duplicate `ShotDetect.inc` copy exists

## Maintainer checklist

- [ ] The existing API was preserved, or the major change is documented.
- [ ] The public path remains `#include <ShotDetect>`.
- [ ] No runtime artifacts, vendored dependencies, or lockfiles were added.
