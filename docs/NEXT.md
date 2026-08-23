# CSGOVR — NEXT

## CURRENT GATE
**XR TRANSPORT / headset-submission precondition.**

The Quest asymmetric projection child is already prepared and remains the immediate next gate after transport PASS.

## NEXT ONE STEP
Run:
`CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF`

Artifact SHA256:
`DFEA881888C83812A5B9EE3A7049F79D32935240F2C612DE4ABDF4F9A98834A3`

Manifest:
`runtime/children/v0.7.1_xr_transport/README.md`

## PREFLIGHT
**MISSING FACT:** can the already-proven HLAE pass1/pass2 world images from one HLAE outer-pass contract be transported into a live Quest 3 OpenXR projection layer?  
**WHY NOT COLD:** HLAE scheduling, D3D9 readback, OpenXR input and projection controls are already known; only a LIVE can close end-to-end submission.  
**PASS:** T0 XR diagnostic green; one structural same-HLAE L/R pair crosses x86->x64; both eyes non-black/RGB-distinct; game projection layer submits; no >2 s freeze.  
**FAIL:** exact stage evidence identifies T0 XR, HLAE pair, D3D9 capture, shared memory, x64 upload or XR submission failure.  
**BASELINE MUST HOLD:** v0.5.5, `CViewSetup=0x198`, pass1 LEFT/pass2 RIGHT/pass3 monitor, exact frozen hashes, no culling/head/weapon mutation.

## Evidence required from the run
- T0 host/session/diagnostic state
- same `CAfxStreams*` for both eyes
- LEFT `m_DoRenderViewCount 0 -> 1`
- RIGHT `1 -> 2`
- producer `DISTINCT PAIR PROOF`
- host `PAIR PROOF PASS`
- RGB hashes / non-black status
- pair age and LEFT->RIGHT gap telemetry
- GAME layer submit status
- one exact same-pair LEFT/RIGHT capture
- cleanup/regression status

## PASS path
1. Freeze v0.7.1 transport as the accepted transport baseline.
2. STOP transport research.
3. Run the already-committed byte-locked Quest projection child:
   - `experiments/quest_projection_child/eye_patch_quest_projection.c`
   - `experiments/quest_projection_child/QUEST_PROJECTION_MINIMAL.patch`
   - `tools/verify_quest_projection_child.py`
4. If projection convention still needs discrimination, use only the bounded `P00..P07` family. Do not rediscover FOV or offsets.

## FAIL path
Adapt only the exact failed transport stage.

Do not reopen:
- projection math
- Quest FOV discovery
- broad camera archaeology
- `+0x0525BA60` both-eye theory
- broad origin/culling batches

until a concrete visible/runtime blocker requires them.
