# HLAE / AdvancedFX / SourceVR Findings — 2026-08-24

Purpose: preserve exact source/static findings needed by the current T0 transport gate and later eye-state work. This is provenance, not a runtime PASS.

## Provenance
Pinned HLAE source identity:
`advancedfx/advancedfx@b97636852b8eecae09285b5a386192bb285638eb`

Relevant source reviewed:
- `AfxHookSource/AfxStreams.cpp/.h`
- `AfxHookSource/AfxInterop.cpp/.h`
- `AfxHookSource/d3d9Hooks.cpp/.h`
- `AfxHookSource/RenderView.cpp`
- `AfxHookSource/CMakeLists.txt`

Companion lineage reviewed:
- `advancedfx/afx-unity-interop` native D3D11 side and Unity wrapper

Valve lineage reviewed:
- SourceVR `client_virtualreality.cpp`
- SourceVR `sourcevirtualreality.cpp`
- `public/sourcevr/isourcevirtualreality.h`

## Exact HLAE stream ordering
Semantic order in the frozen lineage:
```text
skyView copy
-> SKY GetMatricesForView
-> NORMAL GetMatricesForView(view)
-> OnRenderBegin
-> DoRenderView(... view ...)
```

The normal stream uses the shared `const CViewSetup& view`; `m_CurrentView` refers to the current shared view through stream-stage logic.

Implication for later eye-origin work:
- late `DoRenderView` mutation can be too late for matrices already computed by NORMAL `GetMatricesForView`;
- if projection/transport are green but eye-origin/parallax is visibly wrong, the narrow finalist is save/apply/restore of the shared `0x198` CView before NORMAL matrix acquisition, with the same eye state surviving through `DoRenderView`.
- exact binary hook fingerprint remains PENDING.

## Pass identity anchors
- `CAfxStreams::DoRenderView = AfxHookSource.dll+0x00022340`
- `CViewSetup_csgo` copy ctor `+0x00014BA0`
- `this+0x30 = m_ForceCacheFullSceneState`
- `this+0x50 = m_DoRenderViewCount`
- `this+0x54 = m_ShowRenderViewCount`
- `DoRenderView` increments `m_DoRenderViewCount` before real Source `RenderView`

GOLDEN mapping remains pass1 LEFT / pass2 RIGHT / pass3 monitor.

## Copy-constructor candidate classification
The copy ctor `+0x14BA0` is **not** a clean standalone per-eye early hook: it also participates in sky/other CView copies. Blindly hooking it is STOPPED. Use stream-specific context if the earlier coherent eye-state gate opens.

## AfxInterop is in the exact Win32 build contract
At the pinned source:
- `AFX_INTEROP` is a compile definition for `AfxHookSource`;
- Win32 `target_sources` explicitly includes `AfxInterop.cpp` and `AfxInterop.h`.

Class: **PROVEN exact-source/build-contract fact**.

This does not prove that a particular interop server mode was active in previous CSGOVR runs.

## Shared render-target primitive
The older AfxInterop drawing path can:
1. receive `sharedColorTextureHandle`;
2. inspect current tracked D3D9 RT width/height/format;
3. call D3D9 `CreateTexture(... D3DUSAGE_RENDERTARGET, D3DPOOL_DEFAULT, ..., &sharedHandle)`;
4. obtain surface level 0;
5. `AfxD3d9PushRenderTargetEx(replacementSurface)`;
6. pop at `DrawingThread_OnRenderViewEnd`.

This is the exact class of D3D9<->external GPU sharing needed for T0.

Important resource limit:
- the opened replacement matches the tracked D3D9 RT dimensions/format;
- it does not automatically make CS:GO render natively at Quest `1504x1616`;
- a first transport proof may copy/scale in the external D3D11 host into XR swapchain images.

## GPU completion primitive
Exact `d3d9Hooks.cpp` contains:
- `CreateQuery(D3DQUERYTYPE_EVENT, &waitQuery)`
- `waitQuery->Issue(D3DISSUE_END)`
- loop `GetData(NULL,0,D3DGETDATA_FLUSH)` until completion

Class: **PROVEN exact-source primitive**.

