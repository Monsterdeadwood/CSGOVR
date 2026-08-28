# CSGOVR — NEXT

Canonical state: `docs/SYNC.md`.

## Active machine claims

- `CSGO-RC3-PRELIVE-IMPLEMENTATION-CLOSURE`
- `CSGO-PACING-PVS`

Queued after COLD PASS:
- `CSGO-RC3-SAFE161-3HOOK-RUNTIME`

## Primary next work

**COLD only. Do not launch the game from this gate.**

MISSING FACT:
whether the exact prepared RC3 package/source implements the complete fail-closed three-hook command-provenance contract, rather than merely describing it in the LIVE ticket.

Required exact-package checks are canonical in `docs/SYNC.md` and include:
- full ring/session/command/local-player identity;
- publication/consumption ordering;
- reconnect/map/session stale-entry invalidation;
- NaN/Inf rejection and signed-zero canonicalization for bitwise aim identity;
- no `IN_ATTACK`-only stamping hole;
- stock fallback on every rejected pointer path;
- behaviorally fail-closed partial hook installation;
- relocation-aware `A1` guard;
- forbidden RunCommand/TLS/FX/PVS/render-fire mutations absent;
- SAFE161 protected renderer/HMD lineage unchanged.

## RC3 remains the runtime candidate

Package:
`CSGOVR_ULTIMATE_SAFE161_NATIVE_AIM_RC3_3HOOK_GUARDFIX_ONECLICK.zip`

SHA256:
`B988DEB8DEE490CFB6964ADA004DF69F454AF48A689BDD60E2A67CCE5B72C2FB`

Ticket:
`experiments/RC3_SAFE161_3HOOK_GUARDFIX_LIVE_TICKET_20260828.md`

Do not create RC4. If exact implementation closure PASSes, promote this same RC3 runtime claim to ACTIVE and use its existing five-line LIVE preflight.

## Separate lane

`CSGO-PACING-PVS` remains independent. No PVS/culling mutation belongs in the implementation closure or RC3 weapon LIVE.
