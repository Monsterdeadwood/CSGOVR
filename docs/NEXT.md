# CSGOVR — NEXT

Snapshot: 2026-08-25 after `CSGOVR_XR_TRANSPORT_RESULT_20260825_044816.zip`.

## CURRENT GATE
**MASTER 2 — exact Quest 3 asymmetric matrix-only causal raster proof.**

MASTER 1 is PASS. The live CPU pair transport reached Meta OpenXR with coherent distinct LEFT/RIGHT pairs. Do not reopen transport architecture now.

## NEXT ONE STEP
Run `CSGOVR_MASTER2_QUEST_MATRIX_v0.8.0_ONECLICK.zip` on the frozen target and return `CSGOVR_QUEST_COHERENCE_RESULT_*.zip`.

The package is **PREPARED**, not runtime-proven.

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
2. Injected DLL must log the absolute file path it actually opened, SHA256 of those actual bytes, and parsed LEFT/RIGHT M00/M02/M11/M12.
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
GPU-sharing A2 is parked. It may open only if the later MASTER 4 120-second CPU performance gate numerically FAILS. It is not a transport prerequisite anymore.

## AFTER MASTER 2 PASS
Only then open MASTER 3, starting with the bounded early native `CViewSetup +0xB8` origin ownership probe at `client.dll + 0x003A5280`. No broad reverse.
