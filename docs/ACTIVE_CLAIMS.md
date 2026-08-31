# CSGOVR — ACTIVE CLAIMS REGISTRY

CONTROL_PLANE_ONLY. This file records concurrency/branch ownership only. It is not evidence and cannot promote a claim.

## Active COLD lanes

| ROLE | CLAIM_ID | BRANCH | BASE_MAIN_SHA | PRIMARY FACT | SCOPE |
|---|---|---|---|---|---|
| R2 | `CSGO-RC3-PRELIVE-IMPLEMENTATION-CLOSURE` | `cold/r2/native-aim-implementation` | `9cb401e5e8f1ee78cbc4484eb20d244705bd5dad` | prove the exact prepared native-aim implementation satisfies the fail-closed three-hook contract before any runtime authorization | COLD_ONLY |
| R4 | UNASSIGNED | `cold/r4/sidecar-ownership` | `9cb401e5e8f1ee78cbc4484eb20d244705bd5dad` | classify exact semantics/lifetime/view-dependence/mutation and consumers of recovered renderer/cache fields before sidecar/replacement promotion | COLD_ONLY |

## Claim collision law
Before creating a new CSGOVR research branch, search this registry and existing branches. Same ROLE + PRIMARY FACT => REUSE/ADAPT/FIX/EXTEND, not duplicate research.

## Main drift law
`BASE_MAIN_SHA` pins the accepted world the COLD lane started from.

Before any COLD conclusion is used to authorize LIVE or promotion:
1. compare `BASE_MAIN_SHA..main`;
2. classify changes touching M3.2B/SAFE161 rollback authority, exact target hashes, RC3 implementation contract, sidecar ownership substrate or same claim;
3. relevant drift => `DRIFT_RECONCILE` before promotion;
4. no blind semantic rebase.

Governance-only/non-overlapping drift may be acknowledged as `MAIN_DRIFT_ACK`.

Once a COLD branch has unique commits, do not automatically move its ref to follow main.
