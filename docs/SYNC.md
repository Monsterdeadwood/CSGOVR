# CSGOVR — STATE SYNC

Snapshot: 2026-08-25 after MASTER 1 PASS, MASTER 2 PASS, MASTER 3.1 camera-origin authority PASS, and the first MASTER 3.2A HMD-rotation build attempt.

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
Retain CPU D3D9 complete-pair transport to Meta OpenXR. Do not reopen.

## MASTER 2 — PASS
Retain exact Quest 3 asymmetric projection through `CViewSetup +0x68/+0x6C`. Do not rerun.

## MASTER 3.1 — PASS: NATIVE ORIGIN AUTHORITY
Live result: `CSGOVR_MASTER31_EARLY_ORIGIN_RESULT_20260825_085119.zip`.

Runtime + cold raster proof established that `client.dll+0x003A5280` with a local full `CViewSetup` is a surviving camera seam for origin. The old automatic BASE_B reset metric was contaminated by a later camera rotation, but depth-dependent parallax remained decisive: far-scene compensation reduces far residual to ~1.6–2.5 px while near geometry retains ~86–89 px extra displacement in the preregistered direction.

Do not repeat M3.1.

## MASTER 3.2A v1.0.0 — INVALID / BUILD INFRASTRUCTURE
The first HMD-rotation attempt passed target/HLAE/matrix/source-contract preflight but failed while compiling the x64 OpenXR host.

Exact compiler error:

```text
xr_host.cpp:154:9: error: use of undeclared identifier 'log_line'
```

Root cause: the new `publish_relative_hmd_orientation()` helper called `log_line()` before the logger declaration/definition.

No HMD runtime work occurred. This is **not** HMD rotation FAIL.

Detailed record:
`results/MASTER32A_HMD_ROTATION_20260825_091002_INVALID_BUILD_LOG_LINE.md`

## PREPARED MASTER 3.2A v1.0.1 — COMPILE ORDER FIX
Package:
`CSGOVR_MASTER3_2A_HMD_ROTATION_v1.0.1_COMPILE_ORDER_FIX_ONECLICK.zip`

SHA256:
`0FFCE550F64460571BAEEDEC320364CC4DAA652F154A1C5E5D7C20FA503967F3`

Delta from v1.0.0 is infrastructure only:
- forward-declare `log_line()` before the pose publisher;
- clean runner labels;
- remove ambiguous legacy `00_RUN_MASTER31.bat`;
- project-defined use-before-declaration audit: zero remaining issues in `xr_host.cpp` and `eye_transport.c`;
- static verification: 13/13 PASS.

The M3.2A causal design is unchanged:
- x64 host publishes relative Quest HMD quaternion in unused shared-header reserved fields, keeping the M1 332-byte ABI;
- one pose latched on LEFT and reused by RIGHT;
- OpenXR-to-Source basis mapping retained;
- native local `CViewSetup.angles` composed at `client.dll+0x003A5280`;
- per-eye origin recentered and IPD re-applied around rotated head-right;
- producer handshake enables host `ROT_LIVE_ANCHOR_POS` submission: live orientation with anchored position;
- Quest matrix and M1 pair transport retained;
- pass3 untouched.

Excluded:
- HMD positional translation;
- hands/controllers;
- weapon/aim/muzzle;
- culling;
- A2/GPU replay;
- MASTER4.

## CURRENT GATE
**MASTER 3.2A — Quest HMD rotation-only proof.**

## NEXT
Run only `00_RUN_MASTER32A_HMD_ROTATION.bat` from v1.0.1. During LIVE, do not move the mouse; turn the headset left/right and up/down. Return `CSGOVR_MASTER32A_HMD_ROTATION_RESULT_*.zip` plus headset observation. Only coherent rotation PASS opens M3.2B translation-only.
