# CSGOVR — TRANSPORT BRANCH STATE SYNC — 2026-08-24

Purpose: preserve all useful transport-branch work without overwriting the parallel GPU-transport reconciliation in `main`.

Read together with:
- `research/BRANCH_SYNC_20260824.md`
- `docs/STATE.md`
- `docs/NEXT.md`
- `docs/GATES.md`

## REMOTE RACE / CROSS-BRANCH RECONCILIATION
This branch began from remote `main@20f76e6bbd849b5aff5f25d00ae63505ef1b96c0`.

While synchronization was in progress, parallel CSGOVR branches advanced main twice:
1. `c41a20d1f90ec24947fddb6dd648c549e2b12352` — urgent state sync / projection state preservation.
2. `595c21c004cae620488fab78465728e5bf83da0e` — transport-gate reconciliation selecting exact pinned HLAE GPU interop as T0 primary.

Those changes were imported instead of overwritten.

### Final branch relationship
Both transport branches agree on the CURRENT GATE:
**T0 / G2.5 XR TRANSPORT CERTIFICATION.**

They differ only in implementation finalist:
- **FINALIST A / remote primary:** pinned HLAE GPU interop -> x64 D3D11/OpenXR.
- **FINALIST B / this branch prepared fallback:** HLAE pass-aware D3D9 CPU readback -> atomic shared pair -> x64 D3D11/OpenXR.

No LIVE result has yet falsified either transport finalist. Therefore both are retained, with A as NEXT and B as prepared compatibility fallback.

## LATEST RESULT
No completed LIVE transport result was produced here.

Prepared output:
`CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF`

- artifact SHA256 `DFEA881888C83812A5B9EE3A7049F79D32935240F2C612DE4ABDF4F9A98834A3`
- PREPARED / STATIC-AUDITED / NOT LIVE-PROVEN
- supersedes prepared `v0.7.0_XR_TRANSPORT` before LIVE
- manifest: `runtime/children/v0.7.1_xr_transport/README.md`

## PROVEN IMPORTED INTO THIS BRANCH
### Frozen target
- standalone frozen legacy CS:GO x86
- Steam AppID `4465480`
- BuildID `22100934`
- game path `C:\Program Files (x86)\Steam\steamapps\common\csgo legacy`
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- `RenderView = client.dll+0x003A5280`

### HLAE / GOLDEN
- GOLDEN `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`
- canonical result `CSGOVR_HLAE_EYE_PATCH_RESULT_20260821_015737.zip`
- HLAE 2.191.1
- AdvancedFX commit `b97636852b8eecae09285b5a386192bb285638eb`
- `HLAE.exe` SHA256 `D7BA05E1E540D82AC485A9B39F13D5C4CCF9FD94F460E0C9ECF069B94C880A59`
- `AfxHookSource.dll` SHA256 `5B9D13AE9ABFFEEF80851E10861302162A47D1B43D8483B4A8FCEC7EAA19B035`
- `CAfxStreams::DoRenderView = AfxHookSource.dll+0x00022340`
- `CViewSetup_csgo` copy ctor `+0x00014BA0`
- pass1 LEFT, pass2 RIGHT, pass3 monitor
- full local `CViewSetup=0x198`; final byte copied `+0x197`
- HLAE object `this+0x30=m_ForceCacheFullSceneState`, `+0x50=m_DoRenderViewCount`, `+0x54=m_ShowRenderViewCount`
- observed repeating count sequence `1 -> 2 -> 3`, forceCache `1 -> 1 -> 0`

Exact source ordering retained:
`skyView copy -> SKY GetMatricesForView -> NORMAL GetMatricesForView -> OnRenderBegin -> DoRenderView -> QueueCapture -> OnRenderEnd`.

### Frozen CViewSetup fields
- `+0x68 m_bCustomProjMatrix`
- `+0x6C..+0xAB m_matCustomProjMatrix`
- `+0xB0 world FOV`
- `+0xB4 viewmodel FOV`
- `+0xB8 origin`
- `+0xC4 angles`
- `+0xC8 yaw`
- `+0xD0/+0xD4 world zNear/zFar`
- `+0xD8/+0xDC viewmodel zNear/zFar`
- `+0xE0 aspect`
- `+0x138/+0x13C/+0x140/+0x144` off-center top/bottom/left/right
- `+0x148.bit0 m_bOffCenter`

Parallel sync additionally retains `+0xC0 origin.z`, DoF defaults `+0xE4..+0xF8`, motion-blur mode `+0x100`, and source-layout candidates `+0x34/+0x38..+0x67/+0xAC` with their existing evidence grades.

