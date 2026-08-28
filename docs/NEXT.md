# CSGOVR — NEXT

## Canonical routing

Current state: `docs/SYNC.md`.

Active machine claims:
- `CSGO-RC3-SAFE161-3HOOK-RUNTIME`
- `CSGO-PACING-PVS`

Do not use older v1.7/barrel-axis or T0 transport notes as the current gate.

## Primary next experiment

`experiments/RC3_SAFE161_3HOOK_GUARDFIX_LIVE_TICKET_20260828.md`

Package:
`CSGOVR_ULTIMATE_SAFE161_NATIVE_AIM_RC3_3HOOK_GUARDFIX_ONECLICK.zip`

SHA256:
`B988DEB8DEE490CFB6964ADA004DF69F454AF48A689BDD60E2A67CCE5B72C2FB`

Why RC3 exists:
- RC2 LIVE failed before native aim installed;
- the failing runtime guard compared a relocation-sensitive `A1 imm32` operand against preferred-image bytes under ASLR;
- that root cause is COLD closed;
- RC2 also regressed the visible far world, so it is not the visual ancestor;
- RC3 returns to the user-verified SAFE161 visual lineage and changes only the three-hook command-owned native aim integration plus relocation-aware guard.

## Exact LIVE preflight

MISSING FACT: whether the user-verified SAFE161 visual baseline remains intact while the relocation-aware three-hook CUserCmd native-aim path actually activates and owns authoritative shot direction.
WHY NOT COLD: the ASLR guard, currentCmd owner, register map and hook topology are cold-verifiable, but actual hook installation, prediction/network consumption, authoritative impacts and far-world visual stability require execution and the user's eyes.
PASS: SAFE161 far textures/objects and shot audio remain stable; three hooks activate with RunCommand/TLS absent; client publish/client gate/server currentCmd gate occur; impacts follow RIGHT Touch while stock Weapon_ShootPosition, recoil, spread and ammo remain normal.
FAIL: any far-world/texture disappearance, native aim inactive, center-screen authoritative fire, unauthorized pointer grant, currentCmd/ring mismatch, crash/audio regression, or stock recoil/spread/origin/ammo semantics change.
BASELINE MUST HOLD: exact frozen hashes; SAFE161 user-verified textures/audio; M3.2B HMD 6DoF/Quest stereo; exact slot250 RIGHT-hand follow; stock Weapon_ShootPosition/recoil/spread/ammo; no attachment/DrawModel/FX/global-pointer mutation.

## Separate lane

`CSGO-PACING-PVS` remains independent. Do not add PVS/culling mutation to RC3. If RC3 on SAFE161 still loses distant content, return to the T14/T15 quality lane as a separate causal experiment.

## Stop

Do not create RC4 before RC3 produces a RESULT or a newly discovered specific COLD blocker. Machine plumbing alone does not promote RC3; the user-visible far-world/audio and impact-direction verdicts are mandatory.
