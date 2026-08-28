# CSGOVR RC2 3-HOOK LIVE TICKET

Snapshot: 2026-08-28

Package: `CSGOVR_ULTIMATE_NATIVE_AIM_RC2_3HOOK_ONECLICK.zip`

Package SHA256: `2EE99EF5FDD970ECB0B560977973F47A33D566AEFCF51EAD7E285EA74FFE8F14`

Parent: `CSGOVR_ULTIMATE_NATIVE_AIM_RC1_1_SOURCEGATEFIX_ONECLICK`

Primary question: does the COLD-proven three-hook native `CUserCmd::aimdirection` architecture survive real client prediction, command transport, authoritative server consumption, and stock ballistics while preserving the protected VR baseline?

## PREFLIGHT

MISSING FACT: whether exact three-hook native-aim implementation produces controller-owned client prediction and authoritative server fire while granting pointer semantics only to the six approved gameplay consumers.
WHY NOT COLD: hook installation, real command publication/consumption timing, prediction replay and authoritative shot behavior require execution.
PASS: three hooks active; RunCommand/TLS absent; exact ring matches occur client/server; six-call allowlist grants only valid local VR commands; rejects remain stock; controller aim drives authoritative shots.
FAIL: any unauthorized pointer grant, command/player mismatch, missed normal fire path, crash/regression, or controller aim fails to own authoritative trajectory.
BASELINE MUST HOLD: frozen hashes; MASTER3.2B HMD 6DoF; slot250 hand-follow; stock Weapon_ShootPosition, spread, recoil, ammo, textures/audio and normal non-VR pointer semantics.

## COLD authority entering LIVE

- `player+0x1034 = current CUserCmd*` is frozen-retail proven from StartCommand store through FinishCommand clear.
- Server approved caller/register map is frozen-retail proven for all six gameplay final-aim consumers.
- `CUserCmd::aimdirection +0x18/+0x1C/+0x20` transport and server copy to `player+0x14AC/+0x14B0/+0x14B4` are frozen-retail proven.
- Three-hook architecture is the primary integration target.
- Former server RunCommand/TLS hook is rollback-only and must be absent from RC2.
- `FX_FireBullets` is stock in the primary architecture.

## RC2 implementation identity

```text
HOOK 1 CLIENT CheckSplitScreenMimic
HOOK 2 CLIENT IsInputDeviceAPointer
HOOK 3 SERVER IsInputDeviceAPointer
SERVER RunCommand hook = ABSENT
SERVER native-aim TLS = ABSENT
```

Server pointer gate requires exact approved return RVA, frozen caller-specific player register, `player+0x1034` current command, command/player aim mirror equality, exact provenance ring identity, and matching local player index. All rejects delegate to stock helper behavior.

## Pre-LIVE package audit

- source structural three-hook verifier: PASS
- exact five-line preflight: PASS
- PowerShell 5.1 UTF-8 BOM: PASS
- i686 naked register-capture ABI harness: PASS
- protected `xr_host.cpp`, injector, controller protocol, transport protocol, and Quest matrix byte-identical to RC1.1: PASS
- full ZIP manifest verification: PASS
- full Windows package build is intentionally performed by the one-click runner on the frozen target machine and fails closed before launch if compilation or frozen machine preflight fails.

## Required RESULT evidence

Runtime PASS requires all of:

- `NATIVE_AIM_EXACT_GUARD PASS hooks=3 ... RunCommandHook=ABSENT TLS=ABSENT`
- `NATIVE_AIM_HOOK PASS architecture=3_HOOK_NATIVE_CURRENTCMD_PROVENANCE`
- `NATIVE_AIM_PRODUCER ARMED`
- `NATIVE_AIM_PUBLISH`
- `NATIVE_AIM_CLIENT_GATE PASS`
- `NATIVE_AIM_SERVER_GATE PASS`
- HMD LEFT/RIGHT baseline alive
- slot250 `CALCVIEWMODEL_APPLY` alive
- no old server-arm/4-hook log
- no legacy FX hook activity

No promotion to runtime-proven status without the generated RESULT ZIP.