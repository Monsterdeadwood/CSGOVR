# CSGOVR - STATE SYNC

Snapshot: **2026-09-08 after executed RC3 SAFE161 three-hook runtime reconciliation.**

Canonical evidence for this delta:
- `research/RC3_RUNTIME_RECONCILE_20260907.md`
- RESULT `CSGOVR_ULTIMATE_SAFE161_NATIVE_AIM_RC3_3HOOK_RESULT_20260907_050436.zip`
- prior ownership evidence `research/NATIVE_AIM_EVIDENCE_20260828.md`
- prior server current-command closure `research/SERVER_CURRENTCMD_CLOSURE_20260828.md`
- prior RC2 failure `research/RC2_3HOOK_RUNTIME_FAIL_20260828.md`
- RC3 prepared ticket `experiments/RC3_SAFE161_3HOOK_GUARDFIX_LIVE_TICKET_20260828.md`

## FROZEN TARGET
- legacy CS:GO x86, AppID `4465480`, BuildID `22100934`
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- `server.dll` SHA256 `699757BB5C8DDB71193F68DABA22F59F697491DDBD768BC8656AC2CE435C4DB8`
- HLAE 2.191.1
- local/offline `-insecure` only

## WORLD / HMD ROLLBACK AUTHORITY - UNCHANGED
Protected baseline anchor:
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256:
`5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Evidence:
`CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`

User visual verdict remains: **VR + full HMD 6DoF works.** Tearing/cadence remains a separate quality defect.

Do not reopen without contradictory exact evidence:
- Quest stereo transport;
- asymmetric projection;
- native camera origin;
- HMD yaw/pitch/roll;
- positional 6DoF.

## SAFE161 WEAPON / AIM VISUAL ANCESTOR
Preferred clean weapon-integration visual/render ancestor remains:
`CSGOVR_WEAPON_SAFE_POSTCALC_AIM_v1.6.1_ONECLICK`

Retained visual evidence:
- distant textures/objects did not disappear;
- shot sound was present.

This is an ancestor/control, not a replacement for M3.2B world/HMD rollback authority.

## VIEWMODEL / HAND - RETAINED
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

`g_weaponGrabRotOffset` is a visual/grip calibration object. It is not promoted as gameplay-fire provenance.

## FX / FIRE ABI - RETAIL_PROVEN
Frozen client/server FX ABI remains:
```text
ECX       = iPlayerIndex
EDX       = weapon object
[ESP+04]  = econ/item view object
[ESP+08]  = vOrigin
[ESP+0C]  = vAngles
cleanup   = 0x2C
```
The older `[ESP+08]=vAngles` interpretation remains FALSIFIED.

Global FX mutation is not the preferred native-aim architecture. Stock FX remains protected.

## CUSERCMD AIM OWNERSHIP - RETAINED
Frozen retail `CUserCmd::aimdirection` lives at `+0x18/+0x1C/+0x20` and travels through stock command serialization/network transport.

Client current-command owner:
```text
CPrediction::RunCommand = client+0x00343BF0
client+0x00343C0F  player+0x3348 = current CUserCmd*
client+0x00343E73  player+0x3348 = NULL
```

Server current-command owner:
```text
RunCommand        = server+0x00339890
StartCommand      = server+0x00339210
server+0x0033921F [player+0x1034] = CUserCmd*
server+0x00339E63 [player+0x1034] = NULL
reader            = server+0x0033AC6F
```
Therefore `player+0x1034 = current CUserCmd*` at the proven lifetime.

Approved frozen final-aim caller/register map:
```text
58F88B  grenade        ESI
5A7A34  bullet A       EBX
5A8037  bullet B       EDI
5B0349  M3             EDI
5B37BB  NOVA/Sawedoff  EDI
5BC61B  XM1014         EDI
```
Hard rejects remain:
```text
216DCA  use / interaction trace
49F492  motion-controller recoil scaling
```

## RC2 - FALSIFIED / ROOT CAUSE RETAINED
`CSGOVR_ULTIMATE_NATIVE_AIM_RC2_3HOOK_RESULT_20260828_205342.zip` remains a HARD FAIL.

RC2 native aim never installed because its exact guard compared relocation-sensitive `A1 imm32` bytes as if the preferred-image operand were runtime-stable under ASLR.

Root cause remains PROVEN. RC2 is not a visual or gameplay ancestor.

## RC3 2026-09-07 EXECUTED RESULT - SPLIT VERDICT
Result:
`CSGOVR_ULTIMATE_SAFE161_NATIVE_AIM_RC3_3HOOK_RESULT_20260907_050436.zip`

Machine/runtime proof:
```text
NATIVE_AIM_EXACT_GUARD=True
NATIVE_AIM_RELOC_GUARD=True
NATIVE_AIM_3HOOK_ACTIVE=True
```

Executed native chain observed:
```text
RIGHT Touch
 -> CUserCmd::aimdirection publication
 -> client currentCmd gate
 -> stock command/prediction transport
 -> server currentCmd gate
 -> authoritative final-aim path
