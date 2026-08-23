# CSGOVR — RESULT INDEX

Authority: canonical KB + 2026-08-24 cross-branch sync.
Evidence labels: GOLDEN / PROVEN / USER-CONFIRMED / SUPPORTED / PENDING / INFRASTRUCTURE / FALSIFIED / INVALID / SUPERSEDED / UNRUN.

## GOLDEN — `CSGOVR_HLAE_EYE_PATCH_RESULT_20260821_015737.zip`
Build: `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`
Class: GOLDEN.

Proves/retains:
- pass1 LEFT world, pass2 RIGHT world, pass3 monitor
- full local CViewSetup copies `0x198`
- HLAE DoRenderView seam `+0x22340`
- copy ctor `+0x14BA0`
- nominal half-eye `1.2598425 Source units`
- original BMP re-analysis supports stable depth-dependent horizontal L/R parallax with negligible vertical disparity

Exact runtime source is committed under `runtime/golden_v0.5.5/src/`.

## `CSGOVR_CORRECTNESS_RESULT_20260821_011601.zip`
Class: PROVEN causal retail evidence.

- CViewSetup origin -> GPU VS `c2 / CAMERA_POS`
- WORLD_X_PAIR small translation changes pixels and draw membership
- projection-family constants include c4/c8 families
- important counter-evidence against treating one late DUAL_ORIGIN capture as a general eye-translation falsification

## `CSGOVR_PROJECTION_TRUTH_RESULT_20260821_035411.zip`
Class: PROVEN projection/off-center semantics.

- off-center top `+0x138`
- bottom `+0x13C`
- left `+0x140`
- right `+0x144`
- flag `+0x148 bit0`

## `CSGOVR_CUSTOM_PROJECTION_MATRIX_RESULT_20260821_042218.zip`
Class: PROVEN custom projection path / current-gate parent evidence.

- custom projection enable `+0x68`
- matrix `+0x6C..+0xAB`
- deterministic frozen camera/bots-kicked measurement
- M00/M02/M11/M12 causally measured
- at 960x540: M02 +/-0.10 produced approximately -/+48 px X; M12 +/-0.10 produced approximately +/-27 px Y
- M00/M11 0.80 scale canaries matched expected axis scaling

Verified source donor: `runtime/children/v0.6.1_projection_oracle/`.

## `CSGOVR_OPENXR_FOV_DIAGNOSTIC_20260821_150332.zip`
Class: PROVEN XR INPUT.

- Oculus OpenXR 1.205.0 + Quest 3
- `xrLocateViews=XR_SUCCESS`, 2 views, flags `0xF`
- LEFT L=-54 R=+40 U=+44 D=-55
- RIGHT L=-40 R=+54 U=+44 D=-55
- recommended eye size 1504x1616
- passive eye separation ~64.780 mm
- derived matrix coefficients preserved in docs/STATE.md and docs/PROVEN.md

## `CSGOVR_NATIVE_STEREO_SEAM_RESULT_20260821_041928`
Class: PROVEN/SUPPORTED split.

- full 0x198 copies confirmed
- late projection/off-center changes can survive
- one tested late DUAL_ORIGIN intervention did not authoritatively alter its final captured raster

Scope correction 2026-08-24:
This is a negative result for that exact late intervention/capture only. It does not cancel v0.5.5 GOLDEN parallax or WORLD_X_PAIR causal evidence.

## `CSGOVR_TTD_FRUSTUM_CACHE_REPLAY_RESULT_20260822_012630.zip`
Class: FALSIFIED + HARVEST.

FALSIFIED:
- `client+0x0525BA60` shared cache reused inside BOTH eye markers.

Harvest:
- `GeneratePerspectiveFrustum client+0x0086E9C0`
- writers around `+0x0086EB3C`
- readers `+0x003A16A6`, `+0x003A01E4`

## `CSGOVR_TTD_REPLAY_FRUSTUM_READER_OWNER_3A16A6_RECOVERED_RESULT_20260823_160154.zip`
Class: PROVEN evidence packet / destination semantics PENDING.

- target `client+0x003A16A6..+0x003A16DC` hit
- call stack captured
- bounded 24-step dataflow
- 3 store candidates
- resume only if G4 culling opens

## `CSGOVR_TTD_FRUSTUM_ARG2_PERSISTENCE_RESULT_20260823_164733.zip`
Class: PROVEN + FALSIFIED split.

- first helper arg2 0x40 bytes temporary in-place transpose/restore: PROVEN
- persistent output in `+0x40..+0x1FF`: FALSIFIED
- older parser-generated falsification of first 0x40: INVALID/SUPERSEDED

## `CSGOVR_CAUSAL_ORIGINX_ZERO_CONTROL_RESULT_20260823_043045.zip`
Class: INFRASTRUCTURE.

Retained:
- `client+0x0038D123`
- origin `CViewSetup+0xB8`
- next RenderView `+0x003A5280`
- candidates `+0x052632CC`, `+0x05263384`

Not camera ownership proof.

## `CSGOVR_CAUSAL_YAW_FINAL_POSE_CONTROL_RESULT_20260823_071600.zip`
Class: INFRASTRUCTURE.

Retained:
- `client+0x0038D15B`
- yaw `CViewSetup+0xC8`
- `+0x0038D1DE`
- `+0x05245AB8`

Not final-pose proof.

## `CSGOVR_FULL_EXPERIMENTAL_FAILURE_20260821_022832.zip`
Class: INVALID RUN / build-workflow failure.

- Crysis2VR source/root contamination
- Zig/build precheck failure
- proves nothing about CSGOVR camera/projection/culling/XR/viewmodel

## v0.6.5 XRFOV end-to-end attempt
Class: infrastructure failure / XR subfact PROVEN.

- XR view/FOV acquisition succeeded
- end-to-end application failed because x86 path wiring was broken (`g_xrFovPath` empty/uninitialized; stale control path vs PowerShell INI)
- did not falsify OpenXR, stereo or projection math

## CURRENT REPO G3 CANDIDATE — UNRUN
Files:
- `experiments/quest_projection_child/eye_patch_quest_projection.c`
- `experiments/quest_projection_child/QUEST_PROJECTION_MINIMAL.patch`
- `tools/verify_quest_projection_child.py`
- `.github/workflows/evidence-integrity.yml`

Class: UNRUN integration candidate.

Design:
- exact v0.5.5 parent
- preserve existing eye origin/pass schedule
- add known Quest asymmetric custom matrix only
- pass3 untouched
- no culling/head/weapon mutation

This is CURRENT FINALIST #1.

## BRANCH-LOCAL G3 DIAGNOSTIC — UNRUN
`CSGOVR_G3_QUEST_COHERENCE_BATCH_v1.0.1_SYNCED_ONE_CLICK.zip`
SHA256 `595DA174AC39BA334F725DC1D130089F7F7C98D2BC4878F87B12F113BC0EAB70`
Class: UNRUN diagnostic support, not baseline.

Primary-useful phase: Quest matrix only.
Secondary phases: FOV/aspect/150 controls for possible later G4 culling harvest.

Cross-branch decision: do not let secondary culling phases redefine G3 PASS/FAIL.

## Current engineering handoff
**CURRENT GATE: G3 Quest asymmetric projection integration.**

**NEXT:** run the repo byte-locked minimal child after integrity verification; obtain in-headset/same-frame verdict; freeze projection on PASS. Only then open transport/culling/origin work if a concrete visible blocker remains.
