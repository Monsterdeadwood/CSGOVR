# CSGOVR State

## Compatibility entrypoint

**Canonical current state is `docs/SYNC.md`.**

This file exists only for old links/TAB instructions that expect `docs/STATE.md`. Do not maintain an independent full state narrative here.

Machine control plane:
- `RESEARCH_CONTRACT.yaml`
- `CLAIM_GRAPH.json`

## Current machine lanes

Primary COLD gate:
`CLAIM_ID = CSGO-RC3-PRELIVE-IMPLEMENTATION-CLOSURE`

Separate quality:
`CLAIM_ID = CSGO-PACING-PVS`

Queued runtime candidate:
`CSGO-RC3-SAFE161-3HOOK-RUNTIME`.

## Current gate

Before RC3 LIVE, prove the exact RC3 package/source implements the required fail-closed provenance/session/install contract. The ownership/dataflow architecture itself remains COLD-proven; this gate is only exact implementation closure.

See `docs/SYNC.md` for the ten exact PASS items and STOP rule.

RC3 package remains prepared:
`CSGOVR_ULTIMATE_SAFE161_NATIVE_AIM_RC3_3HOOK_GUARDFIX_ONECLICK.zip`

SHA256:
`B988DEB8DEE490CFB6964ADA004DF69F454AF48A689BDD60E2A67CCE5B72C2FB`

It is **not LIVE-authorized from this state until the COLD gate passes**.

## Baseline hierarchy

World/HMD rollback authority:
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

Weapon/aim visual ancestor:
`CSGOVR_WEAPON_SAFE_POSTCALC_AIM_v1.6.1_ONECLICK`

## Hard routing rule

On resume:
1. read `docs/SYNC.md`;
2. read `RESEARCH_CONTRACT.yaml` and `CLAIM_GRAPH.json`;
3. follow only their active claim IDs;
4. treat this file as a compatibility pointer.

Prepared package != RESULT. Architecture proof != exact implementation proof.
