# CSGOVR — PROVEN

Snapshot: **2026-08-27 cross-tab sync.**

Evidence authority: exact frozen-retail evidence, user-confirmed runtime where stated, and `research/CROSS_TAB_SYNC_20260827.md`. Do not silently promote SUPPORTED / PARTIAL / UNRUN candidates.

## CURRENT WORLD / HMD GOLDEN
Package:
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256:
`5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Evidence:
`CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`

User visual verdict: **VR + full HMD 6DoF works.**

Tearing / cadence remains a quality problem; it does not reopen world-camera ownership.

## EXACT RETAIL VIEWMODEL OWNER — PROVEN
Frozen client SHA256:
`183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`

Exact functions:
- `CBaseViewModel::CalcViewModelView = client.dll + 0x001B7F80`
- `CPredictedViewModel::CalcViewModelView = client.dll + 0x003420E0`
- exact runtime `CPredictedViewModel` vtable slot = `250`

ABI:
`__thiscall(this, owner, Vector const&, QAngle const&)`, callee `ret 0x0C`.

Independent fingerprints:
- predicted override references `cl_use_new_headbob`
- predicted override directly calls base `0x001B7F80`
- base references exact `viewmodel_offset_x/y/z` ConVar objects

Exact ConVar RVAs:
- `viewmodel_offset_x = 0x0325CCC0`
- `viewmodel_offset_y = 0x0325CD28`
- `viewmodel_offset_z = 0x0325CD90`
- `cl_use_new_headbob = 0x0526F910`

## RIGHT TOUCH HAND / WEAPON FOLLOW — PROVEN RUNTIME PATH
The stable route is the stock viewmodel path through exact slot250, not DrawModel mutation.

Retained facts:
- RIGHT grip owns the grabbed viewmodel translation
- RIGHT aim owns orientation
- first valid pose performs rigid SE(3) grab calibration
- stock arms / stock weapon render route retained
- later v1.7 RESULT still reports `WEAPON_HAND_FOLLOW=PASS`

The v1.3.0 static audit was `85/85 PASS`.
Package SHA256:
`03C75627A29E5F0B29D410D0F82D7861B88680CF223897ABC348CF1204C49B72`.

## EXACT RETAIL FIRE SEAM / ABI — PROVEN
Frozen server SHA256:
`699757BB5C8DDB71193F68DABA22F59F697491DDBD768BC8656AC2CE435C4DB8`

Exact functions:
- client `FX_FireBullets = client.dll + 0x0044DC90`
- server `FX_FireBullets = server.dll + 0x004C4650`

Exact callsites:
- client `client.dll + 0x003F6989`
- server `server.dll + 0x005A7B09`

Call ABI:
- `ECX = iPlayerIndex`
- `EDX = nItemDefIndex`
- `[ESP+4] = vOrigin*`
- `[ESP+8] = vAngles*`
- 9 more stack dwords
- caller cleanup `ADD ESP,0x2C`

This proves a narrow direction seam where `vAngles` can be changed while leaving `vOrigin` / stock `Weapon_ShootPosition()` untouched.

## FIRE ANGLE REDIRECTION — PROVEN ACTIVE
`CSGOVR_WEAPON_BULLET_AIM_PROOF_RESULT_20260826_230123.zip` retained:
- `FX_FIREBULLETS_HOOK_ACTIVE=True`
- client angle redirects `580`
- server angle redirects `580`
- client/server coherence seen

The visual-proof runner used `sv_showimpacts`, no-spread and infinite ammo as test visibility controls. Infinite ammo in that proof was not a hook defect.

Scope: this proves the fire-angle seam and controller-linked trajectory activity, not a final physical muzzle-origin solution.

## v1.7 FRESH-EPOCH FIRE DELTA — PROVEN RUNTIME COHERENCE / FRESHNESS
Evidence:
`CSGOVR_WEAPON_FRESH_EPOCH_FIRE_DELTA_RESULT_20260827_143029.zip`

Result status reports:
- `WEAPON_FRESH_EPOCH_FIRE_DELTA=PASS`
- `WEAPON_HAND_FOLLOW=PASS`
- `WEAPON_BULLET_AIM=PASS`
- `WORLD_STEREO_HMD=PASS`
- exact CalcViewModel guard/hook active
- exact client/server FX_FireBullets hooks active
- client/server angle redirects `229/231`
- origin redirects `0/0`
- fire-delta publishes `3120`
- client/server applies `229/231`
- stale fallbacks `0`
- torn-read fallbacks `0`
- max applied sample age `78 ms`
- freshness contract PASS
- normal ammo/spread retained
- game remained running after snapshot

The v1.7 semantic transplant is:
- `Rs = rotation(stock EyeAngles)`
- `Rv = rotation(VR hand-owned EyeAngles)`
- `D = Rv * transpose(Rs)`
- read original retail fire angle `Rf`
- `RvrFire = D * Rf`

Equivalent:
`RvrFire = Rv * (transpose(Rs) * Rf)`.

This preserves the original retail `GetFinalAimAngle` relationship instead of replacing it absolutely.

Scope caveat: automatic runtime PASS proves the architecture, freshness and client/server coherence. Direct user-visible final barrel-axis accuracy for that exact v1.7 run remains a separate visual verdict unless explicitly confirmed.

## STOCK FIRE ORIGIN — CURRENT SAFE OWNER
In the current v1.7 line:
- client fire-origin redirects = `0`
- server fire-origin redirects = `0`
- `[ESP+4]` is not mutated
- `Weapon_ShootPosition()` remains the gameplay/spatial-audio origin owner

This is proven as the current safe behavior. It does **not** prove that stock origin is the ideal final VR muzzle origin; only that it preserves the stable baseline and is the active owner.

## v1.6.1 SAFETY RECOVERY — SUPPORTED + COLD-PROVEN COMPONENTS
Cold exact binary confirms final viewmodel-root angle writes at:
- `self+0xD0`
- `self+0xD4`
- `self+0xD8`

v1.6.1 checks:
- static audit `120/120 PASS`
- cold verify `12/12 PASS`
- attachment query removed
- origin mutation absent
- stock fire origin retained

User observation from that attempt:
- textures stable again
- shot sound present again
- bullets still not where expected

Therefore render/audio safety recovery is retained, while the absolute-root-angle fire semantic is classified in `FALSIFIED.md`.

## OpenXR / Quest 3 input
- Meta/Oculus OpenXR diagnostic path succeeded.
- `xrLocateViews` returned two valid views with `viewStateFlags=0xF`.
- LEFT FOV: L=-54°, R=+40°, U=+44°, D=-55°.
- RIGHT FOV: L=-40°, R=+54°, U=+44°, D=-55°.
- Recommended eye size: 1504x1616; max 3008x3232.
- Passive eye separation: about 64.780414 mm.
- Projection coefficients:
  - LEFT M00≈0.9027383, M02≈-0.2425126, M11≈0.8354789, M12≈-0.1931875
  - RIGHT M00≈0.9027383, M02≈+0.2425126, M11≈0.8354789, M12≈-0.1931875

Provenance: `CSGOVR_OPENXR_FOV_DIAGNOSTIC_20260821_150332.zip`.

## Native SourceVR HMD / eye-space scale
**PROVEN at Valve SourceVR source semantics.**

Valve SourceVR consumes OpenVR eye-to-head transforms with `inchesPerMeter = 39.3700787`.

Therefore for the native SourceVR HMD/eye camera path:
- `1 meter = 39.3700787 Source units`
- `1 Source unit = 1 inch`
- measured Quest 64.780414 mm ≈ `2.55041 Source units` total / `1.27520` half-IPD

This rules out the temporary 35% scale-error hypothesis.

## Stereo GOLDEN / HLAE
Historical GOLDEN:
`CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`.

Proven facts:
- two real world streams: pass1 LEFT, pass2 RIGHT, pass3 monitor
- exact full `CViewSetup` size `0x198`
- HLAE `CAfxStreams::DoRenderView = +0x00022340`
- HLAE `CViewSetup_csgo` copy ctor `+0x00014BA0`, copies through byte `0x197`
- HLAE control offsets: `this+0x30`, `this+0x50`, `this+0x54`
- v0.5.5 local eye model nominal half-eye `1.2598425 Source units`

Recovered physical source remains under `runtime/golden_v0.5.5/src/`.

## GOLDEN visual parallax — reconciled evidence
Re-analysis of original v0.5.5 captures shows:
- same-eye consecutive captures are much more stable than L/R difference
- LEFT/RIGHT difference is predominantly horizontal and depth-dependent
- median horizontal sparse-flow displacement about `-2.94 px`
- median vertical displacement approximately zero

This supports genuine geometric stereo in the actual v0.5.5 path.

## Retail `CViewSetup` map
PROVEN retail fields:
- `+0x68 m_bCustomProjMatrix`
- `+0x6C..+0xAB m_matCustomProjMatrix`
- `+0xB0 world FOV`
- `+0xB4 viewmodel FOV`
- `+0xB8 origin`
- `+0xC0 origin.z`
- `+0xC4 angles`
- `+0xC8 yaw`
- `+0xD0/+0xD4 world zNear/zFar`
- `+0xD8/+0xDC viewmodel zNear/zFar`
- `+0xE0 aspect`
- `+0xE4..+0xF8` observed DoF defaults
- `+0x100` motion blur mode
- `+0x138/+0x13C/+0x140/+0x144` off-center T/B/L/R
- `+0x148 bit0 m_bOffCenter`

Strong source-layout candidates, not retail proof:
- `+0x34 m_bCustomViewMatrix`
- `+0x38..+0x67 m_matCustomViewMatrix`
- `+0xAC m_pCSMVolumeCuller`

## CPU -> GPU / shader provenance
Provenance: `CSGOVR_CORRECTNESS_RESULT_20260821_011601.zip` and projection provenance family.

- `CViewSetup.origin` maps exactly to VS `c2 / CAMERA_POS`
- `c4..c7` = MODELVIEWPROJ family
- `c8..c11` = VIEWPROJ family
- `c17` retained branch constant
- `c58..c60` bounded model candidate family
- WORLD_X_PAIR proved RenderView origin translation changes GPU camera state, pixels and draw membership
- D3D9 `SetPixelShader` slot 107 and `SetPixelShaderConstantF` slot 109 work

## Custom projection
Provenance: `CSGOVR_CUSTOM_PROJECTION_MATRIX_RESULT_20260821_042218.zip` / v0.6.1.

- custom projection bool `+0x68` and matrix `+0x6C..+0xAB` are PROVEN
- M00/M02/M11/M12 semantics causally measured
- at 960x540:
  - `M02 +0.10 -> about -48 px X`
  - `M02 -0.10 -> about +48 px X`
  - `M12 +0.10 -> about +27 px Y`
  - `M12 -0.10 -> about -27 px Y`
- Source/D3D Z convention retained

## Capture determinism
An old sequential comparator is INVALID for quantitative projection/origin claims because temporal same-eye change dominated some L/R differences.

The v0.6.1 internal freeze/capture path reached within-phase MAE around `0.0000..0.0026 / 255`, proving deterministic same-state capture is possible.

If capture truth becomes a blocker, require same-eye control `<1%` before quantitative L/R claims.

## Culling / pre-eye dataflow
Retained exact evidence:
- `client+0x003A16A6..+0x003A16DC` concrete pre-eye reader/dataflow site
- second reader `client+0x003A01E4..+0x003A021A`
- `GeneratePerspectiveFrustum = client+0x0086E9C0`
- first helper arg2 `0x40` undergoes temporary in-place transpose and restore
- CSimpleWorldView main-world lineage:
  - vtable `client+0x00BF4434`
  - Draw `client+0x003B0870`
  - `+0x168 IWorldRenderList*`
  - `+0x16C CClientRenderablesList*`
  - `+0x170 ClientWorldListInfo_t*`
  - lazy hydration `+0x003B0067/+0x003B0083`
  - current slot `client+0x05248ED4`
  - arrays `+0x0524A2F4/+0x0524A2F8/+0x0524A2FC`, stride `0x13F0`

## Exact HLAE / AdvancedFX source-build findings
At AdvancedFX commit `b97636852b8eecae09285b5a386192bb285638eb`:
- Win32 AfxHookSource defines `AFX_INTEROP`
- stream order: sky-view copy / SKY GetMatricesForView -> NORMAL GetMatricesForView -> OnRenderBegin -> DoRenderView
- normal stream reuses shared `const CViewSetup& view`
- DoRenderView increments `m_DoRenderViewCount` before real Source RenderView
- older AfxInterop path can open shared D3D9 texture/surface and push as render target
- D3D9 interop GPU wait query exists
- AdvancedFX D3D11 companion lineage creates/exports shared resources

Protocol-version caveat remains: this does not prove modern AfxInterop is plug-and-play per-eye VR transport.

Detailed provenance: `research/HLAE_INTEROP_FINDINGS.md`.

## Important non-PROVEN retained seams
INFRASTRUCTURE/candidates only:
- `client+0x0038D123` origin.x writer seam
- `client+0x052632CC` persistent/global CView candidate
- `client+0x05263384` origin-global candidate
- `client+0x0038D15B` yaw seam
- `client+0x0038D1DE` global commit candidate
- `client+0x05245AB8` g_vecRenderAngles.y candidate

## T14 PVS scope caveat
`CSGOVR_T14_EXTERNAL_RESULT_20260827_143839.zip` collected valid infrastructure counters but failed its own `ACTIVE_DURATION_NOT_REACHED` gate. Its PVS hypothesis is therefore **not** listed as PROVEN here.

## Current detailed delta
See:
`research/CROSS_TAB_SYNC_20260827.md`
