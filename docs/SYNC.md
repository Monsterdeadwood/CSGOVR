# CSGOVR — STATE SYNC

Snapshot: 2026-08-24 urgent cross-branch sync.

## REMOTE-FIRST CHECK
- repo: `Monsterdeadwood/CSGOVR`
- branch: `main`
- only visible branch at sync time: `main`
- remote HEAD before this sync: `20f76e6bbd849b5aff5f25d00ae63505ef1b96c0`
- that remote already contained the projection child integrity CI and the preceding byte-locked Quest projection candidate work
- no available branch/result had closed G3

## LATEST RESULT
- Current-gate accepted projection result: `CSGOVR_CUSTOM_PROJECTION_MATRIX_RESULT_20260821_042218.zip` — custom projection semantics PROVEN.
- XR input: `CSGOVR_OPENXR_FOV_DIAGNOSTIC_20260821_150332.zip` — Quest FOV/two views PROVEN.
- Latest forensic result overall: `CSGOVR_TTD_FRUSTUM_ARG2_PERSISTENCE_RESULT_20260823_164733.zip` — first 0x40 transpose/restore PROVEN; persistent tail FALSIFIED.
- No accepted G3 runtime/headset result yet.
- Unrun local diagnostic package: `CSGOVR_G3_QUEST_COHERENCE_BATCH_v1.0.1_SYNCED_ONE_CLICK.zip`, SHA256 `595DA174AC39BA334F725DC1D130089F7F7C98D2BC4878F87B12F113BC0EAB70`; not baseline.

## PROVEN
- GOLDEN `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`.
- pass1 LEFT world, pass2 RIGHT world, pass3 monitor.
- full `CViewSetup=0x198`; old full-size `0x14C` is SUPERSEDED.
- HLAE `DoRenderView +0x00022340`; `CViewSetup_csgo` copy ctor `+0x00014BA0`.
- HLAE object controls: `this+0x30 forceCache`, `this+0x50 DoRenderViewCount`.
- custom projection `+0x68`, matrix `+0x6C..+0xAB`; M00/M02/M11/M12 semantics causally measured.
- Quest 3 FOV and matrix coefficients are PROVEN; recommended eye size 1504x1616; passive separation ~64.780 mm.
- off-center fields `+0x138/+0x13C/+0x140/+0x144`, flag `+0x148 bit0`.
- CPU `CViewSetup.origin` -> GPU VS `c2/CAMERA_POS` exact path; `WORLD_X_PAIR` proved a small RenderView origin translation can affect pixels/draw membership.
- original v0.5.5 BMPs independently show depth-dependent L/R parallax with negligible vertical disparity.
- pre-eye reader `client+0x003A16A6` has a recovered evidence packet; destination semantics PENDING.
- first helper arg2 `0x40` bytes transpose/restore is PROVEN.
- CSimpleWorldView main-world lineage PROVEN.

## EVIDENCE CONFLICT / RECONCILIATION
Do not erase either path:
- `CSGOVR_NATIVE_STEREO_SEAM_RESULT_20260821_041928`: one tested late DUAL_ORIGIN intervention did not authoritatively change its final capture.
- GOLDEN + stock causal evidence: v0.5.5 images show real stereo parallax; WORLD_X_PAIR shows RenderView-origin translation can reach GPU/raster.

Resolution: the late intervention is a negative result for that exact point/capture, not a general falsification of eye translation. Do not restart broad TAB3 origin search before G3.

## FALSIFIED / CLOSED
- `client+0x0525BA60` reused inside BOTH eye marker regions.
- persistent helper arg2 output beyond first 0x40.
- old parser conclusion falsifying first-0x40 transpose — INVALID/SUPERSEDED.
- originX/yaw byte-integrity wrappers as camera ownership proof.
- broad OpenXR rediscovery.
- sequential screenshot comparison as quantitative culling/disparity truth.
- contaminated FULL_EXPERIMENTAL failure as renderer evidence.
- repeating sign/transpose projection roulette after v0.6.1.
- old 8–12 point eye-origin batch until a post-G3 visible origin blocker exists.

## CURRENT FINALISTS
1. `experiments/quest_projection_child/eye_patch_quest_projection.c` — byte-locked minimal G3 child; parent exact v0.5.5.
2. `experiments/quest_projection_child/QUEST_PROJECTION_MINIMAL.patch` + `tools/verify_quest_projection_child.py` + Evidence Integrity CI.
3. Unrun diagnostic support package `CSGOVR_G3_QUEST_COHERENCE_BATCH_v1.0.1_SYNCED_ONE_CLICK.zip`; matrix-only is PRIMARY-useful, FOV/aspect phases secondary G4 harvest.
4. If and only if G3 leaves a visible culling blocker: `+0x003A16A6`, `+0x003A01E4`, `+0x0086E9C0`, CSimpleWorldView.

## CONCRETE TARGETS
- frozen `RenderView client+0x003A5280`
- HLAE `DoRenderView +0x22340`
- HLAE copy ctor `+0x14BA0`
- CViewSetup custom projection `+0x68/+0x6C`
- origin `+0xB8`, angles `+0xC4`, zNear/zFar `+0xD0/+0xD4`, aspect `+0xE0`
- off-center `+0x138/+0x13C/+0x140/+0x144`, flag `+0x148 bit0`
- pre-eye readers `client+0x003A16A6`, `client+0x003A01E4`
- `GeneratePerspectiveFrustum client+0x0086E9C0`, writers around `+0x0086EB3C`
- CSimpleWorldView vtable `client+0x00BF4434`, Draw `+0x003B0870`, fields `+0x168/+0x16C/+0x170`
- retained infrastructure only: `+0x0038D123`, `+0x052632CC`, `+0x05263384`, yaw `+0x0038D15B/+0x0038D1DE`, `+0x05245AB8`

## CURRENT GATE
**G3 — integrate the already-PROVEN Quest asymmetric projection into the known-good v0.5.5 eye path and obtain a runtime/headset verdict.**

## NEXT
Run the repository byte-locked minimal projection child after integrity verification. Preserve v0.5.5 origin and pass scheduling; no culling/origin/head/weapon mutation in the same PRIMARY. If G3 passes, freeze projection and select the next gate from the actual visible blocker. If transport is black/absent, solve transport as transport. If culling visibly fails, use the retained G4 targets.

## METHOD ALREADY USED
- source/donor corpus harvest
- retail CViewSetup canaries
- D3D9 GPU constant provenance
- HLAE seam/pass archaeology
- full-0x198 eye-copy GOLDEN
- custom projection/off-center causal tests
- deterministic frozen projection capture
- OpenXR FOV acquisition
- TTD frustum/cache and targeted reader recovery
- x32dbg origin/yaw infrastructure controls
- CSimpleWorldView class funnel
- GitHub source integrity verifier + CI

## HARD CONSTRAINTS
- evidence grades stay strict
- do not upgrade INFRASTRUCTURE to ownership
- do not retest `+0x0525BA60` both-eye reuse without contradictory evidence
- preserve v0.5.5 GOLDEN
- no broad OpenXR/projection/origin archaeology before G3 verdict
- no weapon/HUD work before upstream gates
