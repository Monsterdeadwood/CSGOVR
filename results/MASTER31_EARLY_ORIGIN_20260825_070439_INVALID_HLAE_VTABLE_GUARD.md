# MASTER 3.1 v0.9.1 — INVALID HLAE VTABLE GUARD

Result: `CSGOVR_MASTER31_EARLY_ORIGIN_RESULT_20260825_070439.zip`

Scientific verdict: **INVALID / infrastructure guard failure**, not origin FAIL.

## Proven in this run
- frozen `client.dll` hash PASS
- exact HLAE 2.191.1 PASS
- x86 DLL / injector / x64 OpenXR host build PASS
- OpenXR session and diagnostic layer PASS
- `de_dust2` reached gameplay load
- injection PASS with exact `-insecure`
- exact Quest matrix runtime path/hash/value proof PASS
- native target address resolved as `client.dll+0x003A5280`

## Blocking observation
Runtime:
- client base `0x2D5F0000`
- native RenderView target `0x2D995280` = `client+0x003A5280`
- `CViewRender.slot6 = 0x2D995280` = native target
- `AfxHookSource base = 0x5FD80000`
- `CCSViewRender.slot6 = 0x5FE1A160` = `AfxHookSource+0x0009A160`

v0.9.1 wrongly required BOTH vtable slot6 values to still equal the native client target after HLAE had already installed its normal hook.

Exact HLAE 2.191.1 source `csgo_CViewRender.cpp` confirms this redirect is expected: it calls `AfxDetourPtr` on the `CCSViewRender` RenderView vtable entry, pointing it at `touring_CCSViewRender_RenderView`, which routes into `g_AfxStreams.OnRenderView(...)`.

Therefore the correct post-HLAE guard is:
1. `CViewRender.slot6 == client+0x003A5280`
2. `CCSViewRender.slot6 == exact AfxHookSource+0x0009A160`

## Consequence
The DLL aborted before `EARLY_ORIGIN_APPLY`, pair publication, or causal raster capture. Missing captures/logs are downstream consequences only. MASTER 3.1 remains open.

## Prepared fix
`CSGOVR_MASTER3_1_EARLY_ORIGIN_v0.9.2_HLAE_VTABLE_GUARD_FIX_ONECLICK.zip`
SHA256 `7FBA01E24138A56EB6596ED0BA8DADCE692DE12B2FDF85D01EB64286E4B964F8`.

Causal variable remains unchanged: local full `CViewSetup 0x198`, only `origin +0xB8`, deterministic `+32 Source units` camera-right, exact Quest projection retained, continuous `BASE_A -> SHIFT -> BASE_B`, no HMD pose/hands/A2/M4.
