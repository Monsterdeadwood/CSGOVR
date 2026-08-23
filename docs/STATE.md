# CSGOVR State

Snapshot: 2026-08-24 final cross-branch reconciliation after remote race.

## STATUS
ACTIVE.

## AUTHORITY / REMOTE SYNC
Canonical evidence base remains `CSGOVR_KNOWLEDGE_BASE_CANONICAL_20260823.md`, augmented by the 2026-08-24 cross-branch sync.

During this sync, `main` advanced from `20f76e6bbd849b5aff5f25d00ae63505ef1b96c0` to `fb2adabdc2e735526ca4281c94848ed106f8810d` via 8 concurrent commits. Those commits were read and their useful evidence was imported before this handoff was committed. See `research/BRANCH_SYNC_20260824.md`.

## FROZEN TARGET / DEPENDENCIES
- legacy CS:GO x86, Steam AppID `4465480`, BuildID `22100934`
- game dir: `C:\Program Files (x86)\Steam\steamapps\common\csgo legacy`
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- `RenderView = client.dll+0x003A5280`
- mandatory `-insecure`
- HLAE `2.191.1`; AdvancedFX source pin `b97636852b8eecae09285b5a386192bb285638eb`
- HLAE package/exe SHA256 `D7BA05E1E540D82AC485A9B39F13D5C4CCF9FD94F460E0C9ECF069B94C880A59`
- `AfxHookSource.dll` SHA256 `5B9D13AE9ABFFEEF80851E10861302162A47D1B43D8483B4A8FCEC7EAA19B035`
- Meta/Oculus OpenXR `1.205.0`, Quest 3
- shared tools root: `C:\GOG Games\VR Research Tools`

## LATEST RESULT
Latest accepted scientific result overall:
`CSGOVR_TTD_FRUSTUM_ARG2_PERSISTENCE_RESULT_20260823_164733.zip`
- PROVEN: first helper arg2 `0x40` bytes undergo temporary in-place transpose and exact restore.
- FALSIFIED: persistent output in arg2 `+0x40..+0x1FF`.
- STOP: persistent-arg2-output theory.

Current-gate supporting accepted results:
- `CSGOVR_HLAE_EYE_PATCH_RESULT_20260821_015737.zip` — GOLDEN stereo path.
- `CSGOVR_CUSTOM_PROJECTION_MATRIX_RESULT_20260821_042218.zip` — custom projection semantics PROVEN.
- `CSGOVR_OPENXR_FOV_DIAGNOSTIC_20260821_150332.zip` — Quest/OpenXR input PROVEN.

Prepared, NOT accepted RESULT:
- `CSGOVR_G3_QUEST_COHERENCE_BATCH_v1.0.1_SYNCED_ONE_CLICK.zip`, SHA256 `595DA174AC39BA334F725DC1D130089F7F7C98D2BC4878F87B12F113BC0EAB70` — UNRUN; matrix-only phase is G3-primary-useful, FOV/aspect phases diagnostic only.
- `CSGOVR_XR_TRANSPORT_v0.7.0_BUILD_FIRST_ONECLICK.zip` — UNRUN; CPU readback/shared-memory transport; COLD-SUPERSEDED AS PRIMARY, not runtime-falsified.
- `experiments/quest_projection_child/` — prepared byte-locked projection implementation; not runtime-PROVEN.

## PROVEN
### Stereo GOLDEN
- `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`
- pass1 LEFT world, pass2 RIGHT world, pass3 monitor
- exact full `CViewSetup=0x198`; `0x14C` full-size claim SUPERSEDED
- HLAE `CAfxStreams::DoRenderView = AfxHookSource.dll+0x00022340`
- HLAE `CViewSetup_csgo` copy ctor `+0x00014BA0`, copies through byte `0x197`
- HLAE controls: `this+0x30 m_ForceCacheFullSceneState`, `this+0x50 m_DoRenderViewCount`, `this+0x54 m_ShowRenderViewCount`
- nominal v0.5.5 half-eye offset `1.2598425 Source units`
- original v0.5.5 BMPs show stable depth-dependent horizontal L/R disparity with negligible vertical disparity

