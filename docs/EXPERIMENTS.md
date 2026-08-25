# CSGOVR — EXPERIMENTS / TOOLS INDEX

Purpose: show a fresh branch what is current, what is accepted, and what must not be repeated.

Snapshot: **2026-08-25 late sync after M34 rollback.**

## CURRENT EXPERIMENT FAMILY — VIEWMODEL / HAND PATH
Protected baseline:
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256:
`5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Evidence:
`CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`

Class: **CURRENT GOLDEN / user-confirmed runtime.**
- VR works
- full HMD 6DoF works
- tearing remains a quality defect

### M34 v0.2.x
Class: **INVALID / FROZEN baseline, useful failure-boundary evidence.**

`_RUN_20260825_221228` reached game launch, `de_dust2`, transport injection and hand-probe injection, then stopped at the first HLAE stereo render.

Coupled changes:
1. LEFT/RIGHT `drawViewModel 0 -> 1`
2. passive/global `C_BaseAnimating::BuildTransformations` vtable index 184 hook

Because both were active before the first failing render, M34 cannot identify the culprit. Do not patch M34 forward.

### Next causal sequence
A. exact M3.2B GOLDEN
B. `drawViewModel=1` only, no hand DLL
C. only after B PASS: passive `BuildTransformations` observation, no controller, no mutation
D. only after C PASS: controller IPC
E. only after D PASS: viewmodel transform mutation

A one-click runner may execute A/B/C as separate auto-closed sessions and aggregate the results, but each process must have one primary mutation.

## RETAINED POSE / CADENCE EVIDENCE
### `CSGOVR_MASTER32A_POSE_COHERENCE_RESULT_20260825_194358.zip`
Class: **PROVEN instrumentation / quality finding.**
- host identity PASS
- native guard PASS
- Quest matrix PASS
- pair proof PASS
- LEFT/RIGHT same pose PASS
- exact render-pose association PASS
- all 9 phases PASS
- pose lookup misses 0
- capture failures 0
- `droppedPairs=179`
- two-slot starvation identified

Later M33 reached real pairs on slot8/controller-capable lineage, so slot depth alone is not the leading M34 suspect.

## CLOSED / ACCEPTED WORLD-CAMERA EXPERIMENTS
### `CSGOVR_HLAE_EYE_PATCH_RESULT_20260821_015737.zip`
Class: GOLDEN historical stereo foundation.
- pass1 LEFT / pass2 RIGHT / pass3 monitor
- full `CViewSetup=0x198`
- depth-dependent stereo parallax

### `CSGOVR_CORRECTNESS_RESULT_20260821_011601.zip`
Class: PROVEN causal retail renderer evidence.
- `CViewSetup.origin -> GPU VS c2/CAMERA_POS`
- WORLD_X_PAIR changes pixels/draw membership

### `CSGOVR_CUSTOM_PROJECTION_MATRIX_RESULT_20260821_042218.zip`
Class: PROVEN projection semantics.
- custom matrix enable `+0x68`
- matrix `+0x6C..+0xAB`
- M00/M02/M11/M12 causal semantics

### `CSGOVR_OPENXR_FOV_DIAGNOSTIC_20260821_150332.zip`
Class: PROVEN XR input.
- Quest 3 two views
- asymmetric FOV
- recommended 1504x1616
- adapter LUID `00000000:000118D1`

### MASTER1 / MASTER2 / MASTER3.1 / MASTER3.2A / MASTER3.2B
Class: **CLOSED.**
Transport, Quest projection, native origin ownership, HMD rotation and positional 6DoF are not current research questions.

## HISTORICAL TRANSPORT LINEAGE
### Exact pinned HLAE AfxInterop capability
Class: exact-source proof / historical implementation evidence.

### `CSGOVR_XR_TRANSPORT_v0.7.0_BUILD_FIRST_ONECLICK.zip`
Class: SUPERSEDED prepared CPU lineage.

### `CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF`
Class: historical prepared/static-audited fallback lineage.
SHA256 `DFEA881888C83812A5B9EE3A7049F79D32935240F2C612DE4ABDF4F9A98834A3`.

Do not reactivate old T0 GPU-vs-CPU architecture debate merely because later integration has a viewmodel failure.

## RETAINED FORENSIC RESULTS
- `CSGOVR_NATIVE_STEREO_SEAM_RESULT_20260821_041928` — one late DUAL_ORIGIN intervention negative, narrow scope only
- `CSGOVR_TTD_FRUSTUM_CACHE_REPLAY_RESULT_20260822_012630.zip` — falsified `+0x0525BA60` both-eye reuse; harvested frustum/readers
- `CSGOVR_TTD_REPLAY_FRUSTUM_READER_OWNER_3A16A6_RECOVERED_RESULT_20260823_160154.zip` — concrete reader/dataflow packet
- `CSGOVR_TTD_FRUSTUM_ARG2_PERSISTENCE_RESULT_20260823_164733.zip` — first 0x40 transpose/restore proven; persistent tail falsified
- origin/yaw control results — infrastructure anchors only, not final ownership proof

## INVALID / STOPPED
- full `CViewSetup=0x14C`
- raw Khronos column-major memcpy into Source VMatrix
- `+0x0525BA60` both-eye cache theory
- persistent arg2 tail theory
- broad projection sign/transpose rediscovery
- broad origin ownership rediscovery
- treating tearing as proof the world-camera architecture is wrong
- M34 v0.2.x as a new baseline
- combining viewmodel render, passive hand hook, controller IPC and transform mutation in one process

## Tooling / references
- `docs/STATE.md`
- `docs/NEXT.md`
- `docs/GATES.md`
- `docs/M34_FAILURE_ROLLBACK_20260825.md`
- `research/LATE_SYNC_20260825_M34_ROLLBACK.md`
- `research/RESULT_INDEX.md`

## Do not repeat rule
Before any new LIVE/build/DLL/patch/probe, read the current STATE/NEXT plus RESULT_INDEX. If the missing fact is already closed, do not reopen it. Several facts may be collected per user action, but mutation remains serial per process.
