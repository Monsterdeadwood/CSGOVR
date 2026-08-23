# CSGOVR State

## STATUS
ACTIVE.

## AUTHORITY / CROSS-BRANCH SYNC
Canonical source remains `CSGOVR_KNOWLEDGE_BASE_CANONICAL_20260823.md`, but this file incorporates the 2026-08-24 urgent cross-branch sync.

Remote-first check before this sync:
- repository: `Monsterdeadwood/CSGOVR`
- branch: `main`
- only visible branch: `main`
- remote HEAD before sync: `20f76e6bbd849b5aff5f25d00ae63505ef1b96c0` (`Add CSGOVR evidence integrity CI`)
- no other branch had closed G3
- remote already contained the exact v0.5.5 source, v0.6.1 projection donor, minimal Quest projection child, patch, verifier and CI

Do not silently overwrite conflicting evidence. See `## EVIDENCE RECONCILIATION` below.

## LATEST RESULT
Distinguish accepted runtime results from prepared-but-unrun builds:

### Latest accepted result relevant to current gate
`CSGOVR_CUSTOM_PROJECTION_MATRIX_RESULT_20260821_042218.zip`
- custom projection path PROVEN
- frozen camera / bots kicked
- M00/M02/M11/M12 semantics causally measured
- custom symmetric reconstruction matches native Source projection closely

### XR input result
`CSGOVR_OPENXR_FOV_DIAGNOSTIC_20260821_150332.zip`
- Quest 3 / Oculus OpenXR 1.205.0
- `xrLocateViews = XR_SUCCESS`, count=2, `viewStateFlags=0xF`
- FOV and eye separation captured

### Latest forensic result overall
`CSGOVR_TTD_FRUSTUM_ARG2_PERSISTENCE_RESULT_20260823_164733.zip`
- PROVEN: first helper-arg2 `0x40` bytes undergo temporary in-place transpose and restore
- FALSIFIED: persistent output beyond `+0x40..+0x1FF`

### Prepared, not a RESULT
`CSGOVR_G3_QUEST_COHERENCE_BATCH_v1.0.1_SYNCED_ONE_CLICK.zip`
- SHA256 `595DA174AC39BA334F725DC1D130089F7F7C98D2BC4878F87B12F113BC0EAB70`
- not run
- PRIMARY useful phase: Quest custom matrix only
- later FOV/aspect phases are secondary G4/culling harvest only
- not a baseline and must not supersede the repository's byte-locked minimal child

## PROVEN
### Frozen target
- standalone legacy CS:GO x86, Steam AppID `4465480`, BuildID `22100934`
- game directory: `C:\Program Files (x86)\Steam\steamapps\common\csgo legacy`
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- `RenderView = client.dll+0x003A5280`

### HLAE / stereo GOLDEN
- GOLDEN: `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`
- HLAE 2.191.1 package SHA256 `D7BA05E1E540D82AC485A9B39F13D5C4CCF9FD94F460E0C9ECF069B94C880A59`
- `AfxHookSource.dll` SHA256 `5B9D13AE9ABFFEEF80851E10861302162A47D1B43D8483B4A8FCEC7EAA19B035`
- `CAfxStreams::DoRenderView = AfxHookSource.dll+0x00022340`
- exact `CViewSetup_csgo` copy ctor `AfxHookSource.dll+0x00014BA0`
- copy extends through byte `0x197`; exact full object size `0x198`
- HLAE object offsets: `this+0x30 = m_ForceCacheFullSceneState`, `this+0x50 = m_DoRenderViewCount`
- pass1 LEFT world, pass2 RIGHT world, pass3 unmodified monitor
- v0.5.5 nominal half-eye offset `1.2598425 Source units`
- original v0.5.5 BMPs show real horizontal depth-dependent L/R disparity with negligible vertical disparity; this is evidence for genuine geometric stereo in the GOLDEN path

### Retail CViewSetup map
- `+0x68 m_bCustomProjMatrix`
- `+0x6C..+0xAB m_matCustomProjMatrix`
- `+0xB0 world FOV`
- `+0xB4 viewmodel FOV`
- `+0xB8 origin`
- `+0xC0 origin.z` proven subfield
- `+0xC4 angles`
- `+0xC8 yaw` proven subfield
- `+0xD0/+0xD4 world zNear/zFar`
- `+0xD8/+0xDC viewmodel zNear/zFar`
- `+0xE0 aspect`
- `+0xE4..+0xF8` observed DoF defaults
- `+0x100` motion blur mode, observed default 2
- off-center: `+0x138 top`, `+0x13C bottom`, `+0x140 left`, `+0x144 right`, `+0x148 bit0 m_bOffCenter`
- strong source-layout candidates only: `+0x34 m_bCustomViewMatrix`, `+0x38..+0x67 m_matCustomViewMatrix`, `+0xAC m_pCSMVolumeCuller`

