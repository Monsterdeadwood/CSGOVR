# CSGOVR

Research and development repository for VR integration in **Counter-Strike: Global Offensive**.

This repository tracks OpenXR view/FOV research, stereo projection, camera/rendering integration, reproducible diagnostics, and the path toward a stable VR build.

## Working model

`SYNC -> THINK -> KEEP/ADAPT/STOP -> BATCH HARVEST -> CONVERGE -> PROVEN/FALSIFIED -> NEXT GATE -> SYNC`

Principle: **parallelize evidence, serialize mutation**.

## Repository map

- `docs/STATE.md` — synchronized project state
- `docs/PROVEN.md` — verified findings
- `docs/FALSIFIED.md` — disproven paths
- `docs/GATES.md` — blockers and acceptance criteria
- `docs/ROADMAP.md` — route to playable VR
- `research/` — rendering/OpenXR research
- `experiments/` — reproducible probes
- `runtime/` — VR integration code
- `tools/` — project tooling

## Scope

Primary near-term objective: preserve the confirmed OpenXR stereo/FOV baseline and use it to drive a correct in-engine stereo projection path.
