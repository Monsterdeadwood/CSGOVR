# CSGOVR - canonical one-click workflow pattern

Purpose: preserve the reliable BAT/PowerShell/native packaging method used across CSGOVR experiments so old chat tabs are not required to reconstruct it.

This document describes tooling and validation. It does not promote an UNRUN package to evidence.

## Front door
Use a minimal no-logic BAT:

```bat
@echo off
setlocal
cd /d "%~dp0"
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0RUN_EXPERIMENT.ps1"
```

Keep all complex quoting, downloads, build logic, process control and RESULT packaging in PowerShell.

## Strict PowerShell
Recommended beginning:

```powershell
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version 2.0
[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
```

Avoid reserved automatic variables such as `$PID` for custom state. Historical v0.7.1 explicitly corrected this class of runner bug.

## Exact target identity
Before doing anything intrusive, verify:
- frozen `client.dll` SHA256;
- exact HLAE/AfxHookSource identities when used;
- legacy CS:GO path/build contract;
- `-insecure` launch contract;
- expected x86/x64 PE machine for producer/injector/host;
- correct Meta/Oculus OpenXR runtime for the Quest transport gate.

Target hash mismatch is a hard stop, not a reason to guess offsets.

## Dependency policy
Search known local roots first:
`C:\GOG Games\VR Research Tools` and project bootstrap/dependency caches.

If a dependency must be fetched:
- pin a commit/version;
- cache it under the package `_deps` directory;
- verify the expected file/architecture after extraction;
- record the dependency identity in RESULT/build metadata.

Historical v0.7.1 uses pinned MinHook/OpenXR source or loader fallbacks rather than floating latest dependencies.

## Build separation
Frozen CS:GO is x86 D3D9 while OpenXR host is x64.

Build and validate separately:
- x86 eye/transport producer;
- x86 injector when required;
- x64 D3D11/OpenXR host.

Check PE machine after build. Cross-bitness shared memory structs require explicit fixed-width types and packing; do not trust compiler defaults.

## Runtime gate before science
Verify OpenXR ActiveRuntime and session readiness before interpreting game behavior.

For transport experiments, separate these facts:
- OpenXR session works;
- HLAE/game producer works;
- same-cycle L/R pair identity is valid;
- x86 -> x64 transport works;
- both eye resources are non-black/distinct;
- one projection layer is submitted.

Failure in an earlier transport step does not falsify later projection/culling science because that science was never exercised.

## Pair integrity
Transport packages must prevent `L(N)+R(N+1)` mixing.

Use an explicit pair identity/generation contract. Only a completed coherent pair becomes READY. Reset/device generation changes must invalidate pooled resources and stale pairs.

Present may own a D3D9 device pointer but must not be treated as the eye-identity oracle.

## One mutation/gate
T0 transport must not simultaneously change:
- Quest projection;
- origin/head pose;
- culling;
- viewmodel/weapon;
- HUD.

Freeze transport first, then advance G3 projection.

## RESULT contract
A transport RESULT should contain enough to answer the gate without another run:
- exact target/tool/runtime hashes;
- build logs/identity;
- HLAE pass/pair counters;
- same-pair proof tokens;
- OpenXR session/frame/submit counters;
- non-black/distinct-eye checks;
- reset/stale/mixed-pair counters;
- failure stage if blocked;
- cleanup state.

Large raw captures stay in Library/cold storage with hash/provenance references.

## Tool failure classification
These historical failures are infrastructure, not scientific target falsifications:
- x32dbg foreign breakpoint noise;
- CDB/TTD command argument-type mismatch;
- CDB replay watchdog timeout;
- parser/quoting/stale PID defects;
- wrong AfxInterop protocol assumption;
- missing build flags/defines.

Keep exact static evidence recovered before the failure.

## Origin/camera control lesson
A one-field byte mutation can be mechanically clean while still failing to prove camera ownership. Require:
- validated target hit;
- same object identity into the intended consumer;
- predicted visible/GPU effect;
- exact restore.

A snapshot-only or wrapper-only success is `INFRASTRUCTURE`, not camera proof.

## Cleanup
Use `try/catch/finally` and clean only package-owned state/processes. Preserve logs even on failure. Do not let a packaging exception erase the actual earlier evidence stage.

## Evidence labels
Every package/result should explicitly use one of:
- PREPARED / UNRUN
- STATIC-AUDITED / NOT LIVE-PROVEN
- INFRASTRUCTURE
- MECHANICAL_ONLY
- PROVEN
- FALSIFIED
- SUPERSEDED

A runnable ZIP is not evidence until its required RESULT exists.
