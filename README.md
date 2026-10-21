# CS:GO Legacy VR 0.12

**Experimental native OpenXR VR mod for the separate frozen legacy CS:GO x86. Not CS2.**

This repository's former RC3/research files have been removed from the current main tree. **The full F6 source and binary archive are not uploaded to GitHub yet**. The canonical accepted release archive is `csgo 0.12.zip`, SHA256 `ff9be3ff7c1d65d3d14ff0286aba8bf7865cb50aacbc1a1f6c72f070041aa41e`. The complete replacement project and an authenticated Git upload utility are supplied separately in the ChatGPT delivery ZIP; that tool will atomically replace this temporary staging tree with the complete source and release file.

### Features

- Native binocular in-game rendering sent to OpenXR.
- Right-hand tracked-controller aiming and stock recoil/haptics.
- Local 5v5 match with nine bots, game/buy menus, AUG/AWP optics.
- 2560 or 2048 pixels per eye (presets).

### Installation (after the release ZIP is uploaded)

You need a separate copy of [legacy CS:GO on Steam](https://store.steampowered.com/app/4465480/). Download **csgo 0.12.zip** (not GitHub's automatic source ZIP), unpack its single `put me in a game folder` directory beside `csgo.exe`, activate PCVR and launch the mod folder's `RUN.bat`. The mod uses `-insecure` / `sv_lan 1` and only supports local bots. CS2, secure matchmaking and online servers are unsupported.

**This is not yet a published GitHub Release.** Do not circulate a download claim until the exact ZIP has been uploaded and third-party redistribution rights reviewed.

The F6 author gave positive feedback after a Meta PCVR session. This does not certify all headsets, weapons or 72 new game FPS. F6F1 is a separate test candidate, not this release. The left game arm is not independently 6DoF tracked.

[Known issues](KNOWN_ISSUES.md) · [Release notes](RELEASE_NOTES.md) · [Publishing status](PUBLISH_STATUS.md) · [Third-party rights](THIRD_PARTY.md)

Independent community project. Not affiliated with Valve, Steam or Meta.
