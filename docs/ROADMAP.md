# CSGOVR — ROADMAP

Snapshot: **2026-08-25 late sync after M34 rollback.**

## 1. Current gate — viewmodel reintroduction on M3.2B GOLDEN
Protected runtime baseline:
`CSGOVR_MASTER3_2B_HMD_6DOF_v1.1.1_AUTO20_ONECLICK.zip`

Goal: restore viewmodel/hand functionality without destabilizing the already-working world-camera path.

Serial sequence:
1. A — exact GOLDEN control.
2. B — `drawViewModel=1` only, no hand DLL.
3. C — only after B PASS, passive `C_BaseAnimating::BuildTransformations` observation.
4. D — only after C PASS, controller IPC.
5. E — only after D PASS, viewmodel transform mutation / weapon ownership.

Do not combine B/C/D/E in one process.

## 2. Preserve closed world-camera gates
Do not reopen without contradictory evidence:
- MASTER1 transport
- MASTER2 Quest asymmetric projection
- MASTER3.1 native origin ownership at `client+0x003A5280`
- MASTER3.2A HMD rotation
- MASTER3.2B positional HMD translation / full 6DoF

The current GOLDEN already gives user-confirmed VR + full HMD 6DoF.

## 3. Tearing / cadence quality lane
Tearing remains open but is a separate quality/performance problem.

Retain useful cadence evidence from pose-coherence work:
- exact render-pose association
- pair proof
- two-slot starvation finding (`droppedPairs=179`)
- stale bursts / eye-gap spikes
- later M33 real pairs on slot8/controller-capable lineage

Only optimize host pacing/transport cadence when it does not contaminate the current viewmodel ownership test, or when tearing itself prevents a verdict.

## 4. Weapon/controller integration
After passive viewmodel observation is stable:
- add controller IPC
- establish visible viewmodel root ownership
- only then decide weapon aim / muzzle / ballistics policy

Keep world-camera ownership separate from weapon ownership.

## 5. HUD / comfort
After weapon/viewmodel path is stable:
- HUD placement
- viewmodel near/far handling
- comfort options
- menu/gameplay transitions

## 6. Performance / regression / release
Measure before redesigning:
- stable headset FPS
- dropped/mixed pair behavior
- stale/cadence metrics
- no black/freeze
- exact Quest projection retained
- HMD 6DoF retained
- no eye-specific geometry corruption
- reversible one-click setup/restore

## Historical architecture lanes
Older T0/G2.5 GPU-vs-CPU transport finalist work, queued G3/T1 projection work and M3.2A slot-depth A/B are retained as historical evidence in the dated docs and `research/RESULT_INDEX.md`. They are not the operational CURRENT gate.

## Decision rule
After each RESULT: KEEP if distance to a playable VR build shrinks; ADAPT if stronger evidence gives a shorter route; STOP a line when falsified or when another branch closes it.
