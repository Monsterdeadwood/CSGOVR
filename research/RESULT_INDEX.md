# CSGOVR — RESULT INDEX

Authority: canonical KB + 2026-08-24 cross-branch reconciliation.
Evidence: GOLDEN / PROVEN / SUPPORTED / PENDING / INFRASTRUCTURE / FALSIFIED / INVALID / SUPERSEDED / UNRUN.

## GOLDEN — `CSGOVR_HLAE_EYE_PATCH_RESULT_20260821_015737.zip`
Build `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`.
- pass1 LEFT, pass2 RIGHT, pass3 monitor
- full local CViewSetup `0x198`
- HLAE DoRenderView `+0x22340`, copy ctor `+0x14BA0`
- nominal half-eye `1.2598425 Source units`
- original BMPs show stable depth-dependent horizontal L/R parallax
- source `runtime/golden_v0.5.5/src/`

## `CSGOVR_CORRECTNESS_RESULT_20260821_011601.zip`
Class: PROVEN causal retail.
- CViewSetup origin -> VS `c2/CAMERA_POS`
- WORLD_X_PAIR translation changes pixels/draw membership
- projection families c4..c7 / c8..c11

## `CSGOVR_PROJECTION_TRUTH_RESULT_20260821_035411.zip`
Class: PROVEN off-center semantics.
- `+0x138/+0x13C/+0x140/+0x144`, flag `+0x148 bit0`

## `CSGOVR_CUSTOM_PROJECTION_MATRIX_RESULT_20260821_042218.zip`
Class: PROVEN custom projection.
- enable `+0x68`, matrix `+0x6C..+0xAB`
- M00/M02/M11/M12 causal semantics
- M02 +/-0.10 ≈ -/+48 px X at 960x540
- M12 +/-0.10 ≈ +/-27 px Y
- M00/M11 0.80 axis-scale controls
- donor `runtime/children/v0.6.1_projection_oracle/`

### v0.6.1 deterministic capture subresult
Class: PROVEN method.
- within-phase MAE about `0.0000..0.0026 / 255`
- old sequential comparator (~62% same-eye temporal vs ~54% L/R) INVALID for quantitative stereo/culling

## `CSGOVR_OPENXR_FOV_DIAGNOSTIC_20260821_150332.zip`
Class: PROVEN XR input.
- Oculus OpenXR 1.205.0 + Quest 3
- 2 views, `viewStateFlags=0xF`
- LEFT `-54,+40,+44,-55`; RIGHT `-40,+54,+44,-55`
- recommended `1504x1616`, max `3008x3232`
- adapter LUID `00000000:000118D1`, min FL `0xB000`, created `0xB100`
- passive eye separation ~64.780414 mm

## Valve SourceVR native scale cross-check — 2026-08-24
Class: PROVEN source semantics.
- `OpenVRToSourceCoordinateSystem()` uses `39.3700787 inches/meter`
- native HMD/eye path `1 Source unit = 1 inch`
- 64.780414 mm ≈ 2.55041 units total / 1.27520 half-IPD
- AdvancedFX Unity 0.01905 m/unit FALSIFIED as native SourceVR eye scale

## `CSGOVR_NATIVE_STEREO_SEAM_RESULT_20260821_041928`
Class: PROVEN/SUPPORTED split.
- full 0x198 copies
- late projection/off-center survives
- one late DUAL_ORIGIN intervention did not authoritatively alter final capture
Scope: negative only for that intervention, not general eye translation.

## `CSGOVR_TTD_FRUSTUM_CACHE_REPLAY_RESULT_20260822_012630.zip`
Class: FALSIFIED + harvest.
- FALSIFIED `client+0x0525BA60` both-eye reuse
- harvest `+0x0086E9C0`, writers `+0x0086EB3C`, readers `+0x003A16A6/+0x003A01E4`

## `CSGOVR_TTD_REPLAY_FRUSTUM_READER_OWNER_3A16A6_RECOVERED_RESULT_20260823_160154.zip`
Class: PROVEN evidence packet / semantics PENDING.
- `+0x003A16A6..+0x003A16DC` hit
- call stack, 24-step dataflow, 3 store candidates

## `CSGOVR_TTD_FRUSTUM_ARG2_PERSISTENCE_RESULT_20260823_164733.zip`
Class: PROVEN + FALSIFIED.
- first 0x40 temporary transpose/restore PROVEN
- persistent `+0x40..+0x1FF` FALSIFIED

