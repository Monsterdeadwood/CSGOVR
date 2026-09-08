# CSGOVR — LATEST SYNC

SYNC_EPOCH = 2026-09-08
PROJECT_SYNC_CONTRACT = v1.1
SOURCE_OF_TRUTH = `GLOBAL_STATE.md`
GLOBAL_REVISION_REQUIRED = TRUE
LOCAL_STATE_PRIORITY = LOWER_THAN_GLOBAL

## Mandatory read order
1. `GLOBAL_STATE.md` — canonical source of truth, revision/timestamp, PENDING_SYNC, ACTIVE/PENDING ownership/conflicts and FIRST_MISSING_EDGE.
2. `CLAIM_GRAPH.json` — canonical claim statuses/evidence bindings.
3. `RESEARCH_CONTRACT.yaml` — machine control-plane contract.
4. `docs/EVIDENCE_BOUNDARY.md` and `docs/BRANCH_PROMOTION_POLICY.md`.
5. `docs/ACTIVE_CLAIMS.md` — control-lane ownership registry.
6. `docs/SYNC.md` — compatibility/evidence snapshot subordinate to `GLOBAL_STATE.md`.

## Current promoted truth
- M3.2B world/HMD 6DoF remains protected rollback authority.
- RC3 relocation-aware three-hook native CUserCmd execution is PROVEN for the actually observed runtime corridor.
- RC3 as an all-up SAFE161 integration baseline is FALSIFIED because the protected visual baseline failed.
- observed server runtime corridor was `BULLET_B_EDI`; do not promote all firearm corridors from this RESULT.
- controller -> authoritative-shot alignment remains `ACTIVE`, owner `UNBOUND`.
- FAR_WORLD / `CSGO-PACING-PVS` is now `PENDING_VALIDATION`, owner `UNBOUND`, through `research/FARWORLD_PENDING_SYNC_20260908.md`.
- stock `Weapon_ShootPosition`, recoil/AimPunch, spread/inaccuracy and FX semantics remain protected.

## PENDING_SYNC
```text
1. CSGO-EV-FARWORLD-PENDING-20260908
   research/FARWORLD_PENDING_SYNC_20260908.md
   CLAIM = CSGO-PACING-PVS
   STATUS = PENDING_VALIDATION
   OWNER = UNBOUND
```

The packet does not prove PVS, CPU visibility-envelope causality, portal causality, pixel-visibility causality, or any proposed fix. It exists to validate provenance and scope with exact current-lineage COLD evidence.

`RESEARCH_CONTRACT.pending_evidence_refs` points to this packet and `block_live_when_pending=true`, so LIVE is blocked until pending convergence is resolved.

## Governance state
`GLOBAL_STATE.md` is canonical under PROJECT_SYNC_CONTRACT v1.1. The old `meta/tab5-governance` branch is stale read-only provenance. The registered R4 sidecar branch remains `HOLD / DRIFT_RECONCILE_REQUIRED`.

## Stop rule
Every cycle starts from the newest `GLOBAL_STATE.md`. Do not continue from stale LOCAL_STATE. Do not mix AIM and FAR_WORLD. Do not execute LIVE while PENDING_SYNC is non-empty. Do not reintroduce `g_weaponGrabRotOffset` ballistic correction, render-space muzzle/attachment ownership, global FX/pointer mutation, or RC2 relocation-sensitive guarding.