### Quest/OpenXR
Result `CSGOVR_OPENXR_FOV_DIAGNOSTIC_20260821_150332.zip`:
- Meta/Oculus OpenXR 1.205.0 in recorded run
- two views; `viewStateFlags=0xF`
- recommended `1504x1616`; max `3008x3232`
- OpenXR D3D11 adapter LUID retained by parallel sync as `00000000:000118D1`
- LEFT L=-54°, R=+40°, U=+44°, D=-55°
- RIGHT L=-40°, R=+54°, U=+44°, D=-55°
- passive eye separation ~`64.780414 mm`

Derived projection core:
- LEFT `M00=0.9027383310563113`, `M02=-0.24251263135515727`, `M11=0.8354788620481540`, `M12=-0.19318743951911962`
- RIGHT `M00=0.9027383310563113`, `M02=+0.24251263135515727`, `M11=0.8354788620481540`, `M12=-0.19318743951911962`

### D3D9 CPU capture primitive
Historical CSGOVR source/result proves:
- a passive D3D9 Present hook can latch the real `IDirect3DDevice9*`
- CPU snapshots execute in frozen retail
- MSAA-safe readback uses `StretchRect` to a non-MSAA DEFAULT RT before `GetRenderTargetData` into SYSTEMMEM

This primitive is PROVEN; v0.7.1 end-to-end transport is not.

### Culling / frustum
- `GeneratePerspectiveFrustum = client+0x0086E9C0`
- writes `+0x0086EB3C/+0x0086EB46/+0x0086EB51/+0x0086EB59`
- concrete pre-eye reader `client+0x003A16A6..+0x003A16DC`
- second reader `client+0x003A01E4..+0x003A021A`
- helper arg2 first `0x40` bytes temporary in-place transpose/restore PROVEN
- CSimpleWorldView main-world lineage PROVEN: vtable `client+0x00BF4434`, Draw `client+0x003B0870`
- lazy hydration `+0x003B0067..+0x003B023D`
- current view-slot index `client+0x05248ED4`
- arrays `+0x0524A2F4/+0x0524A2F8/+0x0524A2FC`, stride `0x13F0`

## TRANSPORT-SPECIFIC COLD FINDINGS FROM THIS BRANCH
### Exact HLAE capture seam
The existing eye hook regains execution after original HLAE `DoRenderView` has rendered the current stream, before `CaptureStreamToBuffer` proceeds to `QueueCapture`. This structurally permits pass-aware capture without using Present to infer eye identity.

### Existing CSGOVR Present/device latch
Older CSGOVR bridge source already used a dummy D3D9 vtable Present hook (slot 17) to cache the real device pointer. This was reused conceptually for the CPU fallback.

### Reset lifecycle
Exact HLAE source releases/recreates tracked D3D9 resources around Reset/device-lost handling. The prepared CPU fallback therefore adds system D3D9 Reset slot 16 handling: generation increment + pooled-resource/pair invalidation rather than keeping stale COM surfaces.

### HLAE D3D9Ex history
Exact AdvancedFX source/history contains disabled code for hiding a D3D9Ex underlying device from CS:GO and a historical commit described as `Backup of dead end` for forced D3D9Ex work. Recorded practical issues included MSAA/depth-sharing limitations.

This branch therefore demoted **forced D3D9Ex game-device conversion**. It does not contradict the newer remote GPU-interoperability finalist, which uses the pinned HLAE interop machinery rather than blindly forcing the game device into Ex mode.

### Public AfxInterop sample caveat
The public Unity interop sample is an architecture donor only and historically expects queued/threaded rendering, while the frozen lab uses `mat_queue_mode 0`. Do not transplant that sample protocol/assumptions blindly into CSGOVR.

## v0.7.1 PREPARED CPU FALLBACK
Pipeline:
`HLAE completed pass -> current D3D9 RT -> MSAA-safe CPU readback -> 2-slot packed shared pair ring -> x64 D3D11/OpenXR host`.

Transport ABI:
- fixed-width types only
- `#pragma pack(4)` for x86/x64 stability
- generation / pair id / eye / pass / dimensions / pitch / format / timestamps
- slot state machine prevents partial-pair consumption

### Structural same-HLAE pair proof
Added after TAB4/same-frame sync:
- same `CAfxStreams*`
- LEFT `m_DoRenderViewCount 0 -> 1`
- RIGHT `1 -> 2`
- same control/projection mode
- complete pair only becomes READY

One-shot evidence tokens independent of throttled logging:
- producer `DISTINCT PAIR PROOF`
- host `PAIR PROOF PASS`

