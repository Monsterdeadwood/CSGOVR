# Publication status — staged, not live

**2026-10-22:** Main branch was replaced by this small *honestly labeled landing tree*. Old RC3/research source is absent from the new tree. The normal prior Git commit history is intentionally retained for audit/recovery.

The complete 0.12 F6 repository cannot be transferred from the current session by the available GitHub connector as a local binary ZIP. The user-facing package `CSGOVR_012_GITHUB_REPLACEMENT.zip` contains the full repository including unchanged accepted `repository/release/csgo 0.12.zip`, mirrored original sources and license notices, as well as root `RUN.bat` -> Publish.

Next step: On a Windows PC with Git credentials, unpack that package, run root `RUN.bat`, choose **2. Publish repository to GitHub**. The script checks hashes, clones current main, removes its tracked files in a temporary checkout, copies the complete clean release repository, makes a **normal fast-forward commit** and pushes, without force. It only acts after explicit user confirmation and leaves original local games untouched.

Then use GitHub **Releases -> Draft a new release**, tag `v0.12`, attach the *exact* `csgo 0.12.zip` and publish only after reviewing third-party redistribution rights. No automated release or public visibility change has been performed.

Expected F6 asset SHA256: `ff9be3ff7c1d65d3d14ff0286aba8bf7865cb50aacbc1a1f6c72f070041aa41e`.
