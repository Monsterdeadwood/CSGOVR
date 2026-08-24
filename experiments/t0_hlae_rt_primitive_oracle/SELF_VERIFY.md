# SELF VERIFY

Status before runtime: **PREPARED / UNRUN**.

- PASS: exact `AfxHookSource.dll` SHA256 is pinned.
- PASS: BAT runs a Windows PowerShell parser gate before execution.
- PASS: no game/HLAE launch command exists in the package.
- PASS: no injector or game-file write path exists in the package.
- PASS: Ghidra operates on a staged byte-identical DLL copy.
- PASS: `CAfxStreams::DoRenderView` known RVA `0x22340` is retained as an anchor only.
- PASS: D3D9 slot fingerprints are explicit: SetRT 37, GetRT 38, StretchRect 34, CreateTexture 23, CreateQuery 118.
- PASS: operand matching tolerates Ghidra rendering variants, while semantic promotion still requires callers/decompilation/source agreement.
- PASS: candidate output includes RVAs, call contexts, callers and decompilation.
- PASS: exact five-line PREFLIGHT exists.
- PASS: output requires `SCAN_COMPLETE=YES` before packaging.
- PASS: RESULT is labeled STATIC-AUDITED / NOT LIVE-PROVEN.
- PASS: no candidate RVA is automatically promoted to PROVEN.
- PASS: protected v0.5.5 GOLDEN is not modified.

Runtime verdict is intentionally impossible from this package.
