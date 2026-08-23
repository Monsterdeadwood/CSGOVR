# CSGOVR — PROVEN

## Evidence authority
Canonical source: `CSGOVR_KNOWLEDGE_BASE_CANONICAL_20260823.md`, augmented by the 2026-08-24 cross-branch sync. Do not silently promote SUPPORTED/INFRASTRUCTURE candidates.

## OpenXR / Quest 3 input
- Meta/Oculus OpenXR 1.205.0 + Quest 3 diagnostic path succeeded.
- `xrLocateViews` returned two valid views with `viewStateFlags=0xF`.
- LEFT FOV: L=-54°, R=+40°, U=+44°, D=-55°.
- RIGHT FOV: L=-40°, R=+54°, U=+44°, D=-55°.
- Recommended eye size: 1504x1616.
- Passive eye separation observed: about 64.780 mm.
- Projection coefficients:
  - LEFT M00≈0.9027383, M02≈-0.2425126, M11≈0.8354789, M12≈-0.1931875
  - RIGHT M00≈0.9027383, M02≈+0.2425126, M11≈0.8354789, M12≈-0.1931875

Provenance: `CSGOVR_OPENXR_FOV_DIAGNOSTIC_20260821_150332.zip`.

## Stereo GOLDEN / HLAE
- GOLDEN: `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`.
- Two real world streams: pass1 LEFT, pass2 RIGHT, pass3 monitor.
- Exact full `CViewSetup` size `0x198`; prior `0x14C` full-size claim superseded.
- HLAE `CAfxStreams::DoRenderView = +0x00022340`.
- HLAE `CViewSetup_csgo` copy ctor `+0x00014BA0`, copies through byte `0x197`.
- HLAE control offsets: `this+0x30 m_ForceCacheFullSceneState`, `this+0x50 m_DoRenderViewCount`.
- v0.5.5 local eye model uses nominal half-eye `1.2598425 Source units` with LEFT=`-right`, RIGHT=`+right`.

Recovered physical source:
- `runtime/golden_v0.5.5/src/eye_patch.c`
- `runtime/golden_v0.5.5/src/injector.c`

Provenance: `CSGOVR_HLAE_EYE_PATCH_RESULT_20260821_015737.zip` plus exact source recovery already committed to GitHub.

## GOLDEN visual parallax — reconciled evidence
Re-analysis of original v0.5.5 BMPs from `CSGOVR_HLAE_EYE_PATCH_RESULT_20260821_015737.zip` shows:
- same-eye consecutive captures are extremely stable relative to L/R difference;
- LEFT/RIGHT difference is predominantly horizontal and depth-dependent;
- median horizontal sparse-flow displacement was about `-2.94 px`, with a broad depth-dependent spread (roughly `-6.39..-0.60 px` in the 10th–90th percentiles in the branch analysis);
- median vertical displacement was approximately zero.

This supports genuine geometric stereo in the actual v0.5.5 path. It coexists with the separate negative result from one late DUAL_ORIGIN intervention; see `docs/FALSIFIED.md` and `docs/STATE.md` for reconciliation.

## Retail CViewSetup map
PROVEN retail fields:
- `+0x68 m_bCustomProjMatrix`
- `+0x6C..+0xAB m_matCustomProjMatrix`
- `+0xB0 world FOV`
- `+0xB4 viewmodel FOV`
- `+0xB8 origin`
- `+0xC0 origin.z` subfield
- `+0xC4 angles`
- `+0xC8 yaw` subfield
- `+0xD0/+0xD4 world zNear/zFar`
- `+0xD8/+0xDC viewmodel zNear/zFar`
- `+0xE0 aspect`
- `+0x138/+0x13C/+0x140/+0x144` off-center T/B/L/R
- `+0x148 bit0 m_bOffCenter`

Strong source-layout candidates, not retail proof:
- `+0x34 m_bCustomViewMatrix`
- `+0x38..+0x67 m_matCustomViewMatrix`
- `+0xAC m_pCSMVolumeCuller`

## CPU -> GPU / shader provenance
Provenance: `CSGOVR_CORRECTNESS_RESULT_20260821_011601.zip` and projection provenance family.

- `CViewSetup.origin` maps exactly to VS `c2 / CAMERA_POS` across the causal corpus.
- `c4..c7` = MODELVIEWPROJ family.
- `c8..c11` = VIEWPROJ family.
- `c17` retained branch constant.
- `c58..c60` bounded model candidate family.
- `WORLD_X_PAIR` proved a small RenderView origin translation can change GPU camera state, pixels and draw membership.

## Custom projection
Provenance: `CSGOVR_CUSTOM_PROJECTION_MATRIX_RESULT_20260821_042218.zip` / v0.6.1.

- custom projection bool `+0x68` and matrix `+0x6C..+0xAB` are PROVEN.
- M00/M02/M11/M12 semantics were causally measured.
- v0.6.1 used `PoseFreeze=TRUE`, `BotsKicked=TRUE`.
- same-eye temporal drift in frozen phases was effectively negligible for the intended measurement.
- at 960x540:
  - `M02 +0.10 -> about -48 px X`
  - `M02 -0.10 -> about +48 px X`
  - `M12 +0.10 -> about +27 px Y`
  - `M12 -0.10 -> about -27 px Y`
  - M00/M11 0.80 canaries produced the expected single-axis scale changes.
- Source/D3D Z convention retained: `M22=zFar/(zNear-zFar)`, `M23=zNear*zFar/(zNear-zFar)`, `M32=-1`, `M33=0`.

Verified child donor source is preserved under `runtime/children/v0.6.1_projection_oracle/`; test-only freeze/INI/canary/FOV-widening behavior is not the production architecture.

## Current byte-locked G3 implementation candidate
Remote cross-branch work already produced:
- `experiments/quest_projection_child/eye_patch_quest_projection.c`
- `experiments/quest_projection_child/QUEST_PROJECTION_MINIMAL.patch`
- `tools/verify_quest_projection_child.py`
- `.github/workflows/evidence-integrity.yml`

This is a prepared implementation candidate, **not a PROVEN runtime RESULT**. Its design preserves v0.5.5 origin behavior and adds only the known Quest asymmetric custom matrix to the local eye copy.

## Culling / pre-eye dataflow
- `client+0x003A16A6..+0x003A16DC` is a concrete pre-eye reader/dataflow site with recovered call stack and bounded dataflow. Destination semantics remain PENDING.
- second retained reader: `client+0x003A01E4..+0x003A021A`.
- `GeneratePerspectiveFrustum = client+0x0086E9C0`, lane writers around `+0x0086EB3C`.
- first helper arg2 `0x40` bytes undergo temporary in-place transpose and restore.
- CSimpleWorldView main-world lineage PROVEN:
  - vtable `client+0x00BF4434`
  - Draw `client+0x003B0870`
  - retained `+0x168 IWorldRenderList*`, `+0x16C CClientRenderablesList*`, `+0x170 ClientWorldListInfo_t*`
  - lazy hydration `+0x003B0067` / `+0x003B0083`

## Important non-PROVEN retained seams
INFRASTRUCTURE/candidates only:
- `client+0x0038D123` origin.x writer seam
- `client+0x052632CC` persistent/global CView candidate
- `client+0x05263384` origin-global candidate
- `client+0x0038D15B` yaw seam
- `client+0x0038D1DE` global commit candidate
- `client+0x05245AB8` g_vecRenderAngles.y candidate

Do not upgrade these without a new causal experiment.
