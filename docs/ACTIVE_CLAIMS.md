# CSGOVR - ACTIVE RESEARCH LANES REGISTRY

CONTROL_PLANE_ONLY. This file does **not** own canonical claim status. Canonical status is owned by `GLOBAL_STATE.md`, `CLAIM_GRAPH.json`, and `RESEARCH_CONTRACT.yaml`.

## Active lanes

| ROLE | LANE | CANONICAL_BINDING | BRANCH | BASE_MAIN_SHA | STATUS | PRIMARY FACT | SCOPE |
|---|---|---|---|---|---|---|---|
| R4 | `CSGO-R4-SIDECAR-OWNERSHIP` | `UNBOUND_COLD_LANE` | `cold/r4/sidecar-ownership` | `9cb401e5e8f1ee78cbc4484eb20d244705bd5dad` | `HOLD / DRIFT_RECONCILE_REQUIRED` | classify exact semantics/lifetime/view-dependence/mutation and consumers of recovered renderer/cache fields before sidecar/replacement promotion | COLD_ONLY |
| META | `CSGO-TAB5-GOVERNANCE` | `GLOBAL_STATE_CONTROL` | `GLOBAL_STATE.md` | `GLOBAL_REVISION` | `COLD_CONTROL` | merge sibling state/evidence/results, enforce evidence scope and mutation semaphore, detect contradictions/collisions, compute FIRST_MISSING_EDGE and STOP repeated/falsified work | CONTROL_ONLY |

## R4 drift conflict
`cold/r4/sidecar-ownership` last recorded `MAIN_DRIFT_ACK / NON_OVERLAPPING` against `main=943fb84be55e56871769e71505e7b2f1440e11e9`. That ACK explicitly predates canonical promotion of the 2026-09-07 RC3 runtime reconciliation. Current main is sixteen commits ahead of that ACK boundary and includes RC3 runtime reconciliation, active AIM/FAR_WORLD claim changes, `GLOBAL_STATE.md`, `CLAIM_GRAPH.json`, synchronized control-plane files, and one non-semantic empty `NOOP` commit.

Therefore the old R4 ACK is stale. The branch remains provenance-preserved but may not feed LIVE, integration or promotion until it performs a new drift reconcile against newest `main` / `GLOBAL_REVISION`.

## Legacy META branch
`meta/tab5-governance` is retained as provenance only. It was based on `943fb84be55e56871769e71505e7b2f1440e11e9` and contains pre-promotion assumptions that are stale after RC3 reconciliation and GLOBAL_STATE migration. Do not continue research from that local state.

## Retired/superseded lane
`cold/r2/native-aim-implementation` previously owned `CSGO-RC3-PRELIVE-IMPLEMENTATION-CLOSURE`. The executed 2026-09-07 RC3 RESULT supersedes that pre-LIVE gate operationally. Do not continue that branch as if runtime activation were still unknown.

## Post-LIVE state reference
`state/rc3-safe161-runtime-gate` is historical post-LIVE provenance. Its accepted facts were already returned to canonical `main` through PR #20; it is not a substitute for newest `GLOBAL_STATE.md`.

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

Dedicated technical ownership remains UNBOUND until a formal sibling RESULT / STATE / HANDOFF proves the current owner. Do not invent ownership from TAB labels.

## Collision law
Before creating a new CSGOVR branch, read newest `GLOBAL_STATE.md`, record `GLOBAL_REVISION_AT_START`, inspect `PENDING_SYNC` and `CLAIM_GRAPH.json`, then inspect this registry. Same ROLE + same PRIMARY FACT means `REUSE -> ADAPT -> FIX -> EXTEND`, not duplicate research.

## Main drift law
Before any COLD conclusion is used for LIVE or promotion, compare its pinned base to current `main` and newest `GLOBAL_REVISION`. Reconcile changes touching M3.2B rollback authority, exact target identity, current native-aim execution claims, alignment ownership, visibility/PVS, sidecar renderer substrate, or the control plane itself.

Do not blindly rebase semantic conclusions. Do not wholesale-merge COLD/LIVE/state branches into `main`.