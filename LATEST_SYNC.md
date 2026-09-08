# CSGOVR — LATEST SYNC

SYNC_EPOCH = 2026-09-08
BRANCH = `meta/tab5-governance`
ROLE = META / TAB5 MASTER MERGER + REDTEAM + MUTATION SEMAPHORE
STATE = COLD / CONTROL-PLANE

Mandatory read order:
1. `docs/META/TAB5_GOVERNANCE_SYNC_20260907.md`
2. `state/rc3-safe161-runtime-gate` result/reconcile state where branch-cross-reference is needed
3. `promote/rc3-runtime-reconcile-20260908` / draft PR #20 for the unmerged canonical RC3 delta
4. `CLAIM_GRAPH.json` on current canonical main
5. `docs/ACTIVE_CLAIMS.md`
6. role/tab-specific STATE/EVIDENCE/RESULT for the fact being arbitrated

Operational tab map:
- TAB1 AIM
- TAB2 FAR WORLD / VISIBILITY
- TAB3 ALIGNMENT / BALLISTICS
- TAB4 PACING / XR TRANSPORT
- TAB5 META governance / merger / STOP

Do not infer R-role IDs from TAB numbers. The current RC3 RESULT must remain split into native execution PROVEN, all-up integration FALSIFIED, alignment OPEN and far-world causality OPEN.

META does not execute another lane's LIVE and does not wholesale-merge COLD/state branches into main.
