# SELF VERIFY

Status before runtime: **PREPARED / UNRUN**.

- PASS: exact `AfxHookSource.dll` SHA256 is pinned.
- PASS: BAT runs a Windows PowerShell parser gate before execution.
- PASS: no game/HLAE launch command exists in the package.
- PASS: no injector or game-file write path exists in the package.
- PASS: Ghidra operates on a staged byte-identical DLL copy.
- PASS: `CAfxStreams::DoRenderView` known RVA `0x22340` is retained as an anchor only.
- PASS: D3D9 device slot fingerprints are explicit: SetRT 37, GetRT 38, StretchRect 34, CreateTexture 23, CreateQuery 118.
- PASS: D3D9 query slot fingerprints are explicit: Issue 6, GetData 7.
- PASS: v1.1 adds source-guided role ranking for Push member, Pop member and GPU-wait member.
- PASS: source role ranking remains explicitly heuristic and cannot auto-promote an RVA to PROVEN.
- PASS: thin direct callers are emitted to expose likely global wrapper functions around member implementations.
- PASS: v1.1.1 runner extracts first thin-caller RVA for Push/Pop/GPU-wait into the compact role summary.
- PASS: thin-caller extraction remains heuristic and is not runtime authorization.
- PASS: operand matching tolerates Ghidra rendering variants, while semantic promotion still requires callers/decompilation/source agreement.
- PASS: candidate output includes RVAs, role hints, call contexts, callers, thin callers and decompilation.
- PASS: runner emits `ROLE_SUMMARY.txt` and compact best-member/best-wrapper fields.
- PASS: `SemanticVerdict=BOUNDED_FINALISTS` means bounded static candidates only, not runtime authorization.
- PASS: exact five-line PREFLIGHT exists.
- PASS: output requires `SCAN_COMPLETE=YES` before packaging.
- PASS: RESULT is labeled STATIC-AUDITED / NOT LIVE-PROVEN.
- PASS: protected v0.5.5 GOLDEN is not modified.

Runtime verdict is intentionally impossible from this package.
