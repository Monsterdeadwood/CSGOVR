# CSGOVR v0.6.1 CUSTOM PROJECTION MATRIX ORACLE — VERIFIED CHILD / DONOR

**DO NOT PROMOTE THIS WHOLE BUILD TO GOLDEN.**

Parent GOLDEN remains:
`CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`.

## Physical package
Library artifact:
`CSGOVR_CUSTOM_PROJECTION_MATRIX_ORACLE_v0.6.1_ONE_CLICK.zip`

- bytes: `21,733`
- SHA256: `DB155F87F4BFF15611B1B5A6EFBAFAE756D789165BF6D1C52B632BE68E95CEC3`

Observed package member identities are recorded in `PACKAGE_SHA256.txt`.

## Source relationship to v0.5.5
`src/injector.c` is byte-identical to the original v0.5.5 GOLDEN injector:

`E6133CEFC23460BA3AD82350803251C604AA24425474B224BEF7FEE01EA5183D`

v0.6.1 behavioral change is concentrated in `src/eye_patch.c`:

- v0.5.5 eye patch SHA256 `3844B2C323F1077A901581A0A82908A174F9D1DF3F4213C5EB68980B7E7D148F`
- v0.6.1 eye patch SHA256 `2CD99D89034DCD16A26A048376F193701A43CCF3C6A805090735545938CE2201`

## Useful donor pieces
v0.6.1 concretely implements the already-proven CViewSetup custom-projection seam:
- custom projection enable byte at `+0x68`
- custom VMatrix at `+0x6C..+0xAB`
- matrix slot convention used by this line:
  - `M00 = matrix[0]`
  - `M02 = matrix[2]`
  - `M11 = matrix[5]`
  - `M12 = matrix[6]`
  - `M22 = matrix[10]`
  - `M23 = matrix[11]`
  - `M32 = matrix[14]`
  - `M33 = matrix[15]`
- custom matrix is applied to the already-local `0x198` eye `CViewSetup` copy before the proven HLAE `DoRenderView` call

## Explicitly do NOT transplant
v0.6.1 also introduced test-only behavior that must not contaminate the next projection child:
- frozen center origin/angles/FOV/aspect/zNear/zFar
- `custom_projection_mode.ini` polling thread
- synthetic `M02/M12 +/-0.10` canary modes
- synthetic `M00/M11 * 0.80` wide modes
- forcing legacy FOV to `140` degrees
- phase-control logging state

Those were oracle mechanics, not the shipping projection architecture.

## Current use
Use `PROJECTION_TRANSPLANT.md` to build the narrow child:

```text
exact v0.5.5 GOLDEN source
+ exact Quest asymmetric matrix only
= projection integration child
```

No new camera ownership or culling mutation belongs in that child.
