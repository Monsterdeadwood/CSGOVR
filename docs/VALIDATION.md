# Validation recorded for 0.12

This page summarizes `evidence/TEST_SUMMARY.json` in the supplied
`CSGO_VR_0.12_RELEASE_KIT_20261021.zip`. It records earlier tests; the
repository import did not run the game or a headset session.

## Package identity

- Build: `NOVA-2026.10.21-F6`.
- Installable ZIP: `release/csgo 0.12.zip`, 2,342,717 bytes.
- ZIP SHA-256: `ff9be3ff7c1d65d3d14ff0286aba8bf7865cb50aacbc1a1f6c72f070041aa41e`.
- All 101 extracted release files are preserved byte for byte.
- The supplied ZIP, package manifests and diagnostic hashes were checked
  during the import. This is an integrity check, not a new gameplay test.

## Recorded XR Sim check

The handoff reports a 30.031-second native XR Sim run in the actual offline
game, hand movement, eight native shot receipts, and opening and closing
the buy and game menus. The recorded pipeline error counters were zero,
and the game and host closed. Simulator results alone do not establish
physical-headset pixels or frame rate.

## Recorded Meta session

The later user session ran on 21 October 2026, 07:24:52–07:30:05 UTC,
and received positive user feedback. Its reported window statistics were:

| Measure | 10th percentile | Median | 90th percentile |
| --- | ---: | ---: | ---: |
| New XR world images per second | 36.96 | 69.20 | 72.35 |
| XR end rate, Hz | 71.43 | 72.17 | 74.22 |

The XR end rate includes reused images. It is not the rate of new game
frames. The recorded maxima for haptic, graphics, GPU-fence and XR error
counters were zero. This is one session, with dips; stable 72 new frames
per second and broad headset compatibility are not established.

The sealed package predates that session. Its original `TESTED.json` and
`RELEASE.json` retain the earlier pending/retest headset fields. They are
preserved as supplied; the release-kit summary is the source of the later
session information.

For unresolved reports, including reports whose exact tested build is
unknown, see [KNOWN_ISSUES.md](../KNOWN_ISSUES.md).
