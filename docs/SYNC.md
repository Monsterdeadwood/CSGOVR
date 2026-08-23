# CSGOVR — STATE SYNC

Snapshot imported: 2026-08-23

## LATEST RESULT
- CONTROL PROVEN on the retail client camera path: `client.dll+0x0038D123` controls a persistent `CViewSetup` at `client.dll+0x052632CC`, including `origin.x` at `+0xB8`.
- The controlled value survives through to `RenderView` at `client.dll+0x003A5280` in the tested path.

## PROVEN
- OpenXR/Quest 3 two-eye FOV/view acquisition is already valid and is not the current blocker.
- Persistent `CViewSetup` base: `client.dll+0x052632CC`.
- `origin.x` field in the proven object: `+0xB8`.
- Retail control instruction: `client.dll+0x0038D123`.
- Downstream render consumer boundary: `client.dll+0x003A5280` (`RenderView`).
- The controlled camera-origin mutation reaches the downstream render path in the tested experiment.

## FALSIFIED / CLOSED
- Repeating broad OpenXR availability/FOV diagnostics: closed unless new evidence reopens the question.
- Broad camera analyzers after the persistent `CViewSetup` and control instruction were identified: stopped as the primary path.
- Treating valid OpenXR eye data alone as proof of correct CSGO stereo: rejected.

## CURRENT FINALISTS
1. Persistent main camera/view object at `client.dll+0x052632CC` and its proven writer/control path.
2. Read-only object/classification path through `client+0x05263788` for distinguishing main-world vs shadow/aux view state and locating culling-relevant state.

## CONCRETE TARGETS
- Writer/control: `client.dll+0x0038D123`.
- Persistent `CViewSetup`: `client.dll+0x052632CC`.
- Proven field: `origin.x +0xB8`.
- RenderView boundary: `client.dll+0x003A5280`.
- Read-only classification pointer/path: `client+0x05263788`.
- Projection/asymmetric-eye field mapping inside the proven object lineage: OPEN.
- Culling-relevant view/frustum state associated with main-world rendering: OPEN.

## CURRENT GATE
Classify the proven persistent view object as main-world versus shadow/aux state with read-only inspection, then locate the culling/projection-relevant fields on that same proven ownership path.

## NEXT
1. Read-only ReClass/object inspection through `client+0x05263788`.
2. Separate main-world view state from shadow/auxiliary view objects without mutation.
3. Converge frozen-retail static lineage, HLAE/source evidence, and runtime/TTD ownership on the proven `CViewSetup` path.
4. Identify projection/FOV/frustum fields before any new intrusive camera mutation.
5. Preserve the proven `origin.x` control as a known-good causal anchor.

## METHOD ALREADY USED
- OpenXR idle/runtime FOV acquisition and projection derivation.
- Frozen retail static mapping.
- TTD origin-writer provenance work.
- Passive runtime ownership tracing.
- Targeted causal camera-origin control experiment.
- HLAE/source lineage as supporting evidence.

## HARD CONSTRAINTS
- Stop broad analyzers now that a concrete persistent object and control instruction are proven.
- Use read-only classification before the next intrusive mutation.
- Keep frozen/known-good baselines intact.
