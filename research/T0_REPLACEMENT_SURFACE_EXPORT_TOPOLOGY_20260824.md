# T0 HLAE replacement-surface export topology — 2026-08-24

Evidence class: **EXACT PINNED SOURCE + T0 DESIGN CONSEQUENCE**.

This document narrows the preferred A1 implementation. It does not claim runtime transport success.

## Exact HLAE source facts

Pinned source: `advancedfx/advancedfx@b97636852b8eecae09285b5a386192bb285638eb`.

In the old `PrepareDrawReply_Continue` replacement-target path HLAE:

1. reads the external shared color-texture handle;
2. obtains the authoritative HLAE tracked render target via `AfxGetRenderTargetSurface()`;
3. calls `GetDesc` on that tracked RT;
4. opens/creates the replacement texture using exactly:
   - `desc.Width`,
   - `desc.Height`,
   - one mip,
   - `D3DUSAGE_RENDERTARGET`,
   - `desc.Format`,
   - `D3DPOOL_DEFAULT`,
   - the supplied shared handle;
5. obtains surface level 0 as `m_RenderTargetReplacement`;
6. calls `AfxD3d9PushRenderTargetEx(m_RenderTargetReplacement)`.

At render-view end, if the replacement was pushed, HLAE calls:

`AfxD3d9PopRenderTarget(true)`.

The exact Push/Pop semantics are independently source-matched to binary primitives:

- Push: `AfxHookSource.dll+0x00034830`;
- Pop: `+0x000346A0`.

## What this proves about replacement size/format

The native old replacement path is **same-size / same-format** with the authoritative tracked RT.

Therefore do not design A1a around a replacement surface of arbitrary XR dimensions or an arbitrary carrier format. The pinned path itself mirrors the current game RT's width/height/format.

This removes one ambiguity from T0.

## Preferred A1 topology after this finding

### A1a — direct replacement sharing

Use the HLAE replacement surface itself as the cross-process shared resource only if all direct D3D9->D3D11 conditions hold:

- tracked eye format is inside the documented direct cross-API set;
- non-MSAA;
- game/replacement dimensions are compatible with the intended D3D11 destination operation;
- D3D11 import succeeds on the OpenXR-required adapter.

### A1b — authoritative replacement -> second GPU carrier

If format, MSAA or dimensions block direct A1a, **do not abandon the replacement surface as the eye source**.

Preferred topology:

`v0.5.5 eye render`
`    -> HLAE source-proven replacement surface (authoritative completed eye)`
`    -> GPU-only resolve / StretchRect / shader conversion into transport carrier`
`    -> producer EVENT fence`
`    -> READY(pairSeq, eye, generation, handle)`
`    -> x64 D3D11 OpenSharedResource`
`    -> direct copy OR D3D11 shader blit into acquired XR texture`
`    -> consumer completion fence`
`    -> FREE`

This separates **render correctness** from **transport compatibility**.

The HLAE replacement surface remains tied to the game RT contract. A second carrier is allowed to satisfy the D3D9->D3D11 transport contract.

## Why this is stronger than naive post-hook RT0 copy

A raw `GetRenderTarget(0)` after the outer `DoRenderView` hook returns is not authoritative because HLAE may already have restored RT state.

The replacement surface is better because:

- HLAE itself uses it as the render destination;
- source/replacement mapping is tracked internally;
- its pointer is passed through the exact Push primitive;
- it remains an explicit object independent of whatever RT0 is restored afterward.

Therefore a transport child may observe/cache the replacement-surface pointer at the proven Push seam, preserve its COM lifetime correctly, and use that explicit surface as the eye-source object after the eye render completes.

## Safe implementation seam candidate

The exact binary Push primitive at `+0x34830` compiles as a one-argument function in the observed binary call graph:

- caller pushes the replacement-surface pointer;
- calls `+0x34830`;
- fixes the stack afterward.

This allows a minimal hook on the Push primitive to observe the exact `IDirect3DSurface9* replacementSurface` without inventing a hidden `this` ABI.

The existing v0.5.5 `DoRenderView` pass discriminator remains the authority for LEFT/RIGHT/pass3 identity.

Candidate data flow:

1. PushRT hook observes replacement surface pointer and takes a bounded COM reference for the current render generation.
2. Original PushRT executes normally.
3. Existing v0.5.5 eye-local `DoRenderView` proceeds unchanged.
4. On return from the original eye render, transport uses the captured explicit replacement surface, not raw RT0.
5. GPU carrier operation + EVENT fence complete before pair publication.
6. temporary reference is released after the carrier no longer needs the source.

This must still be implemented and runtime-proven. It is a source-derived seam, not yet a LIVE result.

## Interaction with HLAE Pop(true)

Pinned HLAE calls `AfxD3d9PopRenderTarget(true)` at render-view end. Pop restores the tracked/original RT and StretchRects replacement -> original source when requested.

Transport code must not race or reorder that lifecycle. The safest initial design is observational/additive:

- let HLAE Push/Pop run exactly as designed;
- capture the explicit replacement object identity;
- perform transport work only at a proven post-render point where the replacement contains the completed eye;
- do not replace HLAE's internal source/replacement maps.

## New runtime facts still required

A bounded compatibility control should now log from the explicit replacement surface:

- width / height;
- D3DFORMAT;
- multisample type / quality;
- device / D3D9Ex availability;
- D3D9Ex adapter LUID;
- direct shared-create result;
- `A8B8G8R8` carrier capability;
- `CheckDeviceFormatConversion(source -> A8B8G8R8)`;
- if source MSAA: resolve-compatible carrier result;
- if XR dimensions differ: selected D3D9-side or D3D11-side scaling path.

No camera/projection/origin/culling/viewmodel/HUD mutation belongs in this control.

## KEEP / ADAPT / STOP

- `KEEP`: HLAE replacement target as authoritative completed-eye source.
- `ADAPT`: A1b becomes replacement-surface -> second GPU carrier, not raw-RT0 post-copy.
- `STOP`: assumption that native HLAE replacement can arbitrarily choose XR dimensions/format.
- `STOP`: naive post-return `GetRenderTarget(0)` as the preferred eye-source oracle.
- `KEEP`: v0.5.5 pass1 LEFT / pass2 RIGHT / pass3 monitor unchanged.