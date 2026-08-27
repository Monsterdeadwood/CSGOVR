# CSGOVR — FALSIFIED / CLOSED / STOPPED

Snapshot: **2026-08-27 cross-tab sync through v1.7 FRESH_EPOCH_FIRE_DELTA and T14.**

## CURRENT WEAPON / VIEWMODEL STOPS

### Dynamic viewmodel vtable locator 0..239
**FALSIFIED / STOPPED.**

Exact retail `CPredictedViewModel::CalcViewModelView` is runtime vtable slot `250`. The old v1.2.0 dynamic scan searched only `0..239`, so it physically could not reach the real owner and also scanned unsafely beyond function boundaries.

Do not revive the dynamic locator. Use exact frozen target guards plus slot250.

### DrawModel transform mutation as active hand route
**STOPPED.**

The stable hand-follow lineage uses exact `CPredictedViewModel::CalcViewModelView` slot250 while retaining the stock arms/viewmodel render route. Do not return to DrawModel mutation merely to move the weapon.

### v1.6 FINAL MUZZLE MATRIX / render attachment inside CalcViewModelView
Evidence:
`CSGOVR_WEAPON_FINAL_MUZZLE_MATRIX_RESULT_20260827_012837.zip`

**FALSIFIED as an integration architecture / STOP.**

Although attachment `"1"` was found and matrix sampling reached `FINAL_MATRIX_RATE=1.0`, the integration caused:
- disappearing textures
- missing/degraded shot audio
- client/server fire-call divergence `119/361`
- game not running after snapshot

A first-person viewmodel attachment matrix is render/FOV formatted and is not automatically a safe gameplay-space origin. Calling `GetAttachment/SetupBones` from the slot250 path crossed subsystem boundaries.

Do not patch v1.6 forward.
Do not use this render-space matrix as current fire origin.

### Absolute post-Calc root angle == final gameplay fire angle
Evidence: v1.6.1 safe post-Calc line + user observation.

**FALSIFIED as a complete semantic substitution.**

Removing attachment/origin mutation restored texture/audio stability, but bullets were still not where expected. The retail final fire path already supplies its own weapon-aware `GetFinalAimAngle`; replacing it absolutely with the post-Calc viewmodel-root angle discards/distorts that native relationship.

v1.7 therefore preserves the original retail fire angle and transforms its basis instead.

### Fire origin mutation without gameplay-space owner
**STOPPED by policy.**

Current safe architecture keeps `[ESP+4] vOrigin` untouched and retains stock `Weapon_ShootPosition()`.

Do not mutate fire origin until an independently proven gameplay-space muzzle owner exists.

## PVS / PACING STOP CLASSIFICATION
### `CSGOVR_T14_EXTERNAL_RESULT_20260827_143839.zip`
T14 is **not a scientific PASS**.

Its own summary:
- `VERDICT=FAIL`
- `FIRST_MISSING_EDGE=ACTIVE_DURATION_NOT_REACHED`

It collected useful transport/controller evidence, but do not promote the multi-origin PVS hypothesis from this run.

## HISTORICAL CURRENT-INVALID LINE
### M34 v0.2.x
Class: **INVALID as a causal baseline, useful failure-boundary evidence.**

`_RUN_20260825_221228` reached launch, `de_dust2`, transport DLL injection and hand-probe DLL injection, then stopped at the first HLAE stereo render.

Two changes were coupled before that render:
1. LEFT/RIGHT `drawViewModel 0 -> 1`
2. passive/global `C_BaseAnimating::BuildTransformations` vtable index 184 hook

The run does not falsify either independently. The coupled M34 configuration is stopped as a baseline.

Later exact slot250 owner/hand-follow work supersedes M34 isolation as the current operational gate.

## CLOSED WORLD-CAMERA QUESTIONS
The following are no longer open research gates:
- MASTER1 transport
- MASTER2 Quest asymmetric projection
- MASTER3.1 native camera-origin ownership at `client+0x003A5280`
- MASTER3.2A HMD rotation
- MASTER3.2B positional HMD translation / full 6DoF

