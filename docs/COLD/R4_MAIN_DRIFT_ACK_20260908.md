# CSGOVR — R4 MAIN DRIFT ACK 2026-09-08

BRANCH = `cold/r4/sidecar-ownership`
PINNED_BASE = `9cb401e5e8f1ee78cbc4484eb20d244705bd5dad`
CURRENT_MAIN = `943fb84be55e56871769e71505e7b2f1440e11e9`
VERDICT = `MAIN_DRIFT_ACK / NON_OVERLAPPING`

Compact compare reports 7 main commits since the pinned base. Changed files are governance/control-plane only: research-integrity workflow, `LATEST_SYNC.md`, `docs/ACTIVE_CLAIMS.md`, evidence-boundary metadata and meta promotion records.

No changed main file touches R4's recovered renderer/cache field semantics, sidecar ownership substrate, exact target identity, M3.2B runtime baseline or sidecar/replacement PRIMARY FACT.

The separate 2026-09-07 RC3 runtime reconciliation currently lives on `state/rc3-safe161-runtime-gate` and claim-scoped promotion branch `promote/rc3-runtime-reconcile-20260908`; it has not been wholesale-merged into main. R4 must consume any future accepted native-aim/visibility canonical delta only after promotion, and only where it overlaps sidecar ownership.

No semantic rebase is justified now.