## Origin/yaw control results
- `CSGOVR_CAUSAL_ORIGINX_ZERO_CONTROL_RESULT_20260823_043045.zip` — INFRASTRUCTURE; anchors `+0x0038D123`, `+0xB8`, `+0x003A5280`, `+0x052632CC/+0x05263384`
- `CSGOVR_CAUSAL_YAW_FINAL_POSE_CONTROL_RESULT_20260823_071600.zip` — INFRASTRUCTURE; anchors `+0x0038D15B`, `+0xC8`, `+0x0038D1DE`, `+0x05245AB8`
Not ownership proof.

## `CSGOVR_FULL_EXPERIMENTAL_FAILURE_20260821_022832.zip`
Class: INVALID build/workflow failure. Cross-project contamination; no renderer/XR conclusion.

## v0.6.5 XRFOV attempt
Class: infrastructure failure / XR subfact PROVEN.
XR acquisition succeeded; x86 path wiring failed. Did not falsify XR/stereo/projection math.

## Exact HLAE / AdvancedFX interop source review — 2026-08-24
Class: PROVEN source/build capability; runtime transport PENDING.
Source `advancedfx/advancedfx@b97636852b8eecae09285b5a386192bb285638eb`.
- Win32 includes AfxInterop
- older shared-handle D3D9 replacement RT path exists
- `D3DQUERYTYPE_EVENT` GPU completion wait exists
- NORMAL GetMatricesForView precedes DoRenderView; shared view persists

Protocol guard:
- v6/v7 bypass older `PrepareDrawReply` replacement-target exchange and use `HandleDrawingMessage`
- modern companion old-handshake use NOT PROVEN
- T0 requires verified-compatible mode or minimal shared-target/fence transplant

See `research/HLAE_INTEROP_FINDINGS.md`.

## `CSGOVR_XR_TRANSPORT_v0.7.0_BUILD_FIRST_ONECLICK.zip`
Class: UNRUN / SUPERSEDED inside CPU prepared lineage.
CPU readback/shared-memory architecture; not runtime-falsified.

## `CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF`
Class: PREPARED / STATIC-AUDITED / UNRUN T0 FALLBACK.
SHA256 `DFEA881888C83812A5B9EE3A7049F79D32935240F2C612DE4ABDF4F9A98834A3`.
Manifest `runtime/children/v0.7.1_xr_transport/README.md`.

Prepared hardening:
- same CAfxStreams instance pair proof
- LEFT count 0->1, RIGHT 1->2
- complete pair only READY
- `DISTINCT PAIR PROOF` / `PAIR PROOF PASS`
- fixed-width pack(4) ABI
- Reset generation invalidation
- RGB-only X8R8G8B8 hashing
- live v0.5.5 camera input
- two-slot shared pair ring

End-to-end runtime still UNPROVEN. Finalist B only if GPU finalist A concretely blocks or as bounded compatibility control.

## CURRENT T0 / G2.5 FINALIST A — UNRUN
Goal:
- exact v0.5.5 BASE geometry
- same-cycle pass1 LEFT + pass2 RIGHT -> one `pairSeq`
- verified-compatible HLAE shared-resource path or minimal transplant
- GPU complete before publish
- x64 D3D11 on LUID `00000000:000118D1`
- acquire/wait/copy/release both XR swapchains
- one xrEndFrame per coherent pair
- mixedPairCount=0
- pass3 monitor unchanged

## QUEUED G3/T1 — UNRUN
- `experiments/quest_projection_child/eye_patch_quest_projection.c`
- `QUEST_PROJECTION_MINIMAL.patch`
- `tools/verify_quest_projection_child.py`
- exact Quest P00 + Source-native scalar FOV110 after T0 PASS

Diagnostic `CSGOVR_G3_QUEST_COHERENCE_BATCH_v1.0.1_SYNCED_ONE_CLICK.zip`, SHA256 `595DA174AC39BA334F725DC1D130089F7F7C98D2BC4878F87B12F113BC0EAB70`, UNRUN/NOT BASELINE.

## CURRENT GATE
**T0 / G2.5 — GPU XR TRANSPORT CERTIFICATION.**

## NEXT
Run Finalist A with BASE geometry using an explicitly verified compatible AfxInterop protocol path or minimal shared-target/fence transplant. Prove same-cycle pairSeq, GPU completion, x64 import/copy into both Meta OpenXR swapchains, one xrEndFrame, mixedPairCount=0 and pass3 intact. If A hits a concrete interop blocker, activate prepared v0.7.1 Finalist B without repeating its research. T0 PASS -> freeze transport -> G3/T1 P00 + FOV110.
