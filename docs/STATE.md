# CSGOVR State

## Status
ACTIVE.

## Authority
Canonical source: `CSGOVR_KNOWLEDGE_BASE_CANONICAL_20260823.md`.

## Current gate
Integrate the already-proven Quest 3 asymmetric projection into the known-good HLAE eye path and test it in-headset. Continue reverse-engineering only against a concrete visible blocker that remains after this integration.

## Current known state
- GOLDEN: `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`.
- Two real world streams are PROVEN: pass1 LEFT, pass2 RIGHT, pass3 monitor.
- Full `CViewSetup` size is `0x198`; old `0x14C` full-size claim is superseded.
- Custom projection path is PROVEN: enable bool `+0x68`, matrix `+0x6C..+0xAB`; M00/M02/M11/M12 semantics were causally measured.
- Off-center path is mapped: top `+0x138`, bottom `+0x13C`, left `+0x140`, right `+0x144`, `m_bOffCenter` at flags `+0x148` bit0.
- Quest 3 asymmetric FOV/coefficients are PROVEN; eye target 1504x1616, `viewStateFlags=0xF`, passive eye separation about 64.780 mm.
- `client.dll+0x003A16A6` is a concrete pre-eye frustum/cache reader with recovered call stack and bounded dataflow; exact semantic destination is still OPEN.
- `client.dll+0x0525BA60` is a real cache region, but reuse inside BOTH eye marker regions is FALSIFIED.
- `client.dll+0x0038D123`, `origin.x +0xB8`, and `client.dll+0x052632CC` are retained infrastructure/candidate anchors, not causal ownership proof.

## Operating protocol
- Preserve the v0.5.5 GOLDEN.
- Do not upgrade INFRASTRUCTURE controls to camera ownership proof.
- Same-frame/in-headset validation outranks sequential screenshot comparison because temporal drift is material.
- If culling remains broken after projection integration, continue from `+0x003A16A6` / CSimpleWorldView lineage.
- PARALLELIZE EVIDENCE, SERIALIZE MUTATION.

## Next
1. Feed the known Quest asymmetric projection through the proven LEFT/RIGHT HLAE projection path.
2. Test stereo in headset while preserving v0.5.5 GOLDEN behavior.
3. Record a same-frame result and explicit regression status.
4. If a visible culling blocker survives, map the semantic destination of `+0x003A16A6` and continue through CSimpleWorldView.
5. Keep origin/yaw seams as infrastructure candidates until a new causal experiment proves authority.

See `docs/SYNC.md` for the detailed canonical snapshot.
