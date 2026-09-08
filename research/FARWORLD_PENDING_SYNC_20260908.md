# CSGOVR FAR_WORLD Pending Sync Packet — 2026-09-08

RESEARCH_DOC_CLASS: CONVERGENCE
EVIDENCE_ID: CSGO-EV-FARWORLD-PENDING-20260908
CLAIM_ID: CSGO-PACING-PVS
FACT_STATUS: CANDIDATE / PENDING_VALIDATION
OWNER: UNBOUND
SOURCE_GLOBAL_REVISION: 3
SOURCE_MAIN_SHA: 27ec4162bee5d04d6d77768f51fa1bea680e56d5
VALIDATED_THROUGH_GLOBAL_REVISION: 4

## PRIMARY FACT
Determine whether the RC3 far-world / distant-object disappearance is caused by a CPU visibility-envelope or other late ordinary-geometry culling mismatch, while explicitly refusing to treat PVS, pixel visibility, portal behavior, asymmetric raster, or AIM-hook infrastructure as causal until exact evidence isolates the owner.

This packet is an unpromoted HANDOFF / SYNC_PACKET. It preserves candidate evidence for validation. It does not change any technical claim to PROVEN or FALSIFIED and does not bind a technical owner.

## Canonical inputs already accepted
- The executed RC3 result proves that far-world / distant-texture disappearance occurred in the all-up RC3 integration.
- The same result does not prove PVS as the cause.
- M3.2B remains the rollback authority and protected world/HMD baseline.
- AIM alignment and FAR_WORLD causality must remain separate.
- No AIM, weapon, ballistic or native-CUserCmd mutation belongs in a FAR_WORLD causal intervention.

## Existing runtime control: T14
Historical executed result:
`CSGOVR_T14_EXTERNAL_RESULT_20260827_143839.zip`

Its own scientific gate failed because active duration was not reached. Infrastructure/transport observations were collected, but `PVS hypothesis = PROVEN` did not follow. Treat T14 as a control and incomplete causal result, not as PVS proof.

Prepared T15 material is not evidence of execution by existence alone. Keep it UNRUN unless an executed RESULT is supplied.

## COLD convergence candidates
### C1 — simple shared-world ownership family is strongly weakened
Existing CSGOVR COLD analysis reports eye-local or versioned behavior for ordinary geometry stages including PVS/world-list/renderable construction and hardware occlusion view identity. It also rejects the simple models:
- one center PVS blindly reused by both eyes;
- one persistent LEFT world/renderables list reused by RIGHT;
- simple renderables job race as the current root;
- one immortal global occlusion camera/result;
- HLAE full-scene cache as ordinary world/PVS ownership.

STATUS: `CANDIDATE_CONVERGENCE / PROVENANCE_VALIDATION_PARTIAL`

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

## Validation progress — current-lineage and hook-isolation pass
### V1 — SAFE161 is a strong negative control against asymmetry/PVS-by-itself causality
Recovered lineage records state that RC3 was rebuilt from SAFE161, where far-world was visually stable, while preserving:

```text
HMD 6DoF functions = IDENTICAL
Quest projection   = IDENTICAL
D3D hooks          = IDENTICAL
xr_host.cpp         = BYTE IDENTICAL
slot250 transform   = SAME
PVS/culling mutation = NONE
```

RC3 added the native-aim hook layer on top of that ancestor. Therefore the mere existence of Quest asymmetric projection or the previously identified asymmetric area-frustum correctness issue is not sufficient to explain the RC3 far-world regression.

STATUS: `SUPPORTED_NEGATIVE_CONTROL`

### V2 — RC3 client player-aim mutation is real but no visibility edge is proven
Recovered RC3 source audit shows the client pointer gate could copy:

```text
currentCmd->aimdirection
    -> player+0x382C/+3830/+3834
```

and leave that value resident. Later frozen audit marks client player-aim mutation `DO_NOT_WRITE` because the field has consumers outside the exact shot site.

However the frozen client xref census subsequently classified actual `m_AimDirection` reads as pointer-gated or as stock player->CUserCmd producer behavior. The previously suspicious `client+0x3E67A8` read is itself downstream of `GetCurrentInputDevice -> IsInputDeviceAPointer` and only executes in pointer mode. No exact path from `player+0x382C` to `CViewSetup`, frustum generation, PVS, portal refinement, occlusion identity or ordinary-world draw membership has been recovered.

Therefore:

```text
RC3 client playerAim write = BAD AIM-SIDE EFFECT
RC3 client playerAim write -> FAR_WORLD = NOT PROVEN
```

STATUS: `SHARED_SEMANTIC_CAUSE_STRONGLY_WEAKENED`

### V3 — client IsInputDeviceAPointer override is caller-isolated away from render/view consumers
Frozen census is exhaustive at direct-call scope:

```text
CLIENT IsInputDeviceAPointer = 14 direct callers
SERVER IsInputDeviceAPointer = 8 direct callers
```

The firearm-final-aim family forms a distinct allowlisted subset. Other client callers classify as other `GetFinalAimAngle` consumers, viewmodel motion-controller transform, auxiliary aim-vector behavior, recoil-scale gate and crosshair/reticle gate. The native-aim design uses return-address gating and falls through to original for non-allowlisted callers.

RC3 source audit reports six Stage-1 paths rather than the later corrected five: five conventional bullet-firearm paths plus one grenade path. That is an AIM-scope defect, but still not a renderer/frustum caller class.

No view/frustum/PVS caller has been shown to receive forced pointer semantics from the RC3 allowlist.

STATUS: `POINTER_OVERRIDE_TO_VISIBILITY_EDGE_NOT_FOUND`

