# CSGOVR META Numeric Convergence — 2026-09-08

RESEARCH_DOC_CLASS: CONVERGENCE
EVIDENCE_ID: CSGO-EV-META-NUMERIC-CONVERGENCE-20260908
SOURCE_GLOBAL_REVISION: 5
SOURCE_MAIN_SHA: 8727cb02536e9b9699da7528efda76bb8f9d6bc5
ROLE: META
FACT_STATUS: COLD_CONVERGENCE / NO LIVE AUTHORIZATION

## Purpose
Reconcile the current CSGOVR branches into one minimal numeric architecture without mixing visibility, raster, weapon-render and gameplay-fire semantics.

No number in this document is promoted as a causal runtime fix unless explicitly marked PROVEN. The objective is to assign each already-known number to exactly one owner.

## 1. Raster owner — KEEP exact Quest custom projection
The GPU raster remains owned by the existing exact asymmetric Quest `m_matCustomProjMatrix` path.

Protected:
- `m_bCustomProjMatrix = 1`
- exact LEFT / RIGHT Quest projection matrices
- HMD 6DoF pose
- eye-local origin
- near/far
- monitor pass

Do not replace this with a symmetric projection.

## 2. CPU visibility owner — strongest COLD candidate
Current frozen/source lineage supports a dual-carrier model:

```text
GPU raster      = exact asymmetric Quest custom matrix
CPU visibility  = conservative scalar FOV/aspect envelope
```

Candidate values:

```text
world FOV    = 108.0 deg
world aspect = tan(54 deg) / tan(55 deg)
             = 0.963752996169473
vertical CPU visibility FOV = 110.0 deg
```

This covers conservative Quest bounds:

```text
horizontal = +/-54 deg
vertical   = +/-55 deg
```

Preferred carrier is the late eye-local native `CViewSetup` copy only:

```text
+B0 world FOV    = 108.0
+E0 world aspect = 0.963752996169473
```

Do NOT change:
- `+B4` viewmodel FOV
- custom projection matrix
- eye origin / angles
- near/far
- PVS/portal/occlusion cvars
- game-owned original `CViewSetup`
- Weapon_ShootPosition

Status: `CANDIDATE_INTERVENTION / NOT AUTHORIZED LIVE`.

## 3. Why these visibility numbers matter
The old monitor-like pair is approximately:

```text
world FOV = 106.260201 deg
aspect    = 1.777778
```

Under the Source scalar conversion this corresponds to only about:

```text
vertical FOV ~= 73.739785 deg
```

while the conservative Quest vertical visibility requirement is about `110 deg`.

The numerical mismatch is therefore large in the CPU visibility domain even while the GPU raster can already be correct through the custom matrix.

This supports the dual-carrier hypothesis but does NOT prove it caused the RC3 regression, because SAFE161 is a negative control against claiming asymmetry by itself as the RC3-specific root.

## 4. Stereo/IPD — KEEP protected baseline
Historical stereo half-separation `1.2598425` Source units corresponds to approximately 32 mm per eye / 64 mm total if interpreted under the Source-inch scale.

The recovered Quest eye-separation figure around `64.780 mm` differs by only about `1.22%` from 64 mm.

Do not chase this difference as the current root:
- it is far too small to explain the observed gross weapon-angle error;
- current world/HMD stereo is protected by the M3.2B closure;
- current native eye origins should remain the owner.

## 5. Gameplay AIM owner — direct command-synchronous RIGHT AIM
Ballistic aim must branch before visual weapon grab calibration.

Preferred command-synchronous math:

```text
Rb = SourceAngleMatrix(cmdN.viewangles)
Rr = OpenXRRelativeAimQuatToSourceMatrix(RIGHT_AIM.relativeQuat)
Rw = Rb * Rr

cmdN.aimdirection = normalize({
    Rw[0][0],
    Rw[1][0],
    Rw[2][0]
})
```

Rules:
- no Euler round-trip;
- no `g_weaponGrabRotOffset` in gameplay aim;
- no pre-baked AimPunch;
- stock AimPunch/recoil is applied later;
- stock spread/inaccuracy remains later;
- stock `Weapon_ShootPosition()` remains fire-origin owner;
- initial gate remains five conventional firearm corridors only;
- grenade remains stock.

