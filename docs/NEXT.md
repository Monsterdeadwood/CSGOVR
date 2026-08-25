# CSGOVR — NEXT

Snapshot: 2026-08-25 after MASTER 3.1 v0.9.2 pre-causal XR-invalid run `CSGOVR_MASTER31_EARLY_ORIGIN_RESULT_20260825_071816.zip`.

## CURRENT GATE
**MASTER 3.1 — one bounded early native origin ownership probe.**

MASTER 1 = PASS.
MASTER 2 = PASS.
Do not repeat transport or Quest projection.
Do not open MASTER 3.2 until 3.1 runtime causal PASS.
A2 remains parked until MASTER4 PERF numerically FAILS.

## INVALID v0.9.2 RESULT
v0.9.2 built successfully, but the run stopped before CS:GO launch and before any causal origin work.

Passed before abort:
- frozen target/hash preflight
- exact HLAE package preflight
- x86 DLL/injector + x64 OpenXR host build
- Meta OpenXR manifest selection
- XR instance and D3D11 device creation
- Quest view configuration and swapchain-format setup

Blocking raw observation:
- `XR SESSION STATE=1`
- `running=0`
- no `XR SESSION RUNNING`
- no diagnostic projection submission
- no game launch
- no injection / `EARLY_ORIGIN_APPLY`
- no HLAE phase BMPs

Scientific verdict: **INVALID / PRE-CAUSAL XR AVAILABILITY**, not origin FAIL. The analyzer's missing-runtime failures are downstream noise because the causal section was never attempted.

Detailed record: `results/MASTER31_EARLY_ORIGIN_20260825_071816_INVALID_XR_SESSION.md`.

## NEXT ONE STEP
Run `CSGOVR_MASTER3_1_EARLY_ORIGIN_v0.9.3_XR_SESSION_RESILIENCE_ONECLICK.zip`.
SHA256 `C4846D8E02039FF47F8ACE8BF976FEE2983656AC8BEA57AF1A70B4A2158729FB`.

v0.9.3 is runner/T0 resilience only:
1. probe/start official Meta `OVRService` where available;
2. start official Meta/Oculus desktop client if installed and not already running;
3. optionally send WAKEUP/MENU through already-authorized ADB only;
4. wait up to 180 s for `XR SESSION RUNNING` with automatic assist retries;
5. if T0 still fails, report `INVALID_PRE_CAUSAL` and skip the M3.1 raster analyzer.

The C/C++ causal source is byte-identical to v0.9.2.

## CAUSAL CONTRACT UNCHANGED
Only one variable changes:
`CViewSetup.origin +0xB8` at native `client.dll+0x003A5280` by `+32 Source units` along camera RIGHT.

Retain:
- full local `CViewSetup 0x198` copy;
- caller-owned view untouched;
- exact Quest matrix continuously enabled;
- M1 same-HLAE L/R pair transport;
- one continuous `BASE_A -> ORIGIN_SHIFT -> BASE_B` live stream;
- pass3 untouched;
- `-insecure`, local/offline.

Excluded:
- HMD pose / xrLocateViews camera application;
- angle mutation;
- hands / weapon / muzzle;
- culling;
- A2 GPU sharing;
- MASTER 4.

## PASS
Shift is logged at the native early RenderView for both eyes and the final same-eye raster moves in the preregistered direction, with BASE reset returning coherently.

## FAIL
A valid instrument proves the early native origin mutation was applied but the final raster does not respond in the expected way.

## AFTER PASS
Open MASTER 3.2 and feed real Quest HMD pose into the winning seam. No broad reverse.
