# Projection transplant map — v0.5.5 GOLDEN + Quest asymmetric projection

## Scope
Current CSGOVR gate allows **projection integration only**.

Parent:
`runtime/golden_v0.5.5/src/eye_patch.c`

Verified oracle donor:
`CSGOVR_CUSTOM_PROJECTION_MATRIX_ORACLE_v0.6.1_ONE_CLICK`

Do not replace the v0.5.5 GOLDEN with v0.6.1 wholesale.

## 1. Preserve v0.5.5 eye ownership unchanged
The parent schedule stays exactly:

```text
HLAE DoRenderView
  pass1 + forceCache -> LEFT local 0x198 CViewSetup copy
  pass2 + forceCache -> RIGHT local 0x198 CViewSetup copy
  pass3              -> untouched monitor
```

Preserve:
- exact HLAE PE guards
- exact `-insecure` gate
- `DoRenderView RVA 0x22340`
- `m_ForceCacheFullSceneState @ this+0x30`
- `m_DoRenderViewCount @ this+0x50`
- `CViewSetup` copy size `0x198`
- existing LEFT/RIGHT origin offset
- unchanged injector

## 2. Use the proven custom-projection fields
Within each already-local LEFT/RIGHT `CViewSetup` copy:

```text
m_bCustomProjMatrix  = +0x68
m_matCustomProjMatrix = +0x6C .. +0xAB
```

Set the enable byte only after a complete valid matrix has been written.

Do not modify the original engine-owned `view` object. Modify only `localView`.

## 3. Source matrix storage convention
The verified v0.6.1 implementation and current retail evidence agree on:

```text
M00 -> matrix[0]
M02 -> matrix[2]
M11 -> matrix[5]
M12 -> matrix[6]
M22 -> matrix[10]
M23 -> matrix[11]
M32 -> matrix[14]
M33 -> matrix[15]
```

For the current RH D3D-style Source projection used by this line:

```text
M32 = -1
M33 = 0
```

Depth terms may continue to use the live parent eye-copy `zNear @ +0xD0` and `zFar @ +0xD4`:

```text
M22 = zFar / (zNear - zFar)
M23 = (zNear * zFar) / (zNear - zFar)
```

## 4. Build asymmetry from the actual eye FOV
For each eye let:

```text
L = tan(angleLeft)
R = tan(angleRight)
D = tan(angleDown)
U = tan(angleUp)
```

Then:

```text
M00 = 2 / (R - L)
M02 = (R + L) / (R - L)
M11 = 2 / (U - D)
M12 = (U + D) / (U - D)
```

This preserves the already-established Source custom-projection sign convention.

Do **not** replace the asymmetric eye FOV by one symmetric `fov` value.

## 5. Current Quest 3 numerical oracle
Current measured FOV:

LEFT:
```text
L=-54 deg
R=+40 deg
U=+44 deg
D=-55 deg
```

RIGHT:
```text
L=-40 deg
R=+54 deg
U=+44 deg
D=-55 deg
```

Derived coefficients from those angles:

LEFT:
```text
M00 ~= 0.902738278
M02 ~= -0.242512644
M11 ~= 0.835478849
M12 ~= -0.193187454
```

RIGHT:
```text
M00 ~= 0.902738278
M02 ~= +0.242512644
M11 ~= 0.835478849
M12 ~= -0.193187454
```

These numbers are a validation oracle. If the integration has live per-eye OpenXR FOV available, derive the matrix from the live angles rather than hardcoding these coefficients.

## 6. Minimal mutation inside the existing v0.5.5 hook
Conceptually, after the parent has copied `view -> localView` and applied its existing LEFT/RIGHT origin offset:

```cpp
float matrix[16] = {};

// derive M00/M02/M11/M12 from the selected eye's exact OpenXR FOV
// derive M22/M23 from localView zNear/zFar
matrix[14] = -1.0f;
matrix[15] = 0.0f;

memcpy(localView + 0x6C, matrix, sizeof(matrix));
localView[0x68] = 1;

g_original(..., localView, ...);
```

The real child should use a typed helper and explicit finite/range checks, but the ownership order above is the invariant.

## 7. Fail closed
Do not enable the custom matrix for an eye when any required input is missing or invalid.

Fail/drop conditions include:
- no valid LEFT/RIGHT FOV for that eye
- non-finite tangent/coefficient
- degenerate `(R-L)` or `(U-D)` denominator
- invalid `zNear` / `zFar`
- matrix fails expected Source shape

A malformed matrix must never silently fall back to a guessed symmetric projection while being labelled successful asymmetric stereo.

## 8. Explicit STOP list from v0.6.1
Do not transplant:
- `freeze_center_camera_once`
- `g_frozenOrigin`
- `g_frozenAngles`
- `g_frozenFov`
- `g_frozenAspect`
- `g_frozenZNear/g_frozenZFar`
- projection INI polling/control thread
- `PROJ_X_POS/NEG`
- `PROJ_Y_POS/NEG`
- `PROJ_X_WIDE/Y_WIDE`
- `PROJ_SHIFT=0.10`
- `PROJ_WIDE_SCALE=0.80`
- legacy FOV forced to 140 degrees

Those are oracle mechanics, not the next integration contract.

## 9. Required telemetry
Record at minimum for LEFT and RIGHT:
- pass number
- source FOV L/R/U/D
- written M00/M02/M11/M12
- zNear/zFar
- custom projection enable byte
- monitor pass untouched count
- HLAE pass order

For regression, retain the parent proof that pass1/pass2 remain two real world streams and pass3 remains monitor.

## 10. Acceptance
PASS requires in-headset:
- correct fused stereo
- correct horizontal asymmetry rather than symmetric toe-in-style appearance
- no new eye inversion / cross-eye condition
- v0.5.5 pass scheduling unchanged
- monitor pass unchanged
- no unrelated camera freeze

If visibility/culling remains wrong after projection PASS, do not alter projection again. Resume the already-preserved culling lineage at `client.dll+0x003A16A6` / `CSimpleWorldView`.
