# CSGOVR FAR_WORLD Pending Sync Packet — 2026-09-08

RESEARCH_DOC_CLASS: CONVERGENCE
EVIDENCE_ID: CSGO-EV-FARWORLD-PENDING-20260908
CLAIM_ID: CSGO-PACING-PVS
FACT_STATUS: CANDIDATE / PENDING_VALIDATION
OWNER: UNBOUND
SOURCE_GLOBAL_REVISION: 3
SOURCE_MAIN_SHA: 27ec4162bee5d04d6d77768f51fa1bea680e56d5

## PRIMARY FACT
Determine whether the RC3 far-world / distant-object disappearance is caused by a CPU visibility-envelope or other late ordinary-geometry culling mismatch, while explicitly refusing to treat PVS, pixel visibility, portal behavior, or asymmetric raster as causal until exact evidence isolates the owner.

This packet is an unpromoted HANDOFF / SYNC_PACKET. It preserves candidate evidence for validation. It does not change any technical claim to PROVEN or FALSIFIED and does not bind a technical owner.

## Canonical inputs already accepted
- The executed RC3 result proves that far-world / distant-texture disappearance occurred in the all-up RC3 integration.
- The same result does not prove PVS as the cause.
- M3.2B remains the rollback authority and protected world/HMD baseline.
- AIM alignment and FAR_WORLD causality must remain separate.
- No AIM, weapon, ballistic or native-CUserCmd mutation belongs in this packet.

## Existing runtime control: T14
Historical executed result:
`CSGOVR_T14_EXTERNAL_RESULT_20260827_143839.zip`

Its own scientific gate failed because active duration was not reached. Infrastructure/transport observations were collected, but `PVS hypothesis = PROVEN` did not follow. Treat T14 as a control and incomplete causal result, not as PVS proof.

Prepared T15 material is not evidence of execution by existence alone. Keep it UNRUN unless an executed RESULT is supplied.

## COLD convergence candidates to validate
### C1 — simple shared-world ownership family is strongly weakened
Existing CSGOVR COLD analysis reports eye-local or versioned behavior for ordinary geometry stages including PVS/world-list/renderable construction and hardware occlusion view identity. It also rejects the simple models:
- one center PVS blindly reused by both eyes;
- one persistent LEFT world/renderables list reused by RIGHT;
- simple renderables job race as the current root;
- one immortal global occlusion camera/result;
- HLAE full-scene cache as ordinary world/PVS ownership.

STATUS: `CANDIDATE_CONVERGENCE / REQUIRES_PROVENANCE_VALIDATION`

### C2 — pixel-visibility identity collision is real scope evidence but not ordinary-world proof
Source-family COLD evidence indicates pixel-visibility history can key on split-screen slot plus view identity. Two same-frame VR eyes sharing slot 0 and `VIEW_MAIN` can therefore collide in query/history lifetime.

That mechanism naturally targets glow/sun obstruction, overlay and related visibility effects. It must not be silently promoted as the cause of missing ordinary distant geometry.

STATUS: `SUPPORTED_SCOPE_BOUNDARY / NOT_WORLD_ROOT_PROOF`

### C3 — portal behavior can be explained by frustum refinement without broken PVS
Source-family COLD analysis shows `ClientLeafSystem` can cull against a camera frustum and narrower portal-refined area frustums. Therefore an observation of the form:

```text
r_portalsopenall 1
-> distant objects return
```

does not uniquely imply broken PVS or wrong PVS origin. A too-narrow CPU camera visibility envelope followed by narrower area-frustum `CullBox` tests is a competing explanation.

T13/T14 variants that used extreme FOV values are contaminated controls for this question and must not be treated as clean causal proof.

STATUS: `CANDIDATE_MECHANISM`

### C4 — conservative CPU visibility envelope is an intervention candidate, not a fact
One existing COLD proposal derives a conservative symmetric visibility envelope covering approximate Quest bounds:

```text
world FOV    = 108.0 deg
world aspect = tan(54 deg) / tan(55 deg)
             ~= 0.963752996
vertical visibility FOV ~= 110 deg
```

The intended separation is:
- exact asymmetric Quest custom projection continues to own raster geometry;
- only CPU visibility-driving world FOV/aspect on the late eye-local CViewSetup copy would be widened;
- viewmodel FOV, origin, angles, near/far, Quest matrix, Weapon_ShootPosition, portal cvars, occlusion cvars, game-owned CViewSetup and monitor pass remain untouched.

STATUS: `CANDIDATE_INTERVENTION / NOT_AUTHORIZED_LIVE`

## Contradiction / negative-control requirements
Validation must preserve these counterclaims instead of selecting a favorite explanation:
- `r_portalsopenall` recovery is not sufficient evidence for PVS ownership.
- pixel visibility may be a genuine stereo-history defect without explaining ordinary world disappearance.
- asymmetric Quest projection existing in a stable ancestor is a negative control against claiming asymmetry alone caused the RC3 regression.
- a visibility-envelope candidate is not causal until exact SAFE161/RC3 lineage and current CViewSetup/frustum consumers are reconciled.

## Validation request — COLD first
Before any LIVE, close these exact edges from current frozen/current lineage evidence:
1. Reconcile SAFE161 vs executed RC3 source/hook changes that can affect world FOV, aspect, CViewSetup lifetime, frustum generation, split-screen/view identity, portal refinement, occlusion view identity or other ordinary-geometry membership state.
2. Confirm the exact path by which the late eye-local CViewSetup world FOV/aspect feed camera frustum and portal-refined `CullBox` decisions in the relevant current lineage.
3. Confirm whether any RC3-added hook or shared infrastructure can alter those inputs indirectly despite declared renderer identity.
4. Separate ordinary-world membership from pixel-visibility/glow history so a PASS in one subsystem cannot be misreported as a world-geometry fix.
5. Identify exactly one runtime-only fact remaining after this static reconciliation, if any.

## PASS for packet validation
The packet becomes `SUPPORTED` only if provenance is recovered for the cited COLD conclusions and the current SAFE161/RC3 lineage leaves one discriminating visibility owner or a sharply ranked candidate set without contradicting canonical RC3/M3.2B facts.

## FAIL / discard conditions
Discard or rewrite this packet if:
- the cited COLD conclusions cannot be tied to CSGOVR provenance;
- exact current lineage shows the proposed FOV/aspect/frustum edge is absent or irrelevant;
- another already-promoted claim/result answers the same primary fact;
- a sibling packet with the same primary fact is already ACTIVE/PENDING and this packet is duplicative.

## DO_NOT_REPEAT
- no new PVS-disable or occlusion-disable LIVE merely to rediscover ownership already attacked COLD;
- no `r_portalsopenall` result promoted as PVS proof;
- no pixel-visibility fix promoted as ordinary-world repair without scope proof;
- no AIM/weapon mutation in FAR_WORLD validation;
- no new renderer architecture before exact current-lineage diff is exhausted;
- no LIVE while this packet remains pending.

## Handoff state
```text
PACKET = CSGO-EV-FARWORLD-PENDING-20260908
CLAIM = CSGO-PACING-PVS
STATUS = PENDING_VALIDATION
OWNER = UNBOUND
LIVE = BLOCKED_BY_PENDING_SYNC
NEXT = exact current-lineage COLD validation of visibility ownership
```