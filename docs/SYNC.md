# CSGOVR — STATE SYNC

Snapshot: 2026-08-24 final reconciliation.

## REMOTE-FIRST CHECK
- repo `Monsterdeadwood/CSGOVR`
- authoritative integration branch `main`
- remote moved concurrently multiple times during sync; each newer main state was read before final reconciliation
- projection-sync provenance: `research/BRANCH_SYNC_20260824.md`
- transport-fallback provenance: `research/TRANSPORT_SYNC_20260824.md`

## LATEST RESULT
Latest accepted scientific result overall:
`CSGOVR_TTD_FRUSTUM_ARG2_PERSISTENCE_RESULT_20260823_164733.zip`
- first helper arg2 0x40 temporary transpose/restore PROVEN
- persistent tail `+0x40..+0x1FF` FALSIFIED

Current integration evidence:
- v0.5.5 stereo GOLDEN PROVEN
- custom projection semantics PROVEN
- Quest/OpenXR FOV/views PROVEN
- no accepted end-to-end two-eye CSGOVR OpenXR transport RESULT

Prepared/unrun:
- `CSGOVR_G3_QUEST_COHERENCE_BATCH_v1.0.1_SYNCED_ONE_CLICK.zip` SHA256 `595DA174AC39BA334F725DC1D130089F7F7C98D2BC4878F87B12F113BC0EAB70`
- repository `experiments/quest_projection_child/`
- `CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF` SHA256 `DFEA881888C83812A5B9EE3A7049F79D32935240F2C612DE4ABDF4F9A98834A3`, CPU fallback, not LIVE-proven
- v0.7.0 transport is superseded by v0.7.1 inside the CPU lineage

## PROVEN
- frozen client SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- v0.5.5 pass1 LEFT / pass2 RIGHT / pass3 monitor
- full CViewSetup `0x198`
- HLAE DoRenderView `+0x22340`, copy ctor `+0x14BA0`
- HLAE object `+0x30/+0x50/+0x54`; observed count pattern 1/2/3
- custom projection `+0x68/+0x6C..+0xAB`; M00/M02/M11/M12 causal semantics
- Quest FOV, two views, 1504x1616 recommended size, OpenXR adapter LUID `00000000:000118D1`
- actual v0.5.5 BMPs show depth-dependent horizontal L/R parallax
- `CViewSetup.origin -> VS c2`; WORLD_X_PAIR changes GPU/raster/draw membership
- passive D3D9 Present device latch + CPU readback/MSAA-resolve primitive works in frozen retail
- pre-eye reader `client+0x3A16A6` evidence packet
- temporary arg2 0x40 transpose/restore
- CSimpleWorldView main-world lineage

## COLD TRANSPORT CAPABILITY IMPORT
Exact pinned AdvancedFX `b97636852b8eecae09285b5a386192bb285638eb` contains:
- AfxInterop OnRenderView/OnRenderViewEnd
- replacement RT through `AfxD3d9PushRenderTargetEx`
- PrepareDrawReply Skip/Retry/Continue
- WaitForGpu -> `AfxD3D_WaitForGPU()`

Label: exact-source capability PROVEN; end-to-end CSGOVR transport runtime still OPEN.

## EVIDENCE CONFLICT / RECONCILIATION
Camera/origin:
- late DUAL_ORIGIN intervention failed to authoritatively alter that final capture
- GOLDEN images + WORLD_X_PAIR independently prove eye-origin-related geometry/raster behavior exists
Resolution: negative result applies only to that exact late intervention point.

Gate order:
- projection branch preserved a valid G3 child but also recorded final transport as unproven
- transport branches independently converged on transport certification first
Resolution: **T0/G2.5 XR TRANSPORT before G3; G3 queued, not falsified.**

Transport implementation:
- remote finalist A: pinned HLAE GPU interop
- transport branch finalist B: prepared v0.7.1 CPU readback/shared-pair fallback
Resolution: A stays PRIMARY because it is the stronger current exact-source path; B is retained, not discarded or runtime-falsified, and activates only on a concrete A blocker or bounded compatibility control.