### CPU -> GPU / shader provenance
- `CViewSetup.origin` reaches VS `c2 / CAMERA_POS`
- projection families include `c4..c7 MODELVIEWPROJ`, `c8..c11 VIEWPROJ`
- retained branch constant `c17`
- bounded `c58..c60` model candidate family
- `WORLD_X_PAIR` causal work proved a small RenderView origin translation can change GPU camera state, final pixels and draw membership in the stock causal oracle

### Projection
- custom projection path at `+0x68/+0x6C` is PROVEN
- M00/M02/M11/M12 semantics causally measured
- v0.6.1 deterministic projection oracle used `PoseFreeze=TRUE`, `BotsKicked=TRUE`
- at 960x540, canaries matched analytical response: `M02 +/-0.10 -> about -/+48 px X`; `M12 +/-0.10 -> about +/-27 px Y`; M00/M11 0.80 scale canaries matched expected axis scaling
- Quest 3 recorded FOV:
  - LEFT L=-54°, R=+40°, U=+44°, D=-55°
  - RIGHT L=-40°, R=+54°, U=+44°, D=-55°
- recommended eye size `1504x1616`
- passive XR eye separation about `64.780 mm`
- working projection coefficients:
  - LEFT `M00≈0.9027383, M02≈-0.2425126, M11≈0.8354789, M12≈-0.1931875`
  - RIGHT `M00≈0.9027383, M02≈+0.2425126, M11≈0.8354789, M12≈-0.1931875`
- Source/D3D Z convention retained: `M22=zFar/(zNear-zFar)`, `M23=zNear*zFar/(zNear-zFar)`, `M32=-1`, `M33=0`

### Culling / world-view lineage
- `client.dll+0x003A16A6..+0x003A16DC` is a concrete pre-eye frustum/cache reader/dataflow site with recovered call stack and bounded dataflow; destination semantics remain PENDING
- second retained reader `client.dll+0x003A01E4..+0x003A021A`
- `GeneratePerspectiveFrustum = client.dll+0x0086E9C0`; lane writers start around `+0x0086EB3C`
- first helper arg2 `0x40` bytes transpose in place temporarily and are restored
- CSimpleWorldView is the proven main-world finalist:
  - vtable `client+0x00BF4434`
  - Draw `client+0x003B0870`
  - retained fields: `+0x168 IWorldRenderList*`, `+0x16C CClientRenderablesList*`, `+0x170 ClientWorldListInfo_t*`
  - lazy hydration: `client+0x003B0067` checks `+0x168`; `client+0x003B0083` loads world-list pointer from per-slot array
- slot/global helpers retained: current split-screen/view slot `client+0x05248ED4`; arrays `+0x0524A2F4/+0x0524A2F8/+0x0524A2FC`, stride `0x13F0`

## EVIDENCE RECONCILIATION — DO NOT COLLAPSE THESE INTO ONE CLAIM
Two evidence paths coexist:

1. `CSGOVR_NATIVE_STEREO_SEAM_RESULT_20260821_041928`:
   - one tested late DUAL_ORIGIN intervention did not authoritatively alter that final captured raster
   - late DUAL_OFFCENTER did alter output
   - this remains a valid negative result for that exact intervention/capture point

2. GOLDEN/stock causal evidence:
   - original `CSGOVR_HLAE_EYE_PATCH_RESULT_20260821_015737.zip` BMPs show stable depth-dependent L/R parallax in the actual v0.5.5 path
   - `CSGOVR_CORRECTNESS_RESULT_20260821_011601.zip` / WORLD_X_PAIR proved RenderView-origin translation can affect GPU `c2`, pixels and draw membership

Conclusion: the late intervention is **not** a general falsification of eye translation. The old broad TAB3 plan to try 8–12 origin write points is STOPPED until G3 reveals a real headset-visible eye-origin defect.

## FALSIFIED / CLOSED
- full `CViewSetup=0x14C` — SUPERSEDED
- `client.dll+0x0525BA60` reused inside BOTH LEFT and RIGHT eye-marker regions — FALSIFIED; watched events were outside eye markers
- persistent helper arg2 output beyond first 0x40 bytes — FALSIFIED
- earlier parser claim falsifying the first-0x40 transpose — INVALID/SUPERSEDED
- originX/yaw byte-integrity wrappers as final camera ownership proof — rejected; INFRASTRUCTURE only
- broad OpenXR rediscovery while current XR baseline remains valid — CLOSED
- sequential screenshots as quantitative stereo/culling truth — CLOSED; temporal drift can dominate
- cross-project contaminated FULL_EXPERIMENTAL failure — INVALID renderer evidence
- repeated sign/transpose projection roulette — CLOSED because v0.6.1 already causally fixed storage/sign semantics
- old broad 8–12 candidate eye-origin batch — STOP until a concrete post-G3 blocker exists

