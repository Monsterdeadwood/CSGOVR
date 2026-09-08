# CSGOVR Promotion — RC3 Runtime Reconciliation

CLAIM_ID: CSGO-RC3-NATIVE-AIM-EXECUTION
ROLE: META
TARGET: canonical CSGOVR research state
EVIDENCE_CLASS: CAUSAL_LIVE_RESULT_CONVERGENCE
PRIOR_SCOPE: RC3 pre-LIVE implementation closure with runtime still queued
NEW_SCOPE: native execution proven at observed corridor; all-up integration falsified; alignment and visibility remain active separate gates

## Accepted delta
Executed result:
`CSGOVR_ULTIMATE_SAFE161_NATIVE_AIM_RC3_3HOOK_RESULT_20260907_050436.zip`

Promote only the following scoped facts:
- relocation-aware exact guard executed successfully;
- relocation-aware three-hook native CUserCmd aim became active;
- publish -> client currentCmd gate -> server currentCmd gate executed;
- observed authoritative server runtime corridor was `BULLET_B_EDI` (16/16 observed server PASS cases);
- observed client PASS callsite was `0x006AA5C4` (12/12 observed client PASS cases);
- stock `Weapon_ShootPosition`, AimPunch/recoil, spread/inaccuracy and FX semantics remained in the observed chain.

## Falsified promotion scope
The RC3 package is **not** promoted as an all-up SAFE161 integration baseline because the protected visual baseline failed:
- controller/weapon/shot alignment was visibly crooked;
- distant/far-world objects/textures disappeared.

Therefore `CSGO-RC3-SAFE161-3HOOK-RUNTIME` becomes FALSIFIED at the all-up integration scope while the narrower `CSGO-RC3-NATIVE-AIM-EXECUTION` claim becomes PROVEN.

## New active gates
- `CSGO-RC3-AIM-ALIGNMENT`: exact controller-to-authoritative-shot angular misalignment owner after execution proof.
- `CSGO-PACING-PVS`: independent far-world/visibility/pacing causality.

These gates must remain causally separate.

## Superseded gate
`CSGO-RC3-PRELIVE-IMPLEMENTATION-CLOSURE` is SUPERSEDED operationally because RC3 has now executed. The historical implementation evidence remains retained; it is no longer the current gate.

## Protected baseline
No change to M3.2B world/HMD rollback authority or stock fire-origin/recoil/spread/FX semantics.

No runtime code, COLD branch history or state branch is wholesale-merged by this promotion.
