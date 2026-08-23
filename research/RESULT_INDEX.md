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
- off-center fields: top `+0x138`, bottom `+0x13C`, left `+0x140`, right `+0x144`, `m_bOffCenter = flags+0x148 bit0`
- controlled +/- canary phases exercised the fields for LEFT/RIGHT eye passes

## `CSGOVR_CUSTOM_PROJECTION_MATRIX_RESULT_20260821_042218.zip`
**Class:** PROVEN custom projection path

- custom projection enable at `+0x68`
- `VMatrix +0x6C..+0xAB`
- structurally anchored by FOV at `+0xB0`
- M00/M02/M11/M12 semantics causally measured
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
- tested late origin eye shift did not change final captured raster

Interpretation: only that tested late origin intervention point failed to authoritatively move final raster; eye translation itself is not falsified.

## `CSGOVR_TTD_FRUSTUM_CACHE_REPLAY_RESULT_20260822_012630.zip`
**Class:** FALSIFIED + HARVEST

FALSIFIED:
`client.dll+0x0525BA60` reused inside both LEFT and RIGHT eye marker regions.

Harvest:
- `GeneratePerspectiveFrustum client.dll+0x0086E9C0`
- lane writers starting `+0x0086EB3C`
- downstream pre-eye readers `+0x003A16A6`, `+0x003A01E4`

## `CSGOVR_TTD_REPLAY_FRUSTUM_READER_OWNER_3A16A6_RECOVERED_RESULT_20260823_160154.zip`
**Class:** PROVEN evidence packet / destination semantics PENDING

- target `client.dll+0x003A16A6..+0x003A16DC` hit
- call stack captured
- bounded 24-step dataflow recovered
- auto store candidates: 3

Exact semantic destination object/field is still not classified.

## `CSGOVR_TTD_FRUSTUM_ARG2_PERSISTENCE_RESULT_20260823_164733.zip`
**Class:** PROVEN + FALSIFIED split

- PROVEN: first `0x40` bytes of helper arg2 undergo temporary in-place transpose and restore
- FALSIFIED: persistent output hypothesis in arg2 `+0x40..+0x1FF`

Earlier parser-generated FALSIFIED verdict for the first 0x40 transpose is INVALID/SUPERSEDED.

## `CSGOVR_CAUSAL_ORIGINX_ZERO_CONTROL_RESULT_20260823_043045.zip`
**Class:** INFRASTRUCTURE

Retained seams:
- `client.dll+0x0038D123`
- `CViewSetup.origin.x +0xB8`
- next `RenderView +0x003A5280`
- persistent/global CView candidate `+0x052632CC`
- origin global candidate `+0x05263384`

Byte integrity passed; no camera ownership/control proof.

## `CSGOVR_CAUSAL_YAW_FINAL_POSE_CONTROL_RESULT_20260823_071600.zip`
**Class:** INFRASTRUCTURE

- seam `client.dll+0x0038D15B`
- yaw `CViewSetup+0xC8`
- global commit `client.dll+0x0038D1DE`
- `g_vecRenderAngles.y` candidate `+0x05245AB8`
- byte integrity only; not final-pose proof

## `CSGOVR_FULL_EXPERIMENTAL_FAILURE_20260821_022832.zip`
**Class:** INVALID RUN / BUILD-WORKFLOW FAILURE

- contaminated build pulled Crysis2VR path/source
- Zig precheck failure before useful renderer evidence

## `CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF`
**Class:** PREPARED / STATIC-AUDITED / NOT YET LIVE-PROVEN

Artifact SHA256:
`DFEA881888C83812A5B9EE3A7049F79D32935240F2C612DE4ABDF4F9A98834A3`

Supersedes prepared `v0.7.0_XR_TRANSPORT` before LIVE.

Purpose:
- preserve v0.5.5 HLAE scheduling;
- capture LEFT/RIGHT after each real HLAE eye render;
- prove both eyes are one HLAE outer-pass transaction;
- publish an atomic shared-memory pair;
- submit through x64 D3D11/OpenXR;
- harvest one exact same-pair image pair and stage telemetry.

Structural same-pair requirement:
- same `CAfxStreams*`
- LEFT `m_DoRenderViewCount 0 -> 1`
- RIGHT `1 -> 2`
- same mode/pair identity
- only complete pair becomes READY

Runtime proof tokens:
- producer `DISTINCT PAIR PROOF`
- host `PAIR PROOF PASS`

T0 runs the OpenXR host/session/diagnostic path before game launch.

**No scientific verdict yet. A LIVE RESULT is still required.**

## Current engineering handoff — corrected 2026-08-24
**CURRENT GATE: XR TRANSPORT.**

The existing Quest projection child remains queued immediately after transport PASS. The prior GitHub state that called projection current did not include an exact transport/headset PASS in this result ledger, so gate order was corrected without deleting projection work.

If transport PASS: freeze it, then run the already-prepared projection integration.  
If transport FAIL: adapt only the exact failed stage.

If a concrete culling blocker survives projection, resume from `client.dll+0x003A16A6` + CSimpleWorldView lineage. Do not reopen broad archaeology or the falsified shared-cache hypothesis.
