# CSGOVR — NEXT

Snapshot: 2026-08-25 after valid pose-coherence Observatory v1.0.4 result `CSGOVR_MASTER32A_POSE_COHERENCE_RESULT_20260825_194358.zip`.

## CLOSED GATES
MASTER 1 = PASS.
MASTER 2 = PASS.
MASTER 3.1 native camera-origin ownership = PASS.
MASTER 3.2A real Quest HMD rotation = PASS.

Do not repeat transport identity, Quest projection, +32 origin ownership, or rotation ownership.
A2 remains parked until MASTER4 PERF numerically FAILS.

## POSE COHERENCE v1.0.4 — INSTRUMENT PASS
Raw result:
- `MASTER32A_VERDICT=READY_FOR_VISUAL_CONFIRMATION`
- `CAUSAL_ATTEMPTED=True`
- `HOST_IDENTITY=PASS`
- `NATIVE_GUARD=PASS`
- `QUEST_MATRIX=PASS`
- `PAIR_PROOF=PASS`
- `LEFT_RIGHT_SAME_POSE=PASS`
- `EXACT_RENDER_SUBMIT=PASS`
- `NO_POSE_LOOKUP_MISS=PASS`
- `ALL_9_PHASES=PASS`
- `captureFailures=0`

Exact historical render-pose submission therefore works. Do not revert it without contradictory evidence.

## NEW LEADING BLOCKER — TWO-SLOT STARVATION
The same 194358 run accumulated `droppedPairs=179`.

Code-path review plus logs show:
- no LEFT/RIGHT provenance rejects;
- no pose/mode mismatch;
- no capture failures;
- no pose-history lookup misses.

Therefore the drops are produced by `claim_pair_slot()` finding both shared-memory slots busy. A short host stall can fill two slots, after which newer complete pairs are discarded until a slot frees. This can leave the compositor showing an older pair and matches the user's slight shake during head turns.

Observed long-run cadence also contains large stale bursts: phase p95 values reached roughly 131–175 ms in several phases, while `eyeGapMs` occasionally spiked above 80 ms.

## CURRENT GATE
**MASTER3.2A transport cadence quality: direct 2-slot vs 8-slot A/B on the already-proven exact-pose camera path.**

## NEXT ONE RUN
Run:
`CSGOVR_MASTER3_2A_SLOT_DEPTH_AB_v1.0.5_ONECLICK.zip`

SHA256:
`2045FD7B13CC95A5F4FE7A145C729BA597C014DE35678F64C11EE19E4E2F25D9`

Static verification: 20/20 PASS.

The transport contains 8 physical pair slots but starts with producer limit 2, reproducing the old behavior. Halfway through the same run only the active producer slot limit switches to 8.

Matched A/B phases:
1. A 2-slot STILL
2. A 2-slot FAST YAW
3. A 2-slot REVERSAL
4. B 8-slot STILL
5. B 8-slot FAST YAW
6. B 8-slot REVERSAL
7. B 8-slot PITCH+ROLL
8. B 8-slot MICRO MOTION
9. B 8-slot CENTER STILL

Compare visually especially phase 2 vs 5 and phase 3 vs 6. Analyzer compares per-phase dropDelta, staleMs and eyeGapMs.

Everything else stays fixed: exact Quest matrix, native `client+0x003A5280`, proven HMD rotation, exact render-pose submit, same-pair L/R policy. HMD translation/hands/weapon remain excluded.

## DECISION
If 8-slot phases materially reduce drop growth/stale bursts and feel smoother: freeze 8-slot transport and resume MASTER3.2B positional 6DoF.
If 8 slots do not help: next blocker is host upload/pacing, not camera ownership. Do not reopen camera/projection.
