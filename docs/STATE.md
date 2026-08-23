# CSGOVR State

Snapshot: 2026-08-24 final cross-branch reconciliation including GPU/CPU transport branches and ending-branch protocol guard.

## STATUS
ACTIVE.

## AUTHORITY / REMOTE SYNC
Canonical evidence base remains `CSGOVR_KNOWLEDGE_BASE_CANONICAL_20260823.md`, augmented by repository state-sync documents. `main` moved repeatedly during synchronization; newer remote evidence was imported rather than overwritten. Only visible integration branch: `main`.

## FROZEN TARGET / DEPENDENCIES
- legacy CS:GO x86, Steam AppID `4465480`, BuildID `22100934`
- game `C:\Program Files (x86)\Steam\steamapps\common\csgo legacy`
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- `RenderView = client.dll+0x003A5280`
- mandatory `-insecure`
- HLAE `2.191.1`; exact AdvancedFX source `b97636852b8eecae09285b5a386192bb285638eb`
- HLAE package SHA256 `D7BA05E1E540D82AC485A9B39F13D5C4CCF9FD94F460E0C9ECF069B94C880A59`
- `AfxHookSource.dll` SHA256 `5B9D13AE9ABFFEEF80851E10861302162A47D1B43D8483B4A8FCEC7EAA19B035`
- Meta/Oculus OpenXR `1.205.0`, Quest 3
- D3D11 adapter LUID `00000000:000118D1`, minimum FL `0xB000`, created `0xB100`
- shared tools `C:\GOG Games\VR Research Tools`
- lab baseline: local `sv_lan`, Dust II, windowed, `mat_queue_mode 0`, no persistent game-file patch

## LATEST RESULT
Latest accepted scientific result overall:
`CSGOVR_TTD_FRUSTUM_ARG2_PERSISTENCE_RESULT_20260823_164733.zip`
- PROVEN first helper arg2 `0x40` temporary transpose/restore
- FALSIFIED persistent `+0x40..+0x1FF`

Accepted supporting results:
- `CSGOVR_HLAE_EYE_PATCH_RESULT_20260821_015737.zip` — GOLDEN stereo
- `CSGOVR_CUSTOM_PROJECTION_MATRIX_RESULT_20260821_042218.zip` — custom projection semantics
- `CSGOVR_OPENXR_FOV_DIAGNOSTIC_20260821_150332.zip` — Quest/OpenXR input

Prepared, NOT accepted RESULT:
- `CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF`, SHA256 `DFEA881888C83812A5B9EE3A7049F79D32935240F2C612DE4ABDF4F9A98834A3` — PREPARED / STATIC-AUDITED / NOT LIVE-PROVEN CPU fallback, supersedes prepared v0.7.0 before LIVE
- `CSGOVR_XR_TRANSPORT_v0.7.0_BUILD_FIRST_ONECLICK.zip` — superseded inside CPU lineage; not runtime-falsified
- `experiments/quest_projection_child/` — queued G3/T1 projection implementation, UNRUN
- `CSGOVR_G3_QUEST_COHERENCE_BATCH_v1.0.1_SYNCED_ONE_CLICK.zip`, SHA256 `595DA174AC39BA334F725DC1D130089F7F7C98D2BC4878F87B12F113BC0EAB70` — UNRUN/NOT BASELINE

## PROVEN
### Stereo GOLDEN / HLAE
- GOLDEN `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`
- pass1 LEFT world, pass2 RIGHT world, pass3 monitor
- full `CViewSetup=0x198`; `0x14C` full-size claim SUPERSEDED
- `CAfxStreams::DoRenderView = AfxHookSource.dll+0x00022340`
- `CViewSetup_csgo` copy ctor `+0x00014BA0`, through byte `0x197`
- HLAE `this+0x30 m_ForceCacheFullSceneState`, `+0x50 m_DoRenderViewCount`, `+0x54 m_ShowRenderViewCount`; observed repeating count 1/2/3
- nominal half-eye `1.2598425 Source units`
- original v0.5.5 BMPs show stable depth-dependent horizontal L/R disparity with negligible vertical disparity
- exact source order retained: skyView copy -> SKY GetMatricesForView -> NORMAL GetMatricesForView -> OnRenderBegin -> DoRenderView -> QueueCapture -> OnRenderEnd

