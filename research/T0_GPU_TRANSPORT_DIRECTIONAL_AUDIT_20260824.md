# CSGOVR T0 GPU transport directional audit — 2026-08-24

Evidence label: **STATIC-AUDITED / NOT LIVE-PROVEN**.

## Scope

Resolve the exact T0 GPU-export architecture around protected parent:

`CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`

Pinned source:
`advancedfx/advancedfx@b97636852b8eecae09285b5a386192bb285638eb`.

No projection, origin, pose, culling, viewmodel or HUD mutation belongs in this audit.

## Direction result

### Old pre-v6/v7 replacement-target path

`DrawingThreadPrepareDraw` can receive `sharedColorTextureHandle`, inspect the HLAE tracked game RT, open a same-size/same-format D3D9 replacement render-target texture using the supplied handle, obtain its surface and call `AfxD3d9PushRenderTargetEx`.

This is an output/replacement-target path:

**game render -> shared replacement target**.

Class: exact-source export-capable primitive PROVEN.

### Modern v6/v7 shared-texture Continue

`HandleDrawingMessage` receives an external shared texture, opens it, binds it through `AfxDrawRect` as a texture source and draws into the current D3D9 target.

Direction:

**external shared texture -> game/current RT composite**.

It is not proof that a completed LEFT/RIGHT game eye is exported.

Class: exact-source directional semantics PROVEN.

## Exact binary promotion

The first v1.1.3 heuristic incorrectly ranked `AfxHookSource.dll+0x36150` as Push. Manual exact-source review proved it is an R32F helper lineage and FALSIFIED it as Push.

Corrected v1.1.4 exact-hash CI independently reproduced the manually reviewed primitives:

- PushRT: `AfxHookSource.dll+0x00034830`
- PopRT: `+0x000346A0`
- GPU EVENT wait: `+0x000343F0`

Push entry signature:

`558BECA1B0D6211083EC0C85C00F848100000056578B7D08B91CD721108945F8`

The HLAE binary seam is therefore no longer a current unknown.

See `research/T0_HLAE_BINARY_PROMOTION_20260824.md`.

## Replacement surface is authoritative staging, not necessarily final transport storage

Pinned HLAE source opens the replacement texture using the current tracked RT's exact width, height and format, then Pushes that replacement and later `PopRenderTarget(true)` restores/copies back through HLAE's tracked state.

HLAE stores one persistent `m_RenderTargetReplacement`, so pointer persistence does not imply persistent LEFT and RIGHT contents.

Treat the replacement as **mutable authoritative eye staging**.

Required per-pass invariant:

`pass1 LEFT -> immediate GPU snapshot to LEFT transport slot`

before

`pass2 RIGHT -> immediate GPU snapshot to RIGHT transport slot`.

Do not wait until both eye renders are over to capture both from one mutable staging surface.

See `research/T0_REPLACEMENT_SURFACE_EXPORT_TOPOLOGY_20260824.md`.

## Current GPU candidates

### A1a — direct staging-resource sharing

Eligible only if runtime proves all of:

- replacement format is inside documented D3D9->D3D11 direct interop set;
- non-MSAA;
- shared creation succeeds;
- size is compatible with chosen x64 operation;
- x64 import succeeds on OpenXR-required D3D11 adapter;
- producer/consumer ownership prevents staging overwrite while consumer reads it.

Because HLAE staging may be reused sequentially, direct A1a can lose to dedicated carriers for lifetime reasons even if its format is technically shareable.

### A1b-D9 — preferred dedicated D3D9 carrier

`authoritative HLAE staging eye`
` -> immediate D3D9 GPU StretchRect/resolve/format/size bridge`
` -> dedicated non-MSAA shared carrier`
` -> D3D9 EVENT producer fence`
` -> READY(pairSeq, eye, generation)`.

Preferred 8-bit carrier candidate:
`D3DFMT_A8B8G8R8 -> DXGI_FORMAT_R8G8B8A8_UNORM`.

Actual format conversion/resolve support remains a runtime device-cap fact.

### A1b-D11 — x64 shader-blit/scaler

If a legal shared carrier can be imported at source size but XR destination size/format differ:

`shared carrier -> D3D11 SRV -> fullscreen shader blit -> XR RTV`.

The XR swapchain must request `COLOR_ATTACHMENT_BIT` for this path. Its exact format/color interpretation must come from `xrEnumerateSwapchainFormats`, not a hard-coded guess.

### A2 — naive post-hook raw RT0 copy

