# CSGOVR — PROVEN

## Evidence authority
Canonical source: `CSGOVR_KNOWLEDGE_BASE_CANONICAL_20260823.md`.
Do not silently promote SUPPORTED/INFRASTRUCTURE candidates into this file.

## OpenXR / Quest 3 input
- Meta/Oculus OpenXR 1.205.0 + Quest 3 diagnostic path succeeded.
- `xrLocateViews` returned two valid views with `viewStateFlags = 0xF` in the recorded idle test.
- LEFT FOV: L=-54°, R=+40°, U=+44°, D=-55°.
- RIGHT FOV: L=-40°, R=+54°, U=+44°, D=-55°.
- Recommended eye size: 1504x1616.
- Passive eye separation observed: about 64.780 mm.
- Independent asymmetric projection reference includes approximately M00=0.9027383311, M11=0.8354788620, M12=-0.1931874395 and mirrored horizontal M02 asymmetry.

Derived validation coefficients from the recorded FOV angles:
- LEFT M00≈0.902738278, M02≈-0.242512644, M11≈0.835478849, M12≈-0.193187454
- RIGHT M00≈0.902738278, M02≈+0.242512644, M11≈0.835478849, M12≈-0.193187454

## Stereo GOLDEN / HLAE path
- GOLDEN: `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`.
- Two real world streams are PROVEN: pass1 LEFT, pass2 RIGHT, pass3 monitor.
- Exact full `CViewSetup` copy size is `0x198`; the former `0x14C` full-size interpretation is superseded.
- Exact HLAE `CAfxStreams::DoRenderView` seam is RVA `0x00022340` in the preserved HLAE lineage.
- `CViewSetup_csgo` copy constructor at HLAE `+0x00014BA0` copies through byte `0x197`.

## Original v0.5.5 GOLDEN source recovered
Physical Library package:
`CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX.zip`

- bytes `17,726`
- SHA256 `20D382A537CF97AA432CA0BA94DE279EB12764B4724793C217722538BCB78A6D`

Exact runtime source imported to GitHub and post-write Git blob identity verified:
- `runtime/golden_v0.5.5/src/eye_patch.c`
  - source SHA256 `3844B2C323F1077A901581A0A82908A174F9D1DF3F4213C5EB68980B7E7D148F`
  - Git blob `54a78d9ad16511b5e4d1d643db85ee9d2ad15179`
- `runtime/golden_v0.5.5/src/injector.c`
  - source SHA256 `E6133CEFC23460BA3AD82350803251C604AA24425474B224BEF7FEE01EA5183D`
  - Git blob `f46bd680f8dfbf48fb96585c51954ae79a93f99e`

Canonical successful runtime binary identities retained by RESULT evidence:
- patch DLL SHA256 `11F800361DB2416F58A2014D0FE57861C572B30FB72521F2CD41020A477CB578`
- injector SHA256 `C987F156BDEF706A98282FC8BA0FF943CF019E8E6A7D7AAADD9BFC06B68811F0`

This source is the parent for projection integration.

## Projection path
- Custom projection enable bool at `CViewSetup+0x68` is PROVEN.
- Custom projection matrix at `CViewSetup+0x6C..+0xAB` is PROVEN.
- M00/M02/M11/M12 projection semantics were causally measured in controlled projection experiments.
- Off-center fields are structurally/causally mapped in the HLAE eye path:
  - top `+0x138`
  - bottom `+0x13C`
  - left `+0x140`
  - right `+0x144`
  - `m_bOffCenter` at flags `+0x148` bit0.
- Late HLAE `DoRenderView` projection/off-center changes can survive to the rendered output in the tested lineage.

## v0.6.1 projection oracle source — verified child, not GOLDEN
Physical package:
`CSGOVR_CUSTOM_PROJECTION_MATRIX_ORACLE_v0.6.1_ONE_CLICK.zip`

- bytes `21,733`
- SHA256 `DB155F87F4BFF15611B1B5A6EFBAFAE756D789165BF6D1C52B632BE68E95CEC3`
- `src/injector.c` is byte-identical to v0.5.5: `E6133CEFC23460BA3AD82350803251C604AA24425474B224BEF7FEE01EA5183D`
- v0.6.1 `src/eye_patch.c` SHA256 `2CD99D89034DCD16A26A048376F193701A43CCF3C6A805090735545938CE2201`

Useful donor implementation:
- custom projection bool `+0x68`
- matrix `+0x6C`
- verified matrix slot convention for M00/M02/M11/M12/M22/M23/M32/M33
- mutation is applied to the local `0x198` eye copy before the proven HLAE call

Test-only v0.6.1 behavior is NOT the next architecture:
- frozen center camera
- INI phase controller
- synthetic ±0.10 projection shifts
- synthetic 0.80 wide scales
- forced 140-degree legacy FOV

Current surgical map:
`runtime/children/v0.6.1_projection_oracle/PROJECTION_TRANSPLANT.md`

## Culling / pre-eye dataflow
- `client.dll+0x003A16A6..+0x003A16DC` is a concrete pre-eye frustum/cache reader/dataflow site.
- Recovered result `CSGOVR_TTD_REPLAY_FRUSTUM_READER_OWNER_3A16A6_RECOVERED_RESULT_20260823_160154.zip` hit the target, captured a call stack, and recovered bounded dataflow with store candidates.
- The first `0x40` bytes of helper arg2 undergo a temporary in-place transpose and are restored. That operation is PROVEN; an older auto-generated FALSIFIED parser verdict is invalid/superseded.
- CSimpleWorldView lineage is PROVEN, while final visibility ownership remains OPEN.

## Current integration equation

```text
exact v0.5.5 GOLDEN eye path
+ Quest asymmetric custom matrix only
= next projection child
```

Do not reopen RenderView ownership or add a culling mutation until this projection child receives an in-headset verdict.

## Important non-PROVEN retained seams
The following are intentionally NOT promoted to causal camera ownership:
- `client.dll+0x0038D123` origin.x writer/mutation seam
- `CViewSetup.origin.x +0xB8`
- `client.dll+0x052632CC` persistent/global CView candidate
- `client.dll+0x0038D15B` yaw mutation seam
Their 2026-08-23 wrappers classify the mutation windows as INFRASTRUCTURE/byte-integrity evidence, not camera-control proof.
