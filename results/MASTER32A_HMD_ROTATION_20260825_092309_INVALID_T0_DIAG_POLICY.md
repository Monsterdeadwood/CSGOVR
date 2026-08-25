# MASTER 3.2A HMD ROTATION — 2026-08-25 09:23:09

Result: `CSGOVR_MASTER32A_HMD_ROTATION_RESULT_20260825_092309.zip`

SHA256: `DE97379040CECCE21FCC7771A2497D2639BDF1F24556AEAF847AA0FD0A95C2D4`

## VERDICT

**INVALID / PRE-CAUSAL T0 DIAG POLICY**

This is not an HMD-rotation FAIL. The build succeeded, the Meta OpenXR session reached RUNNING, but the runner aborted before launching CS:GO because the optional pre-game diagnostic projection layer did not appear inside its fixed wait window.

## RAW EVIDENCE

`RESULT_STATUS.txt`:
- `MASTER32A_VERDICT=INVALID_PRE_CAUSAL`
- `CAUSAL_ATTEMPTED=False`
- `XR_SESSION_PASS=True`
- `XR_DIAG_PASS=False`
- `QUEST_MATRIX_PATH_PROOF_PASS=False`
- `NATIVE_RENDERVIEW_GUARD_PASS=False`
- `HMD_ROTATION_APPLY_PASS=False`

`XR_HOST.log`:
- x64 host banner: `CSGOVR XR HOST MASTER3.2A v1.0.1 ROTATION`
- shared-memory ABI ready: `header=332`
- XR instance, D3D11 device, stereo view config, swapchain format all ready
- `XR SESSION RUNNING` occurred repeatedly
- initial diagnostic frames were `layerCount=0 source=DIAG pose=NONE`
- Meta session cycled through visibility/focus/stop states before any logged DIAG layerCount=1
- no GAME pair existed because CS:GO was never launched

## CROSS-CHECK AGAINST SUCCESSFUL 085119 RUN

The earlier successful M3.1 host also began with several `layerCount=0 source=DIAG` frames after `XR SESSION RUNNING`, and only later reached `layerCount=1 source=DIAG` once Meta state became sufficiently visible/focused. Therefore the 092309 abort reflects a brittle pre-game timing gate, not a regression in the M1 transport or proof against M3.2A.

MASTER1 is already PASS, including real GAME projection and DIAG behavior. Requiring DIAG to re-pass before every later MASTER 3 experiment does not reduce the active uncertainty.

## ADAPT

Prepared v1.0.2 runner-only gate fix:
`CSGOVR_MASTER3_2A_HMD_ROTATION_v1.0.2_GAME_GATE_FIRST_ONECLICK.zip`

SHA256:
`425EB626DCDBF6BA5AE1D46B25335651AAA4FB992D09EF82AE64C96474EE4F2B`

v1.0.2 keeps `XR SESSION RUNNING` mandatory but makes the imported M1 DIAG best-effort/non-blocking. It proceeds to CS:GO and, after HLAE recording starts, waits for the actual M3.2A pipeline:

`PAIR PROOF -> XR GAME POSE ANCHOR -> HMD_POSE_PUBLISH -> HMD_ROT_BASE_ANCHOR -> HMD_ROT_EYE LEFT+RIGHT -> ROT_LIVE_ANCHOR_POS`

Only after that pipeline is present does the runner ask the user to physically turn the headset.

The C/C++ causal source is byte-identical to v1.0.1. No quaternion, native camera seam, IPD, Quest matrix, or OpenXR pose-policy code changed.
