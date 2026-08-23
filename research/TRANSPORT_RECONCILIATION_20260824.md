# CSGOVR — TRANSPORT RECONCILIATION 2026-08-24

Purpose: preserve cross-branch scheduling, transport evidence and the exact AfxInterop protocol guard so a fresh branch can continue without old chats.

## Remote-first provenance
`main` advanced repeatedly while state sync was in progress. Newer remote state was imported before this reconciliation. The concurrent branch preserved G3 projection work and inserted T0 / G2.5 transport certification before it because final coherent two-eye OpenXR submission remained unproven.

## Evidence preserved from prior branch
### GOLDEN / stock causal path
- original v0.5.5 BMPs show stable depth-dependent horizontal L/R parallax with negligible vertical disparity
- WORLD_X_PAIR proves small RenderView-origin change can alter GPU `c2`, pixels and draw membership
- one late DUAL_ORIGIN failure remains negative only for that exact intervention point

### Prepared G3 projection assets
- `experiments/quest_projection_child/eye_patch_quest_projection.c`
- `experiments/quest_projection_child/QUEST_PROJECTION_MINIMAL.patch`
- `tools/verify_quest_projection_child.py`
- `.github/workflows/evidence-integrity.yml`
- `CSGOVR_G3_QUEST_COHERENCE_BATCH_v1.0.1_SYNCED_ONE_CLICK.zip`, SHA256 `595DA174AC39BA334F725DC1D130089F7F7C98D2BC4878F87B12F113BC0EAB70`, UNRUN/NOT BASELINE

G3/T1 remains queued after T0 PASS; it is not falsified.

## Pinned HLAE GPU interop capability — exact source proof
At AdvancedFX commit `b97636852b8eecae09285b5a386192bb285638eb`, frozen HLAE source contains:
- `AfxInterop::OnRenderView` / `OnRenderViewEnd`
- replacement RT support via `AfxD3d9PushRenderTargetEx`
- older `PrepareDrawReply_Skip/Retry/Continue` drawing exchange
- GPU wait via `D3DQUERYTYPE_EVENT`, `Issue(D3DISSUE_END)`, `GetData(...D3DGETDATA_FLUSH)`
- Win32 build enables `AFX_INTEROP` and includes `AfxInterop.cpp/.h`

Class: COLD exact-source capability proof. End-to-end CSGOVR transport remains runtime OPEN.

## Critical protocol-generation reconciliation
Exact source exposes more than one AfxInterop protocol generation.

Older path:
- `DrawingThreadPrepareDraw` performs `PrepareDrawReply_*` exchange
- receives shared color/depth handles
- opens matching D3D9 replacement target and pushes it

Server major v6/v7 path:
- `DrawingThreadPrepareDraw` bypasses that older shared-target exchange
- newer flow uses `HandleDrawingMessage`

Therefore do **not** merge these into one unqualified claim.

### Evidence path A
Pinned HLAE definitely contains the required shared-target and GPU-completion primitives.

### Evidence path B
A modern v6/v7 companion is **not** proven to automatically exercise the old replacement-target `PrepareDraw` handshake per eye.

### Reconciled T0 implementation rule
T0 may proceed by either:
1. a deliberately verified compatible AfxInterop protocol/host mode that actually exercises the replacement-target path; or
2. a minimal internal transplant of the proven shared-target + EVENT-fence primitives into the already-proven v0.5.5 pass discriminator.

The current transport gate remains valid, but the actual protocol path must be logged and proven. Never assume v6/v7 equals old PrepareDraw behavior.

Detailed source provenance: `research/HLAE_INTEROP_FINDINGS.md`.

## Prepared CPU transport package
`CSGOVR_XR_TRANSPORT_v0.7.0_BUILD_FIRST_ONECLICK.zip`
- PREPARED/UNRUN
- D3D9 `GetRenderTargetData -> SYSTEMMEM -> CPU/shared memory -> x64 D3D11/OpenXR`
- COLD-SUPERSEDED AS PRIMARY while GPU sharing remains viable
- not runtime-falsified; retain as fallback/reference if GPU interop is concretely blocked

