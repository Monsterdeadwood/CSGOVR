# CSGOVR — GLOBAL STATE

PROJECT_SYNC_CONTRACT = v1.1
SOURCE_OF_TRUTH = GLOBAL_STATE.md
GLOBAL_REVISION = 1
GLOBAL_TIMESTAMP = 2026-09-08T14:32:27Z
SOURCE_MAIN_SHA = ecf9fdda4ffd8679eb3bcab36f80df6fff943dc1
LOCAL_STATE_PRIORITY = LOWER_THAN_GLOBAL

## Migration boundary
This is the initial GLOBAL_STATE bootstrap. It is derived only from the already-promoted current `main` state at `SOURCE_MAIN_SHA`, using:
- `docs/SYNC.md`
- `CLAIM_GRAPH.json`
- `RESEARCH_CONTRACT.yaml`
- `LATEST_SYNC.md`
- `research/RC3_RUNTIME_RECONCILE_20260907.md`

No branch-local COLD conclusion is silently promoted by this migration.

## PENDING_SYNC
Ordered set of formal unpromoted HANDOFF / SYNC_PACKET entries:

```text
[]
```

At migration time no formal unpromoted packet is registered in canonical `main`. Library/chat-only analyses remain `BRANCH_LOCAL` or `CANDIDATE` until packetized, validated, and promoted.

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
META CSGO-TAB5-GOVERNANCE = COLD_CONTROL
R4   CSGO-R4-SIDECAR-OWNERSHIP = UNBOUND_COLD_LANE
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
  lane   = CSGO-TAB5-GOVERNANCE
  branch = meta/tab5-governance
  old BASE_MAIN_SHA = 943fb84be55e56871769e71505e7b2f1440e11e9
  current main      = ecf9fdda4ffd8679eb3bcab36f80df6fff943dc1
  drift             = RELEVANT / DRIFT_RECONCILE_REQUIRED
  scope             = CONTROL_ONLY

R4:
  lane   = CSGO-R4-SIDECAR-OWNERSHIP
  branch = cold/r4/sidecar-ownership
  BASE_MAIN_SHA = 9cb401e5e8f1ee78cbc4484eb20d244705bd5dad
  status = COLD_ACTIVE / MAIN_DRIFT_ACK_20260908
  scope  = COLD_ONLY

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
- R4 sidecar ownership is independent and must not silently become the current renderer mainline.
- META depends on newest GLOBAL_STATE, then PENDING_SYNC / ACTIVE_CLAIMS / CLAIM_GRAPH before any local conclusion.

## CONFLICTS
### META branch vs current main
`meta/tab5-governance` was written against pre-promotion main `943fb84...` and stated PR #20 was unmerged. Current `main` is `ecf9fdda...`; PR #20 is merged. The local META branch is therefore stale on that point and must reconcile before further governance work.

### Legacy state pointers
Before this migration, `RESEARCH_CONTRACT.yaml` and `docs/STATE.md` still named `docs/SYNC.md` as canonical state. Under PROJECT_SYNC_CONTRACT v1.1 those pointers must be migrated to `GLOBAL_STATE.md`; `docs/SYNC.md` remains evidence/state snapshot material subordinate to the global revision.

No evidence conflict is resolved by silent overwrite. Both old provenance and promoted current facts remain preserved.

## HOLD / mutation semaphore
- META may not execute another lane's LIVE.
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
BOOTSTRAP / VALIDATE / PROMOTE PROJECT_SYNC_CONTRACT v1.1 GLOBAL_STATE
```
Reason: the new contract names `GLOBAL_STATE.md` as source of truth, but current `main` had no such file. Until this migration is validated/promoted, a strict cycle cannot honestly record `GLOBAL_REVISION_AT_START` and therefore must not continue branch research from stale local state.

After promotion, META's recurring task is:
```text
READ newest GLOBAL_STATE
-> record GLOBAL_REVISION_AT_START
-> read PENDING_SYNC / ACTIVE_CLAIMS / CLAIM_GRAPH
-> reconcile sibling RESULT / STATE / HANDOFF
-> resolve drift / conflicts
-> select highest-value uncovered FIRST_MISSING_EDGE
```

## Current governance verdict
```text
GLOBAL_STATE_BOOTSTRAP = PROPOSED
RESEARCH_CONTINUATION  = HOLD UNTIL GLOBAL_STATE PROMOTION
LIVE                   = FORBIDDEN FROM META
```
