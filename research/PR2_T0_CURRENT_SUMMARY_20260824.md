# PR #2 current T0 research summary — 2026-08-24

Status: **COLD closure complete enough for bounded compatibility control; no T0 LIVE result claimed.**

Protected parent remains `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`.

## Closed static/binary facts

- old HLAE replacement path is game -> shared target; modern v6/v7 shared-texture Continue is external -> game/composite;
- exact PushRT primitive: `AfxHookSource.dll+0x34830`;
- exact PopRT primitive: `+0x346A0`;
- exact GPU EVENT wait primitive: `+0x343F0`;
- v1.1.3 false Push `+0x36150` is FALSIFIED as Push and identified as R32F helper lineage;
- corrected v1.1.4 exact-hash CI independently reproduced all three promoted primitive RVAs and entry signatures;
- native HLAE replacement is same-size/same-format with tracked RT;
- one persistent replacement object may be reused, so each eye must be snapshotted immediately before the next pass can overwrite staging;
- D3D9Ex legacy sharing is unsynchronized and requires explicit producer/consumer ownership;
- D3D11 `CopyResource` cannot scale;
- OpenXR D3D11 transfer bits do not create extra bind flags; shader-blit fallback requires `COLOR_ATTACHMENT_BIT`;
- OpenXR color interpretation distinguishes linear UNORM from sRGB formats;
- canonical CSGOVR evidence does not preserve the Meta runtime's actual swapchain-format enumeration, so T0 host must log it once as secondary harvest.

## Current preferred architecture

`pass1 LEFT -> HLAE replacement staging -> immediate LEFT GPU carrier -> D3D9 EVENT`

`pass2 RIGHT -> HLAE replacement staging -> immediate RIGHT GPU carrier -> D3D9 EVENT`

Same-cycle pair becomes READY only after both producer fences.

Then x64 D3D11 on the OpenXR-required adapter:

`OpenSharedResource -> direct copy OR shader-blit/scaler -> D3D11 EVENT consumer fence -> XR submission`.

Transport slots return FREE only after the consumer GPU has completed reading them.

## Candidate classes

- `A1a`: direct replacement sharing only if format/MSAA/size/lifetime/import all fit.
- `A1b-D9`: D3D9 GPU resolve/format/size bridge into dedicated `A8B8G8R8` shared carrier.
- `A1b-D11`: legal shared carrier at source size, then x64 D3D11 shader-blit/scaler into XR RTV.
- `B`: v0.7.1 CPU fallback remains fallback, not primary.

## Next gate

One bounded **D3D9 replacement-surface compatibility control** only.

PRIMARY: determine actual frozen replacement pointer reuse, width, height, D3DFORMAT, MSAA and sharing/conversion capabilities, then select A1a / A1b-D9 / A1b-D11.

The package `experiments/t0_rt_format_compat_control/` remains sealed:

- reviewed COLD primitive/signature values are in the template;
- template remains `Status=NOT_PROMOTED` / `ReviewerVerdict=REVIEWED_COLD_ONLY`;
- no active `PROMOTED_RVAS.ini` is committed;
- no LIVE is authorized by this summary.

## Broad research STOP list

Do not spend another cycle on broad HLAE RT RVA hunting, AfxInterop direction rediscovery, OpenXR FOV/pose rediscovery, projection/origin/culling/viewmodel work inside T0, sequential screenshot disparity analysis, or hard-coded XR format/gamma guesses.

The next useful information comes from the bounded compatibility result, not more broad static archaeology.