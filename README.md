# CSGOVR

Native VR research for **frozen legacy CS:GO x86** using HLAE/OpenXR and exact retail evidence.

## Current status
**ACTIVE — G3 Quest asymmetric projection integration.**

GOLDEN:
`CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`

Current engineering gate:
**run/validate the already-prepared minimal Quest projection child on the exact v0.5.5 LEFT/RIGHT path; do not reopen camera/culling archaeology unless a concrete visible blocker survives.**

Primary runtime candidate already in repo:
`experiments/quest_projection_child/eye_patch_quest_projection.c`

Integrity guard:
`tools/verify_quest_projection_child.py` + `.github/workflows/evidence-integrity.yml`

## LATEST RESULT / branch status
- Latest accepted projection result: `CSGOVR_CUSTOM_PROJECTION_MATRIX_RESULT_20260821_042218.zip` — custom matrix semantics PROVEN.
- XR input result: `CSGOVR_OPENXR_FOV_DIAGNOSTIC_20260821_150332.zip` — Quest 3 FOV / two valid views PROVEN.
- Latest forensic result overall: `CSGOVR_TTD_FRUSTUM_ARG2_PERSISTENCE_RESULT_20260823_164733.zip` — first 0x40-byte transpose/restore PROVEN; persistent output beyond 0x40 FALSIFIED.
- No accepted RESULT yet closes G3.
- `CSGOVR_G3_QUEST_COHERENCE_BATCH_v1.0.1_SYNCED_ONE_CLICK.zip` was prepared in a chat branch as a diagnostic batch, SHA256 `595DA174AC39BA334F725DC1D130089F7F7C98D2BC4878F87B12F113BC0EAB70`, but **has not been run** and is not a new baseline. Its PRIMARY useful phase is matrix-only; FOV/aspect phases are secondary G4 harvest.

## Already proven
- two real world streams: pass1 LEFT, pass2 RIGHT, pass3 monitor
- full `CViewSetup` size `0x198`
- custom projection enable `+0x68`
- custom projection matrix `+0x6C..+0xAB`
- world FOV `+0xB0`, origin `+0xB8`, angles `+0xC4`, zNear/zFar `+0xD0/+0xD4`, aspect `+0xE0`
- off-center fields `+0x138/+0x13C/+0x140/+0x144`, flag `+0x148 bit0`
- Quest 3 asymmetric FOV/coefficients and recommended eye size 1504x1616
- CPU `CViewSetup.origin` -> GPU VS `c2 / CAMERA_POS` exact causal path
- original v0.5.5 image set contains real depth-dependent LEFT/RIGHT parallax; this does **not** erase the separate late-origin intervention failure
- concrete pre-eye frustum/cache reader `client.dll+0x003A16A6`
- CSimpleWorldView main-world lineage

## Evidence conflict that must remain explicit
Two facts coexist:
1. `CSGOVR_NATIVE_STEREO_SEAM_RESULT_20260821_041928` found that one tested **late** DUAL_ORIGIN intervention did not authoritatively alter its final captured raster.
2. Re-analysis of original `CSGOVR_HLAE_EYE_PATCH_RESULT_20260821_015737.zip` BMPs shows stable depth-dependent L/R disparity from the actual v0.5.5 GOLDEN path; earlier retail `WORLD_X_PAIR` causal work also proved RenderView-origin changes can reach GPU/raster.

Therefore do **not** promote the late intervention to a general falsification of eye translation and do not rerun broad origin-owner search before G3.

## Important closed paths
- `0x14C` as full CViewSetup — SUPERSEDED
- `client.dll+0x0525BA60` reused inside BOTH eye markers — FALSIFIED
- origin/yaw byte-integrity wrappers as camera ownership proof — rejected; INFRASTRUCTURE only
- sequential screenshots as quantitative disparity/culling truth — closed; use deterministic/same-frame evidence
- broad OpenXR rediscovery — closed while XR baseline is valid
- contaminated FULL_EXPERIMENTAL build failures — INVALID renderer evidence
- old 8–12 point eye-origin batch — STOP until a post-G3 visible eye-origin blocker exists

## Frozen target / dependencies
- Steam AppID `4465480`, frozen legacy CS:GO x86, BuildID `22100934`
- game dir: `C:\Program Files (x86)\Steam\steamapps\common\csgo legacy`
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- HLAE `2.191.1`; HLAE package SHA256 `D7BA05E1E540D82AC485A9B39F13D5C4CCF9FD94F460E0C9ECF069B94C880A59`
- `AfxHookSource.dll` SHA256 `5B9D13AE9ABFFEEF80851E10861302162A47D1B43D8483B4A8FCEC7EAA19B035`
- Meta/Oculus OpenXR runtime recorded as `1.205.0`, Quest 3
- canonical safety/lab scenario: exact `-insecure`, local `sv_lan`, Dust II, windowed, `mat_queue_mode 0`, no persistent patch

## Start here
1. `docs/STATE.md` — full current handoff
2. `docs/NEXT.md` — one-step continuation
3. `docs/SYNC.md` — cross-branch snapshot and evidence grades
4. `docs/GATES.md` — integration ladder
5. `docs/PROVEN.md` — accepted facts + provenance
6. `docs/FALSIFIED.md` — closed/superseded paths
7. `docs/EXPERIMENTS.md` — experiment/tool lineage and do-not-repeat map
8. `research/RESULT_INDEX.md` — exact result ledger
9. `research/BRANCH_SYNC_20260824.md` — urgent branch-sync provenance

## Working model
`SYNC -> THINK -> KEEP/ADAPT/STOP -> BATCH HARVEST -> CONVERGE -> PROVEN/FALSIFIED -> NEXT GATE -> SYNC`

**PARALLELIZE EVIDENCE — SERIALIZE MUTATION.**

Evidence grades are strict: GOLDEN / PROVEN / USER-CONFIRMED / SUPPORTED / PENDING / INFRASTRUCTURE / FALSIFIED / INVALID / SUPERSEDED.

## Repository map
- `docs/` — canonical state and policy
- `research/` — evidence and archaeology
- `experiments/` — reproducible controlled experiments
- `runtime/` — accepted/preserved integration code
- `tools/` — project tooling and integrity verification
