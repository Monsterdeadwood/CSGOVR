# CSGOVR — NATIVE AIM EVIDENCE HANDOFF

Snapshot: **2026-08-28**

Purpose: converge the weapon/fire branch after the frozen-retail COLD re-analysis. This file supersedes the stale Aug 27 interpretation of the `FX_FireBullets` ABI and records the current native `CUserCmd::aimdirection` mainline.

## FROZEN TARGET

- legacy CS:GO x86, AppID `4465480`, BuildID `22100934`
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- `server.dll` SHA256 `699757BB5C8DDB71193F68DABA22F59F697491DDBD768BC8656AC2CE435C4DB8`
- local/offline `-insecure` target only

## PROVEN — CORRECTED FX ABI

The old Aug 27 labels were wrong. Frozen caller producer + callee consumer analysis closes the physical entry ABI as:

```text
ECX       = iPlayerIndex
EDX       = weapon object
[ESP+04]  = econ/item view object
[ESP+08]  = vOrigin
[ESP+0C]  = vAngles
cleanup   = 0x2C
client/server symmetric
```

Consequences:

- `[ESP+08]` is origin and must not be used as the fire-angle slot.
- `[ESP+0C]` is the true fire-angle slot.
- the old `EDX = item definition index` claim is FALSIFIED for this frozen retail target.
- the old `[ESP+04]=vOrigin / [ESP+08]=vAngles` claim is FALSIFIED.

`client+0x003F6989` is not a unique local weapon proof caller. It is the network `C_TEFireBullets` consumer path. A global FX hook can therefore double-apply a controller delta to a locally-originated server shot after the fire event returns to the client.

## PROVEN — NATIVE CUserCmd AIM CHANNEL

Frozen 2023 binaries contain the native command-owned aim channel:

```text
CUserCmd::aimdirection
+0x18
+0x1C
+0x20
```

Frozen `client.dll` contains the literal field names:

```text
aimdirection[0]
aimdirection[1]
aimdirection[2]
```

The field is carried through the stock command transport:

```text
CLIENT CUserCmd aimdirection
    -> stock WriteUsercmd
    -> network
    -> SERVER ReadUsercmd
    -> SERVER CUserCmd +18/+1C/+20
```

Evidence class: **RETAIL_PROVEN**.

## PROVEN — SERVER COMMAND TO GAMEPLAY AIM

Frozen server command processing copies:

```text
CUserCmd +18/+1C/+20
    -> player +14AC/+14B0/+14B4
```

The same player aim vector is later consumed by the pointer-input final-aim branch before weapon fire, converted to an angle, then combined with native aim punch before the stock fire path.

Therefore the native authoritative chain is:

```text
CUserCmd::aimdirection
    -> player AimDirection
    -> pointer-input final aim
    -> native AimPunch
    -> stock spread / inaccuracy
    -> stock Weapon_ShootPosition
    -> stock FX_FireBullets
```

Evidence class: **RETAIL_PROVEN**.

## PROVEN — CLIENT PREDICTION COMMAND OWNER

Corrected RVA:

```text
CPrediction::RunCommand = client+0x00343BF0
```

The earlier `client+0x00342BF0` value was an RVA conversion error and is FALSIFIED.

Frozen command lifetime:

```text
client+0x00343C0F
player+0x3348 = current CUserCmd*

client+0x00343E73
player+0x3348 = NULL
```

This gives client reprediction an exact command identity instead of a latest-pose heuristic.

## PROVEN — POINTER-SEMANTICS CENSUS

Machine COLD self-test result:

```text
NATIVE_AIM_STATIC_SELFTEST=PASS
CLIENT_POINTER_CALLERS=14
SERVER_POINTER_CALLERS=8
CLIENT_GAMEPLAY_AIM_ALLOW=6
SERVER_GAMEPLAY_AIM_ALLOW=6
FX_MUTATION_REQUIRED=NO
```

Server pointer-helper census:

```text
ALLOW
58F88B  grenade final aim
5A7A34  bullet
5A8037  bullet
5B0349  M3
5B37BB  NOVA/Sawedoff
5BC61B  XM1014

REJECT
216DCA  use / interaction trace
49F492  motion-controller recoil scaling
```

Global pointer/Hydra spoofing is therefore forbidden. It would also change the motion-controller recoil scale and violate the protected gameplay baseline.

The rule is:

```text
POINTER SEMANTICS
ONLY FOR APPROVED FINAL WEAPON/GRENADE AIM CONSUMERS
```

