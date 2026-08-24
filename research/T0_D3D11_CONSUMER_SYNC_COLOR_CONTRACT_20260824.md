# T0 D3D11 consumer synchronization and color contract — 2026-08-24

Evidence class: **OFFICIAL D3D11 + OPENXR CONTRACT / T0 DESIGN CONSEQUENCE**.

This document narrows the x64 consumer. It does not claim a runtime/headset PASS.

## 1. Consumer GPU completion is a real ownership gate

Microsoft defines `D3D11_QUERY_EVENT` as a query that determines whether the GPU has finished processing commands. For this query type `Begin` is disabled: the application places the marker with `ID3D11DeviceContext::End(query)` and polls `GetData` until the data is available. `GetData` returns `S_FALSE` while the result is not yet available and `S_OK` when it is available.

Therefore the CSGOVR transport slot must not return from `CONSUMER_READING` to `FREE` merely because `CopyResource`, shader-blit draw submission, or `Flush` returned on the CPU.

Preferred first implementation:

1. x64 host acquires a READY transport slot;
2. queues D3D11 copy / shader-blit that consumes the imported shared texture;
3. calls `End(D3D11_QUERY_EVENT)` after those commands;
4. polls the immediate context `GetData` for that query;
5. only after `S_OK` with completed event does the host acknowledge consumer completion and return the slot to `FREE`.

The exact waiting policy may later be optimized, but the first T0 proof should favor correctness and explicit timing evidence over throughput.

### Required telemetry

Per slot / pair record:

- consumer query creation result;
- `End` sequence number / pairSeq;
- number of `GetData` polls;
- final HRESULT;
- final BOOL value when applicable;
- time from consumer submit to completion;
- slot state transition `CONSUMER_READING -> CONSUMER_FENCING -> FREE`;
- no producer reuse before the transition to FREE.

Do not promote `ID3D11DeviceContext::Flush` as equivalent to GPU completion without separate proof.

## 2. OpenXR and CSGOVR have two separate ownership domains

OpenXR acquire/wait/release protects the application/runtime ownership of an XR swapchain image.

The CSGOVR EVENT query protects the producer/consumer ownership of the imported legacy D3D9 shared source.

They are not the same lifecycle.

A correct eye path is conceptually:

`D3D9 producer EVENT complete`
` -> shared slot READY`
` -> D3D11 consumes imported source`
` -> D3D11 EVENT complete`
` -> source slot may become FREE`

while independently:

`xrAcquireSwapchainImage`
` -> xrWaitSwapchainImage`
` -> application writes acquired XR image`
` -> xrReleaseSwapchainImage`
` -> xrEndFrame`

Do not release a CSGOVR source slot merely because an XR image was released, and do not treat a D3D11 source query as an OpenXR ownership primitive.

## 3. Color encoding is part of transport correctness

Current OpenXR 1.1 states that composition is assumed to operate internally in linear color space.

If submitted image values are intended to represent non-linear sRGB-encoded color, the swapchain must use an API-specific sRGB format such as `DXGI_FORMAT_R8G8B8A8_UNORM_SRGB` so the runtime knows that sRGB-to-linear conversion is required when sampling it. Other formats are treated as linear values.

OpenXR also recommends avoiding linear-encoded 8-bit color such as plain `DXGI_FORMAT_R8G8B8A8_UNORM` when possible because of potential banding.

Therefore **format equality alone is not enough**. T0 must record and preserve the intended color encoding.

### Do not guess the game eye encoding

The D3D9 replacement surface may report `X8R8G8B8`, `A8R8G8B8`, `A8B8G8R8`, or another concrete format, but that surface format by itself does not prove whether the pixel values at that stage should be interpreted as linear light or display/sRGB encoded.

T0 should harvest:

- source/replacement D3DFORMAT;
- relevant D3D9 sRGB-write state if practical at the eye snapshot seam;
- transport carrier format;
- selected OpenXR DXGI swapchain format;
- exact copy/blit path;
- whether any shader performs explicit linear<->sRGB conversion.

