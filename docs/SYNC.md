# CSGOVR — CROSS-BRANCH STATE SYNC

Sync time: **2026-08-24**  
Remote base inspected before mutation: `main@20f76e6bbd849b5aff5f25d00ae63505ef1b96c0`

## CROSS-BRANCH CONFLICT CHECK
Remote-main had already prepared a narrow Quest projection child and marked projection integration as current. Its own `research/RESULT_INDEX.md`, however, did **not** contain an exact completed XR transport/headset PASS.

Current transport branch produced a stronger prepared transport candidate but no LIVE result yet: `CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF`.

These are not contradictory scientific results:
- remote projection branch says projection inputs/implementation are ready;
- transport branch says final two-eye HLAE -> Quest submission remains unproven.

**Resolved common state: XR TRANSPORT first. Projection child remains queued and is not discarded.**

No other GitHub branch existed at sync time besides `main`; reconciliation uses accumulated main history plus retained CSGOVR evidence state.

## LATEST RESULT
No new completed LIVE after the canonical 2026-08-23 result family.

Newest prepared candidate:
- `CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF`
- artifact SHA256 `DFEA881888C83812A5B9EE3A7049F79D32935240F2C612DE4ABDF4F9A98834A3`
- PREPARED / STATIC-AUDITED / NOT YET LIVE-PROVEN
- supersedes prepared v0.7.0 before LIVE

## PROVEN
- frozen target identity and client hash
- v0.5.5 HLAE stereo GOLDEN
- pass1 LEFT / pass2 RIGHT / pass3 monitor
- full `CViewSetup=0x198`
- HLAE DoRenderView `+0x22340`, copy ctor `+0x14BA0`
- custom projection `+0x68/+0x6C..+0xAB`
- off-center `+0x138/+0x13C/+0x140/+0x144`, flag `+0x148.bit0`
- Quest 3 FOV/coefficients and recommended eye size
- D3D9 Present device-latch and CPU readback primitive
- `GeneratePerspectiveFrustum +0x86E9C0`
- pre-eye reader `+0x3A16A6`
- temporary arg2 `0x40` transpose/restore
- CSimpleWorldView lineage and lazy-hydration machinery

## FALSIFIED / STOPPED
- `0x14C` full CViewSetup
- `+0x0525BA60` shared-inside-eyes reuse
- persistent arg2 output outside `0x40`
- old post-SetUpView consumer candidates
- sequential screenshots as quantitative same-frame oracle
- origin/yaw infrastructure windows promoted as ownership
- broad OpenXR rediscovery
- Present as eye oracle
- forced D3D9Ex first
- HLAE preview scheduler replacement before P0 transport verdict
- v0.7.0 prepared build, superseded by v0.7.1 before LIVE

## CURRENT FINALISTS
1. HLAE eye pass -> CPU readback -> atomic shared pair -> x64 D3D11/OpenXR.
2. After transport PASS: exact v0.5.5 + known Quest asymmetric custom matrix only.
3. If culling still visibly fails: `+0x3A16A6` + CSimpleWorldView lineage.

## CONCRETE TARGETS
Critical current ones:
- `client.dll+0x003A5280`
- `AfxHookSource.dll+0x00022340`
- `AfxHookSource.dll+0x00014BA0`
- `CViewSetup+0x68/+0x6C..+0xAB`
- `client.dll+0x0086E9C0`
- `client.dll+0x003A16A6`
- `client.dll+0x003A01E4`
- `client.dll+0x003B0870`
- `client.dll+0x0524A2F4/+0x0524A2F8/+0x0524A2FC`, stride `0x13F0`

## CURRENT GATE
**XR TRANSPORT**

## NEXT
Run `CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF` exactly once under the frozen lab contract and return its RESULT ZIP plus headset observation.

PASS -> freeze transport -> projection integration.  
FAIL -> adapt the exact failed transport stage only.

## METHOD ALREADY USED
- source/PDB/SDK/donor harvest
- exact static mapping
- HLAE source/binary seam mapping
- HLAE multipass runtime proof
- D3D9 device latch/readback/MSAA resolve
- projection/off-center causal canaries
- Quest OpenXR diagnostic
- TTD frustum/cache replay and reader recovery
- origin/yaw byte-integrity controls
- CSimpleWorldView class/lazy-hydration mapping
- public transport donor comparison
- exact AdvancedFX D3D9Ex historical review

## IMPORTANT STATE-ORDER NOTE
The existing repository projection child, verifier, CI and Issue #1 are retained. They are **queued work**, not deleted. The only correction is gate ordering: they begin after XR TRANSPORT PASS unless a new exact result proves transport already closed.
