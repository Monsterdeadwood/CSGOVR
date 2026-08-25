# MASTER 3.1 v0.9.2 — INVALID / PRE-CAUSAL XR SESSION

Result: `CSGOVR_MASTER31_EARLY_ORIGIN_RESULT_20260825_071816.zip`

Result ZIP SHA256: `C4AB4FD0DFA9D8F90D7C47E18588EEC95185F9FEDC242B3175B1B469FFFBF91C`

## Verdict
**INVALID / PRE-CAUSAL INFRASTRUCTURE**, not an early-origin FAIL.

The run never launched CS:GO and never reached injection, `EARLY_ORIGIN_APPLY`, HLAE phase capture, pair publication, or causal raster analysis.

## What passed
- frozen target/hash preflight
- exact HLAE package preflight
- x86 DLL/injector and x64 XR host build
- Meta OpenXR manifest selection
- XR instance creation
- D3D11 device creation
- Quest view configuration enumeration
- OpenXR swapchain-format setup

## Blocking raw observation
`XR_HOST.log` remained at:
- `XR SESSION STATE=1`
- `running=0`
- no `XR SESSION RUNNING`
- no diagnostic projection submission
- no producer pairs

Runner failure message:
`T0: OpenXR session did not reach RUNNING. Activate Quest Link / wake Quest 3.`

Therefore the missing M3.1 runtime files/BMPs are downstream consequences of the pre-causal T0 abort. The generated analyzer FAIL is not a semantic origin verdict.

## Adaptation
Prepared runner-only retry:
`CSGOVR_MASTER3_1_EARLY_ORIGIN_v0.9.3_XR_SESSION_RESILIENCE_ONECLICK.zip`
SHA256 `C4846D8E02039FF47F8ACE8BF976FEE2983656AC8BEA57AF1A70B4A2158729FB`

v0.9.3 changes only XR/T0 runner resilience:
- best-effort official Meta OVRService start/probe;
- official Meta/Oculus desktop client start if installed and not already running;
- best-effort WAKEUP/MENU through already-authorized ADB only;
- up to 180 s wait for XR SESSION RUNNING with repeated assist;
- pre-causal aborts are explicitly `INVALID_PRE_CAUSAL` and skip the raster analyzer.

C/C++ causal source is byte-identical to v0.9.2. The experiment remains only `+32 Source units` along camera RIGHT at native `client+0x003A5280`, with exact Quest projection and continuous BASE_A -> SHIFT -> BASE_B capture.

## Current gate
MASTER 3.1 remains open. Do not open MASTER 3.2 until a valid causal PASS.