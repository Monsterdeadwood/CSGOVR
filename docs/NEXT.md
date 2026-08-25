# CSGOVR — NEXT

Snapshot: 2026-08-25 after MASTER 3.2A HMD rotation PASS, user report of slight image shake during head turns, and invalid Observatory v1.0.3 build `CSGOVR_MASTER32A_POSE_COHERENCE_RESULT_20260825_184330.zip`.

## CLOSED GATES
MASTER 1 = PASS.
MASTER 2 = PASS.
MASTER 3.1 native camera-origin ownership = PASS.
MASTER 3.2A real Quest HMD rotation = PASS.

Do not repeat transport, Quest projection, +32 origin ownership, or rotation ownership.
A2 remains parked until MASTER4 PERF numerically FAILS.

## CURRENT BLOCKER
User reports the picture is almost normal VR but shakes slightly while turning the head.

Leading timing hypothesis from the successful 172017 run: GAME pairs are often roughly 19–33 ms stale while HMD pose updates faster than GAME pair production. The old host can therefore submit a newer orientation than the orientation used to render the displayed pair.

## OBSERVATORY v1.0.3 RESULT — INVALID / BUILD INFRASTRUCTURE
Result: `CSGOVR_MASTER32A_POSE_COHERENCE_RESULT_20260825_184330.zip`.

Raw status:
- `MASTER32A_VERDICT=INVALID_PRE_CAUSAL`
- `CAUSAL_ATTEMPTED=False`
- `HOST_SHA256=MISSING`

Exact compiler error:
`xr_host.cpp:176:47: error: use of undeclared identifier 'g_lastPairConsumeAgeMs'`

The `XR_HOST.log` packaged in that result is stale contamination from an older `MASTER3.2B v1.1.0 FULL 6DOF` host and is not evidence for v1.0.3.

## NEXT ONE STEP
Run:
`CSGOVR_MASTER3_2A_POSE_COHERENCE_OBSERVATORY_v1.0.4_BUILD_ISOLATION_FIX_ONECLICK.zip`

SHA256:
`310830529F5BC33251A5EF3D56489140336241D935E47E0E2F28F4954CC7DE1D`

Static verification: 19/19 PASS.

v1.0.4 fixes infrastructure only around the Observatory and keeps the same single causal test:
- moves transport telemetry globals before `observe_host_phase()`;
- unique current log `host_poseobs_v104.log`;
- kills all stale `csgovr_*xr_host*` processes before launch;
- requires exact v1.0.4 host banner before startup PASS;
- records current host PID/SHA identity in the result.

## SINGLE ACTIVE CHANGE
Each complete LEFT/RIGHT GAME pair carries the exact HMD pose sequence that rendered it. OpenXR submits that historical orientation as `ROT_RENDER_LATCHED_ANCHOR_POS` instead of a newer pose sampled at submit time.

HMD position remains disabled. Rotation camera ownership remains frozen PASS.

## NINE PASSIVE PHASES IN ONE RUN
1. STILL baseline
2. SLOW yaw
3. FAST yaw
4. PITCH
5. ROLL
6. DIAGONAL yaw+pitch
7. direction REVERSAL
8. MICRO motion
9. CENTER still

Per phase collect renderPoseSeq/currentPoseSeq, sequence gap, render-to-current angular gap, staleMs, consumeAgeMs, eyeGapMs, drops, capture failures, pose lookup misses.

## PASS
Exact pose lookup survives all phases with no capture failures/lookup misses and user reports visibly less jitter while correct head rotation remains.

## FAIL
Exact render-pose association is proven but jitter remains materially unchanged. Then use phase metrics to choose the next timing/cadence correction only. Do not reopen camera ownership.

## AFTER PASS
Freeze exact render-pose policy and resume MASTER 3.2B HMD positional 6DoF on top of the smooth rotation path.
