# MASTER 3.2A HMD ROTATION — v1.0.0 INVALID / BUILD INFRASTRUCTURE

Run: `2026-08-25 09:10` local

Package line: `CSGOVR_MASTER3_2A_HMD_ROTATION_v1.0.0_LIVE`

## Result
The causal HMD-rotation experiment did **not** run.

Preflight passed:
- frozen CS:GO target hash
- exact HLAE 2.191.1
- exact Quest matrix staging/hash
- M3.2A source contract

Build then failed in the x64 OpenXR host:

```text
xr_host.cpp:154:9: error: use of undeclared identifier 'log_line'
```

Root cause: the new `publish_relative_hmd_orientation()` helper called `log_line()` before the logger had been declared in the C++ translation unit.

Scientific status: **INVALID / BUILD INFRASTRUCTURE**, not HMD-rotation FAIL.

No runtime conclusion about HMD orientation, same-pair pose latch, native RenderView angle ownership, rotated IPD, or `ROT_LIVE_ANCHOR_POS` can be drawn from this run.

## Fix prepared
`CSGOVR_MASTER3_2A_HMD_ROTATION_v1.0.1_COMPILE_ORDER_FIX_ONECLICK.zip`

SHA256:
`0FFCE550F64460571BAEEDEC320364CC4DAA652F154A1C5E5D7C20FA503967F3`

Delta is compile-order only:

```cpp
static void log_line(const char *fmt, ...);
```

is declared before `publish_relative_hmd_orientation()`.

Additional cold static audit found zero remaining project-defined use-before-declaration issues in both `xr_host.cpp` and `eye_transport.c`.

The HMD-rotation causal logic is unchanged from v1.0.0.