## Source-native projection policy queued after T0
Valve Source VR derives scalar FOV from the widest edge after asymmetric projection. Quest max half-angle is 55°, so queued G3/T1 scalar FOV is `110°`.

Quest P00 remains primary. Aspect `0.9254939479` and FOV150 remain diagnostics only after a concrete post-P00 clipping result.

## Native SourceVR HMD / eye scale
Valve `OpenVRToSourceCoordinateSystem()` uses `39.3700787 inches/meter`.
- 1 Source unit = 1 inch for native HMD/eye camera-space
- Quest 64.780414 mm ≈ 2.55041 Source units total / 1.27520 half-IPD
- nominal v0.5.5 half 1.2598425 is only ~1.2% lower

AdvancedFX Unity `0.01905 m/unit` is a different scene convention and is FALSIFIED as the native SourceVR eye scale.

## Capture-truth fallback
Old sequential comparison from one result set was invalid: same-eye temporal difference ~62% vs L/R ~54%.

v0.6.1 deterministic freeze/capture achieved within-phase MAE around `0.0000..0.0026 / 255`.

If T0/G3 visual measurement is noisy, reuse this method and require same-eye same-state drift `<1%` before quantitative L/R claims.

## Scheduling conflict and final resolution
Earlier concurrent state had G3 current but explicitly noted final XR transport was unproven.

Final schedule:
1. **T0 / G2.5 GPU XR TRANSPORT CERTIFICATION** with BASE projection/origin/camera only
2. on T0 PASS, freeze transport
3. immediately run **G3/T1 exact Quest P00 + Source-native FOV110**
4. only then open culling/origin/head work if a concrete visible blocker remains

## CURRENT GATE
**T0 / G2.5 — GPU XR TRANSPORT CERTIFICATION.**

Primary missing fact:
Can exact v0.5.5 pass1 LEFT + pass2 RIGHT be published as one coherent GPU pair to a separate x64 Meta OpenXR host, with BASE projection/origin/camera unchanged, through an actually verified compatible shared-resource path?

## NEXT
1. Preserve exact v0.5.5 scheduler/geometry and pass3 monitor.
2. Select/log either a compatible pinned AfxInterop protocol mode or minimal shared-target/fence transplant.
3. One same-cycle pass1 LEFT + pass2 RIGHT = one `pairSeq`.
4. GPU-complete each eye before pair publication.
5. x64 host uses D3D11 on OpenXR adapter LUID `00000000:000118D1`, imports eye resource(s), acquire/wait/copy/release both XR swapchains, one `xrEndFrame` per pair.
6. Require `mixedPairCount=0` and no GOLDEN regression.
7. T0 PASS -> freeze transport -> queued G3/T1 P00 + FOV110.
8. T0 FAIL -> fix only the concrete transport/protocol defect; do not widen search.

## Current finalists
1. verified-compatible pinned HLAE GPU shared-target path to x64 Meta OpenXR host
2. minimal shared-target + EVENT-fence transplant if protocol-mode mismatch blocks direct AfxInterop path
3. queued G3/T1 byte-locked Quest projection child after T0 PASS
4. only after visible post-G3 culling: `+0x3A16A6`, `+0x3A01E4`, `+0x86E9C0`, CSimpleWorldView lineage

## Do-not-repeat additions
- do not run CPU-readback XR_TRANSPORT v0.7.0 as default while GPU sharing is viable
- do not assume modern v6/v7 companion uses old shared-target PrepareDraw exchange
- do not combine transport + projection + origin + head pose in first T0 proof
- do not interpret prepared package/source capability as runtime RESULT
- do not reopen projection sign/transpose roulette
- do not use aspect/FOV150 as production fix without concrete post-P00 clipping evidence
