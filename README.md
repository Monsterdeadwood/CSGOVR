# CSGOVR

Native VR research for **frozen legacy CS:GO x86** using HLAE/OpenXR and exact retail evidence.

## Current status
**ACTIVE — T0 / G2.5 GPU XR TRANSPORT CERTIFICATION.**

Protected GOLDEN:
`CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`

The previous G3 Quest projection gate remains valid and is **queued immediately after T0**. It is not falsified or discarded.

## CURRENT GATE
Prove that exact v0.5.5 pass1 LEFT + pass2 RIGHT can be transported as **one coherent GPU pair** through the pinned HLAE GPU interop path to a separate x64 Meta OpenXR host, while BASE projection/origin/camera and pass3 monitor remain unchanged.

Why T0 was inserted before G3:
- no accepted CSGOVR result yet proves final two-eye OpenXR transport/submission;
- exact pinned AdvancedFX `b97636852b8eecae09285b5a386192bb285638eb` already contains GPU replacement-target interop and GPU-wait primitives;
- the prepared CPU-readback transport package is therefore not the preferred first architecture;
- transport and projection should be serialized into separate mutations.

## NEXT
1. Keep exact v0.5.5 eye generation and BASE geometry.
2. Use pinned HLAE `afx_interop` replacement eye targets.
3. Bind pass1 LEFT + pass2 RIGHT from one HLAE cycle to one `pairSeq`.
4. Wait for GPU completion before publishing the pair.
5. In external x64 host, use D3D11 on OpenXR-required adapter LUID `00000000:000118D1`, acquire/wait/copy/release both XR swapchains, and issue one `xrEndFrame` for the coherent pair.
6. On T0 PASS, freeze transport and run the already-prepared G3/T1 Quest P00 projection child with Source-native scalar FOV `110°`.

## LATEST RESULT / branch status
- Latest accepted projection result: `CSGOVR_CUSTOM_PROJECTION_MATRIX_RESULT_20260821_042218.zip` — custom matrix semantics PROVEN.
- XR input: `CSGOVR_OPENXR_FOV_DIAGNOSTIC_20260821_150332.zip` — Quest FOV/two valid views PROVEN.
- Latest forensic result overall: `CSGOVR_TTD_FRUSTUM_ARG2_PERSISTENCE_RESULT_20260823_164733.zip` — first 0x40-byte transpose/restore PROVEN; persistent output beyond 0x40 FALSIFIED.
- No accepted end-to-end CSGOVR GPU/XR transport RESULT yet.
- `CSGOVR_G3_QUEST_COHERENCE_BATCH_v1.0.1_SYNCED_ONE_CLICK.zip`, SHA256 `595DA174AC39BA334F725DC1D130089F7F7C98D2BC4878F87B12F113BC0EAB70`, is PREPARED/UNRUN and not a baseline.
- `CSGOVR_XR_TRANSPORT_v0.7.0_BUILD_FIRST_ONECLICK.zip` is PREPARED/UNRUN; its CPU-readback architecture is COLD-SUPERSEDED AS PRIMARY by pinned HLAE GPU interop, not runtime-falsified.

## Core proven facts
- frozen legacy CS:GO x86 AppID `4465480`, BuildID `22100934`
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- `-insecure` mandatory
- HLAE 2.191.1; exact AfxHookSource lineage pinned to AdvancedFX `b976368...`
- pass1 LEFT, pass2 RIGHT, pass3 monitor
- full `CViewSetup=0x198`
- custom projection `+0x68`, matrix `+0x6C..+0xAB`
- Quest asymmetric P00 coefficients and 1504x1616 recommended eye size
- CPU `CViewSetup.origin -> VS c2/CAMERA_POS`
- v0.5.5 BMPs contain real depth-dependent L/R parallax
- concrete pre-eye reader `client+0x003A16A6`
- CSimpleWorldView main-world lineage

## Important conflict kept explicit
One tested **late** DUAL_ORIGIN intervention did not authoritatively change its final capture. Independently, the actual v0.5.5 GOLDEN shows depth-dependent stereo parallax and WORLD_X_PAIR proves RenderView-origin changes can reach GPU/raster. Therefore only that exact late intervention point is negative; eye translation in general is not falsified.

## Start here
1. `research/BRANCH_SYNC_20260824.md` — remote-race provenance and final reconciliation
2. `docs/STATE.md` — full working state
3. `docs/NEXT.md` — one-step continuation
4. `docs/SYNC.md` — cross-branch snapshot
5. `docs/GATES.md` — gate ladder
6. `docs/PROVEN.md` / `docs/FALSIFIED.md`
7. `docs/EXPERIMENTS.md`
8. `research/RESULT_INDEX.md`
9. `runtime/golden_v0.5.5/` — protected source baseline
10. `experiments/quest_projection_child/` — queued G3/T1 projection child

**PARALLELIZE EVIDENCE — SERIALIZE MUTATION.**
