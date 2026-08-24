# CSGOVR T0 COLD HLAE RT primitive oracle v1.0.0

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

## What the probe searches

D3D9 device vtable fingerprints used by the exact source implementation:

- slot 37 / displacement `0x94`: `SetRenderTarget`
- slot 38 / displacement `0x98`: `GetRenderTarget`
- slot 34 / displacement `0x88`: `StretchRect`
- slot 23 / displacement `0x5C`: `CreateTexture`
- slot 118 / displacement `0x1D8`: `CreateQuery`

Operand matching is deliberately tolerant to Ghidra rendering variants. Promotion is never based on the displacement alone; the candidate must also match callers, decompilation, and pinned source semantics.

For bounded candidates it records:

- exact VA/RVA;
- score and matched D3D9 operations;
- relevant disassembly context;
- direct callers;
- decompiled body;
- any surviving symbol-name matches;
- the known DoRenderView anchor decompilation for context.

The result is intentionally a candidate census. An RVA is promoted only when its exact binary semantics match the pinned source behavior.

## Source-semantic target

The strongest current target is the HLAE-aware replacement-target pair:

`AfxD3d9PushRenderTargetEx(replacementSurface)`

`AfxD3d9PopRenderTarget(stretchRect)`

Pinned source shows that Push links the current tracked source surface to the replacement, switches raw RT0 when appropriate, and updates HLAE `trackedRenderTarget`. Pop reverses that mapping and restores the state. This is safer than a blind out-of-band `IDirect3DDevice9::SetRenderTarget` from the injected child.

The oracle also harvests `CreateTexture` / `CreateQuery` candidates needed to map shared-resource creation and GPU completion.

## Why this gate exists

The 2026-08-24 directional audit proved that modern AfxInterop v6/v7 `HandleDrawingMessage Continue` opens an external shared texture and draws it into the current game RT. That is an input/composite direction, not proof of exporting a completed game eye.

The older replacement-target path is the exact source-proven game-render-to-shared-target primitive. T0 therefore needs either a verified compatible old protocol mode or a minimal transplant of its HLAE-state-aware mechanics.

See `research/T0_GPU_TRANSPORT_DIRECTIONAL_AUDIT_20260824.md`.

## PREFLIGHT

MISSING FACT: exact frozen HLAE binary RVAs/fingerprints for HLAE-state-aware RT replacement and GPU completion.

WHY NOT COLD: this is fully answerable from the exact-hash binary plus pinned source, so runtime would have lower information efficiency and unnecessary mutation risk.

PASS: bounded candidates match source semantics closely enough to select exact helper RVAs/fingerprints for A1 implementation.

FAIL: candidate semantics remain ambiguous or contradict pinned source; then A1 is not authorized from this result.

BASELINE MUST HOLD: no CS:GO/HLAE launch, no injection, no patching, exact input hash only.
