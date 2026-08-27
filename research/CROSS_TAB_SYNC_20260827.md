# CSGOVR — CROSS-TAB / EVIDENCE SYNC — 2026-08-27

This note reconciles the post-2026-08-25 work that had not yet reached GitHub. It does **not** replace the historical evidence corpus. It updates the operational truth while keeping evidence classes separate.

## 1. AUTHORITY LADDER

### World / HMD runtime GOLDEN — unchanged
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256:
`5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Evidence:
`CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`

Class: **GOLDEN / user-confirmed runtime**.

Retain as rollback authority for world stereo, Quest projection, HMD rotation/translation and basic 6DoF. Tearing remains a quality/pacing defect, not a camera-ownership falsification.

### Latest weapon/fire runtime authority
`CSGOVR_WEAPON_FRESH_EPOCH_FIRE_DELTA_RESULT_20260827_143029.zip`

Class: **RUNTIME INSTRUMENTATION PASS / weapon integration authority; not a replacement for the world GOLDEN.**

Its `RESULT_STATUS.txt` reports:
- `WEAPON_FRESH_EPOCH_FIRE_DELTA=PASS`
- `WORLD_STEREO_HMD=PASS`
- `WEAPON_HAND_FOLLOW=PASS`
- `WEAPON_BULLET_AIM=PASS`
- stock viewmodel draw route active
- exact CalcViewModelView guard/hook active
- exact client/server FX_FireBullets guard/hook active
- client angle redirects `229`, server angle redirects `231`
- client/server origin redirects `0/0`
- original `Weapon_ShootPosition()` remains owner of fire origin
- client/server fire coherence seen
- fresh fire-delta publication/apply seen
- no torn-read fallback
- maximum applied sample age `78 ms`
- freshness contract PASS (`<=120 ms` policy)
- normal ammo and normal spread commands seen
- game still running after snapshot
- RIGHT controller valid; Meta Touch Plus profile active

This closes the runtime plumbing/freshness question for the v1.7 fire-delta architecture. Final user-visible barrel-axis accuracy should still be treated separately from the automatic runtime PASS until explicitly visually confirmed for this exact v1.7 result.

---

## 2. VIEWMODEL OWNER — CLOSED EXACT RETAIL

Frozen client SHA256:
`183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`

Exact retail owners:
- `CBaseViewModel::CalcViewModelView` = `client.dll + 0x001B7F80`
- `CPredictedViewModel::CalcViewModelView` = `client.dll + 0x003420E0`
- exact runtime `CPredictedViewModel` vtable slot = `250`
- x86 ABI: `__thiscall(this, owner, Vector const&, QAngle const&)`, callee `ret 0x0C`

Fingerprints:
- predicted override references `cl_use_new_headbob` and calls base `0x001B7F80`
- base references the exact `viewmodel_offset_x/y/z` ConVar objects

Exact ConVar RVAs:
- `viewmodel_offset_x = 0x0325CCC0`
- `viewmodel_offset_y = 0x0325CD28`
- `viewmodel_offset_z = 0x0325CD90`
- `cl_use_new_headbob = 0x0526F910`

The old v1.2.0 dynamic vtable locator is **STOPPED**. It searched only slots `0..239`, so it could not reach the true slot 250, and it also scanned unsafely beyond function boundaries.

## 3. HAND-FOLLOW — CLOSED RUNTIME PATH

The successful exact-owner lineage uses RIGHT Touch to drive the stock first-person arms/weapon through `CPredictedViewModel::CalcViewModelView` rather than DrawModel mutation.

Retained v1.3.0 facts:
- RIGHT grip -> translation
- RIGHT aim -> orientation
- first valid pose performs rigid SE(3) grab calibration
- stock arms / stock viewmodel draw route retained
- DrawModel mutation remains STOPPED
- static audit `85/85 PASS`
- package SHA256 `03C75627A29E5F0B29D410D0F82D7861B88680CF223897ABC348CF1204C49B72`

Later runtime evidence keeps `WEAPON_HAND_FOLLOW=PASS` and the exact slot250 owner active.

## 4. FIRE SEAM / ABI — CLOSED EXACT RETAIL

Frozen server SHA256:
`699757BB5C8DDB71193F68DABA22F59F697491DDBD768BC8656AC2CE435C4DB8`

Exact functions:
- client `FX_FireBullets = client.dll + 0x0044DC90`
- server `FX_FireBullets = server.dll + 0x004C4650`

Exact callsites retained by v1.7 cold verification:
- client callsite `client.dll + 0x003F6989`
- server callsite `server.dll + 0x005A7B09`

Proven optimized call ABI:
- `ECX = iPlayerIndex`
- `EDX = nItemDefIndex`
- `[ESP+4] = vOrigin*`
- `[ESP+8] = vAngles*`
- 9 additional stack dwords
- caller cleanup `ADD ESP, 0x2C`

This is a narrow seam: direction can be changed while preserving the native trigger, ammo, cadence, recoil/event path and stock `Weapon_ShootPosition()` origin.

## 5. BULLET AIM LINEAGE

### v1.4 / v1.4.1 — fire-angle redirect proved active
`CSGOVR_WEAPON_BULLET_AIM_PROOF_RESULT_20260826_230123.zip`

Retained runtime facts from the v1.4 family:
- `FX_FIREBULLETS_HOOK_ACTIVE=True`
- client redirects `580`
- server redirects `580`
- client/server coherence seen
- the visual-proof runner used `sv_showimpacts`, no-spread and infinite ammo only to expose trajectory clearly

Class: **PROVEN fire-angle redirection path / controller-linked trajectory evidence**, but not the final muzzle-axis solution.

### v1.6.1 — safe rollback-forward
Package SHA256:
`8BE519A71EFCF7E13660DDC392703209F02707926E6B07E29B75D455C20B6175`

Cold exact facts:
- retail final viewmodel-root angles are written at `self+0xD0/+0xD4/+0xD8`
- corresponding write signatures were cold-verified
- attachment query disabled
- origin mutation absent
- stock fire origin preserved
- static audit `120/120 PASS`
- cold verify `12/12 PASS`

User observation from the attempted v1.6.1 run:
- textures stable again
- shot sound present again
- bullets still not where expected

Therefore the v1.6.1 architecture restored render/audio safety, but **absolute post-Calc viewmodel-root angle is not equivalent to the final desired retail fire-angle semantic**.

### v1.7.0 — FRESH EPOCH FIRE DELTA
Instead of replacing the retail fire angle absolutely, v1.7 retains the game's original `GetFinalAimAngle()` relation and transports it into the RIGHT-Touch basis.

At slot250:
- `Rs = rotation(stock EyeAngles)`
- `Rv = rotation(VR hand-owned EyeAngles)`
- publish `D = Rv * transpose(Rs)`

At exact client/server `FX_FireBullets`:
- read original retail fire-angle matrix `Rf`
- compute `RvrFire = D * Rf`

Equivalent:
`RvrFire = Rv * (transpose(Rs) * Rf)`

Safety contract:
- `[ESP+4] vOrigin` never mutated
- stock `Weapon_ShootPosition()` retained
- attachment / GetAttachment / SetupBones remains STOPPED
- non-finite / invalid data fails closed
- runtime implementation uses per-slot sequence protection and freshness by controller sample birth

Executed result:
`CSGOVR_WEAPON_FRESH_EPOCH_FIRE_DELTA_RESULT_20260827_143029.zip`

Runtime counts:
- fire-delta publishes `3120`
- client applies `229`
- server applies `231`
- stale fallbacks `0`
- invalid fallbacks `117` (fail-closed, not applied as valid data)
- controller unique sample advances `960`
- duplicate polls `3256`
- CalcVM stale-sample rejects `22`
- max applied age `78 ms`
- origin redirects `0/0`
- game remained running

Class: **PASS for the fire-delta freshness/coherence architecture.**

## 6. MUZZLE ORIGIN / MATRIX — STOPPED PATH

### v1.5 muzzle-origin attempt
`CSGOVR_WEAPON_MUZZLE_ORIGIN_RESULT_20260827_003016.zip`

Observed/retained:
- textures stable
- recoil present
- trigger firing present
- bullet hook active
- infinite ammo in that test came from runner command, not from the hook
- muzzle origin produced `0/0` because the attachment name/path was wrong
- visual bullet direction was not closed

Class: **PARTIAL / muzzle-origin FAIL**.

### v1.6 FINAL MUZZLE MATRIX
`CSGOVR_WEAPON_FINAL_MUZZLE_MATRIX_RESULT_20260827_012837.zip`

The matrix lookup itself worked (`attachment "1"`, `FINAL_MATRIX_RATE=1.0`), but the integration was not playable:
- textures disappeared
- shot sounds disappeared / degraded
- client/server fire calls diverged (`119` vs `361`)
- game was no longer running after snapshot

Calling render/viewmodel attachment matrix logic from inside `CalcViewModelView` crossed the render/gameplay boundary and contaminated render/audio/fire behavior. A first-person attachment matrix is subject to viewmodel-FOV/render formatting and is not automatically a safe world-space gameplay fire origin.

Decision:
**STOP v1.6 render-attachment/muzzle-matrix path. Do not patch it forward.**

Fire origin stays stock until a new narrow gameplay-space origin owner is independently proven.

## 7. T14 PVS / VISIBILITY QUALITY LANE — SEPARATE

Latest external result:
`CSGOVR_T14_EXTERNAL_RESULT_20260827_143839.zip`

This is a separate culling/PVS/visibility lane, not a replacement for weapon/fire state.

The T14 injection identifies itself as:
`CSGOVR T14 REAL MULTI-ORIGIN PVS v1.2.0`

It cold-links:
- SetUpView post-hook `client+0x0038C9C0`
- parent `client+0x0038C180`
- callsite `client+0x0038C192`
- frustum call `client+0x0038C5A1`
- retained main RenderView `client+0x003A5280`

The collected T2R3 summary reports:
- `VERDICT=FAIL`
- `FIRST_MISSING_EDGE=ACTIVE_DURATION_NOT_REACHED`
- engine/drawing connected
- `10245` RenderViews
- LEFT/RIGHT/MONITOR `3415/3415/3415`
- invalid schedule `0`
- pairs submitted `3415`
- dropped `0`
- mixed `0`
- right controller pose samples `3166`
- XR retry PASS on first attempt

Interpretation: useful infrastructure/transport evidence was collected, but T14 did **not** satisfy its own active-duration scientific gate. Do not promote its PVS hypothesis to PROVEN.

Prepared later quality tool:
`CSGOVR_T15_PACING_CLEAN_AB_v1.3.0_120S_ONECLICK.zip`

Class: **UNRUN / prepared**, unless a later executed RESULT is supplied.

## 8. LATEST COLD / PREPARED TOOLS — NOT EVIDENCE BY THEMSELVES

These exist in the 2026-08-27 library but are not promoted without executed output:
- `CSGOVR_COLD_SLOT249_251_LAG_OWNER_v3.1.1.zip`
- `CSGOVR_FX_FIRE_ABI_SLOT_CLOSURE_v1.0.0_COLD_ONLY_ONECLICK.zip`
- `CSGOVR_T15_PACING_CLEAN_AB_v1.3.0_120S_ONECLICK.zip`

Prepared BAT/ZIP != RESULT.

## 9. CURRENT STATE

### PROVEN / RETAIN
- exact world/HMD M3.2B GOLDEN
- Quest asymmetric projection and full CViewSetup 0x198 lineage
- exact slot250 `CPredictedViewModel::CalcViewModelView` owner
- RIGHT Touch hand-follow through stock viewmodel path
- exact client/server `FX_FireBullets` functions and call ABI
- stock fire origin should remain untouched in the current safe architecture
- v1.7 fresh-epoch delta publication/application and client/server coherence PASS
- render/audio stability recovered when render-attachment origin mutation is removed

### FALSIFIED / STOPPED
- dynamic vtable search limited to 0..239
- DrawModel weapon-transform mutation as the active hand route
- v1.6 render attachment / muzzle matrix called inside CalcViewModelView
- absolute v1.6.1 post-Calc viewmodel-root angle as a complete substitute for retail `GetFinalAimAngle`
- treating T14 as PASS when its own active-duration gate failed

### STILL UNKNOWN / NOT FULLY CLOSED
1. Exact user-visible barrel-axis accuracy of the executed v1.7 fire-delta result.
2. A safe gameplay-space muzzle-origin owner, if physical muzzle origin is still required.
3. Independent LEFT skeletal/offhand hand path (`NOT_IMPLEMENTED` in latest result).
4. PVS/culling T14 hypothesis beyond infrastructure evidence.
5. Tearing / pacing / cadence quality.
6. HUD, comfort, release/performance regression.

## 10. STOP RULE

Do not reopen transport, Quest projection, world origin, HMD rotation or basic positional 6DoF without contradictory evidence.

Do not return to render-space `GetAttachment/SetupBones` muzzle-origin mutation merely because bullet alignment still needs refinement.

For weapon work, preserve the latest safe chain:
`stock world/HMD GOLDEN -> exact slot250 hand-follow -> original retail fire angle -> fresh VR eye-basis delta -> exact FX_FireBullets angle only -> stock Weapon_ShootPosition origin`.

For pacing/PVS work, keep it in a separate lane and do not contaminate weapon causality.
