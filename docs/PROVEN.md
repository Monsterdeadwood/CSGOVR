# CSGOVR — PROVEN

## Evidence authority
Canonical source: `CSGOVR_KNOWLEDGE_BASE_CANONICAL_20260823.md`, augmented by the 2026-08-24 cross-branch sync. Do not silently promote SUPPORTED/INFRASTRUCTURE/PENDING candidates.

## OpenXR / Quest 3 input
- Meta/Oculus OpenXR 1.205.0 + Quest 3 diagnostic path succeeded.
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

Valve `CSourceVirtualReality::GetMidEyeFromEye()` consumes OpenVR `GetEyeToHeadTransform()` and `OpenVRToSourceCoordinateSystem()` uses `inchesPerMeter = 39.3700787`, with comments explicitly mapping OpenVR meters to Source inches.

Therefore for the native SourceVR HMD/eye camera path:
- `1 meter = 39.3700787 Source units`
- `1 Source unit = 1 inch`
- measured Quest 64.780414 mm ≈ `2.55041 Source units` total / `1.27520` half-IPD
- v0.5.5 nominal half-eye `1.2598425` is only about 1.2% lower

This rules out the temporary 35% scale-error hypothesis.

## Stereo GOLDEN / HLAE
- GOLDEN: `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`.
- Two real world streams: pass1 LEFT, pass2 RIGHT, pass3 monitor.
- Exact full `CViewSetup` size `0x198`; prior `0x14C` full-size claim superseded.
- HLAE `CAfxStreams::DoRenderView = +0x00022340`.
- HLAE `CViewSetup_csgo` copy ctor `+0x00014BA0`, copies through byte `0x197`.
- HLAE control offsets: `this+0x30 m_ForceCacheFullSceneState`, `this+0x50 m_DoRenderViewCount`, `this+0x54 m_ShowRenderViewCount`.
- v0.5.5 local eye model uses nominal half-eye `1.2598425 Source units` with LEFT=`-right`, RIGHT=`+right`.

Recovered physical source:
- `runtime/golden_v0.5.5/src/eye_patch.c`
- `runtime/golden_v0.5.5/src/injector.c`

Provenance: `CSGOVR_HLAE_EYE_PATCH_RESULT_20260821_015737.zip` plus exact source recovery committed to GitHub.

## GOLDEN visual parallax — reconciled evidence
Re-analysis of original v0.5.5 BMPs from `CSGOVR_HLAE_EYE_PATCH_RESULT_20260821_015737.zip` shows:
- same-eye consecutive captures are extremely stable relative to L/R difference;
- LEFT/RIGHT difference is predominantly horizontal and depth-dependent;
- median horizontal sparse-flow displacement about `-2.94 px`, broad 10th–90th percentile roughly `-6.39..-0.60 px` in branch analysis;
- median vertical displacement approximately zero.

This supports genuine geometric stereo in the actual v0.5.5 path. It coexists with the separate negative result from one late DUAL_ORIGIN intervention.

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
- `+0xE4..+0xF8` observed DoF defaults `20,100,250,1000,10,5`
- `+0x100` motion blur mode, observed default 2
- `+0x138/+0x13C/+0x140/+0x144` off-center T/B/L/R
- `+0x148 bit0 m_bOffCenter`

Strong source-layout candidates, not retail proof:
- `+0x34 m_bCustomViewMatrix`
- `+0x38..+0x67 m_matCustomViewMatrix`
- `+0xAC m_pCSMVolumeCuller`

## CPU -> GPU / shader provenance
Provenance: `CSGOVR_CORRECTNESS_RESULT_20260821_011601.zip` and projection provenance family.

- `CViewSetup.origin` maps exactly to VS `c2 / CAMERA_POS`.
- `c4..c7` = MODELVIEWPROJ family.
- `c8..c11` = VIEWPROJ family.
- `c17` retained branch constant.
- `c58..c60` bounded model candidate family.
- `WORLD_X_PAIR` proved a small RenderView origin translation can change GPU camera state, pixels and draw membership.
- D3D9 `SetPixelShader` vtable slot 107 and `SetPixelShaderConstantF` slot 109 work.

## Custom projection
Provenance: `CSGOVR_CUSTOM_PROJECTION_MATRIX_RESULT_20260821_042218.zip` / v0.6.1.

- custom projection bool `+0x68` and matrix `+0x6C..+0xAB` are PROVEN.
- M00/M02/M11/M12 semantics were causally measured.
- v0.6.1 used `PoseFreeze=TRUE`, `BotsKicked=TRUE`.
- at 960x540:
  - `M02 +0.10 -> about -48 px X`
  - `M02 -0.10 -> about +48 px X`
  - `M12 +0.10 -> about +27 px Y`
  - `M12 -0.10 -> about -27 px Y`
  - M00/M11 0.80 canaries produced expected axis scale changes.
