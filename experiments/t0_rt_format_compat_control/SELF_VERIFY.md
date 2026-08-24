# SELF VERIFY

Current status: **PREPARED / GATED / DO NOT RUN**.

- PASS: exact five-line PREFLIGHT exists and asks one PRIMARY question only: A1a direct vs A1b GPU carrier readiness on the actual frozen D3D9 replacement surface.
- PASS: `PROMOTED_RVAS.template.ini` is intentionally non-authorizing (`Status=NOT_PROMOTED`, zero RVAs, unset signature).
- PASS: repository does not contain an authorizing `PROMOTED_RVAS.ini`.
- PASS: builder refuses unless `Status=PROVEN_SOURCE_MATCH` and `ReviewerVerdict=PROVEN_SOURCE_MATCH`.
- PASS: builder requires exact pinned `AfxHookSource.dll` SHA256 provenance.
- PASS: builder requires a 64-hex COLD RESULT SHA256.
- PASS: builder rejects zero/invalid Push wrapper RVA.
- PASS: builder requires a 6..64 byte promoted machine signature and generates the C header only after all promotion gates pass.
- PASS: build is Zig 0.16.0 x86 and uses source MinHook; no prebuilt unknown hook binary is accepted.
- PASS: build launcher runs a PowerShell parser gate first.
- PASS: build package contains no game launcher and no injector.
- PASS: probe independently requires exact `-insecure` in the target process.
- PASS: probe checks exact HLAE PE timestamp and SizeOfImage before touching the promoted address.
- PASS: probe compares promoted machine bytes before installing its hook.
- PASS: probe hooks only the promoted Push wrapper; it does not double-hook protected `CAfxStreams::DoRenderView` used by GOLDEN v0.5.5.
- PASS: probe calls the original Push wrapper and does not alter its replacement-surface argument.
- PASS: probe observation is limited to `GetDesc`, `GetDevice`, D3D9Ex QI, creation parameters, D3D9 capability checks, and one-shot unbound shared texture creation attempts.
- PASS: no `SetRenderTarget` call exists in probe source.
- PASS: no `StretchRect` execution exists in probe source; only `CheckDeviceFormatConversion` capability is queried.
- PASS: no `GetRenderTargetData`, LockRect pixel readback, CPU frame copy, OpenXR creation or OpenXR submission exists.
- PASS: unique-surface gating bounds resource creation attempts and avoids allocating on every Push call.
- PASS: direct verdict requires D3D9Ex, non-MSAA, documented cross-API format class, successful same-format shared CreateTexture and non-null shared handle.
- PASS: A1b D3D9-side readiness requires D3D9Ex, render-target support for `D3DFMT_A8B8G8R8`, successful `CheckDeviceFormatConversion`, successful shared carrier creation and non-null handle.
- PASS: D3D9-side readiness is not mislabeled as D3D11 import proof or OpenXR proof.
- PASS: no runtime RESULT is claimed by repository state.

### Expected current behavior

Running `00_BUILD_ONLY_AFTER_PROMOTION.bat` **before** semantic promotion must fail closed because `PROMOTED_RVAS.ini` is absent. That failure is the intended gate, not an experiment failure.
