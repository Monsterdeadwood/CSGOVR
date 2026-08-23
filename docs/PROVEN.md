# CSGOVR — PROVEN

Only directly established facts belong here. Prepared builds are documented in `docs/STATE.md` / `research/RESULT_INDEX.md`, not promoted to PROVEN before LIVE.

## Frozen identity
- standalone legacy CS:GO x86, AppID `4465480`, BuildID `22100934`
- expected `client.dll` SHA256: `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- frozen retail `RenderView = client.dll+0x003A5280`

## HLAE / stereo GOLDEN
GOLDEN: `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`

Canonical result: `CSGOVR_HLAE_EYE_PATCH_RESULT_20260821_015737.zip`

- exact HLAE `2.191.1`
- AdvancedFX commit `b97636852b8eecae09285b5a386192bb285638eb`
- `HLAE.exe` SHA256 `D7BA05E1E540D82AC485A9B39F13D5C4CCF9FD94F460E0C9ECF069B94C880A59`
- `AfxHookSource.dll` SHA256 `5B9D13AE9ABFFEEF80851E10861302162A47D1B43D8483B4A8FCEC7EAA19B035`
- `CAfxStreams::DoRenderView = AfxHookSource.dll+0x00022340`
- `CViewSetup_csgo` copy ctor `= +0x00014BA0`
- pass1 LEFT world, pass2 RIGHT world, pass3 monitor
- exact full `CViewSetup` copy size `0x198`

Observed HLAE object fields:
- `this+0x30 = m_ForceCacheFullSceneState`
- `this+0x50 = m_DoRenderViewCount`
- `this+0x54 = m_ShowRenderViewCount`

Observed repeating runtime pass sequence:
`count1/forceCache1 -> count2/forceCache1 -> count3/forceCache0`.

Exact source order:
`SKY GetMatricesForView -> NORMAL GetMatricesForView -> OnRenderBegin -> DoRenderView -> QueueCapture -> OnRenderEnd`.

## `CViewSetup` layout
- `+0x68` custom projection enable
- `+0x6C..+0xAB` custom projection matrix
- `+0xB0` world FOV
- `+0xB4` viewmodel FOV
- `+0xB8` origin
- `+0xC4` angles
- `+0xC8` yaw
- `+0xD0/+0xD4` world near/far
- `+0xD8/+0xDC` viewmodel near/far
- `+0xE0` aspect
- `+0x138/+0x13C/+0x140/+0x144` off-center top/bottom/left/right
- `+0x148.bit0` off-center enable
- object end `+0x197`, size `0x198`

Exact frozen copy-like function closing the custom projection layout: VA `0x10260F40` in the retained static analysis.

## Projection semantics
Results include:
- `CSGOVR_PROJECTION_TRUTH_RESULT_20260821_035411`
- `CSGOVR_CUSTOM_PROJECTION_MATRIX_RESULT_20260821_042218.zip`

Causally measured:
- M02 horizontal center shift
- M12 vertical center shift
- M00 horizontal scale
- M11 vertical scale
- off-center path is live

Normalized Source off-center semantics use `0..1`; symmetric identity is `[0,1]`.

## Quest 3 / OpenXR
Result: `CSGOVR_OPENXR_FOV_DIAGNOSTIC_20260821_150332.zip`

- 2 views, `viewStateFlags=0xF`
- recommended eye size `1504x1616`
- max `3008x3232`
- passive eye separation `~64.780414 mm`
- LEFT L=-54°, R=+40°, U=+44°, D=-55°
- RIGHT L=-40°, R=+54°, U=+44°, D=-55°

Derived core:
- LEFT M00 `0.9027383310563113`
- LEFT M02 `-0.24251263135515727`
- LEFT M11 `0.8354788620481540`
- LEFT M12 `-0.19318743951911962`
- RIGHT M00 `0.9027383310563113`
- RIGHT M02 `+0.24251263135515727`
- RIGHT M11 `0.8354788620481540`
- RIGHT M12 `-0.19318743951911962`

Do not re-probe these values merely to obtain the same numbers.

## D3D9 readback primitive
Prior CSGOVR runtime experiments establish that:
- a passive `Present` vtable hook can latch the real `IDirect3DDevice9*`;
- CPU snapshots execute against frozen retail;
- MSAA-safe readback uses a non-MSAA resolve target before `GetRenderTargetData`.

This proves the primitive, **not** the v0.7.1 end-to-end transport result.

## Visual stereo / measurement truth
Retained projection captures support that GOLDEN LEFT and RIGHT are genuinely different world images and ZERO control largely collapses disparity.

However, sequential same-eye captures changed even more than L/R because simulation/time drift was large. Therefore same-frame identity is mandatory for quantitative culling/disparity claims.

## Frustum / culling
- `GeneratePerspectiveFrustum = client.dll+0x0086E9C0`
- lane writers `+0x0086EB3C/+0x0086EB46/+0x0086EB51/+0x0086EB59`
- concrete pre-eye reader/dataflow `client.dll+0x003A16A6..+0x003A16DC`
- second retained reader `client.dll+0x003A01E4`
- recovered 3A16A6 packet includes hit, call stack, bounded dataflow and store candidates
- first `0x40` of helper arg2 is temporarily transposed in-place and restored
- `CSimpleWorldView` lineage is PROVEN: vtable `client+0x00BF4434`, Draw `client+0x003B0870`
- lazy hydration writers/readers are concrete: `+0x003B0067..+0x003B023D`
- per-slot arrays: `+0x0524A2F4/+0x0524A2F8/+0x0524A2FC`, stride `0x13F0`

Final visibility ownership/field semantics remain OPEN.

## Important non-PROVEN seams
These are retained infrastructure/candidate targets only:
- `client.dll+0x0038D123`
- `CViewSetup.origin.x +0xB8`
- `client.dll+0x052632CC`
- `client.dll+0x05263384`
- `client.dll+0x0038D15B`
- `client.dll+0x0038D1DE`
- `client.dll+0x05245AB8`

Their 2026-08-23 controls do not prove final camera ownership.
