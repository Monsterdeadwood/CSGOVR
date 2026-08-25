# CSGOVR — STATE SYNC

Snapshot: **2026-08-25 late reconciliation after M34 v0.2.x failure boundary was recorded.**

## FROZEN TARGET
- legacy CS:GO x86, AppID `4465480`, BuildID `22100934`
- frozen `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- HLAE 2.191.1
- pass1 LEFT / pass2 RIGHT / pass3 monitor lineage retained
- full `CViewSetup = 0x198`
- `-insecure`, local/offline only

## CURRENT GOLDEN
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256:
`5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Evidence:
`CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`

User visual verdict:
**VR + full HMD 6DoF works.**

Known remaining quality defect: tearing.

## CLOSED
- MASTER1 transport = PASS
- MASTER2 Quest projection = PASS
- MASTER3.1 native origin authority at `client+0x003A5280` = PASS
- MASTER3.2A real Quest HMD rotation = PASS
- MASTER3.2B positional translation / full 6DoF = PASS at the current GOLDEN

## RETAINED POSE-COHERENCE EVIDENCE
`CSGOVR_MASTER32A_POSE_COHERENCE_RESULT_20260825_194358.zip` remains useful instrumentation evidence:
- host/native/Quest-matrix/pair proof PASS
- LEFT/RIGHT same pose PASS
- exact render-pose submission PASS
- all 9 phases PASS
- pose lookup miss 0
- capture failure 0

That run also exposed two-slot starvation (`droppedPairs=179`) and stale bursts. This is retained as a cadence finding, not the current camera-ownership blocker.

## M34 INVALID BOUNDARY
`M34 v0.2.x = INVALID / FROZEN`.

`_RUN_20260825_221228` reached launch, map load and both DLL injections. It then stopped at the first HLAE stereo render after both injections.

Two changes were active together:
1. LEFT/RIGHT `drawViewModel 0 -> 1`
2. passive/global `C_BaseAnimating::BuildTransformations` vtable index 184 hook

Because both were enabled before the same first render, the result cannot assign causality to either one.

M33 had already reached real game pairs with the slot8/controller-capable transport lineage, so slot depth alone is not the leading M34 suspect.

## CURRENT GATE
**Serial viewmodel-path isolation on exact M3.2B GOLDEN.**

A. exact GOLDEN baseline
B. GOLDEN + `drawViewModel=1` only, no hand DLL
C. only after B PASS: passive `BuildTransformations` observation, no controller, no mutation
D. only after C PASS: controller IPC
E. only after D PASS: viewmodel transform mutation

## SYNC RESOLUTION
Older documents that still say the current gate is T0/G2.5 transport certification or M3.2A slot-depth A/B are historical snapshots. The current runtime authority is the M3.2B GOLDEN plus the M34 rollback decision above.

Detailed boundary note:
`docs/M34_FAILURE_ROLLBACK_20260825.md`

Cross-tab reconciliation:
`research/LATE_SYNC_20260825_M34_ROLLBACK.md`
