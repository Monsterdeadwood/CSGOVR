# CSGOVR — PROVEN

## OpenXR / Quest 3 diagnostic baseline
- Tested runtime: Meta/Oculus OpenXR 1.205.0 with Quest 3.
- `XR_KHR_win32_convert_performance_counter_time` was reported available in the diagnostic path.
- `xrConvertWin32PerformanceCounterToTimeKHR` returned success in the recorded idle test.
- `xrLocateViews` returned `XR_SUCCESS` in idle mode with `viewCount = 2` and `viewStateFlags = 0xF`.
- Recorded eye FOVs:
  - LEFT: L=-54°, R=+40°, U=+44°, D=-55°
  - RIGHT: L=-40°, R=+54°, U=+44°, D=-55°
- Recommended per-eye image size observed: 1504x1616.

## Derived projection reference
From the recorded FOVs, an independent projection calculation produced approximately:
- both eyes: M00 ~= 0.9027383311
- both eyes: M11 ~= 0.8354788620
- both eyes: M12 ~= -0.1931874395
- LEFT M02 ~= -0.242512 (RIGHT carries the mirrored horizontal asymmetry under the same convention)

## Retail game-side camera control — CONTROL PROVEN
- Persistent `CViewSetup` object: `client.dll+0x052632CC`.
- Proven field: `origin.x` at object offset `+0xB8`.
- Retail control/writer instruction: `client.dll+0x0038D123`.
- Downstream render boundary: `client.dll+0x003A5280` (`RenderView`).
- A targeted causal experiment demonstrated that changing the proven camera-origin path controls the persistent object and the change reaches the downstream `RenderView` path.

## Current interpretation boundary
- The project now has a proven game-side camera ownership/control anchor, not merely OpenXR input.
- Projection/FOV/frustum fields on the persistent view lineage are not yet causally mapped.
- `client+0x05263788` is a current read-only classification path/target, not yet a proven projection/culling field.
- Valid OpenXR eye data plus a proven camera origin path still does **not** by itself prove correct stereo rendering.