Do not add a gamma transform unless evidence requires it.

## 4. Swapchain format selection policy

`xrEnumerateSwapchainFormats` returns concrete graphics-API formats in runtime preference order. With D3D11 these are concrete DXGI formats, never typeless formats.

The CSGOVR canonical evidence currently preserves Quest FOV, eye sizes and D3D11 adapter LUID, but does **not** preserve an authoritative CSGOVR list of the Meta runtime's enumerated swapchain formats.

Consequently the future T0 x64 host must enumerate and log them as a small secondary harvest. This does not justify a separate FOV/OpenXR rediscovery experiment.

Preferred policy:

1. enumerate the full format list;
2. retain runtime order in the log;
3. prefer an sRGB color format if supported and if that matches the intended encoded values from the transport path;
4. otherwise choose a supported concrete format and explicitly document the color interpretation/conversion;
5. never hard-code a format solely because another VR project or older Meta run used it.

## 5. D3D11 shader-blit fallback requires COLOR_ATTACHMENT usage

OpenXR's D3D11 mapping guarantees:

- `XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT` -> `D3D11_BIND_RENDER_TARGET`;
- `XR_SWAPCHAIN_USAGE_SAMPLED_BIT` -> `D3D11_BIND_SHADER_RESOURCE`;
- `XR_SWAPCHAIN_USAGE_TRANSFER_SRC_BIT` / `TRANSFER_DST_BIT` -> no additional D3D11 bind flag;
- D3D11 XR swapchain textures use `D3D11_USAGE_DEFAULT`.

Therefore:

### direct-copy candidate

Request at least the intended transfer-destination usage and prove the actual D3D11 copy succeeds from the returned texture desc.

### `A1b-D11` shader-blit/scaler candidate

Also request `XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT`, because the fallback needs an RTV-compatible XR destination.

If a future implementation wants to sample an XR swapchain image itself, request `SAMPLED_BIT` for that use rather than assuming it.

## 6. View-format discipline

D3D11 views are typed interpretations of resources. Shader-resource and render-target views must use formats compatible with the resource.

For T0:

- log imported shared texture `D3D11_TEXTURE2D_DESC`;
- log SRV format used for a shader-blit source;
- log XR destination texture desc;
- log RTV format used for shader-blit destination;
- log whether selected swapchain format is UNORM or UNORM_SRGB;
- do not silently reinterpret an sRGB path as linear or vice versa.

The OpenXR runtime may internally allocate a typeless DXGI resource, but it interprets the image according to the concrete format selected by the application at swapchain creation. T0 should reason about the application-selected concrete format, not the hidden allocation format.

## 7. T0 PASS additions

Add these fields to eventual T0 evidence:

- enumerated OpenXR DXGI format list in runtime preference order;
- selected concrete XR format;
- selected swapchain usage flags;
- source/carrier color-format metadata;
- direct-copy vs D3D11 shader-blit path;
- SRV/RTV formats when shader-blit is used;
- consumer `D3D11_QUERY_EVENT` result per submitted pair;
- no source-slot reuse before consumer query completion;
- color-space policy (`PRESERVE_ENCODED`, `LINEAR_PATH`, or explicit conversion) with evidence;
- headset color verdict as a regression observation, not merely transport counters.

## KEEP / ADAPT / STOP

- `KEEP`: explicit two-sided GPU completion before resource ownership handoff.
- `ADAPT`: x64 consumer uses `D3D11_QUERY_EVENT` as its first explicit completion primitive.
- `ADAPT`: request `COLOR_ATTACHMENT_BIT` when retaining D3D11 shader-blit/scaler fallback.
- `STOP`: `Flush == GPU finished` assumption.
- `STOP`: hard-coded XR swapchain format without enumeration.
- `STOP`: treating plain UNORM and UNORM_SRGB as interchangeable transport semantics.

No camera, projection, pose, culling, viewmodel or HUD mutation belongs in this contract.