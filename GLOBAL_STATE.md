# CSGOVR — GLOBAL STATE

PROJECT_SYNC_CONTRACT = v1.1
SOURCE_OF_TRUTH = GLOBAL_STATE.md
GLOBAL_REVISION = 3
GLOBAL_TIMESTAMP = 2026-09-08T15:05:26Z
PREVIOUS_GLOBAL_REVISION = 2
GLOBAL_REVISION_BASE_MAIN_SHA = fb695eed99a376da78e1918cccf08bb8900f03d0
LOCAL_STATE_PRIORITY = LOWER_THAN_GLOBAL

## Revision 3 reason
Revision 2 completed the GLOBAL_STATE bootstrap and ownership-recovery pass. Revision 3 adjudicates a newly discovered MAIN_DRIFT conflict: the only registered sibling COLD branch, `cold/r4/sidecar-ownership`, still relied on `R4_MAIN_DRIFT_ACK_20260908.md`, but that ACK compared only through `main=943fb84...` and explicitly predated the accepted RC3 runtime reconciliation. Current main is sixteen commits ahead of that ACK boundary and now contains the RC3 runtime reconciliation, updated claim graph, GLOBAL_STATE control plane, active far-world/visibility gate, plus one non-semantic empty `NOOP` commit that triggered a required promotion resync.

No R4 technical finding is falsified or promoted by this revision.

## PENDING_SYNC
Ordered set of formal unpromoted HANDOFF / SYNC_PACKET entries:

```text
[]
```

No formal unpromoted packet is registered in canonical state at this revision. Library/chat-only analyses remain `BRANCH_LOCAL` or `CANDIDATE` until packetized, validated, and promoted.

## ACTIVE_CLAIMS
```text
CSGO-RC3-AIM-ALIGNMENT = ACTIVE
CSGO-PACING-PVS        = ACTIVE
```

## CURRENT GATES - RC3 AIM ALIGNMENT + FAR-WORLD VISIBILITY

### ALIGNMENT OPEN AFTER EXECUTION PROOF
`CSGO-RC3-AIM-ALIGNMENT`: identify the exact controller -> authoritative-shot angular misalignment owner after proven native CUserCmd execution, while preserving stock fire semantics.

### QUALITY / VISIBILITY LANE - SEPARATE FROM AIM
`CSGO-PACING-PVS`: isolate the exact far-world/distant-texture causality independently of weapon/aim mutation; PVS is not assumed to be the root cause without causal isolation.

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

### UNKNOWN / ACTIVE
- exact controller -> authoritative-shot angular misalignment owner after proven native execution
- exact cause of far-world / distant-texture disappearance
- runtime proof for structurally mapped firearm corridors other than the observed `BULLET_B_EDI` path

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
  legacy BASE_MAIN_SHA = 943fb84be55e56871769e71505e7b2f1440e11e9
  legacy branch state  = STALE_READ_ONLY / DO_NOT_CONTINUE
  scope                = CONTROL_ONLY

R4:
  lane                 = CSGO-R4-SIDECAR-OWNERSHIP
  branch               = cold/r4/sidecar-ownership
  PINNED_BASE           = 9cb401e5e8f1ee78cbc4484eb20d244705bd5dad
  LAST_ACK_MAIN         = 943fb84be55e56871769e71505e7b2f1440e11e9
  CURRENT_MAIN_AT_REV3  = fb695eed99a376da78e1918cccf08bb8900f03d0
  status               = HOLD / DRIFT_RECONCILE_REQUIRED
  reason               = accepted RC3 runtime + visibility gate + GLOBAL_STATE changes postdate the branch ACK
  scope                = COLD_ONLY

ALIGNMENT:
  canonical claim = CSGO-RC3-AIM-ALIGNMENT
  dedicated technical owner = UNBOUND in canonical registry

FAR_WORLD:
  canonical claim = CSGO-PACING-PVS
  dedicated technical owner = UNBOUND in canonical registry
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
- FAR WORLD work depends on preserving M3.2B rollback authority and keeping visibility mutation independent from aim mutation.
- R4 sidecar ownership must reconcile accepted RC3 runtime/visibility state before any R4 conclusion can feed LIVE, integration, or promotion.
- META depends on newest GLOBAL_STATE, then PENDING_SYNC / ACTIVE_CLAIMS / CLAIM_GRAPH before any local conclusion.

