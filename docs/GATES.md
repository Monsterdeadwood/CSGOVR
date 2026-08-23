# CSGOVR — GATES

Snapshot: 2026-08-24 final cross-branch reconciliation.

## G0 — OpenXR / Quest input
**PASS.** Quest 3 two views, asymmetric FOV, projection coefficients and D3D11 adapter requirements are established.

## G1 — HLAE stereo GOLDEN
**PASS.** `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`: pass1 LEFT, pass2 RIGHT, pass3 monitor, full `CViewSetup=0x198`. Original GOLDEN BMPs support real depth-dependent horizontal stereo parallax.

## G2 — Projection-control semantics
**PASS.** Custom projection `+0x68/+0x6C..+0xAB`, off-center tail and M00/M02/M11/M12 causal semantics are established.

## T0 / G2.5 — GPU XR transport certification
**CURRENT GATE — OPEN.**

Inserted by late branch reconciliation because final two-eye CSGOVR OpenXR transport/submission is still unproven and can be isolated without changing geometry.

Preferred path is exact pinned HLAE GPU interop, not CPU readback:
- replacement eye render targets through HLAE interop
- pass1 LEFT + pass2 RIGHT from same HLAE cycle -> one `pairSeq`
- GPU completion before publish
- external x64 D3D11 device on OpenXR adapter LUID `00000000:000118D1`
- acquire/wait/copy/release both eye swapchains
- one `xrEndFrame` per coherent pair

PASS requires no mixed L/R sequence, no black/freeze, and no v0.5.5 geometry/monitor regression.

## G3 / T1 — Quest asymmetric projection integration
**QUEUED AFTER T0 PASS; NOT FALSIFIED.**

Repository candidate already exists:
- `experiments/quest_projection_child/eye_patch_quest_projection.c`
- `QUEST_PROJECTION_MINIMAL.patch`
- verifier / CI

Apply exact P00 to correct local eye copy, preserve engine zNear/zFar, use Source-native scalar FOV `110°` for conservative culling envelope, preserve v0.5.5 origin behavior and pass3 monitor.

G3 diagnostic batch v1.0.1 is UNRUN; matrix-only phase is primary-useful. Aspect `0.9254939479` and FOV150 are secondary clipping diagnostics only.

## G4 — Concrete culling / visibility blocker, if any
**CLOSED UNTIL G3 VERDICT.**

Retained targets:
- `client+0x003A16A6`
- `client+0x003A01E4`
- `client+0x0086E9C0`
- writers `+0x0086EB3C`
- CSimpleWorldView vtable `+0x00BF4434`, Draw `+0x003B0870`, fields `+0x168/+0x16C/+0x170`

Do not reopen `client+0x0525BA60` both-eye reuse.

## G5 — Authoritative eye origin / parallax
Only if T1 visibly lacks correct depth/parallax. Late DUAL_ORIGIN negative result applies only to that exact intervention point; GOLDEN + WORLD_X_PAIR provide independent positive origin/parallax evidence.

## G6 — Quantitative capture truth
Require same-cycle L/R and repeated same-eye stability before pixel-difference culling/disparity claims.

## G7 — Head pose
Only after transport/projection/origin gates are coherent.

## G8 — Weapon / controller / HUD
After world camera is regression-green. Keep world and weapon ownership separate.

## G9 — Regression / release
Frozen hashes, `-insecure`, monitor pass, coherent pairSeq, XR copy/submit stability, no eye-specific corruption, menu/gameplay transitions, reversible one-click restore.
