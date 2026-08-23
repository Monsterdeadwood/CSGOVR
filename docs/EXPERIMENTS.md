# CSGOVR — EXPERIMENTS / TOOLS INDEX

Purpose: let a fresh branch see what has already been tried, what is accepted, and what must not be repeated.

## Accepted / high-value experiments

### `CSGOVR_HLAE_EYE_PATCH_RESULT_20260821_015737.zip`
Class: GOLDEN.

Proved/retained:
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
- CPU `CViewSetup.origin` -> GPU VS `c2/CAMERA_POS`
- WORLD_X_PAIR small translation changes pixels/draw membership
- projection-family shader constants around c4/c8

### `CSGOVR_CUSTOM_PROJECTION_MATRIX_RESULT_20260821_042218.zip`
Class: PROVEN projection semantics.

Retained:
- `+0x68` custom projection enable
- `+0x6C..+0xAB` 4x4 matrix
- `M02 +/-0.10 -> about -/+48 px X` at 960x540
- `M12 +/-0.10 -> about +/-27 px Y`
- M00/M11 axis scale semantics
- deterministic measurement with pose freeze / bots kicked

Repo donor/transplant:
- `runtime/children/v0.6.1_projection_oracle/`
- `runtime/children/v0.6.1_projection_oracle/PROJECTION_TRANSPLANT.md`

Do NOT copy test-only freeze/INI/canary/FOV-widening behavior into the final architecture.

### `CSGOVR_OPENXR_FOV_DIAGNOSTIC_20260821_150332.zip`
Class: PROVEN XR INPUT.

Retained:
- Quest 3 two views
- `viewStateFlags=0xF`
- LEFT FOV `-54,+40,+44,-55`
- RIGHT FOV `-40,+54,+44,-55`
- eye size 1504x1616
- eye separation ~64.780 mm

Do not repeat broad XR discovery while this baseline remains valid.

### `CSGOVR_NATIVE_STEREO_SEAM_RESULT_20260821_041928`
Class: split evidence.

Retained:
- full 0x198 copies
- late projection/off-center can affect output
- one tested late DUAL_ORIGIN intervention did not authoritatively change its final capture

Important: this does not cancel independent v0.5.5 parallax and WORLD_X_PAIR evidence.

### `CSGOVR_TTD_FRUSTUM_CACHE_REPLAY_RESULT_20260822_012630.zip`
Class: FALSIFIED + harvest.

FALSIFIED:
- `client+0x0525BA60` reused inside both eye marker regions.

Harvest:
- `GeneratePerspectiveFrustum client+0x0086E9C0`
- writers around `+0x0086EB3C`
- readers `+0x003A16A6`, `+0x003A01E4`

### `CSGOVR_TTD_REPLAY_FRUSTUM_READER_OWNER_3A16A6_RECOVERED_RESULT_20260823_160154.zip`
Class: PROVEN evidence packet / semantics PENDING.

Retained:
- target hit `client+0x003A16A6..+0x003A16DC`
- call stack captured
- bounded 24-step dataflow
- 3 store candidates

Do not build another broad frustum analyzer; resume here only if G4 opens.

### `CSGOVR_TTD_FRUSTUM_ARG2_PERSISTENCE_RESULT_20260823_164733.zip`
Class: PROVEN + FALSIFIED split.

- first arg2 0x40 bytes transpose/restore: PROVEN
- persistent tail `+0x40..+0x1FF`: FALSIFIED

### `CSGOVR_CAUSAL_ORIGINX_ZERO_CONTROL_RESULT_20260823_043045.zip`
Class: INFRASTRUCTURE.

Retained anchors only:
- `client+0x0038D123`
- `CViewSetup.origin +0xB8`
- next RenderView `+0x003A5280`
- global candidates `+0x052632CC`, `+0x05263384`

Not camera ownership proof.

### `CSGOVR_CAUSAL_YAW_FINAL_POSE_CONTROL_RESULT_20260823_071600.zip`
Class: INFRASTRUCTURE.

Retained:
- `client+0x0038D15B`
- yaw `CViewSetup+0xC8`
- global commit `+0x0038D1DE`
- `+0x05245AB8` candidate

Not final-pose proof.

## Current prepared G3 experiment
Repo canonical candidate:
- `experiments/quest_projection_child/eye_patch_quest_projection.c`
- `experiments/quest_projection_child/QUEST_PROJECTION_MINIMAL.patch`
- `experiments/NEXT_EXPERIMENT.md`
- verifier `tools/verify_quest_projection_child.py`
- CI `.github/workflows/evidence-integrity.yml`

This is the preferred current finalist because it is byte-locked to exact v0.5.5 and adds only the Quest asymmetric custom matrix.

## Branch-local diagnostic package
`CSGOVR_G3_QUEST_COHERENCE_BATCH_v1.0.1_SYNCED_ONE_CLICK.zip`
SHA256 `595DA174AC39BA334F725DC1D130089F7F7C98D2BC4878F87B12F113BC0EAB70`

Contained:
- `00_RUN_EVERYTHING.bat`
- `ONE_CLICK.ps1`
- `src/eye_patch.c`
- `src/injector.c`
- `analysis/analyze_result.py`
- `analysis/static_verify.py`
- evidence/prediction/preflight/path-contract files

Status: UNRUN, not baseline, not a replacement for the repo minimal child.

Its phases were designed as:
1. BASE
2. Quest matrix only — primary-useful G3 measurement
3. Quest matrix + FOV110 + old aspect — secondary
4. Quest matrix + FOV110 + projection-derived aspect — secondary
5. old FOV150 control — secondary

Cross-branch sync decision: only phase 2 belongs to G3 PRIMARY. The later scalar FOV/aspect phases are G4/culling harvest and must not be mixed into G3 verdict.

## Failed / invalid approaches to remember
- full-size `0x14C`
- `mirv_cam attachCommands` as current-stream CView mutation; too late for already-created stream view
- v0.6.5 broken XRFOV path wiring
- stale PID/gameplay countdown orchestration runs
- cross-project source-root contamination
- repeated projection sign/transpose roulette after v0.6.1
- shared `+0x0525BA60` both-eye cache theory
- persistent arg2 tail theory
- treating origin/yaw byte integrity as ownership
- old broad origin-point batch without a post-G3 visible blocker

## Useful source/donor architecture already consumed
- AdvancedFX/HLAE exact seam
- Valve Source SDK VR architecture
- L4D2VR / Portal2VR local per-eye CViewSetup copies
- d3d9-vr projection path
- OpenXR SDK/reference timing and FOV conventions

Donor addresses/vtables/ABI are never retail proof.

## Tooling
- `tools/verify_quest_projection_child.py` — byte/source integrity for GOLDEN -> Quest child
- `.github/workflows/evidence-integrity.yml` — runs verifier on push/PR
- shared local root: `C:\GOG Games\VR Research Tools`
- previously used targeted tools: Ghidra, TTD/WinDbg, x32dbg/x64dbg, D3D9 hooks/capture, MinHook, OpenXR diagnostics

## Do not repeat rule
Before any new LIVE/TTD/build, search `research/RESULT_INDEX.md`, `docs/PROVEN.md`, `docs/FALSIFIED.md`, and this file. If the question is already closed or another experiment has a concrete target, use that target instead of building a new broad analyzer.
