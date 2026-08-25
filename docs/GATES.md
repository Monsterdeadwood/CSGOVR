# CSGOVR — GATES

Snapshot: **2026-08-25 late sync after M34 rollback.**

## G0 — OpenXR / Quest input
**PASS.** Quest 3 two views, asymmetric FOV and adapter/runtime requirements established.

## G1 — HLAE stereo world path
**PASS.** Frozen v0.5.5 lineage established two real world streams, pass1 LEFT / pass2 RIGHT / pass3 monitor, full `CViewSetup=0x198`.

## G2 — Projection semantics
**PASS.** Custom projection and Quest asymmetric projection are established. Do not reopen sign/transpose roulette.

## G2.5 / MASTER1 — End-to-end transport
**PASS in the accepted MASTER1 lineage.** Historical T0 GPU-vs-CPU finalist debate is no longer the current gate.

## G3 / MASTER2 — Quest projection integration
**PASS.** Exact Quest projection is part of the later accepted runtime path.

## G4 / MASTER3.1 — Native camera origin ownership
**PASS.** Native authority at `client+0x003A5280` is closed.

## G5 / MASTER3.2A — HMD rotation
**PASS.** Real Quest HMD rotation drives the native camera path.

Supporting pose-coherence instrumentation:
`CSGOVR_MASTER32A_POSE_COHERENCE_RESULT_20260825_194358.zip`.

## G6 / MASTER3.2B — Positional HMD translation / full 6DoF
**PASS / CURRENT RUNTIME GOLDEN.**

Package:
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256:
`5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Evidence:
`CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`

User visual verdict: **VR + full HMD 6DoF works.** Tearing remains a separate quality defect.

## G7 — Viewmodel reintroduction / hand-path isolation
**CURRENT GATE — OPEN.**

M34 v0.2.x is INVALID/FROZEN because two changes were enabled before the first stereo render:
- `drawViewModel=1`
- passive/global `BuildTransformations` vtable index 184 hook

The run reached launch, map and both DLL injections, then failed at the first HLAE stereo render, so the two changes must be isolated serially.

Required sequence:
A. exact M3.2B GOLDEN
B. `drawViewModel=1` only
C. passive `BuildTransformations` observation only after B PASS
D. controller IPC only after C PASS
E. viewmodel transform mutation only after D PASS

## G8 — Weapon/controller/HUD
**QUEUED.** Do not bind weapon/controller ownership to the world-camera experiment. Controller IPC begins only after passive viewmodel observation is stable.

## G9 — Quality / performance / regression / release
**OPEN LATER.** Includes tearing/cadence quality, performance, comfort, HUD polish, transition stability and reversible release packaging.

## STOP RULE
Do not reopen G0-G6 without contradictory evidence. Do not patch M34 forward from its coupled failing state. Preserve exact M3.2B GOLDEN while isolating G7.
