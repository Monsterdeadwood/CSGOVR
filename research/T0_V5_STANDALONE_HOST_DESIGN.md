# CSGOVR T0 V5 STANDALONE HOST DESIGN

Date: 2026-08-24
Evidence class: COLD exact-source convergence. NOT a runtime RESULT.

## Decision

Narrow T0 Finalist A to a standalone x64 AdvancedFX AfxInterop **server major 5** host, derived from the official `advancedfx/afx-unity-interop` v5 server protocol but with Unity removed.

Keep exact frozen HLAE 2.191.1 / AdvancedFX `b97636852b8eecae09285b5a386192bb285638eb` and exact v0.5.5 CSGOVR eye generation.

Do not invent a new IPC protocol for T0.

## Why server major 5

Exact pinned HLAE client explicitly accepts server major 5, 6 and 7.

For major 5, `DrawingThreadPrepareDraw` uses the old `PrepareDrawReply_Continue` exchange which accepts:
- `colorTextureWasLost`
- `sharedColorTextureHandle`
- depth lost/handle

When a new non-null color handle is supplied, HLAE opens it with the current tracked D3D9 render-target width/height/format using `IDirect3DDevice9::CreateTexture(..., D3DUSAGE_RENDERTARGET, D3DPOOL_DEFAULT, ..., &sharedHandle)`, gets surface level 0, and pushes it as the replacement render target.

Server major 6/7 bypasses this old shared-handle exchange and must not be assumed equivalent.

The official `afx-unity-interop` test server advertises AfxInterop version 5 and demonstrates the intended D3D9/D3D11 shared-surface architecture.

## Pair identity is COLD-proven

Pinned HLAE owns `m_EngineFrame`.

`m_EngineFrame` increments exactly once in `AfxInterop::BeforeFrameRenderStart()`.
Every `AfxInterop::OnRenderView()` during that engine frame sends the same `m_EngineFrame` to the server and queues drawing `PrepareDraw` with that same frame number.

Therefore T0 pair identity is:

`pairSeq = engineFrame`

Under the already-proven v0.5.5 HLAE schedule, accepted T0 callback order is exactly:
- ordinal 1 = LEFT world
- ordinal 2 = RIGHT world
- ordinal 3 = monitor

Any extra/missing/reordered `OnRenderView` inside the same frame invalidates that pair. Do not guess eye identity from Present.

## Two-shared-eye-texture architecture

Use two D3D11 shared textures owned by the x64 host:
- `SharedLeft`
- `SharedRight`

Primary expected format from the frozen path: D3D9 `A8R8G8B8` <-> DXGI `B8G8R8A8_UNORM`.
Actual width/height/format must be logged and verified at runtime.

Per frame N:

### Ordinal 1 / LEFT
Return `PrepareDrawReply_Continue` with:
- `colorTextureWasLost = true`
- handle = `SharedLeft`

HLAE releases any old replacement, opens SharedLeft through D3D9, and renders LEFT into it.
Request `AfterHud = true` on the engine response.

### Ordinal 2 / RIGHT
Return:
- `colorTextureWasLost = true`
- handle = `SharedRight`

HLAE releases the LEFT D3D9 wrapper, opens SharedRight, and renders RIGHT into it.
Request `AfterHud = true`.

### Ordinal 3 / MONITOR
Return:
- `colorTextureWasLost = true`
- handle = NULL

HLAE releases the RIGHT replacement and does not push a replacement RT. Monitor remains stock.
Do not request an eye capture callback.

This architecture avoids a shared-surface overwrite race without any CPU readback and without any intermediate D3D11 L/R copy during the game passes.

## GPU ownership / fence rule

For server major 5, exact HLAE `DrawingThread_AfterHud()` does:
1. `AfxD3D_WaitForGPU()`
2. sends `DrawingMessage_AfterHud`
3. blocks reading booleans until server returns `done=true`

Thus an observed AfterHud message is a real D3D9 GPU-completion boundary for that eye.

T0 records:
- `leftD3d9Fence = true` only after LEFT AfterHud
- `rightD3d9Fence = true` only after RIGHT AfterHud

Do not mark the pair READY merely because `OnRenderViewEnd` arrived.

### HUD contingency
`OnAfterHud` is wired to the real CS:GO `VGui_DrawHud_Out` hook. Therefore it is not promoted to an unconditional universal render-end callback.

T0 direct-v5 path PASS requires exactly two eye AfterHud callbacks for each accepted 1/2/3 cycle.

If either eye lacks AfterHud:
- do not submit that pair
- classify direct-v5 synchronization as concretely blocked for this schedule
- ADAPT to A', a minimal source-proven D3D9 EVENT fence at the v0.5.5 pass seam
- do not fall back to CPU readback unless A' is also concretely blocked

## Pair READY boundary

Pair N becomes READY only after all are true:
- same `engineFrame` for ordinals 1,2,3
- ordinal 1 mapped to SharedLeft
- LEFT AfterHud D3D9 fence observed
- ordinal 2 mapped to SharedRight
- RIGHT AfterHud D3D9 fence observed
- ordinal 3 restored stock target
- ordinal 3 drawing `OnRenderViewEnd` observed

Waiting until monitor `OnRenderViewEnd` means both eye replacement wrappers have already been removed from the game side before the host consumes them.

