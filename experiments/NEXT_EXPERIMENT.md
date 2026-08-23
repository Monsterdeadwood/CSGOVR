# CSGOVR — NEXT EXPERIMENT

## Decision
**XR TRANSPORT FIRST.**

The remote projection child remains valid and queued, but no exact completed headset transport PASS exists in the current evidence ledger.

## Primary question
Can v0.5.5 HLAE pass1/pass2 be captured from one HLAE outer-pass contract and submitted live to Quest 3 by the separate x64 OpenXR host?

## PREFLIGHT — exactly 5 lines
**MISSING FACT:** end-to-end same-HLAE-pair XR transport from frozen CS:GO to Quest 3.  
**WHY NOT COLD:** HLAE scheduling, D3D9 readback, Quest XR input and projection APIs are already structurally known; only a LIVE proves transport.  
**PASS:** T0 XR diagnostic green; same-HLAE L/R pair crosses shared memory; both RGB images are non-black/distinct; GAME projection layer submits; no >2 s freeze.  
**FAIL:** exact stage/API evidence identifies XR startup, HLAE pairing, D3D9 capture, shared memory, D3D11 upload or XR submission blocker.  
**BASELINE MUST HOLD:** v0.5.5, full `0x198`, pass1 LEFT/pass2 RIGHT/pass3 monitor, frozen game/HLAE hashes, no origin/culling/weapon mutation.

## Candidate
`CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF`

Artifact SHA256:
`DFEA881888C83812A5B9EE3A7049F79D32935240F2C612DE4ABDF4F9A98834A3`

Repo package/manifest:
`runtime/children/v0.7.1_xr_transport/`

## Same-run evidence
- T0 host/session/diagnostic status
- HLAE count/object provenance
- producer `DISTINCT PAIR PROOF`
- host `PAIR PROOF PASS`
- RGB hashes
- pair age / L->R gap telemetry
- GAME projection-layer submit
- one exact same-pair LEFT/RIGHT BMP
- cleanup / baseline status

## Conditional next step
PASS:
- freeze transport;
- start the already-prepared Quest projection integration;
- if convention needs a batch, restrict first family to `P00..P07`.

FAIL:
- adapt only the exact failed stage.

Do not open new reverse work before the transport verdict.
