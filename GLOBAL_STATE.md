# CSGOVR — GLOBAL STATE

PROJECT_SYNC_CONTRACT = v1.1
SOURCE_OF_TRUTH = GLOBAL_STATE.md
GLOBAL_REVISION = 5
GLOBAL_TIMESTAMP = 2026-09-08T16:13:00Z
PREVIOUS_GLOBAL_REVISION = 4
GLOBAL_REVISION_BASE_MAIN_SHA = 66f7d26eddd71fc673c949b587486dc815703ef7
LOCAL_STATE_PRIORITY = LOWER_THAN_GLOBAL

## Revision 5 reason
Revision 5 records a COLD validation pass over the pending FAR_WORLD packet. No FAR_WORLD cause or fix is promoted. The pass narrows the common-cause search by reconciling SAFE161/RC3 renderer lineage, RC3 native-aim semantic writes, the exact pointer-helper caller census, client `m_AimDirection` consumers, and the recovered CheckSplitScreenMimic publish contract.

Result: direct semantic coupling from the RC3 AIM layer into ordinary-world visibility was not found. The leading model is now independent AIM misalignment plus a separate FAR_WORLD visibility defect, while exact byte-level ABI verification of the raw executed RC3 detours remains unavailable in current Library access and therefore unclaimed.

## PENDING_SYNC
```text
1. research/FARWORLD_PENDING_SYNC_20260908.md
   EVIDENCE_ID = CSGO-EV-FARWORLD-PENDING-20260908
   CLAIM_ID = CSGO-PACING-PVS
   FACT_STATUS = PENDING_VALIDATION / NARROWED
   OWNER = UNBOUND
   LEADING_MODEL = INDEPENDENT AIM + FAR_WORLD DEFECTS
```

The packet remains unpromoted evidence. LIVE remains blocked while `PENDING_SYNC` is non-empty.

## ACTIVE_CLAIMS
```text
CSGO-RC3-AIM-ALIGNMENT = ACTIVE
CSGO-PACING-PVS        = PENDING
```

## CURRENT GATES - RC3 AIM ALIGNMENT + FAR-WORLD VISIBILITY
### ALIGNMENT OPEN AFTER EXECUTION PROOF
Identify the exact controller -> authoritative-shot angular misalignment owner after proven native CUserCmd execution, preserving stock fire origin, recoil, spread/inaccuracy and FX.

### QUALITY / VISIBILITY LANE - SEPARATE FROM AIM
Identify the exact ordinary-world visibility owner that removes distant renderables. Current COLD target is the late visibility chain:

```text
late eye-local CViewSetup
    -> CPU camera frustum
    -> area-frustum refinement / CullBox
    -> ordinary renderable membership
```

This chain is a target for proof, not a promoted causal result.

## CLAIM_GRAPH — canonical summary
### PROVEN_EXACT_TARGET / promoted
- `CSGO-WORLD-HMD-6DOF`
- `CSGO-FIRE-ABI`
- `CSGO-SERVER-CURRENT-CMD-OWNER`
- `CSGO-RC2-ASLR-GUARD-ROOT`
- `CSGO-RC3-NATIVE-AIM-EXECUTION` for observed runtime scope only:
  - server corridor `BULLET_B_EDI`
  - client PASS callsite `0x006AA5C4`

### FALSIFIED
- `CSGO-RC3-SAFE161-3HOOK-RUNTIME` as an all-up integration baseline
- `CSGO-RC2-3HOOK-RUNTIME`
- `CSGO-MUZZLE-MATRIX-V16`

### SUPERSEDED
- `CSGO-RC3-PRELIVE-IMPLEMENTATION-CLOSURE`
- `CSGO-THREE-HOOK-CONTRACT-REDTEAM`
- `CSGO-WEAPON-BARREL-AXIS` as old mainline
- `CSGO-T0-GPU-TRANSPORT-WORKSTREAM` as current gate

### ACTIVE / PENDING
- `CSGO-RC3-AIM-ALIGNMENT` = ACTIVE
- `CSGO-PACING-PVS` = PENDING_VALIDATION / NARROWED

## FAR_WORLD COLD validation promoted as scope boundaries
### SAFE161 negative control
Recovered lineage records state RC3 preserved from SAFE161:
```text
HMD 6DoF functions = IDENTICAL
Quest projection   = IDENTICAL
D3D hooks          = IDENTICAL
xr_host.cpp         = BYTE IDENTICAL
slot250 transform   = SAME
PVS/culling mutation = NONE
```
SAFE161 was the stable far-world ancestor. Therefore asymmetric Quest projection or the known asymmetric area-frustum correctness issue alone is insufficient to explain the RC3 regression.

### RC3 client player-aim mutation
RC3 source audit recovered a persistent client write from current command aim into `player+0x382C/+3830/+3834`. That mutation is rejected for AIM architecture. Frozen xref census, however, does not recover a path from this field into CViewSetup/frustum/PVS/ordinary-world membership. Actual reads classify as pointer-gated or stock player->CUserCmd producer behavior.

Verdict:
```text
BAD AIM-SIDE EFFECT = YES
FAR_WORLD CAUSAL EDGE = NOT FOUND
```

### IsInputDeviceAPointer scope
Frozen direct-call census:
```text
client = 14 callers
server = 8 callers
```
The native-aim allowlist occupies a weapon-final-aim subset. Other callers include viewmodel/reticle/recoil/other final-aim consumers and fall through to original. RC3 also included a grenade route, which is an AIM-scope defect, not a recovered renderer/frustum route.

