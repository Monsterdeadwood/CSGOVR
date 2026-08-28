# CSGOVR - STATE SYNC

Snapshot: **2026-08-28 after RC2 three-hook LIVE failure and RC3 SAFE161 guard-fix preparation.**

Primary evidence:
- `research/NATIVE_AIM_EVIDENCE_20260828.md`
- `research/SERVER_CURRENTCMD_CLOSURE_20260828.md`
- `research/RC2_3HOOK_RUNTIME_FAIL_20260828.md`
- `experiments/RC3_SAFE161_3HOOK_GUARDFIX_LIVE_TICKET_20260828.md`

## FROZEN TARGET

- legacy CS:GO x86, AppID `4465480`, BuildID `22100934`
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- `server.dll` SHA256 `699757BB5C8DDB71193F68DABA22F59F697491DDBD768BC8656AC2CE435C4DB8`
- HLAE 2.191.1
- local/offline `-insecure` only

## WORLD / HMD ROLLBACK AUTHORITY

`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256 `5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Evidence: `CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`.

User visual verdict: VR + full HMD 6DoF works. Tearing remains a separate quality defect.

## SAFE161 VISUAL INTEGRATION BASE

For weapon/aim integration, the preferred visual/render ancestor is now:

`CSGOVR_WEAPON_SAFE_POSTCALC_AIM_v1.6.1_ONECLICK`

Retained user visual evidence:
- distant textures did not disappear;
- shot sound was present.

This does not replace M3.2B as rollback authority. It is the preferred weapon-integration renderer/slot250 ancestor because the later RC2 parent regressed the user-visible far world.

## VIEWMODEL / HAND

Exact retail owner:

```text
CBaseViewModel::CalcViewModelView = client+0x001B7F80
CPredictedViewModel::CalcViewModelView = client+0x003420E0
runtime slot = 250
```

RIGHT Touch hand-follow through the stock arms/viewmodel path = PASS.

Visual weapon ownership remains separate from gameplay fire ownership.

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

The older `[ESP+08]=vAngles` interpretation is FALSIFIED.

Global FX mutation is not the preferred architecture because the client FX path includes network/replay consumption. `FX_FireBullets` remains stock in the native-aim mainline.

## NATIVE CUSERCMD AIM - COLD PROVEN

Frozen retail contains `CUserCmd::aimdirection` at `+0x18/+0x1C/+0x20` and transports it through stock `WriteUsercmd` / network / `ReadUsercmd`.

Frozen server command processing copies:

```text
cmd+18/+1C/+20 -> player+14AC/+14B0/+14B4
```

Preferred semantic chain:

```text
RIGHT AIM
 -> CUserCmd::aimdirection
 -> stock client prediction
 -> stock command transport
 -> authoritative server CUserCmd
 -> player AimDirection
 -> approved pointer final-aim branch
 -> native AimPunch
 -> stock spread/inaccuracy
 -> stock Weapon_ShootPosition
 -> stock FX_FireBullets
```

`IN_ATTACK` is not aim provenance. FIRE action and HAND_AIM remain separate semantic owners.

## CLIENT CURRENT COMMAND OWNER - RETAIL_PROVEN

```text
CPrediction::RunCommand = client+0x00343BF0
client+0x00343C0F  player+0x3348 = current CUserCmd*
client+0x00343E73  player+0x3348 = NULL
```

## SERVER CURRENT COMMAND OWNER - RETAIL_PROVEN

Result: `CSGOVR_FX_FIRE_OWNERSHIP_CLOSURE_RESULT_20260828_201156.zip`.

Frozen owner lifetime:

```text
RunCommand = server+0x00339890
StartCommand = server+0x00339210
server+0033921F  [player+0x1034] = CUserCmd*
server+00339E63  [player+0x1034] = NULL
```

Independent frozen reader at `server+0x0033AC6F` reads `player+0x1034` and NULL-checks it.

Therefore:

```text
player+0x1034 = current CUserCmd*
```

Evidence class: RETAIL_PROVEN.

## SERVER POINTER GAMEPLAY MAP - RETAIL_PROVEN

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

Global pointer/Hydra spoofing remains forbidden.

## THREE-HOOK ARCHITECTURE - COLD PROVEN, LIVE NOT YET PROVEN

```text
HOOK 1 CLIENT CheckSplitScreenMimic
  write normalized RIGHT AIM to current CUserCmd
  publish exact command provenance

HOOK 2 CLIENT IsInputDeviceAPointer
  approved caller
  + local player
  + player+0x3348 currentCmd
  + exact ring identity
  => TRUE

HOOK 3 SERVER IsInputDeviceAPointer
  approved caller
  + exact caller-specific player register
  + player+0x1034 currentCmd
  + player+0x14AC aim mirror == cmd aim
  + exact ring identity
  + server player index == ring localPlayerIndex
  => TRUE
```

Server RunCommand detour/TLS capability state is not part of the preferred design.

```text
3_HOOK_ARCHITECTURE = COLD_PROVEN
3_HOOK_RUNTIME       = UNPROVEN
4_HOOK_RUNCOMMAND_TLS = ROLLBACK_ONLY
2_HOOK               = NO_JUSTIFICATION
FX_HOOK              = EMERGENCY_FALLBACK_OR_ORACLE_ONLY
```

## RC2 LIVE 205342 - HARD FAIL

Result: `CSGOVR_ULTIMATE_NATIVE_AIM_RC2_3HOOK_RESULT_20260828_205342.zip`.

