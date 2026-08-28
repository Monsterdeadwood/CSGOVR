RESEARCH_DOC_CLASS: ARCHIVE
SUPERSEDED_BY: research/SERVER_CURRENTCMD_CLOSURE_20260828.md

# CSGOVR — SERVER CURRENT COMMAND COLD RECONCILIATION

Snapshot: **2026-08-28**

Result reviewed:
`CSGOVR_FX_FIRE_OWNERSHIP_CLOSURE_RESULT_20260828_195742.zip`

Frozen target:
- `client.dll` SHA256 `183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03`
- `server.dll` SHA256 `699757BB5C8DDB71193F68DABA22F59F697491DDBD768BC8656AC2CE435C4DB8`

## MACHINE VERDICT IS NOT THE SCIENTIFIC VERDICT

The result emitted:
`KEEP_4_HOOKS_COLD_NOT_CLOSED`.

Manual frozen-disassembly review shows the automatic gate contained false negatives and therefore must not be interpreted as falsifying the 3-hook architecture.

False negative 1:
- the register heuristic searched for `player+0x14AC` before `IsInputDeviceAPointer`;
- the exact gameplay TRUE path uses `player+0x14AC` immediately after the helper returns.

False negative 2:
- the RunCommand aim-copy heuristic failed despite an exact frozen copy sequence being present.

The direct-caller count for `CPlayerMove::RunCommand` is not an architecture invariant because this path is virtual/indirect.

## RETAIL PROVEN — SERVER POINTER PLAYER MAP 6/6

At the exact approved server pointer-helper returns:

```text
58F88B  grenade final aim   player = ESI
5A7A34  bullet A            player = EBX
5A8037  bullet B            player = EDI
5B0349  M3                  player = EDI
5B37BB  NOVA/Sawedoff       player = EDI
5BC61B  XM1014              player = EDI
```

All recovered player registers are x86 callee-saved registers, which is compatible with a detour at `IsInputDeviceAPointer`.

Evidence class: **RETAIL_PROVEN**.

## RETAIL PROVEN — RUNCOMMAND AIM COPY

Frozen `CPlayerMove::RunCommand = server+0x00339890` establishes:

```text
player = ESI
cmd    = EDI
```

Exact frozen copy:

```text
server+00339C5B  mov eax,[edi+18]
server+00339C5E  mov [esi+14AC],eax
server+00339C64  mov eax,[edi+1C]
server+00339C67  mov [esi+14B0],eax
server+00339C6D  mov eax,[edi+20]
server+00339C71  mov [esi+14B4],eax
```

Evidence class: **RETAIL_PROVEN**.

## STRONG CURRENT-COMMAND OWNER CANDIDATE — player+0x1034

Frozen RunCommand end contains:

```text
server+00339E63
player+0x1034 = NULL
```

This occurs in the same end-of-command region as prediction cleanup.

A separate frozen consumer contains:

```text
server+0033AC6F
read player+0x1034
store local
TEST pointer
NULL error/assert path
```

This independently gives `player+0x1034` current-command-like semantics rather than merely being an arbitrary field cleared at RunCommand exit.

Public cstrike15 source is candidate/invariant support only: `CPlayerMove::StartCommand` stores `player->m_pCurrentCommand = cmd`, `FinishCommand` clears it, and server lag compensation reads `GetCurrentUserCommand()` and treats NULL as invalid command execution context. These source facts do not replace frozen retail writer proof.

## ONLY REMAINING PRIMARY FACT

Still UNKNOWN on frozen retail:

```text
CPlayerMove::StartCommand
arg1 player
arg2 CUserCmd*
    -> [player+0x1034] = cmd
```

Until that exact writer is recovered, 3 hooks are not promoted.

## ARCHITECTURE STATUS

```text
3 HOOKS = VERY STRONG COLD CANDIDATE, ONE FROZEN WRITER SHORT
4 HOOKS = AUTHORITATIVE SAFE FALLBACK
2 HOOKS = NO JUSTIFICATION
FX HOOKS = FALLBACK / ORACLE ONLY
LIVE = DENIED
```

If frozen StartCommand proves `cmd -> player+0x1034`, the server RunCommand detour/TLS can be removed without weakening command provenance:

```text
SERVER IsInputDeviceAPointer
  -> exact approved return RVA
  -> exact callsite player register
  -> player+0x1034 currentCmd
  -> exact command/ring identity
  -> exact localPlayerIndex identity
  -> TRUE
```

## NEXT COLD GATE

REUSE/ADAPT the existing `CSGOVR_FX_FIRE_OWNERSHIP_CLOSURE_v1.2.0_COLD_OMNI.bat` lineage.

Mine:
1. frozen CPlayerMove vtable around known RunCommand;
2. candidate slot4 StartCommand body;
3. complete frozen read/write/clear census for displacement `+0x1034`;
4. exact StartCommand argument provenance;
5. retain `server+00339E63` clear and `server+0033AC6F` NULL-checked reader anchors.

PASS only if the frozen body proves `arg2 CUserCmd* -> [arg1 player+0x1034]` with no layout guess.

FAIL retains RunCommand/TLS.

## STOP RULE

Do not create LIVE/build/injection while this one frozen edge remains unknown.
Do not interpret the previous automatic `0/6` register result as evidence against 3 hooks.
Do not weaken command identity merely to reduce hook count.
