# CSGOVR — FALSIFIED

- **Assuming OpenXR view data is unavailable while the runtime is idle** — falsified by the successful idle `xrLocateViews` diagnostic.
- **Assuming a symmetric per-eye frustum is sufficient for the tested Quest 3 configuration** — falsified by the measured asymmetric horizontal FOVs for left and right eyes.
- **Treating successful OpenXR pose/FOV acquisition as proof of solved in-game stereo** — rejected as an inference. The game render path still requires independent runtime proof.

Keep this file narrow: failed build attempts or environment mistakes are not automatically architectural falsifications.
