# CSGOVR — NEXT

Snapshot: 2026-08-25 after MASTER 3.1 camera-origin PASS and MASTER 3.2A v1.0.1 pre-causal DIAG-policy invalid run `092309`.

## CLOSED GATES
MASTER 1 = PASS.
MASTER 2 = PASS.
MASTER 3.1 camera-origin ownership = PASS.

Do not repeat transport, Quest projection, or the +32 origin ownership probe.
A2 remains parked until MASTER4 PERF numerically FAILS.

## MASTER 3.1 PASS
Winning native camera seam:
- `client.dll+0x003A5280`
- full local `CViewSetup = 0x198`
- `origin +0xB8` reaches final stereo raster
- `angles +0xC4` is the current bounded control target

Cold re-analysis of `CSGOVR_MASTER31_EARLY_ORIGIN_RESULT_20260825_085119.zip` separated nuisance rotation from translation: BASE_A drift ~0.5 px, only ~1.436 deg nuisance rotation entering SHIFT (~10.9 px expected), actual shift ~114–116 px in both eyes, far-scene homography residual ~1.6–2.5 px, near geometry residual ~86–89 px. The depth-dependent residual closes origin authority positively.

## CURRENT GATE
**MASTER 3.2A — real Quest HMD ROTATION ONLY on the winning native camera seam.**

## v1.0.1 RESULT — INVALID / PRE-CAUSAL T0 DIAG POLICY
Result: `CSGOVR_MASTER32A_HMD_ROTATION_RESULT_20260825_092309.zip`
SHA256: `DE97379040CECCE21FCC7771A2497D2639BDF1F24556AEAF847AA0FD0A95C2D4`

Build succeeded. Meta OpenXR repeatedly reached `XR SESSION RUNNING`, but the runner aborted before CS:GO launch because no pre-game `layerCount=1 source=DIAG` token appeared inside the fixed wait window.

Raw status:
- `CAUSAL_ATTEMPTED=False`
- `XR_SESSION_PASS=True`
- `XR_DIAG_PASS=False`
- no GAME pair
- no injection
- no HMD pose/application telemetry

This is not HMD-rotation FAIL.

Cross-check: successful run `085119` also began with several `layerCount=0 source=DIAG` frames and only later reached the DIAG projection layer once Meta focus/visibility stabilized. MASTER1 already proved DIAG and GAME projection, so requiring DIAG to re-pass before every later MASTER3 experiment is unnecessary.

Detailed record:
`results/MASTER32A_HMD_ROTATION_20260825_092309_INVALID_T0_DIAG_POLICY.md`

## NEXT ONE STEP
Run:
`CSGOVR_MASTER3_2A_HMD_ROTATION_v1.0.2_GAME_GATE_FIRST_ONECLICK.zip`

SHA256:
`425EB626DCDBF6BA5AE1D46B25335651AAA4FB992D09EF82AE64C96474EE4F2B`

v1.0.2 is runner-policy only. C/C++ causal source is byte-identical to v1.0.1.

Changes:
- `XR SESSION RUNNING` remains mandatory;
- imported M1 DIAG is best-effort/non-blocking;
- CS:GO is launched after XR session readiness even if the pre-game DIAG token is late;
- after HLAE `record start`, runner waits up to 45 s for the actual M3.2A pipeline:
  `PAIR PROOF -> XR GAME POSE ANCHOR -> HMD_POSE_PUBLISH -> HMD_ROT_BASE_ANCHOR -> HMD_ROT_EYE LEFT+RIGHT -> ROT_LIVE_ANCHOR_POS`;
- only then does the runner ask for physical head movement;
- HMD angular proof window is 18 s.

HMD causal architecture remains unchanged:
- relative Quest quaternion `inverse(anchor) * current`;
- one pose latched on LEFT and reused by RIGHT;
- OpenXR -> Source basis conversion;
- native local `CViewSetup.angles` application at `client+0x003A5280`;
- per-eye IPD rebuilt around rotated head-right;
- caller-owned view untouched;
- host submits live orientation + anchor position after producer handshake using `ROT_LIVE_ANCHOR_POS`;
- HMD translation, hands, weapon, culling, A2, MASTER4 excluded.

## PASS
Telemetry proves pose publication, both-eye same-pair angle application, angular range, retained M1 pair proof, and GAME projection with `pose=ROT_LIVE_ANCHOR_POS`; user sees head left/right/up/down turn the world once and coherently.

## FAIL
Valid pose/application telemetry exists but final headset world does not follow coherently.

## AFTER PASS
Open M3.2B positional HMD translation only on the same seam. No broad reverse.
