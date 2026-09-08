# CSGOVR — RC3 SAFE161 native-aim runtime RESULT

DATE = 2026-09-07
ARTIFACT = `CSGOVR_ULTIMATE_SAFE161_NATIVE_AIM_RC3_3HOOK_RESULT_20260907_050436.zip`
CLAIM_SCOPE = relocation-aware 3-hook CUserCmd native-aim execution + protected SAFE161 visual baseline
VERDICT = FACT_PASS + BASELINE_FAIL

## Machine/runtime facts — PROVEN in this RESULT

```text
NATIVE_AIM_EXACT_GUARD=True
NATIVE_AIM_RELOC_GUARD=True
NATIVE_AIM_3HOOK_ACTIVE=True
```

Runtime publication/consumption was observed across the intended native chain:

```text
RIGHT Touch
  -> CUserCmd::aimdirection publish
  -> client current-command gate
  -> stock command transport / prediction
  -> server current-command gate
  -> authoritative fire path
```

Stock gameplay semantics remained in the chain:
- `Weapon_ShootPosition()` remains fire-origin owner;
- native AimPunch/recoil remains active;
- stock spread/inaccuracy remains active;
- stock FX fire path remains active;
- no RunCommand/TLS native-aim ownership is promoted from this RESULT.

## Exact runtime scope boundary

Do not generalize one observed corridor to all firearms.

- server PASS observations: **16/16 were `BULLET_B_EDI`**;
- client PASS observations: **12/12 were callsite `0x006AA5C4`**;
- other mapped firearm corridors remain exact-retail COLD-supported only until separately executed.

Therefore:

```text
RC3_NATIVE_AIM_EXECUTION = PROVEN
CUSERCMD_PUBLISH_TO_CLIENT_GATE_TO_SERVER_CURRENTCMD_GATE = PROVEN_EXECUTION
ALL_FIREARM_CORRIDORS_RUNTIME_PROVEN = FALSE
```

## User-visible baseline — FAIL

The protected visual baseline did not hold:
- shots/weapon alignment was visibly crooked;
- distant/far world content and textures disappeared;
- recoil was visibly present.

The crooked-aim symptom is not permission to rotate gameplay aim with the viewmodel grab calibration. Existing evidence separates visual grip/model space from gameplay aim space; `g_weaponGrabRotOffset` remains graphics-only and must not be injected into native ballistic provenance.

The far-world disappearance is a separate visibility/culling/transport-quality failure. Its causal owner is not proven by this combined RESULT.

Therefore:

```text
RC3_AS_PROMOTABLE_SAFE161_INTEGRATION = FALSIFIED
FINAL_AIM_ALIGNMENT = FAIL / OPEN
FAR_WORLD_CAUSALITY = UNKNOWN
M3.2B_WORLD_HMD_GOLDEN = UNCHANGED ROLLBACK AUTHORITY
```

## STOP / DO-NOT-REPEAT
- Do not call the whole RC3 package PROVEN because native-aim counters passed.
- Do not call native aim inactive; that was RC2, not this RC3 RESULT.
- Do not use `g_weaponGrabRotOffset`, attachment matrices, DrawModel, or render-space muzzle transforms as gameplay-fire correction.
- Do not combine a new PVS/culling mutation with aim-corridor mutation in one causal LIVE.
- Do not promote other firearm corridors from structural evidence to runtime proof.

## Reuse consequence
The next work must REUSE the verified native CUserCmd ownership chain and isolate the remaining alignment and visibility facts. The proven 3-hook execution substrate is evidence, while the RC3 visual/integration baseline is not a release ancestor.
