# CSGOVR

Native VR research for **frozen legacy CS:GO x86** using HLAE/OpenXR and exact retail evidence.

## Current status
**ACTIVE. Stereo GOLDEN and projection semantics are already proven.**

GOLDEN:
`CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`

Current engineering gate:
**integrate the known Quest 3 asymmetric projection into the known-good LEFT/RIGHT eye path and validate it in-headset/same-frame.**

Do not restart broad camera/OpenXR archaeology unless a concrete visible blocker survives this integration.

## Already proven
- two real world streams: LEFT, RIGHT, plus monitor pass
- full `CViewSetup` size `0x198`
- custom projection enable `+0x68`
- custom projection matrix `+0x6C..+0xAB`
- off-center fields `+0x138/+0x13C/+0x140/+0x144`, flag `+0x148` bit0
- Quest 3 asymmetric FOV/coefficients and 1504x1616 recommended eye size
- concrete pre-eye frustum/cache reader `client.dll+0x003A16A6`

## Important evidence correction
`client.dll+0x0038D123`, `CViewSetup.origin.x +0xB8` and `client.dll+0x052632CC` are retained infrastructure/candidate seams. Their 2026-08-23 control wrappers do **not** prove final camera ownership.

`client.dll+0x0525BA60` is a real cache region, but the theory that it is reused inside BOTH eye marker regions is FALSIFIED.

## Start here
1. `docs/SYNC.md` — canonical current truth and evidence grades
2. `docs/STATE.md` — concise active state
3. `docs/GATES.md` — current integration ladder
4. `docs/PROVEN.md` — proven XR/stereo/projection/culling facts
5. `docs/FALSIFIED.md` — dead/superseded paths
6. `research/RESULT_INDEX.md` — exact result ledger
7. GitHub Issue #1 — active projection-integration gate

## Working model
`SYNC -> THINK -> KEEP/ADAPT/STOP -> BATCH HARVEST -> CONVERGE -> PROVEN/FALSIFIED -> NEXT GATE -> SYNC`

**PARALLELIZE EVIDENCE — SERIALIZE MUTATION.**

Evidence grades are strict: GOLDEN / PROVEN / SUPPORTED / PENDING / INFRASTRUCTURE / FALSIFIED / INVALID / SUPERSEDED.

## Repository map
- `docs/` — canonical state and policy
- `research/` — evidence and static/runtime archaeology
- `experiments/` — reproducible controlled experiments
- `runtime/` — accepted integration code
- `tools/` — project tooling