Machine evidence:

```text
NATIVE_AIM_EXACT_GUARD=False
NATIVE_AIM_3HOOK_ACTIVE=False
NATIVE_AIM_BASELINE_GATE=False
NATIVE_AIM_PRODUCER_ARMED=False
NATIVE_AIM_PUBLISH_SEEN=False
NATIVE_AIM_CLIENT_GATE_SEEN=False
NATIVE_AIM_SERVER_GATE_SEEN=False
```

Repeated runtime log:

```text
NATIVE_AIM_EXACT_GUARD FAIL reason=HEAD_OWNER_OR_SEAM_BYTES mutation=0
```

Therefore native aim never installed. The center-screen shot direction in RC2 was stock behavior and is not evidence that active `CUserCmd::aimdirection` points at screen center.

User visual verdict also reports disappearing distant textures/objects. That is a baseline FAIL even though automated stereo/HMD plumbing counters were alive.

RC2 must not be promoted or patched forward as the visual ancestor.

## RC2 GUARD ROOT CAUSE - COLD CLOSED

At frozen `server+0x005BC67A`, the player-index formula includes `A1 imm32`. The operand is an absolute preferred-image VA and has a base relocation.

Incorrect RC2 live check:
- compare the full runtime bytes against on-disk preferred-image bytes.

Correct RC3 live check:
- compare relocation-stable prefix through opcode `A1`;
- read live operand;
- require `operand == actual server module base + 0x00A968E0`;
- compare relocation-stable suffix;
- report the exact failing seam.

This explains why the exact frozen DLL passed offline static verification but failed the old runtime full-byte guard under ASLR.

## CURRENT LIVE CANDIDATE

`CSGOVR_ULTIMATE_SAFE161_NATIVE_AIM_RC3_3HOOK_GUARDFIX_ONECLICK.zip`

SHA256:
`B988DEB8DEE490CFB6964ADA004DF69F454AF48A689BDD60E2A67CCE5B72C2FB`

Identity:

```text
VISUAL/RENDER BASE = SAFE161 user-verified lineage
GAMEPLAY DELTA      = 3-hook CUserCmd currentCmd provenance
RUNTIME GUARD       = relocation-aware
RunCommand hook     = ABSENT
native-aim TLS      = ABSENT
FX mutation         = ABSENT
PVS mutation        = ABSENT
```

Protected SAFE161 HMD/render/transport functions are function-for-function identical in the package audit. Slot250 gameplay-transform behavior is unchanged; only telemetry was made truthful about whether native aim is actually active.

## RC3 LIVE PREFLIGHT

MISSING FACT: whether the user-verified SAFE161 visual baseline remains intact while the relocation-aware three-hook CUserCmd native-aim path actually activates and owns authoritative shot direction.
WHY NOT COLD: the ASLR guard, currentCmd owner, register map and hook topology are cold-verifiable, but actual hook installation, prediction/network consumption, authoritative impacts and far-world visual stability require execution and the user's eyes.
PASS: SAFE161 far textures/objects and shot audio remain stable; three hooks activate with RunCommand/TLS absent; client publish/client gate/server currentCmd gate occur; impacts follow RIGHT Touch while stock Weapon_ShootPosition, recoil, spread and ammo remain normal.
FAIL: any far-world/texture disappearance, native aim inactive, center-screen authoritative fire, unauthorized pointer grant, currentCmd/ring mismatch, crash/audio regression, or stock recoil/spread/origin/ammo semantics change.
BASELINE MUST HOLD: exact frozen hashes; SAFE161 user-verified textures/audio; M3.2B HMD 6DoF/Quest stereo; exact slot250 RIGHT-hand follow; stock Weapon_ShootPosition/recoil/spread/ammo; no attachment/DrawModel/FX/global-pointer mutation.

## COMMAND PROVENANCE CONTRACT

Ring identity:

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
- full identity, never modulo slot alone;
- canonicalize signed zero;
- reject NaN/Inf;
- controller freshness belongs to command creation;
- command identity belongs to command consumption;
- remote/bot commands must fail local-player identity;
- unknown/rejected pointer callers always use stock behavior.

## OWNERSHIP MODEL

```text
HEAD        = HMD/world camera
HAND_GRIP   = visual grip space
HAND_AIM    = controller aim space
WEAPON      = stock viewmodel / slot250
FIRE_AIM    = CUserCmd::aimdirection
FIRE_ORIGIN = stock Weapon_ShootPosition
```

Do not route gameplay fire through render attachments, muzzle matrices, DrawModel, post-Calc viewmodel root, or `weaponGrabRotOffset`.

## QUALITY / PVS LANE - SEPARATE

Far-world disappearance is now a protected visual baseline check in RC3, not a second mutation.

If the SAFE161-based RC3 still shows disappearing distant world content, classify that independently as a culling/PVS failure and return to the T14/T15 quality lane. Do not combine a PVS mutation with weapon/aim integration.

## STOP RULE

Do not promote RC2.
Do not call machine stereo/HMD counters a visual PASS when the user reports a visible regression.
Do not print or interpret gameplay aim as active unless the exact hook PASS is present.
Do not reintroduce RunCommand/TLS without a specific falsified 3-hook invariant.
Do not reactivate global FX mutation, global pointer mode, DrawModel gameplay mutation, or render-space muzzle ownership.
Do not combine PVS/culling and weapon/aim mutations in one causal LIVE.