### v0.7.1 hardening / fixed implementation traps
- removed camera freeze accidentally inherited from the old projection oracle; transport uses live v0.5.5-style input view
- stale control INI deleted before control thread, avoiding prior v0.6.5-style stale-path confusion
- PowerShell parameter avoids automatic `$PID` name collision
- x86/x64 ABI pack fixed to 4-byte alignment
- shared mapping reduced to two pair slots to reduce x86 VA pressure
- RGB-only hashing avoids undefined X byte in `D3DFMT_X8R8G8B8`
- pair-mode switch between LEFT/RIGHT invalidates pair instead of publishing mixed geometry
- sRGB-preferred XR color formats; D3D11 BGRA support
- T0 host/session diagnostic before launching game
- loader lookup reuses known OpenXR tool-root path/fallback
- known `XR_ERROR_FORM_FACTOR_UNAVAILABLE` treated as Link/HMD availability infrastructure condition
- cleanup order closes injected test game before host
- build failures also produce a compact RESULT/log set

## FALSIFIED / CLOSED / DO NOT REPEAT
- full `CViewSetup=0x14C`
- `client+0x0525BA60` reused inside both eye marker brackets
- persistent arg2 output outside first `0x40`
- parser-generated old falsification of the first-0x40 transpose
- post-SetUpView candidates already retired in canonical state
- sequential screenshot sets as quantitative same-frame disparity/culling truth
- origin/yaw byte-integrity controls promoted to camera ownership
- broad Quest FOV/OpenXR rediscovery
- Present as eye identity/frame oracle
- forced D3D9Ex conversion as the default CPU-fallback path
- HLAE preview as first scheduler replacement
- prepared `v0.7.0_XR_TRANSPORT`, superseded by v0.7.1 before LIVE

## CURRENT FINALISTS AFTER RECONCILIATION
### A — PRIMARY
Pinned HLAE GPU interop / replacement eye targets -> same-cycle coherent GPU pair -> external x64 D3D11/OpenXR host.

Use the exact current remote `docs/NEXT.md` contract.

### B — PREPARED FALLBACK
`CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF`

Activate only if A reaches a concrete GPU interop/shared-resource/runtime blocker or as a bounded compatibility control. Do not run B merely because it already exists if A remains the stronger current path.

### QUEUED T1/G3
Repository byte-locked minimal Quest asymmetric projection child. It is not falsified and begins immediately after transport PASS.

## CONCRETE TARGETS / SYMBOLS / HOOKS
### HLAE / render
- `client.dll+0x003A5280` RenderView
- `AfxHookSource.dll+0x00022340` CAfxStreams::DoRenderView
- `AfxHookSource.dll+0x00014BA0` CViewSetup copy ctor
- HLAE object `+0x30/+0x50/+0x54`
- D3D9 device Present vtable slot 17 for passive device latch
- D3D9 device Reset vtable slot 16 for fallback resource generation handling

### Projection
- `CViewSetup+0x68`
- `+0x6C..+0xAB`
- off-center `+0x138/+0x13C/+0x140/+0x144/+0x148.bit0`

### Camera retained infrastructure only
- `client+0x0038D123`
- `client+0x0038D15B`
- `client+0x0038D1DE`
- `client+0x05245AB8`
- `client+0x052632CC`
- `client+0x05263384`

### Culling
- `client+0x0086E9C0`
- `client+0x003A16A6`
- `client+0x003A01E4`
- `client+0x003B0870`
- `client+0x0524A2F4/+0x0524A2F8/+0x0524A2FC`
- stride `0x13F0`

## CURRENT GATE
**T0 / G2.5 — XR TRANSPORT CERTIFICATION.**

## NEXT
Follow `docs/NEXT.md`: test the pinned HLAE GPU-interoperability finalist first with BASE geometry.

If that path is concretely blocked, the exact prepared CPU fallback state is preserved at `runtime/children/v0.7.1_xr_transport/README.md` and can be activated without repeating the research that produced it.

After T0 PASS: freeze transport and immediately run the queued Quest projection child.

## METHOD ALREADY USED
- source/donor corpus harvest
- exact frozen static mapping
- HLAE source/binary seam mapping
- v0.5.5 multipass runtime GOLDEN
- custom projection/off-center causal canaries
- deterministic projection oracle
- Quest OpenXR FOV diagnostic
- D3D9 Present/device latch + CPU readback/MSAA resolve
- TTD frustum/cache replay and pre-eye reader recovery
- x32dbg/origin-yaw infrastructure controls
- CSimpleWorldView class/lazy-hydration mapping
- OBS / FEAR-VR / CallOfDuty4-VR / capture-engine transport donor comparison
- exact AdvancedFX D3D9Ex/history review
- cross-branch GitHub reconciliation against concurrent GPU transport work

## UNRESOLVED
- T0 end-to-end transport runtime verdict
- whether pinned GPU interop A is compatible with exact frozen runtime under current lab mode
- whether CPU fallback B is needed
- G3 Quest projection in-headset verdict after transport
- authoritative eye-origin seam only if visible defect remains
- culling policy only if visible defect remains
- head pose, weapon/hands/aim, HUD, regression/release gates
