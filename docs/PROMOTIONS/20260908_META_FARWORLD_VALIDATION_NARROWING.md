# META Promotion — FAR_WORLD Validation Narrowing / GLOBAL_STATE rev5

CLAIM_ID: CSGO-PACING-PVS
ROLE: META
TARGET: pending convergence scope only
EVIDENCE_CLASS: COLD_CONVERGENCE
PRIOR_SCOPE: broad FAR_WORLD pending packet with common-cause and visibility candidates
NEW_SCOPE: independent AIM/FAR_WORLD defects lead; common semantic/infrastructure roots demoted; ordinary-world membership chain becomes FIRST_MISSING_EDGE

## Accepted scope-boundary findings
- SAFE161 preserved renderer/HMD/projection/D3D/xr_host lineage into RC3 and was the stable far-world ancestor; asymmetry/PVS correctness issues alone are therefore insufficient as an RC3 regression explanation.
- RC3 client `player+0x382C` write is a rejected AIM-side mutation, but no exact visibility/frustum/PVS ownership edge has been recovered from that field.
- exhaustive frozen `IsInputDeviceAPointer` direct-call census isolates the native-aim allowlist to weapon-final-aim routes; non-allowlisted viewmodel/reticle/recoil/other consumers fall through to stock behavior.
- recovered CheckSplitScreenMimic producer contract calls stock behavior first and then writes command/provenance state; no deliberate split-view/frustum mutation is present.
- exact raw executed RC3 detour ABI remains unverified in current Library access and is preserved as an independent negative-control unknown.

## Model ranking after validation
```text
1 independent AIM + FAR_WORLD defects = LEADING
2 shared infrastructure root = WEAKENED
3 shared semantic root = STRONGLY WEAKENED
```

## New FIRST_MISSING_EDGE
```text
late CViewSetup
 -> CPU camera frustum
 -> portal/area frustum refinement
 -> ClientLeafSystem / CullBox
 -> ordinary renderable membership
```

## No technical promotion
This record does not prove PVS, portal causality, CPU visibility-envelope causality, pixel-visibility causality, the 108-degree envelope, or any FAR_WORLD fix. `CSGO-PACING-PVS` remains PENDING_VALIDATION and LIVE remains blocked.