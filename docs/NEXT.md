# CSGOVR — NEXT

Snapshot: 2026-08-25 after accepted MASTER2 result `CSGOVR_QUEST_COHERENCE_RESULT_20260825_054023.zip`.

## CURRENT GATE
**MASTER 3.1 — early native RenderView origin ownership.**

MASTER 1 = PASS.
MASTER 2 = PASS.
MASTER 3.2, hands and MASTER 4 remain closed until 3.1 PASS.
A2 remains parked until MASTER4 PERF numerically FAILS.

## M2 RESULT RETAINED
Exact Quest matrix is now runtime proven to reach raster through `CViewSetup +0x68/+0x6C`.
- LEFT M02 negative produced about +116 px X
- RIGHT M02 positive produced about -116 px X
- common M12 negative produced about -52 px Y
- reset returned approximately to 0 px
- same-eye and reset controls are far below 1% MAE

The previous word `downward` for M12<0 was a prediction-text error. Prior causal calibration and current raster agree on negative image Y. Do not rerun M2.

## UX RULE FROM M2
Do not repeat burst capture. v0.8.1 showed three apparent static states because each phase ran `mirv_streams record` only long enough for two BMPs and stopped it again. MASTER3.1 must start recording once, keep the real stereo/game stream continuous, switch causal mode while recording, and stop only after the final reset.

## PREPARED / UNRUN PACKAGE
`CSGOVR_MASTER3_1_EARLY_ORIGIN_v0.9.0_LIVE_ONECLICK.zip`
SHA256 `680D76F1C95BE286E0E2CCFF0B1327BF566A3F869C92811CE1AA61F09E8B2D4F`
Status: **PREPARED / STATIC-VERIFIED 39/39 / UNRUN**.
This is not runtime evidence.

Runner policy:
- exactly one causal `mirv_streams record start`;
- continuous BASE_A -> ORIGIN_SHIFT_RIGHT -> BASE_B_RESET;
- minimum 6 new frames/eye and 1200 ms per state;
- exactly one causal `record end` after reset;
- 12 selected evidence BMPs retained; bulk temporary raw BMPs removed after RESULT collection;
- previous `$Host`, `$PID`, and Object[] eye-log failure classes guarded.

## NEXT ONE STEP
Run the prepared package on the frozen target and return `CSGOVR_MASTER31_EARLY_ORIGIN_RESULT_*.zip`.

Bounded native origin probe:
- exact target `client.dll+0x003A5280`
- validate timestamp `0x65272025`, SizeOfImage `0x05509000`, exact prologue and both known RenderView vtable slot6 targets
- use a local full `0x198` CViewSetup copy
- mutate only `origin +0xB8`
- deterministic shift = +32 Source units along current camera RIGHT
- current exact Quest matrix remains active
- current M1 complete-pair CPU transport remains active
- HLAE pass1/pass2 supplies eye identity; pass3 remains untouched
- no stop/start between causal states

## PREFLIGHT — exactly 5 lines
**MISSING FACT:** does a deterministic origin change at native RenderView entry survive to the real frozen-retail eye raster?
**WHY NOT COLD:** the origin field and early function are exact-target facts, but only runtime raster proves this seam is authoritative at this point in the lifecycle.
**PASS:** full-copy local origin mutation is logged at native entry and the selected SHIFT same-pair raster changes consistently beyond same-eye/reset drift while BASE_RESET returns to baseline, with predicted camera-right parallax direction.
**FAIL:** exact hook/mutation is proven active but SHIFT raster does not exceed valid control drift, or mutation is overwritten before raster.
**BASELINE MUST HOLD:** frozen client hash, HLAE 2.191.1, CViewSetup 0x198, M1 transport, M2 exact Quest matrix, pass3 untouched, -insecure/local only.

## EXPLICIT EXCLUSIONS
- no real HMD pose yet
- no +0xC4 angle mutation
- no hands/controllers/aim
- no weapon/HUD
- no culling
- no A2
- no performance or comfort work
- no broad origin reverse

## AFTER 3.1 PASS
Open MASTER3.2 only: feed real `xrLocateViews` pose into the winning early seam, keeping camera ownership separate from body yaw, weapon aim and bullet origin.
