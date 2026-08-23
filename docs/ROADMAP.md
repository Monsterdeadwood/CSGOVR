# CSGOVR — ROADMAP

The project starts from an existing stereo GOLDEN and proven projection semantics. Do not restart camera/OpenXR discovery from zero.

## 1. Quest asymmetric projection integration — CURRENT
Baseline:
`CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`.

Already known:
- LEFT/RIGHT real world streams
- full `CViewSetup` size `0x198`
- custom projection bool `+0x68`
- custom projection matrix `+0x6C..+0xAB`
- off-center fields `+0x138/+0x13C/+0x140/+0x144`, flag `+0x148 bit0`
- Quest 3 asymmetric FOV/coefficients

Integration gate:
1. feed exact LEFT/RIGHT Quest projection through the known-good eye path;
2. keep monitor pass intact;
3. validate in headset and same-frame;
4. record explicit regression state;
5. freeze if green.

## 2. Concrete culling blocker, only if still visible
Do not reverse culling preemptively after projection integration.

If geometry still disappears incorrectly, continue from existing evidence:
- `client.dll+0x003A16A6` concrete pre-eye reader
- `client.dll+0x003A01E4` second reader
- `client.dll+0x0086E9C0` GeneratePerspectiveFrustum
- lane writes starting `client.dll+0x0086EB3C`
- CSimpleWorldView lineage

Do not reopen the FALSIFIED `client.dll+0x0525BA60` both-eye cache-reuse hypothesis.

## 3. Authoritative eye translation / camera, only if visible blocker remains
The tested late HLAE `DoRenderView` origin nudge did not move final captured raster.

Retained seams `+0x0038D123`, `origin.x +0xB8`, `+0x052632CC` are INFRASTRUCTURE/candidates, not camera ownership proof. Reopen only if actual headset behavior requires it.

## 4. Weapon / controller separation
After projection/culling/camera are regression-green:
- preserve world camera
- independently drive viewmodel/controller pose
- prove visible root ownership before ballistics

## 5. Regression / packaging
- same-frame L/R
- monitor pass intact
- Quest asymmetric projection matches numerical oracle
- no eye-specific geometry loss
- HUD/overlays/post stable
- menu/gameplay transitions stable
- exact frozen binary checks
- one-click restore

## Measurement rule
Sequential screenshots are not acceptable for quantitative disparity/culling decisions because temporal drift is material. Prefer same-frame capture or headset acceptance against deterministic controls.
