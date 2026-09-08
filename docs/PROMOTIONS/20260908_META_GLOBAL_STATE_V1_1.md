# META Promotion — PROJECT_SYNC_CONTRACT v1.1 GLOBAL_STATE

CLAIM_ID: META-GLOBAL-STATE-V1-1
ROLE: META
TARGET: repository control plane
EVIDENCE_CLASS: TARGET_STATIC_EXACT
PRIOR_SCOPE: canonical research state routed through `docs/SYNC.md` without monotonic GLOBAL_REVISION
NEW_SCOPE: explicit `GLOBAL_STATE.md` source of truth with PROJECT_SYNC_CONTRACT v1.1 migration boundary

## Accepted delta
Bootstrap the repository control plane required by PROJECT_SYNC_CONTRACT v1.1.

Accepted control-plane changes only:
- create `GLOBAL_STATE.md` with initial `GLOBAL_REVISION = 1`, UTC `GLOBAL_TIMESTAMP`, current promoted main provenance and explicit PENDING_SYNC / ACTIVE ownership / conflict / STOP state;
- bind `RESEARCH_CONTRACT.yaml` canonical state to `GLOBAL_STATE.md` while keeping checker-compatible schema version 2;
- make `LATEST_SYNC.md` and legacy `docs/STATE.md` point to the global source of truth;
- synchronize CURRENT issue #4 to the same source-of-truth path;
- protect `GLOBAL_STATE.md` from direct COLD/LIVE branch mutation through the existing research-integrity gate.

## Evidence boundary
This promotion does **not** promote any branch-local COLD conclusion, Library/chat analysis, new runtime fact, new LIVE authorization, renderer mutation, aim mutation, PVS hypothesis or sidecar architecture.

The initial global snapshot is derived only from already-promoted `main` at:
`ecf9fdda4ffd8679eb3bcab36f80df6fff943dc1`

Therefore the existing RC3 split verdict remains unchanged:
- native CUserCmd execution = PROVEN only at the observed runtime scope;
- all-up RC3 SAFE161 integration = FALSIFIED;
- alignment and far-world visibility remain separate ACTIVE gates.

## Migration rule
After promotion every meaningful cycle must start by reading newest `GLOBAL_STATE.md` and recording `GLOBAL_REVISION_AT_START`. Local STATE remains subordinate. A future global write must resynchronize if the canonical revision or relevant main identity changed before submission.
