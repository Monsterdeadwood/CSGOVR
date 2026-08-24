# T0 Windows D3D9Ex -> D3D11 interop contract — 2026-08-24

Evidence class: **OFFICIAL PLATFORM CONTRACT + CSGOVR ENGINEERING CONSEQUENCES**.

This document narrows T0 transport. It does not claim a CSGOVR runtime PASS.

## Official Microsoft facts

### 1. D3D9Ex shared surfaces are unsynchronized

Microsoft describes Direct3D 9Ex surface sharing as **unsynchronized**. When a D3D9Ex surface is shared with D3D10/11, the application must supply manual synchronization so the consumer does not read a surface before producer GPU work has completed.

Microsoft's interop model uses a producer/consumer queue of shared surfaces rather than assuming one mutable shared texture is safe for both devices at the same time. Ownership transfers at enqueue/dequeue boundaries. Multiple surfaces allow producer and consumer work to overlap; Microsoft examples use two shared surfaces.

Reference: Microsoft Learn, `Surface Sharing Between Windows Graphics APIs`.

### 2. Cross-process D3D9 shared resources have no built-in synchronization

Microsoft Direct3D 9 documentation states that cross-process shared surfaces provide no synchronization mechanism and recommends event queries or locking for synchronization. Shared resources opened in another process must be recreated/opened with matching dimensions and `D3DPOOL_DEFAULT`.

Reference: Microsoft Learn, `Feature Summary (Direct3D 9 for Windows Vista)`.

### 3. D3D9 -> D3D11 OpenSharedResource is tightly constrained

For a texture created in D3D9 and opened by `ID3D11Device::OpenSharedResource`:

- resource is 2D;
- exactly one mip level;
- default usage / pool;
- non-MSAA;
- suitable render-target / shader-resource usage;
- only the cross-API formats corresponding to:
  - `DXGI_FORMAT_R8G8B8A8_UNORM`,
  - `DXGI_FORMAT_R10G10B10A2_UNORM`,
  - `DXGI_FORMAT_R16G16B16A16_FLOAT`
  are documented as allowed.

Reference: Microsoft Learn, `ID3D11Device::OpenSharedResource`.

### 4. Exact useful D3D9 format mapping

Microsoft legacy-format mapping defines:

- `D3DFMT_A8B8G8R8` -> `DXGI_FORMAT_R8G8B8A8_UNORM`;
- `D3DFMT_A8R8G8B8` -> `DXGI_FORMAT_B8G8R8A8_UNORM`;
- `D3DFMT_X8R8G8B8` -> `DXGI_FORMAT_B8G8R8X8_UNORM`.

Therefore `D3DFMT_A8B8G8R8` is the strongest documented 8-bit D3D9 carrier for the narrow D3D9 -> D3D11 OpenSharedResource contract.

Reference: Microsoft Learn, `Legacy Formats: Mapping Direct3D 9 Formats to Direct3D 10`.

### 5. D3D9 EVENT query is a valid producer completion fence

Microsoft's event-query pattern is:

`CreateQuery(D3DQUERYTYPE_EVENT)` -> `Issue(D3DISSUE_END)` -> poll `GetData(..., D3DGETDATA_FLUSH)` while `S_FALSE`.

The marker represents completion of GPU work before the event. Queries are lost on device loss and must be recreated.

References: Microsoft Learn, `Queries (Direct3D 9)` and `Asynchronous Notification (Direct3D 9)`.

This independently agrees with exact HLAE primitive `AfxHookSource.dll+0x000343F0`.

### 6. Legacy shared handles are lifetime tokens, not normal NT handles

DXGI legacy shared handles can be marshaled between processes, but Microsoft states they are not NT handles and must not be passed to `CloseHandle` or `DuplicateHandle`. Their validity is tied to the lifetime of the underlying video-memory resource. The creator must keep the resource alive until intended consumers have opened it.

Reference: Microsoft Learn, `IDXGIResource::GetSharedHandle`.

## CSGOVR transport consequences

### Two-slot minimum becomes an ownership protocol, not merely buffering

