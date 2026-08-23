# CSGOVR — URGENT BRANCH SYNC 2026-08-24

Purpose: preserve the useful state of the expiring ChatGPT branch after a remote-first CROSS-BRANCH STATE SYNC. This is provenance, not a chat transcript.

## REMOTE STATE BEFORE WRITE
- repo `Monsterdeadwood/CSGOVR`
- branch `main`
- only visible branch `main`
- remote HEAD before this sync: `20f76e6bbd849b5aff5f25d00ae63505ef1b96c0`
- remote already contained exact GOLDEN source, v0.6.1 donor/transplant, the minimal Quest projection patch/source, integrity verifier and CI
- no available branch/result had closed G3

## LATEST RESULT
Current-gate accepted projection result:
`CSGOVR_CUSTOM_PROJECTION_MATRIX_RESULT_20260821_042218.zip`

XR input:
`CSGOVR_OPENXR_FOV_DIAGNOSTIC_20260821_150332.zip`

Latest forensic result overall:
`CSGOVR_TTD_FRUSTUM_ARG2_PERSISTENCE_RESULT_20260823_164733.zip`

No accepted runtime/headset RESULT yet closes G3.

## PROVEN imported by this branch
- GOLDEN v0.5.5, pass1 LEFT / pass2 RIGHT / pass3 monitor, full `CViewSetup=0x198`
- HLAE `DoRenderView +0x22340`, copy ctor `+0x14BA0`, object controls `+0x30/+0x50`
- custom projection bool/matrix `+0x68/+0x6C..+0xAB`
- M00/M02/M11/M12 causal semantics from deterministic v0.6.1
- off-center `+0x138/+0x13C/+0x140/+0x144`, flag `+0x148 bit0`
- Quest 3 FOV, coefficients, recommended 1504x1616, eye separation ~64.780 mm
- CPU origin -> GPU `c2/CAMERA_POS`; WORLD_X_PAIR translation can affect pixels/draw membership
- original v0.5.5 BMPs show real depth-dependent horizontal L/R disparity with negligible vertical disparity
- pre-eye `+0x003A16A6` evidence packet; exact destination semantics PENDING
- helper arg2 first 0x40 temporary transpose/restore
- CSimpleWorldView main-world lineage

## EVIDENCE RECONCILIATION
Do not silently replace either evidence path:

### Path A — late intervention negative
`CSGOVR_NATIVE_STEREO_SEAM_RESULT_20260821_041928`
- one tested late DUAL_ORIGIN intervention did not authoritatively alter that final captured raster
- late projection/off-center did alter output

### Path B — actual GOLDEN / stock causal positive
- `CSGOVR_HLAE_EYE_PATCH_RESULT_20260821_015737.zip` original images show depth-dependent L/R parallax in the actual v0.5.5 path
- `CSGOVR_CORRECTNESS_RESULT_20260821_011601.zip` WORLD_X_PAIR proves a small RenderView-origin change can reach GPU c2, pixels and draw membership

Reconciled conclusion:
Path A falsifies only that exact late intervention/capture as an authoritative final eye-origin control point. It does not falsify eye translation generally. The old broad origin-point search is STOPPED until a new post-G3 visible defect exists.

## FALSIFIED / CLOSED imported
- full-size `0x14C`
- `client+0x0525BA60` reused inside both eye markers
- persistent helper arg2 output beyond first 0x40
- parser-generated falsification of first-0x40 transpose
- origin/yaw byte-integrity wrappers as camera ownership proof
- broad OpenXR rediscovery
- sequential screenshots as quantitative culling/disparity truth
- projection sign/transpose roulette after v0.6.1
- contaminated FULL_EXPERIMENTAL renderer conclusions
- old 8–12 origin-point batch before G3

## CURRENT FINALISTS
1. Repo byte-locked G3 child:
   - `experiments/quest_projection_child/eye_patch_quest_projection.c`
   - `experiments/quest_projection_child/QUEST_PROJECTION_MINIMAL.patch`
   - `tools/verify_quest_projection_child.py`
   - `.github/workflows/evidence-integrity.yml`
2. Branch-local unrun diagnostic:
   - `CSGOVR_G3_QUEST_COHERENCE_BATCH_v1.0.1_SYNCED_ONE_CLICK.zip`
   - SHA256 `595DA174AC39BA334F725DC1D130089F7F7C98D2BC4878F87B12F113BC0EAB70`
   - matrix-only phase = G3-primary useful
   - FOV/aspect phases = secondary G4 harvest only
3. If G4 opens after G3: `+0x003A16A6`, `+0x003A01E4`, `+0x0086E9C0`, CSimpleWorldView.

## CONCRETE TARGETS / OFFSETS / SYMBOLS / CLASSES
### Frozen client
- `RenderView client+0x003A5280`
- `CViewSetup +0x68 customProj bool`
- `+0x6C..+0xAB matrix`
- `+0xB0 world FOV`
- `+0xB4 viewmodel FOV`
- `+0xB8 origin`
- `+0xC4 angles`, `+0xC8 yaw`
- `+0xD0/+0xD4 world zNear/zFar`
- `+0xD8/+0xDC viewmodel zNear/zFar`
- `+0xE0 aspect`
- `+0x138/+0x13C/+0x140/+0x144` off-center
- `+0x148 bit0 m_bOffCenter`
- source-layout candidates only: `+0x34 customView bool`, `+0x38..+0x67 customView matrix`, `+0xAC CSMVolumeCuller`