### Retail `CViewSetup`
- `+0x68 m_bCustomProjMatrix`, `+0x6C..+0xAB m_matCustomProjMatrix`
- `+0xB0 world FOV`, `+0xB4 viewmodel FOV`
- `+0xB8 origin`, `+0xC0 origin.z`
- `+0xC4 angles`, `+0xC8 yaw`
- `+0xD0/+0xD4 world zNear/zFar`, `+0xD8/+0xDC viewmodel zNear/zFar`
- `+0xE0 aspect`
- `+0xE4..+0xF8` observed DoF defaults; `+0x100` motion blur mode default 2
- off-center `+0x138/+0x13C/+0x140/+0x144`, enable `+0x148 bit0`
- source-layout candidates only `+0x34 customView bool`, `+0x38..+0x67 customView matrix`, `+0xAC CSMVolumeCuller`

### CPU -> GPU / shaders
- `CViewSetup.origin -> VS c2 / CAMERA_POS`
- `c4..c7 MODELVIEWPROJ`, `c8..c11 VIEWPROJ`, `c17`, bounded `c58..c60`
- WORLD_X_PAIR proves RenderView-origin translation can affect GPU c2, pixels and draw membership
- D3D9 `SetPixelShader` slot 107, `SetPixelShaderConstantF` slot 109

### Projection / Quest
- custom projection `+0x68/+0x6C..+0xAB` PROVEN; M00/M02/M11/M12 causal semantics
- Quest 2 views, `viewStateFlags=0xF`, recommended `1504x1616`, max `3008x3232`
- LEFT FOV L=-54 R=+40 U=+44 D=-55; RIGHT L=-40 R=+54 U=+44 D=-55 degrees
- LEFT P00 M00 `0.9027383310563113`, M02 `-0.24251263135515727`, M11 `0.8354788620481540`, M12 `-0.19318743951911962`
- RIGHT same except M02 `+0.24251263135515727`
- Source/D3D depth convention `M22=zFar/(zNear-zFar)`, `M23=zNear*zFar/(zNear-zFar)`, `M32=-1`, `M33=0`
- queued Source-native scalar FOV policy = `110°`

### Native SourceVR scale
Valve SourceVR converts OpenVR meters using `39.3700787 inches/meter`.
- native HMD/eye path uses `1 Source unit = 1 inch`
- Quest `64.780414 mm` ≈ `2.55041 Source units` total / `1.27520` half-IPD
- nominal v0.5.5 half 1.2598425 differs only about 1.2%

### Capture truth
- old sequential comparator INVALID in one result set: same-eye temporal difference ~62% vs L/R ~54%
- v0.6.1 deterministic freeze/capture achieved within-phase MAE ~`0.0000..0.0026 / 255`
- same-eye same-state drift `<1%` required before quantitative L/R claims when capture truth is in question

### Exact pinned HLAE GPU interop capability — source/build proof
At `b976368...`:
- Win32 build defines `AFX_INTEROP`, includes `AfxInterop.cpp/.h`
- replacement RT support via `AfxD3d9PushRenderTargetEx`
- older `PrepareDrawReply_Skip/Retry/Continue` shared-target path
- GPU completion via `D3DQUERYTYPE_EVENT`: `Issue(D3DISSUE_END)` + `GetData(...D3DGETDATA_FLUSH)`
- NORMAL `GetMatricesForView` precedes `DoRenderView`; same shared view / `m_CurrentView` persists through stage

Capability is source-PROVEN; end-to-end transport remains runtime-OPEN.