```

Stock gameplay semantics remained in the observed chain:
- `Weapon_ShootPosition()` remained fire-origin owner;
- native AimPunch/recoil remained active;
- stock spread/inaccuracy remained active;
- stock FX fire remained active;
- RunCommand/TLS native-aim ownership is not promoted by this result.

### Runtime evidence scope
Do not generalize the observed corridor to all firearms:
- server PASS observations: **16/16 `BULLET_B_EDI`**;
- client PASS observations: **12/12 callsite `0x006AA5C4`**.

Therefore:
```text
CSGO-RC3-NATIVE-AIM-EXECUTION = PROVEN
ALL_FIREARM_CORRIDORS_RUNTIME_PROVEN = FALSE
```

### Protected visual baseline verdict
The all-up RC3 integration did **not** preserve the protected visual baseline:
- controller/weapon/shot alignment was visibly crooked;
- distant/far-world objects/textures disappeared;
- recoil remained visibly present.

Therefore:
```text
CSGO-RC3-SAFE161-3HOOK-RUNTIME = FALSIFIED AS ALL-UP INTEGRATION BASELINE
NATIVE EXECUTION SUBSTRATE       = PROVEN AT OBSERVED SCOPE
```

Passing plumbing counters do not erase the user-visible baseline failure.

## PRE-LIVE IMPLEMENTATION CLOSURE - SUPERSEDED
`CSGO-RC3-PRELIVE-IMPLEMENTATION-CLOSURE` is no longer an active gate because the exact RC3 package has now executed and produced a runtime RESULT.

Its architecture/package checks remain historical evidence, but runtime activation is no longer unknown. Do not continue `cold/r2/native-aim-implementation` as though RC3 still awaits first execution.

## CURRENT GATES - RC3 AIM ALIGNMENT + FAR-WORLD VISIBILITY

### `CSGO-RC3-AIM-ALIGNMENT` - ACTIVE
State anchor: **ALIGNMENT OPEN AFTER EXECUTION PROOF**

Primary fact:
identify the exact controller-to-authoritative-shot angular misalignment owner after the proven native CUserCmd execution chain is active.

Constraints:
- preserve the proven CUserCmd execution substrate;
- preserve stock `Weapon_ShootPosition`, recoil/AimPunch, spread/inaccuracy and FX;
- do not use `g_weaponGrabRotOffset`, attachment matrices, DrawModel or render-space muzzle transforms as ballistic calibration;
- do not combine a visibility/PVS mutation with the alignment mutation.

Still unknown:
- the first exact basis/space/sign/temporal conversion where RIGHT Touch aim diverges from the authoritative shot direction;
- whether the error is producer-space, command encoding, final-aim interpretation, or a corridor-specific semantic difference.

### `CSGO-PACING-PVS` - ACTIVE
State anchor: **QUALITY / VISIBILITY LANE - SEPARATE FROM AIM**

Primary fact:
close the exact cause of far-world/distant-texture disappearance independently of weapon/aim mutation.

The RC3 result proves the visual symptom occurred while native aim was active; it does **not** prove PVS itself is the cause.

Preserve T14/T15 and prior quality evidence, but do not promote a PVS hypothesis without a causal isolation result.

## OPERATIONAL TAB SEPARATION
Current tab-level coordination:
- TAB1 = AIM / native CUserCmd and fire-corridor provenance;
- TAB2 = FAR WORLD / VISIBILITY negative-control lane;
- TAB3 = ALIGNMENT / BALLISTICS source atlas;
- TAB4 = PACING / XR transport diagnostics;
- TAB5 = META governance / merger / STOP.

TAB numbers do not automatically define R1/R2/R3/R4/R5 role IDs.

META governance branch:
`meta/tab5-governance`

## ACTIVE RESEARCH CONTROL
- `cold/r2/native-aim-implementation` = operationally SUPERSEDED by executed RC3 result; retain as historical COLD lineage, do not continue its old gate.
- `cold/r4/sidecar-ownership` = independent COLD lane; main drift inspected as non-overlapping.
- `state/rc3-safe161-runtime-gate` = post-LIVE state/evidence lineage, not a wholesale merge source.
- accepted RC3 delta returns through claim-scoped promotion only.

## STOP RULES
- do not call the entire RC3 package PROVEN because native execution passed;
- do not call native aim inactive; that was RC2, not RC3;
- do not generalize `BULLET_B_EDI` runtime evidence to all mapped firearm corridors;
- do not reintroduce RunCommand/TLS without a specifically falsified 3-hook invariant;
- do not reintroduce global FX mutation, global pointer mode, DrawModel gameplay mutation or render-space muzzle ownership;
- do not combine aim/alignment and PVS/visibility mutations in one causal LIVE;
- do not reopen M3.2B world/HMD closure without contradictory evidence;
- prepared package, passing counters and implementation intent are not substitutes for scoped RESULT evidence.