### Retail CViewSetup map
- `+0x68 m_bCustomProjMatrix`
- `+0x6C..+0xAB m_matCustomProjMatrix`
- `+0xB0 world FOV`
- `+0xB4 viewmodel FOV`
- `+0xB8 origin`, `+0xC0 origin.z`
- `+0xC4 angles`, `+0xC8 yaw`
- `+0xD0/+0xD4 world zNear/zFar`
- `+0xD8/+0xDC viewmodel zNear/zFar`
- `+0xE0 aspect`
- `+0xE4..+0xF8` observed DoF defaults
- `+0x100` motion blur mode, observed default 2
- off-center `+0x138 top`, `+0x13C bottom`, `+0x140 left`, `+0x144 right`, `+0x148 bit0`
Strong source-layout candidates only: `+0x34 m_bCustomViewMatrix`, `+0x38..+0x67 m_matCustomViewMatrix`, `+0xAC m_pCSMVolumeCuller`.

### CPU -> GPU / shader provenance
- `CViewSetup.origin -> VS c2 / CAMERA_POS`
- projection families `c4..c7 MODELVIEWPROJ`, `c8..c11 VIEWPROJ`
- retained branch constant `c17`
- bounded model candidate family `c58..c60`
- `WORLD_X_PAIR` from `CSGOVR_CORRECTNESS_RESULT_20260821_011601.zip` proves small RenderView-origin translation can change GPU c2, final pixels and draw membership

### Projection / Quest
- custom projection `+0x68/+0x6C..+0xAB` PROVEN
- M00/M02/M11/M12 semantics causally measured
- Source/D3D depth convention retained: `M22=zFar/(zNear-zFar)`, `M23=zNear*zFar/(zNear-zFar)`, `M32=-1`, `M33=0`
- Quest views: count=2, `viewStateFlags=0xF`, recommended `1504x1616`
- OpenXR adapter LUID `00000000:000118D1`, minimum FL `0xB000`, created `0xB100`
- LEFT FOV L=-54°, R=+40°, U=+44°, D=-55°
- RIGHT FOV L=-40°, R=+54°, U=+44°, D=-55°
- passive eye separation ~`64.780 mm` (harvest, not final engine eye-origin policy)
- P00 LEFT: M00 `0.9027383310563113`, M02 `-0.24251263135515727`, M11 `0.8354788620481540`, M12 `-0.19318743951911962`
- P00 RIGHT: same except M02 `+0.24251263135515727`

### Exact pinned HLAE GPU interop capability — COLD/source proof
At exact `b976368...` the frozen HLAE lineage already contains:
- `AfxInterop::OnRenderView` / `OnRenderViewEnd`
- replacement render target via `AfxD3d9PushRenderTargetEx`
- `PrepareDrawReply_Skip/Retry/Continue`
- `WaitForGpu -> AfxD3D_WaitForGPU()`
This proves capability exists without upgrading HLAE. End-to-end CSGOVR transport is still runtime-UNPROVEN.

### Culling / world-view lineage
- `client+0x003A16A6..+0x003A16DC` exact pre-eye reader/dataflow site with recovered call stack and bounded dataflow; destination semantics PENDING
- second reader `client+0x003A01E4..+0x003A021A`
- `GeneratePerspectiveFrustum = client+0x0086E9C0`; writer family starts `+0x0086EB3C`
- CSimpleWorldView main-world lineage PROVEN/FINALIST: vtable `client+0x00BF4434`, Draw `client+0x003B0870`
- retained candidates: `+0x168 IWorldRenderList*`, `+0x16C CClientRenderablesList*`, `+0x170 ClientWorldListInfo_t*`
- hydration: `+0x3B0067/+0x3B0083/+0x3B008F/+0x3B00A3/+0x3B00BF/+0x3B00CB`; post `+0x3B00D9`; consumers `+0x3B010D/+0x3B01FD/+0x3B0225/+0x3B023D`
- current slot `+0x05248ED4`; arrays `+0x0524A2F4/+0x0524A2F8/+0x0524A2FC`, stride `0x13F0`

## EVIDENCE RECONCILIATION
Keep both paths:
1. `CSGOVR_NATIVE_STEREO_SEAM_RESULT_20260821_041928`: one tested late DUAL_ORIGIN intervention did not authoritatively change its final capture; late DUAL_OFFCENTER did.
2. v0.5.5 GOLDEN + WORLD_X_PAIR: actual GOLDEN has depth-dependent stereo parallax and RenderView-origin translation can affect GPU/raster.

Conclusion: only that exact late intervention point is negative. Do not generalize it into “eye translation does not work.”

