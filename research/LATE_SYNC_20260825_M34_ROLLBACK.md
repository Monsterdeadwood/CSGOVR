# CSGOVR late cross-tab reconciliation — 2026-08-25

## Why this note exists
Several CSGOVR branches/tabs advanced at different speeds. Repository pages still contained older current-gate statements (T0/G2.5 transport certification and later M3.2A slot-depth A/B) after newer runtime evidence had already moved the project to full HMD 6DoF and then into viewmodel/hand integration.

This note reconciles those branches without discarding older evidence.

## Accepted runtime authority
Last user-confirmed runtime GOLDEN:

`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

SHA256:
`5E4092F2407D8C2C9C8E6C9B1CC11826099534D088F1F9E078921A72ACFFF6B2`

Runtime evidence:
`CSGOVR_MASTER32B_HMD_6DOF_RESULT_20260825_183719.zip`

User visual verdict:
**VR + full HMD 6DoF works.**

The remaining tearing is real but is classified as a quality/cadence defect. It does not invalidate the already-working world-camera architecture.

## Gates now considered closed
- MASTER1 transport
- MASTER2 Quest projection
- MASTER3.1 native origin ownership at `client+0x003A5280`
- MASTER3.2A real Quest HMD rotation
- MASTER3.2B positional HMD translation / full 6DoF

These should not be re-researched absent contradictory evidence.

## Pose-coherence branch retained, not discarded
`CSGOVR_MASTER32A_POSE_COHERENCE_RESULT_20260825_194358.zip` remains valid instrumentation evidence.

It proved:
- host identity PASS
- native guard PASS
- Quest matrix PASS
- pair proof PASS
- LEFT/RIGHT same pose PASS
- exact render-pose submission PASS
- all 9 phases PASS
- pose lookup misses 0
- capture failures 0

It also found `droppedPairs=179`, consistent with two-slot starvation, plus large stale bursts and occasional eye-gap spikes.

That cadence finding remains useful for later tearing/performance work. It is no longer the main architecture gate because later integration continued past that point.

## M33 convergence
M33 reached real game pairs using the slot8/controller-capable transport lineage.

This matters because it weakens any theory that the later M34 first-render failure is simply the old two-slot starvation problem.

Do not promote M33 above the user-confirmed M3.2B GOLDEN unless its own complete runtime package/result is separately frozen and validated.

## M34 v0.2.x failure boundary
Status:
`INVALID / FROZEN`

Do not patch it forward and do not adopt it as a new baseline.

Evidence from `_RUN_20260825_221228`:
- CS:GO launched
- `de_dust2` loaded
- transport DLL injection returned non-zero `LoadLibraryW`
- hand-probe DLL injection returned non-zero `LoadLibraryW`
- NETCON ends at first `mirv_streams record start; echo CSGOVR_M34_START`

Therefore the failing boundary is the **first HLAE stereo render after both injections**.

The run does not implicate:
- Zig build
- game launch
- map loading
- LoadLibrary injection itself

## Why M34 is causally invalid
Before that same first render, M34 enabled two coupled changes:

1. `CSGOVRLEFT/RIGHT drawViewModel 0 -> 1`
2. passive/global `C_BaseAnimating::BuildTransformations` vtable index 184 hook in `hand_probe.dll`

Because both became active before the observed failure, M34 cannot distinguish which mutation caused it.

This violates the desired serial-mutation rule even though the package itself gathered useful boundary evidence.

## Correct rollback / next lane
Return to exact M3.2B v1.1.1 GOLDEN.

Then isolate:

### A — baseline control
Exact GOLDEN, no new viewmodel or hand changes.

### B — viewmodel render only
Change only LEFT/RIGHT `drawViewModel=1`.
No `hand_probe.dll`.
No controller IPC.
No transform mutation.

### C — passive skeleton seam only
Only after B PASS, add passive/global `C_BaseAnimating::BuildTransformations` observation.
No controller IPC.
No pose mutation.

### D — controller IPC
Only after C PASS, add controller IPC while leaving viewmodel transformation passive.

### E — transform ownership
Only after D PASS, mutate the viewmodel transform and establish weapon/hand ownership.

## Multi-session one-click policy
A future runner may perform A/B/C in one user action only if they are separate processes/sessions with automatic close, independent baseline checks and per-session verdicts.

This is compatible with the project rule:
**several facts per user action, one primary mutation per process.**

## Current quality side-lane
Tearing remains open.
Potential transport cadence/host pacing work belongs to its own quality/performance lane unless it prevents a viewmodel causal verdict.

Do not use tearing as justification to reopen projection, origin, rotation or positional 6DoF ownership.

## Supersession map
The following old current-gate statements are historical snapshots:
- T0 / G2.5 GPU XR transport certification
- queued G3/T1 Quest projection as not yet integrated
- MASTER3.2A 2-slot vs 8-slot A/B as the active gate

They remain valuable historical evidence but are no longer operational NEXT.

## Operational current gate
**Viewmodel reintroduction / hand-path isolation on exact M3.2B GOLDEN.**

Primary next missing fact:
**Does `drawViewModel=1` alone survive the first HLAE stereo render?**
