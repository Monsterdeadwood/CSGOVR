# CSGOVR - recovered SetUpView finalization lineage

Date: 2026-08-24 old-tab archaeology import.

Returned artifact:
`CSGOVR_COLD_SETUPVIEW_LINEAGE_RESULT_20260823_065304.zip`

Classification:
**EXACT-RETAIL COLD LINEAGE PROVEN.**

This does not alter the current T0 GPU-transport gate. It is retained for future post-T0/G3 origin/head/culling work only.

## Frozen target and returned verdict
The old mapper targeted the final SetUpView region around:
- `client+0x0038D08C`
- already-used `client+0x0038D123`
- `client+0x0038D1F5`
- persistent CViewSetup global `client+0x052632CC`
- render-origin global family beginning `client+0x05245AA8`.

Returned summary:
- `D08C=True`
- `D123=True`
- `D1F5=True`
- `renderGlobalRefInFinalization=True`
- `viewGlobalRefs=1`
- `renderGlobalRefs=36`
- `setupViewStringXrefs=1`
- `VERDICT: PROVEN`.

## Exact finalization window facts
Selected exact-retail instructions:

```asm
+0x0038D08C  CALL +0x0038B740
+0x0038D091  MOVSS XMM0,[EDI]
...
+0x0038D115  MOVSS XMM0,[EBP-0x30]
+0x0038D11A  LEA EDX,[EBP-0x30]
+0x0038D11D  MOV ECX,[global]
+0x0038D123  MOVSS [EDI],XMM0
+0x0038D12C  MOVSS [EDI+0x04],XMM0
+0x0038D138  MOVSS [EDI+0x08],XMM0
...
+0x0038D142  MOVSS [EBX],XMM0
+0x0038D14B  MOVSS [EBX+0x04],XMM0
+0x0038D156  MOVSS [EBX+0x08],XMM0
+0x0038D15B  CALL [EAX+0x4C]
...
+0x0038D170  MOVQ XMM0,[client+0x05245AA8]
...
+0x0038D1AB  MOVSS [client+0x05245AA8],XMM0
+0x0038D1B8  MOVSS [client+0x05245AAC],XMM0
+0x0038D1C5  MOVSS [client+0x05245AB0],XMM0
+0x0038D1D1  MOVSS [client+0x05245AB4],XMM0
+0x0038D1DE  MOVSS [client+0x05245AB8],XMM0
+0x0038D1ED  MOVSS [client+0x05245ABC],XMM0
+0x0038D1F5  MOV byte ptr [EAX+0x150],0
```

This exact window is consistent with the independently recovered old controls:
- origin one-field seam at `+0x0038D123`;
- yaw/final-pose seam around `+0x0038D15B`;
- global yaw commit at `+0x0038D1DE`;
- same CViewSetup can reach next true `RenderView+0x003A5280` in the bounded infrastructure controls.

## Persistent/global references recovered
The cold mapper found the persistent CViewSetup global reference:
- `client+0x052632CC` referenced at `client+0x003A0381`.

Candidate render-origin global family:
- base `client+0x05245AA8`;
- 36 static references in exact retail;
- references inside the finalization region include `+0x0038D174` and `+0x0038D1AF` family positions.

The old mapper also retained post/follow-up candidates including `+0x0038E5D7` and `+0x0038EC08`.

Important scope boundary:
Those later candidate references are **not final ownership proof**. Current project state correctly closes/rejects broad static post-SetUpView candidate chasing unless a concrete post-G3 defect requires it.

## HLAE/source bridge support
The result also recovered one exact SetUpView-string xref and a CSGO view-pattern/global-pointer context from the pinned HLAE/source side.

This is source/lineage support only. Exact frozen-retail causal evidence remains higher authority.

## Future use rule
Do not reopen this branch during T0 transport certification.

After T0 and G3, only if a visible origin/head/culling defect remains:
1. begin from this exact finalization window and the already-proven one-field persistence controls;
2. ask one concrete ownership question;
3. do not restart broad SetUpView archaeology;
4. preserve v0.5.5 pass schedule and frozen transport/projection baselines.
