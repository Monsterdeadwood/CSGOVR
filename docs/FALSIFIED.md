# CSGOVR — FALSIFIED / CLOSED / STOPPED

Canonical source: `CSGOVR_KNOWLEDGE_BASE_CANONICAL_20260823.md`, augmented by the 2026-08-24 cross-branch sync.

## Architectural / scientific falsifications
- **Full `CViewSetup=0x14C`** — SUPERSEDED. Exact HLAE copy ctor proves `0x198`.
- **`client+0x0525BA60` reused inside BOTH LEFT and RIGHT eye marker regions** — FALSIFIED. Region is real; observed accesses were outside eye brackets.
- **Persistent helper arg2 output beyond first `0x40` bytes (`+0x40..+0x1FF`)** — FALSIFIED.
- **Earlier auto-generated FALSIFIED verdict for the first `0x40` transpose** — INVALID/SUPERSEDED. Temporary in-place transpose/restore is PROVEN.
- **OriginX/yaw mutation wrappers as causal camera ownership proof** — REJECTED. They passed byte-integrity but are INFRASTRUCTURE only.
- **Assuming symmetric eye projection is sufficient for Quest 3** — FALSIFIED by measured asymmetric FOV.
- **Old off-center identity `[-1,+1]`** — FALSIFIED; normalized Source semantics are the retained interpretation.

## Exact late-origin negative result — scope must stay narrow
`CSGOVR_NATIVE_STEREO_SEAM_RESULT_20260821_041928` showed that one tested late DUAL_ORIGIN intervention did not authoritatively change that final captured raster.

This **does not** falsify eye translation generally because independent evidence exists:
- actual v0.5.5 GOLDEN BMPs show depth-dependent L/R parallax;
- stock `WORLD_X_PAIR` causal work proved a small RenderView origin translation can reach GPU c2, pixels and draw membership.

Therefore the falsified statement is only: **that exact late intervention/capture was not an authoritative final eye-origin control point**.

## Closed experiment classes / do not repeat
- broad OpenXR FOV/view rediscovery while the current Quest XR baseline remains valid
- proving again that two world streams exist
- proving again that `CViewSetup.origin` is live
- proving again that frozen `RenderView client+0x003A5280` exists
- sign/transpose/row-vs-column projection roulette already closed by v0.6.1 causal semantics
- sequential screenshots as quantitative disparity/culling truth; temporal drift can dominate
- treating compositor submission alone as proof of correct stereo geometry
- treating two renders alone as proof of correct culling
- forcing world projection blindly onto viewmodel
- coupling HMD camera ownership to weapon aim
- building a replacement renderer before exhausting the Source view path
- old broad TAB3 batch of 8–12 origin write points **until a concrete post-G3 eye-origin defect exists**
- rebuilding a broad analyzer after a concrete target already exists

## Invalid / infrastructure failures that are NOT scientific falsifications
- `CSGOVR_FULL_EXPERIMENTAL_FAILURE_20260821_022832.zip` — cross-project Crysis2VR source/root contamination + build workflow failure; proves nothing about CSGOVR renderer/XR behavior.
- v0.6.5 end-to-end XRFOV application failure — broken path wiring (`g_xrFovPath` empty/uninitialized, stale control path vs PowerShell INI); did not falsify XR, stereo or matrix math.
- compile failures caused by removed `HALF_EYE_SEP` / `DEG2RAD`, missing `-municode`, stale PID/gameplay-gate races, parser/tooling errors — infrastructure only unless the scientific prediction was actually exercised.

## Retired speculative camera ownership promotions
Do not treat these as PROVEN ownership:
- `client+0x0038D123`
- `CViewSetup.origin +0xB8` writer-window alone
- `client+0x052632CC`
- `client+0x05263384`
- yaw `client+0x0038D15B`, `+0x0038D1DE`, `+0x05245AB8`

They remain useful infrastructure/concrete anchors only.

## STOP rule at current gate
G3 is projection integration. Do not open culling/origin/head/weapon reverse work before a G3 runtime/headset verdict supplies a concrete visible blocker.
