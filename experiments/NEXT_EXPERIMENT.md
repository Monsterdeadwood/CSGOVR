# CSGOVR — NEXT EXPERIMENT

Canonical state: `docs/SYNC.md`.

`CLAIM_ID = CSGO-RC3-PRELIVE-IMPLEMENTATION-CLOSURE`

Class: **COLD / READ-ONLY PACKAGE-SOURCE AUDIT**.

## Primary fact

Does the exact prepared RC3 package/source implement the complete fail-closed command-provenance and three-hook installation contract required by the already COLD-proven architecture?

This does not reopen native `CUserCmd::aimdirection`, current-command ownership, the six approved caller/register cases, FX ABI, or the three-hook architecture itself.

## Required PASS surface

The exact RC3 implementation must prove all items listed under `CURRENT GATE - RC3 PRE-LIVE IMPLEMENTATION CLOSURE` in `docs/SYNC.md`, including session/ring identity and invalidation, stock rejects, behavioral hook-install atomicity, relocation-aware guard, forbidden mutations absent and SAFE161 protected lineage unchanged.

## Available package identity

Prepared RC3:
`CSGOVR_ULTIMATE_SAFE161_NATIVE_AIM_RC3_3HOOK_GUARDFIX_ONECLICK.zip`

SHA256:
`B988DEB8DEE490CFB6964ADA004DF69F454AF48A689BDD60E2A67CCE5B72C2FB`

The accessible Library currently exposes the exact SHA sidecar, while GitHub exposes the RC3 LIVE ticket, but neither surface currently exposes the exact RC3 package source/ZIP bytes for independent audit.

## Decision

PASS -> `CSGO-RC3-SAFE161-3HOOK-RUNTIME` becomes ACTIVE and the existing RC3 LIVE ticket is used unchanged.

FAIL -> fix only the exact missing implementation invariant and re-audit. Do not create a different gameplay architecture.

## Stop

No new weapon LIVE while this claim is ACTIVE. No RC4. No PVS mutation in this lane.
