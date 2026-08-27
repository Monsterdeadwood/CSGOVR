# CSGOVR — GATES

Snapshot: **2026-08-27 cross-tab sync.**

## G0 — OpenXR / Quest input
**PASS.** Quest 3 two views, asymmetric FOV and adapter/runtime requirements established.

## G1 — HLAE stereo world path
**PASS.** Frozen v0.5.5 lineage established two real world streams, pass1 LEFT / pass2 RIGHT / pass3 monitor, full `CViewSetup=0x198`.

## G2 — Projection semantics
**PASS.** Custom projection and Quest asymmetric projection are established. Do not reopen sign/transpose roulette.

## G2.5 / MASTER1 — End-to-end transport
**PASS.** Historical T0 GPU-vs-CPU finalist debate is not a current gate.

## G3 / MASTER2 — Quest projection integration
**PASS.** Exact Quest projection is part of the accepted runtime path.

## G4 / MASTER3.1 — Native camera origin ownership
**PASS.** Native authority at `client+0x003A5280` is closed.

## G5 / MASTER3.2A — HMD rotation
**PASS.** Real Quest HMD rotation drives the native camera path.

## G6 / MASTER3.2B — Positional HMD translation / full 6DoF
**PASS / CURRENT WORLD-HMD GOLDEN.**

Package:
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256:
`5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Evidence:
`CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`

User visual verdict: **VR + full HMD 6DoF works.** Tearing remains a separate quality defect.

## G7 — Exact viewmodel owner
**PASS / CLOSED.**

Exact frozen retail:
- `CBaseViewModel::CalcViewModelView = client+0x001B7F80`
- `CPredictedViewModel::CalcViewModelView = client+0x003420E0`
- runtime vtable slot `250`

The old M34 `drawViewModel + BuildTransformations` ambiguity is historical. Later exact-owner evidence supersedes it as the operational gate.

Dynamic scan `0..239` is STOPPED.

## G8 — RIGHT Touch hand / weapon ownership
**PASS for current one-hand viewmodel path.**

Current route:
`RIGHT Touch -> exact slot250 CalcViewModelView -> stock arms / stock weapon render path`.

Retained:
- RIGHT grip translation
- RIGHT aim orientation
- rigid grab calibration
- DrawModel mutation STOPPED

Independent LEFT skeletal/offhand path remains open.

## G9 — Exact fire seam / ABI
**PASS / CLOSED.**

Exact seams:
- client `FX_FireBullets = +0x0044DC90`
- server `FX_FireBullets = +0x004C4650`
- client callsite `+0x003F6989`
- server callsite `+0x005A7B09`

ABI includes `[ESP+4] vOrigin*`, `[ESP+8] vAngles*`, caller cleanup `ADD ESP,0x2C`.

Current safe contract: mutate angle only; stock `Weapon_ShootPosition()` owns origin.

## G10 — Fresh controller-owned fire direction
**PASS for runtime plumbing/freshness/coherence; visual barrel-axis closure remains.**

Evidence:
`CSGOVR_WEAPON_FRESH_EPOCH_FIRE_DELTA_RESULT_20260827_143029.zip`

Runtime:
- client/server angle redirects `229/231`
- origin redirects `0/0`
- fire-delta publishes `3120`
- applies `229/231`
- stale fallbacks `0`
- torn-read fallbacks `0`
- max applied sample age `78 ms`
- freshness contract PASS
- game stable after snapshot

v1.7 preserves original retail `GetFinalAimAngle` semantics and transforms them into the RIGHT-Touch basis.

Remaining narrow gate:
**direct exact user-visible barrel/controller axis correctness for the executed v1.7 line, if not already explicitly confirmed.**

## G11 — Physical VR muzzle origin
**OPEN / NOT REQUIRED FOR CURRENT SAFE FIRE PATH.**

v1.5 muzzle origin failed to produce a valid origin redirect.

v1.6 render-space attachment matrix route is **STOPPED** because render/audio/game stability regressed.

Do not reopen physical muzzle origin until a gameplay-space owner is independently proven.

## G12 — PVS / culling quality
**OPEN / T14 NOT PROVEN.**

`CSGOVR_T14_EXTERNAL_RESULT_20260827_143839.zip` reports:
`FAIL / ACTIVE_DURATION_NOT_REACHED`.

Useful infrastructure counters were collected, but the PVS hypothesis is not closed.

## G13 — Pacing / tearing / performance / release
**OPEN.**

Includes:
- tearing/cadence quality
- pacing
- HUD/comfort
- performance
- transition stability
- independent LEFT hand
- regression and reversible release packaging

Prepared `CSGOVR_T15_PACING_CLEAN_AB_v1.3.0_120S_ONECLICK.zip` is UNRUN unless an executed RESULT exists.

## STOP RULE
Do not reopen G0-G9 without contradictory evidence.
Do not patch M34 or v1.6 matrix path forward.
Do not mutate fire origin without a proven gameplay-space owner.
Keep PVS/pacing intrusive work separate from weapon/fire mutation.

Detailed current handoff:
`research/CROSS_TAB_SYNC_20260827.md`
