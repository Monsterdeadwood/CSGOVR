# MASTER 3.2A — HMD ROTATION PASS

Result: `CSGOVR_MASTER32A_HMD_ROTATION_RESULT_20260825_172017.zip`

SHA256: `BC21C70411C60564620D13C580AFF925EE789231D61059E2269C99FE76DBF539`

## Runtime verdict
**PASS**

This is the first result where the real Quest HMD rotation pipeline reached the final stereo OpenXR path and the user reported that it was "almost normal VR".

## Raw status
- `MASTER32A_VERDICT=READY_FOR_VISUAL_CONFIRMATION`
- `CAUSAL_ATTEMPTED=True`
- `XR_SESSION_PASS=True`
- `XR_DIAG_PASS=True`
- `QUEST_MATRIX_PATH_PROOF_PASS=True`
- `NATIVE_RENDERVIEW_GUARD_PASS=True`
- `HMD_ROTATION_APPLY_PASS=True`
- `M1_TRANSPORT_RETAINED=True`
- `SAME_HLAE_PAIR_PASS=True`
- `XR_GAME_LAYER_PASS=True`
- `ANALYZER_EXIT=0`

Analyzer:
- `MASTER32A_INSTRUMENT=PASS`
- `POSE_PUBLISH=PASS`
- `ROT_BASE_ANCHOR=PASS`
- `ROT_HANDSHAKE=PASS`
- `LEFT_APPLY=PASS`
- `RIGHT_APPLY=PASS`
- `ROT_RANGE=PASS`
- `PAIR_PROOF=PASS`
- `XR_ROT_LIVE=PASS`

Measured angular proof window:
- pitch range `6.081 deg`
- yaw range `18.126 deg`
- roll range `2.863 deg`

Later telemetry expanded to roughly:
- pitch span `22.218 deg`
- yaw span `51.577 deg`
- roll span `21.683 deg`

## Proven path
`Quest xrLocateViews -> relative quaternion -> shared memory -> one pose latched on LEFT and reused RIGHT -> native client.dll+0x003A5280 -> local CViewSetup.angles -> rotated IPD -> OpenXR GAME projection pose=ROT_LIVE_ANCHOR_POS`.

M1 pair transport remained live with `drops=0`, `captureFailures=0`, and distinct L/R hashes.

## Interpretation
User visual observation and telemetry agree. M3.2A is closed as PASS. Do not repeat rotation-only proof.

The remaining major HMD camera component is positional translation. Proceed to M3.2B, adding only HMD head-center position while retaining the proven rotation path unchanged.
