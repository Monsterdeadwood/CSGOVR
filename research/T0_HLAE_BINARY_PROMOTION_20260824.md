# T0 HLAE exact-binary promotion — 2026-08-24

Evidence class: **COLD / STATIC / EXACT-BINARY + EXACT-SOURCE REVIEW + INDEPENDENT CORRECTED-ORACLE REPLICATION**.

Runtime authorization: **NO by this document alone**.

## Inputs

- HLAE: `2.191.1`
- AdvancedFX source: `b97636852b8eecae09285b5a386192bb285638eb`
- exact `AfxHookSource.dll` SHA256: `5B9D13AE9ABFFEEF80851E10861302162A47D1B43D8483B4A8FCEC7EAA19B035`
- first COLD CI run: `32683870149`
- first COLD artifact: `9505100191` / `csgovr-t0-hlae-cold-32683870149`
- first artifact digest: `sha256:46d52ea178ed663af32932bd0fa1a5f1d051606811672431bab842bf8a90dd44`
- first full scan SHA256: `5A90DA7BCC6A068460C8652FA17942E940CA62E614A4D1D0B863C7B6241B9124`
- first compact-result commit: `b45ebd15992bb8d10edc7f18517f9462d25df0ec`
- corrected v1.1.4 compact-result commit: `cb843ff21f93f8942277da090dfa967182fb500d`
- corrected v1.1.4 source commit under analysis: `b1e5a296c8523eb879523353ded1a6eafc2e9465`
- corrected v1.1.4 full scan SHA256: `8DAF6C9574F959D80C50EAE26F635D90192FE6353910263672E7353636D38476`
- Ghidra: `Ghidra_12.1.3_build`

Both CI routes resolved `hlae_2_191_1.zip` from the official AdvancedFX release and accepted the target only after the exact `AfxHookSource.dll` SHA256 matched. No game launch, HLAE launch, injection or patching occurred.

## Manual source-semantic review of v1.1.3

The original v1.1.3 heuristic summary must **not** be used as an automatic promotion table. Exact pinned source and exact-binary decompilation were compared manually.

### `AfxHookSource.dll+0x00036150` — FALSIFIED as PushRT

v1.1.3 ranked `+0x36150` as its top Push candidate because it contains `GetRenderTarget` + `SetRenderTarget` and no `StretchRect`.

That classification is wrong.

Binary evidence at this function includes `CreateTexture` with format immediate `0x72`. `0x72 == 114 == D3DFMT_R32F`. The broader function creates/manages an R32F target and then switches RT state. This matches the pinned-source R32F render-target helper lineage, not `AfxD3d9PushRenderTargetEx`.

**Verdict:** `FALSIFIED_AS_PUSH`. Keep only as an R32F helper lineage candidate. Do not use it for T0 replacement-target transport.

### `AfxHookSource.dll+0x00034830` — PROVEN_SOURCE_MATCH Push primitive

Exact-binary semantics:

- checks the current HLAE tracked render target;
- establishes source/replacement mapping;
- calls `GetRenderTarget(0)`;
- if raw RT0 equals the tracked source, calls `SetRenderTarget(0, replacementSurface)`;
- releases the temporary current-target reference;
- changes the tracked RT to the replacement surface;
- AddRefs the replacement.

This agrees with exact pinned source:

`AfxD3d9PushRenderTargetEx(IDirect3DSurface9 *replacementSurface)`.

The exact call graph shows callers push the replacement-surface pointer on the stack, call `+0x34830`, and clean the stack afterward. No separate unknown plain-wrapper gate is needed for the bounded compatibility-control seam.

**Verdict:** `PROVEN_SOURCE_MATCH_PRIMITIVE`.

### `AfxHookSource.dll+0x000346A0` — PROVEN_SOURCE_MATCH Pop primitive

Exact-binary semantics:

- resolves the original source surface from replacement mapping;
- calls `GetRenderTarget(0)`;
- conditionally restores RT0 with `SetRenderTarget`;
- if the boolean argument is true, executes `StretchRect(replacement -> source)`;
- unlinks source/replacement mapping;
- releases replacement and restores tracked state.

