# CSGOVR — RUNTIME BASELINE

## Target identity
- legacy CS:GO x86
- AppID `4465480`
- BuildID `22100934`
- client.dll SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`

## GOLDEN
`CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`

Canonical result:
`CSGOVR_HLAE_EYE_PATCH_RESULT_20260821_015737.zip`

Generated package SHA256:
`20D382A537CF97AA432CA0BA94DE279EB12764B4724793C217722538BCB78A6D`

This is the protected stereo baseline for current integration work.

## Proven baseline invariants
- pass1 = LEFT real world stream
- pass2 = RIGHT real world stream
- pass3 = monitor
- full `CViewSetup` copy size = `0x198`
- HLAE `CAfxStreams::DoRenderView` seam RVA = `0x00022340`
- `CViewSetup_csgo` copy constructor at HLAE `+0x00014BA0` copies full object
- custom projection enable = `CViewSetup+0x68`
- custom projection matrix = `CViewSetup+0x6C..+0xAB`
- off-center fields: top `+0x138`, bottom `+0x13C`, left `+0x140`, right `+0x144`
- `m_bOffCenter` = flags `+0x148` bit0

## Quest 3 numerical oracle
- LEFT FOV: L=-54°, R=+40°, U=+44°, D=-55°
- RIGHT FOV: L=-40°, R=+54°, U=+44°, D=-55°
- recommended eye size: 1504x1616
- `viewStateFlags=0xF`
- passive eye separation: ~64.780 mm
- asymmetric projection coefficients are preserved in canonical evidence; do not replace with symmetric guesses

## Current authorized mutation surface
Projection integration only:
- feed the known Quest asymmetric projection into the proven LEFT/RIGHT custom/off-center projection path
- preserve the existing v0.5.5 eye scheduling and monitor pass
- no unrelated camera/culling mutation in the same build

## Explicitly not baseline / not proven ownership
The following remain INFRASTRUCTURE/candidates only:
- `client.dll+0x0038D123`
- `CViewSetup.origin.x +0xB8`
- `client.dll+0x052632CC`
- yaw seam `client.dll+0x0038D15B`

Do not build the new integration around an assumption that these are final camera ownership points.

## Falsified culling theory
`client.dll+0x0525BA60` is a real cache region, but reuse inside BOTH LEFT and RIGHT eye marker regions is FALSIFIED. Do not make it the baseline culling architecture.

## If projection integration reveals a culling blocker
Resume from existing evidence only:
- pre-eye reader `client.dll+0x003A16A6`
- second reader `client.dll+0x003A01E4`
- GeneratePerspectiveFrustum `client.dll+0x0086E9C0`
- lane writes starting `+0x0086EB3C`
- CSimpleWorldView lineage

## Source recovery status
The canonical evidence proves the v0.5.5 GOLDEN and its layout and now provides exact package identity. The standalone package/source has not yet been materialized into GitHub source form; older v0.5.3/v0.5.4 package identities are historical only.
