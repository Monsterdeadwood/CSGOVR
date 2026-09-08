# CSGOVR - ACTIVE COLD LANES REGISTRY

CONTROL_PLANE_ONLY. This file does **not** own canonical claim status. Canonical status is owned by `CLAIM_GRAPH.json` together with `RESEARCH_CONTRACT.yaml` and canonical state.

## Active COLD lanes

| ROLE | COLD_LANE | CANONICAL_BINDING | BRANCH | BASE_MAIN_SHA | PRIMARY FACT | SCOPE |
|---|---|---|---|---|---|---|
| R4 | `CSGO-R4-SIDECAR-OWNERSHIP` | `UNBOUND_COLD_LANE` | `cold/r4/sidecar-ownership` | `9cb401e5e8f1ee78cbc4484eb20d244705bd5dad` | classify exact semantics/lifetime/view-dependence/mutation and consumers of recovered renderer/cache fields before sidecar/replacement promotion | COLD_ONLY |

## Retired/superseded lane
`cold/r2/native-aim-implementation` previously owned `CSGO-RC3-PRELIVE-IMPLEMENTATION-CLOSURE`. The executed 2026-09-07 RC3 RESULT supersedes that pre-LIVE gate operationally. Do not continue that branch as if runtime activation were still unknown.

## Post-LIVE state reference
`state/rc3-safe161-runtime-gate` contains the executed-result reconciliation. Accepted facts are being returned to canonical state only through the claim-scoped promotion branch `promote/rc3-runtime-reconcile-20260908`.

## Canonical active claims without dedicated COLD branches
- `CSGO-RC3-AIM-ALIGNMENT`: exact controller -> authoritative-shot angular misalignment owner after native execution is active.
- `CSGO-PACING-PVS`: independent far-world/visibility/pacing causality lane.

Do not create a branch merely to fill a role slot. Create/recover one only when the exact existing sibling research line and PRIMARY FACT are known.

## Collision law
Before creating a new CSGOVR branch, inspect this registry and `CLAIM_GRAPH.json`. Same ROLE + same PRIMARY FACT means `REUSE -> ADAPT -> FIX -> EXTEND`, not duplicate research.

## Main drift law
Before any COLD conclusion is used for LIVE or promotion, compare its pinned base to current `main` and reconcile changes touching M3.2B rollback authority, exact target identity, current native-aim execution claims, alignment ownership, visibility/PVS, or sidecar renderer substrate.

Do not blindly rebase semantic conclusions. Do not wholesale-merge COLD/LIVE/state branches into `main`.
