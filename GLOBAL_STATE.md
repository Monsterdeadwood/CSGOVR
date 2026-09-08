# CSGOVR — GLOBAL STATE

PROJECT_SYNC_CONTRACT = v1.1
SOURCE_OF_TRUTH = GLOBAL_STATE.md
GLOBAL_REVISION = 4
GLOBAL_TIMESTAMP = 2026-09-08T15:13:58Z
PREVIOUS_GLOBAL_REVISION = 3
GLOBAL_REVISION_BASE_MAIN_SHA = 27ec4162bee5d04d6d77768f51fa1bea680e56d5
LOCAL_STATE_PRIORITY = LOWER_THAN_GLOBAL

## Revision 4 reason
Revision 3 quarantined stale R4 branch state. Revision 4 creates the first formal PROJECT_SYNC_CONTRACT v1.1 claim-scoped `PENDING_SYNC` packet for the unresolved FAR_WORLD lane without silently promoting Library/chat COLD conclusions.

The packet preserves candidate evidence, requests COLD validation, keeps technical ownership UNBOUND, and blocks LIVE while pending. AIM alignment remains causally separate and ACTIVE.

## PENDING_SYNC
Ordered set of formal unpromoted HANDOFF / SYNC_PACKET entries:

```text
1. research/FARWORLD_PENDING_SYNC_20260908.md
   EVIDENCE_ID = CSGO-EV-FARWORLD-PENDING-20260908
   CLAIM_ID = CSGO-PACING-PVS
   FACT_STATUS = CANDIDATE / PENDING_VALIDATION
   OWNER = UNBOUND
   SOURCE_GLOBAL_REVISION = 3
```

This packet is unpromoted evidence. It must be validated, falsified/rejected, or converged before removal from `PENDING_SYNC`. Its presence blocks LIVE under `RESEARCH_CONTRACT.block_live_when_pending=true`.

## ACTIVE_CLAIMS
```text
CSGO-RC3-AIM-ALIGNMENT = ACTIVE
CSGO-PACING-PVS        = PENDING
```

## CURRENT GATES - RC3 AIM ALIGNMENT + FAR-WORLD VISIBILITY

### ALIGNMENT OPEN AFTER EXECUTION PROOF
`CSGO-RC3-AIM-ALIGNMENT`: identify the exact controller -> authoritative-shot angular misalignment owner after proven native CUserCmd execution, while preserving stock fire semantics.

### QUALITY / VISIBILITY LANE - SEPARATE FROM AIM
`CSGO-PACING-PVS`: validate the claim-scoped FAR_WORLD packet and isolate the exact far-world/distant-texture causality independently of weapon/aim mutation. PVS, pixel visibility, portal behavior, asymmetric raster, and the proposed visibility envelope remain non-canonical causal candidates until validation.

Control lanes are not canonical claim owners:
```text
META CSGO-TAB5-GOVERNANCE = COLD_CONTROL / GLOBAL_STATE_AUTHORITY
R4   CSGO-R4-SIDECAR-OWNERSHIP = HOLD / DRIFT_RECONCILE_REQUIRED
```

## CLAIM_GRAPH — current canonical summary
### PROVEN_EXACT_TARGET / promoted
- `CSGO-WORLD-HMD-6DOF`
- `CSGO-FIRE-ABI`
- `CSGO-SERVER-CURRENT-CMD-OWNER`
- `CSGO-RC2-ASLR-GUARD-ROOT`
- `CSGO-RC3-NATIVE-AIM-EXECUTION` at the actually observed runtime scope only:
  - authoritative server corridor `BULLET_B_EDI`
  - observed client PASS callsite `0x006AA5C4`
  - do not generalize this RESULT to every structurally mapped firearm corridor

### FALSIFIED
- `CSGO-RC3-SAFE161-3HOOK-RUNTIME` as an all-up integration baseline
- `CSGO-RC2-3HOOK-RUNTIME`
- `CSGO-MUZZLE-MATRIX-V16`

