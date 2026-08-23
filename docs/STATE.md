# CSGOVR — ACTIVE STATE

Updated by CROSS-BRANCH STATE SYNC: **2026-08-24**.

## LATEST RESULT
There is **no new completed LIVE result** after the canonical 2026-08-23 evidence set.

Newest branch output is a prepared integration candidate:

`CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF`

- artifact SHA256: `DFEA881888C83812A5B9EE3A7049F79D32935240F2C612DE4ABDF4F9A98834A3`
- class: **PREPARED / STATIC-AUDITED / NOT YET LIVE-PROVEN**
- supersedes prepared `v0.7.0_XR_TRANSPORT` before LIVE
- exact one-click package is retained in this repo as base64 under `runtime/children/v0.7.1_xr_transport/`

The previous GitHub-main state treated Quest projection integration as current, but its own RESULT index contained no exact transport/headset PASS. This is recorded as a **state-order conflict**, not as a contradiction in PROVEN evidence:
- remote projection branch: projection child prepared and ready;
- transport branch: final OpenXR submission of the proven HLAE streams is still unproven.

Resolution: **XR TRANSPORT remains CURRENT GATE; projection is next immediately after PASS.**

## TARGET / ENVIRONMENT
- Game: standalone frozen legacy CS:GO x86.
- Steam AppID: `4465480`.
- BuildID: `22100934`.
- Lab path: `C:\Program Files (x86)\Steam\steamapps\common\csgo legacy`.
- Expected `client.dll` SHA256: `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`.
- Safety lab: `-insecure`, local `sv_lan`, Dust II, windowed, `mat_queue_mode 0`.
- Shared tools root: `C:\GOG Games\VR Research Tools`.
- Quest target: Meta Quest 3 via Quest Link.
- Frozen XR gate requires active Meta/Oculus OpenXR provider manifest `oculus_openxr_64.json`.

### Exact HLAE baseline
- HLAE `2.191.1`.
- AdvancedFX commit `b97636852b8eecae09285b5a386192bb285638eb`.
- `HLAE.exe` SHA256 `D7BA05E1E540D82AC485A9B39F13D5C4CCF9FD94F460E0C9ECF069B94C880A59`.
- `AfxHookSource.dll` SHA256 `5B9D13AE9ABFFEEF80851E10861302162A47D1B43D8483B4A8FCEC7EAA19B035`.

## GOLDEN
`CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`

Canonical runtime result:
`CSGOVR_HLAE_EYE_PATCH_RESULT_20260821_015737.zip`

What it proves:
- two real Source world renders exist;
- pass1 = LEFT world;
- pass2 = RIGHT world;
- pass3 = monitor/unmodified;
- full local `CViewSetup` copies are `0x198`;
- historical half-eye translation used by GOLDEN = `1.2598425 Source units`.

It does **not** prove final Quest projection, final eye-origin authority, final same-frame culling, head pose, weapon/hands, HUD, or OpenXR compositor transport.

## PROVEN
### Frozen `CViewSetup`
- full size `0x198`; old `0x14C` full-size assumption is SUPERSEDED.
- `+0x68` `m_bCustomProjMatrix`
- `+0x6C..+0xAB` `m_matCustomProjMatrix`
- `+0xB0` world FOV
- `+0xB4` viewmodel FOV
- `+0xB8` origin
- `+0xC4` angles
- `+0xC8` yaw
- `+0xD0` world zNear
- `+0xD4` world zFar
- `+0xD8` viewmodel zNear
- `+0xDC` viewmodel zFar
- `+0xE0` aspect
- `+0x138/+0x13C/+0x140/+0x144` off-center top/bottom/left/right
- `+0x148.bit0` `m_bOffCenter`
- final byte copied `+0x197`

### HLAE
- `CAfxStreams::DoRenderView = AfxHookSource.dll+0x00022340`.
- `CViewSetup_csgo` copy ctor `= AfxHookSource.dll+0x00014BA0`.
- HLAE object: `this+0x30 = m_ForceCacheFullSceneState`, `this+0x50 = m_DoRenderViewCount`, `this+0x54 = m_ShowRenderViewCount`.
- runtime pass sequence observed repeatedly: `count 1 / forceCache 1 -> count 2 / forceCache 1 -> count 3 / forceCache 0`.
- exact source ordering in `CaptureStreamToBuffer`: `skyView copy -> SKY GetMatricesForView -> NORMAL GetMatricesForView -> OnRenderBegin -> DoRenderView -> QueueCapture -> OnRenderEnd`.
- therefore the current `DoRenderView` hook regains control after the actual stream render and before HLAE continues to its capture queue.

### Projection / Quest 3
OpenXR diagnostic: `CSGOVR_OPENXR_FOV_DIAGNOSTIC_20260821_150332.zip`

