# CSGOVR — FALSIFIED / CLOSED / DO NOT REPEAT

For conflicts, preserve both evidence paths and use corrected causal/runtime evidence first.

## Structure / projection
- `0x14C` as full `CViewSetup` size — **SUPERSEDED** by exact `0x198`.
- off-center symmetric identity `[-1,+1]` — **FALSIFIED**; normalized Source semantics are `0..1`, symmetric `[0,1]`.
- assuming symmetric eye projection is sufficient for Quest 3 — closed by measured asymmetric Quest FOV.

## Camera / origin
- treating `client.dll+0x0038D123`, `+0x0038D15B` byte-integrity mutation windows as final camera ownership — rejected; those wrappers are **INFRASTRUCTURE**.
- late HLAE `DoRenderView` eye-origin nudge as authoritative final eye translation point — not supported by the tested result. It did not change final captured raster. This does **not** falsify eye translation generally.

## Culling / frustum
- `client.dll+0x0525BA60` as a cache reused **inside BOTH** eye marker regions — **FALSIFIED**.
- persistent helper arg2 output outside the first `0x40` (`+0x40..+0x1FF`) — **FALSIFIED**.
- earlier parser-generated FALSIFIED verdict for the first `0x40` transpose — **INVALID/SUPERSEDED**; corrected result proves temporary transpose/restore.
- post-SetUpView static consumer candidates `+0x38E5D7`, `+0x38EC08`, `+0x3A0381` — **FALSIFIED**.
- TTD world-list ownership tool path at `client+0x003A0333` — tool path failed due debugger argument mismatch; do not repeat unchanged.
- world-list slot-context replay that exceeded watchdog — tool path closed; preserve existing trace.

## Measurement
- sequential screenshots as quantitative same-frame disparity/culling oracle — closed. Same-eye temporal drift exceeded the L/R difference in retained captures.

## OpenXR / transport
- broad OpenXR/FOV rediscovery while the proven Quest diagnostic remains valid — closed.
- `Present` as eye identity/frame oracle — rejected. Present may latch device ownership only; HLAE pass is the eye oracle.
- forced D3D9Ex as first transport path — STOP for now. Exact AdvancedFX history contains a D3D9Ex "dead end" caused by practical MSAA/depth-sharing limitations. Revisit only after CPU P0 works and profiling proves a bottleneck.
- HLAE preview as first scheduler replacement — STOP for current gate; preview changes viewport/layout semantics and is unnecessary before the proven recording scheduler receives a transport verdict.

## Integration/build failures that do NOT falsify renderer science
- `v0.6.5` end-to-end XRFOV failed due wiring: empty/uninitialized x86 `g_xrFovPath`, stale `g_controlPath`, and PowerShell writing a different INI path. This does not falsify OpenXR, HLAE stereo, or Quest projection math.
- cross-project contaminated FULL_EXPERIMENTAL build failure — INVALID as renderer evidence.
- prepared `v0.7.0_XR_TRANSPORT` — SUPERSEDED before LIVE by `v0.7.1_SYNC_PAIR_PROOF` after cross-branch same-pair hardening and self-contract fixes.

## Do not repeat without new contradictory evidence
- Quest FOV acquisition
- frozen `RenderView+0x003A5280`
- `CViewSetup.origin -> VS c2`
- FOV/aspect -> projection family
- existence of two real HLAE world streams
- custom projection field discovery
- `+0x0525BA60` both-eye reuse
- generic broad camera/OpenXR archaeology
