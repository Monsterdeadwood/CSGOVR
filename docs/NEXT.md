# CSGOVR — NEXT

## CURRENT GATE
**G3 — Quest asymmetric projection integration.**

## NEXT ONE STEP
Run the already-committed byte-locked minimal Quest projection child and obtain a runtime/headset verdict.

Primary files:
- `experiments/quest_projection_child/eye_patch_quest_projection.c`
- `experiments/quest_projection_child/QUEST_PROJECTION_MINIMAL.patch`
- `experiments/NEXT_EXPERIMENT.md`
- `tools/verify_quest_projection_child.py`

## Before LIVE
Run the integrity verifier / CI-equivalent checks and preserve:
- frozen `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- exact HLAE 2.191.1 / `AfxHookSource.dll` lineage
- exact `-insecure`
- v0.5.5 pass scheduling
- full local `0x198` eye copies
- pass3 monitor untouched

## PRIMARY mutation
Only the already-proven Quest custom projection matrix is added to the existing local LEFT/RIGHT eye copies.

Do not add in the same PRIMARY:
- new origin write point
- culling mutation
- head pose
- weapon/hands
- HUD
- new OpenXR FOV discovery

## PASS
- pass1 gets LEFT Quest matrix (`M02<0`)
- pass2 gets RIGHT Quest matrix (`M02>0`)
- M00/M11/M12 match the proven Quest coefficients
- LEFT/RIGHT world streams remain healthy
- pass3 monitor remains healthy
- headset/same-frame visual verdict is coherent enough to close projection integration
- regression: NONE or only explicitly expected projection change

## FAIL handling
If matrix logging/write timing fails, fix only that integration defect.

If matrix application is proven but headset is black/absent, promote **XR transport/submission** as the concrete blocker; do not reopen projection math.

If matrix application is proven and a peripheral geometry/culling defect remains, open G4 using existing targets:
- `client+0x003A16A6`
- `client+0x003A01E4`
- `client+0x0086E9C0`
- CSimpleWorldView lineage

If eye-origin/parallax is visibly wrong after G3, reconcile against the existing v0.5.5 parallax evidence before any new origin-owner reverse.

## Prepared diagnostic batch
`CSGOVR_G3_QUEST_COHERENCE_BATCH_v1.0.1_SYNCED_ONE_CLICK.zip`
SHA256 `595DA174AC39BA334F725DC1D130089F7F7C98D2BC4878F87B12F113BC0EAB70`

Status: **UNRUN / NOT BASELINE**.
Use matrix-only as the PRIMARY-useful phase. Its FOV/aspect/150 phases are secondary harvest for a future G4 culling question and must not be allowed to redefine G3 PASS/FAIL.

## After RESULT
Record exactly:
- PRIMARY FACT: RESOLVED / NOT RESOLVED
- REGRESSION: NONE / EXPECTED / UNEXPECTED
- KEEP: exact source/build/matrix/path retained
- headset-visible blocker, if any
- NEXT ONE STEP

Then sync GitHub before opening a new expensive branch.
