# CSGOVR

Native VR research for **frozen legacy CS:GO x86** using HLAE + a separate x64 OpenXR host, with exact-retail evidence.

## Current state
**ACTIVE. Current gate: XR TRANSPORT.**

The last remote state had already prepared the next Quest projection child, but it did **not** contain an exact headset/transport PASS. Cross-branch sync on 2026-08-24 therefore keeps projection integration queued behind transport instead of silently treating transport as solved.

### Current integration candidate
`CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF`

Artifact SHA256:
`DFEA881888C83812A5B9EE3A7049F79D32935240F2C612DE4ABDF4F9A98834A3`

Status: **PREPARED / STATIC-AUDITED / NOT YET LIVE-PROVEN**.

Purpose:
- preserve v0.5.5 HLAE LEFT/RIGHT/monitor scheduling;
- capture completed pass1/pass2 pixels through the proven D3D9 CPU readback primitive;
- publish only an atomic same-HLAE pair;
- submit through an x64 D3D11/OpenXR host;
- collect one exact same-pair LEFT/RIGHT capture and transport-stage telemetry.

## GOLDEN
`CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`

Do not replace it. The transport child is an integration candidate, not a new GOLDEN.

## Gate order
`XR TRANSPORT -> CORRECT PROJECTION -> TRUE EYE ORIGIN/PARALLAX -> VALID CAPTURE/CULLING TRUTH -> HEAD POSE -> WEAPON/HANDS -> HUD -> REGRESSION -> RELEASE CANDIDATE`

The prepared Quest projection child already in this repo is **queued immediately after XR TRANSPORT PASS**.

## Start here
1. `docs/SYNC.md` — cross-branch synchronized truth
2. `docs/STATE.md` — complete active handoff
3. `docs/PROVEN.md` — directly established facts
4. `docs/FALSIFIED.md` — dead/superseded paths
5. `docs/GATES.md` — gate ladder
6. `experiments/NEXT_EXPERIMENT.md` — exact next LIVE contract
7. `research/RESULT_INDEX.md` — result/tool provenance ledger
8. `runtime/children/v0.7.1_xr_transport/` — current transport candidate package/manifest

## Working protocol
`SYNC -> THINK -> KEEP/ADAPT/STOP -> BATCH HARVEST -> CONVERGE -> PROVEN/FALSIFIED -> NEXT GATE -> SYNC`

**PARALLELIZE EVIDENCE — SERIALIZE MUTATION.**

Evidence grades are strict:
`GOLDEN / PROVEN / USER-CONFIRMED / SUPPORTED / PENDING / INFRASTRUCTURE / FALSIFIED / INVALID / SUPERSEDED`.
