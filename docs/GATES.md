# CSGOVR — GATES

## G0 — OpenXR input baseline
**PASS:** valid two-eye view/FOV data can be acquired reproducibly from the tested runtime.

## G1 — Retail game-side camera ownership
**PASS:** persistent `CViewSetup` ownership/control is causally anchored.

Concrete proof:
- writer/control `client.dll+0x0038D123`
- persistent `CViewSetup` `client.dll+0x052632CC`
- proven field `origin.x +0xB8`
- downstream `RenderView` boundary `client.dll+0x003A5280`

## G2 — Main-world view classification + projection/frustum map
**PASS:** classify the proven persistent view lineage as main-world versus shadow/aux state, then locate projection/FOV/frustum/culling-relevant fields without broad candidate expansion.

Current read-only target: `client+0x05263788`.

**CURRENT GATE: G2**

## G3 — Same-frame coherent stereo
**PASS:** both eyes render from one simulation/HMD pose with the measured asymmetric per-eye projection and correct eye separation.

## G4 — Stereo-safe culling / post path
**PASS:** geometry, overlays and post effects needed by either eye remain stable; mono assumptions are corrected or given documented fallbacks.

## G5 — Camera / weapon separation
**PASS:** HMD camera pose and weapon/controller pose can be driven independently without reintroducing stereo regressions.