## FALSIFIED / CLOSED / STOPPED
- `0x14C` as full CViewSetup
- raw Khronos column-major memcpy into Source VMatrix
- `client+0x0525BA60` reused inside BOTH eye marker regions
- persistent helper arg2 output beyond first `0x40`
- earlier parser falsification of first-0x40 transpose — INVALID/SUPERSEDED
- static post-SetUpView candidates `+0x38E5D7/+0x38EC08/+0x3A0381`
- CSkyboxView (`+BF451C/+3AEA90`) as main-world owner
- CShadowDepthView (`+BF44F4/+3AEAE0`) as main-world owner
- origin/yaw byte-integrity wrappers as camera ownership proof
- broad OpenXR rediscovery while baseline remains valid
- repeated sign/transpose projection roulette
- sequential screenshots as quantitative stereo/culling truth
- broad 8–12 point eye-origin batch until a concrete post-G3 defect exists
- CPU-readback `XR_TRANSPORT_v0.7.0` as PRIMARY transport architecture: COLD-SUPERSEDED, not runtime-falsified

## RETAINED INFRASTRUCTURE ONLY
- origin seam `client+0x0038D123`
- global CView candidate `+0x052632CC`
- global origin candidate `+0x05263384`
- yaw seam `+0x0038D15B`, global commit `+0x0038D1DE`, angles.y candidate `+0x05245AB8`
Do not promote to ownership without new causal evidence.

## CURRENT FINALISTS
1. **T0 / G2.5:** pinned HLAE `afx_interop` GPU replacement eye targets -> GPU completion -> external x64 D3D11/Meta OpenXR host.
2. **G3/T1 after T0 PASS:** repo byte-locked minimal Quest projection child, exact P00 + Source-native scalar FOV `110°`, engine zNear/zFar retained.
3. **Conditional only after visible post-P00 clipping:** aspect `0.9254939479` and/or FOV150 as diagnostic controls, not production fixes.
4. **Only if a concrete culling blocker survives:** `+0x3A16A6`, `+0x3A01E4`, `+0x86E9C0`, CSimpleWorldView lineage.

## CURRENT GATE
**T0 / G2.5 — GPU XR TRANSPORT CERTIFICATION.**

Remote concurrent branch had G3 current and explicitly noted final XR transport was still unproven. This sync inserts a transport-only certification before G3 to serialize mutations; G3 is queued, not falsified.

## NEXT
1. Preserve exact v0.5.5 geometry/pass schedule and pass3 monitor.
2. Use pinned HLAE interop replacement eye RT(s); pass1 LEFT + pass2 RIGHT from same HLAE cycle = one `pairSeq`.
3. GPU-complete before publication.
4. External x64 host uses D3D11 on OpenXR adapter LUID, imports eye resources, acquire/wait/copy/release both swapchains, one `xrEndFrame` per coherent pair.
5. T0 must not change Quest projection, origin, HMD pose, culling, viewmodel or HUD.
6. T0 PASS -> freeze transport -> immediately run G3/T1 exact Quest P00 + FOV110.
7. T0 FAIL -> fix only the concrete transport defect; do not widen search.

## METHOD ALREADY USED
- source/donor corpus harvest
- retail CViewSetup canaries
- D3D9 constant provenance
- HLAE exact seam/pass archaeology
- full-0x198 v0.5.5 eye-copy GOLDEN
- custom projection/off-center causal canaries
- deterministic frozen projection capture
- OpenXR Quest FOV acquisition / independent matrix derivation
- TTD frustum/cache replay and reader recovery
- arg2 persistence replay
- x32dbg origin/yaw infrastructure controls
- CRendering3dView class funnel
- exact source recovery / projection transplant / integrity verifier + CI
- prepared G3 and CPU transport package review without promotion to RESULT

## EXISTING REPO ARTIFACTS
- `runtime/golden_v0.5.5/src/eye_patch.c`, `injector.c`, `BUILD_REPRO.md`, `SOURCE_SHA256.txt`
- `runtime/children/v0.6.1_projection_oracle/`
- `experiments/quest_projection_child/PREFLIGHT.txt`
- `experiments/quest_projection_child/QUEST_PROJECTION_MINIMAL.patch`
- `experiments/quest_projection_child/eye_patch_quest_projection.c`
- `tools/verify_quest_projection_child.py`
- `.github/workflows/evidence-integrity.yml`
- `research/RESULT_INDEX.md`
- `research/BRANCH_SYNC_20260824.md`

## UNRESOLVED
- end-to-end frozen-CSGO HLAE GPU pair -> x64 Meta OpenXR runtime proof
- atomic pairSeq / no L(N)+R(N+1) mixing
- G3/T1 headset fusion after T0
- same-frame/repeated-eye quantitative capture certification
- culling semantic destination after `+0x3A16A6` only if visible later
- authoritative final head pose / eye-origin integration only if visible later
- weapon/hands/HUD/regression/release later
