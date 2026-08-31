# META PROMOTION — CONTROL-PLANE DATE SEMANTICS — 2026-08-31

CLAIM_ID: `META-CONTROL-PLANE-DATE-SEMANTICS`
ROLE: `META`
EVIDENCE_CLASS: `CONTROL_PLANE`
RUNTIME_CLAIM_CHANGE: NONE
BASELINE_CHANGE: NONE

## Problem
Legacy `RESEARCH_CONTRACT.yaml` includes `docs/*EVIDENCE*.md` in `evidence_globs`. `docs/EVIDENCE_BOUNDARY.md` carried a calendar `Updated:` date, so `research_integrity.py` incorrectly classified a governance document as newer research evidence and raised `STATE_STALE`.

## Fix
Replace the calendar update marker with `CONTROL_PLANE_REV: 1`. The boundary remains readable/auditable but no longer masquerades as dated research evidence.

## Why SYNC is not changed
No CSGO runtime/evidence fact changed. Advancing canonical `docs/SYNC.md` merely to satisfy a governance timestamp would contaminate research chronology.

## Expected verification
Both `Research integrity` and `Research Integrity Gate` must PASS on this promotion PR before merge.
