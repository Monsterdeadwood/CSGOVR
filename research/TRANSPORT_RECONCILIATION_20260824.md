# CSGOVR — BRANCH SYNC 2026-08-24

Purpose: preserve the late cross-branch reconciliation so a fresh branch can recover the current project state without reading old chats.

## Remote-first provenance
During this sync, `main` advanced while the handoff was being prepared:
- earlier observed HEAD: `20f76e6bbd849b5aff5f25d00ae63505ef1b96c0`
- concurrent branch pushed 8 commits
- new observed HEAD: `fb2adabdc2e735526ca4281c94848ed106f8810d`
- those commits added/updated README, STATE, PROVEN, FALSIFIED, GATES, NEXT, EXPERIMENTS and other handoff material

The newer remote evidence was imported before this branch wrote anything. No concurrent result is intentionally discarded.

## Evidence imported from concurrent remote branch
### GOLDEN / stock causal path
- original v0.5.5 BMPs show stable depth-dependent horizontal L/R parallax with negligible vertical disparity
- `CSGOVR_CORRECTNESS_RESULT_20260821_011601.zip` / WORLD_X_PAIR proves a small RenderView-origin change can alter GPU `c2`, pixels and draw membership
- therefore the separate late DUAL_ORIGIN failure is only a negative result for that exact late intervention point, not a general falsification of eye translation

### Additional frozen CView / shader details
- `+0xC0 origin.z` proven subfield
- `+0xC8 yaw` proven subfield
- source-layout candidates only: `+0x34 m_bCustomViewMatrix`, `+0x38..+0x67 m_matCustomViewMatrix`, `+0xAC m_pCSMVolumeCuller`
- `CViewSetup.origin -> VS c2 / CAMERA_POS`
- projection families: `c4..c7 MODELVIEWPROJ`, `c8..c11 VIEWPROJ`
- retained branch constant `c17`
- bounded model candidate family `c58..c60`

### Newer prepared G3 diagnostic package
`CSGOVR_G3_QUEST_COHERENCE_BATCH_v1.0.1_SYNCED_ONE_CLICK.zip`
- SHA256 `595DA174AC39BA334F725DC1D130089F7F7C98D2BC4878F87B12F113BC0EAB70`
- status UNRUN / NOT BASELINE
- phase 2 Quest-matrix-only is the only PRIMARY-useful G3 phase
- FOV110/aspect/FOV150 phases are secondary culling diagnostics and must not redefine projection PASS/FAIL

## Evidence imported from this late branch
### Pinned HLAE interop capability — COLD exact-source proof
At exact AdvancedFX commit `b97636852b8eecae09285b5a386192bb285638eb`, the frozen HLAE lineage already contains:
- `AfxInterop::OnRenderView`
- `AfxInterop::OnRenderViewEnd`
- replacement render-target push via `AfxD3d9PushRenderTargetEx`
- drawing protocol `PrepareDrawReply_Skip/Retry/Continue`
- `WaitForGpu -> AfxD3D_WaitForGPU()`

This is capability proof from exact pinned source, not yet an end-to-end CSGOVR runtime PASS.

### Prepared CPU transport package reviewed
`CSGOVR_XR_TRANSPORT_v0.7.0_BUILD_FIRST_ONECLICK.zip`
- status PREPARED/UNRUN
- architecture uses D3D9 `GetRenderTargetData` -> SYSTEMMEM / CPU copy / shared memory -> x64 D3D11/OpenXR
- it is not runtime-falsified
- it is COLD-SUPERSEDED AS PRIMARY because the exact pinned HLAE already exposes a cleaner GPU replacement-target path and GPU completion primitive
- retain only as fallback/reference if GPU interop is concretely blocked

### Source-native culling policy
Valve Source VR derives scalar FOV from the widest edge after installing the asymmetric projection. Retained Quest edge angles have max half-angle 55 degrees, yielding scalar FOV `110°`.

Label: SOURCE-NATIVE integration policy / derived from proven Quest FOV. Do not silently call it frozen-retail culling ownership proof.

