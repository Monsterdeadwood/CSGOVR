# MASTER 2 QUEST MATRIX — INVALID RUNNER RESULT

Run: `CSGOVR_QUEST_COHERENCE_RESULT_20260825_051205.zip`
Date: 2026-08-25

Verdict: **INVALID / NO MASTER2 SCIENTIFIC VERDICT**.

Observed headset behavior:
- BLUE LEFT / ORANGE RIGHT diagnostic remained visible;
- real CS:GO gameplay did not replace the diagnostic layer in Quest.

Raw evidence:
- OpenXR host initialized and submitted DIAG layers;
- NetCon shows `Host_NewGame on map de_dust2`, so CS:GO launch/map load path did execute;
- HLAE streams `CSGOVRLEFT` and `CSGOVRRIGHT` were created;
- no causal phase reached `mirv_streams record start`;
- no GAME pair reached OpenXR in this run;
- `RESULT_STATUS.txt`: `PATH_PROOF_PASS=False`, `XR_SESSION_PASS=True`, `XR_GAME_LAYER_PASS=False`;
- failure reason: `QUEST_MATRIX_PATH_PROOF line missing`.

Root cause in runner:
PowerShell functions emit external executable stdout as SuccessOutput. `Inject-Transport` invoked the injector and also returned the intended `eye_patch.log` path. Therefore `$eyeLog=Inject-Transport ...` could become an `Object[]` rather than one scalar path. The subsequent path-proof stage then tested the wrong object/path and false-failed before any HLAE causal recording started.

This also explains the earlier MASTER1 runner false-negative: the x86 producer later emitted coherent pairs visible in `XR_HOST.log`, while the runner-side eye-log polling had already been operating on the wrong return object.

Fix prepared:
`CSGOVR_MASTER2_QUEST_MATRIX_v0.8.1_RUNNER_SCALAR_LOG_FIX_ONECLICK.zip`
- captures injector stdout internally;
- writes `INJECTOR_OUTPUT.txt`;
- returns one scalar `eye_patch.log` path only;
- removes stale same-PID log before injection;
- requires the exact literal log path to exist and be fresh before path proof.

C/C++ transport, Quest matrix values, HLAE schedule, projection carrier, and causal protocol are unchanged from v0.8.0.
