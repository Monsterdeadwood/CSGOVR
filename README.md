# CSGOVR

Native VR research for frozen legacy **CS:GO x86**.

## Canonical truth

**Read `docs/SYNC.md` first.**

Machine control plane:
- `RESEARCH_CONTRACT.yaml`
- `CLAIM_GRAPH.json`

Do not infer the current gate from old package names or historical handoffs.

## Current lanes

Primary weapon/aim COLD gate:
`CLAIM_ID = CSGO-RC3-PRELIVE-IMPLEMENTATION-CLOSURE`

Separate quality lane:
`CLAIM_ID = CSGO-PACING-PVS`

Prepared runtime candidate after COLD PASS:
`CSGO-RC3-SAFE161-3HOOK-RUNTIME` (QUEUED).

## Why RC3 is not authorized for LIVE yet

The three-hook **ownership/dataflow architecture is COLD-proven**, including native `CUserCmd::aimdirection`, client/server current-command ownership and the six approved server final-aim caller/register cases.

What remains is narrower: prove the **exact RC3 package implementation** really contains the required fail-closed ring/session/hook-install/reject behavior. The RC3 ticket reports a structural package audit, but accessible GitHub/Library evidence exposes the ticket and SHA sidecar rather than the exact RC3 source/package bytes needed for an independent implementation audit.

See `docs/SYNC.md` for the ten exact COLD items. Do not create RC4 and do not run RC3 from this tab before that gate closes.

## Baseline hierarchy

World/HMD rollback authority:
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

Weapon/aim visual ancestor:
`CSGOVR_WEAPON_SAFE_POSTCALC_AIM_v1.6.1_ONECLICK`

RC2 three-hook LIVE is **FAIL / DO NOT PROMOTE**. Its native aim never installed due to the ASLR-sensitive old guard, and its visual parent regressed distant world content.

## Retained gameplay architecture

`RIGHT AIM -> CUserCmd::aimdirection -> stock prediction/transport -> authoritative server CUserCmd -> player AimDirection -> approved final-aim semantics -> native AimPunch -> stock spread/inaccuracy -> stock Weapon_ShootPosition -> stock FX_FireBullets`

Semantic owners remain separate:
`HEAD != HAND_GRIP != HAND_AIM != WEAPON != FIRE_AIM != FIRE_ORIGIN`.

## Start here

1. `docs/SYNC.md`
2. `RESEARCH_CONTRACT.yaml`
3. `CLAIM_GRAPH.json`
4. Issue #4 (`CURRENT:`)
5. `experiments/RC3_SAFE161_3HOOK_GUARDFIX_LIVE_TICKET_20260828.md` as the **queued** runtime ticket
6. `research/RC2_3HOOK_RUNTIME_FAIL_20260828.md`
7. `research/SERVER_CURRENTCMD_CLOSURE_20260828.md`
8. `research/NATIVE_AIM_EVIDENCE_20260828.md`

Prepared package is never a RESULT. Architecture proof is not automatically exact implementation proof.
