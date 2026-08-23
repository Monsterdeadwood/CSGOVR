# CSGOVR — ROADMAP

1. Preserve the confirmed OpenXR/Quest 3 eye/FOV baseline as the numerical oracle.
2. Map the CSGO/Source retail view/projection writer and render-pass schedule.
3. Inject the measured asymmetric per-eye projection at one proven point.
4. Establish one-frame stereo with one latched HMD pose and correct eye separation.
5. Audit culling, overlays and post effects for mono assumptions.
6. Add independent HMD camera and controller/weapon pose only after stereo stays green.
7. Package regression diagnostics that compare game-side matrices against the OpenXR oracle.

The next breakthrough target is a concrete game-side view/projection target, not more OpenXR probing.
