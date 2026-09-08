# CSGOVR FAR_WORLD Pending Sync Packet — 2026-09-08

RESEARCH_DOC_CLASS: CONVERGENCE
EVIDENCE_ID: CSGO-EV-FARWORLD-PENDING-20260908
CLAIM_ID: CSGO-PACING-PVS
FACT_STATUS: CANDIDATE / PENDING_VALIDATION / NARROWED
OWNER: UNBOUND
SOURCE_GLOBAL_REVISION: 3
SOURCE_MAIN_SHA: 27ec4162bee5d04d6d77768f51fa1bea680e56d5
VALIDATED_THROUGH_GLOBAL_REVISION: 5
LATEST_META_CONVERGENCE: research/META_NUMERIC_CONVERGENCE_20260908.md

## PRIMARY FACT
Determine whether the RC3 far-world / distant-object disappearance is caused by a CPU visibility-envelope mismatch or another late ordinary-world render/capture defect, without promoting PVS, pixel visibility, portal behavior, asymmetric raster or AIM-hook infrastructure as causal unless exact evidence isolates the owner.

This remains one unpromoted HANDOFF / SYNC_PACKET. No duplicate FAR_WORLD packet is created.

## Canonical inputs
- RC3 native CUserCmd execution is runtime-proven for the observed firearm corridor.
- RC3 all-up integration is falsified because alignment remained crooked and far-world/distant content disappeared.
- M3.2B remains protected world/HMD rollback authority.
- AIM alignment and FAR_WORLD causality remain separate lanes.
- No AIM/weapon/ballistic mutation belongs in a FAR_WORLD causal intervention.

## COLD convergence already closed
### Ordinary-world ownership
Current CSGOVR source/frozen lineage supports:
- eye-local/versioned PVS/world-list/renderable construction;
- camera frustum regenerated from late eye-local `CViewSetup` FOV/aspect;
- `g_Frustum` participates in ordinary-world `CullBox` decisions;
- portal/area frustums are narrower refinements of the camera frustum;
- hardware occlusion uses view identity rather than one immortal global camera/result.

Simple models are strongly rejected:
- one center PVS blindly reused by both eyes;
- one persistent LEFT world/renderable list blindly reused by RIGHT;
- simple renderables-job race as the current root;
- one immortal global CPU occlusion view;
- HLAE full-scene cache as ordinary PVS ownership.

### Pixel visibility boundary
Pixel-visibility query/history collision remains a real stereo-history concern for effects such as glow/sun obstruction/overlay behavior. It is not ordinary-world root proof.

### Portal boundary
`r_portalsopenall` recovery is not unique PVS proof. A too-narrow camera frustum followed by narrower area-frustum `CullBox` tests is a competing mechanism.

## RAW RC3 access correction
The exact RC3 package and executed result are now recoverable from Library:

```text
CSGOVR_ULTIMATE_SAFE161_NATIVE_AIM_RC3_3HOOK_GUARDFIX_ONECLICK.zip
CSGOVR_ULTIMATE_SAFE161_NATIVE_AIM_RC3_3HOOK_RESULT_20260907_050436.zip
```

Recovered package/source audit shows:
- `xr_host.cpp` is byte-identical to SAFE161 protected lineage;
- protected HMD/Quest/D3D/SHM render subset remains unchanged;
- no recovered PVS/culling mutation;
- `CheckSplitScreenMimic` uses the expected x86 thiscall->fastcall bridge;
- client pointer helper is a normal fastcall detour;
- server helper uses a naked wrapper preserving return address plus ESI/EBX/EDI before calling a cdecl helper;
- i686 compile/assembly proof agrees with the source form;
- runtime result proves the hook bundle actually activated and the server firearm corridor executed.

Therefore the old access-only statement:

```text
RAW_EXECUTED_RC3_DETOUR_ABI = UNVERIFIED_IN_CURRENT_ACCESS
```

is superseded. Exact byte identity of the runtime-built target DLL is still not claimed unless retained separately.

Verdict:

```text
SHARED SEMANTIC ROOT = STRONGLY WEAKENED
SHARED INFRASTRUCTURE ROOT = VERY STRONGLY WEAKENED
INDEPENDENT AIM + FAR_WORLD DEFECTS = LEADING
```