## PREFERRED OWNERSHIP MODEL

Keep these authorities separate:

```text
HEAD        = HMD/world camera
HAND_GRIP   = visual/controller grip space
HAND_AIM    = controller aim space
WEAPON      = stock viewmodel via slot250 visual owner
FIRE_AIM    = CUserCmd::aimdirection
FIRE_ORIGIN = stock Weapon_ShootPosition
```

`weaponGrabRotOffset` is visual/model alignment and must not be reused as gameplay fire calibration.

`IN_ATTACK` is not part of VR aim provenance. Aim and fire action can live in the same `CUserCmd` while remaining distinct semantic owners.

## CURRENT SAFE IMPLEMENTATION CONTRACT

Current conservative design has four hooks:

```text
1 CLIENT CheckSplitScreenMimic
  after stock input is complete:
  write normalized controller aim to cmd+18/+1C/+20
  publish command provenance ring

2 CLIENT IsInputDeviceAPointer
  only approved gameplay callers
  + local player
  + player+0x3348 current command
  + exact ring identity
  => return TRUE

3 SERVER CPlayerMove::RunCommand
  local player
  + exact ring identity
  => scope a temporary capability token around original RunCommand

4 SERVER IsInputDeviceAPointer
  temporary capability token
  + approved gameplay caller
  => return TRUE
```

`FX_FireBullets` remains stock in this preferred mainline.

## COMMAND PROVENANCE CONTRACT

A published VR aim command owns its aim by command identity, not by wall-clock age at later consumers.

Recommended ring identity:

```text
sessionEpoch
command_number
tick_count
canonical aimBits[3]
localPlayerIndex
sampleId
poseSeq
```

Hardening already required:

- full identity check, never modulo-slot identity alone
- power-of-two ring is acceptable only with full identity verification
- canonicalize `-0.0f` to `+0.0f`
- reject NaN / Inf
- controller freshness is checked at command creation only
- once a command is validly published, later network delay does not revoke its VR aim
- remote/bot server commands must fail the local-player identity check even if command numbers collide

Principle:

```text
FRESHNESS = property of command creation
IDENTITY  = property of command consumption
```

## NEW COLD GATE — 4 HOOKS -> 3

The remaining high-value question is whether the frozen server already exposes the currently executing `CUserCmd*` from the player/command context at each approved final-aim pointer call.

If frozen retail proves a server-side current-command owner with exact lifetime, Hook 3 and TLS/capability state can be removed. The server pointer helper would locally prove:

```text
approved return RVA
AND recovered player
AND player.currentCmd != NULL
AND exact ring identity
AND derived server player index == ring.localPlayerIndex
```

This would reduce the architecture from four hooks to three without weakening fail-closed ownership.

### STILL UNKNOWN

- exact frozen server current-command field/offset, if present
- exact set/clear lifetime around authoritative command execution
- exact player-register map at all six approved server pointer-helper callers
- whether all required provenance can be reconstructed locally at the pointer helper without the RunCommand hook

### KILL CONDITION

If frozen server has no stable current-command owner reachable at the final-aim consumer, or if recovering it requires weaker/probabilistic inference, retain the four-hook design. Do not remove the RunCommand capability token merely for hook-count aesthetics.

## FALSIFIED / STOPPED

Do not resurrect:

- global FX mutation as preferred architecture
- `fresh controller pose => mutate next FX call`
- `ECX local player` alone as an FX ownership gate
- one proof callsite representing all FX callers
- global pointer/Hydra mode
- whole-command input-device spoof
- post-Calc/viewmodel root as gameplay fire owner
- render attachment/muzzle matrix as bullet direction owner
- `weaponGrabRotOffset` as gameplay aim calibration
- `IN_ATTACK` as required VR aim provenance

Corrected FX + `LOCAL_PLAYER ∩ WEAPON_CALLER ∩ true [ESP+0C]` remains an emergency fallback/oracle, not the preferred mainline.

## LIVE STATUS

```text
LIVE=NO
```

COLD is not saturated for the server current-command question. Do not build/inject/run a new weapon mutation until this gate is closed or killed.

## PROTECTED BASELINE

Must remain unchanged:

- `CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`
- full HMD 6DoF/stereo world path
- slot250 RIGHT Touch visual hand-follow
- stock Weapon_ShootPosition origin
- stock spread/ammo/recoil semantics
- textures/audio/game stability
- pacing/PVS lane remains separate from weapon/aim work
