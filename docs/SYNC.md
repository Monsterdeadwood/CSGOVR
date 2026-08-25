# CSGOVR — STATE SYNC

Snapshot: 2026-08-25 after valid pose-coherence Observatory v1.0.4 result.

## FROZEN BASELINE
- legacy CS:GO x86, AppID `4465480`, BuildID `22100934`
- frozen `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- HLAE 2.191.1 exact
- pass1 LEFT / pass2 RIGHT / pass3 monitor
- full `CViewSetup = 0x198`
- `-insecure`, local/offline only

## CLOSED
MASTER1 transport semantics = PASS.
MASTER2 Quest projection = PASS.
MASTER3.1 native origin authority at `client+0x003A5280` = PASS.
MASTER3.2A real Quest HMD rotation = PASS.

## POSE COHERENCE RESULT 194358
`CSGOVR_MASTER32A_POSE_COHERENCE_RESULT_20260825_194358.zip`

Instrument verdict: PASS / ready for visual comparison.
Evidence:
- current host identity PASS;
- native guard PASS;
- exact Quest matrix retained;
- pair proof PASS;
- LEFT/RIGHT same pose PASS;
- exact render-pose lookup/submission PASS;
- 9/9 phases present;
- pose lookup misses 0;
- capture failures 0.

Exact render-pose submission is retained as the current timing policy.

## NEW QUALITY BLOCKER
The same run accumulated 179 `droppedPairs`.
There were no provenance rejects, no pose/mode mismatches, no capture failures and no lookup misses. Source inspection shows these drops are generated when `claim_pair_slot()` finds both shared-memory pair slots occupied.

This converts the jitter investigation from a broad pose question into a bounded cadence question: two-slot starvation during short host stalls can discard newer pairs and leave an older complete pair displayed.

Long-run metrics also show large stale bursts in some phases, with stale p95 roughly 131–175 ms and occasional eye-gap spikes above 80 ms.

## PREPARED v1.0.5 — SLOT DEPTH A/B
Package:
`CSGOVR_MASTER3_2A_SLOT_DEPTH_AB_v1.0.5_ONECLICK.zip`

SHA256:
`2045FD7B13CC95A5F4FE7A145C729BA597C014DE35678F64C11EE19E4E2F25D9`

Static verification: 20/20 PASS.

Implementation:
- shared transport grows to 8 physical complete-pair slots;
- unique V105 mapping prevents stale ABI collision;
- producer begins with active slot limit 2;
- same live run switches active slot limit to 8;
- host always consumes newest complete pair and frees older ready pairs;
- exact render-pose association remains unchanged;
- camera/projection/rotation ownership remain unchanged.

Matched phases compare 2-slot vs 8-slot STILL, FAST YAW and REVERSAL. Additional 8-slot pitch+roll, micro-motion and center-still phases measure stability.

## CURRENT GATE
MASTER3.2A cadence quality, 2-slot vs 8-slot A/B.

## NEXT
Run `00_RUN_MASTER32A_SLOT_DEPTH_AB.bat`, keep mouse still, follow prompts, and compare phase 2 vs 5 and phase 3 vs 6. Return result ZIP plus which half felt smoother.

If 8 slots reduce drop growth/stale bursts and feel smoother, freeze 8-slot cadence and resume MASTER3.2B positional HMD translation. If not, optimize x64 host upload/pacing next. Do not reopen camera ownership or Quest projection.
