# CSGOVR — FALSIFIED / CLOSED / STOPPED

Snapshot: **2026-08-25 late sync after M34 rollback.**

## CURRENT INVALID / FROZEN LINE
### M34 v0.2.x
Class: **INVALID as a causal baseline, useful failure-boundary evidence.**

`_RUN_20260825_221228` reached launch, `de_dust2`, transport DLL injection and hand-probe DLL injection, then stopped at the first HLAE stereo render.

Two changes were coupled before that same render:
1. LEFT/RIGHT `drawViewModel 0 -> 1`
2. passive/global `C_BaseAnimating::BuildTransformations` vtable index 184 hook

Therefore this run does **not** falsify either change independently. What is stopped is the **coupled M34 configuration as a baseline**.

Do not patch M34 forward. Return to exact M3.2B GOLDEN and isolate B/C/D/E serially.

## CLOSED WORLD-CAMERA QUESTIONS
The following are no longer open research gates:
- MASTER1 transport
- MASTER2 Quest asymmetric projection
- MASTER3.1 native camera-origin ownership at `client+0x003A5280`
- MASTER3.2A HMD rotation
- MASTER3.2B positional HMD translation / full 6DoF

Last user-confirmed runtime GOLDEN:
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

Evidence:
`CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`

User visual verdict: VR + full HMD 6DoF works. Tearing remains a quality defect and is not evidence that these ownership gates failed.

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

This does not falsify eye translation generally. Historical GOLDEN parallax, WORLD_X_PAIR and later working full 6DoF all forbid reopening that broad conclusion.

## HISTORICAL TRANSPORT CLASSIFICATIONS
- `CSGOVR_XR_TRANSPORT_v0.7.0_BUILD_FIRST_ONECLICK.zip` as primary architecture — SUPERSEDED prepared lineage
- blind assumption that modern AfxInterop v6/v7 uses the older replacement-target handshake — CLOSED
- blind HLAE copy ctor `+0x14BA0` as a clean universal per-eye early hook — STOP

These remain historical implementation lessons, not the current gate.

## INVALID / INFRASTRUCTURE FAILURES, NOT SCIENTIFIC FALSIFICATIONS
- `CSGOVR_FULL_EXPERIMENTAL_FAILURE_20260821_022832.zip` — build/workflow contamination
- v0.6.5 XRFOV failure — path wiring failure
- TTD worldlist ownership replay failures that terminated before the scientific prediction executed
- origin/yaw control packets lacking the required terminal causal sequence
- missing defines, stale PID, parser, gameplay-gate and launcher failures unless the scientific prediction actually executed

## DO NOT REPEAT
- broad OpenXR discovery
- proving again that two world streams exist
- proving again frozen `RenderView client+0x003A5280`
- projection sign/transpose roulette
- sequential screenshot quantitative claims without same-eye control
- broad origin ownership batches
- treating compositor submission alone as stereo-geometry proof
- coupling HMD camera ownership to weapon aim
- replacement-renderer detours while the Source/HLAE path remains workable
- old T0/G2.5 GPU-vs-CPU architecture debate as a reaction to a viewmodel crash
- slot-depth as the default M34 explanation; M33 already reached real pairs with slot8/controller-capable lineage
- combining `drawViewModel=1`, passive `BuildTransformations`, controller IPC and transform mutation in one process

## CURRENT STOP RULE
Current gate is **viewmodel reintroduction / hand-path isolation on exact M3.2B GOLDEN**.

Allowed sequence:
A exact GOLDEN -> B drawViewModel only -> C passive BuildTransformations -> D controller IPC -> E transform mutation.

Do not reopen transport, Quest projection, origin, HMD rotation or basic positional 6DoF unless new contradictory evidence appears.
