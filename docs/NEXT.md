# CSGOVR — NEXT

Snapshot: 2026-08-25 after fresh live `CSGOVR_MASTER31_EARLY_ORIGIN_RESULT_20260825_085119.zip` and cold causal re-analysis.

## CLOSED GATES
MASTER 1 = PASS.
MASTER 2 = PASS.
MASTER 3.1 camera-origin ownership = PASS.

Do not repeat transport, Quest projection, or the +32 origin ownership probe.
A2 remains parked until MASTER4 PERF numerically FAILS.

## MASTER 3.1 PASS — WHY THE PACKAGED AUTO-ANALYZER WAS TOO CONSERVATIVE
The live run reached the actual causal section and proved:
- exact frozen client/HLAE guards PASS;
- exact Quest matrix path/hash PASS;
- native RenderView guard PASS at `client.dll+0x003A5280`;
- local full `CViewSetup 0x198`, caller-owned view untouched;
- `+32 Source units` camera-RIGHT origin mutation applied to LEFT and RIGHT with vector error about 3e-5 / 2e-5;
- M1 distinct same-HLAE pairs and GAME OpenXR layer remained live.

The bundled analyzer marked the result INVALID because the user/game camera rotated substantially during SHIFT/RESET, so BASE_B could not match BASE_A. That invalidates the reset-control metric, not the origin carrier.

Cold image re-analysis separates nuisance rotation from translation:
- BASE_A same-eye median feature drift is about -0.5 px;
- camera-right vector changes only about 1.436 degrees entering SHIFT;
- with Quest M00=0.902738 at 960 px width, that angular change accounts for only about 10.9 px of rotational motion;
- observed BASE_A -> SHIFT motion is about -114 to -116 px in BOTH eyes;
- a homography fitted only to far-scene features leaves far residual about 1.6-2.5 px but near geometry retains about -86 to -89 px residual.

That strong depth-dependent residual is the preregistered signature of true camera translation. Therefore the missing fact is closed positively: native early RenderView origin survives to final stereo raster.

## CURRENT GATE
**MASTER 3.2A — real Quest HMD ROTATION ONLY on the winning native camera seam.**

Prepared package:
`CSGOVR_MASTER3_2A_HMD_ROTATION_v1.0.0_LIVE_ONECLICK.zip`
SHA256 `C6E924E27C86E853960EBFB5498E8252BBED2DBE9A878660ABF237F32A9F84CE`.
Static verification: 25/25 PASS.

## M3.2A SINGLE CHANGE
Feed relative Quest HMD orientation into native `client.dll+0x003A5280` local `CViewSetup`.

Implementation contract:
- x64 host publishes `inverse(anchor) * current` HMD quaternion through unused `CSGOVR_XR_HEADER.reserved[]`, preserving the 332-byte M1 shared-memory ABI;
- one pose is latched on LEFT and reused by RIGHT;
- OpenXR -> Source basis: Source X=-XR Z, Source Y=-XR X, Source Z=+XR Y;
- native hook composes HMD orientation onto a frozen base game camera orientation;
- per-eye IPD is rebuilt around the newly rotated head-right vector;
- after engine rotation handshake, OpenXR submits LIVE eye orientation with ANCHOR eye position to avoid double rotation while deliberately excluding HMD translation;
- exact Quest asymmetric projection remains continuously enabled;
- pass3 monitor remains untouched;
- local/offline `-insecure` only.

Excluded from M3.2A:
- HMD positional translation;
- hands/controllers;
- weapon/aim/muzzle;
- culling fixes;
- GPU fast path/A2;
- MASTER4.

## PASS
Telemetry proves both-eye native HMD rotation, sufficient angular range, retained M1 pair proof, and `XR pose=ROT_LIVE_ANCHOR_POS`; user confirms turning head left/right/up/down turns the world once and coherently, without doubled rotation.

## FAIL
Valid HMD pose and native angle-application telemetry exists but the final headset world does not follow coherently.

## AFTER PASS
Open M3.2B positional HMD translation only on the same seam. Do not add hands or weapon until head 6DoF is closed.
