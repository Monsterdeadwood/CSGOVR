# CSGOVR — NEXT

Snapshot: 2026-08-25 after MASTER 3.1 v0.9.1 invalid HLAE vtable-guard run `CSGOVR_MASTER31_EARLY_ORIGIN_RESULT_20260825_070439.zip`.

## CURRENT GATE
**MASTER 3.1 — one bounded early native origin ownership probe.**

MASTER 1 = PASS.
MASTER 2 = PASS.
Do not repeat transport or Quest projection.
Do not open MASTER 3.2 until 3.1 runtime causal PASS.
A2 remains parked until MASTER4 PERF numerically FAILS.

## INVALID v0.9.1 RESULT
v0.9.1 built successfully and reached injection, but aborted before any causal origin mutation.

Passed before abort:
- frozen client hash
- exact HLAE 2.191.1
- x86 DLL/injector + x64 OpenXR host build
- OpenXR diagnostic layer
- de_dust2 load
- exact `-insecure`
- exact Quest matrix runtime path/hash/value proof

Blocking runtime observation:
- native RenderView target = `client+0x003A5280`
- `CViewRender.slot6` remained native target
- `CCSViewRender.slot6 = AfxHookSource+0x0009A160`

v0.9.1 incorrectly required both slot6 values to remain native after HLAE launch. Exact HLAE source proves HLAE intentionally detours the CCSViewRender RenderView vtable through `AfxDetourPtr` / `touring_CCSViewRender_RenderView`.

Scientific verdict: **INVALID / guard incompatibility**, not origin FAIL. No `EARLY_ORIGIN_APPLY` or causal raster existed.

Detailed record: `results/MASTER31_EARLY_ORIGIN_20260825_070439_INVALID_HLAE_VTABLE_GUARD.md`.

## NEXT ONE STEP
Run `CSGOVR_MASTER3_1_EARLY_ORIGIN_v0.9.2_HLAE_VTABLE_GUARD_FIX_ONECLICK.zip`.
SHA256 `7FBA01E24138A56EB6596ED0BA8DADCE692DE12B2FDF85D01EB64286E4B964F8`.

v0.9.2 changes only the guard:
1. require `CViewRender.slot6 == client+0x003A5280`;
2. require `CCSViewRender.slot6 == exact AfxHookSource+0x0009A160`;
3. retain exact AfxHookSource SHA256 gate in the runner.

## CAUSAL CONTRACT UNCHANGED
Only one variable changes:
`CViewSetup.origin +0xB8` at native `client.dll+0x003A5280` by `+32 Source units` along camera RIGHT.

Retain:
- full local `CViewSetup 0x198` copy;
- caller-owned view untouched;
- exact Quest matrix continuously enabled;
- M1 same-HLAE L/R pair transport;
- one continuous `BASE_A -> ORIGIN_SHIFT -> BASE_B` live stream;
- pass3 untouched;
- `-insecure`, local/offline.

Excluded:
- HMD pose / xrLocateViews camera application;
- angle mutation;
- hands / weapon / muzzle;
- culling;
- A2 GPU sharing;
- MASTER 4.

## PASS
Shift is logged at the native early RenderView for both eyes and the final same-eye raster moves in the preregistered direction, with BASE reset returning coherently.

## FAIL
A valid instrument proves the early native origin mutation was applied but the final raster does not respond in the expected way.

## AFTER PASS
Open MASTER 3.2 and feed real Quest HMD pose into the winning seam. No broad reverse.
