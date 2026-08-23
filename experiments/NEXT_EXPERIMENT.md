# CSGOVR — NEXT EXPERIMENT

Snapshot: 2026-08-24 final cross-branch reconciliation.

## Decision
**ADAPT SCHEDULING: T0 GPU TRANSPORT FIRST, G3 PROJECTION QUEUED.**

The byte-locked Quest projection child remains valid and must not be discarded. T0 is inserted first only to isolate the still-unproven final XR transport from the projection mutation.

## Primary question
Can exact v0.5.5 pass1 LEFT and pass2 RIGHT be delivered as one coherent GPU-shared pair to a separate x64 Meta OpenXR host while projection, origin, camera and pass3 monitor remain BASE?

## PREFLIGHT — exactly 5 lines
**MISSING FACT:** whether exact v0.5.5 LEFT/RIGHT can reach Quest as one same-cycle GPU pair with no CPU readback or eye-sequence mixing.
**WHY NOT COLD:** pinned HLAE source proves replacement-target and GPU-wait primitives exist, but runtime is required to prove shared-resource compatibility, synchronization, pair ordering and XR submission.
**PASS:** one pairSeq contains pass1 LEFT + pass2 RIGHT from the same HLAE cycle, GPU completion precedes publish, both XR eye copies and one xrEndFrame succeed, headset is non-black/coherent, and v0.5.5 geometry/monitor behavior is unchanged.
**FAIL:** D3D9/D3D11 shared-resource failure, mixed L/R sequence, insufficient GPU synchronization, XR acquire/wait/copy/release failure, black/freeze, or GOLDEN regression.
**BASELINE MUST HOLD:** frozen client SHA256, `-insecure`, HLAE 2.191.1/b976, full CViewSetup `0x198`, pass1 LEFT/pass2 RIGHT/pass3 monitor, BASE projection/origin/camera, reversible restore.

## Mutation scope
1. Parent from exact v0.5.5.
2. Use pinned HLAE interop replacement eye targets.
3. Associate pass1/pass2 with one `pairSeq` per HLAE cycle.
4. Wait for GPU completion before publishing the pair.
5. x64 host uses OpenXR-required D3D11 adapter LUID `00000000:000118D1`.
6. Import eye GPU resource(s), acquire/wait/copy/release both XR swapchains, one `xrEndFrame` per coherent pair.
7. Do not apply Quest P00/new origin/HMD pose/culling/viewmodel/HUD in T0.

## Result contract
Collect:
- exact client/HLAE/source hashes
- D3D9 eye RT size/format/shared-resource status
- x64 resource import status
- pass id / eye / pairSeq timeline
- GPU wait/completion status
- OpenXR adapter/feature level
- per-eye acquire/wait/copy/release counters/errors
- xrEndFrame count
- `mixedPairCount` and `droppedPairCount`
- pass3 monitor count/regression
- headset verdict
- same-cycle L/R capture/reference if available
- restore/baseline recovery result

## Do not use as primary
`CSGOVR_XR_TRANSPORT_v0.7.0_BUILD_FIRST_ONECLICK.zip` is PREPARED/UNRUN and uses CPU `GetRenderTargetData -> SYSTEMMEM -> memcpy/shared memory`. Keep it only as fallback/reference if pinned HLAE GPU interop is concretely blocked.

## After T0 PASS
Freeze transport and execute queued G3/T1 using:
- `experiments/quest_projection_child/eye_patch_quest_projection.c`
- `experiments/quest_projection_child/QUEST_PROJECTION_MINIMAL.patch`
- `tools/verify_quest_projection_child.py`
- exact Quest P00 matrix + scalar FOV110, engine zNear/zFar retained

The branch-local `CSGOVR_G3_QUEST_COHERENCE_BATCH_v1.0.1_SYNCED_ONE_CLICK.zip` (SHA256 `595DA174AC39BA334F725DC1D130089F7F7C98D2BC4878F87B12F113BC0EAB70`) remains UNRUN/NOT BASELINE. Matrix-only is primary-useful; aspect/FOV phases are secondary diagnostics.

## FAIL handling
Fix only the concrete transport defect. Do not widen into projection/camera/culling archaeology.
