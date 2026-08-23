# CSGOVR — FALSIFIED / CLOSED

Canonical source: `CSGOVR_KNOWLEDGE_BASE_CANONICAL_20260823.md`.

- **`client.dll+0x0525BA60` reused inside BOTH LEFT and RIGHT eye marker regions** — FALSIFIED. The region is real, but observed accesses in the canonical replay occurred outside the eye marker brackets.
- **Persistent helper arg2 output outside first `0x40` bytes (`+0x40..+0x1FF`)** — FALSIFIED. The first `0x40` temporary transpose/restore is separately PROVEN.
- **Earlier auto-generated FALSIFIED verdict for the first `0x40` transpose** — INVALID/SUPERSEDED parser conclusion. Do not revive it as scientific truth.
- **OriginX/yaw mutation runs as causal camera ownership proof** — rejected. They passed byte-integrity but their wrappers classify them as INFRASTRUCTURE only.
- **Late `DoRenderView` eye-origin nudge as authoritative final eye translation point** — not supported by the tested result; the nudge did not survive to the final captured raster. This does NOT falsify eye translation generally, only that tested late intervention point.
- **Assuming symmetric eye projection is sufficient for Quest 3** — falsified by measured asymmetric eye FOV.
- **Repeating broad OpenXR discovery** — closed while the proven XR baseline remains valid.
- **Sequential screenshots as quantitative disparity/culling evidence** — closed for measurement because temporal drift is significant; use same-frame capture.
- **Cross-project contaminated FULL_EXPERIMENTAL failure as renderer evidence** — INVALID. Build/root contamination proves nothing about CSGOVR rendering behavior.

Tool/debugger/build failures are not architectural falsifications unless the scientific prediction itself was exercised.
