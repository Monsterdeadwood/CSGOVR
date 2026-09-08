# META Promotion — FAR_WORLD Pending Sync Registration

CLAIM_ID: CSGO-PACING-PVS
ROLE: META
TARGET: repository control plane / pending evidence queue
EVIDENCE_CLASS: PENDING_CONVERGENCE_REGISTRATION
PRIOR_SCOPE: FAR_WORLD claim ACTIVE with no formal sibling HANDOFF/SYNC_PACKET
NEW_SCOPE: FAR_WORLD claim PENDING_VALIDATION with one claim-scoped packet and LIVE blocked until convergence

## Packet
`research/FARWORLD_PENDING_SYNC_20260908.md`

```text
EVIDENCE_ID = CSGO-EV-FARWORLD-PENDING-20260908
CLAIM_ID = CSGO-PACING-PVS
FACT_STATUS = CANDIDATE / PENDING_VALIDATION
OWNER = UNBOUND
SOURCE_GLOBAL_REVISION = 3
```

## Accepted governance delta
- advance `GLOBAL_REVISION` 3 -> 4;
- register exactly one FAR_WORLD packet in `GLOBAL_STATE.md#PENDING_SYNC`;
- change `CSGO-PACING-PVS` from `ACTIVE` to `PENDING` while preserving its state anchor and claim identity;
- register packet evidence in `CLAIM_GRAPH.json` and link it to `CSGO-PACING-PVS`;
- set `RESEARCH_CONTRACT.pending_evidence_refs` to the packet;
- leave `CSGO-RC3-AIM-ALIGNMENT` ACTIVE and separate;
- keep FAR_WORLD technical owner UNBOUND;
- keep R4 on drift HOLD;
- block LIVE through the existing pending-evidence contract.

## Evidence-scope boundary
This promotion does **not** promote any FAR_WORLD candidate to PROVEN. In particular it does not prove:
- PVS as root cause;
- CPU FOV/aspect visibility-envelope causality;
- portal-origin causality;
- pixel-visibility as ordinary-world cause;
- the 108-degree / 0.963752996 envelope as a fix.

Those items are preserved only as competing COLD candidates or scope boundaries for validation.

## Validation target
The next step is exact current-lineage COLD reconciliation of SAFE161 versus executed RC3 around CViewSetup, world FOV/aspect, frustum generation, portal-refined area frustums, view/split identity, hardware occlusion identity and ordinary-world draw membership. Only after that may the packet be promoted, narrowed, falsified or discarded.

No LIVE authorization is created by this record.