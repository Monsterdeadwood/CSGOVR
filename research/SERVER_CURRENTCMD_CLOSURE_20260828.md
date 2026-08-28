RESEARCH_DOC_CLASS: CONVERGENCE
EVIDENCE_ID: CSGO-EV-SERVER-CURRENTCMD-CLOSURE-20260828
CLAIM_ID: CSGO-SERVER-CURRENT-CMD-OWNER

# CSGOVR — SERVER CURRENT CUserCmd CLOSURE

Snapshot: 2026-08-28

Result: `CSGOVR_FX_FIRE_OWNERSHIP_CLOSURE_RESULT_20260828_201156.zip`

## FROZEN TARGET

- legacy CS:GO x86, AppID `4465480`, BuildID `22100934`
- `server.dll` SHA256 `699757BB5C8DDB71193F68DABA22F59F697491DDBD768BC8656AC2CE435C4DB8`

## VERDICT

`SERVER_CURRENT_CMD_OWNER = RETAIL_PROVEN`

`3_HOOK_SERVER_ARCHITECTURE = COLD_PROVEN`

`RUNCOMMAND_TLS_IDENTITY_CARRIER = NOT_REQUIRED`

Runtime integration is still unproven until a build/run validates the implementation. This COLD result proves the frozen ownership/lifetime contract needed to remove the server RunCommand/TLS hook from the preferred architecture.

## EXACT STARTCOMMAND OWNER

Frozen `CPlayerMove::RunCommand = server+0x00339890` preserves:

```text
ESI = player
EDI = CUserCmd*
```

At the StartCommand call:

```text
server+00339A16  EDI = [EBP+0C]        ; CUserCmd*
server+00339A1C  push EDI              ; arg2 cmd
server+00339A1D  push ESI              ; arg1 player
server+00339A23  EAX = [EBX]           ; CPlayerMove vtable
server+00339A35  call [EAX+0x10]       ; vtable slot 4
```

Both recovered CPlayerMove vtable candidates resolve slot 4 to:

```text
StartCommand = server+0x00339210
```

Exact callee provenance:

```text
server+00339213  EDX = [EBP+08]        ; arg1 player
server+00339216  ECX = [EBP+0C]        ; arg2 CUserCmd*
server+0033921F  [EDX+0x1034] = ECX
```

Therefore:

```text
player+0x1034 = current CUserCmd*
```

This is exact frozen-retail argument provenance, not source-layout inference.

## EXACT FINISH LIFETIME

The same frozen RunCommand later clears the field:

```text
server+00339E63  [ESI+0x1034] = NULL
```

The field is therefore established before authoritative command processing and cleared at command completion.

Independent frozen code also reads `player+0x1034`, stores it locally, tests it for NULL, and follows a dedicated error/assert path when absent at `server+0x0033AC6F`. This independently corroborates current-command semantics.

## AIM TRANSPORT INSIDE SAME LIFETIME

Frozen RunCommand copies the same current command's native aim vector:

```text
server+00339C5B  cmd+18 -> player+14AC
server+00339C64  cmd+1C -> player+14B0
server+00339C6D  cmd+20 -> player+14B4
```

Thus command identity and the authoritative gameplay AimDirection coexist inside the same StartCommand/FinishCommand lifetime.

## SERVER POINTER GAMEPLAY PLAYER MAP

Manual frozen disassembly review of the preceding result closed all six approved final-aim pointer consumers:

```text
58F88B  grenade        player = ESI
5A7A34  bullet A       player = EBX
5A8037  bullet B       player = EDI
5B0349  M3             player = EDI
5B37BB  NOVA/Sawedoff  player = EDI
5BC61B  XM1014         player = EDI
```

Rejected pointer consumers remain rejected:

```text
216DCA  use / interaction trace
49F492  motion-controller recoil scaling
```

## ARCHITECTURE CONSEQUENCE

Preferred server pointer hook can now prove ownership locally:

```text
approved return RVA
AND recover player from exact caller map
AND currentCmd = *(player+0x1034)
AND currentCmd != NULL
AND exact provenance ring identity matches currentCmd
AND derived server player index == ring.localPlayerIndex
=> TRUE
```

Everything else delegates to the original pointer helper.

The former server RunCommand detour + TLS capability token is no longer needed as an identity carrier.

## CURRENT PREFERRED THREE-HOOK SHAPE

```text
1 CLIENT CheckSplitScreenMimic
  write normalized RIGHT-AIM direction into current CUserCmd
  publish exact command provenance

2 CLIENT IsInputDeviceAPointer
  approved gameplay caller
  + player+0x3348 currentCmd
  + exact ring identity
  => TRUE

3 SERVER IsInputDeviceAPointer
  approved gameplay caller
  + exact caller->player register
  + player+0x1034 currentCmd
  + exact ring identity
  + localPlayerIndex identity
  => TRUE
```

`FX_FireBullets` remains stock.

## SAFETY / FAIL-CLOSED RULES

- do not globally spoof pointer/Hydra input
- do not use whole-command motion-controller mode
- do not use `IN_ATTACK` as VR aim provenance
- do not use latest controller pose at server consumption time
- freshness belongs to command creation
- identity belongs to command consumption
- full ring identity required; modulo slot match alone is invalid
- canonicalize signed zero and reject NaN/Inf before publication
- remote/bot commands must fail local-player identity
- if `player+0x1034 == NULL`, delegate to stock behavior
- unknown/rejected pointer caller always delegates to stock behavior

## EVIDENCE CLASS

The exact owner and lifetime are `RETAIL_PROVEN`.

The three-hook architecture is `COLD_PROVEN / READY_FOR_INTEGRATION`, not yet `RUNTIME_PASS`.
