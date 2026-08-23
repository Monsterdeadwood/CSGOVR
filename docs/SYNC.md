# CSGOVR — STATE SYNC

Snapshot imported from canonical ledger: 2026-08-23

## AUTHORITY
Canonical source: `CSGOVR_KNOWLEDGE_BASE_CANONICAL_20260823.md`.
If an older chat/state conflicts with that ledger, the canonical ledger wins.

## LATEST RESULT / CURRENT TRUTH
- GOLDEN: `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`.
- Stereo: two real world streams are PROVEN; pass1 LEFT, pass2 RIGHT, pass3 monitor; full `CViewSetup` copy size is `0x198`.
- Projection: custom projection enable at `+0x68` and matrix `+0x6C..+0xAB` are PROVEN; M00/M02/M11/M12 semantics were causally measured.
- Quest 3 FOV/derived coefficients are PROVEN; recommended eye target 1504x1616; `viewStateFlags=0xF`; passive eye separation about 64.780 mm.
- Camera origin is real and reaches GPU c2, but the tested late `DoRenderView` eye-origin nudge did not survive to final captured raster.
- Culling: `client.dll+0x0525BA60` shared-inside-both-eyes cache hypothesis is FALSIFIED; `client.dll+0x003A16A6` is a concrete pre-eye reader/dataflow site with a complete recovered evidence packet; exact semantic destination remains unclassified.

## PROVEN
- Full `CViewSetup` size: `0x198`; old `0x14C` full-size claim is SUPERSEDED.
- HLAE two-world-stream stereo baseline is real and known-good.
- Custom projection path: bool `+0x68`, matrix `+0x6C..+0xAB`.
- Quest 3 asymmetric FOV and projection coefficients.
- Off-center projection fields from the controlled HLAE path: top `+0x138`, bottom `+0x13C`, left `+0x140`, right `+0x144`, `m_bOffCenter` at flags `+0x148` bit0.
- `client.dll+0x003A16A6..+0x003A16DC` is a concrete pre-eye frustum/cache reader; call stack and bounded dataflow were recovered.
- First `0x40` bytes of helper arg2 undergo a temporary in-place transpose and are restored.
- CSimpleWorldView lineage is proven; final visibility ownership is still OPEN.

## SUPPORTED / RETAINED CANDIDATES
- `client.dll+0x0038D123`: origin.x writer/mutation seam candidate.
- `CViewSetup.origin.x @ +0xB8`.
- next true `RenderView`: `client.dll+0x003A5280`.
- persistent/global CView candidate: `client.dll+0x052632CC`.
- origin-global candidate: `client.dll+0x05263384`.
- yaw mutation seam `client.dll+0x0038D15B`; global commit `+0x0038D1DE`; `g_vecRenderAngles.y` candidate `+0x05245AB8`.
These origin/yaw control runs are **INFRASTRUCTURE**, not causal camera ownership proof.

## FALSIFIED / CLOSED
- `client.dll+0x0525BA60` as a cache reused inside BOTH LEFT and RIGHT eye marker regions.
- Persistent helper arg2 output outside the first `0x40` bytes (`+0x40..+0x1FF`).
- Treating originX/yaw byte-integrity mutation runs as camera/control proof.
- Broad OpenXR rediscovery: XR input is already proven.
- Sequential screenshot comparisons as quantitative culling/disparity proof; temporal drift requires same-frame capture.

## CURRENT FINALISTS
1. Integration: known Quest asymmetric projection -> known-good HLAE eye path.
2. If a visible culling blocker remains after projection integration: pre-eye reader `client.dll+0x003A16A6` + CSimpleWorldView lineage.

## CONCRETE TARGETS
- GOLDEN eye path: `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`.
- HLAE `CAfxStreams::DoRenderView` RVA `0x00022340`.
- HLAE `CViewSetup_csgo` copy constructor `+0x00014BA0`, exact object size `0x198`.
- Custom projection: `+0x68` enable, `+0x6C..+0xAB` matrix.
- Off-center projection: `+0x138/+0x13C/+0x140/+0x144`, flag `+0x148` bit0.
- Pre-eye frustum/cache reader: `client.dll+0x003A16A6`.
- GeneratePerspectiveFrustum family: `client.dll+0x0086E9C0`, lane writes starting `+0x0086EB3C`.

## CURRENT GATE
Integrate the already-PROVEN Quest asymmetric projection into the known-good eye path and test it in-headset. Reverse further only against a concrete visible blocker that survives this integration.

## NEXT
1. Preserve v0.5.5 GOLDEN exactly.
2. Feed known Quest 3 asymmetric coefficients through the proven custom/off-center projection path for LEFT/RIGHT.
3. Validate same-frame stereo in headset, not by temporally separated screenshot comparison.
4. If geometry/culling still fails, continue from `+0x003A16A6` / CSimpleWorldView, not from the falsified shared-cache theory.
5. Keep origin/yaw seams as infrastructure candidates only until a new causal experiment proves authority.

## METHOD ALREADY USED
- OpenXR FOV/view acquisition and independent projection derivation.
- HLAE source/static seam archaeology.
- v0.5.5 HLAE multipass stereo GOLDEN.
- Custom projection and off-center canary causal tests.
- TTD frustum/cache replay and recovered pre-eye reader dataflow.
- Origin/yaw infrastructure mutation windows with byte-integrity checks.

## HARD CONSTRAINTS
- Evidence labels must follow the canonical ledger: PROVEN / SUPPORTED / PENDING / INFRASTRUCTURE / FALSIFIED / INVALID / SUPERSEDED.
- Do not upgrade infrastructure to ownership proof.
- Do not retest `+0x0525BA60` both-eye reuse without contradictory evidence.
- Preserve known-good stereo GOLDEN while integrating projection.