## RETAINED INFRASTRUCTURE / CONCRETE TARGETS
Do not promote without new causal evidence:
- origin.x writer/mutation seam `client+0x0038D123`
- `CViewSetup.origin +0xB8`
- next true RenderView `client+0x003A5280`
- persistent/global CView candidate `client+0x052632CC`
- origin-global candidate `client+0x05263384`
- yaw seam `client+0x0038D15B`
- global yaw commit `client+0x0038D1DE`
- `g_vecRenderAngles.y` candidate `client+0x05245AB8`

## CURRENT FINALISTS
1. **G3 primary:** repository byte-locked minimal Quest projection child `experiments/quest_projection_child/eye_patch_quest_projection.c` plus `QUEST_PROJECTION_MINIMAL.patch`, parented from exact v0.5.5; verify with `tools/verify_quest_projection_child.py`.
2. **G3 diagnostic support:** `CSGOVR_G3_QUEST_COHERENCE_BATCH_v1.0.1_SYNCED_ONE_CLICK.zip`, unrun, matrix-only phase is the primary useful measurement; later FOV/aspect phases are secondary G4 harvest.
3. **G4 only if visible culling blocker remains:** `+0x003A16A6` / `+0x003A01E4` / `+0x0086E9C0` / CSimpleWorldView lineage.

## CURRENT GATE
**G3 — Quest asymmetric projection integration.**

The repo already contains the minimal child. The missing fact is a runtime/headset verdict, not another projection or camera reverse pass.

## NEXT
1. Run the byte-locked minimal Quest projection child, preserving v0.5.5 LEFT/RIGHT origin behavior and pass3 monitor.
2. Verify exact hashes and run `tools/verify_quest_projection_child.py` before LIVE.
3. Collect eye/pass IDs, applied matrix values, pass counts, same-frame/deterministic capture where available, headset fusion/depth verdict and regression status.
4. PRIMARY PASS requires the Quest asymmetric matrix to apply to the correct eye while stereo GOLDEN and monitor pass remain intact.
5. If G3 passes, freeze projection integration and define the next gate only from the visible remaining blocker.
6. If a concrete culling blocker survives, use existing G4 targets; do not restart broad archaeology.
7. If headset transport itself is still absent/black, treat transport as the concrete blocker and solve swapchain/copy/submission without reopening projection math.

## METHOD ALREADY USED
- source/donor super-harvest across Source-family VR projects
- exact retail CViewSetup canaries
- CPU->GPU provenance / D3D9 constant census
- HLAE seam archaeology and pass partition
- v0.5.5 local full-0x198 per-eye copies
- custom projection and off-center causal canaries
- deterministic/frozen projection capture
- OpenXR Quest FOV acquisition and independent matrix derivation
- TTD frustum/cache replay and targeted reader dataflow recovery
- x32dbg / causal origin/yaw infrastructure mutation windows
- world-view class funnel to CSimpleWorldView
- cross-branch source-integrity verification and GitHub CI

Do not build another analyzer in an already-closed class unless new direct contradictory evidence appears.

## IMPORTANT PATHS / TOOLS
- shared research tools: `C:\GOG Games\VR Research Tools`
- runtime GOLDEN source: `runtime/golden_v0.5.5/src/`
- projection donor/transplant: `runtime/children/v0.6.1_projection_oracle/PROJECTION_TRANSPLANT.md`
- minimal G3 child: `experiments/quest_projection_child/eye_patch_quest_projection.c`
- auditable diff: `experiments/quest_projection_child/QUEST_PROJECTION_MINIMAL.patch`
- preflight: `experiments/quest_projection_child/` files + `experiments/NEXT_EXPERIMENT.md`
- integrity verifier: `tools/verify_quest_projection_child.py`
- CI: `.github/workflows/evidence-integrity.yml`
- result ledger: `research/RESULT_INDEX.md`
- urgent sync provenance: `research/BRANCH_SYNC_20260824.md`

## UNRESOLVED
- G3 has no accepted in-headset/same-frame runtime RESULT yet
- final OpenXR swapchain/submission status for CSGOVR is not proven by current accepted results
- if G3 reveals culling defects: exact semantic destination/ownership after `+0x003A16A6` remains open
- authoritative final head-pose integration remains later
- weapon/hands/HUD remain later gates and must not be coupled into G3
