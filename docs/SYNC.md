# CSGOVR — STATE SYNC

Snapshot: 2026-08-25 after live MASTER 1 PASS and invalid MASTER 2 v0.8.0 runner result.

## AUTHORITY / CURRENT PROGRAM
Current execution order is strict:
`MASTER 1 -> MASTER 2 -> MASTER 3 -> MASTER 4`.
A prepared package is not runtime proof. Raw runtime result > generated status summary > plan/README.

Frozen baseline:
- legacy CS:GO x86, AppID `4465480`, BuildID `22100934`
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- HLAE 2.191.1
- v0.5.5 schedule: pass1 LEFT / pass2 RIGHT / pass3 monitor
- full `CViewSetup = 0x198`
- `-insecure`, local/offline only

## MASTER 1 — PASS
Accepted result: `CSGOVR_XR_TRANSPORT_RESULT_20260825_044816.zip`
SHA256 `FE51FA49B0108FC31E70E4D2F16C25F9BD160CDA7B654841AD64E19B61FB99E7`

Raw runtime evidence:
- `PAIR PROOF PASS id=1 gen=1 pairProof=1 equal=0`
- 123 produced / 123 consumed complete pairs
- drops=0, captureFailures=0
- OpenXR `layerCount=1 source=GAME`
- same-HLAE outer proof `0->1 / 1->2`
- LEFT/RIGHT hashes differ
- first saved pair 960x540
- first-pair timing: leftCaptureUs=4459, rightCaptureUs=2451, consumeAgeMs=27.518, eyeGapMs=13.498
- headset observation: a strange CS:GO image was visible in VR briefly

The result runner's earlier `No complete LEFT/RIGHT pair` summary is a timing/polling false-negative. Later raw `XR_HOST.log` evidence supersedes it.

Detailed record: `results/MASTER1_XR_TRANSPORT_20260825_044816.md`.

## MASTER 2 v0.8.0 — INVALID / NO SCIENTIFIC VERDICT
Result: `CSGOVR_QUEST_COHERENCE_RESULT_20260825_051205.zip`.

Observed:
- Quest remained on BLUE LEFT / ORANGE RIGHT diagnostic;
- NetCon confirms `Host_NewGame on map de_dust2`;
- HLAE streams `CSGOVRLEFT` / `CSGOVRRIGHT` were created;
- causal capture never reached `mirv_streams record start`;
- `PATH_PROOF_PASS=False`, `XR_SESSION_PASS=True`, `XR_GAME_LAYER_PASS=False`.

This is not evidence against the Quest matrix. The run aborted in runner plumbing before the causal phase.

Root cause:
PowerShell `Inject-Transport` leaked injector stdout together with the intended `eye_patch.log` return value, allowing `$eyeLog` to become `Object[]`. The next path-proof stage therefore tested the wrong object/path. The same mechanism explains why the earlier MASTER1 runner summary could false-fail while raw host telemetry later proved coherent pairs.

Detailed record: `results/MASTER2_QUEST_MATRIX_20260825_051205_INVALID.md`.

Prepared retry:
`CSGOVR_MASTER2_QUEST_MATRIX_v0.8.1_RUNNER_SCALAR_LOG_FIX_ONECLICK.zip`
- runner-only fix;
- captures injector stdout to `INJECTOR_OUTPUT.txt`;
- returns exactly one scalar eye-log path;
- deletes stale same-PID eye log before injection;
- requires a fresh literal eye-log path before path proof;
- C/C++ transport, exact matrix, HLAE schedule, projection carrier and causal protocol unchanged.

## PROVEN / RETAIN
- two real HLAE world-eye streams
- exact Quest 3 asymmetric FOV acquisition
- custom projection carrier `CViewSetup +0x68 / +0x6C`
- retail semantics for `M00/M02/M11/M12`
- HLAE post-eye DoRenderView capture boundary
- CPU D3D9 readback / complete-pair shared-ring transport LIVE-proven end-to-end to Meta OpenXR for MASTER 1
- HLAE DoRenderView `+0x22340`, copy ctor `+0x14BA0`
- native RenderView `client+0x3A5280`
- pre-eye reader `client+0x3A16A6`

## NOT YET PROVEN
- exact Quest matrix end-to-end causal raster direction on the current transport
- final eye-origin / HMD 6DoF owner
- controller-owned hands/weapon/muzzle separation
- MASTER 4 performance/comfort/release gates

## CURRENT GATE
**MASTER 2 — exact Quest 3 asymmetric matrix-only causal raster proof.**

Exact causal matrix:
- M00 = `0.9027383311`
- M11 = `0.8354788620`
- M12 = `-0.1931874395`
- LEFT M02 = `-0.2425126314`
- RIGHT M02 = `+0.2425126314`

Required protocol:
1. runtime proof of the actual opened matrix path + SHA256 + parsed values;
2. valid same-eye repeats (<1% MAE);
3. one causal variable only: exact matrix, no FOV/aspect/near/far variants;
4. symmetric `BASE_A -> QUEST_MATRIX -> BASE_B(reset)`;
5. causal raster review must match opposite horizontal L/R off-center and downward vertical displacement.

## FALSIFIED / DO NOT REOPEN
- full CViewSetup 0x14C
- raw Khronos column-major memcpy into Source VMatrix
- `+0x0525BA60` both-eye reuse
- Present as eye oracle
- broad Quest FOV rediscovery
- broad HLAE helper/PDB reverse
- culling reverse without a visible one-eye blocker
- static package/build as runtime proof

## CURRENT POLICY FOR A2
A2 GPU-sharing is **not** the current gate. Under the current MASTER 4 contract, A2 opens only if the later 120-second CPU performance gate numerically FAILS.

## NEXT
Run MASTER2 v0.8.1 on the frozen target, return its RESULT ZIP, issue one PASS/FAIL causal verdict, and only then consider MASTER 3.
