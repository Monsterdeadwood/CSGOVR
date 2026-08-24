# CSGOVR T0 D3D9Ex -> D3D11 shared-format audit 2026-08-24

Evidence label: **STATIC-AUDITED / NOT LIVE-PROVEN**.

## Scope

Close the cross-API resource contract behind T0 FINALIST A before building or running a GPU transport child.

This audit does **not** change projection, origin, camera, culling, viewmodel, HUD or the protected v0.5.5 GOLDEN.

## Microsoft cross-API contract

For a Direct3D 9 resource opened by Direct3D 11 through `ID3D11Device::OpenSharedResource`, Microsoft documents a restricted compatibility set.

The D3D9 texture must be a 2D resource created through the D3D9 shared-handle mechanism (`CreateTexture(..., &pSharedHandle)`). The documented D3D9 -> D3D11 shared-texture restrictions include:

- one mip level;
- `D3DPOOL_DEFAULT` / default usage;
- no multisampling;
- render-target / shader-resource compatible usage;
- matching dimensions and compatible creation/open path;
- explicit cross-device synchronization is required;
- documented cross-API DXGI formats are `R10G10B10A2_UNORM`, `R16G16B16A16_FLOAT`, and `R8G8B8A8_UNORM`.

The relevant D3D9 -> DXGI mappings are:

- `D3DFMT_A8B8G8R8` -> `DXGI_FORMAT_R8G8B8A8_UNORM`;
- `D3DFMT_A2B10G10R10` -> `DXGI_FORMAT_R10G10B10A2_UNORM`;
- `D3DFMT_A16B16G16R16F` -> `DXGI_FORMAT_R16G16B16A16_FLOAT`;
- `D3DFMT_A8R8G8B8` -> `DXGI_FORMAT_B8G8R8A8_UNORM`;
- `D3DFMT_X8R8G8B8` -> `DXGI_FORMAT_B8G8R8X8_UNORM`.

Therefore the common D3D9 ARGB/XRGB formats are **not automatically equivalent** to the documented D3D9 -> D3D11 `R8G8B8A8_UNORM` share path.

References:
- Microsoft `ID3D11Device::OpenSharedResource`
- Microsoft `IDirect3DDevice9::CreateTexture`
- Microsoft Direct3D 9 resource sharing / feature summary
- Microsoft D3D9 <-> DXGI format mapping
- Microsoft `IDirect3DDevice9::StretchRect`
- Microsoft `IDirect3D9::CheckDeviceFormatConversion`

## Exact HLAE consequence

Pinned AdvancedFX `b97636852b8eecae09285b5a386192bb285638eb` old `PrepareDrawReply_Continue` path does **not** choose an arbitrary export format.

It:

1. gets HLAE's authoritative tracked render target;
2. calls `GetDesc(&desc)`;
3. opens the supplied shared texture handle with:

```cpp
device->CreateTexture(
    desc.Width,
    desc.Height,
    1,
    D3DUSAGE_RENDERTARGET,
    desc.Format,
    D3DPOOL_DEFAULT,
    &replacementTexture,
    &sharedColorTextureHandle);
```

So A0 direct replacement sharing inherits the exact current HLAE tracked-RT `desc.Format`.

The external producer/host cannot silently substitute a different color format and still claim it is exercising the exact old replacement-target contract.

## Current CSGOVR evidence on the actual eye RT format

Retained CSGOVR D3D9 transport/capture source accepts both:

- `D3DFMT_X8R8G8B8`
- `D3DFMT_A8R8G8B8`

That proves the fallback was designed for both common 32-bit Source formats. It does **not** prove which one the authoritative v0.5.5 eye render actually uses in the accepted frozen run.

No accepted CSGOVR RESULT currently promotes one exact tracked-eye `desc.Format` as canonical.

Therefore:

**actual HLAE tracked eye RT format remains OPEN.**

Do not infer it from fallback source guards.

## Revised T0 GPU finalists

### A0 - direct old-compatible replacement-target protocol

KEEP conditionally.

A0 is eligible only if all are true:

1. an actually compatible old replacement-target protocol/host mode is pinned and logged;
2. the authoritative tracked eye RT `desc.Format` is directly compatible with the documented D3D9 -> D3D11 shared-resource endpoint;
3. shared texture creation/open uses the same width/height/format and no MSAA;
4. D3D11 `OpenSharedResource` succeeds on the OpenXR-required adapter;
5. GPU completion is explicit before pair publication.

If the exact tracked format is outside the documented cross-API set and runtime open confirms the incompatibility, A0 becomes a concrete format blocker rather than a vague protocol failure.

### A1a - pass-discriminator direct replacement transplant

Same direct format requirement as A0, but bypasses protocol-generation ambiguity by transplanting only the HLAE-state-aware replacement-target mechanics around proven v0.5.5 pass1/pass2.

This remains attractive only when the authoritative eye format is directly shareable to D3D11.

### A1b - HLAE-aware GPU format bridge

