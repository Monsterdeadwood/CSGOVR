# T0 HLAE exact-binary promotion — 2026-08-24

Evidence class: **COLD / STATIC / EXACT-BINARY + EXACT-SOURCE REVIEW**.

Runtime authorization: **NO by this document alone**.

## Inputs

- HLAE: `2.191.1`
- AdvancedFX source: `b97636852b8eecae09285b5a386192bb285638eb`
- exact `AfxHookSource.dll` SHA256: `5B9D13AE9ABFFEEF80851E10861302162A47D1B43D8483B4A8FCEC7EAA19B035`
- COLD CI run: `32683870149`
- COLD artifact: `9505100191` / `csgovr-t0-hlae-cold-32683870149`
- artifact digest: `sha256:46d52ea178ed663af32932bd0fa1a5f1d051606811672431bab842bf8a90dd44`
- full scan SHA256: `5A90DA7BCC6A068460C8652FA17942E940CA62E614A4D1D0B863C7B6241B9124`
- compact-result commit: `b45ebd15992bb8d10edc7f18517f9462d25df0ec`
- Ghidra used by CI: `Ghidra_12.1.3_build`

The CI resolved `hlae_2_191_1.zip` from the official AdvancedFX release and accepted the binary only after the exact SHA256 matched. No game launch, HLAE launch, injection or patching occurred.

## Manual source-semantic review

The v1.1.3 heuristic summary must **not** be used as an automatic promotion table. Exact pinned source and exact-binary decompilation were compared manually.

### `AfxHookSource.dll+0x00036150` — FALSIFIED as PushRT

The v1.1.3 heuristic ranked `+0x36150` as its top Push candidate because it contains `GetRenderTarget` + `SetRenderTarget` and no `StretchRect`.

That classification is wrong.

Binary evidence at this function includes `CreateTexture` with format immediate `0x72`. `0x72 == 114 == D3DFMT_R32F`. The broader function creates / manages an R32F target and then switches RT state. This matches the pinned-source R32F depth-target helper lineage, not `AfxD3d9PushRenderTargetEx`.

**Verdict:** `FALSIFIED_AS_PUSH`. Keep only as an R32F helper lineage candidate. Do not use it for T0 replacement-target transport.

### `AfxHookSource.dll+0x00034830` — PROVEN_SOURCE_MATCH Push primitive

Exact-binary semantics:

- checks the current HLAE tracked render target;
- establishes source/replacement mapping;
- calls `GetRenderTarget(0)`;
- if raw RT0 equals the tracked source, calls `SetRenderTarget(0, replacementSurface)`;
- releases the temporary current target reference;
- changes the tracked RT to the replacement surface;
- AddRefs the replacement.

This agrees with the exact pinned source behavior of:

`AfxD3d9PushRenderTargetEx(IDirect3DSurface9 *replacementSurface)`.

**Verdict:** `PROVEN_SOURCE_MATCH_PRIMITIVE`.

Do not overclaim a separate exported/free wrapper identity. A caller around `+0x16D80` performs additional bookkeeping / replacement-surface acquisition and is therefore not promoted as a plain Push wrapper.

### `AfxHookSource.dll+0x000346A0` — PROVEN_SOURCE_MATCH Pop primitive

Exact-binary semantics:

- resolves the original source surface from the replacement mapping;
- calls `GetRenderTarget(0)`;
- conditionally restores RT0 with `SetRenderTarget`;
- if the boolean argument is true, executes `StretchRect(replacement -> source)`;
- unlinks source/replacement mapping;
- releases the replacement and restores tracked state.

This agrees with pinned source `AfxD3d9PopRenderTarget(bool stretchRect)`.

**Verdict:** `PROVEN_SOURCE_MATCH_PRIMITIVE`.

`+0x16D60` is an outer helper that calls Pop with `false` and decrements a counter. It is not promoted as the plain free wrapper.

### `AfxHookSource.dll+0x000343F0` — PROVEN_SOURCE_MATCH GPU completion primitive

Exact-binary semantics:

- uses the stored `IDirect3DQuery9*`;
- calls query vtable slot `+0x18` with `D3DISSUE_END`;
- repeatedly calls query slot `+0x1C` (`GetData`) with `D3DGETDATA_FLUSH`;
- loops while return value is `S_FALSE`.

This is a direct binary match to the pinned AdvancedFX EVENT-query GPU wait.

**Verdict:** `PROVEN_SOURCE_MATCH_PRIMITIVE`.

## Promoted exact-binary table

| Role | RVA | Verdict |
|---|---:|---|
| HLAE-state-aware PushRT primitive | `0x00034830` | `PROVEN_SOURCE_MATCH_PRIMITIVE` |
| HLAE-state-aware PopRT primitive | `0x000346A0` | `PROVEN_SOURCE_MATCH_PRIMITIVE` |
| EVENT-query GPU completion primitive | `0x000343F0` | `PROVEN_SOURCE_MATCH_PRIMITIVE` |
| v1.1.3 auto-ranked Push `0x00036150` | `0x00036150` | `FALSIFIED_AS_PUSH` |
| outer Pop helper | `0x00016D60` | `INFRASTRUCTURE / NOT PLAIN WRAPPER` |
| outer Push-side helper | `0x00016D80` | `INFRASTRUCTURE / NOT PLAIN WRAPPER` |

## Consequence for T0

The binary seam itself is no longer an unknown. T0 does **not** need another broad HLAE RT archaeology pass.

The next unresolved facts are runtime compatibility facts:

1. authoritative replacement-eye RT format / dimensions / MSAA;
2. D3D9Ex shared-carrier creation capability for the exact device;
3. x64 D3D11 import on the OpenXR-required adapter;
4. producer/consumer GPU ownership synchronization;
5. coherent same-cycle L/R pair publication.

## Oracle correction requirement

v1.1.3 must be superseded for future automated ranking because `CreateTexture + GetRT + SetRT` caused an R32F helper to outrank the true Push primitive.

Next oracle policy should:

- require `GetRT + SetRT + !StretchRect + !CreateTexture` for the primary Push role;
- heavily penalize `CreateTexture` in a Push candidate;
- emit each candidate's own entry-byte signature, not only thin-caller signatures;
- retain manual exact-source semantic review as the promotion gate.

No new LIVE is authorized solely by this document.