### HLAE
- `CAfxStreams::DoRenderView +0x00022340`
- `CViewSetup_csgo` copy ctor `+0x00014BA0`
- `this+0x30 m_ForceCacheFullSceneState`
- `this+0x50 m_DoRenderViewCount`

### Shader/constants
- VS `c2 CAMERA_POS`
- `c4..c7 MODELVIEWPROJ`
- `c8..c11 VIEWPROJ`
- `c17` retained branch constant
- `c58..c60` bounded model candidate

### Culling / world view
- `client+0x003A16A6..+0x003A16DC` recovered pre-eye reader/dataflow
- second reader `client+0x003A01E4..+0x003A021A`
- `GeneratePerspectiveFrustum client+0x0086E9C0`
- lane writers around `+0x0086EB3C`
- CSimpleWorldView vtable `client+0x00BF4434`
- CSimpleWorldView Draw `client+0x003B0870`
- fields `+0x168 IWorldRenderList*`, `+0x16C CClientRenderablesList*`, `+0x170 ClientWorldListInfo_t*`
- lazy hydration `client+0x003B0067` / `+0x003B0083`
- slot `client+0x05248ED4`
- arrays `+0x0524A2F4/+0x0524A2F8/+0x0524A2FC`, stride `0x13F0`

### Infrastructure-only camera anchors
- `client+0x0038D123`
- `client+0x052632CC`
- `client+0x05263384`
- yaw `client+0x0038D15B`
- global commit `client+0x0038D1DE`
- `client+0x05245AB8`

## IMPORTANT PATHS / VERSIONS / DEPENDENCIES
- Steam AppID `4465480`, BuildID `22100934`
- game path `C:\Program Files (x86)\Steam\steamapps\common\csgo legacy`
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- HLAE 2.191.1 package SHA256 `D7BA05E1E540D82AC485A9B39F13D5C4CCF9FD94F460E0C9ECF069B94C880A59`
- `AfxHookSource.dll` SHA256 `5B9D13AE9ABFFEEF80851E10861302162A47D1B43D8483B4A8FCEC7EAA19B035`
- Oculus OpenXR recorded 1.205.0, Quest 3
- shared tools `C:\GOG Games\VR Research Tools`
- canonical test: exact `-insecure`, local `sv_lan`, Dust II, windowed, `mat_queue_mode 0`, no persistent patch

## EXISTING BAT / DLL / SCRIPTS / ANALYZERS
### Repo
- exact GOLDEN source: `runtime/golden_v0.5.5/src/`
- projection donor: `runtime/children/v0.6.1_projection_oracle/`
- minimal G3 child: `experiments/quest_projection_child/eye_patch_quest_projection.c`
- patch: `experiments/quest_projection_child/QUEST_PROJECTION_MINIMAL.patch`
- experiment contract: `experiments/NEXT_EXPERIMENT.md`
- integrity tool: `tools/verify_quest_projection_child.py`
- CI: `.github/workflows/evidence-integrity.yml`

### Branch-local diagnostic package
`CSGOVR_G3_QUEST_COHERENCE_BATCH_v1.0.1_SYNCED_ONE_CLICK.zip` contains:
- `00_RUN_EVERYTHING.bat`
- `ONE_CLICK.ps1`
- `src/eye_patch.c`
- `src/injector.c`
- `analysis/analyze_result.py`
- `analysis/static_verify.py`
- preflight, predictions, path-contract, evidence and hash manifests

It is UNRUN and must not replace the repo canonical minimal child.

## CURRENT GATE
**G3 — Quest asymmetric projection integration.**

## NEXT
1. Run `tools/verify_quest_projection_child.py` / equivalent integrity checks.
2. Run the repo byte-locked minimal Quest projection child from exact v0.5.5.
3. Record correct matrix per eye, pass counts, monitor regression, deterministic/same-frame evidence and headset verdict.
4. On PASS: freeze projection integration.
5. If headset is black/absent after matrix application is proven: next blocker = XR transport/submission, not projection.
6. If visible culling remains: open G4 at existing concrete targets.
7. If a visible origin/parallax defect remains: reconcile it against existing GOLDEN/stock causal evidence before any new origin archaeology.

## METHOD ALREADY USED
- exhaustive Source-family donor corpus
- retail CViewSetup field canaries
- D3D9 constant/projection provenance
- HLAE source + binary seam archaeology
- local full-0x198 eye copies
- projection/off-center causal canaries
- deterministic frozen capture
- Quest OpenXR FOV acquisition
- TTD frustum/cache and targeted reader recovery
- x32dbg causal/infrastructure controls
- CSimpleWorldView class funnel
- GitHub byte-integrity verifier/CI

## UNRESOLVED
- no accepted G3 runtime/headset RESULT yet
- final CSGOVR OpenXR swapchain/submission not established by accepted result set
- `+0x003A16A6` destination semantics/visibility owner unresolved, only relevant if G4 opens
- final head pose later
- weapon/hands/HUD later

## NEW-BRANCH BOOT ORDER
Read, in order:
1. `README.md`
2. `docs/STATE.md`
3. `docs/NEXT.md`
4. `docs/SYNC.md`
5. `docs/PROVEN.md`
6. `docs/FALSIFIED.md`
7. `docs/EXPERIMENTS.md`
8. `research/RESULT_INDEX.md`

Then continue from G3. Do not read the expired chat unless a specific provenance detail is missing.
