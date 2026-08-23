# CSGOVR — NEXT

Snapshot: 2026-08-24 final cross-branch reconciliation.

## CURRENT GATE
**T0 / G2.5 — GPU XR TRANSPORT CERTIFICATION.**

G3 Quest projection remains queued immediately after T0 and is not falsified.

## NEXT ONE STEP — FINALIST A FIRST
Build/run a **transport-only** exact-v0.5.5 child:
- preserve pass1 LEFT / pass2 RIGHT / pass3 monitor
- preserve BASE projection, origin and camera
- same HLAE cycle -> one `pairSeq`
- GPU-complete before pair publication
- external x64 host uses D3D11 on OpenXR adapter LUID `00000000:000118D1`
- acquire/wait/copy/release both XR eye swapchains
- one `xrEndFrame` per coherent pair
- `mixedPairCount = 0`

## CRITICAL AfxInterop PROTOCOL GUARD
Pinned HLAE exact source proves shared-target and GPU-wait primitives exist, but protocol generations differ.

Older `DrawingThreadPrepareDraw` path:
- `PrepareDrawReply_*`
- receives replacement shared handles

Server major v6/v7:
- bypasses that older replacement-target exchange
- uses newer `HandleDrawingMessage` flow

Therefore FINALIST A must explicitly use/log either:
1. a verified-compatible AfxInterop protocol/host mode that actually exercises the shared replacement-target path; or
2. a minimal transplant of the proven shared-target + `D3DQUERYTYPE_EVENT` fence primitives into the v0.5.5 pass discriminator.

Do not assume modern v6/v7 equals the old handshake.

Provenance: `research/HLAE_INTEROP_FINDINGS.md`.

## PREFLIGHT — exactly 5 lines
**MISSING FACT:** whether exact v0.5.5 pass1/pass2 can reach Meta OpenXR as one coherent GPU pair through an actually verified compatible shared-resource path, without CPU readback.
**WHY NOT COLD:** exact HLAE source proves sharing/fence capability and protocol differences, but only runtime proves the selected path, resource import, synchronization, pair sequencing and XR submit.
**PASS:** one pairSeq contains same-cycle LEFT/RIGHT, GPU completion precedes publish, both XR copies/submits succeed, headset is non-black/coherent, actual protocol/path is logged, and v0.5.5 geometry plus monitor remain unchanged.
**FAIL:** protocol mismatch, shared-resource/import failure, L/R sequence mixing, GPU sync failure, XR acquire/wait/copy/release failure, black/freeze, or GOLDEN regression.
**BASELINE MUST HOLD:** frozen client hash, `-insecure`, HLAE 2.191.1/b976, full `0x198` eye copies, pass1 LEFT/pass2 RIGHT/pass3 monitor, BASE projection/origin/camera, reversible restore.

## Mandatory T0 fields
- exact game/HLAE/source hashes
- actual AfxInterop protocol mode or minimal-transplant id
- interop active state
- eye RT size/format/shared-handle state
- pairSeq timeline / pass->eye mapping
- GPU wait/completion status
- x64 D3D11 adapter LUID / feature level
- OpenSharedResource/import result
- per-eye acquire/wait/copy/release counts/errors
- `xrEndFrame` count
- mixedPairCount / droppedPairCount
- pass3 monitor regression
- headset verdict
- same-cycle L/R capture/reference if available
- restore result

## Explicit exclusions in T0
- no Quest P00 mutation
- no new eye origin
- no HMD pose mutation
- no culling mutation
- no viewmodel/weapon/HUD mutation
- no FOV/aspect diagnostic phases
- no CPU `GetRenderTargetData -> SYSTEMMEM` as PRIMARY while GPU path remains viable

## FINALIST B — PREPARED CPU FALLBACK
`CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF`
- SHA256 `DFEA881888C83812A5B9EE3A7049F79D32935240F2C612DE4ABDF4F9A98834A3`
- PREPARED / STATIC-AUDITED / NOT LIVE-PROVEN
- manifest `runtime/children/v0.7.1_xr_transport/README.md`
- supersedes v0.7.0 before LIVE

Activate B only if A reaches a **concrete** GPU interop/shared-resource/protocol blocker or as a bounded compatibility control. Do not repeat the research that built B.

Retained B hardening:
- same `CAfxStreams*` for pair
- LEFT count `0->1`, RIGHT `1->2`
- READY only after complete pair
- tokens `DISTINCT PAIR PROOF` / `PAIR PROOF PASS`
- fixed-width `pack(4)` ABI
- Reset generation invalidation
- RGB-only X8R8G8B8 hashing
- live v0.5.5 camera input
- two-slot pair ring

## Capture fallback
If same-cycle image evidence is noisy, reuse v0.6.1 deterministic freeze/capture. It achieved within-phase MAE around `0.0000..0.0026 / 255`.

Require same-eye same-state drift `<1%` before quantitative L/R claims. Keep `mat_queue_mode 0`; remove actor/bot motion.

## After T0 PASS
Freeze transport, then immediately run **G3/T1**:
- repo byte-locked Quest projection child
- exact P00 LEFT/RIGHT
- engine zNear/zFar retained
- Source-native scalar FOV `110°`
- same coherent pair machinery

Native SourceVR scale retained for later eye policy: `39.3700787 inches/meter`; measured Quest half-IPD ≈ `1.27520 Source units`.

## Conditional only after G3
- aspect `0.9254939479` / FOV150 are diagnostics only after visible post-P00 clipping
- visible culling -> `+0x3A16A6` / CSimpleWorldView lineage
- visible origin/parallax -> reconcile GOLDEN/WORLD_X_PAIR first; narrow candidate is shared-CView mutation before NORMAL `GetMatricesForView`, not blind copy-ctor hook
- no broad archaeology

## Prepared G3 assets
- `experiments/quest_projection_child/eye_patch_quest_projection.c`
- `experiments/quest_projection_child/QUEST_PROJECTION_MINIMAL.patch`
- `tools/verify_quest_projection_child.py`
- `.github/workflows/evidence-integrity.yml`
- `CSGOVR_G3_QUEST_COHERENCE_BATCH_v1.0.1_SYNCED_ONE_CLICK.zip`, SHA256 `595DA174AC39BA334F725DC1D130089F7F7C98D2BC4878F87B12F113BC0EAB70`, UNRUN/NOT BASELINE
