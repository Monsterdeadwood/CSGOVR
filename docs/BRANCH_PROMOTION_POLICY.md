# CSGOVR — BRANCH / PROMOTION POLICY

Updated: 2026-08-31.

This policy prevents COLD native-aim and R4 sidecar research from contaminating accepted exact-retail/runtime truth.

## Branch classes

### `main`
Accepted canon only: governance, exact evidence records, `GLOBAL_LOCKED`, `ROLE_PROVEN`, accepted STATE/SYNC/NEXT and reconciled historical evidence.

### `cold/<role>/<claim>`
Research-only. Active lanes:
- `cold/r2/native-aim-implementation`
- `cold/r4/sidecar-ownership`

Historical `experiment/rc3-prelive-cold-closure`, `research/*` and state branches remain evidence/history. They are not wholesale-merge sources into current `main`.

### `live/<role>/<claim>/<run>`
Created only after COLD saturation and the exact five-line LIVE preflight. MUST branch fresh from current `main`, never from COLD. Copy only the bounded experiment delta needed to test one PRIMARY FACT.

### `promote/<role>/<claim>`
Created fresh from current `main` after independently promotable exact static proof or a valid exact-retail RESULT. Contains only the accepted promotion delta.

## Hard no-wholesale-merge law
- NEVER merge `cold/*` into `main` wholesale.
- NEVER merge `live/*` into `main` wholesale.
- Promotion is claim-scoped reconstruction on fresh `promote/*`.
- Valid causal FAIL may promote to FALSIFIED when experiment transport and baseline were valid.

## Canonical-file write barrier on COLD
COLD branches must not modify as authoritative truth:
- `docs/PROVEN.md`
- `docs/FALSIFIED.md`
- `docs/STATE.md`
- `docs/SYNC.md`
- `docs/NEXT.md`
- `LATEST_SYNC.md`
- M3.2B/SAFE161 rollback authority, exact target hashes or global claim state.

Proposed canonical deltas remain shadow drafts under `docs/COLD/` with `SCOPE=COLD_ONLY`.

## Promotion gate
Record `CLAIM_ID`, ROLE, exact target/build, evidence class, prior/new scope, artifact/source identity, PASS/FAIL, baseline status, contradictions resolved and canonical files changed.

Exact static/source proof can promote ABI/control-flow/implementation properties when those are the claim. Runtime hook activation, authoritative gameplay effect, visual stability and cadence require exact execution.

## Cross-role law
R2 native-aim/replay and R4 sidecar imports are `BORROWED_EVIDENCE` until independently closed in the receiving role.

## Read order
1. `docs/EVIDENCE_BOUNDARY.md`
2. `docs/BRANCH_PROMOTION_POLICY.md`
3. `docs/PORTFOLIO_SYNC_20260831.md`
4. assigned ROLE_LOCK state/evidence
5. exact RESULT/source for the active claim
