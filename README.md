> [!IMPORTANT]
> **[TEST BUILDS & DISCUSSION · FLAT2VR DISCORD](https://discord.gg/P4q9zt7ek)**
> Testing, build requests, feedback and discussion happen in the **Flat2VR Modding Discord**. Join the server and find the **NOVA · CS:GO Legacy VR** thread.

<p align="center">
  <img src="assets/vr-banner.png" alt="NOVA · Counter-Strike: Global Offensive (Legacy) VR" width="710">
</p>

# NOVA · Counter-Strike: Global Offensive (Legacy) VR

[Flat2VR Discord](https://discord.gg/P4q9zt7ek) · [All projects](PORTFOLIO.md)

![Project status](assets/vr-status.svg)

NOVA is a VR mod for the Legacy version of Counter-Strike: Global Offensive. The 0.12 release adds head tracking, a controller-driven weapon, recoil and haptics to local 5v5 matches with nine bots through OpenXR.

**Package baseline on this page: 0.12 / NOVA-2026.10.21-F6.**

## Installation

1. Install the supported [CS:GO Steam release](https://store.steampowered.com/app/4465480/). This package targets CS:GO Legacy.
2. Download [**csgo 0.12.zip**](release/csgo%200.12.zip?raw=1) and extract it.
3. Put the complete **put me in a game folder** folder next to **csgo.exe**.
4. Connect PC VR, select its OpenXR runtime and open **RUN.bat** in the mod folder. Choose 1 for 2560 per eye or 2 for 2048.
5. Use local bot matches. The launcher applies **-insecure** and **sv_lan 1**; also add -insecure to the game’s Steam launch options.

## What this project brings to VR

- Tracked head movement and right-controller weapon aiming.
- Weapon recoil and controller vibration.
- Automatic AUG/AWP optics in the released branch.
- A local 5v5 bot-match setup, recenter and VR-accessible game menus.
- Two resolution choices and a local diagnostics ZIP after the session.

<details>
<summary><b>Controller quick reference</b></summary>

| Input | Action |
| --- | --- |
| Right trigger / left trigger | Fire / interact |
| Right grip / left grip | Buy menu / hold quiet walk |
| A / X | Jump / reload |
| B / Y | Cycle weapons / drop weapon |
| Left stick / click | Move / scoreboard |
| Right stick / click | Turn / crouch; stick down for quiet walk |
| Left Menu tap / hold | Recenter / game menu after 0.7 seconds |

The complete package control sheet takes precedence for its version and interaction profile.

</details>

## Features in newer test builds

- **Throw grenades by moving your hand.** In a local test, the tracked right hand drove the game's throw for six grenade types, including smoke, flashbang and Molotov. XR Sim exercised moving throws, obstacles and reacquiring tracking.
- **Seat a magazine by hand.** The development build tested the reload cycle with either hand on four original weapon models. The approach must match the magazine well before the magazine seats. These tests used XR Sim. Physical-headset handling remains open.

## Current status

**Playable test release.** The released F6/0.12 build has author-reported Meta play acceptance. A tester also reported good results on VDXR; wider device coverage remains limited.

- The released left arm is not an independently tracked 6DoF hand. Some scope, audio/grenade orientation and enemy-visibility reports remain open.
- This release targets local bot matches on its pinned CS:GO build; CS2 and server connections are outside its scope.

A separate private development branch explores two-hand/either-hand carry, physical magazines, native finger grips, expanded optics and recoil. These changes do not alter the preserved F6 release.

[Detailed status and validation scope](docs/PROJECT_STATUS.md)

## Testing & feedback

Post in the game’s [Flat2VR thread](https://discord.gg/P4q9zt7ek) with your **mod version, game/store version, headset, OpenXR runtime, GPU, reproduction steps and the package’s diagnostic ZIP**. A short video is helpful for visual or handling problems.

<!-- COMMUNITY_TESTERS_START -->
## ❤️ Community testers — thank you!

These VR projects improve thanks to the people sharing test reports, logs, videos and practical feedback across the portfolio.

alvaro · Arkturus [CODA] · Arkturus Primacronus [HD2M] · Arthur G [ORE] · Bestgamer3 · Chtiblue · creative name [SC℧M] · Derp Dog · DNana51 [R1Δ] · Gritt [F2VR] · headbangerle. · huligan01 · jeremiasadasmki · Kerin\_Serica [1984] · kilo · Lawyer [WYRD] · little\_boxbr · Lord Beardsteak · MeWhenINameMyself · MetalikSix · neoz\_1980 · Notagameaddict · Pelmeschka · Potato Pancake Person · ReseTV · Rusty Gere · SilentBang [LTS] · SilentVortigaunt · Terry · ThE LaNd GuLl · Thomas-3D-VR · Uruz [F2VR] · Willthethrill · wxorys\_ · Xavier H. G.

[Full community credits, games, feedback contributors and testing volunteers](TESTERS.md).

The linked list shows which games each person helped with. Thank you as well to everyone whose reports arrived without a public display name.

<!-- COMMUNITY_TESTERS_END -->

## Other VR mods by Monsterdeadwood

| Game / project | Focus | Visibility |
| --- | --- | --- |
| [Assassin’s Creed VR](https://github.com/Monsterdeadwood/AC1VR) | First-person exploration, tracked climbing and sword interaction | Private |
| [Crysis 2: VRevamp](https://github.com/Monsterdeadwood/C2VR) | Original Crysis 2 in VR with tracked weapons, physical melee and a spatial HUD | Public |
| [Star Wars: The Force Unleashed II VR](https://github.com/Monsterdeadwood/TFU2VR) | Tracked dual lightsabers, telekinesis and Force powers | Public |
| [TimeShift VR](https://github.com/Monsterdeadwood/TimeShiftVR) | Controller-driven time powers, tracked weapons and roomscale posture | Public |
| [You Are Empty: Absolute Edition VR](https://github.com/Monsterdeadwood/YouAreEmptyAEVR) | Absolute Edition with tracked weapons and melee with either hand | Public |
| [Alone in the Dark (2008) VR](https://github.com/Monsterdeadwood/AloneInTheDark2008VR) | Tracked items, wall contact and the original throw controls | Public |
| [Rainbow Six Siege VR](https://github.com/Monsterdeadwood/SiegeVR) | Shadow Legacy stereo and two-handed weapon handling | Public |
| [Burnout Paradise Remastered VR](https://github.com/Monsterdeadwood/BUrnOutPvr) | Head-tracked stereo driving, vehicle menus and boost effects | Public |
| [Titanfall 1 VR](https://github.com/Monsterdeadwood/Titanfall1VR) | R1Delta pilot aiming, stereo viewing and Titan cockpits | Public |
| [Metro 2033 VR](https://github.com/Monsterdeadwood/Metro2033VR) | Original Metro 2033 with tracked interaction and PhysX experiments | Public |
| [Insurgency VR](https://github.com/Monsterdeadwood/InsurgencyVR) | Early VR mod project for stereo rendering and the game camera | Private |
| [Counter-Strike: Source VR](https://github.com/Monsterdeadwood/CSSVR) | Early VR mod project for stereo rendering and head tracking | Private |

Private repository links are available to invited collaborators. Public project pages are marked above.

## Credits & development notes

Project maintained by [Monsterdeadwood](https://github.com/Monsterdeadwood). This is an independent fan VR project; you need your own supported copy of the original game.

[Release notes](RELEASE_NOTES.md) · [Validation](docs/VALIDATION.md) · [Building](docs/BUILDING.md) · [Known issues](KNOWN_ISSUES.md) · [Third-party notices](THIRD_PARTY.md)

[Previous repository overview, preserved as history](README_RESEARCH_ARCHIVE.md).

Documentation reviewed: **14 November 2026**.
