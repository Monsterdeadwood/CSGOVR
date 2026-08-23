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

## Stereo GOLDEN / HLAE path
- GOLDEN: `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`.
- Two real world streams are PROVEN: pass1 LEFT, pass2 RIGHT, pass3 monitor.
- Exact full `CViewSetup` copy size is `0x198`; the former `0x14C` full-size interpretation is superseded.
- Exact HLAE `CAfxStreams::DoRenderView` seam is RVA `0x00022340` in the preserved HLAE lineage.
- `CViewSetup_csgo` copy constructor at HLAE `+0x00014BA0` copies through byte `0x197`.

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

## Culling / pre-eye dataflow
- `client.dll+0x003A16A6..+0x003A16DC` is a concrete pre-eye frustum/cache reader/dataflow site.
- Recovered result `CSGOVR_TTD_REPLAY_FRUSTUM_READER_OWNER_3A16A6_RECOVERED_RESULT_20260823_160154.zip` hit the target, captured a call stack, and recovered bounded dataflow with store candidates.
- The first `0x40` bytes of helper arg2 undergo a temporary in-place transpose and are restored. That operation is PROVEN; an older auto-generated FALSIFIED parser verdict is invalid/superseded.
- CSimpleWorldView lineage is PROVEN, while final visibility ownership remains OPEN.

## Important non-PROVEN retained seams
The following are intentionally NOT promoted to causal camera ownership:
- `client.dll+0x0038D123` origin.x writer/mutation seam
- `CViewSetup.origin.x +0xB8`
- `client.dll+0x052632CC` persistent/global CView candidate
- `client.dll+0x0038D15B` yaw mutation seam
Their 2026-08-23 wrappers classify the mutation windows as INFRASTRUCTURE/byte-integrity evidence, not camera-control proof.
