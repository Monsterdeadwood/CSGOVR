# CSGOVR v0.5.5 — BUILD REPRODUCTION CONTRACT

Recovered from the original `ONE_CLICK.ps1` inside the byte-verified v0.5.5 package.

## Toolchain
- Zig `0.16.0`
- target: `x86-windows-gnu`
- language: `gnu11`
- optimization: `-O2`
- MinHook source tree required

The original script stages MinHook with its repository-relative layout preserved:
- `include/MinHook.h`
- `src/buffer.c`
- `src/hook.c`
- `src/trampoline.c`
- `src/hde/hde32.c`

It also copies `MinHook.h` beside `eye_patch.c` because the patch source includes it directly.

## Patch DLL
Output:
`csgovr_hlae_eye_patch_v055.dll`

Equivalent compile shape:

```text
zig cc -target x86-windows-gnu -std=gnu11 -O2 -shared \
  eye_patch.c \
  minhook/src/buffer.c \
  minhook/src/hook.c \
  minhook/src/trampoline.c \
  minhook/src/hde/hde32.c \
  -lkernel32 -lshell32 -lm \
  -o csgovr_hlae_eye_patch_v055.dll
```

Canonical successful runtime RESULT recorded patch binary SHA256:
`11F800361DB2416F58A2014D0FE57861C572B30FB72521F2CD41020A477CB578`

## Injector
Output:
`csgovr_hlae_eye_injector_v055.exe`

Equivalent compile shape:

```text
zig cc -target x86-windows-gnu -std=gnu11 -O2 \
  injector.c \
  -lkernel32 -lshell32 \
  -o csgovr_hlae_eye_injector_v055.exe
```

Canonical successful runtime RESULT recorded injector SHA256:
`C987F156BDEF706A98282FC8BA0FF943CF019E8E6A7D7AAADD9BFC06B68811F0`

## Exact target guards in source
The patch refuses the audited seam unless:
- `AfxHookSource.dll` timestamp = `0x6A58EC87`
- image size = `0x0023F000`
- `CAfxStreams::DoRenderView` RVA = `0x00022340`
- expected machine-byte guards match
- target process command line contains exact `-insecure`

The injector independently refuses injection if the remote target command line cannot be read or does not contain exact `-insecure`.

## Runtime ABI contract
- `CViewSetup_csgo` full copy size = `0x198`
- origin = `+0xB8`
- angles = `+0xC4`
- force-cache field = HLAE stream object `+0x30`
- render-view count = HLAE stream object `+0x50`
- half-eye separation = `1.2598425` Source units
- pass1 = LEFT local copy
- pass2 = RIGHT local copy
- pass3 = untouched monitor

This build contract is frozen with the v0.5.5 stereo GOLDEN. New projection work should extend the proven eye-local `CViewSetup` copies without altering the established pass schedule unless that schedule is explicitly the experimental variable.
