# CSGOVR — STATE SYNC

Snapshot: 2026-08-25 after live MASTER 1 PASS and live MASTER 2 PASS.

## AUTHORITY / CURRENT PROGRAM
Execution order remains strict: `MASTER 1 -> MASTER 2 -> MASTER 3 -> MASTER 4`.
Raw executed RESULT > generated status summary > plan/README.
A2 GPU sharing is parked until a later MASTER4 120-second CPU performance gate numerically FAILS.

Frozen baseline:
- legacy CS:GO x86, AppID `4465480`, BuildID `22100934`
- client SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- HLAE 2.191.1
- v0.5.5: pass1 LEFT / pass2 RIGHT / pass3 monitor
- full CViewSetup `0x198`
- `-insecure`, local/offline only

## MASTER 1 — PASS
Accepted: `CSGOVR_XR_TRANSPORT_RESULT_20260825_044816.zip`.
- 123 produced / 123 consumed coherent pairs
- drops=0, captureFailures=0
- `PAIR PROOF PASS`, LEFT/RIGHT different
- OpenXR `source=GAME`, layerCount=1
- user saw CS:GO image in Quest

Detailed record: `results/MASTER1_XR_TRANSPORT_20260825_044816.md`.

## MASTER 2 — PASS
Accepted: `CSGOVR_QUEST_COHERENCE_RESULT_20260825_054023.zip`.
SHA256 `F1246B414A512A4E22B80563D167A6532505FE43C6DD7161476136FE7F384C0F`.

Runtime path proof:
- actual DLL-opened matrix file and runner-staged path match
- SHA256 both `2C45D522FE805ABDDA58D10EF3FE62A846783508FD1AF9EC480CE4B00EB685BF`
- LEFT M00=0.9027383327 M02=-0.2425126284 M11=0.8354788423 M12=-0.1931874454
- RIGHT M00=0.9027383327 M02=+0.2425126284 M11=0.8354788423 M12=-0.1931874454
- FOV/aspect/zNear/zFar unchanged

Instrument validity:
- all same-eye normalized MAE around 0.000015..0.000041
- BASE_A -> BASE_B reset drift ~0.00043 each eye
- safely below the 1% invalidation threshold

Causal raster result:
- LEFT BASE->QUEST center displacement approximately `+116.13 px X, -52.06 px Y`
- RIGHT approximately `-116.32 px X, -51.83 px Y`
- BASE_A->BASE_B approximately 0 px
- matches prior causal calibration: M02 -0.10 -> +48 px X, M02 +0.10 -> -48 px X, M12 -0.10 -> -27 px Y

The runner text `downward` for negative M12 was a wording defect. Stronger prior causal evidence defines negative M12 as negative image Y, and the new run matches that exactly. Do not rerun projection for this wording issue.

Detailed record: `results/MASTER2_QUEST_MATRIX_20260825_054023.md`.

## RUNNER UX DEFECT RETAINED
`BURST_CAPTURE_STALE_PAIR_SLIDESHOW`:
v0.8.1 started/stopped `mirv_streams record` separately for BASE_A, QUEST and BASE_B, only long enough for two BMPs/eye. Between bursts the XR host reprojected the last complete pair, so the headset appeared to show three static frames. This is not a transport or projection limitation. MASTER3+ must keep one continuous gameplay/eye stream through causal mode changes and harvest evidence without replacing VR with slides.

## PROVEN / RETAIN
- two real HLAE world-eye streams
- CPU D3D9 complete-pair transport live to Meta OpenXR
- exact Quest 3 asymmetric FOV and matrix values
- custom projection carrier `CViewSetup +0x68/+0x6C`
- M00/M02/M11/M12 raster semantics
- native RenderView `client+0x003A5280`
- late HLAE DoRenderView origin intervention is not final position authority

## CURRENT GATE
**MASTER 3.1 — bounded early native RenderView origin ownership probe.**

One missing fact only:
Does a deterministic origin translation applied to a full local `0x198` CViewSetup at native `client.dll+0x003A5280` survive to the real eye raster?

Required implementation constraints:
- reuse current M1 transport and M2 exact Quest projection
- hook only exact frozen native RenderView with binary/vtable guards
- never write caller-owned CViewSetup; copy all `0x198` bytes locally
- mutate only origin `+0xB8` for the causal phase
- pass3/monitor untouched
- continuous `mirv_streams record` across BASE -> SHIFT -> BASE reset, no burst stop/start between phases
- save exact same-pair evidence by mode while gameplay stream remains live
- no HMD pose, controller, weapon, culling, A2 or MASTER4 work yet

## AFTER MASTER 3.1
PASS -> MASTER3.2 real HMD pose at the winning early seam.
FAIL -> one leading ownership/timing hypothesis and one bounded next test, no broad reverse.
