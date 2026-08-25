# CSGOVR MASTER 1 — XR TRANSPORT LIVE RESULT

Date: 2026-08-25 EEST
Artifact: `CSGOVR_XR_TRANSPORT_RESULT_20260825_044816.zip`
Artifact SHA256: `FE51FA49B0108FC31E70E4D2F16C25F9BD160CDA7B654841AD64E19B61FB99E7`

## VERDICT

**MASTER 1 = PASS**

Frozen target and HLAE identity passed before LIVE:
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- HLAE 2.191.1 exact
- Meta/Oculus OpenXR runtime active

Raw runtime evidence overrides the runner's early stale `RESULT_STATUS.txt` false-negative:
- `PAIR PROOF PASS id=1 gen=1 pairProof=1 equal=0`
- 123 produced / 123 consumed complete pairs
- `drops=0`
- `captureFailures=0`
- OpenXR frames switched from `source=DIAG` to `source=GAME`, `layerCount=1`
- exact same-outer proof `0->1 / 1->2`
- LEFT/RIGHT hashes differ
- first saved pair 960x540
- first pair `leftCaptureUs=4459`, `rightCaptureUs=2451`, `consumeAgeMs=27.518`, `eyeGapMs=13.498`

Headset observation from the run: a strange CS:GO image was visible in VR briefly. This is compatible with the transport-only BASE-projection gate and is not a projection correctness verdict.

The later `staleMs` increase begins after the runner intentionally issues `mirv_streams record end`; it is not evidence of a transport freeze.

## RECONCILIATION

The old generated `RESULT_STATUS.txt` was written before the later host evidence arrived and said no complete pair was published. That line is superseded by the later raw `XR_HOST.log` pair proof and GAME submissions.

## NEXT GATE

**MASTER 2 — exact Quest 3 matrix-only causal raster proof.**

Do not open A2, pose, hands, culling, performance, or MASTER 4 before their legal gates.