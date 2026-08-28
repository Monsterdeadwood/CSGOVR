# CSGOVR

Native VR research for frozen legacy **CS:GO x86** using HLAE/OpenXR and exact retail evidence.

## Canonical truth

**Read `docs/SYNC.md` first.** It is the current synchronized project truth. `RESEARCH_CONTRACT.yaml` and `CLAIM_GRAPH.json` are the machine-readable gate/evidence registry.

Do not infer the current gate from old package names, historical experiment files, or dated handoffs.

## Current status

Weapon / native aim:
- `CLAIM_ID = CSGO-RC3-SAFE161-3HOOK-RUNTIME`
- current candidate: `CSGOVR_ULTIMATE_SAFE161_NATIVE_AIM_RC3_3HOOK_GUARDFIX_ONECLICK.zip`
- RC2 three-hook LIVE is **FAIL / DO NOT PROMOTE**; native aim never installed because its exact runtime guard compared a relocation-sensitive `A1 imm32` against preferred-image bytes under ASLR
- the three-hook `CUserCmd::aimdirection` architecture remains COLD-proven
- RC3 returns to the user-verified SAFE161 visual ancestor and uses the relocation-aware guard

Separate quality lane:
- `CLAIM_ID = CSGO-PACING-PVS`
- do not combine PVS/culling mutation with weapon/aim mutation

## Baseline hierarchy

### World / HMD rollback authority
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256 `5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Evidence: `CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`.

User visual verdict: **VR + full HMD 6DoF works.** Tearing remains a separate cadence/quality defect.

### Weapon / aim visual integration ancestor
`CSGOVR_WEAPON_SAFE_POSTCALC_AIM_v1.6.1_ONECLICK`

Retained user visual evidence:
- distant textures/objects remained present;
- shot sound remained present;
- exact slot250 RIGHT-hand/viewmodel path is retained.

This SAFE161 ancestor does not replace M3.2B as world/HMD rollback authority.

## Current retained gameplay architecture

`RIGHT AIM -> CUserCmd::aimdirection -> stock prediction/command transport -> authoritative server CUserCmd -> player AimDirection -> approved final-aim pointer branch -> native AimPunch -> stock spread/inaccuracy -> stock Weapon_ShootPosition -> stock FX_FireBullets`

Keep semantic owners separate:
`HEAD != HAND_GRIP != HAND_AIM != WEAPON != FIRE_AIM != FIRE_ORIGIN`.

## Start here

1. `docs/SYNC.md`
2. `RESEARCH_CONTRACT.yaml`
3. `CLAIM_GRAPH.json`
4. `experiments/RC3_SAFE161_3HOOK_GUARDFIX_LIVE_TICKET_20260828.md`
5. `research/RC2_3HOOK_RUNTIME_FAIL_20260828.md`
6. `research/SERVER_CURRENTCMD_CLOSURE_20260828.md`
7. `research/NATIVE_AIM_EVIDENCE_20260828.md`
8. `docs/FALSIFIED.md` and historical ledgers only as needed

Prepared package is never a RESULT. User-visible causal verdict outranks healthy plumbing counters for visual acceptance.

**PARALLELIZE EVIDENCE — SERIALIZE MUTATION.**
