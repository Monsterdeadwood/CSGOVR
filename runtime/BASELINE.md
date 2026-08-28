# CSGOVR — RUNTIME BASELINE HIERARCHY

Canonical current state: `docs/SYNC.md`.

This file defines **rollback / ancestry roles only**. It must not define an independent CURRENT gate.

## Layer A — world / HMD rollback authority

`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256:
`5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Evidence:
`CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`

User visual verdict: **VR + full HMD 6DoF works.**

Protected meaning:
- Quest stereo transport/projection is closed;
- native camera origin is closed;
- HMD yaw/pitch/roll is closed;
- positional 6DoF is closed;
- tearing/cadence remains a separate quality issue.

M3.2B is the rollback authority for world/HMD correctness. Weapon experiments do not replace it.

## Layer B — weapon / aim visual integration ancestor

`CSGOVR_WEAPON_SAFE_POSTCALC_AIM_v1.6.1_ONECLICK`

Retained user-visible properties:
- distant textures/objects remained present;
- shot audio remained present;
- exact slot250 stock arms/viewmodel hand-follow lineage is retained.

This is the preferred visual/render ancestor for RC3 weapon/aim integration because the later RC2 parent regressed the far world.

## Layer C — historical renderer ancestry

`CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`

Canonical historical result:
`CSGOVR_HLAE_EYE_PATCH_RESULT_20260821_015737.zip`

Generated package SHA256:
`20D382A537CF97AA432CA0BA94DE279EB12764B4724793C217722538BCB78A6D`

This remains useful provenance for the original HLAE eye scheduling/full-`CViewSetup` lineage, but it is **not the current integration baseline and does not authorize projection work**.

Retained historical facts:
- pass1 LEFT, pass2 RIGHT, pass3 monitor;
- full `CViewSetup` size `0x198`;
- HLAE `CAfxStreams::DoRenderView` seam `+0x00022340`;
- full-object copy constructor `+0x00014BA0`;
- original recovered source remains under `runtime/golden_v0.5.5/src/`.

## Current active runtime claim

`CLAIM_ID = CSGO-RC3-SAFE161-3HOOK-RUNTIME`

Current package/ticket:
`experiments/RC3_SAFE161_3HOOK_GUARDFIX_LIVE_TICKET_20260828.md`

`CSGOVR_ULTIMATE_SAFE161_NATIVE_AIM_RC3_3HOOK_GUARDFIX_ONECLICK.zip`

RC3 is not a new baseline until an executed RESULT and required user-visible verdict promote it.

## Separate quality lane

`CLAIM_ID = CSGO-PACING-PVS`

Do not mix PVS/culling mutation into the RC3 weapon/aim causal run.

## Baseline law

A prepared package never replaces a baseline. A machine counter never overrides a contradictory visual baseline verdict. Promotion requires the exact acceptance law in `docs/SYNC.md`.
