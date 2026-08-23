# Tools

Project-authored OpenXR oracles, collectors, matrix checkers, Source/CSGO analyzers, launchers and diagnostic packagers belong here.

## Current committed tool
### `verify_quest_projection_child.py`
Purpose:
- verify exact v0.5.5 GOLDEN source identity expected by the prepared projection child;
- verify the prepared Quest projection child/source contract;
- back `.github/workflows/evidence-integrity.yml`.

This protects provenance. It is not a renderer experiment.

## Current runtime integration tool
Exact one-click transport package is retained with its child at:
`runtime/children/v0.7.1_xr_transport/`

It contains the recoverable `CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF_ONECLICK.zip` as base64 plus manifest/hash.

The package itself contains:
- `00_RUN_XR_TRANSPORT.bat` — user entry point
- `RUN_XR_TRANSPORT.ps1` — build/environment/launch/result orchestrator
- `src/eye_transport.c` — x86 HLAE/D3D9 producer
- `src/xr_host.cpp` — x64 D3D11/OpenXR consumer
- `src/transport_protocol.h` — x86/x64 shared-memory ABI
- `src/injector.c` — injector
- `PROVEN_OPENXR_FOV.ini` — frozen Quest FOV input

## Historical analyzer/result families already used
Do not recreate equivalent analyzers unless a new blocker requires it:
- HLAE source/binary seam slicers and pre-GetMatrices oracle
- custom projection/off-center causal oracles
- OpenXR Quest FOV diagnostic
- TTD frustum/cache replay
- TTD pre-eye reader recovery
- arg2 transpose/persistence probe
- origin/yaw byte-integrity controls
- CSimpleWorldView class/lazy-hydration mapping

For exact result identities and scientific verdicts see `research/RESULT_INDEX.md`.

Once a concrete target exists, hand it to the next specialized stage instead of creating another broad analyzer.
