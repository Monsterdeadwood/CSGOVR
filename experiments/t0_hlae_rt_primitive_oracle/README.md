# CSGOVR T0 COLD HLAE RT primitive oracle v1.1.2

Evidence label: **STATIC-AUDITED / NOT LIVE-PROVEN**.

## Purpose

Resolve exact frozen `AfxHookSource.dll` binary candidates for the source-proven HLAE render-target replacement / GPU-completion machinery before building T0 transport FINALIST A1.

This package does not launch CS:GO, launch HLAE, inject a DLL, patch a file, or claim a runtime result.

## Exact input

- HLAE `2.191.1`
- AdvancedFX source identity `b97636852b8eecae09285b5a386192bb285638eb`
- `AfxHookSource.dll` SHA256 `5B9D13AE9ABFFEEF80851E10861302162A47D1B43D8483B4A8FCEC7EAA19B035`
- PE timestamp expected `0x6A58EC87`
- SizeOfImage expected `0x0023F000`
- known `CAfxStreams::DoRenderView` anchor RVA `0x22340`

## Run

`00_RUN_COLD_HLAE_RT_ORACLE.bat`

The BAT first runs a Windows PowerShell parser gate. The runner then locates the exact-hash DLL, stages a byte-identical copy, locates Ghidra under `C:\GOG Games\VR Research Tools`, runs the Java probe headlessly, and emits:

`CSGOVR_T0_HLAE_RT_PRIMITIVE_RESULT_<timestamp>.zip`

Return that ZIP to the CSGOVR analysis branch.

## Source-guided role ranking

The probe still performs a broad D3D9 call census, but ranks exact-source-shaped roles before generic candidates.

Pinned source contract:

- `AfxD3d9PushRenderTargetEx` member path: `GetRenderTarget(slot38 / 0x98)` plus `SetRenderTarget(slot37 / 0x94)`, with no `StretchRect` in the push body.
- `AfxD3d9PopRenderTarget` member path: `GetRenderTarget` plus `SetRenderTarget` plus the optional `StretchRect(slot34 / 0x88)` restoration/copy path.
- GPU wait member path: `IDirect3DQuery9::Issue(slot6 / 0x18)` plus `GetData(slot7 / 0x1C)` in a bounded wait loop. The EVENT query is created separately through `CreateQuery(slot118 / 0x1D8)`.

Heuristic role labels:

- `RT_PUSH_MEMBER_FINALIST`
- `RT_POP_MEMBER_FINALIST`
- `GPU_WAIT_MEMBER_FINALIST`

The scanner also enumerates **thin direct callers** to expose likely global wrappers around the underlying `g_NewDirect3DDevice9` member methods.

The v1.1.2 runner extracts compact role fields:

- `BestPushMemberRVA`
- `BestPushThinCallerRVA`
- `BestPopMemberRVA`
- `BestPopThinCallerRVA`
- `BestGpuWaitMemberRVA`
- `BestGpuWaitThinCallerRVA`
- `BoundedCoreFinalists`
- `SemanticVerdict`

Two hardening changes matter in v1.1.2:

1. member fields are normalized to pure `0x...` RVA values rather than carrying the rest of the scan line;
2. thin-caller extraction is isolated to one candidate block. If a role has no thin caller, it returns `NONE` and cannot accidentally steal a wrapper RVA from a later candidate.

`SemanticVerdict=BOUNDED_FINALISTS` means only that the source-shaped census collapsed to bounded candidates. Member and wrapper RVAs remain heuristic until decompile/call graph review against exact pinned source.

## Broader fingerprints retained

D3D9 device vtable fingerprints:

- slot 37 / displacement `0x94`: `SetRenderTarget`
- slot 38 / displacement `0x98`: `GetRenderTarget`
- slot 34 / displacement `0x88`: `StretchRect`
- slot 23 / displacement `0x5C`: `CreateTexture`
- slot 118 / displacement `0x1D8`: `CreateQuery`

Operand matching is deliberately tolerant to Ghidra rendering variants. Promotion is never based on a displacement alone; the candidate must also match callers, decompilation, and pinned source semantics.

For bounded candidates the scan records exact VA/RVA, source-guided role hint, matched operations, disassembly context, callers, thin callers, decompiled body, symbol-name matches, and the known DoRenderView anchor context.

## Source-semantic target

The strongest current target is the HLAE-aware replacement-target pair:

`AfxD3d9PushRenderTargetEx(replacementSurface)`

`AfxD3d9PopRenderTarget(stretchRect)`

Pinned source shows that Push links the current tracked source surface to the replacement, switches raw RT0 when appropriate, and updates HLAE `trackedRenderTarget`. Pop reverses that mapping and restores state. This is safer than an out-of-band `IDirect3DDevice9::SetRenderTarget` from an injected child.

The GPU completion path is source-proven to use an EVENT query: `Issue(D3DISSUE_END)` followed by `GetData(..., D3DGETDATA_FLUSH)` until completion.

## Why this gate exists

The 2026-08-24 directional audit proved that modern AfxInterop v6/v7 `HandleDrawingMessage Continue` opens an external shared texture and draws it into the current game RT. That is an input/composite direction, not proof of exporting a completed game eye.

The older replacement-target path is the exact source-proven game-render-to-shared-target primitive. T0 therefore needs either a verified compatible old protocol mode or a minimal transplant of its HLAE-state-aware mechanics.

A second guard remains: direct D3D9 -> D3D11 sharing depends on authoritative tracked eye RT format/MSAA. If direct sharing is incompatible, the preferred fallback remains GPU-only conversion into a compatible non-MSAA carrier, not CPU readback.

See:

- `research/T0_GPU_TRANSPORT_DIRECTIONAL_AUDIT_20260824.md`
- `research/T0_D3D9EX_D3D11_SHARED_FORMAT_AUDIT_20260824.md`

## PREFLIGHT

MISSING FACT: exact frozen HLAE binary RVAs/fingerprints for HLAE-state-aware RT replacement and GPU completion.

WHY NOT COLD: this experiment **is** the cold/static closure; runtime is unnecessary and unauthorized here.

PASS: source-guided binary fingerprints collapse PushRT/PopRT/GPU-wait to bounded finalists with decompile/caller context sufficient for exact source-semantic review.

FAIL: core roles remain ambiguous or contradict pinned source; then A1 is not authorized from this result.

BASELINE MUST HOLD: no CS:GO/HLAE launch, no injection, no patching, exact input hash only.
