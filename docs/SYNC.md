# CSGOVR - STATE SYNC

Snapshot: **2026-08-29 after RC2 failure, RC3 SAFE161 preparation, and exact-package pre-LIVE red-team correction.**

Primary evidence:
- `research/NATIVE_AIM_EVIDENCE_20260828.md`
- `research/SERVER_CURRENTCMD_CLOSURE_20260828.md`
- `research/RC2_3HOOK_RUNTIME_FAIL_20260828.md`
- `experiments/RC3_SAFE161_3HOOK_GUARDFIX_LIVE_TICKET_20260828.md`
- exact RC3 package SHA sidecar retained in Library

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

User visual verdict: **VR + full HMD 6DoF works.** Tearing remains a separate cadence/quality defect.

Closed world/HMD facts must not be reopened without contradictory evidence:
- Quest stereo transport;
- asymmetric projection;
- native camera origin;
- HMD yaw/pitch/roll;
- positional 6DoF.

## SAFE161 WEAPON / AIM VISUAL ANCESTOR

Preferred weapon-integration visual/render ancestor:

`CSGOVR_WEAPON_SAFE_POSTCALC_AIM_v1.6.1_ONECLICK`

Retained user visual evidence:
- distant textures/objects did not disappear;
- shot sound was present.

This does not replace M3.2B as world/HMD rollback authority.

## VIEWMODEL / HAND - CLOSED

Exact retail owner:

```text
CBaseViewModel::CalcViewModelView      = client+0x001B7F80
CPredictedViewModel::CalcViewModelView = client+0x003420E0
runtime slot                            = 250
```

RIGHT Touch hand-follow through the stock arms/viewmodel path = PASS.

Semantic separation remains mandatory:

```text
HEAD != HAND_GRIP != HAND_AIM != WEAPON != FIRE_AIM != FIRE_ORIGIN
```

## CORRECTED FX ABI - RETAIL_PROVEN

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

Global FX mutation is not the preferred architecture. `FX_FireBullets` remains stock in the native-aim mainline.

## NATIVE CUSERCMD AIM OWNERSHIP - COLD PROVEN

Frozen retail `CUserCmd::aimdirection` lives at `+0x18/+0x1C/+0x20` and travels through stock command serialization/network transport.

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

`IN_ATTACK` is not aim provenance. FIRE action and HAND_AIM remain separate owners.

## CLIENT CURRENT COMMAND OWNER - RETAIL_PROVEN

```text
CPrediction::RunCommand = client+0x00343BF0
client+0x00343C0F  player+0x3348 = current CUserCmd*
client+0x00343E73  player+0x3348 = NULL
```

## SERVER CURRENT COMMAND OWNER - RETAIL_PROVEN

Result: `CSGOVR_FX_FIRE_OWNERSHIP_CLOSURE_RESULT_20260828_201156.zip`.

```text
RunCommand              = server+0x00339890
StartCommand            = server+0x00339210
server+0x0033921F       [player+0x1034] = CUserCmd*
server+0x00339E63       [player+0x1034] = NULL
independent reader      = server+0x0033AC6F
```

Therefore:

```text
player+0x1034 = current CUserCmd*
```

## SERVER POINTER GAMEPLAY MAP - RETAIL_PROVEN

Approved final-aim callers and player registers:

```text
58F88B  grenade        ESI
5A7A34  bullet A       EBX
5A8037  bullet B       EDI
5B0349  M3             EDI
5B37BB  NOVA/Sawedoff  EDI
5BC61B  XM1014         EDI
```

Hard rejects:

```text
216DCA  use / interaction trace
49F492  motion-controller recoil scaling
```

Global pointer/Hydra spoofing remains forbidden.

## THREE-HOOK ARCHITECTURE - OWNERSHIP LEVEL COLD PROVEN

Architecture:

```text
HOOK 1 CLIENT CheckSplitScreenMimic
  write normalized RIGHT AIM into the command-owned aim channel
  publish command provenance

HOOK 2 CLIENT IsInputDeviceAPointer
  approved caller
  + local player
  + player+0x3348 currentCmd
  + exact provenance identity
  => TRUE

HOOK 3 SERVER IsInputDeviceAPointer
  approved caller
  + exact caller-specific player register
  + player+0x1034 currentCmd
  + player aim mirror == cmd aim
  + exact provenance identity
  + server player index == ring localPlayerIndex
  => TRUE
```

