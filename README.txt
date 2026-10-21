CS:GO Legacy VR - experimental F6 build

Put this whole folder next to csgo.exe (Steam app 4465480), then open RUN.bat.
Select 1 for 2560 per eye, or 2 for 2048. Activate your PCVR headset first.
Uses your selected system OpenXR runtime; the launcher does not change it.
Starts a local 5v5 match with nine bots. -insecure and sv_lan 1 are included.
RECOMMENDED: ADD -insecure TO THE GAME'S STEAM LAUNCH OPTIONS AS WELL.
Close any other game before launching. CS2 is not supported.

Controls
Right trigger: fire. Left trigger: use / interact (E).
Right grip: buy menu toggle. Left grip: quiet walk.
A: jump. X: reload. B: cycle weapons. Y: drop one weapon immediately.
Left stick: movement / ladder movement; click: scores.
Right stick: turn; down: quiet walk; click: crouch.
Left menu: short press recenters; hold 0.7 seconds toggles the game menu.
AUG/AWP optics stay on automatically. Native weapon recoil remains active.

Bug reports: attach the ZIP from REPORTS, your headset/runtime and a description.
A report is collected automatically after exit. RUN.bat --report also works.
Telemetry records fresh XR images separately from source FPS and XR cadence.
No automatic upload. No screenshots or memory dumps in the report.

F6: revised frame buffering and fewer repeated Direct3D state changes.
The real offline game passed a 30-second XR Sim check, hands, firing and menus.
Physical headset acceptance is checked separately; TESTED.json gives the scope.
Keep your previous mod folder for rollback. Game binaries are not replaced.
