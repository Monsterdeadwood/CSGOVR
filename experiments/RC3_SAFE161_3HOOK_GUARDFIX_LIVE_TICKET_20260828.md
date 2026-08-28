# CSGOVR SAFE161 RC3 3-HOOK GUARD-FIX LIVE TICKET

Snapshot: 2026-08-28

Package: `CSGOVR_ULTIMATE_SAFE161_NATIVE_AIM_RC3_3HOOK_GUARDFIX_ONECLICK.zip`

Package SHA256: `B988DEB8DEE490CFB6964ADA004DF69F454AF48A689BDD60E2A67CCE5B72C2FB`

Visual/render ancestor: `CSGOVR_WEAPON_SAFE_POSTCALC_AIM_v1.6.1_ONECLICK`.

Prior visual authority for ancestor: distant textures remained present and shot sound was present.

Failed parent evidence: `CSGOVR_ULTIMATE_NATIVE_AIM_RC2_3HOOK_RESULT_20260828_205342.zip`.

## PREFLIGHT

MISSING FACT: whether the user-verified SAFE161 visual baseline remains intact while the relocation-aware three-hook CUserCmd native-aim path actually activates and owns authoritative shot direction.
WHY NOT COLD: the ASLR guard, currentCmd owner, register map and hook topology are cold-verifiable, but actual hook installation, prediction/network consumption, authoritative impacts and far-world visual stability require execution and the user's eyes.
PASS: SAFE161 far textures/objects and shot audio remain stable; three hooks activate with RunCommand/TLS absent; client publish/client gate/server currentCmd gate occur; impacts follow RIGHT Touch while stock Weapon_ShootPosition, recoil, spread and ammo remain normal.
FAIL: any far-world/texture disappearance, native aim inactive, center-screen authoritative fire, unauthorized pointer grant, currentCmd/ring mismatch, crash/audio regression, or stock recoil/spread/origin/ammo semantics change.
BASELINE MUST HOLD: exact frozen hashes; SAFE161 user-verified textures/audio; M3.2B HMD 6DoF/Quest stereo; exact slot250 RIGHT-hand follow; stock Weapon_ShootPosition/recoil/spread/ammo; no attachment/DrawModel/FX/global-pointer mutation.

## Single mutation

The protected SAFE161 renderer/HMD/transport path is retained. The gameplay delta is only the three-hook native command-owned aim integration plus relocation-aware runtime verification.

```text
HOOK 1 CLIENT CheckSplitScreenMimic
HOOK 2 CLIENT IsInputDeviceAPointer
HOOK 3 SERVER IsInputDeviceAPointer
SERVER RunCommand hook = ABSENT
SERVER native-aim TLS = ABSENT
FX_FireBullets = STOCK
```

Server final-aim authorization requires exact approved caller, caller-specific player register, `player+0x1034` current command, command/player aim mirror equality, exact provenance ring identity, and matching local-player index.

## Guard correction

`server+0x005BC67A` contains an `A1 moffs32` base-relocated operand. Runtime guard compares stable opcodes and requires the live operand to equal `actual server base + 0x00A968E0`. It no longer compares the live relocated operand against preferred-image bytes.

## Pre-LIVE package audit

- exact 5-line preflight: PASS
- source hook structure: exactly 3 hooks
- RunCommand/TLS absent from install block
- relocation-aware runtime guard present
- i686 naked server-register wrapper compile: PASS
- protected `xr_host.cpp`, injector, controller protocol, transport protocol and Quest matrix byte-identical to SAFE161 child: PASS
- protected HMD/render functions function-for-function identical: PASS
- PVS/T14/T15 mutation absent from this delta
- package manifest verify: PASS
- ZIP integrity: PASS

## Promotion law

Machine counters are plumbing evidence only. Do not promote unless the user separately confirms both:

1. distant world textures/objects remain stable and shot audio remains present;
2. impacts follow RIGHT Touch rather than screen center.

If far-world disappearance remains on this SAFE161-based child, classify it as a separate culling/PVS failure and return to the T14/T15 quality lane. Do not add a PVS mutation to the weapon experiment.
