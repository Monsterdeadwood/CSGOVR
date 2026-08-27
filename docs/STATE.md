# CSGOVR State

Snapshot: **2026-08-27 15:xx EEST cross-tab sync through FRESH_EPOCH_FIRE_DELTA and T14 external result.**

## STATUS
ACTIVE.

## AUTHORITY / SYNC RULE
This file is the current operational state. Deep evidence remains in `docs/PROVEN.md`, `docs/FALSIFIED.md`, `research/RESULT_INDEX.md`, dated research notes, and especially `research/CROSS_TAB_SYNC_20260827.md` for the Aug 26-27 delta.

Prepared ZIP/BAT != RESULT. Do not promote an unrun tool to evidence.

## FROZEN TARGET
- legacy CS:GO x86, AppID `4465480`, BuildID `22100934`
- game: `C:\Program Files (x86)\Steam\steamapps\common\csgo legacy`
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- `server.dll` SHA256 `699757BB5C8DDB71193F68DABA22F59F697491DDBD768BC8656AC2CE435C4DB8`
- native RenderView: `client.dll+0x003A5280`
- HLAE 2.191.1
- full `CViewSetup = 0x198`
- Quest 3 / Meta OpenXR
- `-insecure`, local/offline lab only

## WORLD / HMD RUNTIME GOLDEN — UNCHANGED
Package:
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256:
`5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Runtime evidence:
`CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`

User visual verdict:
**VR + full HMD 6DoF works.**

Known defect on this GOLDEN:
**tearing / cadence quality**.

This remains the rollback authority for world stereo, projection, camera origin and HMD 6DoF. Weapon experiments do not replace this GOLDEN.

## CLOSED WORLD / HMD — DO NOT REDISCOVER
- MASTER 1 transport semantics = PASS
- MASTER 2 Quest asymmetric projection = PASS
- MASTER 3.1 native origin authority at `client+0x003A5280` = PASS
- MASTER 3.2A real Quest HMD rotation = PASS
- MASTER 3.2B positional HMD translation / full 6DoF = PASS

Pose-coherence evidence retained:
`CSGOVR_MASTER32A_POSE_COHERENCE_RESULT_20260825_194358.zip`

It also exposed cadence/starvation (`droppedPairs=179`), but that does not reopen camera ownership.

## M34 HISTORICAL FAILURE BOUNDARY
`M34 v0.2.x = INVALID / FROZEN as a causal baseline.`

Its first-render failure coupled:
1. LEFT/RIGHT `drawViewModel 0 -> 1`
2. passive/global `BuildTransformations` hook

That coupled configuration is stopped. The old A/B/C/D/E isolation plan has now been overtaken by later exact-retail viewmodel-owner work; do not treat it as the current gate.

## VIEWMODEL OWNER — CLOSED
Exact frozen retail:
- `CBaseViewModel::CalcViewModelView = client+0x001B7F80`
- `CPredictedViewModel::CalcViewModelView = client+0x003420E0`
- exact runtime vtable slot = `250`
- ABI = x86 `__thiscall(this, owner, Vector const&, QAngle const&)`, callee `ret 0x0C`

ConVar anchors:
- `viewmodel_offset_x = 0x0325CCC0`
- `viewmodel_offset_y = 0x0325CD28`
- `viewmodel_offset_z = 0x0325CD90`
- `cl_use_new_headbob = 0x0526F910`

The old dynamic vtable search limited to slots `0..239` is STOPPED; it could not reach the real slot 250.

## RIGHT-HAND / WEAPON FOLLOW — PASS
Current safe route:
`RIGHT Touch -> exact CPredictedViewModel::CalcViewModelView slot250 -> stock arms + stock weapon render path`.

Retained facts:
- RIGHT grip owns translation
- RIGHT aim owns orientation
- rigid SE(3) grab calibration
- DrawModel weapon-transform mutation STOPPED
- later runtime result still reports `WEAPON_HAND_FOLLOW=PASS`

## FIRE SEAM — CLOSED EXACT ABI
Exact functions:
- client `FX_FireBullets = client+0x0044DC90`
- server `FX_FireBullets = server+0x004C4650`

Exact callsites:
- client `+0x003F6989`
- server `+0x005A7B09`

ABI:
- ECX player index
- EDX item definition index
- `[ESP+4] = vOrigin*`
- `[ESP+8] = vAngles*`
- 9 more stack dwords
- caller cleanup `ADD ESP,0x2C`

## LATEST WEAPON/FIRE RUNTIME AUTHORITY
Executed evidence:
`CSGOVR_WEAPON_FRESH_EPOCH_FIRE_DELTA_RESULT_20260827_143029.zip`

Class:
**RUNTIME INSTRUMENTATION PASS / latest weapon-fire authority, not world GOLDEN.**

Key runtime verdict:
- `WEAPON_FRESH_EPOCH_FIRE_DELTA=PASS`
- `WORLD_STEREO_HMD=PASS`
- `WEAPON_HAND_FOLLOW=PASS`
- `WEAPON_BULLET_AIM=PASS`
- exact CalcViewModel hook active
- exact client/server FX_FireBullets hooks active
- client/server angle redirects `229/231`
- origin redirects `0/0`
- stock `Weapon_ShootPosition()` remains origin owner
- fire-delta publishes `3120`
- client/server applies `229/231`
- stale fallbacks `0`
- torn-read fallbacks `0`
- max applied controller-sample age `78 ms`
- freshness contract PASS
- normal ammo/spread retained
- game remained running

v1.7 semantic rule:
`original retail GetFinalAimAngle` is not replaced absolutely. It is transformed from stock EyeAngles basis into the RIGHT-Touch eye basis via a fresh delta, preserving the native relative fire correction.

Automatic runtime PASS does not by itself replace a direct user-visible barrel-axis verdict for this exact v1.7 run. Keep visual final alignment as a separate remaining check unless explicitly confirmed.

## MUZZLE / ORIGIN LINE — STOPPED AT RENDER-SPACE ROUTE
### v1.5
`CSGOVR_WEAPON_MUZZLE_ORIGIN_RESULT_20260827_003016.zip`
- render stable / recoil present
- muzzle origin `0/0` -> FAIL
- infinite ammo was runner test configuration, not hook behavior

### v1.6 FINAL MUZZLE MATRIX
`CSGOVR_WEAPON_FINAL_MUZZLE_MATRIX_RESULT_20260827_012837.zip`
- attachment matrix lookup active (`FINAL_MATRIX_RATE=1.0`)
- textures regressed
- shot audio regressed
- client/server fire calls diverged `119/361`
- game not left running

Decision:
**STOP GetAttachment/SetupBones/render-viewmodel matrix mutation from CalcViewModelView as fire-origin architecture.**

### v1.6.1 safe rollback-forward
- attachment/matrix query removed
- stock fire origin restored
- user observed textures stable and shot sound present
- bullets still visually wrong in that run

This established that absolute post-Calc viewmodel-root angle alone is not the same semantic object as the retail final fire angle.

## PVS / PACING QUALITY LANE — SEPARATE
`CSGOVR_T14_EXTERNAL_RESULT_20260827_143839.zip`

T14 identifies itself as `REAL MULTI-ORIGIN PVS v1.2.0` and produced useful transport/controller evidence, but its own summary is:
- `VERDICT=FAIL`
- `FIRST_MISSING_EDGE=ACTIVE_DURATION_NOT_REACHED`
- RenderViews `10245`
- LEFT/RIGHT/MONITOR `3415/3415/3415`
- submitted pairs `3415`
- dropped `0`
- mixed `0`
- right controller pose samples `3166`
- XR retry PASS attempt 1

Therefore T14 PVS hypothesis is **not PROVEN**. Keep this lane independent of weapon/fire causality.

Prepared but not promoted without executed RESULT:
- `CSGOVR_T15_PACING_CLEAN_AB_v1.3.0_120S_ONECLICK.zip`
- `CSGOVR_COLD_SLOT249_251_LAG_OWNER_v3.1.1.zip`
- `CSGOVR_FX_FIRE_ABI_SLOT_CLOSURE_v1.0.0_COLD_ONLY_ONECLICK.zip`

## PROVEN / RETAIN
- world/HMD M3.2B GOLDEN
- Quest asymmetric projection / full CViewSetup lineage
- exact slot250 viewmodel owner
- RIGHT Touch stock-viewmodel hand-follow
- exact FX_FireBullets client/server seams and ABI
- stock fire origin as current safe owner
- v1.7 fresh-epoch delta publication/application/coherence/freshness PASS
- render/audio stability returns when render-attachment origin mutation is removed

## FALSIFIED / STOPPED
- dynamic viewmodel vtable search 0..239
- coupled M34 as baseline
- DrawModel mutation as active hand route
- v1.6 render-attachment/muzzle-matrix fire-origin path
- absolute v1.6.1 post-Calc viewmodel-root angle as complete fire-angle replacement
- promoting T14 PVS to PASS despite failed active-duration gate

## STILL UNKNOWN
1. Direct user-visible barrel-axis accuracy of exact v1.7 executed result.
2. Safe gameplay-space physical muzzle-origin owner, if still required.
3. Independent LEFT skeletal/offhand hand path.
4. T14/PVS hypothesis beyond infrastructure evidence.
5. Tearing / host pacing / cadence quality.
6. HUD, comfort, performance and release regression.

## CURRENT GATE
For weapon/fire: **preserve v1.7 safe chain and close only remaining visual/semantic barrel-axis uncertainty without touching fire origin or render attachments.**

For quality: **pacing/PVS remains a separate lane.**

## STOP RULES
Do not reopen transport, Quest projection, world origin, HMD rotation or basic positional 6DoF without contradictory evidence.

Do not patch v1.6 matrix path forward.
Do not mutate `[ESP+4] vOrigin` until a gameplay-space owner is independently proven.
Do not merge pacing/PVS intrusive work into the weapon/fire process.

Detailed Aug 26-27 handoff:
`research/CROSS_TAB_SYNC_20260827.md`
