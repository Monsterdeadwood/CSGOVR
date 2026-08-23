# CSGOVR — FALSIFIED / CLOSED / STOPPED

Canonical source: `CSGOVR_KNOWLEDGE_BASE_CANONICAL_20260823.md`, augmented by 2026-08-24 cross-branch reconciliation.

## Architectural / scientific falsifications
- **Full `CViewSetup=0x14C`** — SUPERSEDED; exact HLAE copy ctor proves `0x198`.
- **`client+0x0525BA60` reused inside BOTH LEFT and RIGHT eye marker regions** — FALSIFIED; observed accesses were outside eye brackets.
- **Persistent helper arg2 output beyond first `0x40` (`+0x40..+0x1FF`)** — FALSIFIED.
- **Earlier parser FALSIFIED verdict for first `0x40` transpose** — INVALID/SUPERSEDED; temporary transpose/restore is PROVEN.
- **OriginX/yaw wrappers as camera ownership proof** — REJECTED; INFRASTRUCTURE only.
- **Symmetric eye projection sufficient for Quest 3** — FALSIFIED by measured asymmetric FOV.
- **Old off-center identity `[-1,+1]`** — FALSIFIED; normalized Source interpretation retained.
- **Raw Khronos column-major matrix memcpy into Source VMatrix** — FALSIFIED/closed by causal matrix semantics.
- **AdvancedFX Unity `0.01905 m/unit` as native SourceVR HMD/eye scale** — FALSIFIED for this purpose; Valve SourceVR uses `39.3700787 inches/meter`.
- **Static post-SetUpView candidates `+0x38E5D7/+0x38EC08/+0x3A0381` as retained final route** — CLOSED by newer lineage.
- **CSkyboxView / CShadowDepthView as main-world owner** — FALSIFIED for the main-world ownership role; CSimpleWorldView lineage retained.

## Exact late-origin negative result — narrow scope
`CSGOVR_NATIVE_STEREO_SEAM_RESULT_20260821_041928` showed one tested late DUAL_ORIGIN intervention did not authoritatively change that final captured raster.

This does **not** falsify eye translation generally because:
- actual v0.5.5 GOLDEN BMPs show depth-dependent L/R parallax;
- WORLD_X_PAIR proved RenderView-origin translation can reach GPU c2, pixels and draw membership.

Falsified statement only: that exact late intervention/capture was not an authoritative final eye-origin control point.

## Transport classifications
- **CPU-readback `CSGOVR_XR_TRANSPORT_v0.7.0_BUILD_FIRST_ONECLICK.zip` as PRIMARY architecture** — COLD-SUPERSEDED while GPU sharing remains viable; package is UNRUN, not runtime-falsified.
- **Assuming modern AfxInterop v6/v7 automatically executes the older shared-target `PrepareDrawReply` handshake** — CLOSED as an assumption. Exact source shows v6/v7 bypasses that older exchange and uses newer `HandleDrawingMessage` flow.
- **Blind HLAE copy ctor `+0x14BA0` as clean per-eye early hook** — STOP; ctor also participates in sky/other CView copies.

Shared-target and `D3DQUERYTYPE_EVENT` primitives themselves remain valid source-proven donors.

## Closed experiment classes / do not repeat
- broad OpenXR FOV/view rediscovery while Quest baseline remains valid
- proving again that two world streams exist
- proving again that `CViewSetup.origin` is live
- proving again frozen `RenderView client+0x003A5280`
- projection sign/transpose/row-vs-column roulette after v0.6.1
- sequential screenshots as quantitative disparity/culling truth without same-eye control
- treating compositor submission alone as proof of correct stereo geometry
- treating two renders alone as proof of correct culling
- forcing world projection blindly onto viewmodel
- coupling HMD camera ownership to weapon aim
- building a replacement renderer before exhausting the Source/HLAE path
- broad 8–12 origin-write-point batch until a concrete post-G3 eye-origin defect exists
- broad analyzer after a concrete target already exists
- combining transport + Quest projection + new origin/head/culling/weapon in the first T0 transport proof

## Invalid / infrastructure failures, not scientific falsifications
- `CSGOVR_FULL_EXPERIMENTAL_FAILURE_20260821_022832.zip` — cross-project source/root contamination + build failure
- v0.6.5 XRFOV end-to-end failure — path wiring (`g_xrFovPath` empty/uninitialized, stale INI path), not XR/stereo/math falsification
- missing defines/link flags/stale PID/gameplay gate/parser failures — infrastructure unless scientific prediction executed

## Retained speculative camera anchors — NOT ownership
- `client+0x0038D123`
- `client+0x052632CC`
- `client+0x05263384`
- yaw `client+0x0038D15B`, `+0x0038D1DE`, `+0x05245AB8`

## STOP rule at CURRENT GATE
CURRENT GATE is **T0 / G2.5 GPU XR TRANSPORT CERTIFICATION**.

T0 must change transport only. Do not mutate Quest P00, origin, head pose, culling, viewmodel, weapon or HUD in the same PRIMARY. Use/log a verified-compatible AfxInterop protocol mode or minimal shared-target/fence transplant; do not assume v6/v7 uses the older replacement-target handshake.

G3/T1 projection is queued immediately after T0 PASS.
