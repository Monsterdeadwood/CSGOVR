# CSGOVR — RESULT INDEX

Canonical authority: `CSGOVR_KNOWLEDGE_BASE_CANONICAL_20260823.md`.
Evidence labels must remain exact: GOLDEN / PROVEN / USER-CONFIRMED / SUPPORTED / PENDING / INFRASTRUCTURE / FALSIFIED / INVALID / SUPERSEDED.

## GOLDEN — `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`
**Class:** GOLDEN

- two real world streams are proven
- pass1 LEFT, pass2 RIGHT, pass3 monitor
- exact full `CViewSetup` copy size `0x198`
- old `0x14C` full-size interpretation is SUPERSEDED

## `CSGOVR_PROJECTION_TRUTH_RESULT_20260821_035411`
**Class:** PROVEN projection semantics

- HLAE stereo GOLDEN preserved as BASE
- off-center fields:
  - top `+0x138`
  - bottom `+0x13C`
  - left `+0x140`
  - right `+0x144`
  - `m_bOffCenter = flags+0x148 bit0`
- controlled +/- canary phases exercised the fields for LEFT/RIGHT eye passes

## `CSGOVR_CUSTOM_PROJECTION_MATRIX_RESULT_20260821_042218.zip`
**Class:** PROVEN custom projection path

- custom projection enable candidate/proven path at `+0x68`
- `VMatrix +0x6C..+0xAB`
- structurally anchored by fov at `+0xB0`
- M00/M02/M11/M12 semantics causally measured in the broader projection result family
- exact HLAE `DoRenderView` seam RVA `0x00022340`
- full `CViewSetup` size `0x198`

## `CSGOVR_OPENXR_FOV_DIAGNOSTIC_20260821_150332.zip`
**Class:** PROVEN XR INPUT

- Meta/Oculus OpenXR 1.205.0 + Quest 3
- two views, `viewStateFlags=0xF`
- LEFT FOV L=-54°, R=+40°, U=+44°, D=-55°
- RIGHT FOV L=-40°, R=+54°, U=+44°, D=-55°
- recommended eye size 1504x1616
- passive eye separation about 64.780 mm
- derived asymmetric coefficients preserved in canonical docs

## `CSGOVR_NATIVE_STEREO_SEAM_RESULT_20260821_041928` corrected postflight
**Class:** PROVEN/SUPPORTED split

- full 0x198 eye copies confirmed
- late projection survives
- tested late origin eye shift did not change the final captured raster

Interpretation: do not falsify eye translation generally; only that tested late origin intervention point failed to authoritatively move final raster.

## `CSGOVR_TTD_FRUSTUM_CACHE_REPLAY_RESULT_20260822_012630.zip`
**Class:** FALSIFIED + HARVEST

Hypothesis falsified:
`client.dll+0x0525BA60` shared cache is reused inside both LEFT and RIGHT eye marker regions.

Observed accesses to watched cache region were outside the LEFT/RIGHT marker brackets. Do not repeat this hypothesis without contradictory evidence.

Useful harvest from same line:
- GeneratePerspectiveFrustum `client.dll+0x0086E9C0`
- frustum/helper lane writers starting `client.dll+0x0086EB3C`
- concrete downstream pre-eye readers including `client.dll+0x003A16A6` and `+0x003A01E4`

## `CSGOVR_TTD_REPLAY_FRUSTUM_READER_OWNER_3A16A6_RECOVERED_RESULT_20260823_160154.zip`
**Class:** PROVEN evidence packet / destination semantics PENDING

- target `client.dll+0x003A16A6..+0x003A16DC` hit
- call stack captured
- bounded 24-step dataflow recovered
- auto store candidates: 3
- scientific recovery packet complete

Exact semantic destination object/field is still not classified.

## `CSGOVR_TTD_FRUSTUM_ARG2_PERSISTENCE_RESULT_20260823_164733.zip`
**Class:** PROVEN + FALSIFIED split

- PROVEN: first `0x40` bytes of helper arg2 undergo a temporary in-place transpose and are restored
- FALSIFIED: persistent output hypothesis in arg2 `+0x40..+0x1FF`

The earlier parser-generated FALSIFIED verdict for the first 0x40 transpose is INVALID/SUPERSEDED.

## `CSGOVR_CAUSAL_ORIGINX_ZERO_CONTROL_RESULT_20260823_043045.zip`
**Class:** INFRASTRUCTURE

Retained seams:
- writer target `client.dll+0x0038D123`
- `CViewSetup.origin.x +0xB8`
- next true RenderView `client.dll+0x003A5280`
- retained persistent/global CView candidate `client.dll+0x052632CC`
- retained origin global candidate `client.dll+0x05263384`

Byte integrity passed, but wrapper verdict is INFRASTRUCTURE, not camera ownership/control proof.

## `CSGOVR_CAUSAL_YAW_FINAL_POSE_CONTROL_RESULT_20260823_071600.zip`
**Class:** INFRASTRUCTURE

- seam `client.dll+0x0038D15B`
- yaw `CViewSetup+0xC8`
- global commit `client.dll+0x0038D1DE`
- retained `g_vecRenderAngles.y` candidate `client.dll+0x05245AB8`
- byte integrity only; not final-pose proof

## `CSGOVR_FULL_EXPERIMENTAL_FAILURE_20260821_022832.zip`
**Class:** INVALID RUN / BUILD-WORKFLOW FAILURE

- contaminated CSGOVR build pulled Crysis2VR path/source
- immediate Zig precheck failure
- proves nothing about camera/projection/culling/OpenXR/viewmodel behavior

## Current engineering handoff
**GATE:** integrate known Quest asymmetric projection into the known-good HLAE eye path and validate in-headset/same-frame.

If a concrete culling blocker survives, resume from `client.dll+0x003A16A6` + CSimpleWorldView lineage. Do not reopen broad archaeology or the falsified shared-cache hypothesis.
