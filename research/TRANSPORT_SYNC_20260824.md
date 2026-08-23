# CSGOVR — XR TRANSPORT BRANCH SYNC — 2026-08-24

This file preserves the transport branch state without overwriting the parallel projection sync. Read together with `research/BRANCH_SYNC_20260824.md`.

## REMOTE-FIRST / CROSS-BRANCH SYNC
Initial remote inspected by this branch:
- repo `Monsterdeadwood/CSGOVR`
- only remote branch initially visible: `main`
- initial HEAD `20f76e6bbd849b5aff5f25d00ae63505ef1b96c0`

During synchronization another branch advanced `main` to:
- `c41a20d1f90ec24947fddb6dd648c549e2b12352`
- commit message `Record urgent CSGOVR branch state sync`
- 10 commits ahead of the original base

The parallel sync added/updated README, STATE, SYNC, PROVEN, FALSIFIED, GATES, NEXT, EXPERIMENTS, RESULT_INDEX and `research/BRANCH_SYNC_20260824.md`.

It was imported rather than overwritten.

### Gate-order conflict found
Parallel main labels **G3 Quest projection integration** current, but its own synchronized STATE explicitly says:
- no accepted G3 runtime result yet
- final OpenXR swapchain/submission status for CSGOVR is not proven
- if headset transport is absent/black, transport is the concrete blocker

This transport branch independently reached the same missing edge and prepared `v0.7.1_SYNC_PAIR_PROOF` to test it.

No accepted result in either evidence path proves final HLAE -> Quest live transport. Therefore this is a **state-order conflict, not contradictory PROVEN evidence**.

Reconciled execution order:
1. prove XR TRANSPORT using v0.7.1
2. freeze transport on PASS
3. run the already-committed byte-locked Quest projection child

No projection evidence or files are discarded.

## LATEST RESULT
No new completed LIVE result was produced in this transport branch.

Latest output:
`CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF`

- artifact SHA256 `DFEA881888C83812A5B9EE3A7049F79D32935240F2C612DE4ABDF4F9A98834A3`
- PREPARED / STATIC-AUDITED / NOT LIVE-PROVEN
- supersedes prepared v0.7.0 before LIVE

## PROVEN IMPORTED / PRESERVED
### Frozen target
- AppID `4465480`
- BuildID `22100934`
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- `RenderView = client.dll+0x003A5280`

### HLAE GOLDEN
- `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`
- HLAE 2.191.1
- AdvancedFX source commit `b97636852b8eecae09285b5a386192bb285638eb`
- `HLAE.exe` SHA256 `D7BA05E1E540D82AC485A9B39F13D5C4CCF9FD94F460E0C9ECF069B94C880A59`
- `AfxHookSource.dll` SHA256 `5B9D13AE9ABFFEEF80851E10861302162A47D1B43D8483B4A8FCEC7EAA19B035`
- `CAfxStreams::DoRenderView = +0x00022340`
- `CViewSetup_csgo` copy ctor `+0x00014BA0`
- full `CViewSetup=0x198`
- pass1 LEFT, pass2 RIGHT, pass3 monitor
- HLAE `this+0x30 m_ForceCacheFullSceneState`, `+0x50 m_DoRenderViewCount`, `+0x54 m_ShowRenderViewCount`
- observed count pattern `1 -> 2 -> 3`

### Exact HLAE ordering
`skyView copy -> SKY GetMatricesForView -> NORMAL GetMatricesForView -> OnRenderBegin -> DoRenderView -> QueueCapture -> OnRenderEnd`

Transport consequence:
the existing HLAE DoRenderView hook regains control after a completed eye render and before HLAE continues to QueueCapture.

### D3D9 primitive
Historical CSGOVR code/results already established:
- passive Present vtable hook can latch `IDirect3DDevice9*`
- `GetRenderTargetData` CPU readback executes on frozen retail
- MSAA-safe path resolves to non-MSAA DEFAULT target with `StretchRect` before SYSTEMMEM readback

Present is NOT an eye oracle.

## PROJECTION / XR FACTS RETAINED
Quest diagnostic:
`CSGOVR_OPENXR_FOV_DIAGNOSTIC_20260821_150332.zip`

- two valid views, `viewStateFlags=0xF`
- recommended `1504x1616`, max `3008x3232`
- passive eye separation ~`64.780414 mm`
- LEFT L=-54°, R=+40°, U=+44°, D=-55°
- RIGHT L=-40°, R=+54°, U=+44°, D=-55°
- LEFT `M00=0.9027383310563113`, `M02=-0.24251263135515727`, `M11=0.8354788620481540`, `M12=-0.19318743951911962`
- RIGHT `M00=0.9027383310563113`, `M02=+0.24251263135515727`, `M11=0.8354788620481540`, `M12=-0.19318743951911962`

Projection path already PROVEN:
- `CViewSetup+0x68` custom matrix enable
- `+0x6C..+0xAB` matrix
- off-center `+0x138/+0x13C/+0x140/+0x144`, `+0x148.bit0`
- M00/M02/M11/M12 causal semantics