### Critical AfxInterop protocol guard
Exact source also proves:
- older `DrawingThreadPrepareDraw` receives replacement shared handles
- server major v6/v7 bypasses that older exchange and uses newer `HandleDrawingMessage`

T0 must therefore use/log a verified-compatible protocol mode or a minimal transplant of the proven shared-target + EVENT-fence primitives. Do not assume modern v6/v7 automatically exercises old `PrepareDraw` behavior.

### D3D9 CPU fallback primitive
Historical CSGOVR transport source/result proves:
- passive D3D9 Present hook can latch real `IDirect3DDevice9*`
- CPU snapshots execute in frozen retail
- MSAA-safe readback uses `StretchRect` to non-MSAA DEFAULT RT then `GetRenderTargetData` to SYSTEMMEM

Primitive PROVEN; v0.7.1 end-to-end transport is not.

### v0.7.1 prepared fallback hardening — STATIC-AUDITED, not LIVE-PROVEN
- same `CAfxStreams*` pair identity
- LEFT `m_DoRenderViewCount 0->1`, RIGHT `1->2`
- complete pair only READY; tokens `DISTINCT PAIR PROOF` / `PAIR PROOF PASS`
- fixed-width `pack(4)` ABI
- Reset slot 16 generation invalidation
- RGB-only hash for X8R8G8B8
- live v0.5.5 camera input
- stale-control cleanup, `$PID` runner fix
- two-slot shared pair ring
- sRGB-preferred XR format / D3D11 BGRA support

Manifest: `runtime/children/v0.7.1_xr_transport/README.md`.

### Culling / world-view
- `client+0x003A16A6..+0x003A16DC` pre-eye reader/dataflow with recovered stack; destination semantics PENDING
- second reader `+0x003A01E4..+0x003A021A`
- `GeneratePerspectiveFrustum +0x0086E9C0`, writers start `+0x0086EB3C`
- CSimpleWorldView vtable `+0x00BF4434`, Draw `+0x003B0870`
- `+0x168 IWorldRenderList*`, `+0x16C CClientRenderablesList*`, `+0x170 ClientWorldListInfo_t*`
- hydration `+0x3B0067/+0x3B0083/+0x3B008F/+0x3B00A3/+0x3B00BF/+0x3B00CB`; post `+0x3B00D9`; consumers `+0x3B010D/+0x3B01FD/+0x3B0225/+0x3B023D`
- current slot `+0x05248ED4`; arrays `+0x0524A2F4/+0x0524A2F8/+0x0524A2FC`, stride `0x13F0`

## EVIDENCE RECONCILIATION
1. one late DUAL_ORIGIN intervention did not authoritatively change final capture
2. GOLDEN shows real depth-dependent parallax and WORLD_X_PAIR proves origin change can reach GPU/raster

Conclusion: only that late point is negative; eye translation generally is not falsified.

## FALSIFIED / CLOSED / STOPPED
- full CViewSetup 0x14C
- raw Khronos column-major memcpy into Source VMatrix
- `+0x0525BA60` both-eye reuse
- persistent arg2 tail
- old parser first-0x40 falsification INVALID/SUPERSEDED
- static post-SetUpView candidates `+0x38E5D7/+0x38EC08/+0x3A0381`
- skybox/shadow-depth classes as main-world owner
- origin/yaw byte-integrity wrappers as ownership proof
- broad OpenXR rediscovery / projection sign roulette
- sequential screenshot quantitative truth without control
- broad origin batch until concrete post-G3 defect
- Present as eye identity oracle
- forced D3D9Ex conversion as default fallback path
- v0.7.0 prepared CPU transport, superseded by v0.7.1 before LIVE
- CPU readback as T0 PRIMARY while GPU sharing remains unblocked; fallback retained
- AdvancedFX Unity `0.01905 m/unit` as native SourceVR eye scale
- blind copy ctor `+0x14BA0` as clean early per-eye hook
- assuming v6/v7 AfxInterop uses old shared-target PrepareDraw handshake