- runtime: Meta/Oculus OpenXR 1.205.0 in recorded diagnostic
- 2 views
- `viewStateFlags=0xF`
- recommended eye size `1504x1616`
- max `3008x3232`
- minimum D3D feature `0xB000`
- created D3D feature `0xB100`
- passive eye separation about `64.780414 mm`

FOV:
- LEFT: L=-54°, R=+40°, U=+44°, D=-55°
- RIGHT: L=-40°, R=+54°, U=+44°, D=-55°

Projection core:
- LEFT `M00=0.9027383310563113`, `M02=-0.24251263135515727`, `M11=0.8354788620481540`, `M12=-0.19318743951911962`
- RIGHT `M00=0.9027383310563113`, `M02=+0.24251263135515727`, `M11=0.8354788620481540`, `M12=-0.19318743951911962`

Causal projection Jacobian established that M00/M02/M11/M12 affect expected scale/center axes in frozen retail.

### D3D9 capture primitive
Prior CSGOVR runtime source/result established:
- passive D3D9 `Present` hook can latch the real `IDirect3DDevice9*`;
- CPU snapshot/readback executes in frozen retail;
- MSAA-safe form is `source RT -> non-MSAA DEFAULT RT via StretchRect -> SYSTEMMEM -> GetRenderTargetData -> LockRect`.

For v0.7.x, `Present` is used only for device ownership, **not** as an eye oracle. Eye identity comes from HLAE pass scheduling.

### Culling / frustum
- `GeneratePerspectiveFrustum = client.dll+0x0086E9C0`
- lane writes begin `+0x0086EB3C`, then `+0x0086EB46/+0x0086EB51/+0x0086EB59`
- concrete pre-eye reader `client.dll+0x003A16A6..+0x003A16DC`
- second retained reader `client.dll+0x003A01E4`
- `CSimpleWorldView` vtable `client+0x00BF4434`, Draw `client+0x003B0870`
- lazy hydration machinery around `+0x003B0067..+0x003B023D`
- per-slot source arrays: slot index `client+0x05248ED4`, world-render-list source `+0x0524A2F4`, world-list-info source `+0x0524A2F8`, renderables-list source `+0x0524A2FC`, stride `0x13F0`
- first `0x40` bytes of helper arg2 are temporarily transposed in-place then restored.

## FALSIFIED / CLOSED
- `0x14C` as full `CViewSetup`.
- `client.dll+0x0525BA60` as a cache reused **inside both** LEFT and RIGHT marker regions.
- persistent helper arg2 output in `+0x40..+0x1FF`.
- old normalized off-center identity `[-1,+1]`; Source semantics are `0..1`, symmetric `[0,1]`.
- treating origin/yaw byte-integrity controls as camera ownership proof.
- late `DoRenderView` eye-origin nudge as authoritative final eye translation point in the tested result.
- sequential screenshots as quantitative same-frame disparity/culling oracle.
- broad OpenXR/FOV rediscovery while the proven baseline remains valid.
- forced D3D9Ex as first transport strategy; demoted until CPU P0 is proven and measured as bottleneck.
- HLAE preview as first transport scheduler replacement.

## CURRENT FINALISTS
1. **P0 / current:** HLAE pass-aware completed RT -> MSAA-safe CPU readback -> 2-slot shared pair ring -> x64 D3D11/OpenXR host.
2. **Queued after transport PASS:** exact v0.5.5 eye path + known Quest asymmetric custom matrix only. Existing repo projection child/source verifier is retained and not duplicated.
3. **Only if a visible culling blocker survives projection:** `+0x003A16A6` + CSimpleWorldView lazy-hydration lineage.

Forced D3D9Ex/shared-texture transport is demoted until CPU P0 is proven and measured as bottleneck.

## CURRENT TRANSPORT CANDIDATE — v0.7.1
`runtime/children/v0.7.1_xr_transport/`

Design:
- T0 before game: x64 host must reach a running Quest OpenXR session and submit a two-view diagnostic layer.
- game starts only after T0 software gate.
- x86 DLL preserves HLAE v0.5.5 pass scheduling.
- `Present` vtable slot 17 latches device pointer only.
- `Reset` vtable slot 16 increments generation and invalidates pooled D3D9 resources/pairs.
- capture occurs after original HLAE `DoRenderView` for LEFT/RIGHT.
- ring is packed with `#pragma pack(4)` for x86/x64 ABI stability.
- 2 pair slots; mapping is kept around ~42 MiB rather than a much larger x86 VA reservation.
- LEFT opens a pair; RIGHT publishes it atomically.
- mode/provenance mismatch discards the pair.
- v0.7.1 additionally requires the same HLAE object and expected `m_DoRenderViewCount` transition: LEFT `0 -> 1`, RIGHT `1 -> 2`.
- producer logs one-shot `DISTINCT PAIR PROOF`; host logs one-shot `PAIR PROOF PASS`, independent of throttled periodic logs.
- host prefers sRGB color swapchain formats, creates D3D11 with BGRA support, and owns all OpenXR calls.
- one exact same-pair LEFT/RIGHT BMP sample is harvested for later analysis.
- no HMD pose is injected into CS:GO in this transport-only gate; host uses a stable world-lock anchor metadata policy for honest transport proof.

