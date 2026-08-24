# CSGOVR T0 RT format compatibility control

Status: **PREPARED / GATED / DO NOT RUN**.

This is the next bounded runtime control after `experiments/t0_hlae_rt_primitive_oracle/` closes and a human/source-semantic review promotes the exact Push wrapper RVA and machine-byte signature.

It is deliberately not a transport implementation. It does not submit OpenXR frames and must not alter projection, origin, camera, eye separation, culling, viewmodel or HUD.

## Single PRIMARY question

For the HLAE replacement surface associated with the frozen v0.5.5 eye schedule, what actual D3D9 descriptor/capability state selects the next GPU transport path?

- **A1a direct** only if the observed target is non-MSAA, uses a D3D9 format allowed by the D3D9->D3D11 shared-resource contract, the device is D3D9Ex-capable, and same-format shared `CreateTexture` succeeds.
- **A1b GPU bridge** if direct sharing is blocked but a non-MSAA `D3DFMT_A8B8G8R8` render-target carrier is supported, shared creation succeeds, and the device reports the relevant `StretchRect` format conversion as supported.
- **NO VERDICT** if the relevant HLAE surface cannot be correlated or capability results are contradictory.

A failure of A1a does not imply CPU fallback. A1b remains the preferred GPU-only fallback until separately falsified.

## Why the package is hard-gated

The exact wrapper RVA is not known yet. The COLD v1.1.2 oracle intentionally emits heuristic member/wrapper finalists, not PROVEN addresses. This control therefore has no usable default RVA.

`PROMOTED_RVAS.template.ini` contains `Status=NOT_PROMOTED` and zero placeholders. The builder must refuse to emit a DLL unless a reviewed `PROMOTED_RVAS.ini` contains all of:

- `Status=PROVEN_SOURCE_MATCH`
- exact COLD RESULT SHA256
- exact pinned AfxHookSource SHA256
- non-zero Push wrapper RVA
- a non-empty machine-byte signature for that wrapper

This prevents a guessed or stale RVA from becoming a LIVE hook.

## Intended observation seam

The promoted global `AfxD3d9PushRenderTargetEx(IDirect3DSurface9 *replacementSurface)` wrapper is preferred because its argument is already an HLAE-owned replacement surface. The probe can query the surface without rebinding it:

1. `IDirect3DSurface9::GetDesc`
2. `IDirect3DSurface9::GetDevice`
3. device `QueryInterface(IDirect3DDevice9Ex)`
4. `GetCreationParameters` / `GetDirect3D`
5. `CheckDeviceFormat`
6. `CheckDeviceFormatConversion`
7. one-shot unbound shared `CreateTexture` attempts

The probe calls the original Push wrapper normally. It does **not** call `SetRenderTarget`, `StretchRect`, `GetRenderTargetData`, OpenXR, or any pixel readback function.

A read-only hook on the already-proven `CAfxStreams::DoRenderView +0x22340` may be used only to timestamp/pass-label the existing force-cache pass1/pass2 schedule. It must pass the original `view` pointer unchanged.

## Microsoft contract carried into the verdict

Microsoft documents D3D9->D3D11 shared textures as 2D, one mip, default usage, non-MSAA, created through D3D9 `CreateTexture(..., pSharedHandle)`. The documented cross-API format set is limited to the D3D9 equivalents of `R10G10B10A2_UNORM`, `R16G16B16A16_FLOAT`, and `R8G8B8A8_UNORM`.

`IDirect3D9::CheckDeviceFormatConversion` can be used to test format combinations permitted for `StretchRect`, and `StretchRect` can resolve a multisampled render target into a non-multisampled render target.

Reference documentation:

- https://learn.microsoft.com/windows/win32/api/d3d11/nf-d3d11-id3d11device-opensharedresource
- https://learn.microsoft.com/windows/win32/api/d3d9/nf-d3d9-idirect3d9-checkdeviceformatconversion
- https://learn.microsoft.com/windows/win32/api/d3d9/nf-d3d9-idirect3ddevice9-stretchrect

## Explicit exclusions

This control does not prove D3D11 import, adapter-LUID match, GPU synchronization, pair coherence, or OpenXR submission. Those remain later gates. The purpose here is narrower: select **direct shared target vs GPU carrier conversion** from actual frozen D3D9 runtime facts.

## Current action

Do not create `PROMOTED_RVAS.ini` manually. First run and return the COLD RT primitive RESULT. Review its decompilation/call graph against pinned source, then promote or reject the wrapper candidate. Only after that review may this package be built.
