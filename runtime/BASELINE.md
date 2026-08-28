# CSGOVR — RUNTIME BASELINE HIERARCHY

Canonical current state: `docs/SYNC.md`.

This file defines **rollback / ancestry roles only**. It does not independently authorize LIVE work.

## Layer A — world / HMD rollback authority

`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256:
`5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Evidence:
`CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`

User visual verdict: **VR + full HMD 6DoF works.**

Protected world/HMD meaning:
- Quest stereo transport/projection closed;
- native camera origin closed;
- HMD yaw/pitch/roll closed;
- positional 6DoF closed;
- tearing/cadence remains a separate quality issue.

## Layer B — weapon / aim visual integration ancestor

`CSGOVR_WEAPON_SAFE_POSTCALC_AIM_v1.6.1_ONECLICK`

Retained user-visible properties:
- distant textures/objects remained present;
- shot audio remained present;
- exact slot250 stock arms/viewmodel hand-follow lineage retained.

This is the preferred visual/render ancestor for RC3. RC2 is not a baseline because it regressed the far world and never activated native aim.

## Layer C — historical renderer ancestry

`CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`

Historical result:
`CSGOVR_HLAE_EYE_PATCH_RESULT_20260821_015737.zip`

Generated package SHA256:
`20D382A537CF97AA432CA0BA94DE279EB12764B4724793C217722538BCB78A6D`

This remains provenance only, not a current mutation gate.

## Current COLD gate

`CLAIM_ID = CSGO-RC3-PRELIVE-IMPLEMENTATION-CLOSURE`

The exact RC3 package must close the fail-closed provenance/session/hook-install implementation contract before LIVE.

## Queued runtime candidate

`CLAIM_ID = CSGO-RC3-SAFE161-3HOOK-RUNTIME`

Ticket:
`experiments/RC3_SAFE161_3HOOK_GUARDFIX_LIVE_TICKET_20260828.md`

Package:
`CSGOVR_ULTIMATE_SAFE161_NATIVE_AIM_RC3_3HOOK_GUARDFIX_ONECLICK.zip`

RC3 remains prepared but is not a baseline and is not LIVE-authorized until the COLD implementation gate passes.

## Separate quality lane

`CLAIM_ID = CSGO-PACING-PVS`

Do not mix PVS/culling mutation into the RC3 implementation closure or weapon/aim causal run.

## Baseline law

A prepared package never replaces a baseline. Architecture proof does not automatically prove one concrete implementation. User-visible causal regression overrides healthy plumbing counters for visual promotion.