- Source/D3D Z convention retained: `M22=zFar/(zNear-zFar)`, `M23=zNear*zFar/(zNear-zFar)`, `M32=-1`, `M33=0`.

Verified donor source is preserved under `runtime/children/v0.6.1_projection_oracle/`; test-only freeze/INI/canary/FOV-widening behavior is not production architecture.

## Capture determinism
Two result sets must not be conflated:
- an old sequential comparator had same-eye temporal change around 62% while L/R was around 54%, so that comparator is INVALID for quantitative projection/origin claims;
- the v0.6.1 internal freeze/capture path achieved within-phase MAE around `0.0000..0.0026 / 255`, proving same-state capture can be made deterministic.

If capture truth becomes a blocker, require same-eye control `<1%` before quantitative L/R claims.

## Current byte-locked G3 implementation candidate
Remote cross-branch work already produced:
- `experiments/quest_projection_child/eye_patch_quest_projection.c`
- `experiments/quest_projection_child/QUEST_PROJECTION_MINIMAL.patch`
- `tools/verify_quest_projection_child.py`
- `.github/workflows/evidence-integrity.yml`

This is a prepared implementation candidate, **not a PROVEN runtime RESULT**. It preserves v0.5.5 origin behavior and adds only the known Quest asymmetric custom matrix to the local eye copy.

## Culling / pre-eye dataflow
- `client+0x003A16A6..+0x003A16DC` is a concrete pre-eye reader/dataflow site with recovered call stack and bounded dataflow. Destination semantics remain PENDING.
- second retained reader `client+0x003A01E4..+0x003A021A`.
- `GeneratePerspectiveFrustum = client+0x0086E9C0`, lane writers around `+0x0086EB3C`.
- first helper arg2 `0x40` bytes undergo temporary in-place transpose and restore.
- CSimpleWorldView main-world lineage PROVEN:
  - vtable `client+0x00BF4434`
  - Draw `client+0x003B0870`
  - `+0x168 IWorldRenderList*`, `+0x16C CClientRenderablesList*`, `+0x170 ClientWorldListInfo_t*`
  - lazy hydration `+0x003B0067` / `+0x003B0083`
  - current slot `client+0x05248ED4`
  - arrays `+0x0524A2F4/+0x0524A2F8/+0x0524A2FC`, stride `0x13F0`

## Exact HLAE / AdvancedFX source-build findings — 2026-08-24
**PROVEN at exact source/build-contract scope; runtime VR integration is PENDING.**

At AdvancedFX commit `b97636852b8eecae09285b5a386192bb285638eb`:
- Win32 `AfxHookSource` defines `AFX_INTEROP` and includes `AfxInterop.cpp/.h`.
- stream semantic order is sky-view copy / SKY `GetMatricesForView` -> NORMAL `GetMatricesForView` -> `OnRenderBegin` -> `DoRenderView`.
- the normal stream reuses the same shared `const CViewSetup& view`; `m_CurrentView` refers to that current view.
- `DoRenderView` increments `m_DoRenderViewCount` before the real Source `RenderView`.
- older AfxInterop drawing path can accept `sharedColorTextureHandle`, open a matching D3D9 texture/surface with `CreateTexture(...,&sharedHandle)`, push it as render target, and pop it at render end.
- `d3d9Hooks.cpp` contains an interop GPU wait query: `CreateQuery(D3DQUERYTYPE_EVENT)`, `Issue(D3DISSUE_END)`, loop `GetData(...D3DGETDATA_FLUSH)` until completion.
- AdvancedFX D3D11 companion lineage creates/exports shared D3D11 resources.

Scope caveat: server protocol v6/v7 diverges from the older replacement-target `PrepareDraw` exchange, so a modern AfxInterop companion is not proven to provide plug-and-play per-eye VR targets.

Detailed provenance: `research/HLAE_INTEROP_FINDINGS.md`.

## Important non-PROVEN retained seams
INFRASTRUCTURE/candidates only:
- `client+0x0038D123` origin.x writer seam
- `client+0x052632CC` persistent/global CView candidate
- `client+0x05263384` origin-global candidate
- `client+0x0038D15B` yaw seam
- `client+0x0038D1DE` global commit candidate
- `client+0x05245AB8` g_vecRenderAngles.y candidate

Do not upgrade these without a new causal experiment.
