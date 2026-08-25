# M34 failure boundary and rollback decision — 2026-08-25

## Status

`M34 v0.2.x = INVALID / FROZEN`

Do not use M34 as a new baseline.

Last user-confirmed runtime GOLDEN remains:

`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256:

`5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Runtime evidence:

`CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`

User visual verdict: VR + full HMD 6DoF works; tearing remains a quality defect.

## `_RUN_20260825_221228` evidence

The v0.2.1 run progressed farther than the earlier host-boot failure:

- CS:GO launched.
- `de_dust2` loaded.
- transport DLL injection returned non-zero `LoadLibraryW`.
- hand probe DLL injection returned non-zero `LoadLibraryW`.
- NETCON transcript ends at the first `mirv_streams record start; echo CSGOVR_M34_START`.

Therefore the failing boundary is the first HLAE stereo render after both injections, not Zig build, target launch, map load, or LoadLibrary injection.

## Convergence

M33 had already reached real game pairs with the slot8/controller-capable transport lineage, so slot depth alone is not the leading suspect.

M34 enabled two coupled viewmodel changes before the same first render:

1. `CSGOVRLEFT/RIGHT drawViewModel 0 -> 1`
2. passive/global `C_BaseAnimating::BuildTransformations` vtable index 184 hook in `hand_probe.dll`

The current run cannot distinguish these two because both became active before the first recording render.

## Decision

Do not patch M34 forward.

Return to exact M3.2B v1.1.1 GOLDEN and isolate the viewmodel path serially:

- A: exact GOLDEN baseline.
- B: GOLDEN + `drawViewModel=1` only, no hand DLL.
- C: only if B survives, add PASSIVE `BuildTransformations` observation, no controller or transform mutation.
- D: only after C PASS, add controller IPC.
- E: only after D PASS, mutate the viewmodel transform.

A future one-click runner may execute A/B/C as separate auto-closed sessions and emit one combined result. That preserves causal isolation while collecting several facts per user action.
