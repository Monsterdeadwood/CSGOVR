# CSGOVR State

## Status
ACTIVE.

## Current gate
Classify the proven persistent `CViewSetup` ownership path as main-world versus shadow/aux state using read-only inspection, then map the projection/FOV/frustum/culling-relevant fields on that same proven path.

## Current known state
- OpenXR/Quest 3 two-eye view/FOV acquisition is already proven input and is no longer the blocker.
- CONTROL PROVEN: `client.dll+0x0038D123` controls persistent `CViewSetup` at `client.dll+0x052632CC`.
- Proven field: `origin.x` at `CViewSetup+0xB8`.
- The controlled value reaches the downstream render boundary at `client.dll+0x003A5280` (`RenderView`) in the tested path.
- Read-only classification path/pointer: `client+0x05263788`.
- Projection/asymmetric-eye field mapping inside the proven object lineage remains OPEN.
- Main-world versus shadow/aux classification and culling-relevant view/frustum state remain OPEN.

## Operating protocol
- Stop broad analyzers now that a concrete persistent object and control instruction are proven.
- Read-only classification comes before the next intrusive mutation.
- Converge frozen-retail static lineage, HLAE/source evidence, and runtime/TTD ownership on the proven path.
- PARALLELIZE EVIDENCE, SERIALIZE MUTATION.
- Preserve the frozen known-good baseline.

## Next
1. Read-only ReClass/object inspection through `client+0x05263788`.
2. Separate main-world from shadow/aux view objects.
3. Locate projection/FOV/frustum/culling-relevant fields on the proven `CViewSetup` lineage.
4. Keep `origin.x +0xB8` as the causal anchor.
5. Only then perform the next single predicted mutation.

See `docs/SYNC.md` for the canonical cross-branch snapshot.
