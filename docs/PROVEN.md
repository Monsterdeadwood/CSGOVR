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
- LEFT M02 ~= -0.242512 (RIGHT should carry the mirrored horizontal asymmetry under the same convention)

## Interpretation boundary
These values prove that valid asymmetric two-eye OpenXR view data can be acquired in the tested environment. They do **not** by themselves prove that CSGO is consuming the correct projection or rendering both eyes correctly.
