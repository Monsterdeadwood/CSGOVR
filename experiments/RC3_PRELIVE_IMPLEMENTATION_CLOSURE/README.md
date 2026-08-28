# RC3 PRE-LIVE IMPLEMENTATION CLOSURE

`CLAIM_ID = CSGO-RC3-PRELIVE-IMPLEMENTATION-CLOSURE`

Class: **COLD**.

This experiment does not modify or run CS:GO. It audits the exact prepared RC3 ZIP and extracts only evidence needed to decide whether the implementation is safe enough to authorize the already-prepared RC3 LIVE ticket.

## Input identity

Expected package:
`CSGOVR_ULTIMATE_SAFE161_NATIVE_AIM_RC3_3HOOK_GUARDFIX_ONECLICK.zip`

Expected SHA256:
`B988DEB8DEE490CFB6964ADA004DF69F454AF48A689BDD60E2A67CCE5B72C2FB`

Run `00_RUN_RC3_PRELIVE_COLD.bat`. With no argument it checks the current folder, this script folder, Downloads and Desktop for the exact filename. You can also drag the exact RC3 ZIP onto the BAT.

## Output

`CSGOVR_RC3_PRELIVE_COLD_RESULT_YYYYMMDD_HHMMSS.zip`

The result contains:
- exact package SHA identity;
- full package file list;
- per-invariant source contexts under `HITS/`;
- complete matching text/source files under `RELEVANT_SOURCE/`, capped at 12 MiB;
- SHA256 ledger of copied relevant source;
- explicit list of files skipped by the size cap;
- a summary that deliberately withholds the semantic PASS/FAIL verdict.

## Why no automatic semantic PASS

A token such as `sessionEpoch`, `bundleReady`, `MH_CreateHook` or `A968E0` proves where to inspect, not that control flow is correct. The returned source must be reviewed against the ten exact implementation invariants in canonical `docs/SYNC.md`.

Zero hits in a required evidence group are useful negative evidence, but still do not automatically prove absence if the implementation uses different names.

## Stop rule

Do not launch RC3 from this experiment. Do not create RC4. Upload/review the RESULT ZIP, close the exact implementation claim, then either activate the existing RC3 runtime ticket or fix only the specific failed invariant.
