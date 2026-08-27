# CSGOVR

Native VR research for frozen legacy **CS:GO x86** using HLAE/OpenXR and exact retail evidence.

## Current status
**ACTIVE. World/HMD M3.2B remains the runtime GOLDEN; exact slot250 weapon hand-follow and v1.7 FRESH_EPOCH_FIRE_DELTA are the current weapon/fire authority.**

### World / HMD GOLDEN
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256:
`5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Evidence:
`CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`

Visual verdict: **VR + full HMD 6DoF works.** Remaining tearing is a quality/cadence defect, not a reason to reopen projection or camera ownership.

### Latest weapon / fire runtime authority
`CSGOVR_WEAPON_FRESH_EPOCH_FIRE_DELTA_RESULT_20260827_143029.zip`

Runtime instrumentation verdict: **PASS**.

Key facts:
- exact `CPredictedViewModel::CalcViewModelView = client+0x003420E0`, runtime slot `250`
- RIGHT Touch hand-follow on stock arms/viewmodel path = PASS
- client `FX_FireBullets = +0x0044DC90`
- server `FX_FireBullets = +0x004C4650`
- v1.7 angle redirects client/server `229/231`
- origin redirects `0/0`
- fire-delta freshness contract PASS, max applied age `78 ms`
- stock `Weapon_ShootPosition()` remains fire-origin owner
- game remained running

The current v1.7 line preserves the game's original retail fire-angle relation and transforms it into the RIGHT-Touch basis instead of replacing it with an absolute viewmodel-root angle.

Final exact user-visible barrel-axis accuracy for this specific run remains a distinct visual verdict unless explicitly confirmed.

## Closed gates
- MASTER 1 transport = PASS
- MASTER 2 Quest asymmetric projection = PASS
- MASTER 3.1 native camera-origin ownership at `client+0x003A5280` = PASS
- MASTER 3.2A HMD rotation = PASS
- MASTER 3.2B positional HMD translation / full 6DoF = PASS
- exact viewmodel owner slot250 = CLOSED
- RIGHT Touch stock-viewmodel hand-follow = PASS
- exact client/server FX_FireBullets seam and ABI = CLOSED
- v1.7 fresh fire-delta publication/application/coherence = PASS

Do not repeat these discovery gates without contradictory evidence.

## Stopped paths
- M34 v0.2.x coupled configuration = INVALID / historical boundary
- dynamic viewmodel vtable scan `0..239` = STOPPED; real slot is 250
- DrawModel weapon-transform mutation as active route = STOPPED
- v1.6 render-space `GetAttachment/SetupBones` muzzle-matrix fire-origin architecture = STOPPED
- absolute v1.6.1 post-Calc root angle as complete replacement for retail final fire angle = FALSIFIED
- fire-origin mutation without a separately proven gameplay-space owner = STOPPED

## Separate PVS / pacing lane
Latest evidence:
`CSGOVR_T14_EXTERNAL_RESULT_20260827_143839.zip`

T14 collected useful transport/controller evidence but its own scientific verdict is:
`FAIL / ACTIVE_DURATION_NOT_REACHED`.

Therefore the T14 multi-origin PVS hypothesis is **not PROVEN**.

Prepared `CSGOVR_T15_PACING_CLEAN_AB_v1.3.0_120S_ONECLICK.zip` remains **UNRUN** unless a later executed RESULT is available.

## Current gate
Weapon/fire:
**preserve the v1.7 safe chain and close only remaining exact barrel-axis / visual correctness without touching render attachments or fire origin.**

Quality:
**pacing/PVS remains a separate causal lane.**

## Frozen target
- legacy CS:GO x86, Steam AppID `4465480`, BuildID `22100934`
- game `C:\Program Files (x86)\Steam\steamapps\common\csgo legacy`
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- `server.dll` SHA256 `699757BB5C8DDB71193F68DABA22F59F697491DDBD768BC8656AC2CE435C4DB8`
- HLAE 2.191.1
- full `CViewSetup = 0x198`
- `-insecure`, local/offline lab only
- Quest 3 / Meta OpenXR

## Start here
1. `docs/STATE.md`
2. `research/CROSS_TAB_SYNC_20260827.md`
3. `docs/NEXT.md`
4. `docs/SYNC.md`
5. `research/RESULT_INDEX.md`
6. `docs/PROVEN.md`
7. `docs/FALSIFIED.md`
8. historical `docs/M34_FAILURE_ROLLBACK_20260825.md` and earlier research notes as needed

**PARALLELIZE EVIDENCE — SERIALIZE MUTATION.**
