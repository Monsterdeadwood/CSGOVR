# CLAIM ROUTER v1.0

CONTROL_PLANE_ONLY. This file changes no scientific claim, RESULT, baseline, target identity or evidence class. It is a mandatory routing guard for every resumed/new research cycle, including generic user prompts such as `думайте дальше` / `continue thinking`.

## Mandatory pre-cycle route
Before selecting work, resolve the current `RC_ROLE`, `CLAIM_ID`, canonical claim status, owner binding, dependencies, conflicts and pending sync from the project's source-of-truth plus `CLAIM_GRAPH.json` / `docs/ACTIVE_CLAIMS.md` where present. Never infer ownership from TAB number/name alone.

## Status router
- `ACTIVE` -> continue only this role's assigned PRIMARY_FACT / highest-value uncovered FIRST_MISSING_EDGE. Do not absorb a sibling claim.
- `PENDING`, `PENDING_VALIDATION`, or non-empty claim-relevant `PENDING_SYNC` -> ingest/validate/adjudicate pending evidence first. Do not start a new PRIMARY_FACT or causal LIVE that could be invalidated by it.
- `HOLD` or `BLOCKED` -> work only the named blocker/dependency/ownership conflict. No mutation/LIVE/build unless canonical state explicitly removes the hold.
- `CLOSED`, `PROVEN`, `FALSIFIED`, `SUPERSEDED`, or `DO_NOT_REPEAT` -> STOP that PRIMARY_FACT. Generic continuation is not permission to reopen it. Move only to an already assigned next claim/dependency allowed by OWNER_MAP; otherwise READY.
- `UNBOUND_COLD_LANE`, `OWNER_UNBOUND`, or equivalent -> COLD analysis only. No LIVE, build, mutation, implementation promotion or canonical-owner takeover until binding is resolved.
- `READY` -> do not invent new work; take only a canonically assigned next edge.
- `CONTRADICTION` / unresolved ownership collision -> freeze mutation, preserve both provenances and adjudicate first.

## Duplicate router
If the same PRIMARY_FACT is already ACTIVE/PROVEN/FALSIFIED/CLOSED or owned by a sibling role, STOP duplicate work unless canonical state explicitly requires independent validation or new exact evidence creates contradiction. Sibling evidence may constrain the local graph as BORROWED_EVIDENCE, but must not silently become the local PRIMARY_FACT.

## COLD -> LIVE router
A LIVE/build/mutation may be considered only when all are true:
1. COLD is saturated for the PRIMARY_FACT;
2. technical owner is bound;
3. relevant PENDING_SYNC is cleared/adjudicated;
4. no HOLD/BLOCKED dependency remains;
5. the exact five-line causal preflight exists: MISSING FACT / WHY NOT COLD / PASS / FAIL / BASELINE MUST HOLD;
6. the LIVE changes one PRIMARY FACT and preserves protected baseline.

## Generic continuation semantics
`думайте дальше`, `continue`, `think further`, or equivalent means: run this router, re-read canonical state as required, continue only eligible routed work, and STOP/READY when the claim is closed instead of generating another variant.

## Required cycle record
- RC_ROLE
- CLAIM_ID
- CLAIM_STATUS
- FIRST_MISSING_EDGE
- ROUTE_DECISION = CONTINUE_ACTIVE | INGEST_PENDING | WORK_BLOCKER | COLD_ONLY_UNBOUND | STOP_CLOSED | READY | ADJUDICATE_CONFLICT
- LIVE_ALLOWED = YES/NO
- DUPLICATE_CHECK = CLEAR/STOP

## Invariant
REUSE -> ADAPT -> FIX -> EXTEND -> NEW. Project-specific GLOBAL_STATE/STOP/promotion rules override this generic router when stricter.
