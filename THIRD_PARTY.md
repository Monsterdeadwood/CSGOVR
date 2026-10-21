# Third-party redistribution review

The accepted F6 distribution includes OpenXR SDK headers with Khronos Apache-2.0 terms, a separately licensed xr-sim diagnostic runtime, and Microsoft Visual C++ runtime files with Microsoft redistribution notices.

**Unresolved:** the diagnostic `xr-tape` DLL is bundled, but its upstream `NOTICE.md` only covers third-party OpenXR headers and does not establish a blanket license for the layer binary itself. Verify permission or create a new, retested build without it before making the repository public. Do not quietly remove it and claim unchanged F6 Meta acceptance.

The overall project license is not established and no MIT/Apache blanket license is asserted for unrelated source. Original CS:GO game executables/assets are not included.

This is an independent community project, not endorsed by Valve/Meta/Khronos.
