# CSGOVR XR TRANSPORT v0.7.1 — SYNC PAIR PROOF

**Evidence class:** PREPARED / STATIC-AUDITED / NOT YET LIVE-PROVEN.

Artifact: `CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF_ONECLICK.zip`  
SHA256: `DFEA881888C83812A5B9EE3A7049F79D32935240F2C612DE4ABDF4F9A98834A3`

This child was produced after transport-focused cross-branch review. It supersedes prepared `v0.7.0_XR_TRANSPORT` before any LIVE run. It does **not** supersede the v0.5.5 stereo GOLDEN.

The latest remote reconciliation selects pinned HLAE **GPU interop** as T0 primary. Therefore v0.7.1 is retained as **FINALIST B / prepared CPU compatibility fallback**, not silently promoted over the remote GPU finalist.

## Purpose
Alternative transport path for the same missing edge:

`HLAE LEFT/RIGHT completed D3D9 render -> MSAA-safe CPU readback -> atomic shared pair -> x64 D3D11/OpenXR -> Quest 3 projection layer`

No head pose, weapon, HUD, culling or new projection science is introduced in this transport gate.

## Exact package inventory
- `00_RUN_XR_TRANSPORT.bat` — one-click entry
- `RUN_XR_TRANSPORT.ps1` — environment/build/launch/result/cleanup orchestrator
- `PROVEN_OPENXR_FOV.ini` — frozen Quest FOV input from the already-PROVEN XR diagnostic
- `src/eye_transport.c` — x86 HLAE/D3D9 producer
- `src/injector.c` — x86 injector
- `src/transport_protocol.h` — fixed-width x86/x64 shared-memory ABI
- `src/xr_host.cpp` — x64 D3D11/OpenXR host

Source SHA256 inside the prepared ZIP:
- `src/eye_transport.c` `9EA00805D454A64632EF467BD080E5C89D4928D074E30CC0F155EC51A9621158`
- `src/injector.c` `8765AF9ED6977497541A90DB2102A7398442B71E4DE10467B855FBAA8DA4D9D9`
- `src/transport_protocol.h` `7E0D80744B0F5D78548844F62119B34253DCDC5D0E439E07DFA8B9F24450B15E`
- `src/xr_host.cpp` `CE319792F0A4B2A458EF5B54B2001589F5BC37139FED5F501F6ACE126136E9E1`
- `RUN_XR_TRANSPORT.ps1` `D317B967F70A3DAB4B335EE44192BFC3E5B93CEF3D11459D8BBB75ACEE4FE264`
- `00_RUN_XR_TRANSPORT.bat` `B137289346B4ED2F1B7ED922D91AB82733AE9F0DE13D7AB995F52183ECDF3751`
- `PROVEN_OPENXR_FOV.ini` `66B9561B4C0CAAFA5E77D208408C8FB1688FFFB74C618CAFC6FA94462A34DE9B`

## Proven primitives reused
Already-established inputs:
- HLAE pass1 LEFT / pass2 RIGHT / pass3 monitor
- full `CViewSetup=0x198`
- passive D3D9 Present hook can latch the real `IDirect3DDevice9*`
- CPU D3D9 readback executes in frozen retail
- MSAA-safe readback pattern: `source RT -> non-MSAA DEFAULT RT via StretchRect -> SYSTEMMEM -> GetRenderTargetData -> LockRect`
- Quest/OpenXR view/FOV input already exists

`Present` is used only for device ownership, **not** as an eye oracle.

## v0.7.1 same-HLAE pair contract
A pair is accepted only when:
- LEFT and RIGHT use the same `CAfxStreams*`
- LEFT observes HLAE `m_DoRenderViewCount 0 -> 1`
- RIGHT observes `1 -> 2`
- mode/pair identity is coherent
- only the complete pair becomes READY

One-shot proof tokens:
- producer: `DISTINCT PAIR PROOF`
- host: `PAIR PROOF PASS`

This specifically addresses the old measurement weakness where sequential captures could come from different simulation epochs.

## Runtime hardening
- T0 x64 host/session diagnostic before CS:GO launch
- Present vtable slot 17 only latches D3D9 device
- Reset vtable slot 16 bumps generation and invalidates pooled resources/pairs
- `#pragma pack(4)` cross-bitness ABI
- two shared pair slots to reduce x86 virtual-address pressure
- mode/provenance mismatch discards a pair
- RGB-only content hashing, avoiding undefined X/alpha byte in X8R8G8B8
- sRGB-preferred XR color formats, D3D11 BGRA support
- stale control file removed before control thread
- PowerShell avoids reserved `$PID`
- Meta/Oculus active-runtime gate; known `XR_ERROR_FORM_FACTOR_UNAVAILABLE` treated as Quest-Link availability, not renderer science
- OpenXR API 1.0 to match the successful prior diagnostic contract
- preferred loader path `C:\GOG Games\VR Research Tools\OpenXR\openxr_loader.dll`, with known fallback
- test CS:GO closed before host cleanup
- one exact same-pair LEFT/RIGHT BMP harvested

## PASS contract if this fallback is activated
Transport is PROVEN only after a returned `CSGOVR_XR_TRANSPORT_RESULT_*.zip` shows:
- T0 OpenXR diagnostic path green
- structural same-HLAE L/R pair crosses x86 -> x64
- both eyes non-black and RGB-distinct
- game projection layer submitted
- no freeze >2 s
- v0.5.5 scheduling/monitor baseline intact

Until then, this remains a prepared fallback candidate.
