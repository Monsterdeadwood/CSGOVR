# CSGOVR — OLD TAB ARCHAEOLOGY 2026-08-24

Purpose: recover exact facts from older tabs / Library artifacts and correct stale verdict labels without disturbing the current T0 GPU-transport serial gate.

## Writer -> same CViewSetup -> next RenderView — already imported
Recovered artifact:
`CSGOVR_BREAKTHROUGH_WRITER_TO_RECLASS_RESULT_20260823_040106.zip`.

Exact chain:
`client.dll+0x0038D123 -> CViewSetup.origin.x+0xB8 -> next client.dll+0x003A5280`.

The returned result recorded the same CViewSetup pointer at writer and RenderView and unchanged raw origin coordinates across that edge. This fact is already represented in current project state and is not promoted again as a new discovery.

## Origin.x=0 one-field control — infrastructure only
Recovered artifact:
`CSGOVR_CAUSAL_ORIGINX_ZERO_CONTROL_RESULT_20260823_043045.zip`.

Target:
`client.dll+0x0038D123 -> CViewSetup.origin.x+0xB8 -> next client.dll+0x003A5280`.

Exact snapshots are 0x198 bytes each.

Byte comparison:
- `01_after_original_writer.bin -> 02_after_originx_zero.bin`: exactly 4 bytes changed, offsets `0xB8..0xBB` only;
- original `origin.x = -857.507019...` (`0xC4566073` raw little-endian bytes `73 60 56 C4`);
- mutated `origin.x = 0.0`;
- `03_at_next_RenderView.bin` still contains `origin.x = 0.0` while unrelated CViewSetup bytes such as FOV changed.

However, the returned `control.log` does not contain a validated WRITER HIT / NEXT TRUE RENDERVIEW HIT sequence; it ends in foreign-breakpoint noise. The artifact's own post-summary therefore records `VERDICT: INFRASTRUCTURE`, not a scientific causal PASS.

Classification:
**INFRASTRUCTURE / FIELD-ISOLATION BYTE PROOF / SNAPSHOT PERSISTENCE SUPPORT / NOT FINAL CAMERA-OWNERSHIP PROOF.**

This agrees with the current rule that origin/yaw wrappers are retained infrastructure only.

## client+0x3A0333 scoped owner-slot path — stale FALSIFIED labels corrected
Recovered artifacts:
- `CSGOVR_TTD_WORLDLIST_OWNERSHIP_RESULT_20260823_024720.zip`
- `CSGOVR_TTD_WORLDLIST_OWNER_SLOT_CONTEXT_RESULT_20260823_041017.zip`

Both old `BREAKTHROUGH_RESULT.txt` files wrote `TYPE/VERDICT: FALSIFIED`, but neither scientific prediction actually executed successfully:
- first failure: debugger/query argument-type mismatch (`Типы аргумента не совпадают`);
- second failure: `CDB replay watchdog exceeded 25 minutes`.

Under the current evidence protocol these are **tool/harness failures, not target falsifications**.

### Exact-retail static code retained
The first result nevertheless preserved exact loaded-retail disassembly around `client+0x3A0310`:

```asm
+0x3A0310  push ebp
+0x3A0311  mov  ebp,esp
+0x3A0315  mov  edi,ecx
+0x3A0317  call +0x3B30C0
+0x3A031C  mov  eax,[edi+4]
+0x3A031F  mov  ecx,[ebp+8]        ; arg1
+0x3A0322  mov  esi,[eax+0x4C0]    ; save old slot
+0x3A0328  mov  [eax+0x4C0],ecx    ; publish arg1 into scoped slot
+0x3A032E  mov  eax,[ecx]
+0x3A0330  call [eax+0x0C]         ; virtual call on arg1
+0x3A0333  mov  eax,[edi+4]
+0x3A0337  mov  [eax+0x4C0],esi    ; exact restore
```

Therefore the following is STATIC PROVEN for exact frozen retail:
- owner/container root begins at `this/EDI`, with container pointer `[this+4]`;
- `container+0x4C0` is a temporary scoped pointer slot;
- arg1 is published into that slot for one virtual call and the previous value is restored immediately afterward.

What remains OPEN:
- the semantic class/name of `this`, container and arg1;
- whether matched LEFT/RIGHT executions use stable per-eye/world-list objects;
- whether this slot is useful for future culling ownership work.

Classification:
**STATIC PROVEN / RUNTIME OWNER ROLE OPEN / OLD TTD ATTEMPTS INVALID AS SCIENTIFIC FALSIFICATIONS.**

Do not record a new TTD trace merely to close this historical branch. Current project gate is T0 GPU XR transport. Revisit `+0x3A0333` only if a concrete post-transport/G3 visual culling defect makes it relevant again.

## Archaeology classification law
Recovered old artifacts must be tagged as one of:
- `PROVEN`
- `FALSIFIED`
- `MECHANICAL_ONLY`
- `INFRASTRUCTURE`
- `PREPARED/UNRUN`
- `SUPERSEDED`
- `HISTORICAL TOOLING`

Debugger syntax errors, replay watchdogs, attach failures and parser failures do not falsify the scientific target by themselves.