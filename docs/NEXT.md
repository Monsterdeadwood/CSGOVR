# CSGOVR — NEXT

Snapshot: 2026-08-25 after MASTER 3.1 v0.9.0 compile-invalid run `CSGOVR_MASTER31_EARLY_ORIGIN_RESULT_20260825_060743.zip`.

## CURRENT GATE
**MASTER 3.1 — one bounded early native origin ownership probe.**

MASTER 1 = PASS.
MASTER 2 = PASS.
Do not repeat transport or Quest projection.
Do not open MASTER 3.2 until 3.1 runtime causal PASS.
A2 remains parked until MASTER4 PERF numerically FAILS.

## INVALID v0.9.0 RESULT
The run stopped in BUILD X86 before game launch:
`eye_transport.c:838:33: error: call to undeclared function 'apply_quest_matrix_only'`.

Scientific verdict: **INVALID / BUILD INFRASTRUCTURE FAILURE**, not origin FAIL.
The subsequent missing runtime logs/captures are expected because no instrument was built.

Root cause: the native-origin merge retained calls to the M2 matrix helpers but accidentally omitted the working M2 v0.8.1 helper implementations, including `load_quest_matrix()` and `apply_quest_matrix_only()`.

Runner defect also fixed: v0.9.0 could print `ExitCode=0` after a caught infrastructure failure.

## NEXT ONE STEP
Run `CSGOVR_MASTER3_1_EARLY_ORIGIN_v0.9.1_COMPILE_GATE_FIX_ONECLICK.zip`.
SHA256 `9325F814B7DBE5EE0CD97E7D4F5D6DABA52676124A68285DE4B2FC9078B9A1EB`.

v0.9.1 infrastructure delta only:
- exact M2 v0.8.1 matrix parse/SHA256/apply block restored;
- source gate requires helper definitions before build;
- infrastructure failures propagate nonzero exit code;
- analyzer does not run after compile failure;
- BOM-free BAT;
- static self-verification 30/30 PASS.

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
