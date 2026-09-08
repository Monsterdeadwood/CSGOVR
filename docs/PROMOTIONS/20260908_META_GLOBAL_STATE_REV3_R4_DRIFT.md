# META Promotion — GLOBAL_STATE Revision 3 / R4 Drift Adjudication

CLAIM_ID: META-GLOBAL-STATE-REV3-R4-DRIFT
ROLE: META
TARGET: repository control plane
EVIDENCE_CLASS: TARGET_STATIC_EXACT
PRIOR_SCOPE: GLOBAL_STATE revision 2 with R4 marked COLD_ACTIVE from an ACK against old main
NEW_SCOPE: R4 sidecar lane held until a fresh main-drift reconcile against current GLOBAL_STATE/main

## Accepted delta
Exact R4 branch evidence records:
- branch `cold/r4/sidecar-ownership`;
- pinned base `9cb401e5e8f1ee78cbc4484eb20d244705bd5dad`;
- last drift ACK evaluated only through `main=943fb84be55e56871769e71505e7b2f1440e11e9`;
- that ACK explicitly stated the 2026-09-07 RC3 runtime reconciliation had not yet been promoted.

Current canonical main at the start of the rebuilt revision is `fb695eed99a376da78e1918cccf08bb8900f03d0`. Exact compare from the R4 ACK boundary shows sixteen commits. The semantic delta includes accepted RC3 runtime reconciliation, updated `CLAIM_GRAPH.json`, `GLOBAL_STATE.md`, `RESEARCH_CONTRACT.yaml`, `docs/SYNC.md`, `docs/STATE.md`, and active-claims control-plane changes. The sixteenth commit adds an empty `NOOP` file only; it has no research semantics but required the promotion ancestry to be reconstructed under `GLOBAL_WRITE_RULE`.

Therefore the old R4 `MAIN_DRIFT_ACK / NON_OVERLAPPING` verdict is stale under PROJECT_SYNC_CONTRACT v1.1.

## Governance effect
- advance `GLOBAL_REVISION` 2 -> 3;
- mark `CSGO-R4-SIDECAR-OWNERSHIP` as `HOLD / DRIFT_RECONCILE_REQUIRED`;
- preserve every R4 technical conclusion as branch-local provenance;
- prohibit R4 LIVE/integration/promotion use until fresh drift reconcile;
- keep `CSGO-RC3-AIM-ALIGNMENT` and `CSGO-PACING-PVS` ACTIVE and technically UNBOUND;
- do not promote any Library/chat candidate evidence.

## Resync record
A first rev3 PR was validated from `main=cdaefbef...` but was closed without merge after main advanced to `fb695eed...`. This rebuilt promotion is based directly on the newer main and preserves the same scientific delta.

## No research promotion
This record does not prove or falsify a renderer, visibility, PVS, alignment, ballistic, sidecar, replacement, or XR hypothesis. It only repairs stale branch-governance state required by `MAIN_DRIFT`, `GLOBAL_WRITE_RULE`, and `STOP_RULE`.