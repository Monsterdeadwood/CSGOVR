# CSGOVR — NEXT EXPERIMENT

## Decision
**KEEP GOLDEN, integrate known projection.** Do not reopen broad OpenXR/camera/culling archaeology before this integration is tested.

## Primary question
Can the proven Quest 3 asymmetric LEFT/RIGHT projection be applied through the known-good v0.5.5 HLAE projection path while preserving the existing two-world-stream stereo GOLDEN and monitor pass?

## PREFLIGHT — exactly 5 lines
**MISSING FACT:** whether the already-proven Quest asymmetric projection coefficients, when fed through the proven `CViewSetup` custom/off-center path, produce correct in-headset eye geometry without destabilizing the v0.5.5 stereo schedule.
**WHY NOT COLD:** field layout, eye FOV and projection semantics are already proven COLD/causally; only headset/same-frame LIVE can establish visual correctness and regression of the integrated eye path.
**PASS:** LEFT/RIGHT retain real distinct world streams, Quest asymmetry is applied to the correct eye, monitor pass remains intact, geometry/depth is comfortable and coherent in-headset, and no new eye-specific corruption appears.
**FAIL:** symmetric/incorrect frustum remains, eye asymmetry is swapped/wrong, one stream regresses, monitor pass breaks, final raster ignores the matrix/off-center change, or a new visible culling/camera defect appears.
**BASELINE MUST HOLD:** `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`, full `CViewSetup=0x198`, pass1 LEFT/pass2 RIGHT/pass3 monitor, custom projection path `+0x68/+0x6C..+0xAB`, exact restore of the untouched baseline path.

## Inputs already proven
Quest FOV:
- LEFT L=-54°, R=+40°, U=+44°, D=-55°
- RIGHT L=-40°, R=+54°, U=+44°, D=-55°

Known projection controls:
- custom projection enable `+0x68`
- matrix `+0x6C..+0xAB`
- off-center top `+0x138`
- bottom `+0x13C`
- left `+0x140`
- right `+0x144`
- `m_bOffCenter` flags `+0x148 bit0`

## Mutation scope
One integration only:
1. preserve v0.5.5 pass scheduling and full-object copying;
2. assign the proven LEFT Quest projection only to LEFT pass;
3. assign the proven RIGHT Quest projection only to RIGHT pass;
4. preserve pass3 monitor behavior;
5. do not add origin/yaw/culling mutations in the same build;
6. restore stock/Golden projection state when experiment is disabled.

## Same-run evidence
Collect:
- eye/pass id and applied projection coefficients
- matrix/off-center path selected per pass
- pass counts LEFT/RIGHT/monitor
- same-frame screenshot/capture if available
- headset user verdict for depth/fusion/peripheral geometry
- baseline restore confirmation

Sequential screenshots taken at different times are not quantitative disparity/culling proof. Prefer same-frame capture or headset comparison under deterministic controls.

## Prediction
If the remaining projection mismatch is simply that known Quest asymmetry has not been installed into the already-proven eye path, this mutation should improve the headset geometry while leaving v0.5.5 stereo scheduling untouched.

## Conditional next step
Only if a concrete geometry/culling blocker remains after this PASS/FAIL:
- use `client.dll+0x003A16A6` / `+0x003A01E4` / CSimpleWorldView lineage;
- do not reopen `client.dll+0x0525BA60` both-eye cache reuse;
- do not promote `+0x0038D123/+0x052632CC` infrastructure seams to camera ownership without new causal proof.

## After PASS
Freeze projection integration. Define the next gate from the actual remaining headset-visible blocker, not from historical speculation.
