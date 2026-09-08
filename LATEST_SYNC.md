# CSGOVR — LATEST SYNC

SYNC_EPOCH = 2026-09-08
PROJECT_SYNC_CONTRACT = v1.1
SOURCE_OF_TRUTH = `GLOBAL_STATE.md`
GLOBAL_REVISION_REQUIRED = TRUE
LOCAL_STATE_PRIORITY = LOWER_THAN_GLOBAL

## Mandatory read order
1. `GLOBAL_STATE.md` — canonical source of truth, revision/timestamp, PENDING_SYNC, ACTIVE ownership/conflicts and FIRST_MISSING_EDGE.
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
- final controller -> authoritative-shot alignment remains ACTIVE with dedicated technical owner UNBOUND.
- far-world/distant-texture disappearance remains ACTIVE with dedicated technical owner UNBOUND in the separate pacing/PVS/visibility lane.
- stock `Weapon_ShootPosition`, recoil/AimPunch, spread/inaccuracy and FX semantics remain protected.

## Governance state
`GLOBAL_STATE.md` is canonical under PROJECT_SYNC_CONTRACT v1.1. The old `meta/tab5-governance` branch is stale read-only provenance.

The registered R4 sidecar branch is held for MAIN_DRIFT reconciliation: its last ACK stopped at `main=943fb84...` before the RC3 runtime reconciliation and GLOBAL_STATE control-plane promotions now present on current main. Do not continue R4 from that stale ACK.

A prior rev3 promotion attempt was closed without merge when main advanced during validation by one empty `NOOP` commit. The rev3 promotion was reconstructed from exact current main as required by GLOBAL_WRITE_RULE.

No formal unpromoted HANDOFF/SYNC_PACKET is currently registered. Existing Library/chat analyses are candidates only until packetized and validated.

## Stop rule
Every cycle starts from the newest `GLOBAL_STATE.md`. Do not continue from stale LOCAL_STATE. Do not repair aim alignment and far-world visibility in the same intrusive LIVE. Do not reintroduce `g_weaponGrabRotOffset` ballistic correction, render-space muzzle/attachment ownership, global FX/pointer mutation, or RC2 relocation-sensitive guarding.