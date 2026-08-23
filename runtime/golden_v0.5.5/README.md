# CSGOVR v0.5.5 FULL_CVIEWSETUP_FIX — ORIGINAL GOLDEN SOURCE

This directory is sourced from the original Library artifact:

`CSGOVR_HLAE_EYE_PATCH_v0.5.5_FULL_CVIEWSETUP_FIX.zip`

Original package identity:
- bytes: `17,726`
- SHA256: `20D382A537CF97AA432CA0BA94DE279EB12764B4724793C217722538BCB78A6D`

The package was materialized directly from Library and its SHA256 was recomputed before source import. This is not a reconstruction from later experiments.

## Exact source imported
The two runtime-critical C sources are byte-identical to the original ZIP payload:

- `src/eye_patch.c`
  - bytes: `9,655`
  - SHA256: `3844B2C323F1077A901581A0A82908A174F9D1DF3F4213C5EB68980B7E7D148F`
  - Git blob SHA1 after import: `54a78d9ad16511b5e4d1d643db85ee9d2ad15179`

- `src/injector.c`
  - bytes: `5,867`
  - SHA256: `E6133CEFC23460BA3AD82350803251C604AA24425474B224BEF7FEE01EA5183D`
  - Git blob SHA1 after import: `f46bd680f8dfbf48fb96585c51954ae79a93f99e`

The Git blob SHA1 values were independently calculated from the original ZIP bytes before upload and then compared with GitHub after import. Both match.

## Original package members
- `00_RUN_EVERYTHING.bat` — 585 bytes
- `ONE_CLICK.ps1` — 18,924 bytes
- `PRE_RUNTIME_RED_TEAM.txt` — 1,336 bytes
- `PROVEN_SEAM.txt` — 1,665 bytes
- `README_RU.txt` — 4,429 bytes
- `SOURCE_SANITY.txt` — 250 bytes
- `src/eye_patch.c` — 9,655 bytes
- `src/injector.c` — 5,867 bytes

See `SOURCE_SHA256.txt` for exact member hashes.

## Packaging-byte boundary
The original package uses:
- CRLF in `00_RUN_EVERYTHING.bat`;
- UTF-8 BOM + LF in `ONE_CLICK.ps1`;
- UTF-8/LF in the C sources and remaining text files.

Only files whose post-import Git blob was independently verified are labelled byte-identical here. The original ZIP remains the canonical packaging artifact.

## Frozen runtime contract
This GOLDEN uses exact HLAE 2.191.1 `CAfxStreams::DoRenderView` at RVA `0x00022340`.

Runtime eye transaction:
- pass1 + forceCache = LEFT
- pass2 + forceCache = RIGHT
- pass3 = untouched monitor
- local `CViewSetup` copy size = `0x198`
- origin = `+0xB8`
- angles = `+0xC4`
- HLAE `m_ForceCacheFullSceneState` = `this+0x30`
- HLAE `m_DoRenderViewCount` = `this+0x50`
- half eye separation = `1.2598425` Source units

The source contains exact PE and machine-byte guards for the audited HLAE binary before hooking.

## Build reproduction
See `BUILD_REPRO.md` for the exact Zig/MinHook compile shape recovered from the original one-click package.
