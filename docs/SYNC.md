# CSGOVR — STATE SYNC

Snapshot: **2026-08-27 cross-tab reconciliation through weapon FRESH_EPOCH_FIRE_DELTA and T14 external evidence.**

## FROZEN TARGET
- legacy CS:GO x86, AppID `4465480`, BuildID `22100934`
- frozen `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- frozen `server.dll` SHA256 `699757BB5C8DDB71193F68DABA22F59F697491DDBD768BC8656AC2CE435C4DB8`
- HLAE 2.191.1
- pass1 LEFT / pass2 RIGHT / pass3 monitor lineage retained
- full `CViewSetup = 0x198`
- `-insecure`, local/offline only

## WORLD / HMD GOLDEN — UNCHANGED
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256:
`5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Evidence:
`CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`

User visual verdict: **VR + full HMD 6DoF works.**

Tearing remains a quality defect.

## CLOSED WORLD / HMD
- MASTER1 transport = PASS
- MASTER2 Quest projection = PASS
- MASTER3.1 native origin authority at `client+0x003A5280` = PASS
- MASTER3.2A HMD rotation = PASS
- MASTER3.2B positional translation / full 6DoF = PASS

## VIEWMODEL / HAND SYNC
Exact retail owner now closes the old M34 ambiguity:
- `CBaseViewModel::CalcViewModelView = client+0x001B7F80`
- `CPredictedViewModel::CalcViewModelView = client+0x003420E0`
- runtime slot `250`

RIGHT Touch hand-follow through the stock arms/viewmodel path = PASS.
Dynamic slot scan `0..239` = STOPPED.
DrawModel transform mutation = STOPPED as the active route.

## FIRE SYNC
Exact seams:
- client `FX_FireBullets = +0x0044DC90`
- server `FX_FireBullets = +0x004C4650`
- client callsite `+0x003F6989`
- server callsite `+0x005A7B09`

Proven ABI:
- ECX player index
- EDX item definition index
- `[ESP+4] vOrigin*`
- `[ESP+8] vAngles*`
- caller cleanup `ADD ESP,0x2C`

Current safe policy:
**mutate angle only; stock Weapon_ShootPosition owns origin.**

## LATEST WEAPON RUNTIME RESULT
`CSGOVR_WEAPON_FRESH_EPOCH_FIRE_DELTA_RESULT_20260827_143029.zip`

Class: **runtime instrumentation PASS**.

Key counters:
- client/server angle redirects `229/231`
- origin redirects `0/0`
- fire-delta publishes `3120`
- applies `229/231`
- stale fallbacks `0`
- torn-read fallbacks `0`
- max applied sample age `78 ms`
- freshness contract PASS
- game remained running

The v1.7 semantic transplant preserves the original retail `GetFinalAimAngle` correction while rotating it into the controller-owned eye basis. Final exact user-visible barrel-axis accuracy remains a separate visual verdict unless explicitly confirmed for this exact run.

## MUZZLE MATRIX SYNC
v1.5 muzzle-origin attempt = partial / origin FAIL.

v1.6 FINAL MUZZLE MATRIX = STOPPED integration path:
- matrix lookup itself worked
- textures/audio regressed
- fire calls diverged
- game stability regressed

v1.6.1 removed attachment/matrix origin mutation and restored texture/audio stability by user observation, but absolute post-Calc root angle was still visually wrong.

Decision: **do not patch v1.6 forward; do not mutate fire origin until a gameplay-space owner is independently proven.**

## PVS / PACING LANE — SEPARATE
`CSGOVR_T14_EXTERNAL_RESULT_20260827_143839.zip`

T14 result summary:
- `VERDICT=FAIL`
- `FIRST_MISSING_EDGE=ACTIVE_DURATION_NOT_REACHED`
- RenderViews `10245`
- L/R/monitor `3415/3415/3415`
- submitted `3415`
- dropped `0`
- mixed `0`
- right controller samples `3166`
- XR retry PASS attempt 1

Useful infrastructure evidence, but PVS hypothesis is not PROVEN.

Prepared/unrun unless a later RESULT exists:
- `CSGOVR_T15_PACING_CLEAN_AB_v1.3.0_120S_ONECLICK.zip`
- `CSGOVR_COLD_SLOT249_251_LAG_OWNER_v3.1.1.zip`
- `CSGOVR_FX_FIRE_ABI_SLOT_CLOSURE_v1.0.0_COLD_ONLY_ONECLICK.zip`

## CURRENT GATES
Weapon: preserve v1.7 safe chain and close only remaining exact barrel-axis/visual correctness without touching render attachments or origin.

Quality: pacing/PVS remains independent.

## STOP RULE
Do not reopen world transport/projection/origin/HMD 6DoF.
Do not return to v1.6 render-space muzzle matrix.
Do not combine intrusive pacing/PVS work with weapon/fire mutation.

Canonical Aug 27 delta:
`research/CROSS_TAB_SYNC_20260827.md`
