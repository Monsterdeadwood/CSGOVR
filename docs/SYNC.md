# CSGOVR — STATE SYNC

Snapshot: **2026-08-28 after frozen server current-CUserCmd closure.**

Primary evidence:
- `research/NATIVE_AIM_EVIDENCE_20260828.md`
- `research/SERVER_CURRENTCMD_CLOSURE_20260828.md`

## FROZEN TARGET
- legacy CS:GO x86, AppID `4465480`, BuildID `22100934`
- frozen `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- frozen `server.dll` SHA256 `699757BB5C8DDB71193F68DABA22F59F697491DDBD768BC8656AC2CE435C4DB8`
- HLAE 2.191.1
- local/offline `-insecure` target only

## WORLD / HMD GOLDEN — UNCHANGED
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256 `5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Evidence `CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`.

User visual verdict: VR + full HMD 6DoF works. Tearing remains a separate quality defect.

Do not reopen world transport/projection/origin/HMD ownership while solving weapon aim.

## VIEWMODEL / HAND
- `CBaseViewModel::CalcViewModelView = client+0x001B7F80`
- `CPredictedViewModel::CalcViewModelView = client+0x003420E0`
- runtime slot `250`
- RIGHT Touch hand-follow through stock arms/viewmodel = PASS

Visual weapon ownership stays separate from gameplay fire ownership.

## CORRECTED FX ABI
Frozen retail ABI:

```text
ECX       = iPlayerIndex
EDX       = weapon object
[ESP+04]  = econ/item view object
[ESP+08]  = vOrigin
[ESP+0C]  = vAngles
cleanup   = 0x2C
```

Old Aug-27 ABI labels are FALSIFIED. `client+0x003F6989` is the network `C_TEFireBullets` consumer, so global FX mutation is not the preferred architecture. Corrected FX remains fallback/oracle only.

## NATIVE AIM MAINLINE
Frozen retail proves `CUserCmd::aimdirection` at `+0x18/+0x1C/+0x20` and stock transport:

```text
RIGHT AIM
 -> CUserCmd::aimdirection
 -> WriteUsercmd/network/ReadUsercmd
 -> server current CUserCmd
 -> player AimDirection +14AC/+14B0/+14B4
 -> approved pointer final-aim branch
 -> native AimPunch
 -> stock spread/inaccuracy
 -> stock Weapon_ShootPosition
 -> stock FX_FireBullets
```

`FX_MUTATION_REQUIRED=NO`.

`IN_ATTACK` is not VR aim provenance. Aim ownership and fire action remain distinct semantics inside the same command.

## CLIENT CURRENT COMMAND OWNER — RETAIL_PROVEN

```text
CPrediction::RunCommand = client+0x00343BF0
client+0x00343C0F  player+0x3348 = current CUserCmd*
client+0x00343E73  player+0x3348 = NULL
```

## SERVER CURRENT COMMAND OWNER — RETAIL_PROVEN 2026-08-28
Result: `CSGOVR_FX_FIRE_OWNERSHIP_CLOSURE_RESULT_20260828_201156.zip`.

Frozen `RunCommand = server+0x00339890` establishes:

```text
ESI = player
EDI = CUserCmd*
```

At its StartCommand call:

```text
server+00339A16  EDI = [EBP+0C]   ; cmd
server+00339A1C  push EDI
server+00339A1D  push ESI         ; player
server+00339A35  call [CPlayerMove vtable +0x10]
```

Recovered CPlayerMove vtable slot 4 resolves to `server+0x00339210`.

Exact callee:

```text
server+00339213  EDX = [EBP+08]   ; player
server+00339216  ECX = [EBP+0C]   ; CUserCmd*
server+0033921F  [EDX+0x1034] = ECX
```

Therefore:

```text
player+0x1034 = current CUserCmd*
```

Same RunCommand clears it at:

```text
server+00339E63  [player+0x1034] = NULL
```

Independent frozen reader at `server+0x0033AC6F` reads `player+0x1034` and NULL-checks it.

Evidence class: **RETAIL_PROVEN**.

