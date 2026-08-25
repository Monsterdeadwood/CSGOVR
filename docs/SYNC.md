# CSGOVR — STATE SYNC

Snapshot: 2026-08-25 after MASTER 1 PASS, MASTER 2 PASS, MASTER 3.1 camera-origin PASS, and MASTER 3.2A v1.0.1 pre-causal DIAG-policy invalid result.

## AUTHORITY / CURRENT PROGRAM
Execution order remains strict:
`MASTER 1 -> MASTER 2 -> MASTER 3 -> MASTER 4`.
Prepared packages are not runtime proof. Raw runtime result > generated status summary > plan/README.

Frozen baseline:
- legacy CS:GO x86, AppID `4465480`, BuildID `22100934`
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- HLAE 2.191.1 exact
- v0.5.5 schedule: pass1 LEFT / pass2 RIGHT / pass3 monitor
- full `CViewSetup = 0x198`
- `-insecure`, local/offline only

## MASTER 1 — PASS
Retain complete-pair CPU transport to Meta OpenXR. Do not reopen. M1 already proved both DIAG projection and real GAME projection.

## MASTER 2 — PASS
Retain exact Quest asymmetric projection through `CViewSetup +0x68/+0x6C`. Do not rerun.

## MASTER 3.1 — PASS
Live result `CSGOVR_MASTER31_EARLY_ORIGIN_RESULT_20260825_085119.zip` plus cold raster re-analysis proved native camera-origin authority at `client.dll+0x003A5280` using a full local `CViewSetup`. The contaminated BASE_B reset metric does not overturn the depth-dependent parallax proof.

Winning seam:
- native RenderView `client+0x003A5280`
- local `CViewSetup 0x198`
- `origin +0xB8` proven surviving
- `angles +0xC4` current bounded control target

Do not repeat M3.1.

## MASTER 3.2A v1.0.0 — INVALID / BUILD INFRASTRUCTURE
The first HMD-rotation attempt failed compiling the x64 host because `publish_relative_hmd_orientation()` called `log_line()` before declaration. No runtime verdict.

## MASTER 3.2A v1.0.1 — INVALID / PRE-CAUSAL T0 DIAG POLICY
Result:
`CSGOVR_MASTER32A_HMD_ROTATION_RESULT_20260825_092309.zip`
SHA256 `DE97379040CECCE21FCC7771A2497D2639BDF1F24556AEAF847AA0FD0A95C2D4`.

What passed:
- frozen target/HLAE/source-contract preflight
- Quest matrix staging/hash
- x86 DLL/injector build
- x64 host build
- shared-memory ABI `332`
- XR instance and D3D11 device
- Quest stereo view config and swapchain-format setup
- Meta OpenXR reached `XR SESSION RUNNING` repeatedly

What did not occur:
- CS:GO launch
- injection
- GAME pair
- HMD pose publication/application
- M3.2A causal live

The runner aborted because the optional pre-game DIAG projection layer did not appear during its fixed wait window. Raw initial frames were `layerCount=0 source=DIAG pose=NONE`, with Meta state cycling through visibility/focus/stop states.

Cross-check against successful `085119`: that proven host also began with several `layerCount=0 source=DIAG` frames, then later reached `layerCount=1 source=DIAG` once Meta focus stabilized. Therefore 092309 is a runner-policy timing invalid, not evidence against the HMD rotation path.

Detailed record:
`results/MASTER32A_HMD_ROTATION_20260825_092309_INVALID_T0_DIAG_POLICY.md`.

## PREPARED MASTER 3.2A v1.0.2 — GAME GATE FIRST
Package:
`CSGOVR_MASTER3_2A_HMD_ROTATION_v1.0.2_GAME_GATE_FIRST_ONECLICK.zip`
SHA256 `425EB626DCDBF6BA5AE1D46B25335651AAA4FB992D09EF82AE64C96474EE4F2B`.

C/C++ causal source is byte-identical to v1.0.1. Only runner policy changes:
- XR SESSION RUNNING remains mandatory;
- M1 DIAG becomes best-effort/non-blocking;
- proceed to CS:GO after session readiness;
- after HLAE recording starts, wait for actual M3.2A pipeline readiness:
  `PAIR PROOF -> XR GAME POSE ANCHOR -> HMD_POSE_PUBLISH -> HMD_ROT_BASE_ANCHOR -> HMD_ROT_EYE LEFT+RIGHT -> ROT_LIVE_ANCHOR_POS`;
- only after pipeline readiness prompt physical head rotation;
- 18 s angular proof window.

Causal architecture remains:
- host relative quaternion `inverse(anchor) * current`;
- one pose latched LEFT and reused RIGHT;
- OpenXR-to-Source basis conversion;
- local `CViewSetup.angles` at native RenderView;
- rotated per-eye IPD around head-right;
- live OpenXR orientation with anchored position after producer handshake;
- Quest matrix and M1 transport retained;
- pass3 untouched.

Excluded:
HMD translation, hands, weapon/aim/muzzle, culling, A2/GPU replay, MASTER4.

## CURRENT GATE
**MASTER 3.2A — Quest HMD rotation-only proof.**

## NEXT
Run only `00_RUN_MASTER32A_HMD_ROTATION.bat` from v1.0.2. Do not move the mouse when the runner explicitly says `PIPELINE READY`; then turn the headset left/right and up/down. Return the result ZIP plus headset observation. Only coherent rotation PASS opens M3.2B translation-only.
