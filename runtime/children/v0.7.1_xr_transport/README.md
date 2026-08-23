# CSGOVR XR TRANSPORT v0.7.1 — SYNC PAIR PROOF

Status: **PREPARED / STATIC-AUDITED / NOT YET LIVE-PROVEN**

Original one-click artifact:
`CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF_ONECLICK.zip`

Artifact SHA256:
`DFEA881888C83812A5B9EE3A7049F79D32935240F2C612DE4ABDF4F9A98834A3`

The exact binary ZIP was produced in the integration branch and is identified by the hash above. This GitHub sync records its full state/provenance and file hashes; it does not falsely label the artifact as LIVE-proven.

## Purpose
Minimal transport child on top of the v0.5.5 HLAE stereo contract.

It intentionally does **not** solve head pose, final projection convention, culling, weapon/hands or HUD.

Pipeline:
`HLAE pass-aware D3D9 eye render -> MSAA-safe CPU readback -> atomic shared pair -> x64 D3D11/OpenXR host -> Quest projection layer`

## Package contents
- `00_RUN_XR_TRANSPORT.bat` — one-click entry.
- `RUN_XR_TRANSPORT.ps1` — environment verification, build, launch, HLAE stream setup, transport gate, result collection and cleanup.
- `PROVEN_OPENXR_FOV.ini` — frozen Quest FOV values from the proven diagnostic.
- `src/eye_transport.c` — x86 producer; HLAE pass discrimination, D3D9 device/reset hooks, readback and pair publication.
- `src/injector.c` — x86 DLL injector.
- `src/transport_protocol.h` — fixed-width `pack(4)` shared-memory protocol.
- `src/xr_host.cpp` — x64 D3D11/OpenXR host/consumer.

## Same-HLAE pair proof
v0.7.1 was hardened after cross-branch sync.

A pair is valid only if:
- LEFT and RIGHT use the same `CAfxStreams*`;
- LEFT observes `m_DoRenderViewCount 0 -> 1`;
- RIGHT observes `1 -> 2`;
- mode/pair identity is coherent;
- only the complete pair becomes READY.

One-shot proof tokens:
- producer: `DISTINCT PAIR PROOF`
- host: `PAIR PROOF PASS`

## Dependencies / environment
Expected game:
`C:\Program Files (x86)\Steam\steamapps\common\csgo legacy`

Shared tool root:
`C:\GOG Games\VR Research Tools`

The runner searches Zig, MinHook source, OpenXR headers, exact HLAE 2.191.1 by hash and x64 `openxr_loader.dll`.

Preferred proven loader path:
`C:\GOG Games\VR Research Tools\OpenXR\openxr_loader.dll`

Fallback can retrieve Khronos `OpenXR.Loader 1.0.10.2`.

Frozen transport gate requires Meta/Oculus active runtime manifest `oculus_openxr_64.json`.

## Evidence status
No user LIVE result has been returned yet. Do not label XR transport PROVEN until `CSGOVR_XR_TRANSPORT_RESULT_*.zip` demonstrates the PASS contract in `experiments/NEXT_EXPERIMENT.md`.

## Source SHA256 inside the prepared ZIP
- `src/eye_transport.c` `9EA00805D454A64632EF467BD080E5C89D4928D074E30CC0F155EC51A9621158`
- `src/injector.c` `8765AF9ED6977497541A90DB2102A7398442B71E4DE10467B855FBAA8DA4D9D9`
- `src/transport_protocol.h` `7E0D80744B0F5D78548844F62119B34253DCDC5D0E439E07DFA8B9F24450B15E`
- `src/xr_host.cpp` `CE319792F0A4B2A458EF5B54B2001589F5BC37139FED5F501F6ACE126136E9E1`
- `RUN_XR_TRANSPORT.ps1` `D317B967F70A3DAB4B335EE44192BFC3E5B93CEF3D11459D8BBB75ACEE4FE264`
- `00_RUN_XR_TRANSPORT.bat` `B137289346B4ED2F1B7ED922D91AB82733AE9F0DE13D7AB995F52183ECDF3751`
- `PROVEN_OPENXR_FOV.ini` `66B9561B4C0CAAFA5E77D208408C8FB1688FFFB74C618CAFC6FA94462A34DE9B`
