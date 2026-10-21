# Building the included sources

Use the supplied release ZIP to play. Rebuilding produces a new candidate and
does not inherit the recorded test results of the supplied binaries.

The included build scripts require Python 3, LLVM `clang++` and `lld-link`.
They search `PATH`, then `C:\Program Files\LLVM\bin` on Windows.
Run these commands from a separate working copy of the repository:

```powershell
python scripts/build.py --universal
python scripts/build_hosts.py
```

The first command builds `bin/nova_scene32_universal.dll` for x86.
The second builds `bin/nova_xr64_palm.exe` and `bin/nova_xr64_sim_palm.exe`
for x64, and refreshes `DIAGNOSTICS_SHA256.json`.
Both write intermediate output below `_build/`.

These commands come from static inspection of the supplied scripts. They
have not been executed as part of this repository import. The scripts do
not rebuild every bundled dependency or executable; the release includes
the remaining supplied files. A compiler version sufficient to establish
bit-for-bit reproducibility is not pinned in this kit.

The release manifest describes the original package. After rebuilding,
its recorded binary hashes and validation status no longer certify the
new outputs. Keep the original ZIP for comparison and installation.