## SERVER POINTER GAMEPLAY MAP — RETAIL_PROVEN

```text
58F88B  grenade        player = ESI
5A7A34  bullet A       player = EBX
5A8037  bullet B       player = EDI
5B0349  M3             player = EDI
5B37BB  NOVA/Sawedoff  player = EDI
5BC61B  XM1014         player = EDI
```

Hard rejects:

```text
216DCA  use / interaction trace
49F492  motion-controller recoil scaling
```

Global pointer/Hydra spoofing and whole-command motion-controller mode remain forbidden.

## CURRENT PREFERRED IMPLEMENTATION — THREE HOOKS

The former server RunCommand detour/TLS capability token is no longer needed as an identity carrier.

```text
HOOK 1 CLIENT CheckSplitScreenMimic
  after original stock input completion:
  compute normalized RIGHT-AIM direction
  write cmd+18/+1C/+20
  publish exact provenance ring

HOOK 2 CLIENT IsInputDeviceAPointer
  approved client gameplay caller
  + local player
  + player+0x3348 currentCmd
  + exact ring identity
  => TRUE

HOOK 3 SERVER IsInputDeviceAPointer
  exact approved return RVA
  + exact caller-specific player register
  + currentCmd = *(player+0x1034)
  + currentCmd != NULL
  + exact ring identity
  + derived server player index == ring.localPlayerIndex
  => TRUE
```

Everything else delegates to the original helper. `FX_FireBullets` remains stock.

Classification:

```text
3_HOOK_ARCHITECTURE = COLD_PROVEN / READY_FOR_INTEGRATION
4_HOOK_RUNCOMMAND_TLS = SAFE ROLLBACK, NOT PRIMARY
2_HOOK = NO JUSTIFICATION
FX HOOK = EMERGENCY FALLBACK / ORACLE ONLY
```

COLD proof is not a runtime pass. Runtime integration still needs its own preflight and result.

## COMMAND PROVENANCE CONTRACT
Recommended ring record:

```text
sessionEpoch
command_number
tick_count
canonical aimBits[3]
localPlayerIndex
sampleId
poseSeq
```

Rules:
- full identity check, never modulo slot alone
- canonicalize `-0.0f` to `+0.0f`
- reject NaN/Inf
- controller freshness checked only at command creation
- a published valid command keeps its aim ownership despite network delay
- remote/bot commands must fail local-player identity even if command numbers collide
- NULL currentCmd or unknown/rejected pointer caller always falls back to stock behavior

`FRESHNESS = property of command creation`

`IDENTITY = property of command consumption`

## OWNERSHIP MODEL

```text
HEAD        = HMD/world camera
HAND_GRIP   = visual grip space
HAND_AIM    = controller aim space
WEAPON      = stock viewmodel / slot250
FIRE_AIM    = CUserCmd::aimdirection
FIRE_ORIGIN = stock Weapon_ShootPosition
```

Do not route gameplay fire through `weaponGrabRotOffset`, post-Calc viewmodel root, render attachment matrices, or muzzle matrices.

## NEXT PRIMARY GATE
Red-team the exact three-hook implementation contract before LIVE:
- ring serialization equality across client/server command transport
- session/reconnect epoch reset
- exact client/server local-player identity derivation
- six server return-RVA/register dispatch cases
- no mutation of rejected pointer semantics

Only after that COLD audit may a three-hook integration build receive a LIVE preflight.

## QUALITY LANE — SEPARATE
Pacing/PVS remains independent. `CSGOVR_T14_EXTERNAL_RESULT_20260827_143839.zip` is useful infrastructure evidence but does not prove the PVS hypothesis.

## STOP RULE
Do not reopen world stereo/HMD 6DoF.
Do not return to v1.6 render-space muzzle matrix.
Do not mutate fire origin without an independently proven gameplay-space owner.
Do not return to global FX mutation as preferred architecture.
Do not globally spoof pointer/Hydra semantics.
Do not combine pacing/PVS experiments with weapon/aim integration.
Do not reintroduce RunCommand/TLS unless the three-hook integration falsifies a specific ownership invariant.
