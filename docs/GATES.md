# CSGOVR — GATES

## G0 — OpenXR input baseline
**PASS:** valid two-eye view/FOV data can be acquired reproducibly from the tested runtime.

## G1 — Game-side view/projection target
**PASS:** the retail CSGO/Source view/projection writer or consumer is identified with runtime call/data evidence.

## G2 — Same-frame coherent stereo
**PASS:** both eyes render from one simulation/HMD pose with the measured asymmetric per-eye projection and correct eye separation.

## G3 — Stereo-safe culling / post path
**PASS:** geometry, overlays and post effects needed by either eye remain stable; mono assumptions are either corrected or given documented fallbacks.

## G4 — Camera / weapon separation
**PASS:** HMD camera pose and weapon/controller pose can be driven independently without reintroducing stereo regressions.

## Current gate
**G1**. OpenXR acquisition is no longer the blocker; mapping the proven XR data into the game render path is.