## CONFLICTS
### R4 stale MAIN_DRIFT ACK
`docs/COLD/R4_MAIN_DRIFT_ACK_20260908.md` on `cold/r4/sidecar-ownership` compared the branch only through `main=943fb84...` and stated that the 2026-09-07 RC3 runtime reconciliation had not yet been promoted. Current `main` is `fb695eed...`, sixteen commits ahead of that boundary. The intervening semantic delta includes accepted RC3 runtime reconciliation, current AIM/FAR_WORLD claim changes, `GLOBAL_STATE.md`, `CLAIM_GRAPH.json`, and synchronized control files. The old `NON_OVERLAPPING` verdict is therefore stale and cannot authorize continued R4 work.

### Promotion resync event
A first rev3 promotion was validated from `main=cdaefbef...`, but main advanced during validation by one empty `NOOP` commit. Under `GLOBAL_WRITE_RULE` the stale promotion was closed without merge and reconstructed from exact current main. The `NOOP` delta carries no research semantics but still invalidated the old promotion ancestry.

### Legacy META branch
The pre-contract `meta/tab5-governance` branch remains preserved as provenance only and is `STALE_READ_ONLY`.

No evidence conflict is resolved by silent overwrite. Both old provenance and promoted current facts remain preserved.

## HOLD / mutation semaphore
- META may not execute another lane's LIVE.
- R4 may not continue from its stale drift ACK until a new reconcile is produced against newest main/GLOBAL_REVISION.
- Do not combine aim/alignment mutation and visibility/PVS mutation in one causal LIVE.
- Do not execute any LIVE while a relevant formal `PENDING_SYNC` packet is unresolved.
- Before LIVE, all five lines are mandatory and must be valid:
  - `MISSING FACT =`
  - `WHY NOT COLD =`
  - `PASS =`
  - `FAIL =`
  - `BASELINE MUST HOLD =`
- If the fact is obtainable from existing RESULT / COLD / sibling evidence, `NO_LIVE`.

## DO_NOT_REPEAT
- do not continue from `meta/tab5-governance` local state; it is stale provenance only
- do not treat `R4_MAIN_DRIFT_ACK_20260908.md` as current after RC3/global-state promotion
- do not infer ALIGNMENT/FAR_WORLD ownership from TAB labels
- do not reopen M3.2B world/HMD closure without contradictory exact evidence
- do not call the whole RC3 package PROVEN because native execution passed
- do not call RC3 native aim inactive; that was the RC2 failure
- do not generalize `BULLET_B_EDI` runtime proof to all firearm corridors
- do not reintroduce `g_weaponGrabRotOffset` as ballistic correction
- do not use attachment matrices, DrawModel or render-space muzzle ownership as gameplay-fire provenance
- do not reintroduce global FX mutation or global pointer-device mutation
- do not reintroduce RunCommand/TLS without a specifically falsified current invariant
- do not combine AIM and FAR_WORLD intrusive mutations
- do not continue superseded pre-LIVE implementation-closure work as if RC3 had never executed

## FIRST_MISSING_EDGE — META
```text
CREATE / VALIDATE CLAIM-SCOPED PENDING SYNC PACKETS FOR THE TWO UNBOUND ACTIVE CLAIMS
```
Reason: ownership recovery found no current formal sibling RESULT / STATE / HANDOFF for `CSGO-RC3-AIM-ALIGNMENT` or `CSGO-PACING-PVS`. Existing library/chat analyses contain candidate evidence but cannot become canonical or establish ownership by implication. The repository already has a compatible pending-evidence mechanism (`PENDING` claim status + registered evidence + `pending_evidence_refs`) that can be adapted to PROJECT_SYNC_CONTRACT v1.1 rather than inventing a parallel format.

Priority:
```text
1 keep ALIGNMENT and FAR_WORLD owners UNBOUND
2 package only claim-scoped, provenance-preserving candidate conclusions
3 register each packet as unpromoted pending evidence
4 validate against exact current RESULT / COLD / sibling provenance
5 promote, falsify, or discard each packet independently
6 only then bind ownership if the packet itself proves a current owner
```

## Current governance verdict
```text
GLOBAL_STATE_BOOTSTRAP = CANONICAL
GLOBAL_REVISION_AT_START_FOR_NEXT_CYCLE = 3
R4_SIDECAR             = HOLD / DRIFT_RECONCILE_REQUIRED
RESEARCH_CONTINUATION  = COLD_ALLOWED_SUBJECT_TO_GLOBAL_SYNC
LIVE_FROM_META         = FORBIDDEN
```