This agrees with pinned source `AfxD3d9PopRenderTarget(bool stretchRect)`.

**Verdict:** `PROVEN_SOURCE_MATCH_PRIMITIVE`.

`+0x16D60` is an outer helper that calls Pop with `false` and decrements a counter. It is infrastructure, not the promoted primitive.

### `AfxHookSource.dll+0x000343F0` — PROVEN_SOURCE_MATCH GPU completion primitive

Exact-binary semantics:

- uses stored `IDirect3DQuery9*`;
- calls query vtable slot `+0x18` with `D3DISSUE_END`;
- repeatedly calls query slot `+0x1C` (`GetData`) with `D3DGETDATA_FLUSH`;
- loops while return value is `S_FALSE`.

This is a direct binary match to the pinned AdvancedFX EVENT-query GPU wait.

**Verdict:** `PROVEN_SOURCE_MATCH_PRIMITIVE`.

## Corrected v1.1.4 independent replication

The corrected oracle changed the Push role to require:

`GetRT + SetRT + NO StretchRect + NO CreateTexture`

and explicitly treated RT-switch helpers that create textures as anti-candidates for Push.

On the same official exact-hash binary it independently returned:

| Role | v1.1.4 RVA | Entry signature (32 bytes) |
|---|---:|---|
| PushRT | `0x00034830` | `558BECA1B0D6211083EC0C85C00F848100000056578B7D08B91CD721108945F8` |
| PopRT | `0x000346A0` | `558BEC83EC18568B35B0D6211085F60F84DC0000008D45FC8975FC508D45E8B9` |
| GPU EVENT wait | `0x000343F0` | `833DE0D5211000742E8B0DBCD6211085C974248B016A0151FF50180F1F440000` |

It also retained the known correction:

`0x00036150 = R32F helper / NOT PushRT`.

This is an independent COLD replication of the manual exact-source review rather than a new hypothesis.

## Final promoted exact-binary table

| Role | RVA | Verdict |
|---|---:|---|
| HLAE-state-aware PushRT primitive | `0x00034830` | `PROVEN_SOURCE_MATCH_PRIMITIVE + V114_REPLICATED` |
| HLAE-state-aware PopRT primitive | `0x000346A0` | `PROVEN_SOURCE_MATCH_PRIMITIVE + V114_REPLICATED` |
| EVENT-query GPU completion primitive | `0x000343F0` | `PROVEN_SOURCE_MATCH_PRIMITIVE + V114_REPLICATED` |
| v1.1.3 false Push | `0x00036150` | `FALSIFIED_AS_PUSH_R32F_HELPER` |
| outer Pop helper | `0x00016D60` | `INFRASTRUCTURE / NOT PRIMITIVE` |
| outer Push-side helper | `0x00016D80` | `INFRASTRUCTURE / NOT PRIMITIVE` |

## Consequence for T0

The HLAE binary seam is now closed strongly enough that another broad HLAE RT archaeology pass has lower information value than the bounded compatibility control.

Remaining facts are runtime compatibility facts:

1. authoritative replacement-eye pointer reuse / dimensions / D3DFORMAT / MSAA;
2. D3D9 shared-carrier creation and conversion capability on the frozen device;
3. producer adapter identity when D3D9Ex LUID is available;
4. x64 D3D11 import on the OpenXR-required adapter;
5. producer and consumer GPU ownership synchronization;
6. immediate per-pass L/R snapshots before mutable HLAE staging can be overwritten;
7. coherent same-cycle pair publication.

## Oracle status

- v1.1.3: retained for provenance, known false-positive ranking corrected.
- v1.1.4: corrected role classifier; independently reproduced all three promoted primitive RVAs and emitted exact entry signatures.
- manual exact-source semantic review remains the scientific promotion gate even when future heuristics agree.

No camera/projection/pose/culling/viewmodel/HUD change is authorized by this document, and no full transport LIVE is claimed.