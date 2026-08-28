# CSGOVR — STATE SYNC

Snapshot: **2026-08-28 convergence through frozen native `CUserCmd::aimdirection` evidence.**

Primary evidence delta:
`research/NATIVE_AIM_EVIDENCE_20260828.md`

## FROZEN TARGET
- legacy CS:GO x86, AppID `4465480`, BuildID `22100934`
- frozen `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- frozen `server.dll` SHA256 `699757BB5C8DDB71193F68DABA22F59F697491DDBD768BC8656AC2CE435C4DB8`
- HLAE 2.191.1
- pass1 LEFT / pass2 RIGHT / pass3 monitor lineage retained
- full `CViewSetup = 0x198`
- `-insecure`, local/offline only

## WORLD / HMD GOLDEN — UNCHANGED
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256:
`5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Evidence:
`CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`

User visual verdict: **VR + full HMD 6DoF works.**

Tearing remains a quality defect.

## CLOSED WORLD / HMD
- MASTER1 transport = PASS
- MASTER2 Quest projection = PASS
- MASTER3.1 native origin authority at `client+0x003A5280` = PASS
- MASTER3.2A HMD rotation = PASS
- MASTER3.2B positional translation / full 6DoF = PASS

Do not reopen these while solving weapon aim.

## VIEWMODEL / HAND SYNC
Exact retail owner:
- `CBaseViewModel::CalcViewModelView = client+0x001B7F80`
- `CPredictedViewModel::CalcViewModelView = client+0x003420E0`
- runtime slot `250`

RIGHT Touch hand-follow through the stock arms/viewmodel path = PASS.
Dynamic slot scan `0..239` = STOPPED.
DrawModel transform mutation = STOPPED as the active route.

Visual ownership remains separate from gameplay fire ownership.

## FIRE ABI — CORRECTED 2026-08-28
The prior Aug 27 ABI labels are superseded.

Frozen caller producer + callee consumer analysis closes:

```text
ECX       = iPlayerIndex
EDX       = weapon object
[ESP+04]  = econ/item view object
[ESP+08]  = vOrigin
[ESP+0C]  = vAngles
cleanup   = 0x2C
client/server symmetric
```

FALSIFIED:
- `EDX = item definition index`
- `[ESP+04] = vOrigin`
- `[ESP+08] = vAngles`
- one proof FX callsite represents all fire producers
- freshness alone proves FX ownership

`client+0x003F6989` is the network `C_TEFireBullets` consumer call, not a unique local weapon producer. A global FX mutation can therefore double-apply a locally-originated authoritative shot after the network fire event returns to the client.

Corrected FX + exact local weapon ownership remains only an emergency fallback/oracle.

## NATIVE CUserCmd AIM — CURRENT MAINLINE
Frozen retail contains:

```text
CUserCmd::aimdirection
+0x18
+0x1C
+0x20
```

Frozen transport is proven:

```text
CLIENT CUserCmd aimdirection
    -> WriteUsercmd
    -> network
    -> SERVER ReadUsercmd
    -> SERVER CUserCmd +18/+1C/+20
```

Server command processing then copies:

```text
CUserCmd +18/+1C/+20
    -> player +14AC/+14B0/+14B4
```

The same player aim vector is consumed by the approved pointer-input final-aim path before stock fire, then combined with native aim punch.

Preferred gameplay chain:

```text
RIGHT AIM
    -> CUserCmd::aimdirection
    -> native client prediction
    -> native network
    -> native server command
    -> native final aim
    -> native AimPunch
    -> stock spread/inaccuracy
    -> stock Weapon_ShootPosition
    -> stock FX_FireBullets
```

`FX_MUTATION_REQUIRED=NO` for the preferred architecture.

## CLIENT PREDICTION OWNER
Corrected frozen RVA:

```text
CPrediction::RunCommand = client+0x00343BF0
```

The earlier `client+0x00342BF0` value is FALSIFIED.

Frozen command lifetime:

```text
client+0x00343C0F
player+0x3348 = current CUserCmd*

client+0x00343E73
player+0x3348 = NULL
```

This exact current-command pointer is the client prediction identity gate.

## POINTER-SEMANTICS CENSUS
Machine COLD self-test:

```text
NATIVE_AIM_STATIC_SELFTEST=PASS
CLIENT_POINTER_CALLERS=14
SERVER_POINTER_CALLERS=8
CLIENT_GAMEPLAY_AIM_ALLOW=6
SERVER_GAMEPLAY_AIM_ALLOW=6
```

Server allowlist:

```text
58F88B  grenade final aim
5A7A34  bullet
5A8037  bullet
5B0349  M3
5B37BB  NOVA/Sawedoff
5BC61B  XM1014
```

Server rejects:

```text
216DCA  use / interaction trace
49F492  motion-controller recoil scaling
```

Rule:

```text
POINTER SEMANTICS
ONLY FOR APPROVED FINAL WEAPON/GRENADE AIM CONSUMERS
```

Global pointer/Hydra mode and whole-command input-device spoof are STOPPED because they alter unrelated semantics, including recoil scaling.

## OWNERSHIP MODEL
Keep these authorities separate:

```text
HEAD        = HMD/world camera
HAND_GRIP   = visual grip space
HAND_AIM    = controller aim space
WEAPON      = stock viewmodel / slot250 visual owner
FIRE_AIM    = CUserCmd::aimdirection
FIRE_ORIGIN = stock Weapon_ShootPosition
```

Do not route gameplay fire through `weaponGrabRotOffset`, post-Calc viewmodel root, render attachments, or muzzle matrices.

`IN_ATTACK` is not required for VR aim provenance. Aim and fire action may share one `CUserCmd` while remaining separate semantic owners.

## COMMAND PROVENANCE
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

Rules:
- verify full identity, never modulo-slot identity alone
- canonicalize `-0.0f` to `+0.0f`
- reject NaN / Inf
- controller freshness is checked at command creation only
- a validly published command remains VR-owned despite later network delay
- remote/bot server commands must fail local-player identity even if command numbers collide

```text
FRESHNESS = property of command creation
IDENTITY  = property of command consumption
```

## CURRENT IMPLEMENTATION SHAPE
Conservative architecture currently requires four hooks:

```text
1 CLIENT CheckSplitScreenMimic
  write cmd aimdirection + publish provenance

2 CLIENT IsInputDeviceAPointer
  approved caller + local player + player+0x3348 currentCmd + exact ring

3 SERVER CPlayerMove::RunCommand
  local player + exact ring -> temporary command capability token

4 SERVER IsInputDeviceAPointer
  capability token + approved gameplay caller -> TRUE
```

`FX_FireBullets` stays stock.

## CURRENT GATES
### Weapon primary gate
**COLD close or kill server current-command ownership so the four-hook design can be reduced to three without weakening fail-closed provenance.**

MISSING FACT:
- exact frozen server current `CUserCmd*` field/owner, if present
- exact set/clear lifetime around authoritative command execution
- player-register map at all six approved server pointer-helper consumers
- whether server pointer helper can locally prove full ring + player ownership without the RunCommand/TLS hook

PASS:
- frozen server exposes a stable current-command owner at all required consumers
- exact command and player identity can be reconstructed locally
- Hook 3/TLS can be removed with no weaker gate

FAIL:
- no stable current-command owner
- lifetime does not cover final-aim consumers
- player/currentCmd recovery is ambiguous
- any reduction becomes probabilistic or broadens pointer semantics

FAIL policy:
retain the proven four-hook contract. Do not optimize hook count for aesthetics.

### Quality gate — separate
Pacing/PVS remains independent from weapon/aim work.

`CSGOVR_T14_EXTERNAL_RESULT_20260827_143839.zip` remains useful infrastructure evidence but does not prove the PVS hypothesis.

Prepared/unrun unless a later RESULT exists:
- `CSGOVR_T15_PACING_CLEAN_AB_v1.3.0_120S_ONECLICK.zip`
- `CSGOVR_COLD_SLOT249_251_LAG_OWNER_v3.1.1.zip`

## MUZZLE / RENDER-SPACE STOPPED PATHS
v1.5 muzzle-origin attempt = partial / origin FAIL.

v1.6 FINAL MUZZLE MATRIX = STOPPED integration path:
- matrix lookup worked
- textures/audio regressed
- fire calls diverged
- game stability regressed

v1.6.1 restored texture/audio stability after removing attachment/matrix origin mutation, but absolute post-Calc root angle remained visually wrong.

Do not patch this line forward.

## STOP RULE
Do not reopen world transport/projection/origin/HMD 6DoF.
Do not return to v1.6 render-space muzzle matrix.
Do not mutate fire origin without an independently proven gameplay-space owner.
Do not return to global FX mutation as preferred architecture.
Do not combine intrusive pacing/PVS work with weapon/aim mutation.
Do not create LIVE/build/injection until the current server current-command COLD gate is closed or killed.

Canonical evidence:
- `research/NATIVE_AIM_EVIDENCE_20260828.md`
- `research/CROSS_TAB_SYNC_20260827.md` for prior baseline history only
