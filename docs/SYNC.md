# CSGOVR — STATE SYNC

Snapshot: 2026-08-25 after live MASTER 1 PASS, MASTER 2 PASS, and MASTER 3.1 v0.9.0 compile-invalid result.

## AUTHORITY / CURRENT PROGRAM
Execution order remains strict:
`MASTER 1 -> MASTER 2 -> MASTER 3 -> MASTER 4`.
Prepared packages are not runtime proof. Raw runtime result > generated status summary > plan/README.

Frozen baseline:
- legacy CS:GO x86, AppID `4465480`, BuildID `22100934`
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- HLAE 2.191.1
- v0.5.5 schedule: pass1 LEFT / pass2 RIGHT / pass3 monitor
- full `CViewSetup = 0x198`
- `-insecure`, local/offline only

## MASTER 1 — PASS
Accepted result: `CSGOVR_XR_TRANSPORT_RESULT_20260825_044816.zip`.
Retain CPU D3D9 readback complete-pair transport to Meta OpenXR as LIVE-proven. Do not reopen transport architecture.

## MASTER 2 — PASS
Accepted executed causal result: `CSGOVR_QUEST_COHERENCE_RESULT_20260825_054023.zip`.
Retain exact Quest 3 asymmetric projection through `CViewSetup +0x68/+0x6C`. The v0.8.1 runner UX exposed only three burst-capture states in the headset, but the causal raster result itself closed the matrix carrier. Do not repeat MASTER 2.

## MASTER 3.1 v0.9.0 — INVALID / BUILD INFRASTRUCTURE ONLY
Result: `CSGOVR_MASTER31_EARLY_ORIGIN_RESULT_20260825_060743.zip`.

Build failed before game/runtime causal work:
`eye_transport.c:838:33: error: call to undeclared function 'apply_quest_matrix_only'`.

This run proves nothing about early origin ownership. The analyzer's missing runtime artifacts are downstream consequences of the compile abort, not semantic FAILs.

Root cause: when the native early-origin layer was merged onto the M1/M2 transport source, the working MASTER2 v0.8.1 matrix helper block was accidentally omitted. Both `load_quest_matrix()` and `apply_quest_matrix_only()` implementations were absent even though their calls remained.

Also corrected: the runner previously returned `ExitCode=0` after infrastructure exceptions because catch/finally collected a RESULT but never propagated failure status.

## PREPARED RETRY
`CSGOVR_MASTER3_1_EARLY_ORIGIN_v0.9.1_COMPILE_GATE_FIX_ONECLICK.zip`

v0.9.1 changes infrastructure only:
- restores the exact M2 v0.8.1 `read_ini_float` / SHA256 / `load_quest_matrix` / `apply_quest_matrix_only` block;
- source-contract now requires those function definitions before build;
- matrix-load abort returns nonzero inside the DLL worker;
- build/runtime exception returns runner `ExitCode=1`;
- analyzer is skipped after a build failure;
- BAT is BOM-free;
- package static self-verification: 30/30 PASS.

Causal experiment is unchanged:
- native RenderView `client.dll+0x003A5280`;
- local full `CViewSetup 0x198` copy;
- only `origin +0xB8` changes;
- deterministic `+32 Source units` along camera RIGHT;
- retained Quest projection continuously enabled;
- one continuous HLAE/OpenXR gameplay stream with `BASE_A -> ORIGIN_SHIFT -> BASE_B`;
- no HMD pose, angle mutation, hands, weapon, culling, A2, or MASTER4 work.

## CURRENT GATE
**MASTER 3.1 — bounded early native origin ownership probe.**

## NEXT
Run v0.9.1 once on the frozen target and return `CSGOVR_MASTER31_EARLY_ORIGIN_RESULT_*.zip`. Only a runtime causal PASS may open MASTER 3.2 real HMD pose.
