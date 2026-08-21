---
name: Bug report
about: Report a reproducible ShotDetect problem
title: "[Bug] "
labels: bug
assignees: ""
---

## Description

Explain the observed behavior and the expected behavior as clearly as possible.

## Environment

- ShotDetect version:
- SA-MP or open.mp version:
- Pawn compiler:
- sampctl version:
- Operating system:
- YSI enabled?:
- `SD_KEY_SYNC_EVERY` value:

## Reproduction

Provide a minimal example, including include order and involved callbacks.

```pawn
// minimal reproducible code
```

## Logs

Paste the complete compiler error or relevant server output:

```text
paste here
```

## Checklist

- [ ] I confirmed there is no older `ShotDetect.inc` copy in the include path.
- [ ] I tested with `#include <ShotDetect>`.
- [ ] I specified whether I use ALS or YSI.
- [ ] I provided a minimal reproducible case.
