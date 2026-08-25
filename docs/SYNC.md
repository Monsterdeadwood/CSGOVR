# CSGOVR — STATE SYNC

Snapshot: 2026-08-25 after MASTER 1 PASS, MASTER 2 PASS, MASTER 3.1 native origin PASS, and MASTER 3.2A real Quest HMD rotation PASS.

## AUTHORITY / CURRENT PROGRAM
Execution order remains strict:
`MASTER 1 -> MASTER 2 -> MASTER 3 -> MASTER 4`.
Prepared packages are not runtime proof. Raw runtime result > generated status summary > plan/README.

Frozen baseline:
- legacy CS:GO x86, AppID `4465480`, BuildID `22100934`
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- HLAE 2.191.1 exact
- v0.5.5 schedule: pass1 LEFT / pass2 RIGHT / pass3 monitor
- full `CViewSetup = 0x198`
- `-insecure`, local/offline only

## MASTER 1 — PASS
Retain complete-pair CPU transport to Meta OpenXR. Do not reopen.

## MASTER 2 — PASS
Retain exact Quest asymmetric projection through `CViewSetup +0x68/+0x6C`. Do not rerun.

## MASTER 3.1 — PASS
Result `CSGOVR_MASTER31_EARLY_ORIGIN_RESULT_20260825_085119.zip` plus cold depth-parallax analysis proved native camera-origin authority at `client.dll+0x003A5280` using a local full `CViewSetup`.

Winning seam:
- native RenderView `client+0x003A5280`
- local `CViewSetup 0x198`
- origin `+0xB8` proven surviving
- angles `+0xC4` proven usable by M3.2A

Do not repeat M3.1.

## MASTER 3.2A — PASS: REAL QUEST HMD ROTATION
Result:
`CSGOVR_MASTER32A_HMD_ROTATION_RESULT_20260825_172017.zip`

SHA256:
`BC21C70411C60564620D13C580AFF925EE789231D61059E2269C99FE76DBF539`

Raw status:
- `CAUSAL_ATTEMPTED=True`
- `XR_SESSION_PASS=True`
- `XR_DIAG_PASS=True`
- `QUEST_MATRIX_PATH_PROOF_PASS=True`
- `NATIVE_RENDERVIEW_GUARD_PASS=True`
- `HMD_ROTATION_APPLY_PASS=True`
- `M1_TRANSPORT_RETAINED=True`
- `SAME_HLAE_PAIR_PASS=True`
- `XR_GAME_LAYER_PASS=True`
- `ANALYZER_EXIT=0`

Analyzer:
- `MASTER32A_INSTRUMENT=PASS`
- HMD pose publisher PASS
- rotation base anchor PASS
- engine rotation handshake PASS
- LEFT/RIGHT HMD apply PASS
- angular range PASS
- M1 pair proof PASS
- OpenXR `source=GAME pose=ROT_LIVE_ANCHOR_POS` PASS

Minimum automated angular range:
- pitch `6.081 deg`
- yaw `18.126 deg`
- roll `2.863 deg`

Later live telemetry expanded to approximately pitch `22.218`, yaw `51.577`, roll `21.683` degrees.

The host published live relative quaternions while the x86 eye DLL applied the same latched pose to LEFT and RIGHT. Native eye recenter/IPD remained coherent. OpenXR submitted live orientation with anchored position, matching the intentional rotation-only render camera.

User visual observation: **almost normal VR**.

Scientific verdict: **PASS**. Do not repeat rotation-only proof.

Detailed record:
`results/MASTER32A_HMD_ROTATION_20260825_172017_PASS.md`

## PREPARED MASTER 3.2B v1.1.0 — HMD POSITION ONLY
Package:
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.0_LIVE_ONECLICK.zip`

SHA256:
`BB4EB0259880AFB71307B4AFA407BD7DF29A69008EBBB4F6EA9A9B95BD632994`

Static verification: 26/26 PASS.
Project-defined use-before-declaration audit: zero issues in host and eye translation units.

Only new causal variable: HMD head-center positional translation.

Host path:
- retain proven relative quaternion
- average live LEFT/RIGHT eye positions to current head center
- average anchor LEFT/RIGHT eye positions to anchor head center
- subtract centers
- rotate delta by inverse anchor orientation to anchor-local OpenXR basis
- publish relPos meters in existing header reserved fields without changing 332-byte ABI

x86 camera path:
- latch one complete HMD pose on LEFT and reuse RIGHT
- retain proven rotation composition
- convert physical +X right, +Y up, -Z forward using the base game camera forward/right/up vectors
- scale by `39.37007874015748` Source units/meter
- add translation to the live game camera center
- rebuild stereo IPD around translated/rotated center
- caller-owned `CViewSetup` untouched

OpenXR coherence:
- before producer full-pose handshake: use anchor
- after handshake: submit matching full live per-eye pose as `POSE_LIVE`

Excluded:
hands/controllers, weapon/aim/muzzle, culling, A2/GPU replay, MASTER4.

## CURRENT GATE
**MASTER 3.2B — Quest HMD positional translation only, with M3.2A rotation frozen PASS.**

## NEXT
Run only `00_RUN_MASTER32B_HMD_6DOF.bat`. During the explicit LIVE window keep mouse still and lean head left/right, forward/back, up/down. Return the result ZIP plus visual observation.

If M3.2B passes, MASTER3 HMD camera is real 6DoF and the next bounded work is controller/weapon ownership. If it fails with valid position telemetry, adjust only sign/axis/scale or pose coherence. Do not reopen origin authority or rotation.
