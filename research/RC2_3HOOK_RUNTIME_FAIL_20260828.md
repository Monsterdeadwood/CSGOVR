# CSGOVR RC2 3-HOOK RUNTIME FAILURE

Snapshot: 2026-08-28

Result: `CSGOVR_ULTIMATE_NATIVE_AIM_RC2_3HOOK_RESULT_20260828_205342.zip`

Classification: **FAIL / DO NOT PROMOTE**.

## Machine result

```text
WORLD_STEREO_HMD=PASS
WEAPON_HAND_FOLLOW=PASS
NATIVE_AIM_EXACT_GUARD=False
NATIVE_AIM_3HOOK_ACTIVE=False
NATIVE_AIM_BASELINE_GATE=False
NATIVE_AIM_PRODUCER_ARMED=False
NATIVE_AIM_PUBLISH_SEEN=False
NATIVE_AIM_CLIENT_GATE_SEEN=False
NATIVE_AIM_SERVER_GATE_SEEN=False
SERVER_RUNCOMMAND_HOOK=ABSENT_BY_BUILD
SERVER_TLS=ABSENT_BY_BUILD
```

`EYE_TRANSPORT.log` repeatedly reports:

```text
NATIVE_AIM_EXACT_GUARD FAIL reason=HEAD_OWNER_OR_SEAM_BYTES mutation=0
```

Therefore the three-hook gameplay path never installed or executed. Center-screen fire in this run is stock behavior and does not falsify the COLD-proven `CUserCmd::aimdirection` ownership architecture.

## User visual verdict

The user reported two hard failures:

1. distant textures/objects disappear;
2. the weapon still fires through screen center.

Human visual baseline verdict overrides the automated world/HMD plumbing counter. The RC2 integration parent is rejected as a visual baseline even though the stereo/HMD counters were alive.

## Root cause of native-aim installation failure

The RC2 runtime exact guard compared the full bytes at frozen `server+0x005BC67A`. That instruction sequence contains:

```text
A1 E0 68 A9 10
```

The `A1` operand is an absolute preferred-image VA and has a base relocation. Under ASLR the loader correctly rewrites it to `actual server module base + 0x00A968E0`. A live full-byte comparison against the on-disk preferred VA therefore rejects the correct frozen module.

Correct runtime guard:

- compare the relocation-stable prefix through opcode `A1`;
- read the live 32-bit operand;
- require `operand == actualServerBase + 0x00A968E0`;
- compare the relocation-stable suffix;
- log the exact failing seam.

Evidence class: root cause **COLD CLOSED**.

## Next integration base

Do not patch the failed RC2 renderer lineage forward.

Use the previously user-verified `CSGOVR_WEAPON_SAFE_POSTCALC_AIM_v1.6.1_ONECLICK` visual/render ancestor, for which distant textures remained present and shot audio was present. Add only:

- the COLD-proven three-hook current-command native aim path;
- relocation-aware exact runtime guard;
- truthful gating that cannot print READY while native aim is inactive.

PVS/culling mutation remains a separate lane and must not be combined with this weapon/aim LIVE.