Verdict:
```text
POINTER OVERRIDE -> VISIBILITY EDGE = NOT FOUND
```

### CheckSplitScreenMimic semantic scope
Recovered command order:
```text
original CheckSplitScreenMimic
    -> RC3 cmd.aimdirection write
    -> provenance write
    -> stock command tail / verified CRC
```
No deliberate active split-screen slot, CViewSetup, view identity, frustum or PVS mutation is present in the recovered RC3 semantic contract.

Verdict:
```text
DIRECT SPLIT/VIEW MUTATION = NOT FOUND
```

### Raw executed RC3 ABI
Later audit marked RC3 hook installation transaction as needing hardening. Helper trampoline safety is supported by frozen instruction analysis, but the exact raw executed RC3 ZIP/source is not currently surfaced as a materializable Library artifact. Therefore:
```text
RAW_EXECUTED_RC3_DETOUR_ABI = UNVERIFIED_IN_CURRENT_ACCESS
```
Do not convert this absence into either PASS or FAIL.

## Current model ranking
```text
1 INDEPENDENT DEFECTS
  AIM alignment problem + separate FAR_WORLD visibility problem
  = LEADING

2 SHARED INFRASTRUCTURE ROOT
  aim-hook layer indirectly corrupts render/view/frustum state
  = WEAKENED; no semantic edge found; raw ABI remains unverified

3 SHARED SEMANTIC ROOT
  same gameplay semantic owner directly causes both defects
  = STRONGLY WEAKENED
```

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
- native AimPunch/recoil remains protected
- stock spread/inaccuracy remains protected
- stock FX fire remains protected
- `HEAD != HAND_GRIP != HAND_AIM != WEAPON != FIRE_AIM != FIRE_ORIGIN`

## ACTIVE_OWNERSHIP / OWNER_MAP
```text
META:
  lane = CSGO-TAB5-GOVERNANCE
  authority = GLOBAL_STATE.md
  scope = CONTROL_ONLY

R4:
  lane = CSGO-R4-SIDECAR-OWNERSHIP
  branch = cold/r4/sidecar-ownership
  status = HOLD / DRIFT_RECONCILE_REQUIRED

ALIGNMENT:
  claim = CSGO-RC3-AIM-ALIGNMENT
  technical owner = UNBOUND

FAR_WORLD:
  claim = CSGO-PACING-PVS
  status = PENDING_VALIDATION / NARROWED
  packet = research/FARWORLD_PENDING_SYNC_20260908.md
  technical owner = UNBOUND
```

Operational TAB labels remain coordination labels only:
```text
TAB1 = AIM
TAB2 = FAR WORLD / VISIBILITY
TAB3 = ALIGNMENT / BALLISTICS
TAB4 = PACING / XR TRANSPORT
TAB5 = META governance / merger / STOP
```

## HOLD / mutation semaphore
- META may not execute another lane's LIVE.
- ALL LIVE is blocked while `PENDING_SYNC` is non-empty.
- do not combine aim/alignment and visibility mutations.
- R4 remains on drift HOLD.
- before any future LIVE, all five lines are mandatory:
  - `MISSING FACT =`
  - `WHY NOT COLD =`
  - `PASS =`
  - `FAIL =`
  - `BASELINE MUST HOLD =`

## DO_NOT_REPEAT
- do not treat pending FAR_WORLD candidates as PROVEN
- do not promote `player+0x382C` as FAR_WORLD root without a real visibility edge
- do not describe return-address-gated pointer overrides as global pointer spoof
- do not use `r_portalsopenall` as standalone PVS proof
- do not use pixel-visibility scope evidence as ordinary-world root proof
- do not run PVS/occlusion disable experiments before ordinary-world ownership is exhausted COLD
- do not mix AIM/weapon mutation into FAR_WORLD validation
- do not reopen M3.2B world/HMD closure without contradictory exact evidence
- do not generalize `BULLET_B_EDI` runtime proof to all firearm corridors

## FIRST_MISSING_EDGE — META
```text
PROVE EXACT ORDINARY-WORLD MEMBERSHIP CHAIN AFTER LATE CViewSetup
```

Priority:
```text
1 CViewSetup world FOV/aspect -> camera frustum construction
2 camera frustum -> portal/area frustum refinement
3 area frustum -> ClientLeafSystem / CullBox ordinary-renderable membership
4 identify lifetime/cache identity per eye and per frame
5 reconcile PVS/world-list/occlusion ownership against this chain
6 only then name exactly one runtime-only discriminating fact, if any
```

Raw RC3 detour ABI verification remains an independent negative control if the exact package becomes available; it is no longer the highest-value blocker for FAR_WORLD COLD progress.

## Current governance verdict
```text
GLOBAL_REVISION_AT_START_FOR_NEXT_CYCLE = 5
FAR_WORLD = PENDING_VALIDATION / NARROWED / OWNER_UNBOUND
ALIGNMENT = ACTIVE / OWNER_UNBOUND
LEADING_MODEL = INDEPENDENT_DEFECTS
R4_SIDECAR = HOLD / DRIFT_RECONCILE_REQUIRED
LIVE = BLOCKED_BY_PENDING_SYNC
RESEARCH_CONTINUATION = COLD_ONLY
```