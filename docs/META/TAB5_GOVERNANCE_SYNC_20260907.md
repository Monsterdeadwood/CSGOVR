# CSGOVR — TAB5 MASTER MERGER / REDTEAM / MUTATION SEMAPHORE

SYNC_EPOCH = 2026-09-08
SOURCE_TAB = TAB5
ROLE = META
BRANCH = `meta/tab5-governance`
STATE = COLD / CONTROL-PLANE
CANONICAL_BINDING = NONE

## Literal line
`LINE_GOVERNANCE`

## Role boundary
TAB5 is the MASTER merger/redteam/STOP engine. It is **not** a LIVE executor and TAB number five does not imply R5.

## Responsibilities
- merge sibling `SYNC_PACKET` / STATE / EVIDENCE / RESULT deltas;
- detect stale target, stale branch, claim collisions and contradictory evidence;
- preserve evidence priority and exact scope;
- keep AIM, VISIBILITY, ALIGNMENT/BALLISTICS and PACING as separate causal lanes;
- enforce the global mutation semaphore: one intrusive PRIMARY FACT at a time;
- compute the current `FIRST_MISSING_EDGE` and highest-information next action;
- veto cosmetic vNext experiments that repeat a falsified approach;
- ensure baseline failures are not hidden by passing plumbing counters.

## Current project split consumed
- TAB1 = AIM / native CUserCmd and fire-corridor provenance;
- TAB2 = FAR WORLD / VISIBILITY negative-control lane;
- TAB3 = ALIGNMENT / BALLISTICS source atlas;
- TAB4 = PACING / XR transport diagnostics;
- TAB5 = META governance / merger / STOP.

This mapping is operational/tab-level. Do not infer R1/R2/R3/R4 role IDs from the TAB numbers unless explicitly recovered elsewhere.

## 2026-09-07 RC3 result governance
The executed `CSGOVR_ULTIMATE_SAFE161_NATIVE_AIM_RC3_3HOOK_RESULT_20260907_050436.zip` must be split, not flattened:
- native 3-hook CUserCmd execution = PROVEN for the actually observed runtime corridor;
- all-up SAFE161 RC3 integration baseline = FALSIFIED due crooked alignment + far-world disappearance;
- final aim alignment = OPEN;
- far-world causality = OPEN and separate;
- observed server runtime corridor = `BULLET_B_EDI`, not all firearm corridors.

Canonical promotion of that split is staged on `promote/rc3-runtime-reconcile-20260908` / draft PR #20. Do not treat unmerged promotion state as already canonical main.

## Protected boundaries
- M3.2B remains world/HMD rollback authority;
- stock `Weapon_ShootPosition`, recoil/AimPunch, spread/inaccuracy and FX semantics remain protected;
- `g_weaponGrabRotOffset`, attachment matrices and render-space muzzle transforms are not ballistic provenance;
- visibility/PVS mutation must not be combined with aim/alignment mutation in one causal LIVE.

## STOP RULE
META may create control-plane or promotion deltas, but may not execute another lane's LIVE. No project-wide wholesale merge of COLD/state branches into main.
