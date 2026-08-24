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

Therefore the final T0 consumer copy can remain a native D3D11 resource operation after the legacy D3D9 resource has been opened/imported into D3D11.

### Swapchain usage must declare intended usage

`XrSwapchainCreateInfo::usageFlags` must describe intended image use. `XR_SWAPCHAIN_USAGE_TRANSFER_DST_BIT` means the image may be used as a transfer destination.

For D3D11 specifically, the OpenXR specification maps this bit to **no additional D3D11 bind flag** (`ignored`). D3D11 OpenXR swapchain textures are created with `D3D11_USAGE_DEFAULT`.

Consequences:

- request `XR_SWAPCHAIN_USAGE_TRANSFER_DST_BIT` for a copy-based transport because it declares the intended OpenXR use;
- do not treat that flag as proof of a special D3D11 bind capability;
- inspect/log the returned texture description and prove the actual copy operation succeeds.

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

- complete relevant format list;
- selected format;
- eye width/height/sampleCount;
- requested usage flags;
- returned `ID3D11Texture2D` desc.

If the imported shared carrier format differs from the XR swapchain format, perform a documented D3D11 conversion/render/copy path instead of assuming `CopyResource` is legal across formats.

### Acquire / wait / copy / release discipline

For each coherent LEFT/RIGHT pair:

- `xrAcquireSwapchainImage` for each eye;
- `xrWaitSwapchainImage` before writing;
- perform the D3D11 copy/render into the acquired image;
- complete whatever consumer GPU ownership fence T0 adopts;
- `xrReleaseSwapchainImage` only after application writes for that image have been queued as required;
- submit one coherent projection layer through one `xrEndFrame` for the pair.

Counters/errors for every stage remain mandatory T0 evidence.

### Pair identity remains independent of OpenXR image index

OpenXR swapchain image index is not the game's stereo pair identity.

The host must carry `pairSeq` from producer metadata and may only submit LEFT and RIGHT from the same game/HLAE cycle. Acquiring two XR images does not itself prove coherent stereo.

### Copy operation must be proven, not inferred from usage flag

A future T0 result should log:

- imported D3D11 source texture desc;
- XR destination texture desc;
- copy/conversion operation chosen;
- HRESULT / debug-layer errors if available;
- consumer GPU completion result;
- per-eye acquire/wait/copy/release status.

`XR_SWAPCHAIN_USAGE_TRANSFER_DST_BIT` is a declared usage, not a substitute for this evidence.

## Interaction with Windows interop contract

The x64 consumer owns a slot only after the D3D9 producer has completed its EVENT fence and published that slot `READY`.

After the x64 host has consumed/copied the shared source, slot ownership must not return to D3D9 until the consumer GPU read is known complete. This is required by the unsynchronized D3D9Ex sharing model even though OpenXR has its own swapchain acquire/wait/release lifecycle.

These are two different ownership domains:

1. **CSGOVR shared-resource slot ownership** between D3D9Ex and D3D11;
2. **OpenXR swapchain-image ownership** between application and runtime.

Do not collapse them into one flag.

## T0 evidence additions

Add to the eventual T0 RESULT:

- `xrGetD3D11GraphicsRequirementsKHR` result;
- runtime-required LUID / min FL;
- actual D3D11 device LUID / FL;
- `xrEnumerateSwapchainFormats` relevant list;
- selected XR format and usage flags;
- D3D11 source texture desc for each transport slot;
- XR destination texture desc;
- copy/conversion result per eye;
- consumer GPU completion state;
- acquire/wait/release counts and errors;
- `xrEndFrame` count;
- pairSeq attached to each submitted eye;
- `mixedPairCount = 0`.

No projection, origin, head-pose, culling, viewmodel or HUD change belongs in this gate.