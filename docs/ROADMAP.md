# CSGOVR — ROADMAP

Snapshot: 2026-08-24 final cross-branch reconciliation.

## 1. T0 / G2.5 GPU XR transport certification — CURRENT
Protected baseline: exact v0.5.5 GOLDEN, BASE projection/origin/camera.

Goal:
- use exact pinned HLAE `afx_interop` replacement eye render targets;
- pass1 LEFT + pass2 RIGHT from one HLAE cycle share one `pairSeq`;
- wait for GPU completion before publication;
- external x64 Meta OpenXR host creates D3D11 on runtime adapter LUID `00000000:000118D1`;
- import/open GPU eye resources, acquire/wait/copy/release both XR swapchains, one `xrEndFrame` per coherent pair;
- no CPU readback production path unless GPU interop is concretely blocked.

Success freezes transport and moves immediately to G3/T1.

## 2. G3 / T1 Quest asymmetric projection
Queued, not falsified.

Use repository byte-locked projection child and exact retained P00:
- custom projection `+0x68/+0x6C`
- correct LEFT/RIGHT M02 sign
- engine zNear/zFar retained
- Source-native scalar FOV `110°`
- preserve v0.5.5 eye-origin behavior and pass3 monitor

Do not add head pose, new origin ownership or culling surgery in the same first T1 build.

## 3. Conditional clipping/culling classification
Only if exact P00 is applied and peripheral geometry still clips:
- aspect `0.9254939479` and FOV150 remain diagnostic controls only
- if a real culling ownership blocker survives, continue from existing concrete evidence:
  - `client+0x003A16A6`
  - `client+0x003A01E4`
  - `client+0x0086E9C0`
  - CSimpleWorldView `+0x00BF4434/+0x003B0870`

Do not reopen `client+0x0525BA60` both-eye reuse.

## 4. True eye origin / parallax, only if visible after T1
Late DoRenderView origin shift is already a failed authority point. GOLDEN + WORLD_X_PAIR independently prove origin/parallax behavior exists. If T1 still lacks correct depth, target the concrete earlier lifetime/overwrite boundary rather than restarting broad ownership search.

## 5. Quantitative capture truth
Before pixel-difference conclusions:
- same-cycle L/R
- repeated same-eye stability
- deterministic/frozen scene

Sequential captures are not a general quantitative oracle because prior temporal drift can dominate.

## 6. Head pose
Integrate HMD rotation/translation only after transport + projection + eye-origin behavior are coherent.

## 7. Weapon/controller separation
After world camera is stable:
- preserve world view
- independently drive viewmodel/controller pose
- prove visible root ownership before ballistics/muzzle policy

## 8. HUD / comfort
Treat HUD and viewmodel near/far separately from world projection. Native Valve `vr_projection_znear_multiplier=0.3` is a later comfort/reference policy, not the first projection proof.

## 9. Regression / release
- frozen client/HLAE hashes
- `-insecure`
- pass3 monitor intact
- coherent pairSeq, mixedPairCount=0
- XR acquire/wait/copy/release stable
- no black/freeze
- exact Quest P00 retained
- no eye-specific geometry loss
- menu/gameplay transitions stable
- reversible one-click restore

## Decision rule
After each RESULT: KEEP if distance to a playable VR build shrinks; ADAPT if stronger evidence gives a shorter route; STOP a line when falsified or when another branch closes it.
