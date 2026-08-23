# CSGOVR — EXPERIMENTS / TOOLS INDEX

Purpose: let a fresh branch see what has already been tried, what is accepted, and what must not be repeated.

## CURRENT T0 transport work
### Exact pinned HLAE `afx_interop` capability
Class: **COLD exact-source PROOF / runtime OPEN**.

At AdvancedFX commit `b97636852b8eecae09285b5a386192bb285638eb`, the frozen HLAE lineage already contains:
- `AfxInterop::OnRenderView` / `OnRenderViewEnd`
- replacement render target path via `AfxD3d9PushRenderTargetEx`
- `PrepareDrawReply_Skip/Retry/Continue`
- `WaitForGpu -> AfxD3D_WaitForGPU()`

Purpose: current T0 finalist A / PRIMARY. Use BASE v0.5.5 geometry and prove same-cycle LEFT/RIGHT GPU transport to the external x64 Meta OpenXR host. Source capability is not an end-to-end runtime PASS.

### `CSGOVR_XR_TRANSPORT_v0.7.0_BUILD_FIRST_ONECLICK.zip`
Class: **SUPERSEDED PREPARED BUILD**.

Historical architecture:
`D3D9 GetRenderTargetData -> SYSTEMMEM -> CPU copy/shared memory -> x64 D3D11/OpenXR`.

It was never run. It is superseded inside the CPU fallback lineage by v0.7.1 below.

### `CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF`
Class: **PREPARED / STATIC-AUDITED / UNRUN / FINALIST B FALLBACK**.

Artifact SHA256:
`DFEA881888C83812A5B9EE3A7049F79D32935240F2C612DE4ABDF4F9A98834A3`

Manifest:
`runtime/children/v0.7.1_xr_transport/README.md`

Architecture:
`HLAE completed pass -> MSAA-safe CPU D3D9 readback -> atomic 2-slot shared pair -> x64 D3D11/OpenXR`.

Important hardening:
- Present is device latch only, not eye oracle
- Reset generation invalidates pooled fallback resources
- fixed-width `pack(4)` x86/x64 ABI
- same `CAfxStreams*` for LEFT/RIGHT
- LEFT HLAE count `0->1`, RIGHT `1->2`
- complete pair only becomes READY
- one-shot producer `DISTINCT PAIR PROOF`
- one-shot host `PAIR PROOF PASS`
- RGB-only hashes for `X8R8G8B8`
- no inherited projection-oracle camera freeze
- stale-control cleanup / PowerShell `$PID` fix
- one exact same-pair BMP pair harvested

Do not run this merely because it is prepared while pinned HLAE GPU interop remains the stronger current path. Activate if FINALIST A hits a concrete interop/shared-resource blocker or as a bounded compatibility control. It is not runtime-falsified.

## Accepted / high-value experiments
### `CSGOVR_HLAE_EYE_PATCH_RESULT_20260821_015737.zip`
Class: GOLDEN.

Retained:
- HLAE pass1 LEFT / pass2 RIGHT / pass3 monitor
- exact full `CViewSetup=0x198`
- local per-eye copy strategy
- nominal half-eye `1.2598425 Source units`
- original BMPs support depth-dependent stereo parallax

Repo source:
- `runtime/golden_v0.5.5/src/eye_patch.c`
- `runtime/golden_v0.5.5/src/injector.c`

### `CSGOVR_CORRECTNESS_RESULT_20260821_011601.zip`
Class: PROVEN causal retail renderer evidence.

Retained:
- CPU `CViewSetup.origin -> GPU VS c2/CAMERA_POS`
- WORLD_X_PAIR small translation changes pixels/draw membership
- projection-family shader constants `c4..c7` / `c8..c11`
- retained `c17`; bounded `c58..c60` model-candidate family

### `CSGOVR_CUSTOM_PROJECTION_MATRIX_RESULT_20260821_042218.zip`
Class: PROVEN projection semantics.

Retained:
- `+0x68` custom projection enable
- `+0x6C..+0xAB` 4x4 matrix
- `M02 +/-0.10 -> about -/+48 px X` at 960x540
- `M12 +/-0.10 -> about +/-27 px Y`
- M00/M11 axis-scale semantics
- deterministic measurement with pose freeze / bots kicked

Repo donor/transplant:
- `runtime/children/v0.6.1_projection_oracle/`
- `runtime/children/v0.6.1_projection_oracle/PROJECTION_TRANSPLANT.md`

Do not copy test-only freeze/INI/canary/FOV-widening behavior into production.

### `CSGOVR_OPENXR_FOV_DIAGNOSTIC_20260821_150332.zip`
Class: PROVEN XR INPUT.

Retained:
- Quest 3 two views, `viewStateFlags=0xF`
- LEFT FOV `-54,+40,+44,-55`
- RIGHT FOV `-40,+54,+44,-55`
- recommended 1504x1616
- runtime adapter LUID `00000000:000118D1`
- passive eye separation ~64.780 mm

Do not repeat broad XR discovery while this baseline remains valid.

### `CSGOVR_NATIVE_STEREO_SEAM_RESULT_20260821_041928`
Class: split evidence.

