# SELF VERIFY

Status before runtime: **PREPARED / UNRUN**.

- PASS: exact `AfxHookSource.dll` SHA256 is pinned.
- PASS: BAT runs a Windows PowerShell parser gate before execution.
- PASS: no game/HLAE launch command exists in the local package.
- PASS: no injector or game-file write path exists in the local package.
- PASS: Ghidra operates on a staged byte-identical DLL copy.
- PASS: `CAfxStreams::DoRenderView` known RVA `0x22340` is retained as an anchor only.
- PASS: D3D9 device slot fingerprints are explicit: SetRT 37, GetRT 38, StretchRect 34, CreateTexture 23, CreateQuery 118.
- PASS: D3D9 query slot fingerprints are explicit: Issue 6, GetData 7.
- PASS: source-guided role ranking covers Push member, Pop member and GPU-wait member.
- PASS: source role ranking is explicitly heuristic and cannot auto-promote an RVA to PROVEN.
- PASS: thin direct callers are emitted to expose likely global wrapper functions around member implementations.
- PASS: v1.1.3 emits 6..32 entry machine bytes, bounded instruction listing and decompile for each thin wrapper candidate.
- PASS: runner extracts first thin-caller RVA and signature for Push/Pop/GPU-wait into the compact role summary.
- PASS: thin-caller parsing is isolated to one candidate block; zero-call candidates return `NONE` rather than borrowing another candidate's wrapper.
- PASS: best-member fields are normalized to pure `0x...` RVAs.
- PASS: wrapper signatures remain heuristic evidence and are not runtime authorization until exact source-semantic review promotes them.
- PASS: operand matching tolerates Ghidra rendering variants, while semantic promotion still requires callers/decompilation/source agreement.
- PASS: candidate output includes RVAs, role hints, call contexts, callers, wrapper bytes, thin caller decompiles and member decompilation.
- PASS: runner emits `ROLE_SUMMARY.txt` and compact best-member/best-wrapper/best-signature fields.
- PASS: `SemanticVerdict=BOUNDED_FINALISTS` means bounded static candidates only, not runtime authorization.
- PASS: exact five-line PREFLIGHT exists.
- PASS: output requires `SCAN_COMPLETE=YES` before packaging.
- PASS: RESULT is labeled STATIC-AUDITED / NOT LIVE-PROVEN.
- PASS: protected v0.5.5 GOLDEN is not modified.
- PASS: CI route resolves only an official AdvancedFX release asset and requires the same exact AfxHookSource SHA before analysis.
- PASS: CI route uses Ghidra only for static analysis; game/HLAE execution and injection are absent.
- PASS: CI full scan is uploaded as an artifact; compact review evidence only is committed under a source-commit-specific path.
- PASS: CI compact result retains `NO AUTO PROMOTION` / `RuntimeAuthorization=NO` semantics.

Runtime verdict is intentionally impossible from this package.
