# CSGOVR — STATE SYNC

Snapshot: 2026-08-25 after MASTER 1 PASS, MASTER 2 PASS, and MASTER 3.1 v0.9.1 guard-invalid result.

## AUTHORITY / CURRENT PROGRAM
Execution order remains strict:
`MASTER 1 -> MASTER 2 -> MASTER 3 -> MASTER 4`.
Prepared packages are not runtime proof. Raw runtime result > generated status summary > plan/README.

Frozen baseline:
- legacy CS:GO x86, AppID `4465480`, BuildID `22100934`
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- HLAE 2.191.1 exact
- v0.5.5 schedule: pass1 LEFT / pass2 RIGHT / pass3 monitor
- full `CViewSetup = 0x198`
- `-insecure`, local/offline only

## MASTER 1 — PASS
Retain CPU D3D9 complete-pair transport to Meta OpenXR. Do not reopen.

## MASTER 2 — PASS
Retain exact Quest 3 asymmetric projection through `CViewSetup +0x68/+0x6C`. Do not rerun.

## MASTER 3.1 v0.9.1 — INVALID / HLAE VTABLE GUARD ONLY
Result: `CSGOVR_MASTER31_EARLY_ORIGIN_RESULT_20260825_070439.zip`.

The package built and reached runtime injection. Before causal origin work it proved:
- OpenXR host/session diagnostic PASS
- game map load PASS
- `-insecure` injection PASS
- exact Quest matrix runtime path/hash/value PASS
- exact HLAE DoRenderView identity PASS

It then aborted on the native vtable guard:
- client base `0x2D5F0000`
- native target `0x2D995280 = client+0x003A5280`
- `CViewRender.slot6 = 0x2D995280`
- AfxHookSource base `0x5FD80000`
- `CCSViewRender.slot6 = 0x5FE1A160 = AfxHookSource+0x0009A160`

This is expected HLAE behavior, not target corruption. Exact HLAE 2.191.1 source `csgo_CViewRender.cpp` explicitly performs `AfxDetourPtr` on the CCSViewRender RenderView vtable and routes it through `touring_CCSViewRender_RenderView` into `g_AfxStreams.OnRenderView(...)`.

Therefore the old both-native guard is FALSIFIED in the HLAE-loaded runtime context.

Scientific verdict: **INVALID**, because the DLL aborted before `EARLY_ORIGIN_APPLY`, L/R pair publication, or causal raster capture. No statement about origin ownership yet.

Detailed record: `results/MASTER31_EARLY_ORIGIN_20260825_070439_INVALID_HLAE_VTABLE_GUARD.md`.

## PREPARED v0.9.2
`CSGOVR_MASTER3_1_EARLY_ORIGIN_v0.9.2_HLAE_VTABLE_GUARD_FIX_ONECLICK.zip`
SHA256 `7FBA01E24138A56EB6596ED0BA8DADCE692DE12B2FDF85D01EB64286E4B964F8`.

Guard is now strict and HLAE-aware:
- `CViewRender.slot6` MUST equal native `client+0x003A5280`;
- `CCSViewRender.slot6` MUST equal exact `AfxHookSource+0x0009A160`;
- runner still SHA256-gates exact AfxHookSource binary.

Static source/runner consistency verification: 24/24 PASS.

Causal experiment unchanged:
- native early RenderView `client+0x003A5280`;
- local full `CViewSetup 0x198`;
- only `origin +0xB8`;
- deterministic `+32 Source units` camera RIGHT;
- exact Quest matrix always retained;
- continuous `BASE_A -> ORIGIN_SHIFT -> BASE_B` stream;
- pass3 untouched;
- no HMD pose, angles, hands, weapon, culling, A2, or MASTER4.

## CURRENT GATE
**MASTER 3.1 — bounded early native origin ownership probe.**

## NEXT
Run v0.9.2 once and return `CSGOVR_MASTER31_EARLY_ORIGIN_RESULT_*.zip`. Only a valid causal 3.1 PASS may open MASTER 3.2 real HMD pose.
