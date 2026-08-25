# CSGOVR — NEXT

Snapshot: 2026-08-25 after MASTER 3.2A real Quest HMD rotation PASS from result `CSGOVR_MASTER32A_HMD_ROTATION_RESULT_20260825_172017.zip`.

## CLOSED GATES
MASTER 1 = PASS.
MASTER 2 = PASS.
MASTER 3.1 native camera-origin ownership = PASS.
MASTER 3.2A real Quest HMD rotation = PASS.

Do not repeat transport, Quest projection, +32 origin ownership, or rotation-only proof.
A2 remains parked until MASTER4 PERF numerically FAILS.

## MASTER 3.2A PASS
Result SHA256:
`BC21C70411C60564620D13C580AFF925EE789231D61059E2269C99FE76DBF539`

Runtime/analyzer evidence:
- native RenderView guard PASS at `client.dll+0x003A5280`
- exact Quest matrix retained
- HMD pose publisher PASS
- one pose latched on LEFT and reused RIGHT
- `HMD_ROT_BASE_ANCHOR` PASS
- `HMD_ROT_HANDSHAKE engineRotationActive=1`
- LEFT/RIGHT HMD rotation apply PASS
- angular range PASS
- M1 same-HLAE pair proof retained
- OpenXR GAME projection used `pose=ROT_LIVE_ANCHOR_POS`
- pair transport remained distinct with zero drops/capture failures in the proof window

Measured minimum proof range:
- pitch `6.081 deg`
- yaw `18.126 deg`
- roll `2.863 deg`

Later live span reached roughly pitch `22.218`, yaw `51.577`, roll `21.683` degrees.

User visual observation: **"almost normal VR"**. This matches the telemetry and closes M3.2A positively.

Detailed record:
`results/MASTER32A_HMD_ROTATION_20260825_172017_PASS.md`

## CURRENT GATE
**MASTER 3.2B — real Quest HMD POSITION ONLY, retaining the proven rotation path.**

Only one new causal variable is added: relative HMD head-center position.

## PREPARED M3.2B v1.1.0
Package:
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.0_LIVE_ONECLICK.zip`

SHA256:
`BB4EB0259880AFB71307B4AFA407BD7DF29A69008EBBB4F6EA9A9B95BD632994`

Static verification: 26/26 PASS. Project-defined use-before-declaration audit: zero issues in `xr_host.cpp` and `eye_transport.c`.

Causal design:
1. retain M3.2A relative quaternion unchanged;
2. host computes stereo eye midpoint at the GAME anchor and at current `xrLocateViews`;
3. current-center minus anchor-center is rotated by inverse anchor HMD orientation, yielding anchor-local OpenXR translation;
4. publish translation in meters through unused shared-header reserved fields, keeping the M1 header ABI at 332 bytes;
5. x86 eye DLL latches one complete pose on LEFT and reuses it on RIGHT;
6. map physical +X right / +Y up / -Z forward into base game camera forward/right/up;
7. scale meters by `39.37007874015748` Source units per meter;
8. add only that translation to the live game camera center;
9. retain proven HMD rotation and rebuild IPD around the translated/rotated head center;
10. after producer full-pose handshake, OpenXR submits the matching full live per-eye pose as `POSE_LIVE`.

Excluded:
hands, controller aim, weapon, muzzle, culling, A2/GPU fast path, MASTER4.

## RUN
Use only:
`00_RUN_MASTER32B_HMD_6DOF.bat`

During the explicit LIVE window keep the mouse still and lean the headset/head left/right, forward/back and up/down. Rotation should remain as good as the M3.2A PASS.

## PASS
Full pose publication + both-eye 6DoF application + positional range proof + retained M1 pair proof + OpenXR `pose=POSE_LIVE`, and user sees natural positional head movement with coherent depth parallax.

## FAIL
Valid positional telemetry/application exists but final headset translation is wrong in sign, scale, axis, or coherence. If so, adapt only that mapping/scale. Do not reopen camera ownership or rotation.

## AFTER PASS
MASTER3 HMD camera ownership is closed as real 6DoF. Next bounded lane is controller/weapon ownership, not broad renderer reverse.
