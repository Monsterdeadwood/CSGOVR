# Repository Data Policy

## Commit to Git
- Project-authored source code, scripts, build files and documentation.
- Small deterministic logs or excerpts needed to reproduce a finding.
- Hashes, versions, offsets, symbol names, call-site notes and compact metadata.
- Sanitized experiment descriptions, predictions and results.

## Keep out of Git history
- Retail game executables, DLLs, assets or archives.
- Large PDBs, memory dumps, RenderDoc/API captures, TTD recordings and bulk traces.
- Third-party source/binaries when redistribution rights are unclear.
- Credentials, tokens, personal data, machine-specific secrets.

## Heavy evidence
Store heavy/raw evidence locally or as an explicitly managed artifact, and commit a small manifest containing filename, hash, provenance, date and the finding it supports.

## Rule
GitHub stores the **reproducible knowledge and authored tooling**, not an uncontrolled mirror of game installations or research dumps.
