# CSGOVR

Native VR research for frozen legacy **CS:GO x86** using HLAE/OpenXR and exact retail evidence.

## Current status
**ACTIVE — M3.2B FULL HMD 6DoF is the runtime GOLDEN. M34 v0.2.x is INVALID / FROZEN.**

Last user-confirmed runtime GOLDEN:

`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256 `5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Runtime evidence:

`CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`

Visual verdict: **VR + full HMD 6DoF works.** Remaining tearing is a quality/cadence defect, not a reason to reopen projection or camera ownership.

## Closed gates
- MASTER 1 transport = PASS
- MASTER 2 Quest projection = PASS
- MASTER 3.1 native camera-origin ownership at `client+0x003A5280` = PASS
- MASTER 3.2A real Quest HMD rotation = PASS
- MASTER 3.2B positional HMD translation / full 6DoF = PASS at the current GOLDEN

Do not repeat transport identity, Quest projection, origin ownership, rotation ownership or basic positional 6DoF discovery unless contradictory evidence appears.

## Latest invalid boundary
`M34 v0.2.x` is **INVALID / FROZEN** and must not become a baseline.

The `_RUN_20260825_221228` evidence reaches CS:GO launch, `de_dust2`, transport DLL injection and hand-probe DLL injection, then stops at the **first HLAE stereo render after both injections**.

Two changes were coupled before that first render:
1. `CSGOVRLEFT/RIGHT drawViewModel 0 -> 1`
2. passive/global `C_BaseAnimating::BuildTransformations` vtable index 184 hook in `hand_probe.dll`

The run cannot attribute the failure to either change individually.

## Current gate
Return to exact M3.2B v1.1.1 GOLDEN and isolate the viewmodel path serially:

A. exact GOLDEN baseline
B. GOLDEN + `drawViewModel=1` only, **no hand DLL**
C. only if B survives, add PASSIVE `BuildTransformations` observation, **no controller and no transform mutation**
D. only after C PASS, add controller IPC
E. only after D PASS, mutate the viewmodel transform

M33 already reached real game pairs with the slot8/controller-capable transport lineage, so slot depth by itself is not the leading M34 suspect.

## Frozen target
- legacy CS:GO x86, Steam AppID `4465480`, BuildID `22100934`
- game `C:\Program Files (x86)\Steam\steamapps\common\csgo legacy`
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- HLAE 2.191.1
- full `CViewSetup = 0x198`
- `-insecure`, local/offline lab only
- Quest 3 / Meta OpenXR

## Start here
1. `docs/STATE.md`
2. `docs/NEXT.md`
3. `docs/SYNC.md`
4. `docs/GATES.md`
5. `docs/M34_FAILURE_ROLLBACK_20260825.md`
6. `research/LATE_SYNC_20260825_M34_ROLLBACK.md`
7. `research/RESULT_INDEX.md`
8. `docs/PROVEN.md` / `docs/FALSIFIED.md` for historical evidence

Older T0/G2.5-current-gate language in historical documents is superseded by this late 2026-08-25 reconciliation.

**PARALLELIZE EVIDENCE — SERIALIZE MUTATION.**