### G3 aspect diagnostic classification
Projection-derived aspect candidate `0.9254939479` and FOV150 control remain diagnostics only. Exact Quest P00 remains the primary projection candidate.

## Scheduling conflict and resolution
Concurrent remote branch state:
- CURRENT GATE = G3 Quest asymmetric projection integration
- final two-eye OpenXR transport/submission explicitly remained unproven
- if headset was black/absent, transport would then be promoted as blocker

Late branch state:
- canonical gate order places XR TRANSPORT before CORRECT PROJECTION
- no accepted frozen-CS:GO end-to-end result proves one coherent HLAE L/R pair reaches Meta OpenXR
- serializer rule says do not combine a new transport path and new projection in the same first mutation if they can be isolated
- exact pinned HLAE provides a GPU transport path that can be tested with BASE geometry first

Resolution:
- G3 projection evidence and prepared child remain valid; nothing is falsified or deleted
- insert **T0 / G2.5 GPU XR transport certification** before G3
- after T0 PASS, immediately execute G3/T1 exact Quest P00 + FOV110
- if a branch later supplies an accepted end-to-end coherent transport RESULT, T0 becomes PASS and G3 resumes automatically

## CURRENT GATE
**T0 / G2.5 — GPU XR TRANSPORT CERTIFICATION.**

Primary missing fact:
Can exact v0.5.5 pass1 LEFT + pass2 RIGHT be published as one coherent GPU pair to a separate x64 Meta OpenXR host, with BASE projection/origin/camera unchanged?

## NEXT
1. Preserve exact v0.5.5 scheduler and geometry.
2. Use pinned HLAE `afx_interop` replacement eye targets.
3. Assign one `pairSeq` across pass1 LEFT and pass2 RIGHT from the same HLAE cycle.
4. Wait for GPU completion before publication.
5. External x64 host creates D3D11 on runtime adapter LUID `00000000:000118D1`, opens/imports eye resource(s), acquire/wait/copy/release both XR swapchains, one `xrEndFrame` per pair.
6. No Quest P00/new origin/head/culling/viewmodel/HUD mutation in T0.
7. T0 PASS -> freeze transport -> run queued G3/T1 exact P00 + Source-native scalar FOV110.

## Current finalists
1. T0 pinned HLAE GPU replacement-target transport -> x64 Meta OpenXR host.
2. G3/T1 repository byte-locked minimal Quest projection child after T0 PASS.
3. G3 v1.0.1 synced package matrix-only phase as diagnostic support, not baseline.
4. Only after a visible post-G3 culling blocker: `+0x3A16A6`, `+0x3A01E4`, `+0x86E9C0`, CSimpleWorldView lineage.

## Do-not-repeat additions
- do not run CPU-readback XR_TRANSPORT v0.7.0 as the default architecture
- do not combine transport + projection + origin + head pose in the first transport proof
- do not interpret prepared package existence as RESULT
- do not reopen sign/transpose projection roulette
- do not use aspect `0.9254939479` or FOV150 as production fix without a concrete post-P00 clipping result

## Important local/runtime paths and versions
- game dir retained by concurrent branch: `C:\Program Files (x86)\Steam\steamapps\common\csgo legacy`
- shared research tools: `C:\GOG Games\VR Research Tools`
- AppID `4465480`, BuildID `22100934`
- exact client SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- HLAE 2.191.1 / AdvancedFX `b97636852b8eecae09285b5a386192bb285638eb`
- AfxHookSource.dll SHA256 `5B9D13AE9ABFFEEF80851E10861302162A47D1B43D8483B4A8FCEC7EAA19B035`
- Meta/Oculus OpenXR 1.205.0, Quest 3
- `-insecure` mandatory

## Provenance pointers
- `docs/PROVEN.md` — accepted facts from concurrent sync
- `docs/FALSIFIED.md` — closed paths from concurrent sync
- `docs/EXPERIMENTS.md` — experiment/tool index
- `research/RESULT_INDEX.md` — retained result ledger
- this file — late scheduling + transport reconciliation