The already-prepared two-slot ring should be formalized per eye-resource slot:

`FREE -> PRODUCER_WRITING -> PRODUCER_FENCING -> READY -> CONSUMER_READING -> CONSUMER_FENCING -> FREE`

A slot cannot be reused by D3D9 while D3D11 may still be reading it.

Metadata per slot should include at minimum:

- `generation` / device-reset generation;
- `pairSeq`;
- eye (`LEFT` / `RIGHT`);
- same HLAE-cycle identity;
- width / height / D3D9 format;
- shared-handle token;
- producer completion state;
- consumer completion state;
- dropped / stale reason if invalidated.

### Pair coherence sits above resource ownership

Resource ownership and stereo pairing are separate invariants.

A T0 pair may publish only when:

- LEFT and RIGHT belong to the same `pairSeq` / HLAE cycle;
- both producer EVENT fences are complete;
- both resources are in `READY` state;
- neither resource belongs to an older Reset/device generation.

The x64 host may submit only the coherent pair. Never construct `L(N)+R(N+1)` because one slot happened to become ready first.

### Producer-side fence

After the eye render / GPU carrier operation is queued:

1. issue the HLAE-proven EVENT query;
2. wait until GPU completion;
3. only then transition slot to `READY`.

Publishing a handle before this completion is insufficient because D3D9Ex cross-API sharing is explicitly unsynchronized.

### Consumer-side completion

After D3D11 has copied the shared source into its private/XR destination, CSGOVR should not immediately return the slot to the D3D9 producer unless GPU completion of that consumer read is known.

Recommended protocol inference:

- use a D3D11 EVENT query (or an equivalently proven GPU completion primitive) after the copy;
- transition `CONSUMER_READING -> FREE` only after completion.

This is an engineering inference from Microsoft's unsynchronized ownership model, not yet a CSGOVR runtime-proven fact. `Flush` should not be promoted as equivalent to GPU completion without proof.

### Keyed mutex is not the default assumption

Do not assume a D3D9-created legacy shared texture exposes `IDXGIKeyedMutex`. Microsoft's D3D9Ex path is described as unsynchronized and requiring manual synchronization; keyed-mutex semantics belong to separately created synchronized DXGI shared resources.

T0 should therefore prefer explicit pair/slot state + GPU event fences unless a real imported resource proves keyed-mutex support.

### Handle lifecycle

Treat the D3D9 shared handle as an opaque legacy token:

- do not `CloseHandle` it;
- do not `DuplicateHandle` it;
- keep the producer texture alive while the consumer may use/open it;
- invalidate all tokens on D3D9 Reset/device-generation change;
- recreate resources and republish fresh metadata after reset.

## A1a vs A1b consequence

### A1a direct

Remain eligible only if the authoritative eye/replacement RT is:

- non-MSAA;
- directly representable by the documented D3D9 -> D3D11 allowed format set;
- actually creatable/openable on the frozen devices.

### A1b GPU carrier

If the source eye RT is `A8R8G8B8`, `X8R8G8B8`, MSAA, or otherwise outside the direct contract, retain the source as authoritative but resolve/convert on GPU into a non-MSAA `D3DFMT_A8B8G8R8` one-mip DEFAULT shared carrier.

This maps exactly to `DXGI_FORMAT_R8G8B8A8_UNORM`, which is inside the documented D3D9 -> D3D11 OpenSharedResource format set.

The actual `StretchRect`/conversion capability remains a runtime device-cap fact and must be tested rather than assumed.

## Revised T0 synchronization acceptance

A future transport PASS should record:

- at least two reusable transport slots or an equivalent ownership-safe queue;
- explicit slot state transitions;
- no producer reuse before consumer completion;
- producer EVENT fence completion before READY;
- same-generation handle/resource identity;
- same-cycle LEFT/RIGHT `pairSeq`;
- `mixedPairCount = 0`;
- dropped/stale pair accounting;
- reset invalidation behavior;
- no `CloseHandle`/`DuplicateHandle` treatment of legacy resource tokens.

This contract strengthens T0 without changing projection, origin, camera, culling, viewmodel or HUD.