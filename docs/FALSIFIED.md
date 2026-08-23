# CSGOVR — FALSIFIED / CLOSED

- **Assuming OpenXR view data is unavailable while the runtime is idle** — falsified by the successful idle `xrLocateViews` diagnostic.
- **Assuming a symmetric per-eye frustum is sufficient for the tested Quest 3 configuration** — falsified by the measured asymmetric horizontal FOVs for left and right eyes.
- **Treating successful OpenXR pose/FOV acquisition as proof of solved in-game stereo** — rejected as an inference.
- **Repeating broad OpenXR availability/FOV diagnostics** — closed unless new evidence specifically reopens runtime-input availability.
- **Broad camera analyzers after identifying persistent `CViewSetup` and `client.dll+0x0038D123`** — stopped as the primary path. A concrete control target already exists.
- **Mutating another camera field before classifying main-world versus shadow/aux view state** — stopped by process. Read-only classification must come first.

Keep this file narrow: failed build attempts or environment mistakes are not automatically architectural falsifications.