### SUPERSEDED
- `CSGO-RC3-PRELIVE-IMPLEMENTATION-CLOSURE`
- `CSGO-THREE-HOOK-CONTRACT-REDTEAM`
- `CSGO-WEAPON-BARREL-AXIS` as the old mainline
- `CSGO-T0-GPU-TRANSPORT-WORKSTREAM` as the current project gate

### ACTIVE
- exact controller -> authoritative-shot angular misalignment owner after proven native execution

### PENDING_VALIDATION
- exact cause of far-world / distant-texture disappearance through `CSGO-EV-FARWORLD-PENDING-20260908`

### UNKNOWN
- runtime proof for structurally mapped firearm corridors other than the observed `BULLET_B_EDI` path

## FAR_WORLD packet boundaries
The pending packet preserves, but does not yet canonize as causal proof:
- T14 did not meet its own active-duration scientific gate and does not prove PVS causality;
- simple shared center-PVS / LEFT-world-list / one-global-occlusion models are reported as strongly weakened by COLD evidence and require provenance validation;
- pixel-visibility identity collision is a plausible effects/history defect but is not ordinary-world geometry proof;
- portal-open behavior has a competing camera-frustum / area-frustum `CullBox` explanation and is not unique PVS evidence;
- a conservative CPU visibility envelope (`world FOV 108 deg`, aspect approximately `0.963752996`) is a candidate intervention only, not a proven fix;
- validation must reconcile exact SAFE161 vs executed RC3 lineage before any LIVE.

## Protected baseline / invariants
Frozen target:
```text
legacy CS:GO x86
AppID 4465480
BuildID 22100934
client.dll SHA256 183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03
server.dll SHA256 699757BB5C8DDB71193F68DABA22F59F697491DDB768BC8656AC2CE435C4DB8
HLAE 2.191.1
local/offline -insecure only
```

Rollback authority:
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

Protected semantics:
- Quest stereo / asymmetric projection / native camera origin / HMD 6DoF remain closed absent contradictory exact evidence
- stock `Weapon_ShootPosition` remains fire-origin owner
- native AimPunch / recoil remains protected
- stock spread / inaccuracy remains protected
- stock FX fire remains protected
- `HEAD != HAND_GRIP != HAND_AIM != WEAPON != FIRE_AIM != FIRE_ORIGIN`

## ACTIVE_OWNERSHIP / OWNER_MAP
```text
META:
  lane                 = CSGO-TAB5-GOVERNANCE
  canonical authority  = GLOBAL_STATE.md
  legacy branch        = meta/tab5-governance
  legacy branch state  = STALE_READ_ONLY / DO_NOT_CONTINUE
  scope                = CONTROL_ONLY

R4:
  lane                 = CSGO-R4-SIDECAR-OWNERSHIP
  branch               = cold/r4/sidecar-ownership
  PINNED_BASE           = 9cb401e5e8f1ee78cbc4484eb20d244705bd5dad
  LAST_ACK_MAIN         = 943fb84be55e56871769e71505e7b2f1440e11e9
  status               = HOLD / DRIFT_RECONCILE_REQUIRED
  scope                = COLD_ONLY

ALIGNMENT:
  canonical claim = CSGO-RC3-AIM-ALIGNMENT
  dedicated technical owner = UNBOUND

FAR_WORLD:
  canonical claim = CSGO-PACING-PVS
  status = PENDING_VALIDATION
  packet = research/FARWORLD_PENDING_SYNC_20260908.md
  dedicated technical owner = UNBOUND
```

Operational TAB labels remain coordination labels only:
```text
TAB1 = AIM
TAB2 = FAR WORLD / VISIBILITY
TAB3 = ALIGNMENT / BALLISTICS
TAB4 = PACING / XR TRANSPORT
TAB5 = META governance / merger / STOP
```
Do not infer technical ROLE IDs from TAB numbers.