World runtime GOLDEN:
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

Evidence:
`CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`

User visual verdict: VR + full HMD 6DoF works. Tearing is a quality defect and does not reopen these gates.

## ARCHITECTURAL / SCIENTIFIC FALSIFICATIONS
- full `CViewSetup=0x14C` — SUPERSEDED; exact HLAE copy path establishes `0x198`
- `client+0x0525BA60` reused inside BOTH LEFT and RIGHT eye marker regions — FALSIFIED
- persistent helper arg2 output beyond first `0x40` (`+0x40..+0x1FF`) — FALSIFIED
- earlier parser falsification of first `0x40` transpose — INVALID/SUPERSEDED; temporary transpose/restore is PROVEN
- originX/yaw byte wrappers as camera-ownership proof — REJECTED; infrastructure only
- symmetric eye projection sufficient for Quest 3 — FALSIFIED
- raw Khronos column-major matrix memcpy into Source VMatrix — FALSIFIED/CLOSED
- AdvancedFX Unity `0.01905 m/unit` as native SourceVR HMD/eye scale — FALSIFIED for this purpose
- static post-SetUpView candidates `+0x38E5D7/+0x38EC08/+0x3A0381` as retained final route — CLOSED
- CSkyboxView / CShadowDepthView as main-world owner — FALSIFIED for that role

## EXACT LATE-ORIGIN NEGATIVE — NARROW SCOPE
`CSGOVR_NATIVE_STEREO_SEAM_RESULT_20260821_041928` showed one tested late DUAL_ORIGIN intervention did not authoritatively change its final capture.

This does not falsify eye translation generally. Historical GOLDEN parallax, WORLD_X_PAIR and later working full 6DoF forbid reopening that broad conclusion.

## HISTORICAL TRANSPORT CLASSIFICATIONS
- `CSGOVR_XR_TRANSPORT_v0.7.0_BUILD_FIRST_ONECLICK.zip` as primary architecture — SUPERSEDED prepared lineage
- blind assumption that modern AfxInterop v6/v7 uses the older replacement-target handshake — CLOSED
- blind HLAE copy ctor `+0x14BA0` as a clean universal per-eye early hook — STOP

## INVALID / INFRASTRUCTURE FAILURES, NOT SCIENTIFIC FALSIFICATIONS
- `CSGOVR_FULL_EXPERIMENTAL_FAILURE_20260821_022832.zip` — build/workflow contamination
- v0.6.5 XRFOV failure — path wiring failure
- TTD worldlist ownership replay failures that terminated before scientific prediction executed
- origin/yaw control packets lacking required terminal causal sequence
- missing defines, stale PID, parser, gameplay-gate and launcher failures unless the scientific prediction executed
- incomplete generic v1.6.1 `RESULT.zip` — insufficient for quantitative fire counters; direct user render/audio observation remains usable

## DO NOT REPEAT
- broad OpenXR discovery
- proving again that two world streams exist
- proving again frozen `RenderView client+0x003A5280`
- projection sign/transpose roulette
- sequential screenshot quantitative claims without same-eye control
- broad origin ownership batches
- treating compositor submission alone as stereo-geometry proof
- coupling HMD camera ownership to weapon aim
- replacement-renderer detours while Source/HLAE path remains workable
- old T0/G2.5 GPU-vs-CPU architecture debate as reaction to a viewmodel issue
- slot-depth as default M34 explanation
- combining intrusive PVS/pacing and weapon/fire mutations in one process
- render-space attachment/muzzle matrix origin experiments without a new independent gameplay-space reason
- absolute post-Calc viewmodel root angle as a substitute for original retail fire-angle semantics

## CURRENT STOP RULE
Preserve:
`world/HMD GOLDEN -> stock viewmodel -> exact slot250 hand-follow -> original retail GetFinalAimAngle -> fresh VR basis delta -> exact FX_FireBullets angle only -> stock Weapon_ShootPosition origin`.

Keep PVS/pacing in a separate lane.

Canonical Aug 27 evidence delta:
`research/CROSS_TAB_SYNC_20260827.md`