## Strongest numeric visibility candidate
A dual-carrier model remains the cleanest current candidate:

```text
GPU raster      = exact asymmetric Quest custom matrix
CPU visibility  = conservative scalar FOV/aspect
```

Candidate values:

```text
world FOV    = 108.0 deg
world aspect = tan(54 deg) / tan(55 deg)
             = 0.963752996169473
vertical CPU visibility FOV = 110.0 deg
```

Preferred carrier:

```text
late eye-local CViewSetup copy only
+B0 world FOV    = 108.0
+E0 world aspect = 0.963752996169473
```

Do not modify:
- viewmodel FOV;
- exact Quest custom matrix;
- origin/angles/near/far;
- PVS/portal/occlusion cvars;
- game-owned original CViewSetup;
- monitor pass;
- Weapon_ShootPosition.

Status: `STRONG COLD CANDIDATE / NOT PROVEN FIX / NOT AUTHORIZED LIVE`.

## Why the numbers are interesting
The old monitor-like scalar pair is approximately:

```text
FOV = 106.260201 deg
aspect = 1.777778
```

which corresponds to about `73.739785 deg` vertical scalar FOV, while the conservative Quest visibility requirement is about `110 deg` vertical.

This is a large CPU-visibility mismatch while the GPU raster can still be correct through the custom matrix.

SAFE161 remains a negative control against saying this mismatch alone is the RC3-specific cause. Therefore the candidate must be tested as a membership discriminator, not assumed as root cause.

## Updated FIRST_MISSING_EDGE
The source ownership chain itself is no longer the highest-value missing edge.

Current edge:

```text
At the same deterministic scene + pose,
does late-eye scalar carrier
FOV=108.0 / aspect=0.963752996169473
change ordinary CullBox/renderable membership
for geometry visible to the exact Quest raster?
```

Priority:
1. reuse existing same-scene frozen membership/capture corpus if available;
2. compute or replay the two frusta deterministically COLD;
3. identify which ordinary renderables switch membership;
4. reconcile those switches against the RC3 far-world symptom;
5. only if COLD cannot answer, define exactly one visibility-only runtime discriminator.

## RC4 boundary
`CSGOVR_SAFE161_NATIVE_AIM_RC4_CLEAN4HOOK_RUNNERFIX_v1.0.2_ONECLICK.zip` is an AIM-side causal discriminator, not FAR_WORLD proof.

It removes persistent client `player+0x382C` mutation, uses five firearm VectorAngles corridors, leaves grenade stock and leaves PVS/culling untouched.

Package existence is not execution evidence and does not authorize combining RC4 AIM mutation with the visibility carrier.

## PASS for packet validation
The packet becomes SUPPORTED only if the numeric carrier or another exact candidate is tied to a deterministic ordinary-renderable membership difference compatible with the observed missing far-world content, without violating SAFE161/M3.2B controls.

## FAIL / discard conditions
Rewrite or discard this candidate if:
- deterministic membership is unchanged for the relevant geometry;
- exact current lineage shows scalar FOV/aspect cannot influence the relevant ordinary-world rejection path;
- a stronger exact render/capture owner is recovered;
- another already-promoted result answers the same primary fact.

## DO_NOT_REPEAT
- no broad PVS-disable or occlusion-disable LIVE;
- no `r_portalsopenall` as standalone proof;
- no pixel-visibility fix promoted as ordinary-world repair;
- no player+0x382C AIM side-effect promoted as FAR_WORLD root without visibility edge;
- no AIM/weapon mutation inside FAR_WORLD validation;
- no global renderer redesign before this deterministic membership discriminator is exhausted;
- no LIVE while this packet remains pending.

## Handoff state
```text
PACKET = CSGO-EV-FARWORLD-PENDING-20260908
CLAIM = CSGO-PACING-PVS
STATUS = PENDING_VALIDATION / NARROWED
OWNER = UNBOUND
LIVE = BLOCKED_BY_PENDING_SYNC
LEADING_MODEL = INDEPENDENT AIM + FAR_WORLD DEFECTS
NEXT = deterministic same-scene membership discriminator for FOV=108 / aspect=0.963752996169473
```
