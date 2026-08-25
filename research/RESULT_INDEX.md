# CSGOVR — RESULT INDEX

Authority: frozen retail evidence + cross-branch reconciliation through **2026-08-25 late M34 rollback**.

Evidence classes: GOLDEN / PROVEN / SUPPORTED / PENDING / INFRASTRUCTURE / FALSIFIED / INVALID / SUPERSEDED / UNRUN.

## CURRENT RUNTIME GOLDEN — `CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`
Build:
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256:
`5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Class: **GOLDEN / user-confirmed runtime.**
- VR works
- real HMD rotation works
- positional HMD translation works
- full HMD 6DoF works
- tearing remains a quality defect

This is the rollback baseline for all viewmodel/hand work.

## `CSGOVR_MASTER32A_POSE_COHERENCE_RESULT_20260825_194358.zip`
Class: **PROVEN instrumentation / quality finding.**
- host identity PASS
- native guard PASS
- Quest matrix PASS
- pair proof PASS
- LEFT/RIGHT same pose PASS
- exact render-pose lookup/submission PASS
- all 9 phases PASS
- pose lookup misses 0
- capture failures 0
- observed `droppedPairs=179`
- two-slot starvation and large stale bursts identified

Retain exact render-pose association. Slot starvation is a cadence finding, not a reason to reopen camera ownership.

## M33 runtime lineage
Class: **SUPPORTED integration evidence.**
M33 reached real game pairs with the slot8/controller-capable transport lineage. Therefore slot depth alone is not the leading explanation for the later M34 first-render failure.

## M34 v0.2.x / `_RUN_20260825_221228`
Class: **INVALID / FROZEN as a baseline; useful failure-boundary evidence.**

The run reached:
- CS:GO launch
- `de_dust2`
- transport DLL injection
- hand-probe DLL injection

It then stopped at the first HLAE stereo render after both injections.

Coupled changes before that render:
1. LEFT/RIGHT `drawViewModel 0 -> 1`
2. passive/global `C_BaseAnimating::BuildTransformations` vtable index 184 hook

Because both were enabled together, this run does **not** identify which change caused the failure.

Decision: roll back to M3.2B GOLDEN and isolate A/B/C serially. See `docs/M34_FAILURE_ROLLBACK_20260825.md`.

---

# HISTORICAL FOUNDATION

## `CSGOVR_HLAE_EYE_PATCH_RESULT_20260821_015737.zip`
Class: **GOLDEN historical stereo foundation.**
Build `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`.
- pass1 LEFT, pass2 RIGHT, pass3 monitor
- full local `CViewSetup=0x198`
- HLAE DoRenderView `+0x22340`, copy ctor `+0x14BA0`
- nominal half-eye `1.2598425 Source units`
- original BMPs show stable depth-dependent horizontal L/R parallax

## `CSGOVR_CORRECTNESS_RESULT_20260821_011601.zip`
Class: **PROVEN causal retail.**
- `CViewSetup.origin -> VS c2/CAMERA_POS`
- WORLD_X_PAIR translation changes pixels/draw membership
- projection families c4..c7 / c8..c11

## `CSGOVR_PROJECTION_TRUTH_RESULT_20260821_035411.zip`
Class: **PROVEN off-center semantics.**
- `+0x138/+0x13C/+0x140/+0x144`, flag `+0x148 bit0`

## `CSGOVR_CUSTOM_PROJECTION_MATRIX_RESULT_20260821_042218.zip`
Class: **PROVEN custom projection.**
- enable `+0x68`, matrix `+0x6C..+0xAB`
- M00/M02/M11/M12 causal semantics
- deterministic v0.6.1 capture method retained

## `CSGOVR_OPENXR_FOV_DIAGNOSTIC_20260821_150332.zip`
Class: **PROVEN XR input.**
- Quest 3 / Meta OpenXR 1.205.0
- two valid views
- asymmetric LEFT/RIGHT FOV
- recommended 1504x1616 per eye
- adapter LUID `00000000:000118D1`
- passive eye separation about 64.780414 mm

## Valve SourceVR native scale cross-check
Class: **PROVEN source semantics.**
- `39.3700787 inches/meter`
- native SourceVR HMD/eye path uses `1 Source unit = 1 inch`
- Quest half-IPD about `1.27520 Source units`

## `CSGOVR_NATIVE_STEREO_SEAM_RESULT_20260821_041928`
Class: **PROVEN/SUPPORTED split.**
One exact late DUAL_ORIGIN intervention was negative. Scope is narrow and does not falsify eye translation generally.

## `CSGOVR_TTD_FRUSTUM_CACHE_REPLAY_RESULT_20260822_012630.zip`
Class: **FALSIFIED + harvest.**
- falsified `client+0x0525BA60` both-eye reuse
- retained frustum/readers lineage

## `CSGOVR_TTD_REPLAY_FRUSTUM_READER_OWNER_3A16A6_RECOVERED_RESULT_20260823_160154.zip`
Class: **PROVEN evidence packet / semantics pending.**

## `CSGOVR_TTD_FRUSTUM_ARG2_PERSISTENCE_RESULT_20260823_164733.zip`
Class: **PROVEN + FALSIFIED.**
- first 0x40 temporary transpose/restore PROVEN
- persistent `+0x40..+0x1FF` FALSIFIED

## Origin/yaw control results
- `CSGOVR_CAUSAL_ORIGINX_ZERO_CONTROL_RESULT_20260823_043045.zip` — INFRASTRUCTURE
- `CSGOVR_CAUSAL_YAW_FINAL_POSE_CONTROL_RESULT_20260823_071600.zip` — INFRASTRUCTURE

They are useful anchors, not final camera-ownership proof.

## `CSGOVR_FULL_EXPERIMENTAL_FAILURE_20260821_022832.zip`
Class: **INVALID build/workflow failure.** No renderer/XR conclusion.

## Exact HLAE / AdvancedFX interop review — 2026-08-24
Class: **PROVEN source/build capability.** Historical transport implementation evidence remains useful, but old T0/G2.5 is no longer the current gate.

## `CSGOVR_XR_TRANSPORT_v0.7.0_BUILD_FIRST_ONECLICK.zip`
Class: **SUPERSEDED prepared lineage.**

## `CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF`
Class: **historical prepared/static-audited fallback lineage.**
SHA256 `DFEA881888C83812A5B9EE3A7049F79D32935240F2C612DE4ABDF4F9A98834A3`.

Its pair/ABI/reset work remains historical implementation evidence. It is not the current task.

## CURRENT GATE
**Viewmodel reintroduction / hand-path isolation on exact M3.2B GOLDEN.**

Next sequence:
A exact GOLDEN -> B drawViewModel only -> C passive BuildTransformations -> D controller IPC -> E transform mutation.
