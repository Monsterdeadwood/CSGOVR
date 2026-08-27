# CSGOVR — RESULT INDEX

Authority: frozen retail evidence + cross-tab reconciliation through **2026-08-27 14:38 EEST**.

Evidence classes: GOLDEN / PROVEN / RUNTIME_PASS / SUPPORTED / PARTIAL / PENDING / INFRASTRUCTURE / FALSIFIED / INVALID / SUPERSEDED / UNRUN.

## CURRENT WORLD / HMD GOLDEN
### `CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`
Build:
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256:
`5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Class: **GOLDEN / user-confirmed runtime.**
- VR works
- real HMD rotation works
- positional HMD translation works
- full HMD 6DoF works
- tearing remains a quality defect

This remains the world-camera rollback baseline.

## LATEST WEAPON / FIRE AUTHORITY
### `CSGOVR_WEAPON_FRESH_EPOCH_FIRE_DELTA_RESULT_20260827_143029.zip`
Class: **RUNTIME_PASS / latest weapon-fire instrumentation authority.**

Result status:
- `WEAPON_FRESH_EPOCH_FIRE_DELTA=PASS`
- `WEAPON_HAND_FOLLOW=PASS`
- `WEAPON_BULLET_AIM=PASS`
- `WORLD_STEREO_HMD=PASS`
- exact slot250 CalcViewModelView guard/hook active
- exact client/server FX_FireBullets guard/hook active
- client/server angle redirects `229/231`
- origin redirects `0/0`
- fire-delta publishes `3120`
- client/server delta applies `229/231`
- stale fallbacks `0`
- torn reads `0`
- maximum applied sample age `78 ms`
- freshness contract PASS
- normal ammo/spread retained
- game remained running

Interpretation: the v1.7 original-retail-fire-angle -> fresh RIGHT-Touch basis-delta architecture is runtime-coherent and stable. Exact user-visible barrel-axis accuracy for this specific run remains a distinct visual verdict unless separately confirmed.

## POST-M34 VIEWMODEL / WEAPON RESULTS

### `CSGOVR_COLD_CLOSURE_RESULT_20260826_035237.zip`
Class: **PROVEN cold closure / owner evidence.**
Retained exact retail viewmodel owner family; see `research/CROSS_TAB_SYNC_20260827.md`.

### CSGOVR WEAPON EXACT HAND FOLLOW v1.3.0
Class: **PROVEN runtime path / hand-follow.**
- exact `CPredictedViewModel::CalcViewModelView` slot250 route
- RIGHT grip translation + RIGHT aim orientation
- stock arms/viewmodel path retained
- rigid SE(3) grab calibration
- DrawModel mutation STOPPED
- static audit `85/85 PASS`
- package SHA256 `03C75627A29E5F0B29D410D0F82D7861B88680CF223897ABC348CF1204C49B72`

### `CSGOVR_WEAPON_BULLET_AIM_PROOF_RESULT_20260826_230123.zip`
Class: **PROVEN fire-angle redirect activity / controller-linked trajectory evidence.**
Retained v1.4-family facts:
- client redirects `580`
- server redirects `580`
- client/server coherence seen
- visual-proof runner used show-impacts / no-spread / infinite ammo for visibility only

This proves the fire-angle seam is real; it is not by itself the final muzzle-axis solution.

### `CSGOVR_WEAPON_MUZZLE_ORIGIN_RESULT_20260827_003016.zip`
Class: **PARTIAL / muzzle-origin FAIL.**
User/run observations:
- textures stable
- recoil present
- trigger firing present
- bullet hook active
- infinite ammo came from runner test command
- muzzle origin `0/0` due failed attachment path/name
- visual bullet direction not closed

### `CSGOVR_WEAPON_FINAL_MUZZLE_MATRIX_RESULT_20260827_012837.zip`
Class: **FALSIFIED integration route / STOP.**
- attachment `"1"` matrix lookup active
- `FINAL_MATRIX_RATE=1.0`
- textures regressed
- shot audio regressed
- client/server fire calls diverged `119/361`
- game not left running after snapshot

Decision: do not use render/viewmodel `GetAttachment/SetupBones` matrix calls from `CalcViewModelView` as fire-origin architecture.

### CSGOVR WEAPON SAFE POSTCALC AIM v1.6.1 / attempted `RESULT.zip`
Package SHA256:
`8BE519A71EFCF7E13660DDC392703209F02707926E6B07E29B75D455C20B6175`

Class: **SUPPORTED safety recovery / scientific fire verdict incomplete.**
Cold/static:
- static audit `120/120 PASS`
- cold verify `12/12 PASS`
- attachment/matrix query disabled
- origin mutation absent
- stock Weapon_ShootPosition retained

User observation:
- textures no longer disappear
- shot sound present
- bullets still not where expected

The uploaded generic archive was incomplete for quantitative v1.6.1 fire counters, so no stronger runtime numerical verdict is inferred. It nevertheless falsified the assumption that absolute post-Calc viewmodel-root angle is a complete replacement for the game's final fire-angle semantic.

## PVS / PACING QUALITY LANE
### `CSGOVR_T14_EXTERNAL_RESULT_20260827_143839.zip`
Class: **INFRASTRUCTURE / scientific FAIL.**

T14 identifies itself as `REAL MULTI-ORIGIN PVS v1.2.0`.
Collected summary:
- `VERDICT=FAIL`
- `FIRST_MISSING_EDGE=ACTIVE_DURATION_NOT_REACHED`
- engine/drawing connected
- RenderViews `10245`
- LEFT/RIGHT/MONITOR `3415/3415/3415`
- invalid schedule `0`
- pairs submitted `3415`
- dropped pairs `0`
- mixed pairs `0`
- right controller pose samples `3166`
- XR retry PASS attempt 1

Do not promote the PVS hypothesis from this run.

### `CSGOVR_T15_PACING_CLEAN_AB_v1.3.0_120S_ONECLICK.zip`
Class: **UNRUN / prepared** unless a later executed RESULT is supplied.

### `CSGOVR_COLD_SLOT249_251_LAG_OWNER_v3.1.1.zip`
Class: **UNRUN / prepared cold tool** unless executed output exists.

### `CSGOVR_FX_FIRE_ABI_SLOT_CLOSURE_v1.0.0_COLD_ONLY_ONECLICK.zip`
Class: **UNRUN / prepared cold tool** unless executed output exists.

---

# RETAINED 2026-08-25 FOUNDATION

## `CSGOVR_MASTER32A_POSE_COHERENCE_RESULT_20260825_194358.zip`
Class: **PROVEN instrumentation / quality finding.**
- host identity PASS
- native guard PASS
- Quest matrix PASS
- pair proof PASS
- LEFT/RIGHT same pose PASS
- exact render-pose lookup/submission PASS
- all 9 phases PASS
- pose lookup misses 0
- capture failures 0
- observed `droppedPairs=179`

Two-slot starvation/stale bursts are a cadence finding, not a reason to reopen camera ownership.

## M33 runtime lineage
Class: **SUPPORTED integration evidence.**
M33 reached real game pairs with the slot8/controller-capable transport lineage.

## M34 v0.2.x / `_RUN_20260825_221228`
Class: **INVALID / FROZEN as a baseline; useful historical failure boundary.**

Reached launch, de_dust2 and both injections, then stopped at first HLAE stereo render after two coupled changes:
1. drawViewModel 0 -> 1
2. passive/global BuildTransformations hook

Later exact slot250 work supersedes M34 as the current operational gate.

---

# HISTORICAL FOUNDATION

## `CSGOVR_HLAE_EYE_PATCH_RESULT_20260821_015737.zip`
Class: **GOLDEN historical stereo foundation.**
- pass1 LEFT, pass2 RIGHT, pass3 monitor
- full local `CViewSetup=0x198`
- HLAE DoRenderView `+0x22340`, copy ctor `+0x14BA0`
- original captures support genuine depth-dependent horizontal stereo parallax

## `CSGOVR_CORRECTNESS_RESULT_20260821_011601.zip`
Class: **PROVEN causal retail.**
- `CViewSetup.origin -> VS c2/CAMERA_POS`
- WORLD_X_PAIR translation changes pixels/draw membership
- projection families c4..c7 / c8..c11

## `CSGOVR_PROJECTION_TRUTH_RESULT_20260821_035411.zip`
Class: **PROVEN off-center semantics.**
- `+0x138/+0x13C/+0x140/+0x144`, flag `+0x148 bit0`

## `CSGOVR_CUSTOM_PROJECTION_MATRIX_RESULT_20260821_042218.zip`
Class: **PROVEN custom projection.**
- enable `+0x68`, matrix `+0x6C..+0xAB`
- M00/M02/M11/M12 causal semantics
- deterministic capture method retained

## `CSGOVR_OPENXR_FOV_DIAGNOSTIC_20260821_150332.zip`
Class: **PROVEN XR input.**
- Quest 3 / Meta OpenXR
- asymmetric LEFT/RIGHT FOV
- recommended 1504x1616 per eye
- passive eye separation about 64.780414 mm

## Valve SourceVR native scale cross-check
Class: **PROVEN source semantics.**
- `39.3700787 inches/meter`
- native SourceVR HMD/eye path uses `1 Source unit = 1 inch`
- Quest half-IPD about `1.27520 Source units`

## `CSGOVR_NATIVE_STEREO_SEAM_RESULT_20260821_041928`
Class: **PROVEN/SUPPORTED split.**
One exact late DUAL_ORIGIN intervention was negative; scope is narrow and does not falsify eye translation generally.

## `CSGOVR_TTD_FRUSTUM_CACHE_REPLAY_RESULT_20260822_012630.zip`
Class: **FALSIFIED + harvest.**
- falsified `client+0x0525BA60` both-eye reuse
- retained frustum/readers lineage

## `CSGOVR_TTD_REPLAY_FRUSTUM_READER_OWNER_3A16A6_RECOVERED_RESULT_20260823_160154.zip`
Class: **PROVEN evidence packet / semantics pending.**

## `CSGOVR_TTD_FRUSTUM_ARG2_PERSISTENCE_RESULT_20260823_164733.zip`
Class: **PROVEN + FALSIFIED.**
- first 0x40 temporary transpose/restore PROVEN
- persistent `+0x40..+0x1FF` FALSIFIED

## Origin/yaw control results
- `CSGOVR_CAUSAL_ORIGINX_ZERO_CONTROL_RESULT_20260823_043045.zip` — INFRASTRUCTURE
- `CSGOVR_CAUSAL_YAW_FINAL_POSE_CONTROL_RESULT_20260823_071600.zip` — INFRASTRUCTURE

## `CSGOVR_FULL_EXPERIMENTAL_FAILURE_20260821_022832.zip`
Class: **INVALID build/workflow failure.**

## Exact HLAE / AdvancedFX interop review — 2026-08-24
Class: **PROVEN source/build capability.**

## `CSGOVR_XR_TRANSPORT_v0.7.0_BUILD_FIRST_ONECLICK.zip`
Class: **SUPERSEDED prepared lineage.**

## `CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF`
Class: **historical prepared/static-audited fallback lineage.**
SHA256 `DFEA881888C83812A5B9EE3A7049F79D32935240F2C612DE4ABDF4F9A98834A3`.

## CURRENT GATES
Weapon/fire: preserve v1.7 safe chain; close only remaining exact barrel-axis / visual correctness without touching fire origin or render attachments.

PVS/pacing: separate quality lane; T14 not proven.

Canonical Aug 27 delta:
`research/CROSS_TAB_SYNC_20260827.md`
