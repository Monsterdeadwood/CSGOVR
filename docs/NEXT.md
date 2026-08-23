# CSGOVR — NEXT

Snapshot: 2026-08-24 final cross-branch reconciliation.

## CURRENT GATE
**T0 / G2.5 — GPU XR TRANSPORT CERTIFICATION.**

The previous remote G3 projection gate remains queued and valid. It was not falsified. T0 is inserted first because no accepted result yet proves final coherent two-eye CSGOVR -> Meta OpenXR submission and the exact pinned HLAE already contains a cleaner GPU interop route that can be tested with BASE geometry.

## NEXT ONE STEP
Build/run a **transport-only** child from exact v0.5.5:
- preserve pass1 LEFT / pass2 RIGHT / pass3 monitor
- preserve BASE projection, origin and camera
- use pinned HLAE `afx_interop` replacement eye targets
- one HLAE cycle -> one `pairSeq`
- GPU-complete before pair publication
- external x64 host uses D3D11 on OpenXR adapter LUID `00000000:000118D1`
- acquire/wait/copy/release both XR eye swapchains
- one `xrEndFrame` per coherent pair
- mixedPairCount must remain 0

## PREFLIGHT — exactly 5 lines
**MISSING FACT:** whether exact v0.5.5 pass1/pass2 can reach Meta OpenXR as one coherent GPU-shared pair without CPU readback.
**WHY NOT COLD:** exact HLAE source proves replacement-target and GPU-wait primitives exist, but only runtime proves resource sharing, synchronization, pair sequencing and XR submit.
**PASS:** one pairSeq contains same-cycle LEFT/RIGHT, GPU completion precedes publish, both XR copies/submits succeed, headset is non-black/coherent, and v0.5.5 geometry plus monitor remain unchanged.
**FAIL:** shared-resource/import failure, L/R sequence mixing, GPU synchronization failure, XR acquire/wait/copy/release failure, black/freeze, or GOLDEN regression.
**BASELINE MUST HOLD:** frozen client hash, `-insecure`, HLAE 2.191.1/b976, full `0x198` eye copies, pass1 LEFT/pass2 RIGHT/pass3 monitor, BASE projection/origin/camera, reversible restore.

## Mandatory T0 result fields
- exact game/HLAE/source hashes
- interop active state
- eye RT size/format/shared-handle state
- pairSeq timeline and pass->eye mapping
- GPU wait/completion status
- x64 D3D11 adapter LUID / feature level
- OpenSharedResource/import result
- per-eye acquire/wait/copy/release counts/errors
- xrEndFrame count
- mixedPairCount / droppedPairCount
- pass3 monitor regression status
- headset verdict
- same-cycle L/R capture/reference if available
- restore/baseline recovery result

## Explicit exclusions in T0
- no Quest P00 mutation
- no new eye origin
- no HMD rotation/translation
- no culling mutation
- no viewmodel/weapon/HUD mutation
- no FOV/aspect diagnostic phases
- do not use CPU `GetRenderTargetData -> SYSTEMMEM -> memcpy` as the default transport architecture

## After T0 PASS
Freeze transport, then immediately run **G3/T1**:
- repository byte-locked minimal Quest projection child
- exact P00 LEFT/RIGHT matrix
- engine zNear/zFar retained
- Source-native scalar FOV `110°`
- same coherent pair machinery

## Conditional only after G3
If exact P00 is applied and peripheral clipping remains visible:
- aspect `0.9254939479` and/or FOV150 may be used as diagnostic controls
- if a real culling blocker remains, continue from `+0x3A16A6` / CSimpleWorldView lineage
- do not reopen broad archaeology

## Prepared G3 assets
- `experiments/quest_projection_child/eye_patch_quest_projection.c`
- `experiments/quest_projection_child/QUEST_PROJECTION_MINIMAL.patch`
- `tools/verify_quest_projection_child.py`
- `.github/workflows/evidence-integrity.yml`
- branch-local `CSGOVR_G3_QUEST_COHERENCE_BATCH_v1.0.1_SYNCED_ONE_CLICK.zip` SHA256 `595DA174AC39BA334F725DC1D130089F7F7C98D2BC4878F87B12F113BC0EAB70`, UNRUN/NOT BASELINE
