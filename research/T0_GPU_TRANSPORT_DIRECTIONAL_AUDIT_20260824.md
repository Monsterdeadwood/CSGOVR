# CSGOVR T0 GPU transport directional audit 2026-08-24

Evidence label: STATIC-AUDITED / NOT LIVE-PROVEN.

## Scope

Resolve one narrow T0 question before another LIVE: which exact pinned HLAE GPU-interoperability path can move a completed v0.5.5 LEFT/RIGHT game render toward the external x64 OpenXR host without changing projection, origin, head pose, culling, viewmodel or HUD.

Pinned source: `advancedfx/advancedfx@b97636852b8eecae09285b5a386192bb285638eb`.
Protected parent: `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`.

Companion format audit: `research/T0_D3D9EX_D3D11_SHARED_FORMAT_AUDIT_20260824.md`.

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

The companion shared-format audit adds a second independent guard: the old replacement path opens the external handle using the authoritative tracked RT's exact `desc.Format`, while Microsoft's D3D9 -> D3D11 shared-resource endpoint has a restricted documented format/resource contract. Therefore protocol compatibility alone is not sufficient for A0/A1a.

## Candidate ordering after audit

### A0 - verified old-compatible direct replacement-target protocol

KEEP conditionally.

Use only if an exact compatible host/protocol mode can be pinned and logged **and** the authoritative tracked eye RT format is directly compatible with the D3D9 -> D3D11 endpoint.

It must actually exercise:

`sharedColorTextureHandle -> exact tracked desc -> m_RenderTargetReplacement -> AfxD3d9PushRenderTargetEx`.

Do not infer direct-share eligibility from protocol success alone.

### A1a - direct replacement transplant in the proven v0.5.5 eye discriminator

Implementation fallback that removes protocol-generation ambiguity but retains the direct shared-format requirement.

For force-cache pass1/pass2 only:

- retain byte-identical v0.5.5 `0x198` local CView transaction;
- obtain the HLAE-tracked RT dimensions/format;
- open per-eye shared D3D9 render-target texture using the same authoritative format;
- use source-proven replacement-surface bookkeeping / push behavior around the eye render;
- wait for GPU completion before publishing the eye generation;
- restore/pop exactly;
- pass3 remains untouched.

This is preferred over a blind `IDirect3DDevice9::SetRenderTarget` because pinned HLAE tracks and unwraps its render target internally. `AfxD3d9PushRenderTargetEx` links source/replacement surfaces and updates `trackedRenderTarget`, preventing HLAE state from silently diverging from raw D3D9 state.

### A1b - HLAE-aware GPU format bridge

New strongest GPU-lane fallback if the exact eye RT format is not directly shareable to D3D11.

The authoritative HLAE eye source remains pass-scoped. While that source is still known, perform a GPU-only resolve/copy plus checked format conversion into a non-MSAA D3D9 shared carrier whose format maps to the documented D3D9 -> D3D11 endpoint, currently `D3DFMT_A8B8G8R8 -> DXGI_FORMAT_R8G8B8A8_UNORM`.

Required gates include:

- actual source `desc.Format` and MSAA state logged;
- `CheckDeviceFormatConversion(actualSourceFormat, D3DFMT_A8B8G8R8) == D3D_OK`;
- relevant `StretchRect` capability present;
- shared carrier creation succeeds in `D3DPOOL_DEFAULT`, 1 mip, non-MSAA;
- GPU copy/resolve-convert succeeds;
- D3D9 EVENT completion precedes READY;
- x64 D3D11 `OpenSharedResource` succeeds on the required adapter;
- same-cycle LEFT/RIGHT remains atomic and `mixedPairCount=0`.

A1b is not a CPU fallback and does not alter projection/origin/camera.

### A2 - naive post-hook GPU copy

**STOP.**

The correct pass image is not proven to remain available as raw RT0 after the external v0.5.5 hook returns. A naive `GetRenderTarget(0)` there can capture a restored or unrelated surface.

Do not confuse A1b with A2: A1b executes against an authoritative HLAE-tracked/pass-scoped source; A2 guessed the source after the ownership boundary was lost.

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

## Current COLD closure state

Closed by source/documentation:

1. modern v6/v7 shared-texture direction is input/composite, not completed-eye export;
2. old replacement-target path is the source-proven game -> shared-target donor;
3. old direct path inherits exact tracked `desc.Format`;
4. D3D9 -> D3D11 sharing has a restricted documented resource/format contract;
5. if direct format sharing is unavailable, GPU format conversion is capability-dependent and must be queried, not assumed.

Still OPEN before the first transport mutation:

1. exact helper RVAs/fingerprints for HLAE-state-aware replacement bookkeeping / GPU completion in the frozen `AfxHookSource.dll`;
2. exact authoritative v0.5.5 eye RT format and MSAA state;
3. direct-share eligibility of that exact format on the frozen adapter/driver path;
4. if needed, actual `CheckDeviceFormatConversion` / `StretchRect` support into the cross-API carrier;
5. Reset/device-generation invalidation for both eye shared resources;
6. same-cycle identity: LEFT count `0->1`, RIGHT `1->2`, same `CAfxStreams*`, complete pair only READY, `mixedPairCount=0`.

The prepared read-only oracle under `experiments/t0_hlae_rt_primitive_oracle/` closes item 1 without launching CS:GO/HLAE.

Items 2-4 are runtime device facts and should be grouped into one bounded compatibility control only after item 1 is closed.

## PREFLIGHT for the first eventual T0 transport LIVE

MISSING FACT: whether exact v0.5.5 pass1/pass2 can be rendered/exported through one verified shared-GPU output path and imported/submitted by the x64 Meta OpenXR host as one coherent pair.
WHY NOT COLD: exact source and API contracts now resolve path direction, HLAE RT ownership and format constraints, but only runtime proves the frozen resource format/caps, sharing, synchronization, pair sequencing and XR submission.
PASS: same-cycle LEFT/RIGHT exported, both GPU-complete before READY, x64 imports/copies both resources, one `xrEndFrame` per coherent pair, `mixedPairCount=0`, non-black headset, pass3/GOLDEN unchanged.
FAIL: no export-capable path, direct/conversion format incompatibility, shared-handle/import failure, wrong RT capture, mixed generation, GPU sync failure, XR copy/submit failure, black/freeze, or GOLDEN/pass3 regression.
BASELINE MUST HOLD: frozen client hash, `-insecure`, HLAE 2.191.1/b976, v0.5.5 full `0x198` local eye copies, pass1 LEFT/pass2 RIGHT/pass3 monitor, BASE projection/origin/camera, reversible cleanup.

## KEEP / ADAPT / STOP

- STOP the interpretation `modern v6/v7 shared-texture Continue == game-frame export`.
- KEEP A0 only as a protocol- and format-conditional direct path.
- ADAPT A1 into A1a direct replacement and A1b HLAE-aware GPU format bridge.
- KEEP old replacement-target primitive as exact-source donor.
- STOP guessing actual eye format from fallback source guards.
- STOP naive post-hook A2.
- KEEP CPU v0.7.1 only as fallback B.

No runtime RESULT is claimed by this document.