Retained:
- full 0x198 copies
- late projection/off-center can affect output
- one tested late DUAL_ORIGIN intervention did not authoritatively change its final capture

This does not cancel v0.5.5 parallax or WORLD_X_PAIR evidence.

### `CSGOVR_TTD_FRUSTUM_CACHE_REPLAY_RESULT_20260822_012630.zip`
Class: FALSIFIED + harvest.

FALSIFIED: `client+0x0525BA60` reused inside both eye marker regions.
Harvest: GeneratePerspectiveFrustum `+0x0086E9C0`, writers `+0x0086EB3C`, readers `+0x003A16A6`, `+0x003A01E4`.

### `CSGOVR_TTD_REPLAY_FRUSTUM_READER_OWNER_3A16A6_RECOVERED_RESULT_20260823_160154.zip`
Class: PROVEN evidence packet / semantics PENDING.

- exact reader hit `+0x3A16A6..+0x3A16DC`
- call stack captured
- bounded 24-step dataflow
- 3 store candidates

Do not build another broad frustum analyzer; resume here only if post-G3 culling opens.

### `CSGOVR_TTD_FRUSTUM_ARG2_PERSISTENCE_RESULT_20260823_164733.zip`
Class: PROVEN + FALSIFIED split; latest accepted forensic RESULT.

- first arg2 0x40 bytes transpose/restore: PROVEN
- persistent tail `+0x40..+0x1FF`: FALSIFIED

### `CSGOVR_TTD_SIMPLEWORLD_LIST_OWNERSHIP_RESULT_20260823_142036.zip`
Class: LINEAGE PROVEN / OWNERSHIP PENDING.

- CSkyboxView `+BF451C/+3AEA90`: main-world FALSIFIED
- CShadowDepthView `+BF44F4/+3AEAE0`: main-world FALSIFIED
- CSimpleWorldView `+BF4434/+3B0870`: main-world lineage PROVEN/FINALIST

### `CSGOVR_CAUSAL_ORIGINX_ZERO_CONTROL_RESULT_20260823_043045.zip`
Class: INFRASTRUCTURE.
Retains `+0x38D123`, origin `+0xB8`, next RenderView `+0x3A5280`, globals `+0x52632CC/+0x5263384`; not ownership proof.

### `CSGOVR_CAUSAL_YAW_FINAL_POSE_CONTROL_RESULT_20260823_071600.zip`
Class: INFRASTRUCTURE.
Retains yaw `+0x38D15B`, `CViewSetup+0xC8`, commit `+0x38D1DE`, `+0x5245AB8`; not final-pose proof.

## Queued G3/T1 projection work
Repo canonical candidate:
- `experiments/quest_projection_child/eye_patch_quest_projection.c`
- `experiments/quest_projection_child/QUEST_PROJECTION_MINIMAL.patch`
- verifier `tools/verify_quest_projection_child.py`
- CI `.github/workflows/evidence-integrity.yml`

Status: PREPARED/UNRUN runtime. Queue immediately after T0 PASS.

Branch diagnostic package:
`CSGOVR_G3_QUEST_COHERENCE_BATCH_v1.0.1_SYNCED_ONE_CLICK.zip`
SHA256 `595DA174AC39BA334F725DC1D130089F7F7C98D2BC4878F87B12F113BC0EAB70`

Status: UNRUN / NOT BASELINE.
- Quest matrix-only phase = primary-useful G3 measurement
- FOV110 + old aspect / projection-derived aspect / FOV150 = secondary clipping/culling diagnostics only
- aspect `0.9254939479` is not a production fix

## Failed / invalid / stopped approaches
- full-size `0x14C`
- raw Khronos column-major memcpy into Source VMatrix
- `mirv_cam attachCommands` as current-stream CView mutation when view already exists
- v0.6.5 broken XRFOV path wiring
- stale PID/gameplay countdown orchestration
- cross-project source-root contamination
- repeated projection sign/transpose roulette after causal closure
- `+0x0525BA60` both-eye cache theory
- persistent arg2 tail theory
- post-SetUpView static candidates `+38E5D7/+38EC08/+3A0381`
- treating origin/yaw byte integrity as ownership
- old broad origin-point batch without a post-G3 visible blocker
- v0.7.0 specifically: superseded by v0.7.1
- CPU-readback architecture as T0 primary unless GPU interop is concretely blocked

## Tooling / paths
- `tools/verify_quest_projection_child.py` — GOLDEN -> projection-child source integrity
- `.github/workflows/evidence-integrity.yml` — verifier on push/PR
- shared local root `C:\GOG Games\VR Research Tools`
- transport fallback manifest `runtime/children/v0.7.1_xr_transport/README.md`
- targeted tools already used: Ghidra, TTD/WinDbg, x32dbg/x64dbg, D3D9 hooks/capture, MinHook, OpenXR diagnostics

## Do not repeat rule
Before any new LIVE/TTD/build, read `research/RESULT_INDEX.md`, `research/BRANCH_SYNC_20260824.md`, `research/TRANSPORT_SYNC_20260824.md`, `docs/PROVEN.md`, `docs/FALSIFIED.md`, and this file. If another result already closed the question or yielded a concrete target, use it instead of building a new broad analyzer.
