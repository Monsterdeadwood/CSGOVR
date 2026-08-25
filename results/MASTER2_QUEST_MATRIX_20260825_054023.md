# MASTER 2 — QUEST MATRIX — ACCEPTED RUNTIME RESULT

Result: `CSGOVR_QUEST_COHERENCE_RESULT_20260825_054023.zip`
SHA256: `F1246B414A512A4E22B80563D167A6532505FE43C6DD7161476136FE7F384C0F`

Verdict: **MASTER 2 PASS** for the bounded missing fact: the exact Quest 3 asymmetric matrix reaches frozen-retail CS:GO raster through the proven custom projection carrier.

## Runtime path proof
- PATH_PROOF=PASS
- staged and DLL-opened path both: `%TEMP%\CSGOVR_QUEST_MATRIX_BRIDGE\quest_matrix.ini`
- staged and DLL-read SHA256 both: `2C45D522FE805ABDDA58D10EF3FE62A846783508FD1AF9EC480CE4B00EB685BF`
- applied LEFT: M00=0.9027383327, M02=-0.2425126284, M11=0.8354788423, M12=-0.1931874454
- applied RIGHT: M00=0.9027383327, M02=+0.2425126284, M11=0.8354788423, M12=-0.1931874454
- FOV/aspect/zNear/zFar unchanged.

## Instrument validity
Same-eye normalized MAE:
- BASE_A LEFT 0.00003119
- BASE_A RIGHT 0.00004130
- QUEST LEFT 0.00002205
- QUEST RIGHT 0.00001509
- BASE_B LEFT 0.00002561
- BASE_B RIGHT 0.00002447

BASE_A -> BASE_B reset drift:
- LEFT 0.00043230
- RIGHT 0.00042704

Both are far below the 1% invalidation threshold.

## Causal raster result
Feature/homography review of returned BMPs, mapping BASE_A -> QUEST:
- LEFT center displacement approximately `+116.13 px X, -52.06 px Y`
- RIGHT center displacement approximately `-116.32 px X, -51.83 px Y`
- BASE_A -> BASE_B center displacement approximately 0 px in both eyes.

This matches the already-runtime-proven v0.6.1 sign calibration:
- M02 +0.10 -> X -48 px
- M02 -0.10 -> X +48 px
- M12 +0.10 -> Y +27 px
- M12 -0.10 -> Y -27 px

Therefore the current exact M02 magnitudes (~0.2425) predict ~116 px opposite horizontal shifts and M12 ~-0.1932 predicts ~-52 px Y, exactly what the returned raster shows.

## Prediction-text correction
The MASTER2 runner's `PREDICTIONS.txt` used the English word `downward` for negative M12. That wording is inconsistent with the earlier causal v0.6.1 calibration, which empirically locked `M12 < 0 -> negative image Y`. Runtime matched the stronger prior causal evidence. This wording error is not a runtime failure and must not cause another projection rerun.

## Headset observation / runner defect
User observed only three apparent frames/states. This is explained by runner cadence, not by the matrix or OpenXR transport:
- each phase starts `mirv_streams record`
- runner stops as soon as two BMPs per eye exist
- each capture window lasts only ~0.2–0.3 s
- between phases XR host reprojects the last complete pair

Thus v0.8.1 is a good causal raster instrument but a bad live-headset UX instrument. Label this separately as `BURST_CAPTURE_STALE_PAIR_SLIDESHOW`. Do not repeat this cadence in MASTER3 or later playable work.

## Next gate
Open MASTER 3.1 only:
- early native `CViewRender::RenderView` at `client.dll+0x003A5280`
- one deterministic constant origin change at `CViewSetup+0xB8`
- prove whether it survives to raster
- no HMD pose yet
- no hands yet
- no broad reverse
- gameplay/headset path must remain continuously live; control capture must not replace the stream with three stale frames.
