# CSGOVR T0 RT format compatibility control

Status: **PREPARED / GATED / DO NOT RUN**.

This is the next bounded runtime control after the HLAE exact-binary COLD closure. Exact source review plus corrected v1.1.4 CI now agree on the primitive seam:

- PushRT `AfxHookSource.dll+0x00034830`
- PopRT `+0x000346A0`
- GPU EVENT wait `+0x000343F0`
- v1.1.3 false Push `+0x00036150` = R32F helper, FALSIFIED as Push

Push entry signature:

`558BECA1B0D6211083EC0C85C00F848100000056578B7D08B91CD721108945F8`

The package remains intentionally inactive. The reviewed values live in `PROMOTED_RVAS.template.ini`, but the template keeps `Status=NOT_PROMOTED`; no active local `PROMOTED_RVAS.ini` is committed.

It is deliberately not a transport implementation. It does not submit OpenXR frames and must not alter projection, origin, camera, eye separation, culling, viewmodel or HUD.

## Single PRIMARY question

For the exact HLAE replacement surface associated with the frozen v0.5.5 eye schedule, what actual descriptor/device/capability state selects the next GPU transport path?

The control should harvest:

- replacement pointer identity and reuse pattern;
- width / height;
- D3DFORMAT;
- MSAA type / quality;
- device / D3D9Ex availability;
- D3D9 adapter ordinal and D3D9Ex LUID when available;
- direct shared `CreateTexture` result;
- `D3DFMT_A8B8G8R8` carrier support;
- `CheckDeviceFormatConversion(source -> A8B8G8R8)`;
- same-format / carrier shared-handle creation outcome.

## Transport selection after the control

### A1a direct

Remain eligible only if the observed replacement is non-MSAA, uses a documented direct D3D9->D3D11 compatible format, shared creation succeeds, and later D3D11 import succeeds on the OpenXR-required adapter.

Because pinned HLAE owns one persistent `m_RenderTargetReplacement`, direct sharing must also prove the staging resource cannot be overwritten while the x64 consumer still reads it. If that lifetime is awkward, prefer dedicated transport carriers even when the format itself is directly shareable.

### A1b-D9

Preferred when D3D9 can resolve/convert/scale the authoritative replacement eye into a dedicated non-MSAA `D3DFMT_A8B8G8R8` shared carrier.

This can solve format, MSAA and size incompatibility before cross-process import.

### A1b-D11

Retain when a legal carrier can be shared at source size but XR dimensions/format differ. The x64 host imports that carrier and performs a D3D11 shader blit/scaler into an XR swapchain created with `COLOR_ATTACHMENT_BIT` as well as the intended transfer usage.

A failure of A1a does not imply CPU fallback. GPU carrier paths remain preferred until concretely falsified.

## Why the package remains hard-gated

The binary seam is no longer unknown, but a COLD proof is not permission to mutate the frozen runtime.

`PROMOTED_RVAS.template.ini` contains the reviewed primitive addresses/signatures while intentionally keeping:

- `Status=NOT_PROMOTED`
- `ReviewerVerdict=REVIEWED_COLD_ONLY`

`BUILD_GATED.ps1` refuses to build an active probe unless a local-only `PROMOTED_RVAS.ini` explicitly changes both gates to `PROVEN_SOURCE_MATCH`.

This preserves the PRE-FLIGHT boundary between static proof and bounded LIVE.

## Observation seam

The exact `AfxD3d9PushRenderTargetEx(IDirect3DSurface9 *replacementSurface)` primitive at `+0x34830` is the observation seam.

Exact binary call graph shows a one-stack-argument ABI for the replacement-surface pointer; a separate guessed wrapper is not required.

The probe can inspect the HLAE-owned replacement surface without rebinding it:

1. call original Push primitive normally;
2. inspect `IDirect3DSurface9::GetDesc`;
3. `GetDevice`;
4. device `QueryInterface(IDirect3DDevice9Ex)` when available;
5. `GetCreationParameters` / `GetDirect3D`;
6. `IDirect3D9Ex::GetAdapterLUID` when available;
7. `CheckDeviceFormat`;
8. `CheckDeviceFormatConversion`;
9. bounded, unbound shared `CreateTexture` capability attempts.

The control must not call `SetRenderTarget`, execute a real transport `StretchRect`, call `GetRenderTargetData`, perform pixel readback, create OpenXR, or submit images.

## Mutable-staging rule

Pinned HLAE source stores one persistent `m_RenderTargetReplacement` and creates it only when absent/lost. Do not assume LEFT and RIGHT have separate HLAE surfaces.

The control should therefore log replacement pointer identity across pass1/pass2/cycles.

Actual T0 transport must snapshot each eye immediately after its eye render into dedicated transport storage before a later pass can overwrite the staging surface.

## Official platform contract carried into the verdict

D3D9->D3D11 sharing is unsynchronized. Producer and consumer need explicit ownership/fence logic.

The documented direct shared resource path requires a one-mip, DEFAULT, non-MSAA 2D texture in the narrow supported cross-API format set. `D3DFMT_A8B8G8R8` maps to `DXGI_FORMAT_R8G8B8A8_UNORM` and is the preferred 8-bit carrier candidate.

`CheckDeviceFormatConversion` is a capability check for D3D9 `StretchRect`; `StretchRect` can also resolve a multisampled RT to a non-MSAA RT. Actual device behavior remains runtime evidence.

If D3D9Ex is available, compare its producer adapter LUID with the OpenXR-required D3D11 LUID. If D3D9Ex is unavailable, record `LUID_UNAVAILABLE` rather than automatically falsifying sharing; the later real D3D11 import test is authoritative.

## Explicit exclusions

This control does not prove:

- D3D11 `OpenSharedResource` success;
- D3D9/D3D11 adapter match when LUID is unavailable;
- D3D11 consumer GPU completion;
- pair coherence;
- OpenXR swapchain formats;
- OpenXR image submission;
- visual color/gamma correctness.

Those are later/secondary T0 gates. The purpose here is narrower: select direct replacement sharing vs dedicated GPU carrier architecture from actual frozen D3D9 facts.

## Current action

Keep the package sealed until a bounded compatibility LIVE is intentionally authorized under the exact five-line PREFLIGHT. No more broad HLAE binary archaeology is needed before that decision.