Parallel projection sync additionally retains:
- Source/D3D depth convention
- `c4..c7 MODELVIEWPROJ`, `c8..c11 VIEWPROJ`
- `c17` retained branch constant
- bounded `c58..c60` model candidate family
- `WORLD_X_PAIR` evidence that origin mutation can reach c2/pixels/draw membership in the correct causal path
- byte-locked projection child + verifier + CI already committed

## FALSIFIED / STOPPED
- `0x14C` as full CViewSetup
- `client+0x0525BA60` reused inside BOTH eye marker brackets
- persistent helper arg2 output `+0x40..+0x1FF`
- old parser FALSIFIED for first-0x40 transpose
- sequential screenshot measurement as same-frame disparity/culling oracle
- origin/yaw byte-integrity windows promoted to camera ownership
- broad OpenXR FOV rediscovery
- Present used to infer eye identity/frame boundaries
- forced D3D9Ex as first transport implementation
- HLAE preview as first scheduler replacement
- v0.7.0 transport candidate, superseded before LIVE

### Why D3D9Ex is demoted
Exact AdvancedFX history was inspected. A historical forced-D3D9Ex experiment is explicitly described as a dead end; practical issues included MSAA loss and D3D9Ex/D3D11 depth-sharing limitations. This is architecture/history evidence, not a frozen-game causal result. Revisit only if CPU P0 works and profiling proves it is the blocker.

## CURRENT FINALISTS
1. **Transport P0:** HLAE pass-aware completed RT -> MSAA-safe CPU readback -> atomic 2-slot shared pair -> x64 D3D11/OpenXR.
2. **Immediately after transport PASS:** repository byte-locked minimal Quest projection child.
3. **Only after visible culling blocker:** `+0x3A16A6` / CSimpleWorldView lineage.

## CONCRETE TARGETS
### HLAE / view
- `client.dll+0x003A5280`
- `AfxHookSource.dll+0x00022340`
- `AfxHookSource.dll+0x00014BA0`
- `CViewSetup+0x68/+0x6C..+0xAB`

### Frustum / world
- `client+0x0086E9C0`
- lane writes `+0x0086EB3C/+0x0086EB46/+0x0086EB51/+0x0086EB59`
- `client+0x003A16A6..+0x003A16DC`
- `client+0x003A01E4..+0x003A021A`
- CSimpleWorldView vtable `client+0x00BF4434`
- CSimpleWorldView Draw `client+0x003B0870`
- hydration `+0x003B0067..+0x003B023D`
- slot index `client+0x05248ED4`
- arrays `+0x0524A2F4/+0x0524A2F8/+0x0524A2FC`
- stride `0x13F0`

### Camera retained as INFRASTRUCTURE/candidates
- `client+0x0038D123`
- `client+0x0038D15B`
- `client+0x0038D1DE`
- `client+0x05245AB8`
- `client+0x052632CC`
- `client+0x05263384`

## v0.7.1 TRANSPORT CONTRACT
Full package manifest: `runtime/children/v0.7.1_xr_transport/README.md`.

Core rules:
- T0 host/session diagnostic before game
- x64 host is sole OpenXR owner
- Present only latches D3D9 device
- capture after original HLAE DoRenderView
- `pack(4)` shared ABI
- two pair slots
- LEFT opens, RIGHT completes/publishes
- Reset invalidates generation/resources
- RGB-only hash
- one exact same-pair BMP pair collected

### Structural same-pair proof added by v0.7.1
A valid pair requires:
- same `CAfxStreams*`
- LEFT HLAE count `0 -> 1`
- RIGHT count `1 -> 2`
- same mode/pair contract

One-shot evidence tokens:
- producer `DISTINCT PAIR PROOF`
- host `PAIR PROOF PASS`

This specifically addresses the older capture caveat where sequential same-eye drift (~62%) was larger than historical L/R pixel difference (~54%).

## CURRENT GATE
**XR TRANSPORT / headset submission precondition for the prepared projection gate.**

## NEXT
Run only `CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF`.

PASS:
- freeze transport
- STOP transport research
- immediately execute the already-committed Quest projection child

FAIL:
- classify exact failed stage: T0 XR, HLAE pair, D3D9 capture, shared memory, x64 upload or XR game-layer submission
- ADAPT only that stage
- no broad Source/OpenXR reverse

## METHOD ALREADY USED
- SOURCE/DONOR harvest
- exact frozen binary/static analysis
- HLAE source + binary archaeology
- multipass runtime proof
- custom projection/off-center causal canaries
- deterministic projection oracle
- Quest OpenXR diagnostic
- D3D9 hooks/readback
- TTD frustum/cache replay
- TTD pre-eye reader recovery
- x32dbg/causal camera infrastructure controls
- CSimpleWorldView ownership/class funnel
- OBS, FEAR-VR, CallOfDuty4-VR and capture-engine transport donor comparison
- AdvancedFX D3D9Ex history review

## UNRESOLVED
- actual final XR transport/headset PASS
- final Quest projection headset verdict after transport
- authoritative eye-origin/angle seam if visible defect remains
- same-frame culling/visibility policy
- exact ClientWorldListInfo ownership semantics if needed
- HMD center pose ownership/timing
- viewmodel/weapon/hands owner
- controller/aim/muzzle policy
- HUD composition
- cadence/recenter/menu/save/load/cutscene/long-gameplay regression
