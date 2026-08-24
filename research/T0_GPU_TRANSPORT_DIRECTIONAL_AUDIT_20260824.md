# CSGOVR T0 GPU transport directional audit 2026-08-24

Evidence label: STATIC-AUDITED / NOT LIVE-PROVEN.

## Scope

Resolve one narrow T0 question before another LIVE: which exact pinned HLAE GPU-interoperability path can move a completed v0.5.5 LEFT/RIGHT game render toward the external x64 OpenXR host without changing projection, origin, head pose, culling, viewmodel or HUD.

Pinned source: `advancedfx/advancedfx@b97636852b8eecae09285b5a386192bb285638eb`.
Protected parent: `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`.

## RESULT

The previous protocol guard was directionally incomplete.

### Old pre-v6/v7 PrepareDraw path

`DrawingThreadPrepareDraw` can receive `sharedColorTextureHandle`, inspect the tracked game RT, open a matching D3D9 render-target texture using that handle, obtain its surface and push it with `AfxD3d9PushRenderTargetEx`.

This is an output/replacement-target path: subsequent game rendering is redirected into the external shared render target while HLAE keeps its tracked render-target state coherent.

Class: exact-source game -> shared-target capability PROVEN.

### Modern v6/v7 HandleDrawingMessage path

For server major 6/7, `DrawingThreadPrepareDraw` bypasses the old PrepareDraw replacement-target exchange.

`HandleDrawingMessage` still has a shared-texture `Continue` operation, but its direction is different:

1. it receives a shared texture handle plus width/height/format;
2. opens that texture through `IDirect3DDevice9Ex::CreateTexture(..., &sharedTextureHandle)`;
3. calls `AfxDrawRect(m_SharedSurface.Texture, ...)`;
4. `AfxDrawRect` binds that texture with `SetTexture(0, texture)` and draws it into the current D3D9 render target;
5. waits for GPU completion.

Therefore this operation is external shared texture -> current game RT / composite. It is not evidence that the completed LEFT/RIGHT game image is exported to the x64 host.

Class: exact-source directional semantics PROVEN.

## Consequence for T0 finalist A

Do not treat a successful modern v6/v7 shared-texture `Continue` as a T0 game-frame export PASS.

The exact-source export-capable primitive remains the old replacement-target mechanism. A modern companion may still be useful for other operations, but its shared-texture draw path does not by itself close T0.

## Candidate ordering after audit

### A0 - verified old-compatible replacement-target protocol

Use only if an exact compatible host/protocol mode can be pinned and logged. It must actually exercise the `sharedColorTextureHandle -> m_RenderTargetReplacement -> AfxD3d9PushRenderTargetEx` route for the relevant eye render.

KEEP only if compatibility can be established without protocol guessing.

### A1 - minimal replacement-target transplant into the proven v0.5.5 eye discriminator

Current strongest implementation fallback.

For force-cache pass1/pass2 only:

- retain byte-identical v0.5.5 `0x198` local CView transaction;
- obtain the HLAE-tracked RT dimensions/format;
- create/open per-eye shared D3D9 render-target texture supplied by the external host;
- use the same source-proven replacement-surface bookkeeping / push behavior around the eye render;
- wait for GPU completion before publishing the eye generation;
- restore/pop exactly;
- pass3 remains untouched.

This is preferred over a blind `IDirect3DDevice9::SetRenderTarget` because pinned HLAE tracks and unwraps its render target internally. `AfxD3d9PushRenderTargetEx` links source/replacement surfaces and updates `trackedRenderTarget`, preventing HLAE state from silently diverging from the raw D3D9 device state.

### A2 - post-render GPU copy

Retain only as a secondary candidate.

Concept: after the proven eye render, copy/resolve the completed eye RT into a shared GPU texture and fence it.

Risk found by this audit: HLAE has its own `trackedRenderTarget`, and the correct pass image is not yet proven to remain available as raw RT0 at the external v0.5.5 hook return boundary. A naive `GetRenderTarget(0)` post-copy can therefore capture the wrong surface or a restored target.

Do not LIVE A2 before its exact capture surface is statically or causally identified.

### B - prepared CPU fallback

`CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF` remains FINALIST B, PREPARED / STATIC-AUDITED / NOT LIVE-PROVEN. Activate only after a concrete GPU export/import blocker or as a bounded compatibility control.

## Exact HLAE render-target state facts imported

Pinned `d3d9Hooks.cpp` maintains a tracked render target. `AfxGetRenderTargetSurface()` returns `UnwrapSurface(trackedRenderTarget)`.

`AfxD3d9PushRenderTargetEx(replacementSurface)`:

- links the tracked source surface to replacement surface;
- if raw RT0 equals the tracked RT, switches raw RT0 to replacement;
- changes HLAE `trackedRenderTarget` to replacement and takes a reference.

`AfxD3d9PopRenderTarget(...)` follows the reverse source/replacement mapping and restores tracked/device state.

This is why the HLAE primitive is more authoritative than an out-of-band COM RT switch.

## Remaining COLD facts before building A1

1. Determine whether the exact functions needed for replacement bookkeeping can be safely called from an injected child by exported symbol, stable RVA/fingerprint, or whether a tiny local transplant is safer.
2. Determine the exact D3D9/D3D9Ex resource creation path and handle ownership needed for x64 D3D11 `OpenSharedResource` on the same adapter.
3. Pin supported color formats and MSAA behavior. Do not assume Quest swapchain dimensions or format match the game RT; host-side copy/scale remains allowed in T0.
4. Define Reset/device-generation invalidation for both eye shared resources.
5. Preserve same-cycle identity: LEFT count `0->1`, RIGHT `1->2`, same `CAfxStreams*`, complete pair only READY, `mixedPairCount=0`.

## PREFLIGHT for the first eventual T0 LIVE

MISSING FACT: whether exact v0.5.5 pass1/pass2 can be rendered/exported through one verified shared-GPU output path and imported/submitted by the x64 Meta OpenXR host as one coherent pair.
WHY NOT COLD: exact source now resolves direction and HLAE RT ownership, but only runtime proves actual resource sharing, adapter compatibility, synchronization, pair sequencing and XR submission in frozen retail.
PASS: same-cycle LEFT/RIGHT exported, both GPU-complete before READY, x64 imports/copies both resources, one `xrEndFrame` per coherent pair, `mixedPairCount=0`, non-black headset, pass3/GOLDEN unchanged.
FAIL: no export-capable path, shared-handle/import failure, wrong RT capture, mixed generation, GPU completion failure, XR copy/submit failure, black/freeze, or GOLDEN/pass3 regression.
BASELINE MUST HOLD: frozen client hash, `-insecure`, HLAE 2.191.1/b976, v0.5.5 full `0x198` local eye copies, pass1 LEFT/pass2 RIGHT/pass3 monitor, BASE projection/origin/camera, reversible cleanup.

## KEEP / ADAPT / STOP

- STOP the interpretation `modern v6/v7 shared-texture Continue == game-frame export`.
- KEEP old replacement-target primitive as exact-source donor.
- ADAPT primary implementation fallback from generic shared-target transplant to HLAE-state-aware replacement-target transplant A1.
- STOP naive post-copy A2 until the authoritative completed-eye surface at hook return is proven.
- KEEP CPU v0.7.1 only as fallback, not current primary.

No runtime RESULT is claimed by this document.