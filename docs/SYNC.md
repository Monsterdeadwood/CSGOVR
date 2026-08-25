# CSGOVR — STATE SYNC

Snapshot: 2026-08-25 after MASTER 1 PASS, MASTER 2 PASS, and MASTER 3.1 v0.9.2 pre-causal XR-invalid result.

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

## MASTER 3.1 v0.9.2 — INVALID / PRE-CAUSAL XR SESSION
Result: `CSGOVR_MASTER31_EARLY_ORIGIN_RESULT_20260825_071816.zip`.
SHA256: `C4AB4FD0DFA9D8F90D7C47E18588EEC95185F9FEDC242B3175B1B469FFFBF91C`.

The package built successfully, but did not launch CS:GO and did not attempt the causal origin experiment.

Passed before abort:
- frozen target/hash preflight
- exact HLAE package preflight
- x86 DLL/injector and x64 OpenXR host build
- Meta OpenXR manifest selection
- XR instance creation
- D3D11 device creation
- Quest view configuration enumeration
- swapchain-format setup

Blocking raw XR observation:
- `XR SESSION STATE=1`
- `running=0`
- no `XR SESSION RUNNING`
- no diagnostic two-eye projection submission
- producer pairs remained `0/0`

Runner failure message:
`T0: OpenXR session did not reach RUNNING. Activate Quest Link / wake Quest 3.`

Because the run stopped at T0, there is no `EYE_TRANSPORT.log`, no `EARLY_ORIGIN_APPLY`, no HLAE continuous BASE/SHIFT/RESET capture, and no phase BMPs. The generated analyzer FAIL is not a semantic origin result.

Scientific verdict: **INVALID / PRE-CAUSAL INFRASTRUCTURE**, not origin FAIL.

Detailed record: `results/MASTER31_EARLY_ORIGIN_20260825_071816_INVALID_XR_SESSION.md`.

## PREPARED v0.9.3
`CSGOVR_MASTER3_1_EARLY_ORIGIN_v0.9.3_XR_SESSION_RESILIENCE_ONECLICK.zip`
SHA256 `C4846D8E02039FF47F8ACE8BF976FEE2983656AC8BEA57AF1A70B4A2158729FB`.

v0.9.3 changes runner/T0 only:
- best-effort official Meta `OVRService` probe/start;
- start official Meta/Oculus desktop client if installed and not running;
- optional best-effort Quest WAKEUP/MENU through already-authorized ADB only;
- wait up to 180 seconds for `XR SESSION RUNNING`, retrying assist;
- pre-causal failures are explicitly `INVALID_PRE_CAUSAL` and do not run the raster analyzer.

C/C++ causal source is byte-identical to v0.9.2. Static retry verification: 12/12 PASS.

Causal experiment remains unchanged:
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
Run v0.9.3 once and return `CSGOVR_MASTER31_EARLY_ORIGIN_RESULT_*.zip`. Only a valid causal 3.1 PASS may open MASTER 3.2 real HMD pose.
