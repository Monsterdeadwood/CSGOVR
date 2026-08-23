# CSGOVR

Native VR research for **frozen legacy CS:GO x86** using HLAE/OpenXR and exact retail evidence.

## Current status
**ACTIVE — T0 / G2.5 GPU XR TRANSPORT CERTIFICATION.**

Protected GOLDEN: `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`

G3 Quest projection remains valid and is queued immediately after T0 PASS.

## CURRENT GATE
Prove exact v0.5.5 pass1 LEFT + pass2 RIGHT can reach a separate x64 Meta OpenXR host as **one coherent GPU pair**, while BASE projection/origin/camera and pass3 monitor remain unchanged.

## T0 FINALISTS
**A / PRIMARY:** pinned HLAE GPU shared-target path -> GPU completion -> x64 D3D11/Meta OpenXR.

**A' / implementation fallback:** minimal transplant of the same source-proven shared-target + EVENT-fence primitives into the v0.5.5 pass discriminator if direct AfxInterop protocol mode is incompatible.

**B / PREPARED FALLBACK:** `CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF`, SHA256 `DFEA881888C83812A5B9EE3A7049F79D32935240F2C612DE4ABDF4F9A98834A3`, STATIC-AUDITED / UNRUN. Activate only on a concrete A blocker or bounded compatibility control. Manifest: `runtime/children/v0.7.1_xr_transport/README.md`.

## CRITICAL INTEROP GUARD
Exact AdvancedFX `b97636852b8eecae09285b5a386192bb285638eb` proves D3D9 replacement-target and GPU-wait primitives exist. It also proves protocol generations differ:
- older `PrepareDrawReply_*` path receives replacement shared handles;
- server major **v6/v7 bypasses that older exchange** and uses newer `HandleDrawingMessage` flow.

Therefore do not assume a modern v6/v7 companion automatically exercises the old shared-target handshake. T0 must log/use an actually compatible protocol mode or use minimal transplant A'.

Full provenance: `research/HLAE_INTEROP_FINDINGS.md`.

## NEXT
1. Keep exact v0.5.5 BASE geometry and pass3 monitor.
2. Select/log compatible HLAE GPU-sharing protocol path or minimal transplant.
3. Bind same-cycle pass1 LEFT + pass2 RIGHT to one `pairSeq`.
4. GPU-complete before publishing each pair.
5. x64 D3D11 host on OpenXR adapter LUID `00000000:000118D1`: import eye resource(s), acquire/wait/copy/release both swapchains, one `xrEndFrame` per coherent pair.
6. Require `mixedPairCount=0`.
7. If A is concretely blocked, activate prepared v0.7.1 B without rediscovery.
8. T0 PASS -> freeze transport -> G3/T1 exact Quest P00 + Source-native scalar FOV `110°`.

## LATEST RESULT / branch status
- latest accepted forensic result `CSGOVR_TTD_FRUSTUM_ARG2_PERSISTENCE_RESULT_20260823_164733.zip`
- accepted projection parent `CSGOVR_CUSTOM_PROJECTION_MATRIX_RESULT_20260821_042218.zip`
- accepted XR input `CSGOVR_OPENXR_FOV_DIAGNOSTIC_20260821_150332.zip`
- no accepted end-to-end CSGOVR GPU/XR transport RESULT yet
- v0.7.1 CPU fallback is prepared, not a RESULT; v0.7.0 is superseded before LIVE
- queued G3 assets remain under `experiments/quest_projection_child/`

## Core proven facts
- AppID `4465480`, BuildID `22100934`, frozen x86 client
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- `-insecure` mandatory
- HLAE 2.191.1 / AdvancedFX `b976368...`
- pass1 LEFT, pass2 RIGHT, pass3 monitor
- full `CViewSetup=0x198`
- custom projection `+0x68`, matrix `+0x6C..+0xAB`
- Quest P00 coefficients / 1504x1616 recommended eye size
- CPU `CViewSetup.origin -> VS c2/CAMERA_POS`
- GOLDEN BMPs contain depth-dependent L/R parallax
- pre-eye reader `client+0x003A16A6`, CSimpleWorldView lineage
- native SourceVR OpenVR conversion uses `39.3700787 inches/meter`; Quest half-IPD ≈ `1.27520 Source units`
- v0.6.1 deterministic capture achieved within-phase MAE about `0.0000..0.0026 / 255`

## Important evidence conflict
One tested late DUAL_ORIGIN intervention failed to authoritatively change its final capture. Independently, actual GOLDEN shows depth-dependent parallax and WORLD_X_PAIR proves RenderView-origin changes can reach GPU/raster. Only that late point is negative; eye translation generally is not falsified.

## Start here
1. `docs/STATE.md`
2. `docs/NEXT.md`
3. `docs/SYNC.md`
4. `research/TRANSPORT_RECONCILIATION_20260824.md`
5. `research/TRANSPORT_SYNC_20260824.md`
6. `research/HLAE_INTEROP_FINDINGS.md`
7. `docs/GATES.md`
8. `docs/PROVEN.md` / `docs/FALSIFIED.md`
9. `docs/EXPERIMENTS.md`
10. `research/RESULT_INDEX.md`
11. `research/BRANCH_SYNC_20260824.md`
12. `runtime/golden_v0.5.5/`
13. `runtime/children/v0.7.1_xr_transport/`
14. `experiments/quest_projection_child/`

**PARALLELIZE EVIDENCE — SERIALIZE MUTATION.**
