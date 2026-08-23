# CSGOVR — GATES

## G0 — Quest/OpenXR input
**PASS.** Quest 3 two-view FOV, validity flags, adapter requirement and eye size are already acquired.

## G1 — HLAE stereo renderer
**PASS.** v0.5.5 provides real LEFT/RIGHT world streams plus monitor with full `0x198` eye copies.

## G2 — Projection-control semantics
**PASS.** Custom matrix/off-center fields and M00/M02/M11/M12 behavior are established.

## G3 — XR TRANSPORT
**CURRENT GATE.**

PASS requires:
- T0 OpenXR two-eye diagnostic submission;
- exact same-HLAE LEFT/RIGHT pair crosses x86 -> x64;
- pair is non-black and RGB-distinct;
- game projection layer is submitted to Quest;
- no >2 s freeze;
- GOLDEN pass schedule/monitor remains intact.

Current candidate: `CSGOVR_XR_TRANSPORT_v0.7.1_SYNC_PAIR_PROOF`.

## G4 — Quest asymmetric projection integration
Prepared remote child already exists. Start only after G3 PASS.

First-priority convention is direct known Quest custom matrix with engine near/far. If visual convention is still ambiguous, batch only the bounded `P00..P07` sign/storage family before opening any new reverse question.

## G5 — Authoritative eye origin / parallax
Only if still visibly wrong after G4. Late DoRenderView origin nudge was not authoritative in the tested result.

## G6 — Valid capture / culling truth
Use same-frame/repeated-eye-stable measurement. If a visible one-eye culling blocker exists, continue from:
- `+0x3A16A6`
- `+0x3A01E4`
- `GeneratePerspectiveFrustum +0x86E9C0`
- CSimpleWorldView lazy-hydration lineage

Do not revive `+0x0525BA60` both-eye reuse.

## G7 — Head pose
Center HMD pose ownership, predicted-time contract, recenter and latency.

## G8 — Weapon / hands / aim
Separate first-person owner from HMD camera and later bind controllers.

## G9 — HUD / UI
Separate composition policy.

## G10 — Regression / release
Normal launch, stereo, head, culling, shadows, HUD, save/load, menu/gameplay/cutscenes, no drift/state corruption, long gameplay.
