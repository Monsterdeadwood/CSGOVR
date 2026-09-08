# CSGOVR — RC3 runtime reconciliation

RESEARCH_DOC_CLASS: CONVERGENCE
EVIDENCE_ID: CSGO-EV-RC3-RUNTIME-20260907
CLAIM_ID: CSGO-RC3-NATIVE-AIM-EXECUTION
DATE: 2026-09-07
RESULT: `CSGOVR_ULTIMATE_SAFE161_NATIVE_AIM_RC3_3HOOK_RESULT_20260907_050436.zip`
PROMOTION_SCOPE: accepted runtime facts only; no wholesale merge of experiment/state branch

## PROVEN from executed RESULT
- relocation-aware exact guard installed successfully;
- relocation-aware three-hook native CUserCmd aim became active;
- command publication -> client currentCmd gate -> server currentCmd gate executed;
- stock `Weapon_ShootPosition`, AimPunch/recoil, spread/inaccuracy and FX fire semantics remained in the observed chain;
- the actually observed authoritative server corridor was `BULLET_B_EDI` (16/16 observed server PASS cases);
- the actually observed client PASS callsite was `0x006AA5C4` (12/12 observed client PASS cases).

Machine status includes:
```text
NATIVE_AIM_EXACT_GUARD=True
NATIVE_AIM_RELOC_GUARD=True
NATIVE_AIM_3HOOK_ACTIVE=True
```

## FALSIFIED as an all-up integration result
The protected SAFE161 visual baseline did not hold:
- controller/weapon/shot alignment was visibly crooked;
- distant/far world content and textures disappeared.

Therefore the whole RC3 package is not a promotable release/integration baseline even though the native execution substrate passed.

## STILL UNKNOWN
1. exact controller -> authoritative-shot angular misalignment source after native CUserCmd execution is active;
2. exact cause of far-world/distant-texture disappearance;
3. runtime proof for structurally mapped firearm corridors other than the observed `BULLET_B_EDI` path.

## Protected boundaries
- M3.2B remains world/HMD rollback authority;
- do not use `g_weaponGrabRotOffset`, attachment matrices, DrawModel or render-space muzzle transforms as gameplay-fire correction;
- do not combine new aim mutation with PVS/culling mutation in one causal LIVE;
- do not generalize one observed firearm corridor to all weapons.

## Promotion verdict
```text
RC3_NATIVE_AIM_EXECUTION = PROVEN
RC3_ALL_UP_SAFE161_INTEGRATION = FALSIFIED
FINAL_AIM_ALIGNMENT = ACTIVE / UNKNOWN OWNER
FAR_WORLD_CAUSALITY = ACTIVE / UNKNOWN OWNER
```