## RETAINED INFRASTRUCTURE ONLY
- origin `+0x0038D123`, CView `+0x052632CC`, origin-global `+0x05263384`
- yaw `+0x0038D15B/+0x0038D1DE/+0x05245AB8`

## CURRENT FINALISTS
1. **T0 A / PRIMARY:** verified-compatible pinned HLAE GPU replacement-eye-target transport -> GPU completion -> x64 D3D11/Meta OpenXR.
2. **T0 A' / implementation fallback inside primary lane:** minimal shared-target + EVENT-fence transplant if protocol-mode mismatch blocks direct AfxInterop usage.
3. **T0 B / PREPARED FALLBACK:** `CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF` CPU readback/shared pair; activate only on concrete A blocker or bounded compatibility control.
4. **G3/T1 after T0 PASS:** repo byte-locked Quest P00 child + FOV110.
5. **Capture fallback:** v0.6.1 deterministic freeze/capture.
6. **Post-G3 culling only if visible:** `+0x3A16A6`, `+0x3A01E4`, `+0x86E9C0`, CSimpleWorldView.
7. **Post-G3 origin only if visible:** shared-CView mutation before NORMAL `GetMatricesForView`, through `DoRenderView`, exact restore; binary hook PENDING.

## CURRENT GATE
**T0 / G2.5 — GPU XR TRANSPORT CERTIFICATION.**

## NEXT
1. Preserve exact v0.5.5 BASE geometry/pass schedule and pass3 monitor.
2. Select/log verified-compatible HLAE protocol path, or minimal shared-target/fence transplant if direct protocol mode is incompatible.
3. Same-cycle pass1 LEFT + pass2 RIGHT = one `pairSeq`; GPU-complete before publication.
4. x64 host on LUID `00000000:000118D1`: import eye resource(s), acquire/wait/copy/release both XR swapchains, one `xrEndFrame` per pair; `mixedPairCount=0`.
5. T0 changes no projection/origin/head/culling/viewmodel/HUD.
6. If A is concretely blocked, activate prepared v0.7.1 fallback without repeating its research.
7. T0 PASS -> freeze transport -> run G3/T1 P00 + FOV110.

## METHOD ALREADY USED
source/donor harvest; retail CView canaries; D3D9 constant provenance; HLAE seam/pass archaeology; full-0x198 GOLDEN; projection/off-center tests; deterministic capture; Quest OpenXR FOV; TTD frustum/cache/reader recovery; x32dbg origin/yaw controls; CSimpleWorldView funnel; source-integrity CI; pinned AfxInterop/d3d9Hooks review; D3D9 CPU readback/MSAA fallback work; v0.7.1 pair/ABI hardening; Valve SourceVR scale cross-check; concurrent GitHub reconciliation.

## EXISTING REPO ARTIFACTS
- `runtime/golden_v0.5.5/`
- `runtime/children/v0.6.1_projection_oracle/`
- `runtime/children/v0.7.1_xr_transport/README.md`
- `experiments/quest_projection_child/`
- `tools/verify_quest_projection_child.py`
- `.github/workflows/evidence-integrity.yml`
- `research/RESULT_INDEX.md`
- `research/BRANCH_SYNC_20260824.md`
- `research/TRANSPORT_SYNC_20260824.md`
- `research/TRANSPORT_RECONCILIATION_20260824.md`
- `research/HLAE_INTEROP_FINDINGS.md`

## UNRESOLVED
- T0 end-to-end GPU transport verdict
- exact compatible AfxInterop protocol mode vs minimal transplant
- whether prepared CPU fallback B is needed
- atomic pairSeq / no L(N)+R(N+1) mixing
- G3/T1 headset fusion after T0
- same-frame/repeated-eye capture certification
- culling semantic destination only if visible later
- final head pose/eye-origin only if visible later
- weapon/hands/HUD/regression/release later