Server RunCommand detour/TLS capability state is not required by the proven ownership model.

Important correction:

```text
OWNERSHIP / DATAFLOW ARCHITECTURE = COLD_PROVEN
EXACT RC3 IMPLEMENTATION CONTRACT = NOT YET FULLY PROVEN
RC3 RUNTIME                        = UNPROVEN
```

The former generic claim `CSGO-THREE-HOOK-CONTRACT-REDTEAM` is therefore **SUPERSEDED**, not promoted as proof of every concrete RC3 implementation invariant.

## RC2 LIVE 205342 - HARD FAIL / DO NOT PROMOTE

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
SERVER_RUNCOMMAND_HOOK=ABSENT_BY_BUILD
SERVER_TLS=ABSENT_BY_BUILD
```

Repeated log:

```text
NATIVE_AIM_EXACT_GUARD FAIL reason=HEAD_OWNER_OR_SEAM_BYTES mutation=0
```

Native aim never installed. RC2 center-screen fire is stock behavior and does not falsify active `CUserCmd::aimdirection` ownership.

User visual verdict also reports disappearing distant textures/objects. Therefore the RC2 visual ancestor is rejected even though stereo/HMD plumbing counters remained alive.

## RC2 ASLR GUARD ROOT CAUSE - COLD CLOSED

At frozen `server+0x005BC67A`, the player-index sequence contains `A1 imm32`; the operand is a base-relocated preferred-image VA.

Incorrect RC2 behavior:
- compare all live bytes with preferred-image bytes.

Correct behavior:
- compare relocation-stable prefix through opcode `A1`;
- read live operand;
- require `operand == actualServerBase + 0x00A968E0`;
- compare relocation-stable suffix;
- fail closed and identify the exact failing seam.

## PREPARED RC3 CANDIDATE - ON COLD HOLD

Package:
`CSGOVR_ULTIMATE_SAFE161_NATIVE_AIM_RC3_3HOOK_GUARDFIX_ONECLICK.zip`

SHA256:
`B988DEB8DEE490CFB6964ADA004DF69F454AF48A689BDD60E2A67CCE5B72C2FB`

Ticket:
`experiments/RC3_SAFE161_3HOOK_GUARDFIX_LIVE_TICKET_20260828.md`

Intended identity:

```text
VISUAL/RENDER BASE = SAFE161 user-verified lineage
GAMEPLAY DELTA      = 3-hook CUserCmd currentCmd provenance
RUNTIME GUARD       = relocation-aware
RunCommand hook     = ABSENT
native-aim TLS      = ABSENT
FX mutation         = ABSENT
PVS mutation        = ABSENT
```

The ticket reports a structural/package audit, but current accessible GitHub/Library evidence exposes the ticket and SHA sidecar, not the exact RC3 source/package bytes needed to independently prove every fail-closed implementation invariant.

Therefore **prepared RC3 != LIVE-authorized RC3** until the current COLD gate closes.

## CURRENT GATE - RC3 PRE-LIVE IMPLEMENTATION CLOSURE

`CLAIM_ID = CSGO-RC3-PRELIVE-IMPLEMENTATION-CLOSURE`

This gate does **not** reopen retail ownership, fire ABI, current-command ownership, six caller/register cases, or the three-hook architecture choice.

It asks one bounded question:

> Does the exact RC3 package/source actually implement the complete fail-closed contract that the architecture requires?

Exact items to prove on the RC3 package/source:

1. provenance identity includes the required command/session identity, including `sessionEpoch`, command number, tick, canonical aim bits and local-player identity;
2. ring publication/consumption ordering cannot accept a partially published/torn entry;
3. stale entries cannot cross reconnect/map/session boundaries, through explicit epoch reset/increment or an equivalently exact invalidation invariant;
4. signed zero is canonicalized where bitwise aim identity is used, and NaN/Inf is rejected;
5. valid command stamping has no `IN_ATTACK`-only hole that would lose burst/delayed fire ownership;
6. client and server reject paths delegate to stock behavior for wrong caller, wrong player, wrong command, remote/bot, mismatch or invalid data;
7. three-hook installation is behaviorally fail-closed if MinHook creation/enable is only partially successful, or the exact implementation provides an equivalently strong no-partial-VR invariant;
8. relocation-aware `A1` runtime guard is present and the old ASLR-sensitive full-byte comparison is absent;
9. RunCommand/TLS, FX mutation, PVS mutation, DrawModel gameplay mutation and render/muzzle fire ownership are absent from the active RC3 path;
10. protected SAFE161 renderer/HMD/transport lineage remains unchanged.

### COLD PASS

All ten items are proven in the exact RC3 source/package and package identity remains the prepared RC3 artifact. Then:

```text
CSGO-RC3-PRELIVE-IMPLEMENTATION-CLOSURE -> PROVEN
CSGO-RC3-SAFE161-3HOOK-RUNTIME         -> ACTIVE
```

The existing RC3 LIVE ticket can then run without inventing RC4.

### COLD FAIL

Any item is absent, ambiguous, or only described in documentation rather than present in the exact implementation. Fix only that exact implementation defect, re-audit, and keep LIVE blocked.

### WHY THIS IS COLD

All remaining questions in this gate are source/package structure and fail-closed implementation properties. They do not require the game to run.

The exact RC3 ZIP/source is not currently available in GitHub or Library through the accessible evidence surface; only its ticket and SHA sidecar are visible. Therefore no new LIVE is justified from this tab until exact package bytes/source are available for audit or equivalent exact implementation evidence is supplied.

## RC3 LIVE - QUEUED AFTER COLD PASS

Existing LIVE preflight remains valid after the implementation closure:

MISSING FACT: whether the user-verified SAFE161 visual baseline remains intact while the relocation-aware three-hook CUserCmd native-aim path actually activates and owns authoritative shot direction.
WHY NOT COLD: after exact implementation closure, hook installation timing, prediction/network consumption, authoritative impacts and far-world visual stability require execution and the user's eyes.
PASS: SAFE161 far textures/objects and shot audio remain stable; three hooks activate with RunCommand/TLS absent; client publish/client gate/server currentCmd gate occur; impacts follow RIGHT Touch while stock Weapon_ShootPosition, recoil, spread and ammo remain normal.
FAIL: any far-world/texture disappearance, native aim inactive, center-screen authoritative fire, unauthorized pointer grant, currentCmd/ring mismatch, crash/audio regression, or stock recoil/spread/origin/ammo semantics change.
BASELINE MUST HOLD: exact frozen hashes; SAFE161 user-verified textures/audio; M3.2B HMD 6DoF/Quest stereo; exact slot250 RIGHT-hand follow; stock Weapon_ShootPosition/recoil/spread/ammo; no attachment/DrawModel/FX/global-pointer mutation.

## COMMAND PROVENANCE CONTRACT

Required conceptual identity remains:

```text
sessionEpoch
command_number
tick_count
canonical aimBits[3]
localPlayerIndex
sampleId
poseSeq
```

Principles:
- full identity, never modulo slot alone;
- controller freshness is a property of command creation;
- command identity is a property of command consumption;
- reprediction must consume immutable command-owned VR aim;
- remote/bot commands must fail local-player ownership;
- unknown/rejected pointer callers always retain stock behavior.

## QUALITY / PVS LANE - SEPARATE

`CLAIM_ID = CSGO-PACING-PVS`

Do not add PVS/culling mutation to the RC3 implementation closure or weapon/aim LIVE.

If a later SAFE161-based RC3 run still shows disappearing distant content, classify that independently and return to the T14/T15 quality lane.

## STOP RULE

- no RC4 before RC3 produces a RESULT or one exact RC3 implementation defect is proven;
- no new weapon LIVE while `CSGO-RC3-PRELIVE-IMPLEMENTATION-CLOSURE` is ACTIVE;
- do not reopen transport, Quest projection, world camera/HMD ownership, native CUserCmd aim ownership, FX ABI or server currentCmd ownership without contradictory exact evidence;
- do not promote package/ticket intent to implementation proof;
- do not combine PVS/culling and weapon/aim mutations in one causal LIVE;
- prepared package != RESULT.