## One-pair lockstep for T0

Do not double-buffer yet.

Use one logical pair at a time:

`FREE -> WRITING_L -> L_FENCED -> WRITING_R -> R_FENCED -> MONITOR -> READY -> SUBMITTING -> FREE`

Before accepting ordinal 1 of frame N+1, block the server response while frame N is READY/SUBMITTING.

This intentionally throttles frozen CS:GO to XR for the certification run. Performance is not the PRIMARY question.

It guarantees SharedLeft/SharedRight are never overwritten while the XR host consumes them.

## x64 D3D11 / OpenXR side

Create D3D11 on the OpenXR-required adapter LUID already proven by the Quest diagnostic:
`00000000:000118D1`.

Use `D3D11_CREATE_DEVICE_BGRA_SUPPORT`.

Create SharedLeft/SharedRight with:
- one mip
- sample count 1
- DEFAULT usage
- render-target-compatible BGRA8 format
- `D3D11_RESOURCE_MISC_SHARED`

Obtain legacy shared handles and return them through AfxInterop v5.

The first LIVE must log whether frozen D3D9 successfully opens both handles. Failure is a concrete adapter/format/share blocker, not projection/camera evidence.

## XR submit

For T0, keep BASE game projection/origin/camera unchanged.

The engine AfxInterop `OnRenderView` message already contains the actual `viewToProjection` matrix used for the current pass. If a layer FOV is required, derive it from that actual matrix rather than inserting Quest P00 during T0.

For the Source projection convention already causally established:
- `tanLeft  = (M02 - 1) / M00`
- `tanRight = (M02 + 1) / M00`
- `tanDown  = (M12 - 1) / M11`
- `tanUp    = (M12 + 1) / M11`
- angles = atan(tan*)

This preserves mutation serialization: transport only in T0, Quest P00 only in queued G3/T1.

Preferred direct-copy path:
- enumerate OpenXR swapchain formats
- if a BGRA8 format compatible with the shared eye textures is offered, create per-eye XR swapchains at a legal size matching the source RT and `CopyResource`
- if direct format/size compatibility is absent, classify exact reason before adding a shader blit

Submit one coherent pair with one `xrEndFrame`.
Do not release/reuse SharedLeft/SharedRight for the next game frame until the host GPU copy into XR images is complete.

## Threading

T0 can stay simple:
- engine named-pipe worker: protocol v5 engine messages and pair/ordinal metadata
- drawing named-pipe worker: PrepareDraw + AfterHud + OnRenderViewEnd
- XR/D3D11 worker: owns D3D11 immediate context and OpenXR session/swapchains

Because D3D11 work occurs only after pair READY in this two-texture lockstep design, no cross-thread immediate-context use is required.

## Official donor use

Reuse/extract the official public `advancedfx/afx-unity-interop` v5 named-pipe message definitions/serialization instead of recreating wire layout from memory.

Do NOT inherit the Unity sample's render-plugin scheduling. Its README says Unity needed queued/threaded rendering and `mat_queue_mode != 0`; CSGOVR baseline remains `mat_queue_mode 0`. A standalone host owns its worker threads and must validate this directly.

Launch contract retained from the official sample:
- HLAE launch option `-afxInterop` is case-sensitive
- default pipe `advancedfxInterop`
- console `afx_interop connect 1`

## T0 exact PREFLIGHT for eventual LIVE

MISSING FACT: whether frozen v0.5.5 LEFT/RIGHT passes can render into two x64-owned D3D11 shared textures through AfxInterop v5 and reach one coherent Meta OpenXR frame.
WHY NOT COLD: exact source proves protocol, handles, pair identity and D3D9 fence behavior, but only runtime can prove adapter sharing, actual HUD-fence occurrence and XR swapchain submission.
PASS: engineFrame 1/2/3 cycle valid; SharedLeft/SharedRight D3D9 opens succeed; exactly two eye D3D9 fences occur; mixedPairCount=0; pass3 monitor remains stock; one coherent pair is copied/submitted to Meta OpenXR and visible; BASE geometry unchanged.
FAIL: v5 connection/handle-open failure, missing eye AfterHud fence, ordinal/frame mismatch, pair overwrite, XR acquire/copy/release/end-frame failure, black/freeze, or GOLDEN/monitor regression.
BASELINE MUST HOLD: frozen client hash, exact `-insecure`, HLAE 2.191.1 / b976, v0.5.5 full-0x198 eye path, pass1 LEFT/pass2 RIGHT/pass3 monitor, BASE projection/origin/camera, reversible restore.

## Current convergence

T0 Finalists after this COLD pass:
1. A5 PRIMARY: standalone x64 AfxInterop server major 5 + two shared eye textures + AfterHud D3D9 fences + one-pair XR lockstep.
2. A' only if eye AfterHud is concretely absent: minimal D3D9 EVENT fence transplant at the proven v0.5.5 pass seam, keeping the same two shared textures.
3. B only if GPU sharing itself is concretely blocked: prepared `CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF` CPU fallback.

No G3 projection, new origin, head pose, culling, viewmodel, weapon or HUD mutation belongs in T0.

## NEXT COLD step

Implement the standalone v5 server core by transplanting the official public v5 protocol/serialization and replacing Unity callbacks with the state machine above. Run static protocol self-tests before any game LIVE.
