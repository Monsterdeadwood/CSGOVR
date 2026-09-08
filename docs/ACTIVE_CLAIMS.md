# CSGOVR - ACTIVE RESEARCH LANES REGISTRY

CONTROL_PLANE_ONLY. This file does **not** own canonical claim status. Canonical status is owned by `CLAIM_GRAPH.json` together with `RESEARCH_CONTRACT.yaml` and canonical state.

## Active lanes

| ROLE | LANE | CANONICAL_BINDING | BRANCH | BASE_MAIN_SHA | STATUS | PRIMARY FACT | SCOPE |
|---|---|---|---|---|---|---|---|
| R4 | `CSGO-R4-SIDECAR-OWNERSHIP` | `UNBOUND_COLD_LANE` | `cold/r4/sidecar-ownership` | `9cb401e5e8f1ee78cbc4484eb20d244705bd5dad` | `COLD_ACTIVE / MAIN_DRIFT_ACK_20260908` | classify exact semantics/lifetime/view-dependence/mutation and consumers of recovered renderer/cache fields before sidecar/replacement promotion | COLD_ONLY |
| META | `CSGO-TAB5-GOVERNANCE` | `CONTROL_PLANE_ONLY` | `meta/tab5-governance` | `943fb84be55e56871769e71505e7b2f1440e11e9` | `COLD_CONTROL` | merge sibling state/evidence/results, enforce evidence scope and mutation semaphore, detect contradictions/collisions, compute FIRST_MISSING_EDGE and STOP repeated/falsified work | CONTROL_ONLY |

## Retired/superseded lane
`cold/r2/native-aim-implementation` previously owned `CSGO-RC3-PRELIVE-IMPLEMENTATION-CLOSURE`. The executed 2026-09-07 RC3 RESULT supersedes that pre-LIVE gate operationally. Do not continue that branch as if runtime activation were still unknown.

## Post-LIVE state reference
`state/rc3-safe161-runtime-gate` contains the executed-result reconciliation. Accepted facts are being returned to canonical state only through the claim-scoped promotion branch `promote/rc3-runtime-reconcile-20260908`.

## Operational TAB map
- TAB1 = AIM
- TAB2 = FAR WORLD / VISIBILITY
- TAB3 = ALIGNMENT / BALLISTICS
- TAB4 = PACING / XR TRANSPORT
- TAB5 = META governance / merger / STOP

Do **not** infer R1/R2/R3/R4/R5 role IDs from those TAB numbers. Create/recover a technical branch only when its exact role and PRIMARY FACT are independently known.

## Canonical active claims without dedicated COLD branches
- `CSGO-RC3-AIM-ALIGNMENT`: exact controller -> authoritative-shot angular misalignment owner after native execution is active.
- `CSGO-PACING-PVS`: independent far-world/visibility/pacing causality lane.

## Collision law
Before creating a new CSGOVR branch, inspect this registry and `CLAIM_GRAPH.json`. Same ROLE + same PRIMARY FACT means `REUSE -> ADAPT -> FIX -> EXTEND`, not duplicate research.

## Main drift law
Before any COLD conclusion is used for LIVE or promotion, compare its pinned base to current `main` and reconcile changes touching M3.2B rollback authority, exact target identity, current native-aim execution claims, alignment ownership, visibility/PVS, or sidecar renderer substrate.

Do not blindly rebase semantic conclusions. Do not wholesale-merge COLD/LIVE/state branches into `main`.