## DEPENDENCIES
- AIM alignment work depends on preserving the proven RC3 native CUserCmd execution substrate and stock fire semantics.
- FAR_WORLD validation depends on M3.2B rollback authority, executed RC3 symptom evidence, T14 as an incomplete control, and exact current-lineage SAFE161/RC3 COLD reconciliation.
- FAR_WORLD validation must not mutate AIM/weapon state.
- R4 sidecar ownership must reconcile current main before its conclusions can feed integration or promotion.
- META depends on newest GLOBAL_STATE, PENDING_SYNC, ACTIVE/PENDING claims and CLAIM_GRAPH before any local conclusion.

## CONFLICTS
### FAR_WORLD causal ambiguity preserved
The executed RC3 result proves the symptom, not its owner. Existing COLD material contains competing scopes: ordinary geometry visibility-envelope/portal-frustum candidates and pixel-visibility/effects-history defects. Neither may silently overwrite the other. The pending packet exists specifically to adjudicate provenance and scope.

### R4 stale MAIN_DRIFT ACK
R4 remains HOLD until its branch is reconciled against current canonical state.

### Legacy META branch
The pre-contract `meta/tab5-governance` branch remains `STALE_READ_ONLY` provenance.

No evidence conflict is resolved by silent overwrite.

## HOLD / mutation semaphore
- META may not execute another lane's LIVE.
- ALL LIVE is blocked while `PENDING_SYNC` is non-empty under current research contract.
- R4 may not continue from its stale drift ACK until a new reconcile is produced.
- Do not combine aim/alignment mutation and visibility/PVS mutation in one causal LIVE.
- Before any future LIVE, all five lines remain mandatory:
  - `MISSING FACT =`
  - `WHY NOT COLD =`
  - `PASS =`
  - `FAIL =`
  - `BASELINE MUST HOLD =`
- If the fact is obtainable from existing RESULT / COLD / sibling evidence, `NO_LIVE`.

## DO_NOT_REPEAT
- do not treat pending FAR_WORLD candidates as PROVEN
- do not use `r_portalsopenall` recovery as standalone PVS proof
- do not use pixel-visibility scope evidence as ordinary-world root proof
- do not run PVS/occlusion disable experiments before exact current-lineage COLD validation
- do not mix AIM or weapon mutation into FAR_WORLD validation
- do not infer ALIGNMENT/FAR_WORLD ownership from TAB labels
- do not treat stale R4 drift ACK as current
- do not reopen M3.2B world/HMD closure without contradictory exact evidence
- do not call the whole RC3 package PROVEN because native execution passed
- do not call RC3 native aim inactive; that was the RC2 failure
- do not generalize `BULLET_B_EDI` runtime proof to all firearm corridors
- do not reintroduce render-space muzzle ownership, global FX/pointer mutation, RunCommand/TLS, or `g_weaponGrabRotOffset` ballistic correction

## FIRST_MISSING_EDGE — META
```text
VALIDATE CSGO-EV-FARWORLD-PENDING-20260908 BY EXACT CURRENT-LINEAGE COLD RECONCILIATION
```

Validation order:
```text
1 recover exact SAFE161 vs executed RC3 files/hook changes relevant to CViewSetup, FOV/aspect, frustum, view/split identity, portals, occlusion and ordinary draw membership
2 validate or reject the claimed COLD provenance for eye-local PVS/world-list/renderables/occlusion identity
3 separate pixel-visibility/effects history from ordinary-world membership
4 determine whether late CPU visibility envelope remains a viable exact-current candidate
5 identify exactly one fact that remains runtime-only, if any
6 RESULT -> HANDOFF -> VALIDATE -> remove/replace PENDING_SYNC -> GLOBAL_STATE
```

## Current governance verdict
```text
GLOBAL_STATE_BOOTSTRAP = CANONICAL
GLOBAL_REVISION_AT_START_FOR_NEXT_CYCLE = 4
FAR_WORLD              = PENDING_VALIDATION / OWNER_UNBOUND
ALIGNMENT              = ACTIVE / OWNER_UNBOUND
R4_SIDECAR             = HOLD / DRIFT_RECONCILE_REQUIRED
LIVE                    = BLOCKED_BY_PENDING_SYNC
RESEARCH_CONTINUATION   = COLD_ONLY_UNTIL_PENDING_RESOLVED
```