## 6. Visual weapon owner — separate from ballistic aim
`g_weaponGrabRotOffset` is a graphics/model calibration, not ballistic calibration.

For convergence, the visible weapon should use RIGHT AIM orientation plus only a stable visual/model-axis correction if required. Dynamic activation-time grab rotation must not become the projectile direction.

Conceptually:

```text
HAND_FRAME != WEAPON_RENDER_FRAME != WEAPON_AIM_FRAME != SHOT_FRAME
```

A visual alignment child may remove dynamic grab rotation while keeping translation, but that is separate from the gameplay vector above.

## 7. Residual parallax after angular alignment
Even with a perfect RIGHT-AIM direction, controller ray origin and stock `Weapon_ShootPosition()` are different points.

A later Stage-2 convergence, if needed, is:

```text
controller ray -> target T
stock fire origin O = Weapon_ShootPosition()
final shot direction = normalize(T - O)
```

This preserves stock fire origin while converging on the controller-selected target.

Do not add this until Stage-1 direction and visibility are independently classified.

## 8. RC3 / RC4 reconciliation
Exact RC3 package and executed RESULT are now recoverable from Library.

RC3 established:
- native CUserCmd execution active;
- client and server firearm gates executed;
- stock Weapon_ShootPosition / recoil / spread preserved;
- user-visible alignment remained crooked;
- far-world/distant content disappeared.

Raw RC3 source/package audit additionally shows:
- protected `xr_host.cpp` identity against SAFE161;
- protected HMD/Quest/D3D/SHM render subset unchanged;
- no recovered PVS/culling mutation;
- x86 hook ABI/source/compile evidence agrees with runtime activation.

Therefore the old `RAW_EXECUTED_RC3_DETOUR_ABI = UNVERIFIED_IN_CURRENT_ACCESS` statement is superseded as an access limitation. Byte identity of the exact runtime-built target DLL is still not claimed unless retained from RESULT.

RC4 CLEAN4HOOK is an AIM-side causal discriminator only:
- removes persistent client `player+0x382C` mutation;
- uses exact VectorAngles redirect for five firearm corridors;
- leaves PVS/culling untouched;
- is not evidence of a visibility fix by package existence.

## 9. Current best architecture

```text
HEAD / WORLD
  native HMD 6DoF + exact eye origins

GPU RASTER
  exact Quest asymmetric custom matrix

CPU VISIBILITY candidate
  late eye-local CViewSetup:
    FOV    = 108.0
    aspect = 0.963752996169473

GAMEPLAY AIM
  cmdN.viewangles * RIGHT_AIM.relativeQuat
  -> normalized Source forward
  -> cmdN.aimdirection

VISUAL WEAPON
  RIGHT AIM orientation
  + visual-only stable model correction if required
  NO ballistic grab offset

FIRE ORIGIN
  stock Weapon_ShootPosition

BALLISTICS
  stock AimPunch + recoil + spread/inaccuracy

OPTIONAL LATER STAGE-2
  controller target T -> normalize(T - stock origin O)
```

## 10. FIRST_MISSING_EDGE after synchronization
The ordinary-world source ownership chain is no longer the highest-value unknown. The next discriminating fact is:

```text
At the same deterministic scene/pose,
would late-eye scalar visibility carrier
FOV=108 / aspect=0.963752996
change CullBox / ordinary renderable membership
for geometry that the exact Quest raster can see?
```

This remains COLD-first. Prefer an existing frozen same-scene membership/capture corpus or deterministic offline replay over a new LIVE.

If COLD cannot answer the membership delta, only then define one visibility-only LIVE with no AIM mutation.

## Governance verdict

```text
NUMERIC_ARCHITECTURE = COHERENT / COLD-SUPPORTED
VISIBILITY_108_0963753 = STRONG CANDIDATE / NOT PROVEN FIX
QUEST_CUSTOM_MATRIX = KEEP
M3.2B HMD/STEREO = KEEP
GAMEPLAY_GRAB_ROT_OFFSET = REMOVE / DO_NOT USE
STOCK FIRE ORIGIN/RECOIL/SPREAD = KEEP
AIM AND FAR_WORLD = SEPARATE LANES
LIVE = BLOCKED WHILE FARWORLD PENDING_SYNC REMAINS
```