### V4 — CheckSplitScreenMimic semantic path does not itself alter render slot state in recovered design
The recovered frozen command order is:

```text
original CheckSplitScreenMimic()
    -> RC3 VR aim write to CUserCmd
    -> provenance ring write
    -> random_seed / remaining command tail
    -> verified copy / checksum
```

The seam was selected after stock mimic behavior so the stock function could first replace the command if needed. Recovered RC3 architecture describes Hook1 as writing only `cmd.aimdirection` plus provenance state. No deliberate active split-screen slot, `CViewSetup`, view identity, frustum or PVS mutation is present in the recovered semantic contract.

STATUS: `DIRECT_SPLIT_VIEW_MUTATION_NOT_FOUND`

### V5 — installation/ABI risk remains bounded but raw RC3 package is not currently surfaced
Later source audit explicitly marked:

```text
RC3_HOOK_INSTALL_TRANSACTION = NEEDS FIX
```

and later hardened designs used queued MinHook activation plus an armed latch. Frozen helper analysis also shows both `IsInputDeviceAPointer` functions have sufficient whole-instruction entry space for ordinary MinHook trampolines, and the producer seam lineage had prior x86 trampoline analysis.

But the exact raw RC3 ZIP/source file is not currently surfaced in the accessible Library as a materializable package. Therefore this META pass does **not** claim a byte-for-byte ABI proof of every executed RC3 detour/trampoline. That remaining uncertainty is implementation-level, not a recovered semantic path into FAR_WORLD.

STATUS: `RAW_EXECUTED_RC3_ABI = UNVERIFIED_IN_CURRENT_ACCESS`

## Current convergence verdict
The two-defect common-root model has narrowed:

```text
SHARED SEMANTIC ROOT
AIM owner also directly owns ordinary-world visibility
    -> STRONGLY WEAKENED

SHARED INFRASTRUCTURE ROOT
RC3 aim hooks indirectly perturb render/view/frustum state
    -> WEAKENED
    -> no semantic edge found
    -> exact raw detour ABI remains unverified

INDEPENDENT DEFECTS
AIM misalignment + FAR_WORLD visibility defect
    -> CURRENT LEADING MODEL
```

This is not yet a claim that FAR_WORLD is caused by CPU FOV/aspect. It only removes several tempting common-cause shortcuts.

## Contradiction / negative-control requirements
Validation must preserve these counterclaims instead of selecting a favorite explanation:
- `r_portalsopenall` recovery is not sufficient evidence for PVS ownership.
- pixel visibility may be a genuine stereo-history defect without explaining ordinary world disappearance.
- asymmetric Quest projection existing in stable SAFE161 is a negative control against claiming asymmetry alone caused RC3 regression.
- bad client player-aim mutation can explain AIM/prediction defects without proving visibility causality.
- caller-isolated pointer overrides must not be described as global pointer-device activation.
- a visibility-envelope candidate is not causal until current CViewSetup/frustum consumers are reconciled.

## Validation request — COLD first
Before any LIVE, close these exact edges from current frozen/current lineage evidence:
1. Prove the exact path by which the late eye-local `CViewSetup` world FOV/aspect feed camera frustum and portal-refined `CullBox` decisions in the relevant frozen/source lineage.
2. Recover exact ownership/lifetime for the ordinary-world membership structures that can remove distant renderables after PVS and before draw submission.
3. Validate or reject the claimed eye-local/versioned PVS/world-list/renderables/occlusion ownership with exact provenance.
4. Keep pixel-visibility/effects history separate from ordinary-world membership.
5. If the raw executed RC3 package becomes available, perform exact detour ABI/trampoline diff as an independent negative control, not as a prerequisite for all FAR_WORLD COLD work.
6. Only after 1-5 identify exactly one runtime-only discriminating fact, if any.

## PASS for packet validation
The packet becomes `SUPPORTED` only if provenance is recovered for the cited COLD conclusions and the current lineage leaves one discriminating ordinary-world visibility owner or a sharply ranked candidate set without contradicting canonical RC3/M3.2B facts.

## FAIL / discard conditions
Discard or rewrite this packet if:
- the cited COLD conclusions cannot be tied to CSGOVR provenance;
- exact current lineage shows the proposed frustum/world-membership edge is absent or irrelevant;
- exact raw RC3 ABI evidence reveals a concrete render/view corruption path that supersedes the independent-defect model;
- another already-promoted claim/result answers the same primary fact;
- a sibling packet with the same primary fact is already ACTIVE/PENDING and this packet is duplicative.

## DO_NOT_REPEAT
- no new PVS-disable or occlusion-disable LIVE merely to rediscover ownership already attacked COLD;
- no `r_portalsopenall` result promoted as PVS proof;
- no pixel-visibility fix promoted as ordinary-world repair without scope proof;
- no `player+0x382C` AIM-side-effect promoted as FAR_WORLD root without a real visibility edge;
- no global-pointer-spoof narrative for a return-address allowlisted hook;
- no AIM/weapon mutation in FAR_WORLD validation;
- no new renderer architecture before ordinary-world ownership is exhausted COLD;
- no LIVE while this packet remains pending.

## Handoff state
```text
PACKET = CSGO-EV-FARWORLD-PENDING-20260908
CLAIM = CSGO-PACING-PVS
STATUS = PENDING_VALIDATION / NARROWED
OWNER = UNBOUND
LIVE = BLOCKED_BY_PENDING_SYNC
LEADING_MODEL = INDEPENDENT AIM + FAR_WORLD DEFECTS
NEXT = exact ordinary-world visibility ownership: late CViewSetup -> frustum -> area CullBox -> renderable membership
```