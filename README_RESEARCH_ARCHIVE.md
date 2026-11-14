# CS:GO Legacy VR 0.12

Experimental VR mod for CS:GO Legacy with motion controls, right-hand tracking,
weapon aiming, recoil and haptics. The current release starts a local 5v5
match with nine bots through your selected OpenXR runtime.

## Download and install

1. Install the separate [CS:GO Steam release](https://store.steampowered.com/app/4465480/).
2. Download [**csgo 0.12.zip**](release/csgo%200.12.zip?raw=1). The same package can be attached to a [GitHub Release](https://github.com/Monsterdeadwood/CSGOVR/releases).
3. Extract the archive and put its **put me in a game folder** folder next to **csgo.exe**.
4. Activate your headset's PCVR session and select its OpenXR runtime.
5. Open **RUN.bat** inside the mod folder. Choose **1** for 2560 per eye or **2** for 2048.

Close any other game before launching. CS2 is not supported.
The launcher uses the system OpenXR runtime without changing it.

Requires Windows x64 and the supported x86 game build. The launcher checks
the five exact game-file hashes listed in [TARGET.json](TARGET.json);
compatibility with other game updates has not been established.

**RECOMMENDED: ADD `-insecure` TO THE GAME'S STEAM LAUNCH OPTIONS AS WELL.**

The included launcher already adds `-insecure` and `sv_lan 1`.
This release is for local matches with bots; server connections are not available.

## Controls

| Input | Action |
| --- | --- |
| Right trigger | Fire |
| Left trigger | Use / interact (E) |
| Right grip | Toggle buy menu |
| Left grip | Hold for quiet walk |
| A | Jump |
| X | Reload |
| B | Cycle weapons |
| Y | Drop one weapon immediately |
| Left stick | Move; click for scoreboard |
| Right stick | Turn; push down for quiet walk; click to crouch |
| Left menu button | Short press to recenter; hold 0.7 seconds to toggle the game menu |

AUG/AWP optics are active automatically. Mouse movement does not rotate the VR camera.

## Compatibility and known issues

The author has played and approved this build through Meta. A tester also
reported good results with VDXR; broader headset compatibility still needs testing.
Performance varies by PC, headset, runtime and resolution.
The right controller drives the weapon. The left game arm is not an
independently tracked 6DoF VR hand.

Some scopes, audio/grenade orientation and occasional enemy visibility need
further investigation. See [known issues](KNOWN_ISSUES.md) for the scope of those reports.

## Bug reports

A diagnostic ZIP is saved locally in **REPORTS** after the session ends.
You can also run `RUN.bat --report`.

Attach that ZIP to your issue and include your headset model, OpenXR runtime,
mod version and steps to reproduce the problem. Reports are not uploaded automatically.

## Source and notices

The release archive includes native sources in `src/`, build and launch scripts
in `scripts/`, dependencies and their notices in `deps/` and `licenses/`.

## Release files and source

This repository contains the supplied **0.12 / NOVA-2026.10.21-F6** package
and its 101 extracted files. The packaged binaries and sources are unchanged.
The ZIP is the installable release; its checksum is in
[release/SHA256SUMS.txt](release/SHA256SUMS.txt).

- [Release notes](RELEASE_NOTES.md)
- [Recorded validation and its limits](docs/VALIDATION.md)
- [Building the included sources](docs/BUILDING.md)
- [Third-party notices](THIRD_PARTY.md)
- [Publication status](PUBLISH_STATUS.md)
- [Русская инструкция](README_RU.md)

The release kit records a later Meta play session than the `PENDING` headset
status inside the sealed ZIP. See the validation notes for that distinction.
