# CSGOVR — STATE SYNC

Snapshot: 2026-08-25 after MASTER 1 PASS, MASTER 2 PASS, and MASTER 3.1 camera-origin authority PASS from live run `CSGOVR_MASTER31_EARLY_ORIGIN_RESULT_20260825_085119.zip` plus cold raster re-analysis.

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

Runtime proof:
- XR session/game layer active;
- exact Quest matrix path/hash proof PASS;
- exact native RenderView guard PASS at `client.dll+0x003A5280`;
- local full `CViewSetup 0x198`, caller-owned write=0;
- `EARLY_ORIGIN_APPLY` LEFT and RIGHT applied exact +32 camera-right translation with tiny vector error;
- same-HLAE L/R pair proof retained, drops=0.

The bundled analyzer reported FAIL/INVALID because BASE_B did not visually reset to BASE_A and SHIFT/BASE_B same-eye captures were unstable. Cold log+raster review found the cause: the camera itself rotated materially during the causal window. This contaminates the reset-control metric but does not erase the causal translation evidence.

Cold image evidence:
- BASE_A A0->A1 median feature drift about -0.5 px for both eyes;
- camera-right vector entering SHIFT changes only ~1.436 deg;
- rotational contribution predicted from Quest M00 and 960 px width is ~10.9 px;
- actual BASE_A->SHIFT motion is ~-114 to -116 px in both eyes;
- far-only homography explains far features to ~1.6-2.5 px residual;
- near geometry retains ~-86 to -89 px residual after that far-scene rotation/homography compensation.

This is depth-dependent parallax in the preregistered direction. Therefore `client.dll+0x003A5280` with `CViewSetup.origin +0xB8` is retained as the winning early native camera seam. Do not repeat M3.1.

## PREPARED MASTER 3.2A — HMD ROTATION ONLY
Package:
`CSGOVR_MASTER3_2A_HMD_ROTATION_v1.0.0_LIVE_ONECLICK.zip`
SHA256 `C6E924E27C86E853960EBFB5498E8252BBED2DBE9A878660ABF237F32A9F84CE`.
Static verification: 25/25 PASS.

Single new variable: relative Quest HMD orientation.

Architecture:
- x64 host publishes relative OpenXR HMD quaternion through unused shared-header reserved fields, preserving M1 ABI size 332;
- first gameplay pose is anchor;
- x86 eye DLL latches one pose for LEFT+RIGHT;
- OpenXR-to-Source basis mapping: Source X=-XR Z, Source Y=-XR X, Source Z=+XR Y;
- native local `CViewSetup` angles are composed from frozen base game camera + relative HMD rotation;
- per-eye origin is recentered and IPD re-applied around the newly rotated head-right vector;
- producer signals engine rotation active;
- host then submits live eye ORIENTATION with anchor eye POSITION, avoiding double rotation while keeping translation intentionally disabled;
- exact Quest matrix and M1 transport remain retained;
- pass3 remains untouched.

Excluded:
- HMD translation;
- hands/controllers;
- weapon/aim/muzzle;
- culling;
- A2/GPU replay;
- MASTER4.

## CURRENT GATE
**MASTER 3.2A — Quest HMD rotation-only proof.**

## NEXT
Run `00_RUN_MASTER32A_HMD_ROTATION.bat`. During the short LIVE window, wear Quest and turn the HEAD left/right then up/down, without moving the mouse. Return `CSGOVR_MASTER32A_HMD_ROTATION_RESULT_*.zip` plus the headset observation. Only coherent rotation PASS opens M3.2B translation-only.