T0 implication: CPU return is insufficient for publishing an eye resource. GPU completion must precede pair publication.

## CRITICAL PROTOCOL-MODE GUARD
The exact source contains multiple AfxInterop protocol generations.

In `DrawingThreadPrepareDraw`:
- older protocol path performs the `PrepareDrawReply_*` exchange and receives the shared replacement-target handles;
- for server major version **6 or 7**, that older replacement-target exchange is bypassed and the code proceeds through newer `HandleDrawingMessage` behavior.

Therefore these two statements must coexist:
1. **PROVEN:** the pinned HLAE source contains replacement-target sharing and GPU-wait primitives.
2. **NOT PROVEN:** simply connecting a modern v6/v7 AfxInterop companion automatically exercises the older shared-target `PrepareDraw` path per LEFT/RIGHT eye.

T0 must explicitly choose one of these implementation routes:
- a protocol mode/host that actually exercises the compatible replacement-target path; or
- a **minimal transplant** of the proven shared-target + EVENT-fence primitives into the already-proven v0.5.5 pass discriminator.

Do not silently assume v6/v7 has the old handshake.

## Current T0 transport finalist, reconciled
Current gate remains:
**T0 / G2.5 — GPU XR TRANSPORT CERTIFICATION.**

Required invariant:
```text
v0.5.5 pass1 LEFT + pass2 RIGHT from same HLAE cycle
-> one pairSeq
-> each eye GPU-complete before publication
-> external x64 D3D11 host on OpenXR adapter LUID 00000000:000118D1
-> acquire/wait/copy/release both XR swapchains
-> one xrEndFrame per coherent pair
-> mixedPairCount = 0
-> pass3 monitor unchanged
```

Implementation choice remains open between compatible pinned AfxInterop protocol usage and the minimal transplant described above. End-to-end runtime proof is PENDING.

## AfxInterop camera override scope
`OnViewOverride(float& Tx,Ty,Tz,Rx,Ry,Rz,Fov)` exists in the general camera override chain. It is useful as a possible center/mid-eye camera ingress, not as the HLAE per-eye duplication mechanism. Do not use it as proof of LEFT/RIGHT eye override.

## Valve SourceVR native scale cross-check
Valve `GetMidEyeFromEye()` takes OpenVR `GetEyeToHeadTransform()` and `OpenVRToSourceCoordinateSystem()` uses:
`inchesPerMeter = 39.3700787`.

Therefore native SourceVR HMD/eye camera space uses Source inches:
- `1 Source unit = 1 inch`
- Quest measured 64.780414 mm total separation ≈ `2.55041 Source units`
- half ≈ `1.27520 Source units`
- v0.5.5 nominal half `1.2598425` differs only about 1.2%

The temporary idea to use AdvancedFX Unity `0.01905 m/unit` as the native SourceVR eye scale is FALSIFIED for this purpose; it is a different Unity/Quake scene convention.

## Native SourceVR stereo structure
Valve SourceVR follows:
```text
middle-eye world transform
x GetMidEyeFromEye(LEFT/RIGHT)
-> per-eye origin/angles
-> independent eye projection
```

This supports the target model:
`MID pose + runtime eye-to-mid transform + per-eye asymmetric projection`.

## Capture-truth method retained
An old sequential comparator was invalid in one result set because same-eye temporal change was about 62% while L/R was about 54%.

The v0.6.1 internal freeze/capture path achieved within-phase MAE about `0.0000..0.0026 / 255`.

If T0/G3 screenshot evidence is noisy:
- reuse deterministic freeze/capture;
- require same-eye same-state drift `<1%` before quantitative L/R conclusions;
- keep `mat_queue_mode 0` and remove actor/bot motion.

## Status / scheduling
- exact HLAE source/build facts: PROVEN within stated scope
- native SourceVR scale: PROVEN source semantics
- T0 end-to-end transport: OPEN / runtime PENDING
- protocol-mode choice inside T0: OPEN, must honor the v6/v7 guard
- G3 Quest projection child: queued immediately after T0 PASS, not falsified
- earlier coherent eye-state seam: only if a visible post-transport/post-projection origin/parallax blocker remains
