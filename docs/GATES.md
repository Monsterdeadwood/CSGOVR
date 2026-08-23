# CSGOVR — GATES

## G0 — OpenXR / Quest input
**PASS.** Two valid Quest 3 views, asymmetric FOV and projection coefficients are already established.

## G1 — HLAE stereo GOLDEN
**PASS.** `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX` provides two real world streams: LEFT, RIGHT, plus monitor pass; full `CViewSetup` copy size `0x198`.

## G2 — Projection-control semantics
**PASS.** Custom projection bool `+0x68`, matrix `+0x6C..+0xAB`, and off-center fields `+0x138/+0x13C/+0x140/+0x144` with flags `+0x148` bit0 are established strongly enough for integration. M00/M02/M11/M12 were causally measured.

## G3 — Quest asymmetric projection integration
**PASS:** integrate the known Quest 3 asymmetric projection into the known-good LEFT/RIGHT eye path, preserve the stereo GOLDEN, and validate in-headset/same-frame.

**CURRENT GATE: G3**

## G4 — Concrete remaining culling blocker, if any
Only opens if a visible blocker survives G3.

Current evidence anchors if needed:
- pre-eye reader `client.dll+0x003A16A6`
- second reader `client.dll+0x003A01E4`
- `GeneratePerspectiveFrustum` `client.dll+0x0086E9C0`
- CSimpleWorldView lineage PROVEN

Do not reopen `client.dll+0x0525BA60` both-eye reuse hypothesis; it is FALSIFIED.

## G5 — Authoritative eye translation / camera integration
Late `DoRenderView` origin nudge was not authoritative for final captured raster. Resolve only if it remains a visible blocker after projection integration.

## G6 — Weapon / controller separation
Proceed only after stereo/projection/culling/camera gates are regression-green.