Known build hardening:
- stale control file removed before control thread starts;
- PowerShell avoids reserved `$PID`;
- Meta/Oculus runtime provider checked before game launch;
- known `XR_ERROR_FORM_FACTOR_UNAVAILABLE` infrastructure condition is handled as Quest-Link availability rather than renderer evidence;
- OpenXR API contract uses 1.0, matching the successful prior diagnostic path;
- loader lookup reuses `C:\GOG Games\VR Research Tools\OpenXR\openxr_loader.dll` and known fallback;
- CPU hash ignores undefined X/alpha byte for `X8R8G8B8` and gates on RGB content;
- cleanup closes test CS:GO before stopping host.

## CONCRETE TARGETS
### Core
- frozen `RenderView = client.dll+0x003A5280`
- HLAE `DoRenderView = AfxHookSource.dll+0x00022340`
- HLAE copy ctor `+0x00014BA0`
- `CViewSetup` custom projection `+0x68/+0x6C..+0xAB`

### Retained camera candidates — NOT ownership proof
- origin.x writer/mutation seam `client.dll+0x0038D123`
- yaw seam `client.dll+0x0038D15B`
- yaw global commit `client.dll+0x0038D1DE`
- `g_vecRenderAngles.y` candidate `client.dll+0x05245AB8`
- persistent/global CView candidate `client.dll+0x052632CC`
- origin global candidate `client.dll+0x05263384`

### Culling
- `client.dll+0x0086E9C0`
- `client.dll+0x003A16A6`
- `client.dll+0x003A01E4`
- `client.dll+0x003B0870`
- `client.dll+0x0524A2F4/+0x0524A2F8/+0x0524A2FC`, stride `0x13F0`

## CURRENT GATE
**XR TRANSPORT**

Question: can the already-proven HLAE LEFT/RIGHT world passes be paired from one HLAE outer-pass contract and submitted as a live two-view OpenXR projection layer in Quest 3?

PASS:
- T0 OpenXR diagnostic layer runs;
- a same-HLAE pair crosses x86 shared memory into x64 host;
- LEFT/RIGHT are both non-black and genuinely different in RGB;
- game projection layer is submitted;
- no freeze >2 s;
- v0.5.5 scheduling/monitor baseline remains intact.

## NEXT
Run only `CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF` and return generated `CSGOVR_XR_TRANSPORT_RESULT_*.zip` plus headset observation.

On PASS:
1. freeze transport;
2. STOP transport research;
3. advance to prepared Quest projection integration (`P00..P07` convention batch only if needed);
4. do not re-probe FOV.

On FAIL:
- use exact logged failed stage (`T0 XR`, HLAE pair, CPU capture, shared memory, x64 upload, XR game layer);
- ADAPT only that stage;
- do not reopen broad Source/OpenXR archaeology.

## METHOD ALREADY USED
- source/SDK/donor corpus harvest;
- exact frozen binary static proof;
- HLAE source-order archaeology;
- HLAE multipass runtime GOLDEN;
- D3D9 Present/device latch;
- CPU readback + MSAA resolve;
- custom projection/off-center causal canaries and Jacobian;
- Quest OpenXR FOV diagnostic;
- TTD frustum/cache replay;
- TTD pre-eye reader recovery;
- x32dbg/origin-yaw byte-integrity infrastructure controls;
- CSimpleWorldView class funnel and lazy hydration mapping;
- OBS / FEAR-VR / CallOfDuty4-VR / capture-engine donor comparison for transport architecture;
- exact AdvancedFX D3D9Ex history review.

## UNRESOLVED QUESTIONS
1. End-to-end XR transport PASS for frozen CS:GO/HLAE.
2. Exact final Quest projection convention in-headset after transport.
3. Authoritative earlier eye-origin/angle seam if translation still does not reach final raster.
4. Same-simulation-frame culling policy with real Quest frusta and eye translation.
5. Exact semantic ownership of `ClientWorldListInfo_t` fields if culling reverse is still required.
6. HMD center pose ownership and timing.
7. First-person viewmodel/weapon/hands transform owner.
8. Controller/aim separation and final muzzle/ballistics policy.
9. HUD/UI composition policy.
10. Final cadence, recenter, save/load/menu/cutscene and long-gameplay regression.
