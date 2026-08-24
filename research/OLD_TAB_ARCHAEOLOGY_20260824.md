# CSGOVR - OLD TAB ARCHAEOLOGY 2026-08-24

Purpose: recover exact facts from older tabs / Library artifacts and correct stale verdict labels without disturbing the current T0 GPU-transport serial gate.

## Writer -> same CViewSetup -> next RenderView - already imported
Recovered artifact:
`CSGOVR_BREAKTHROUGH_WRITER_TO_RECLASS_RESULT_20260823_040106.zip`.

Exact chain:
`client.dll+0x0038D123 -> CViewSetup.origin.x+0xB8 -> next client.dll+0x003A5280`.

The returned result recorded the same CViewSetup pointer at writer and RenderView and unchanged raw origin coordinates across that edge. This fact is already represented in current project state and is not promoted again as a new discovery.

## Origin.x=0 one-field control - infrastructure only
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

## Final-pose yaw one-field control - strong lineage/persistence support, terminal scientific verdict not emitted
Recovered artifact:
`CSGOVR_CAUSAL_YAW_FINAL_POSE_CONTROL_RESULT_20260823_071600.zip`.

Frozen target:
`client+0x0038D15B -> CViewSetup.yaw+0xC8 -> client+0x0038D1DE -> g_vecRenderAngles.y+0x05245AB8 -> next RenderView+0x003A5280`.

Predetermined mutation:
raw float `0x43090000` = `137.0f`, changing only the yaw field.

Observed exact runtime sequence in `yaw_control.log`:
- final-pose seam hit with one CViewSetup pointer;
- normal yaw raw `0x42280000` = `42.0f`;
- one nudge changed yaw to `0x43090000` = `137.0f`;
- commit `client+0x0038D1DE` executed `movss` into the global yaw location;
- source yaw raw at commit was `0x43090000`;
- global yaw after commit was `0x43090000`;
- next true `RenderView+0x003A5280` hit used the **same CViewSetup pointer**;
- that next RenderView snapshot still contained yaw `137.0f`;
- global yaw still matched the same mutated raw value.

Snapshot integrity:
- all snapshots are full `0x198`-byte CViewSetup images;
- before -> after mutation differs only within yaw bytes `+0xC8..+0xCB` (only two byte values physically differ because the unchanged zero bytes of the float remain zero);
- `03_next_RenderView.bin` contains the mutated yaw raw value unchanged.

Why this is **not** promoted to final scientific PROVEN:
The actual runtime log stops after writing the `OBSERVED:` line and does not contain the script's terminal `VERDICT: PROVEN` marker. The generated `BREAKTHROUGH_SUMMARY.txt` deliberately records:
`VERDICT: INFRASTRUCTURE`.

Therefore classification remains:
**STRONG EXACT LINEAGE / ONE-FIELD PERSISTENCE SUPPORT / INFRASTRUCTURE, NOT FINAL CAMERA-OWNERSHIP PASS.**

Reusable exact facts:
- `CViewSetup+0xC8` is yaw in this frozen retail contract;
- `client+0x0038D1DE` commits that yaw into the retained global yaw location;
- a bounded one-field yaw mutation can survive that commit and persist into the next true RenderView on the same CViewSetup object.

Do not reinterpret this result as proof that this seam is the final headset-visible HMD ownership point. Current T0 transport remains the serial gate, and origin/head work stays downstream.

## client+0x3A0333 scoped owner-slot path - stale FALSIFIED labels corrected
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