# CSGOVR — GATES

## G0 — OpenXR / Quest input
**PASS.** Two valid Quest 3 views, asymmetric FOV and projection coefficients are established.

## G1 — HLAE stereo GOLDEN
**PASS.** `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX` provides two real world streams: pass1 LEFT, pass2 RIGHT, pass3 monitor; full `CViewSetup=0x198`.

Original v0.5.5 image re-analysis also supports real depth-dependent horizontal stereo disparity. Do not reopen the old broad eye-origin batch without a new visible blocker.

## G2 — Projection-control semantics
**PASS.** Custom projection bool `+0x68`, matrix `+0x6C..+0xAB`, off-center fields `+0x138/+0x13C/+0x140/+0x144` and flag `+0x148 bit0` are established. M00/M02/M11/M12 semantics were causally measured with deterministic frozen captures.

## G3 — Quest asymmetric projection integration
**CURRENT GATE — OPEN.**

Primary candidate already exists in the repo:
- `experiments/quest_projection_child/eye_patch_quest_projection.c`
- `experiments/quest_projection_child/QUEST_PROJECTION_MINIMAL.patch`
- `tools/verify_quest_projection_child.py`
- Evidence Integrity CI

Required mutation:
- exact v0.5.5 parent
- preserve LEFT/RIGHT origin behavior
- apply known Quest LEFT/RIGHT custom matrix only to the corresponding local eye copy
- pass3 monitor untouched
- no culling/origin/head/weapon mutation in the same PRIMARY

PASS:
- correct eye receives correct Quest asymmetric matrix
- two-world-stream GOLDEN remains intact
- monitor pass remains intact
- no new eye corruption
- headset/same-frame verdict recorded

No accepted RESULT has yet closed G3.

## G4 — Concrete culling / visibility blocker, if any
**CLOSED UNTIL G3 VERDICT.** Opens only if a visible culling blocker survives G3.

Retained targets:
- `client+0x003A16A6`
- `client+0x003A01E4`
- `GeneratePerspectiveFrustum client+0x0086E9C0`
- writers around `+0x0086EB3C`
- CSimpleWorldView vtable `+0x00BF4434`, Draw `+0x003B0870`, fields `+0x168/+0x16C/+0x170`

Do not reopen `client+0x0525BA60` both-eye reuse; FALSIFIED.

## G5 — Head pose / authoritative final eye integration
Only open if a concrete visible pose/origin blocker remains after G3/G4. The old late DUAL_ORIGIN negative result applies only to that exact intervention point; the GOLDEN itself has independent parallax evidence.

## G6 — Final OpenXR transport / submission certification
Current accepted CSGOVR result set does **not** prove final two-eye swapchain submission to Quest. If G3 child renders correctly to HLAE captures but headset transport is absent/black, promote transport itself to the concrete blocker and solve swapchain/copy/submission without reopening projection math.

## G7 — Weapon / controller separation
Proceed only after stereo/projection/culling/head/transport are regression-green.

## G8 — HUD / comfort / regression / release candidate
Last-stage work only.
