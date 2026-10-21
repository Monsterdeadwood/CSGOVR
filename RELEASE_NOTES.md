# CS:GO Legacy VR 0.12 — experimental

NOVA-2026.10.21-F6, the originally accepted candidate. Native two-eye OpenXR game-world output, right-hand weapon aiming, recoil/haptics, 5v5 local bots, AUG/AWP optics, 2560 or 2048 per-eye presets and local REPORTS ZIPs.

Previous limited verification: ~30-second CS:GO + XR simulator session with hand motion, native shots and menus, followed by positive author feedback on Meta PCVR. Selected-window medians in the handoff: ~107.36 source GPU pairs/s, ~69.20 fresh XR uploads/s, ~72.17 xrEndFrame/s. These are different metrics, not a sustained 72-new-frame performance guarantee.

Known limitations: other headset runtimes and all weapons unverified; reports of optics, grenade/audio direction and disappearing enemies are not version-verified. Right controller drives the weapon; not independent two-hand full-body tracking.

**Staging notice:** The actual GitHub Release ZIP has not yet been uploaded. Its expected SHA256 is `ff9be3ff7c1d65d3d14ff0286aba8bf7865cb50aacbc1a1f6c72f070041aa41e`. Full publication instructions in [PUBLISH_STATUS.md](PUBLISH_STATUS.md).
