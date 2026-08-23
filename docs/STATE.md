# CSGOVR State

## Current gate
Turn the confirmed OpenXR eye/FOV data into a correct in-game stereo projection/render path for Counter-Strike: Global Offensive.

## Current known state
- OpenXR idle acquisition succeeded on the tested Meta/Oculus runtime with Quest 3.
- `XR_KHR_win32_convert_performance_counter_time` was available and the Win32 performance-counter conversion call succeeded in the tested diagnostic path.
- `xrLocateViews` succeeded while idle with two views and a fully valid view-state flag set in the recorded run.
- Quest 3 eye FOVs are asymmetric left/right and therefore require per-eye asymmetric projection rather than a single symmetric frustum.
- Recommended eye size observed in the diagnostic path: 1504x1616 per eye.
- Derived projection matrices were calculated independently from the returned FOV values and are available as a numerical reference.

## Operating protocol
- Treat OpenXR eye data as proven input, not as proof that the game render path is already correct.
- Next work must map these proven inputs onto the CSGO/Source render/view/projection path with retail runtime evidence.
- PARALLELIZE EVIDENCE, SERIALIZE MUTATION.
- One causal render mutation at a time with a written prediction and flat-baseline regression check.

## Next
1. Map the game-side view/projection writer and render-pass schedule.
2. Prove the per-eye projection insertion point.
3. Establish same-frame stereo before controller/weapon work.