New strongest fallback **inside the GPU lane** if direct format sharing is blocked.

Pipeline:

```text
v0.5.5 authoritative eye render
        |
        | HLAE tracked/source surface known at the eye boundary
        v
same-format HLAE-compatible eye target
        |
        | GPU-only StretchRect / resolve + format conversion
        v
non-MSAA D3D9 shared texture
D3DFMT_A8B8G8R8
        |
        | D3D9 EVENT query completion
        v
shared HANDLE
        |
        v
x64 D3D11 OpenSharedResource
DXGI_FORMAT_R8G8B8A8_UNORM
        |
        v
OpenXR swapchain copy / xrEndFrame
```

A1b does not revive the old naive post-return copy.

The copy must occur while the authoritative HLAE eye source surface is known and tied to the proven pass discriminator. A raw `GetRenderTarget(0)` after the hook returns remains non-authoritative.

### A2 - naive post-hook raw RT copy

**STOP remains in force.**

Do not conflate A1b with A2. A1b is HLAE-state-aware and pass-scoped; A2 guessed that RT0 after the external hook return was still the completed eye.

### B - CPU fallback

Unchanged: `CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF` remains PREPARED / STATIC-AUDITED / NOT LIVE-PROVEN and is activated only after a concrete GPU blocker or bounded compatibility control.

## A1b capability gates

Before A1b can be promoted, require explicit evidence for:

1. exact authoritative source `desc.Format`;
2. source dimensions and MSAA state;
3. `D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES` where relevant;
4. `CheckDeviceFormatConversion(actualSourceFormat, D3DFMT_A8B8G8R8) == D3D_OK`;
5. creation of non-MSAA `D3DFMT_A8B8G8R8` shared render-target texture in `D3DPOOL_DEFAULT`;
6. successful GPU `StretchRect` / resolve-convert into that carrier;
7. D3D9 EVENT-query completion before READY;
8. x64 D3D11 `OpenSharedResource` succeeds and reports the expected `R8G8B8A8_UNORM` resource on the same adapter;
9. Reset/device-generation invalidates and recreates both eye carriers safely;
10. same-cycle LEFT/RIGHT pair identity remains atomic and `mixedPairCount=0`.

Format conversion support is device/driver capability, so it must be queried rather than assumed.

## What is and is not falsified

### PROVEN / source-closed

- old HLAE replacement path inherits `desc.Format` from its current tracked RT;
- Microsoft cross-API sharing has a restricted documented resource/format contract;
- common D3D9 ARGB/XRGB mappings differ from `D3DFMT_A8B8G8R8 -> R8G8B8A8_UNORM`;
- GPU format conversion through `StretchRect` is capability-dependent and can be checked with `CheckDeviceFormatConversion`.

### OPEN

- exact authoritative v0.5.5 tracked eye RT format;
- whether that exact format opens directly in D3D11 on the frozen NVIDIA path;
- whether the device supports conversion from that source format to `D3DFMT_A8B8G8R8`;
- whether A0/A1a or A1b is the final GPU transport implementation.

### NOT falsified yet

- A0 direct sharing;
- A1a direct transplant;
- GPU transport as a class.

Do not promote a documentation constraint into a runtime FALSIFIED verdict until the exact frozen format/path is identified and the relevant create/open attempt or capability query closes it.

## Bounded compatibility control - future LIVE only after current COLD closure

MISSING FACT: exact frozen tracked eye RT format/MSAA state and whether it can be shared directly to D3D11 or GPU-converted into the documented `D3DFMT_A8B8G8R8 -> R8G8B8A8_UNORM` carrier.
WHY NOT COLD: source and API documentation define the legal candidates, but the exact frozen runtime `desc.Format`, device caps and driver-specific CreateTexture/OpenSharedResource/format-conversion outcomes require the real D3D9Ex device.
PASS: authoritative eye RT identity is logged; direct-share eligibility is deterministically classified; if conversion is needed, `CheckDeviceFormatConversion` and a bounded non-MSAA shared-carrier create/open test succeed with no image/projection/camera mutation.
FAIL: exact eye RT cannot be authoritatively identified, neither direct format nor conversion carrier is supported, cross-adapter/resource open fails, or the compatibility probe perturbs the protected pass schedule/GOLDEN.
BASELINE MUST HOLD: exact client/HLAE hashes, `-insecure`, v0.5.5 full `0x198` eye transaction and pass1 LEFT/pass2 RIGHT/pass3 monitor, BASE projection/origin/camera, no OpenXR submission required for this compatibility control, reversible cleanup.

## KEEP / ADAPT / STOP

- KEEP A0 only as a format- and protocol-conditional direct path.
- ADAPT A1 into A1a direct replacement and A1b GPU format bridge.
- KEEP HLAE tracked-RT ownership as the authoritative source boundary.
- STOP guessing the eye format from fallback source guards.
- STOP naive post-hook A2.
- KEEP CPU v0.7.1 as fallback B.

No runtime RESULT is claimed by this document.
