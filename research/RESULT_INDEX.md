# CSGOVR — RESULT INDEX

This ledger tracks the transition from proven OpenXR input to proven game-side camera ownership and the remaining projection/frustum work.

## 2026-08-23 — persistent CViewSetup CONTROL PROVEN
**Class:** PROVEN / current anchor

- Retail writer/control: `client.dll+0x0038D123`.
- Persistent `CViewSetup`: `client.dll+0x052632CC`.
- Proven field: `origin.x +0xB8`.
- Downstream render boundary: `client.dll+0x003A5280` (`RenderView`).
- Targeted causal control demonstrated that the camera-origin change reaches the downstream render path.

## 2026-08-23 — read-only view classification target
**Class:** OPEN / current frontier

- Read-only path/target: `client+0x05263788`.
- Goal: classify main-world vs shadow/aux view state and locate projection/FOV/frustum/culling-relevant fields on the proven ownership lineage.
- No intrusive mutation until classification is complete.

## 2026-08-21 — TTD origin writer provenance
**Artifact family:** `CSGOVR TTD ORIGIN WRITER PROVENANCE v1.0.0`
**Class:** PROVENANCE / precursor to control proof

- Targeted `CViewSetup origin.x +0xB8` near frozen `RenderView` ownership.
- Fed the later exact retail writer/control convergence.

## 2026-08-21 — OpenXR FOV diagnostic
**Artifact:** `CSGOVR_OPENXR_FOV_DIAGNOSTIC_20260821_150332.zip`
**Class:** PROVEN XR INPUT BASELINE

Recorded facts:
- Meta/Oculus OpenXR 1.205.0 + Quest 3.
- `xrLocateViews` succeeded in idle with 2 views and flags `0xF`.
- LEFT FOV: L=-54°, R=+40°, U=+44°, D=-55°.
- RIGHT FOV: L=-40°, R=+54°, U=+44°, D=-55°.
- Recommended eye size: 1504x1616.
- Independent projection reference: M00≈0.9027383311, M11≈0.8354788620, M12≈-0.1931874395, LEFT M02≈-0.242512.

## Closed repetition paths
- Do not rerun broad OpenXR availability/FOV diagnostics without new evidence.
- Do not restart broad camera analyzers now that the persistent `CViewSetup` and writer are concrete.
- Use the proven `origin.x` path as the causal anchor while mapping projection/frustum state.
