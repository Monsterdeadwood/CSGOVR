# CSGOVR — NEXT

Snapshot: 2026-08-25 after invalid MASTER2 v0.8.0 runner result `CSGOVR_QUEST_COHERENCE_RESULT_20260825_051205.zip`.

## CURRENT GATE
**MASTER 2 — exact Quest 3 asymmetric matrix-only causal raster proof.**

MASTER 1 remains PASS. Do not reopen transport architecture.

The v0.8.0 MASTER2 run is **INVALID / NO SCIENTIFIC VERDICT**, not a projection FAIL:
- OpenXR diagnostic BLUE/ORANGE was visible;
- NetCon confirms `Host_NewGame on map de_dust2`;
- HLAE LEFT/RIGHT streams were created;
- runner aborted before any `mirv_streams record start` causal phase;
- `PATH_PROOF_PASS=False`, `XR_SESSION_PASS=True`, `XR_GAME_LAYER_PASS=False`.

Root cause is runner-only: PowerShell `Inject-Transport` leaked injector stdout together with the intended eye-log path, so `$eyeLog` could become `Object[]` and the path-proof stage tested the wrong object.

Detailed record: `results/MASTER2_QUEST_MATRIX_20260825_051205_INVALID.md`.

## NEXT ONE STEP
Run `CSGOVR_MASTER2_QUEST_MATRIX_v0.8.1_RUNNER_SCALAR_LOG_FIX_ONECLICK.zip` on the frozen target and return `CSGOVR_QUEST_COHERENCE_RESULT_*.zip`.

v0.8.1 is a **runner-only hotfix**:
- capture injector stdout internally;
- save `INJECTOR_OUTPUT.txt`;
- return exactly one scalar `eye_patch.log` path;
- delete stale same-PID eye log before injection;
- require a fresh literal eye-log path before runtime path-proof.

C/C++ transport, Quest matrix values, HLAE schedule, projection carrier, and causal protocol are unchanged.

## PREFLIGHT — exactly 5 lines
**MISSING FACT:** applies the exact asymmetric Quest 3 matrix to real frozen-retail CS:GO raster through `CViewSetup +0x68/+0x6C`.
**WHY NOT COLD:** static/source evidence proves carrier semantics, but only executed raster comparison can prove this exact runtime path and mutation.
**PASS:** runtime path/hash/value proof passes, same-eye repeats and BASE reset are <1% MAE, exact matrix is logged on both eyes, and QUEST raster shifts in the preregistered opposite-horizontal/downward direction.
**FAIL:** wrong file/path/hash/values, unstable instrument, BASE reset drift >=1%, missing applied-matrix log, or raster change that does not match the preregistered direction/order.
**BASELINE MUST HOLD:** frozen client SHA, HLAE 2.191.1, full `CViewSetup 0x198`, pass1 LEFT/pass2 RIGHT/pass3 monitor, `-insecure` local run, no new FOV/aspect/near/far/origin/head-pose mutation.

## EXACT MATRIX
- M00 = `0.9027383311`
- M11 = `0.8354788620`
- M12 = `-0.1931874395`
- LEFT M02 = `-0.2425126314`
- RIGHT M02 = `+0.2425126314`

## REQUIRED RUN PROTOCOL
1. Runner stages one `QUEST_MATRIX.ini`.
2. Injected DLL logs the absolute file path it actually opened, SHA256 of those actual bytes, and parsed LEFT/RIGHT M00/M02/M11/M12.
3. Runner compares DLL path/hash/values to its staged copy. Any mismatch is HARD FAIL before causal capture.
4. Run one continuous symmetric sequence: `BASE_A -> QUEST_MATRIX -> BASE_B(reset)`.
5. Select fixed N=2 LEFT and N=2 RIGHT captures per phase.
6. Validate same-eye repeat first. If >=1% normalized MAE, the instrument is invalid and no semantic verdict is allowed.
7. BASE_A vs BASE_B must converge <1% MAE.
8. QUEST must causally change raster in both eyes.
9. Final semantic review must match the preregistered direction: horizontal off-center opposite sign LEFT/RIGHT, vertical displacement downward in both eyes, correct order of magnitude.

## EXPLICIT EXCLUSIONS
- no FOV variant
- no aspect variant
- no near/far variant
- no new origin / HMD pose mutation
- no culling
- no weapon/HUD integration
- no A2 GPU-sharing
- no MASTER 3 or MASTER 4 work

## A2 POLICY
GPU-sharing A2 is parked. It may open only if the later MASTER 4 120-second CPU performance gate numerically FAILS.

## AFTER MASTER 2 PASS
Only then open MASTER 3, starting with the bounded early native `CViewSetup +0xB8` origin ownership probe at `client.dll +0x003A5280`. No broad reverse.
