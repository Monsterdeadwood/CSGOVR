# CSGOVR — NEXT

Snapshot: **2026-08-27 sync through FRESH_EPOCH_FIRE_DELTA result 14:30 and T14 external result 14:38.**

## BASELINES — DO NOT CONFLATE

### World / HMD rollback GOLDEN
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256:
`5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Evidence:
`CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`

Truth: VR + full HMD 6DoF works. Tearing is a separate quality issue.

### Latest weapon/fire runtime authority
`CSGOVR_WEAPON_FRESH_EPOCH_FIRE_DELTA_RESULT_20260827_143029.zip`

Runtime instrumentation verdict: PASS.
- exact slot250 hand-follow active
- client/server FX_FireBullets active
- angle redirects `229/231`
- origin redirects `0/0`
- freshness contract PASS, max applied sample age `78 ms`
- stock Weapon_ShootPosition retained
- game stable at snapshot

Do **not** replace the world GOLDEN with this weapon result; they prove different layers.

## CURRENT WEAPON MISSING FACT
Does the exact executed v1.7 fire-delta path place real impacts on the intended barrel/controller axis to user-visible accuracy, now that the retail `GetFinalAimAngle` relation is preserved and stale-pose reuse is closed?

The automatic v1.7 RESULT closes hook activity, freshness and client/server coherence. It does not substitute for a direct visual barrel-axis verdict unless that exact run has been explicitly visually confirmed.

## WHY THE OLD NEXT IS SUPERSEDED
The previous A/B/C/D/E gate (`drawViewModel -> BuildTransformations -> controller IPC -> mutation`) was the correct rollback plan after M34, but later exact-retail work already resolved the real viewmodel owner:
- `CPredictedViewModel::CalcViewModelView = client+0x003420E0`
- runtime slot `250`
- hand-follow PASS on the stock viewmodel route

Do not go back to M34 isolation unless new contradictory evidence invalidates slot250 lineage.

## CURRENT SAFE WEAPON CHAIN
Preserve exactly:

`M3.2B world/HMD foundation`
`-> stock viewmodel draw`
`-> exact CPredictedViewModel::CalcViewModelView slot250 hand-follow`
`-> original retail GetFinalAimAngle`
`-> fresh RIGHT-Touch eye-basis delta`
`-> exact client/server FX_FireBullets vAngles only`
`-> stock Weapon_ShootPosition vOrigin`

## HARD STOPS
Do not:
- mutate `[ESP+4] vOrigin` without a separately proven gameplay-space owner
- call render-space `GetAttachment/SetupBones` from CalcViewModelView for fire origin
- patch v1.6 FINAL MUZZLE MATRIX forward
- return to dynamic vtable search `0..239`
- use DrawModel mutation as the active weapon-control route
- reopen transport / Quest projection / world origin / HMD 6DoF

## IF BARREL AXIS IS STILL VISUALLY WRONG
Stay COLD first.

Primary cold question:
**what fixed/local orientation relationship remains between the stock weapon/barrel basis, stock EyeAngles, original GetFinalAimAngle and the slot250 controller-owned viewmodel basis?**

Prefer exact retail callgraph/dataflow/ABI and already-proven fire seam over another broad LIVE probe. A correction must preserve the original retail fire-angle semantics and current origin/audio/render baseline.

Prepared cold artifacts are not evidence until executed:
- `CSGOVR_FX_FIRE_ABI_SLOT_CLOSURE_v1.0.0_COLD_ONLY_ONECLICK.zip`
- `CSGOVR_COLD_SLOT249_251_LAG_OWNER_v3.1.1.zip`

## SEPARATE QUALITY / PVS LANE
Latest evidence:
`CSGOVR_T14_EXTERNAL_RESULT_20260827_143839.zip`

T14 scientific verdict is **FAIL / ACTIVE_DURATION_NOT_REACHED**, despite healthy transport counters and zero dropped/mixed pairs in the collected stage. Do not call the PVS hypothesis proven.

Prepared later tool:
`CSGOVR_T15_PACING_CLEAN_AB_v1.3.0_120S_ONECLICK.zip`

Class: **UNRUN / prepared** unless a later executed RESULT exists.

Keep pacing/PVS intrusive work separate from weapon/fire causality.

## LIVE PREFLIGHT CONTRACT
Before any future LIVE/build/injection/capture, write exactly five lines:
`MISSING FACT / WHY NOT COLD / PASS / FAIL / BASELINE MUST HOLD`.

One PRIMARY FACT per LIVE, broad compatible passive SECONDARY HARVEST allowed.

## STOP RULE
No cosmetic vNext. REUSE -> ADAPT -> FIX -> EXTEND.

Current detailed handoff:
`research/CROSS_TAB_SYNC_20260827.md`
