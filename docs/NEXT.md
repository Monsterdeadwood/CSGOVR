# CSGOVR — NEXT

Snapshot: **2026-08-25 late sync after M34 rollback.**

## FROZEN BASELINE
Use exactly:
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256:
`5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Evidence:
`CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`

Baseline visual truth: **VR + full HMD 6DoF works.** Tearing remains, but it is not part of the next causal question.

## CURRENT MISSING FACT
Does enabling the normal viewmodel render path by itself (`drawViewModel=1`) survive the first HLAE stereo render when absolutely everything else remains identical to M3.2B GOLDEN?

## PREFLIGHT
MISSING FACT: `drawViewModel=1` alone survives or kills the first stereo render on M3.2B GOLDEN.
WHY NOT COLD: static/source evidence cannot prove this runtime boundary.
PASS: stable real-game stereo/6DoF continues after recording starts with viewmodel enabled and no hand DLL.
FAIL: the run dies/freezes at the same first HLAE stereo-render boundary.
BASELINE MUST HOLD: exact M3.2B transport, Quest projection, origin, HMD rotation/translation and runner behavior remain unchanged.

## NEXT ONE MUTATION — B
Start from exact GOLDEN and change only:
- `CSGOVRLEFT drawViewModel 0 -> 1`
- `CSGOVRRIGHT drawViewModel 0 -> 1`

Explicitly exclude:
- `hand_probe.dll`
- `BuildTransformations` hook
- controller IPC
- controller pose
- viewmodel transform mutation
- projection changes
- camera/origin/6DoF changes
- transport redesign

## AFTER B
If B PASS:
- C = add **PASSIVE** `C_BaseAnimating::BuildTransformations` vtable index 184 observation only.
- no controller IPC.
- no transform mutation.

If C PASS:
- D = add controller IPC only.

If D PASS:
- E = mutate the viewmodel transform / establish weapon ownership.

If B FAIL:
- the failure is localized to viewmodel reintroduction itself; do not load the hand probe and do not reopen camera/projection.

## AUTOMATION POLICY
A single one-click package may run A/B/C as **separate auto-closed sessions** and return one combined ZIP, provided each session has an independent baseline check and verdict. Several facts per user action are welcome; several mutations per process are not.

## SEPARATE QUALITY LANE
Tearing / cadence remains open. Keep it separate from the viewmodel ownership experiment unless it prevents the causal verdict.

## DO NOT REPEAT
- MASTER1 transport proof
- MASTER2 Quest projection proof
- MASTER3.1 origin ownership
- MASTER3.2A HMD rotation ownership
- MASTER3.2B basic positional 6DoF
- old T0/G2.5 transport architecture debate
- slot-depth as the default explanation for M34; M33 already reached real pairs with slot8/controller-capable lineage
