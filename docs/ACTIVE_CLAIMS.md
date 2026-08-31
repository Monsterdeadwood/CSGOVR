# CSGOVR - ACTIVE COLD LANES REGISTRY

CONTROL_PLANE_ONLY. The filename `docs/ACTIVE_CLAIMS.md` is retained for compatibility, but this file does **not** own canonical claim status.

Canonical `ACTIVE / PENDING / QUEUED / PROVEN / FALSIFIED / SUPERSEDED` status is owned only by `CLAIM_GRAPH.json` together with `RESEARCH_CONTRACT.yaml` and the canonical state. This registry only records isolated COLD branches currently being worked.

## Active COLD lanes

| ROLE | COLD_LANE | CANONICAL_BINDING | BRANCH | BASE_MAIN_SHA | PRIMARY FACT | SCOPE |
|---|---|---|---|---|---|---|
| R2 | `CSGO-R2-RC3-IMPLEMENTATION-CLOSURE` | `CSGO-RC3-PRELIVE-IMPLEMENTATION-CLOSURE` | `cold/r2/native-aim-implementation` | `9cb401e5e8f1ee78cbc4484eb20d244705bd5dad` | prove the exact prepared native-aim implementation satisfies the fail-closed three-hook contract before any runtime authorization | COLD_ONLY |
| R4 | `CSGO-R4-SIDECAR-OWNERSHIP` | `UNBOUND_COLD_LANE` | `cold/r4/sidecar-ownership` | `9cb401e5e8f1ee78cbc4484eb20d244705bd5dad` | classify exact semantics/lifetime/view-dependence/mutation and consumers of recovered renderer/cache fields before sidecar/replacement promotion | COLD_ONLY |

The independent canonical `CSGO-PACING-PVS` ACTIVE claim remains owned by `CLAIM_GRAPH.json`; absence of a dedicated COLD branch in this registry does not close or supersede it. `UNBOUND_COLD_LANE` has no canonical claim-status authority.

## Claim collision law
Before creating a new CSGOVR COLD branch, inspect both this registry and `CLAIM_GRAPH.json`. Same ROLE + PRIMARY FACT means REUSE / ADAPT / FIX / EXTEND, not duplicate research. A future canonical binding requires explicit promotion.

## Main drift law
`BASE_MAIN_SHA` pins the accepted world the COLD lane started from.

Before any COLD conclusion is used to authorize LIVE or promotion:
1. compare `BASE_MAIN_SHA..main`;
2. classify changes touching M3.2B/SAFE161 rollback authority, exact target hashes, RC3 implementation contract, sidecar ownership substrate, canonical binding or same claim;
3. relevant drift means `DRIFT_RECONCILE` before promotion;
4. no blind semantic rebase.

Governance-only/non-overlapping drift may be acknowledged as `MAIN_DRIFT_ACK`.

Once a COLD branch has unique commits, do not automatically move its ref to follow main.
