# Known issues and tester reports

This is an experimental release. The reports below have not all been reproduced
on version 0.12; the reporting tester's exact build was not established.

- **Scopes:** a tester reported an SSG scope issue and noticeable AUG scope pixelation with the eye close to the optic. The current README only describes automatic AUG/AWP optics; full scope coverage is not claimed.
- **Audio orientation:** sound may rotate with joystick/body turning rather than head movement.
- **Grenade direction:** testers have reported direction following the character or initial facing direction rather than the intended aim.
- **Enemy visibility:** enemies may occasionally disappear. Bot takeover after death was suggested as a possible trigger; the cause is unconfirmed.
- **Movement:** VR-friendly counterstrafing was requested as a possible improvement. It is not a feature of this release.

## Headsets and runtimes

The author tested this build through Meta. A tester reported smoother play with
VDXR, but that report's exact build is unknown. Historical reports included
SteamVR/PSVR2/WMR launch problems; a fresh report is needed to establish their
status in 0.12. Universal headset compatibility is not claimed.

## Reporting

Include the ZIP from **REPORTS**, your mod version, headset, OpenXR runtime,
the affected weapon/map and steps to reproduce. For invisible enemies, mention
whether you had died and taken control of a bot. For scopes, name the weapon.

Local matches with bots are the current supported mode. Online/server play
and guaranteed frame rates are outside this release's tested scope.

## Hand tracking scope

The right controller drives the weapon. The left game arm is not an independently tracked 6DoF VR hand.
