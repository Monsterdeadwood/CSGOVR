# CSGOVR

Native VR research for **frozen legacy CS:GO x86** using HLAE/OpenXR and exact retail evidence.

## Current status
**ACTIVE — XR TRANSPORT is the current precondition; Quest projection is queued immediately after PASS.**

GOLDEN:
`CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`

### Why gate order changed
The 2026-08-24 projection sync correctly preserved the byte-locked Quest projection child, but its own STATE also records that final CSGOVR OpenXR swapchain/submission is **not yet proven**. A transport branch independently prepared a direct test for that missing edge.

No accepted result in either branch closes HLAE -> Quest live transport. This is a gate-order reconciliation, not a contradiction in PROVEN evidence.

Current transport candidate:
`CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF`

Artifact SHA256:
`DFEA881888C83812A5B9EE3A7049F79D32935240F2C612DE4ABDF4F9A98834A3`

Status: **PREPARED / STATIC-AUDITED / NOT LIVE-PROVEN**.

Manifest/provenance:
`runtime/children/v0.7.1_xr_transport/README.md`

## Gate order
`XR TRANSPORT -> QUEST ASYMMETRIC PROJECTION -> EYE ORIGIN/PARALLAX if visibly needed -> SAME-FRAME CULLING/TRUTH -> HEAD POSE -> WEAPON/HANDS -> HUD -> REGRESSION -> RELEASE`

The existing projection child, verifier and CI remain valid and untouched:
- `experiments/quest_projection_child/eye_patch_quest_projection.c`
- `experiments/quest_projection_child/QUEST_PROJECTION_MINIMAL.patch`
- `tools/verify_quest_projection_child.py`
- `.github/workflows/evidence-integrity.yml`

## Core PROVEN facts
- frozen target AppID `4465480`, BuildID `22100934`
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- two real HLAE world streams: pass1 LEFT, pass2 RIGHT, pass3 monitor
- full `CViewSetup=0x198`
- custom projection enable `+0x68`, matrix `+0x6C..+0xAB`
- Quest 3 two-view FOV and asymmetric matrix coefficients
- `CViewSetup.origin -> VS c2/CAMERA_POS`
- concrete pre-eye reader `client+0x003A16A6`
- CSimpleWorldView main-world lineage

## Do not repeat
- `0x14C` full-size assumption
- `client+0x0525BA60` both-eye reuse theory
- broad OpenXR/FOV rediscovery
- sequential screenshots as quantitative same-frame oracle
- Present as an eye oracle
- broad origin/culling reverse before an actual post-transport/post-projection visible blocker exists

## Start here
1. `docs/STATE.md`
2. `docs/SYNC.md`
3. `docs/NEXT.md`
4. `docs/GATES.md`
5. `docs/PROVEN.md`
6. `docs/FALSIFIED.md`
7. `docs/EXPERIMENTS.md`
8. `research/RESULT_INDEX.md`
9. `research/BRANCH_SYNC_20260824.md` — parallel projection sync
10. `research/TRANSPORT_SYNC_20260824.md` — transport reconciliation/provenance

## Working model
`SYNC -> THINK -> KEEP/ADAPT/STOP -> BATCH HARVEST -> CONVERGE -> PROVEN/FALSIFIED -> NEXT GATE -> SYNC`

**PARALLELIZE EVIDENCE — SERIALIZE MUTATION.**