## FALSIFIED / CLOSED
- full CViewSetup 0x14C
- raw Khronos column-major memcpy into Source VMatrix
- `+0x0525BA60` both-eye reuse
- persistent arg2 tail
- `+0x38E5D7/+0x38EC08/+0x3A0381` static consumers
- skybox/shadow-depth classes as main-world owner
- origin/yaw byte-integrity wrappers as ownership proof
- repeated projection sign/transpose roulette
- broad OpenXR rediscovery
- sequential screenshots as quantitative stereo/culling truth
- broad origin batch before a concrete later blocker
- Present as eye oracle
- v0.7.0 prepared CPU transport, superseded by v0.7.1
- CPU-readback transport as T0 PRIMARY while GPU interop remains unblocked; not runtime-falsified as fallback

## CURRENT FINALISTS
1. **T0 A / PRIMARY:** pinned HLAE GPU replacement-eye-target transport -> GPU completion -> x64 D3D11/Meta OpenXR.
2. **T0 B / PREPARED FALLBACK:** `CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF`; same-HLAE structural pair proof + CPU readback/shared ring.
3. **G3/T1 after T0 PASS:** repo byte-locked minimal Quest projection child.
4. G3 v1.0.1 matrix-only phase as diagnostic support; aspect/FOV phases secondary only.
5. Post-G3 visible culling only: `+0x3A16A6`, `+0x3A01E4`, `+0x86E9C0`, CSimpleWorldView.

## v0.7.1 FALLBACK WORKING FIXES
- same `CAfxStreams*` for both eyes
- LEFT `m_DoRenderViewCount 0->1`, RIGHT `1->2`
- complete pair only READY
- producer token `DISTINCT PAIR PROOF`
- host token `PAIR PROOF PASS`
- fixed-width `pack(4)` ABI
- Reset generation invalidation
- RGB-only hash for X8R8G8B8
- live v0.5.5 camera input, not old projection-oracle freeze
- stale-control cleanup / `$PID` runner fix
- exact same-pair BMP harvest

## CONCRETE TARGETS
- RenderView `client+0x3A5280`
- HLAE DoRenderView `+0x22340`, copy ctor `+0x14BA0`
- CView custom projection `+0x68/+0x6C`, world FOV `+0xB0`, origin `+0xB8`, angles `+0xC4`, near/far `+0xD0/+0xD4`, aspect `+0xE0`
- off-center `+0x138/+0x13C/+0x140/+0x144`, flag `+0x148 bit0`
- D3D9 Present vtable slot 17 = passive device latch in fallback; Reset slot 16 = generation/resource invalidation
- readers `+0x3A16A6`, `+0x3A01E4`; GeneratePerspectiveFrustum `+0x86E9C0`, writers `+0x86EB3C`
- CSimpleWorldView vtable `+0xBF4434`, Draw `+0x3B0870`, fields `+0x168/+0x16C/+0x170`
- infrastructure only: `+0x38D123`, `+0x52632CC`, `+0x5263384`, yaw `+0x38D15B/+0x38D1DE/+0x5245AB8`

## CURRENT GATE
**T0 / G2.5 — XR TRANSPORT CERTIFICATION.**

## NEXT
Follow the current `docs/NEXT.md`: keep v0.5.5 geometry BASE and run pinned-HLAE GPU transport FINALIST A. T0 PASS -> freeze transport -> immediately run queued G3/T1 exact P00 + FOV110. If A reaches a concrete interop blocker, activate prepared v0.7.1 FINALIST B without rebuilding or rediscovering it.

## METHOD ALREADY USED
source/donor harvest; retail canaries; D3D9 constant provenance; HLAE seam/pass archaeology; v0.5.5 full-copy GOLDEN; custom projection/off-center causal tests; deterministic capture; Quest OpenXR FOV acquisition; TTD cache/frustum/reader recovery; arg2 persistence; x32dbg origin/yaw infrastructure; CSimpleWorldView class funnel; source-integrity verifier/CI; HLAE GPU-interoperability source review; D3D9 CPU readback/MSAA fallback work; transport donor comparison; concurrent GitHub cross-branch reconciliation.
