# CSGOVR State

## Compatibility entrypoint

**Canonical current state is `GLOBAL_STATE.md`.**

This file exists only for old links/TAB instructions that expect `docs/STATE.md`. Do not maintain an independent full state narrative here and do not let this file override a newer `GLOBAL_REVISION`.

Machine control plane:
- `GLOBAL_STATE.md`
- `RESEARCH_CONTRACT.yaml`
- `CLAIM_GRAPH.json`

## Current promoted gates

Active canonical claims are read from `GLOBAL_STATE.md` and `CLAIM_GRAPH.json`.

At the PROJECT_SYNC_CONTRACT v1.1 migration boundary they are:
```text
CSGO-RC3-AIM-ALIGNMENT = ACTIVE
CSGO-PACING-PVS        = ACTIVE
```

The old pre-LIVE RC3 implementation gate is superseded by the executed 2026-09-07 RESULT. The all-up RC3 SAFE161 integration baseline is falsified, while native CUserCmd execution is proven only at the actually observed runtime scope.

## Baseline hierarchy

World/HMD rollback authority:
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

Weapon/aim visual ancestor:
`CSGOVR_WEAPON_SAFE_POSTCALC_AIM_v1.6.1_ONECLICK`

## Hard routing rule

On resume:
1. read newest `GLOBAL_STATE.md`;
2. record `GLOBAL_REVISION_AT_START`;
3. read `PENDING_SYNC`, active ownership and `CLAIM_GRAPH.json`;
4. reconcile relevant sibling RESULT / STATE / HANDOFF;
5. only then follow the current FIRST_MISSING_EDGE.

`docs/SYNC.md` remains a compatibility/evidence snapshot subordinate to `GLOBAL_STATE.md`.

Prepared package != RESULT. Architecture proof != exact implementation proof. LOCAL_STATE is always lower priority than GLOBAL_STATE.
