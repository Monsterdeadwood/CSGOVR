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
**ORIGINAL PACKAGE RECOVERED.**

Library artifact:
`CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX.zip`

Materialized size: `17,726` bytes.
Recomputed SHA256: `20D382A537CF97AA432CA0BA94DE279EB12764B4724793C217722538BCB78A6D`.

Runtime-critical original source is now imported under:
`runtime/golden_v0.5.5/src/`

Byte-identical post-import verification:
- `eye_patch.c` original SHA256 `3844B2C323F1077A901581A0A82908A174F9D1DF3F4213C5EB68980B7E7D148F`, Git blob `54a78d9ad16511b5e4d1d643db85ee9d2ad15179`
- `injector.c` original SHA256 `E6133CEFC23460BA3AD82350803251C604AA24425474B224BEF7FEE01EA5183D`, Git blob `f46bd680f8dfbf48fb96585c51954ae79a93f99e`

The original package member manifest and build reproduction contract are recorded beside the source. v0.5.3/v0.5.4 remain historical only.
