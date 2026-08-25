# CSGOVR — NEXT

Snapshot: 2026-08-25 after MASTER 3.1 camera-origin PASS and MASTER 3.2A v1.0.0 build-invalid attempt.

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

## v1.0.0 RESULT — INVALID / BUILD INFRASTRUCTURE
The first M3.2A live attempt never reached runtime.

Passed:
- frozen target/HLAE preflight
- Quest matrix staging/hash
- M3.2A source contract

Build failed in `xr_host.cpp`:

```text
xr_host.cpp:154:9: error: use of undeclared identifier 'log_line'
```

Root cause: `publish_relative_hmd_orientation()` called `log_line()` before its declaration.

This is not HMD-rotation FAIL. Detailed record:
`results/MASTER32A_HMD_ROTATION_20260825_091002_INVALID_BUILD_LOG_LINE.md`

## NEXT ONE STEP
Run:
`CSGOVR_MASTER3_2A_HMD_ROTATION_v1.0.1_COMPILE_ORDER_FIX_ONECLICK.zip`

SHA256:
`0FFCE550F64460571BAEEDEC320364CC4DAA652F154A1C5E5D7C20FA503967F3`

v1.0.1 changes compile order only:
- adds `static void log_line(const char *fmt, ...);` before the HMD pose publisher;
- cleans runner labels;
- removes the ambiguous legacy MASTER31 BAT;
- cold static audit reports zero remaining project-defined use-before-declaration issues in `xr_host.cpp` and `eye_transport.c`.

HMD causal architecture is unchanged:
- relative Quest quaternion `inverse(anchor) * current`;
- one pose latched on LEFT and reused by RIGHT;
- OpenXR -> Source basis conversion;
- native local `CViewSetup.angles` application at `client+0x003A5280`;
- per-eye IPD rebuilt around rotated head-right;
- caller-owned view untouched;
- after producer handshake host submits live orientation + anchor position using `ROT_LIVE_ANCHOR_POS`;
- HMD translation, hands, weapon, culling, A2, MASTER4 excluded.

## PASS
Telemetry proves pose publication, both-eye same-pair angle application, angular range, retained M1 pair proof, and GAME projection with `pose=ROT_LIVE_ANCHOR_POS`; user sees head left/right/up/down turn the world once and coherently.

## FAIL
Valid pose/application telemetry exists but final headset world does not follow coherently.

## AFTER PASS
Open M3.2B positional HMD translation only on the same seam. No broad reverse.
