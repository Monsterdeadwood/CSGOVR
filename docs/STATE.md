# CSGOVR State

## Compatibility entrypoint

**Canonical current state is `docs/SYNC.md`.**

This file remains only so old links and TAB instructions that expect `docs/STATE.md` do not enter a stale research epoch. Do not maintain a second full state narrative here.

Machine control plane:
- `RESEARCH_CONTRACT.yaml`
- `CLAIM_GRAPH.json`

## Current machine lanes

Weapon / native aim:
`CLAIM_ID = CSGO-RC3-SAFE161-3HOOK-RUNTIME`

Separate quality:
`CLAIM_ID = CSGO-PACING-PVS`

## Current weapon gate

RC2 three-hook LIVE is **FAIL / DO NOT PROMOTE**. Native aim did not install because the old exact runtime guard compared relocation-sensitive `A1 imm32` bytes at the server player-index seam against preferred-image bytes under ASLR.

The root cause is COLD closed. The three-hook `CUserCmd::aimdirection` architecture itself remains COLD-proven.

Current LIVE candidate:
`CSGOVR_ULTIMATE_SAFE161_NATIVE_AIM_RC3_3HOOK_GUARDFIX_ONECLICK.zip`

SHA256:
`B988DEB8DEE490CFB6964ADA004DF69F454AF48A689BDD60E2A67CCE5B72C2FB`

Visual/render ancestor:
`CSGOVR_WEAPON_SAFE_POSTCALC_AIM_v1.6.1_ONECLICK`

Exact current preflight, PASS/FAIL rules, ownership model and stop rules live in `docs/SYNC.md` and `experiments/RC3_SAFE161_3HOOK_GUARDFIX_LIVE_TICKET_20260828.md`.

## Baseline hierarchy

World/HMD rollback authority:
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

Weapon/aim visual integration ancestor:
`CSGOVR_WEAPON_SAFE_POSTCALC_AIM_v1.6.1_ONECLICK`

Do not confuse these roles.

## Hard routing rule

On resume:
1. read `docs/SYNC.md`;
2. read `RESEARCH_CONTRACT.yaml` and `CLAIM_GRAPH.json`;
3. follow only their active claim IDs;
4. treat this file as a compatibility pointer, not independent authority.

Prepared package != RESULT. Never revive v1.7 barrel-axis, T0 transport, old projection, muzzle-matrix, DrawModel, or global FX work as the current gate without contradictory exact evidence.
