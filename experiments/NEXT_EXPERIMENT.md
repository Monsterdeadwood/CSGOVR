# CSGOVR — NEXT EXPERIMENT

Canonical state: `docs/SYNC.md`.

`CLAIM_ID = CSGO-RC3-SAFE161-3HOOK-RUNTIME`

Authoritative ticket:
`experiments/RC3_SAFE161_3HOOK_GUARDFIX_LIVE_TICKET_20260828.md`

Package:
`CSGOVR_ULTIMATE_SAFE161_NATIVE_AIM_RC3_3HOOK_GUARDFIX_ONECLICK.zip`

SHA256:
`B988DEB8DEE490CFB6964ADA004DF69F454AF48A689BDD60E2A67CCE5B72C2FB`

## Why this replaced T0/v1.7-era NEXT files

World/HMD transport, Quest projection, camera origin, HMD rotation and positional 6DoF are already closed. The native fire architecture later advanced through exact `CUserCmd::aimdirection` ownership and frozen server current-command closure.

RC2 three-hook LIVE then failed before native aim installed because its runtime exact guard compared a relocation-sensitive `A1 imm32` operand against preferred-image bytes under ASLR. RC2 is **FAIL / DO NOT PROMOTE** and its visual ancestor is rejected because the user reported disappearing distant world content.

RC3 returns to the user-verified SAFE161 visual/render ancestor and keeps:
- exactly three native-aim hooks;
- RunCommand/TLS absent;
- stock `FX_FireBullets`;
- stock `Weapon_ShootPosition`;
- stock recoil/spread/ammo;
- no PVS mutation.

The only integration repair is the relocation-aware runtime guard plus the already COLD-proven three-hook `CUserCmd` aim path.

## Exact preflight

MISSING FACT: whether the user-verified SAFE161 visual baseline remains intact while the relocation-aware three-hook CUserCmd native-aim path actually activates and owns authoritative shot direction.
WHY NOT COLD: the ASLR guard, currentCmd owner, register map and hook topology are cold-verifiable, but actual hook installation, prediction/network consumption, authoritative impacts and far-world visual stability require execution and the user's eyes.
PASS: SAFE161 far textures/objects and shot audio remain stable; three hooks activate with RunCommand/TLS absent; client publish/client gate/server currentCmd gate occur; impacts follow RIGHT Touch while stock Weapon_ShootPosition, recoil, spread and ammo remain normal.
FAIL: any far-world/texture disappearance, native aim inactive, center-screen authoritative fire, unauthorized pointer grant, currentCmd/ring mismatch, crash/audio regression, or stock recoil/spread/origin/ammo semantics change.
BASELINE MUST HOLD: exact frozen hashes; SAFE161 user-verified textures/audio; M3.2B HMD 6DoF/Quest stereo; exact slot250 RIGHT-hand follow; stock Weapon_ShootPosition/recoil/spread/ammo; no attachment/DrawModel/FX/global-pointer mutation.

## Stop rule

Do not create another weapon LIVE package before RC3 is executed or a concrete new COLD blocker is found. `CSGO-PACING-PVS` is a separate lane and must not be folded into this experiment.
