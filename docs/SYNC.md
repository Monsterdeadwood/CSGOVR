# CSGOVR — STATE SYNC

Snapshot: 2026-08-25 after MASTER 1 PASS, MASTER 2 PASS, MASTER 3.1 native origin PASS, MASTER 3.2A real Quest HMD rotation PASS, and first pose-coherence Observatory build attempt.

## FROZEN BASELINE
- legacy CS:GO x86, AppID `4465480`, BuildID `22100934`
- frozen `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- HLAE 2.191.1 exact
- pass1 LEFT / pass2 RIGHT / pass3 monitor
- full `CViewSetup = 0x198`
- `-insecure`, local/offline only

## CLOSED
MASTER1 transport = PASS.
MASTER2 Quest projection = PASS.
MASTER3.1 native origin authority at `client+0x003A5280` = PASS.
MASTER3.2A real Quest HMD rotation = PASS from `CSGOVR_MASTER32A_HMD_ROTATION_RESULT_20260825_172017.zip` plus user visual report of almost normal VR.

## ACTIVE QUALITY BLOCKER
User reports slight image shake specifically while turning the head.

Leading hypothesis: render/submit pose mismatch. In the successful rotation run GAME pair staleness is commonly roughly 19–33 ms while HMD pose updates more frequently, so a pair rendered on pose N may be submitted with newer pose N+1/N+2.

## POSE COHERENCE OBSERVATORY v1.0.3 — INVALID / BUILD
Result: `CSGOVR_MASTER32A_POSE_COHERENCE_RESULT_20260825_184330.zip`.

Raw status:
- `MASTER32A_VERDICT=INVALID_PRE_CAUSAL`
- `CAUSAL_ATTEMPTED=False`
- `HOST_SHA256=MISSING`

Exact build failure:
`xr_host.cpp:176:47: error: use of undeclared identifier 'g_lastPairConsumeAgeMs'`

The included `XR_HOST.log` starts with `CSGOVR XR HOST MASTER3.2B v1.1.0 FULL 6DOF`; therefore it is stale contamination from an older host process and must not be used as v1.0.3 evidence.

## PREPARED OBSERVATORY v1.0.4
Package:
`CSGOVR_MASTER3_2A_POSE_COHERENCE_OBSERVATORY_v1.0.4_BUILD_ISOLATION_FIX_ONECLICK.zip`

SHA256:
`310830529F5BC33251A5EF3D56489140336241D935E47E0E2F28F4954CC7DE1D`

Static verification: 19/19 PASS.

Infrastructure fixes:
- `g_lastPairConsumeAgeMs` and `g_lastEyeGapMs` declarations moved before `observe_host_phase()`;
- unique host log `host_poseobs_v104.log`;
- all stale `csgovr_*xr_host*` processes stopped before launch;
- exact v1.0.4 host banner required for startup gate;
- current host PID/SHA written into result identity metadata.

Single active causal change remains exact render-pose association: each complete L/R GAME pair stamps the exact HMD pose sequence used to render it; host retrieves that historical quaternion and submits `ROT_RENDER_LATCHED_ANCHOR_POS`.

HMD position remains disabled during this quality gate.

One run contains nine passive phases: STILL, SLOW YAW, FAST YAW, PITCH, ROLL, DIAGONAL, REVERSAL, MICRO MOTION, CENTER STILL. Per-phase metrics include render/current pose sequence, angular gap, staleMs, consumeAgeMs, eyeGapMs, drops, failures and lookup misses.

## CURRENT GATE
**MASTER3.2A pose coherence / jitter quality gate.**

## NEXT
Run only `00_RUN_MASTER32A_POSE_OBSERVATORY.bat` from v1.0.4, keep mouse still, follow all nine phase prompts, return result ZIP and visual comparison: jitter less / same / more.

If smoother, freeze exact render-pose policy and resume MASTER3.2B positional HMD translation. If unchanged, use the captured phase metrics to adapt timing only. Do not reopen native camera ownership or rotation proof.
