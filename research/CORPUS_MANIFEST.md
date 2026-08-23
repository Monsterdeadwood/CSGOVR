# CSGOVR — CORPUS MANIFEST

Large knowledge/evidence corpora are kept outside ordinary Git history. This file records identity and authority.

## Canonical fact ledger
File: `CSGOVR_KNOWLEDGE_BASE_CANONICAL_20260823.md`

SHA256:
`7377e45b856812ac6b65001b1b60c149d4d4af0c3189b8af589c5183225d45f3`

Role:
- authoritative current fact ledger
- evidence-grade definitions
- contradiction guards / superseded facts
- exact result classification table
- current engineering handoff

When GitHub state conflicts with this file, correct GitHub rather than silently upgrading older claims.

## Full evidence knowledge base
File: `CSGOVR_KNOWLEDGE_BASE_FULL_EVIDENCE_20260823.md`

SHA256:
`c427198d44849c495ed674d615325317b6d1152f76bfe9e898d7fc2403c76b38`

Role:
- broader historical/raw evidence
- lower convenience than the canonical fact ledger for current project decisions

## Evidence grade vocabulary
- GOLDEN
- PROVEN
- USER-CONFIRMED
- SUPPORTED
- PENDING
- INFRASTRUCTURE
- FALSIFIED
- INVALID
- SUPERSEDED

Do not collapse these grades into a binary "works/doesn't work" label.

## Current canonical highlights
- GOLDEN: `CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX`
- full `CViewSetup` size `0x198`
- custom projection bool `+0x68`
- custom matrix `+0x6C..+0xAB`
- off-center fields `+0x138/+0x13C/+0x140/+0x144`, flag `+0x148 bit0`
- concrete pre-eye reader `client.dll+0x003A16A6`
- `client.dll+0x0525BA60` both-eye reuse hypothesis FALSIFIED
- origin/yaw mutation seams are INFRASTRUCTURE, not ownership proof

## Ingestion rule
Do not commit the full evidence vault as one giant blob. Curate current truth into `docs/`, record exact result identities in `research/RESULT_INDEX.md`, and keep this manifest as the identity bridge to the Library corpus.
