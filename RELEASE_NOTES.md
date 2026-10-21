# CS:GO Legacy VR 0.12 - Experimental Release

Motion controls, right-hand tracking, weapon aiming, recoil and haptics for CS:GO
Legacy. Launch a local 5v5 match with bots through your selected OpenXR runtime.

This build improves frame buffering, reduces repeated Direct3D state changes
and records fresh VR frames separately from source FPS and XR refresh cadence.

Download **csgo 0.12.zip**, extract its single folder next to **csgo.exe**,
activate PCVR and open **RUN.bat**. The launcher offers 2560 or 2048 per eye.

Requires [the separate CS:GO Steam release](https://store.steampowered.com/app/4465480/).
The launcher includes `-insecure` and `sv_lan 1`.
**RECOMMENDED: ALSO ADD `-insecure` IN STEAM LAUNCH OPTIONS.**
Local matches with bots only; server connections are not available.

The native XR Sim check covered 30 seconds, hand movement, firing and menus.
The author then played through Meta and approved the result.
This is an experimental release, and broader headset testing is welcome.

Scope issues, audio/grenade orientation and occasional enemy visibility have
been reported and still need investigation. Controls and details are in the README.

For bug reports, attach the ZIP from **REPORTS** and include your headset,
OpenXR runtime, mod version and what happened. Thanks for testing!
