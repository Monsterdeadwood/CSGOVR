# CSGOVR — LATEST SYNC

SYNC_EPOCH = 2026-09-08
PROJECT_SYNC_CONTRACT = v1.1
SOURCE_OF_TRUTH = `GLOBAL_STATE.md`
GLOBAL_REVISION_REQUIRED = TRUE
LOCAL_STATE_PRIORITY = LOWER_THAN_GLOBAL

## Mandatory read order
1. `GLOBAL_STATE.md`
2. `CLAIM_GRAPH.json`
3. `RESEARCH_CONTRACT.yaml`
4. `research/FARWORLD_PENDING_SYNC_20260908.md`
5. `docs/ACTIVE_CLAIMS.md`
6. `docs/SYNC.md`

## Current promoted truth
- M3.2B world/HMD 6DoF remains protected rollback authority.
- RC3 native CUserCmd execution is PROVEN only for the observed runtime corridor.
- RC3 all-up SAFE161 integration baseline remains FALSIFIED because protected visual baseline failed.
- AIM alignment remains ACTIVE / owner UNBOUND.
- FAR_WORLD remains PENDING_VALIDATION / owner UNBOUND.
- LIVE remains blocked while the FAR_WORLD pending packet exists.

## Rev5 COLD narrowing
The pending FAR_WORLD validation now records:
- SAFE161 is a strong negative control against treating asymmetric Quest projection or the known area-frustum correctness issue as sufficient by itself to explain RC3 far-world regression;
- RC3 client `player+0x382C` mutation is a rejected AIM-side effect, but no exact path from that field into CViewSetup/frustum/PVS/ordinary-world membership has been recovered;
- frozen `IsInputDeviceAPointer` census isolates the RC3 override to weapon-final-aim routes rather than render/view/frustum callers; RC3's extra grenade route is an AIM-scope defect, not a visibility owner;
- recovered CheckSplitScreenMimic contract calls stock behavior first and then writes CUserCmd/provenance state; no deliberate split-view/frustum mutation is present;
- exact raw executed RC3 detour ABI remains unverified because the raw package is not currently surfaced as a materializable Library artifact.

Current model ranking:
```text
INDEPENDENT AIM + FAR_WORLD DEFECTS = LEADING
SHARED INFRASTRUCTURE ROOT = WEAKENED
SHARED SEMANTIC ROOT = STRONGLY WEAKENED
```

## FIRST_MISSING_EDGE
```text
late CViewSetup
 -> CPU camera frustum
 -> portal/area frustum refinement
 -> ClientLeafSystem / CullBox
 -> ordinary renderable membership
```
Prove this ownership/lifetime chain COLD before any new causal LIVE.

## Stop rule
Do not use `r_portalsopenall` as standalone PVS proof. Do not promote pixel-visibility effects history as ordinary-world causality. Do not promote AIM-side client player state as FAR_WORLD root without a real visibility edge. Do not mix AIM and FAR_WORLD mutations.