STOP. HLAE may restore raw RT state before a naive outer-hook post-copy, while the explicit replacement surface is already a better source object.

### B — prepared CPU fallback

`CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF` remains fallback only after a concrete GPU blocker or as a bounded compatibility control. It is not T0 primary.

## Windows interop ownership contract

D3D9Ex legacy sharing is unsynchronized. The transport ring must be an ownership protocol, not merely two buffers.

Minimum state machine:

`FREE -> PRODUCER_WRITING -> PRODUCER_FENCING -> READY -> CONSUMER_READING -> CONSUMER_FENCING -> FREE`.

Producer publishes only after D3D9 EVENT completion.

Consumer returns slot to FREE only after explicit D3D11 GPU completion, with `D3D11_QUERY_EVENT` the preferred initial proof primitive.

Pair coherence sits above slot ownership:

- same HLAE cycle;
- same `pairSeq`;
- same reset generation;
- both eyes READY;
- `mixedPairCount = 0`.

Legacy shared handle tokens are not normal NT handles: do not `CloseHandle` / `DuplicateHandle`; invalidate them by resource/generation lifetime.

See:
- `research/T0_WINDOWS_D3D9EX_D3D11_INTEROP_CONTRACT_20260824.md`
- `research/T0_D3D11_CONSUMER_SYNC_COLOR_CONTRACT_20260824.md`

## OpenXR destination contract

Current accepted Quest facts remain:

- Meta/Oculus OpenXR `1.205.0`;
- Quest 3;
- recommended eye `1504x1616`;
- required D3D11 adapter LUID previously `00000000:000118D1`;
- min feature level previously `0xB000`.

But canonical CSGOVR evidence did not preserve the actual `xrEnumerateSwapchainFormats` list. T0 must harvest it in the real host as secondary evidence.

Direct D3D11 `CopyResource` cannot scale. If source/destination dimensions differ, use A1b-D9 sizing or A1b-D11 shader blit.

Color encoding is part of correctness: OpenXR distinguishes plain linear UNORM from sRGB formats. Do not silently interchange them.

See:
- `research/T0_OPENXR_D3D11_COPY_CONTRACT_20260824.md`
- `research/T0_D3D11_CONSUMER_SYNC_COLOR_CONTRACT_20260824.md`.

## Bounded compatibility control status

`experiments/t0_rt_format_compat_control/` is **PREPARED / GATED / DO NOT RUN**.

The reviewed template now contains:

- Push `+0x34830` and exact signature;
- Pop `+0x346A0`;
- GPU wait `+0x343F0`;
- v1.1.3 false Push `+0x36150` explicitly falsified.

But it intentionally remains:

- `Status=NOT_PROMOTED`
- `ReviewerVerdict=REVIEWED_COLD_ONLY`.

No active `PROMOTED_RVAS.ini` is committed. `BUILD_GATED.ps1` refuses an active build until the bounded LIVE is intentionally authorized.

### Future compatibility-control PRIMARY

Observe exact replacement staging descriptor/device capability and decide A1a vs A1b.

Required harvest:

- staging pointer identity pass1/pass2/cycles;
- width/height/format/MSAA;
- D3D9Ex availability;
- producer adapter LUID when available;
- direct shared creation result;
- `A8B8G8R8` carrier support;
- format-conversion capability;
- no camera/projection/origin mutation;
- no actual OpenXR submission.

## KEEP / ADAPT / STOP

- `KEEP`: v0.5.5 pass1 LEFT / pass2 RIGHT / pass3 monitor.
- `KEEP`: HLAE replacement surface as authoritative eye staging.
- `KEEP`: exact promoted Push/Pop/Event primitives.
- `ADAPT`: snapshot each eye immediately into dedicated transport ownership before next pass can overwrite staging.
- `ADAPT`: A1b split into D3D9 carrier and D3D11 shader-blit variants.
- `ADAPT`: explicit D3D9 producer + D3D11 consumer fences.
- `STOP`: modern v6/v7 shared-texture Continue as game-export evidence.
- `STOP`: `+0x36150` as PushRT.
- `STOP`: naive raw-RT0 post-copy.
- `STOP`: `CopyResource` as scaler.
- `STOP`: hard-coded XR format/gamma assumptions.

## Current gate

HLAE static/binary archaeology is sufficiently saturated for T0.

Next information-efficient step remains one bounded D3D9 compatibility control, not another broad source scan and not a full OpenXR transport LIVE.

No runtime RESULT is claimed by this document.