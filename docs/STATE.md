# CSGOVR State

Snapshot: **2026-08-25 late cross-tab / GitHub reconciliation after M34 rollback.**

## STATUS
ACTIVE.

## AUTHORITY / SYNC RULE
This file is the current operational state. Deep historical evidence remains in `docs/PROVEN.md`, `docs/FALSIFIED.md`, `docs/EXPERIMENTS.md`, `research/RESULT_INDEX.md` and the dated research notes.

Any older statement that names **T0 / G2.5 transport certification** or **M3.2A slot-depth A/B** as the current gate is historical and is superseded by this snapshot.

## FROZEN TARGET
- legacy CS:GO x86, AppID `4465480`, BuildID `22100934`
- game: `C:\Program Files (x86)\Steam\steamapps\common\csgo legacy`
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- native RenderView: `client.dll+0x003A5280`
- HLAE 2.191.1
- full `CViewSetup = 0x198`
- Quest 3 / Meta OpenXR
- `-insecure`, local/offline lab only

## CURRENT RUNTIME GOLDEN
Package:
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256:
`5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Runtime evidence:
`CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`

User visual verdict:
**VR + full HMD 6DoF works.**

Known defect that remains on the GOLDEN:
**tearing / cadence quality**.

Treat tearing as a quality/performance problem. It does not reopen already-proven projection, origin, rotation or 6DoF ownership.

## CLOSED / DO NOT REDISCOVER
- MASTER 1 transport semantics = PASS
- MASTER 2 Quest asymmetric projection = PASS
- MASTER 3.1 native origin authority at `client+0x003A5280` = PASS
- MASTER 3.2A real Quest HMD rotation = PASS
- MASTER 3.2B positional HMD translation / full 6DoF = PASS at the current GOLDEN

Supporting pose-coherence result:
`CSGOVR_MASTER32A_POSE_COHERENCE_RESULT_20260825_194358.zip`

Instrument facts retained from that run:
- host identity PASS
- native guard PASS
- Quest matrix PASS
- pair proof PASS
- LEFT/RIGHT same pose PASS
- exact render-pose submission PASS
- pose lookup misses 0
- capture failures 0
- all 9 phases present

The same run exposed two-slot starvation (`droppedPairs=179`) and large stale bursts. Later M33 reached real game pairs with a slot8/controller-capable transport lineage, so slot depth alone is not the leading suspect for the current M34 failure.

## M34 FAILURE BOUNDARY
`M34 v0.2.x = INVALID / FROZEN`.

Do not use it as a baseline.

`_RUN_20260825_221228` progressed through:
- CS:GO launch
- `de_dust2` load
- non-zero transport DLL `LoadLibraryW`
- non-zero hand-probe DLL `LoadLibraryW`

The NETCON transcript ends at the first:
`mirv_streams record start; echo CSGOVR_M34_START`

Therefore the observed failure boundary is the **first HLAE stereo render after both injections**, not build, launch, map load or DLL injection.

Two mutations were coupled before that first render:
1. `CSGOVRLEFT/RIGHT drawViewModel 0 -> 1`
2. passive/global `C_BaseAnimating::BuildTransformations` vtable index 184 hook in `hand_probe.dll`

The M34 run cannot distinguish which caused the failure.

## CURRENT GATE
**VIEWMODEL REINTRODUCTION / HAND-PATH ISOLATION ON EXACT M3.2B GOLDEN.**

Serial plan:
- A: exact M3.2B GOLDEN baseline
- B: GOLDEN + `drawViewModel=1` only, no hand DLL
- C: only after B PASS, add PASSIVE `BuildTransformations` observation, no controller, no transform mutation
- D: only after C PASS, add controller IPC
- E: only after D PASS, mutate viewmodel transform

A future one-click runner may execute A/B/C as separate auto-closed sessions and emit one combined result, but each session must preserve causal isolation.

## STOP RULES
Do not reopen:
- transport identity
- Quest projection signs / matrix layout
- native camera-origin ownership
- basic HMD rotation ownership
- basic positional 6DoF ownership

Do not combine controller IPC or transform mutation into B/C.
Do not patch M34 forward from the failing coupled state.
Do not treat tearing as evidence that the world-camera architecture is wrong.

## UNRESOLVED
1. Does `drawViewModel=1` alone survive the first HLAE stereo render on exact M3.2B GOLDEN?
2. If yes, does passive `BuildTransformations` observation survive independently?
3. Controller IPC after passive observation is stable.
4. Viewmodel transform mutation / weapon ownership.
5. Tearing / host pacing / cadence quality.
6. HUD, comfort, performance, regression and release.

## LATEST DATED NOTE
`docs/M34_FAILURE_ROLLBACK_20260825.md`

Cross-tab reconciliation note:
`research/LATE_SYNC_20260825_M34_ROLLBACK.md`
