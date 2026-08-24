# T0 OpenXR D3D11 copy contract — 2026-08-24

Evidence class: **OFFICIAL OPENXR CONTRACT + T0 INTEGRATION CONSEQUENCES**.

This document narrows the x64 consumer side after D3D9 shared-resource import. It does not claim a headset/runtime PASS.

## Official Khronos facts

### D3D11 device identity is runtime-mandated

With `XR_KHR_D3D11_enable`, `xrGetD3D11GraphicsRequirementsKHR` returns:

- `adapterLuid` — the Windows graphics adapter that must be used;
- `minFeatureLevel` — the minimum D3D11 feature level.

The D3D11 device passed in `XrGraphicsBindingD3D11KHR` to `xrCreateSession` must be created using those requirements.

For the current accepted Quest/Meta diagnostic baseline this project has already observed:

- adapter LUID `00000000:000118D1`;
- minimum feature level `0xB000`;
- created feature level `0xB100`.

T0 must log the live values again and reject a mismatched adapter before treating any D3D9->D3D11 import result as relevant to OpenXR.

### D3D11 swapchain images are ID3D11Texture2D resources

`XrSwapchainImageD3D11KHR` returns an `ID3D11Texture2D*` for each OpenXR swapchain image.

Therefore the final T0 consumer operation can remain a native D3D11 resource/render operation after the legacy D3D9 resource has been opened/imported into D3D11.

### Swapchain usage must declare intended usage

For D3D11, OpenXR maps:

- `XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT` -> `D3D11_BIND_RENDER_TARGET`;
- `XR_SWAPCHAIN_USAGE_SAMPLED_BIT` -> `D3D11_BIND_SHADER_RESOURCE`;
- `TRANSFER_SRC_BIT` / `TRANSFER_DST_BIT` -> no additional D3D11 bind flag;
- D3D11 swapchain textures -> `D3D11_USAGE_DEFAULT`.

Consequences:

- request `XR_SWAPCHAIN_USAGE_TRANSFER_DST_BIT` for a copy-oriented path because it declares intent;
- do not treat it as a special D3D11 bind guarantee;
- if `A1b-D11` shader-blit/scaler remains available, also request `XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT` so the acquired XR image is guaranteed render-target capable;
- inspect/log returned texture descriptions and prove the actual operation succeeds.

### OpenXR color interpretation is format-dependent

OpenXR composition operates in linear color space. Images intended to contain non-linear sRGB-encoded values must use an API-specific sRGB format such as `DXGI_FORMAT_R8G8B8A8_UNORM_SRGB`; other formats are treated as linear values. OpenXR also recommends avoiding linear-encoded 8-bit plain UNORM where possible because of banding.

The runtime enumerates concrete DXGI formats in preference order. Direct3D OpenXR swapchain enumeration never returns typeless formats.

The accepted CSGOVR diagnostic preserves Quest FOV, recommended eye dimensions and D3D11 LUID, but the canonical corpus does **not** preserve an authoritative `xrEnumerateSwapchainFormats` result. Therefore T0 must harvest the current Meta format list once in the actual host rather than invent it from another project or memory.

## T0 x64 host contract

### Device gate

Before importing any shared D3D9 eye resource:

1. call `xrGetD3D11GraphicsRequirementsKHR`;
2. create/select the D3D11 device on the exact returned LUID;
3. meet or exceed `minFeatureLevel`;
4. create the OpenXR session with that same device;
5. log runtime name/version, LUID and actual feature level.

A D3D11 import test on another adapter is not a T0 PASS.

### Swapchain format gate

The host must call `xrEnumerateSwapchainFormats` and select a runtime-supported format rather than hard-code one blindly.

Log at minimum:

- full format list in runtime preference order;
- selected concrete DXGI format;
- eye width/height/sampleCount;
- requested usage flags;
- returned `ID3D11Texture2D` desc;
- intended color interpretation: linear or sRGB encoded.

If the imported shared carrier format differs from the XR swapchain format, perform a documented D3D11 conversion/render/copy path instead of assuming `CopyResource` is legal across formats.

Do not add gamma conversion merely because source and destination format names differ; source encoding must be evidenced.

### Size / scaling gate

Microsoft's D3D11 `CopyResource` contract requires source and destination resources to have identical dimensions and compatible formats. It performs copy only and explicitly does **not** stretch. `CopySubresourceRegion` can place/crop a source region, but it also does not perform stretch/scaling.

The accepted Quest baseline recommends `1504x1616` per eye. The exact HLAE replacement-eye RT dimensions have not yet been promoted as a frozen runtime fact.

Therefore T0 must not assume:

`OpenSharedResource -> CopyResource -> XR swapchain`

unless imported carrier and acquired XR texture are actually dimension-compatible.

If dimensions differ, retain two GPU-only choices:

1. **A1b-D9, D3D9-side sized carrier:** checked `StretchRect`/resolve from authoritative eye staging into a non-MSAA shared carrier sized for the intended destination. D3D9 `StretchRect` supports RT-to-RT scaling and MSAA downsample; conversion support must be tested on the actual device.
2. **A1b-D11, D3D11-side shader blit/scaler:** import a legal carrier at source size, sample it on the x64 D3D11 device, and render a fullscreen pass into the acquired XR texture at the runtime-selected eye size/format. This path requires `COLOR_ATTACHMENT_BIT` at swapchain creation.

Do not use `CopySubresourceRegion` as a fake scaler.

Selection should be based on real format/MSAA/dimension/capability evidence, not assumed performance.

### Acquire / wait / write / release discipline

For each coherent LEFT/RIGHT pair:

- `xrAcquireSwapchainImage` for each eye;
- `xrWaitSwapchainImage` before writing;
- perform D3D11 copy/render into acquired image;
- place the CSGOVR D3D11 consumer-completion fence for the shared source;
- `xrReleaseSwapchainImage` according to OpenXR ownership rules;
- submit one coherent projection layer through one `xrEndFrame` for the pair.

The D3D11 shared-source fence and OpenXR swapchain ownership are independent lifecycles.

### Pair identity remains independent of OpenXR image index

OpenXR swapchain image index is not the game's stereo pair identity.

The host must carry `pairSeq` from producer metadata and may only submit LEFT and RIGHT from the same game/HLAE cycle. Acquiring two XR images does not itself prove coherent stereo.

### Copy/render operation must be proven, not inferred from usage flags

A future T0 result should log:

- imported D3D11 source texture desc;
- XR destination texture desc;
- copy/conversion/scaling operation chosen;
- source and destination dimensions/formats;
- SRV/RTV formats when shader blit is used;
- HRESULT / debug-layer errors if available;
- consumer GPU completion result;
- per-eye acquire/wait/write/release status.

Usage flags declare intended access but do not replace this evidence.

## T0 evidence additions

Add to eventual T0 RESULT:

- `xrGetD3D11GraphicsRequirementsKHR` result;
- runtime-required LUID / min FL;
- actual D3D11 device LUID / FL;
- `xrEnumerateSwapchainFormats` full relevant list;
- selected XR format / color interpretation / usage flags;
- authoritative D3D9 eye/replacement RT width/height/format/MSAA;
- D3D11 source texture desc for each transport slot;
- XR destination texture desc;
- selected path: direct copy / A1b-D9 sized carrier / A1b-D11 shader blit;
- copy/conversion/scaling result per eye;
- consumer GPU completion state;
- acquire/wait/release counts/errors;
- `xrEndFrame` count;
- pairSeq attached to each submitted eye;
- `mixedPairCount = 0`.

No projection, origin, head-pose, culling, viewmodel or HUD change belongs in this gate.