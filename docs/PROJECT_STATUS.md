# NOVA · CS:GO Legacy VR — project status

[← Project page](../README.md) · [Flat2VR testing & discussion](https://discord.gg/P4q9zt7ek)

Reviewed **14 November 2026**. Documented branch: **0.12 / NOVA-2026.10.21-F6**.

## What has been checked

- The released F6/0.12 build has author-reported Meta play acceptance. A tester also reported good results on VDXR; wider device coverage remains limited.

## Current limits

- The released left arm is not an independently tracked 6DoF hand. Some scope, audio/grenade orientation and enemy-visibility reports remain open.
- This release targets local bot matches on its pinned CS:GO build; CS2 and server connections are outside its scope.

## Separate experimental work

- A separate private development branch explores two-hand/either-hand carry, physical magazines, native finger grips, expanded optics and recoil. These changes do not alter the preserved F6 release.

## Build access

The existing 0.12 package is linked from the project page. Later local candidates are separate from that preserved release.

## Documentation basis

This overview was reconciled against the project’s recorded package notes and validation records. The following names identify the records reviewed; they are not download links:

- `docs/VALIDATION.md`
- `TESTED.json`
- `NOVA_PHYSICAL_CARRY_20261112_154317.json`

Game execution, simulator validation and physical-headset feedback are reported separately above. Features from a separate experimental branch are identified explicitly.

## Кратко по-русски

NOVA переносит CS:GO Legacy в VR: трекинг головы, оружие от контроллера, отдача и вибрация. Версия 0.12 запускает локальный матч 5×5 с девятью ботами через выбранный OpenXR runtime.

- Левая рука в опубликованной версии не является независимой 6DoF-рукой. Открыты отдельные вопросы оптики, ориентации звука/гранат и видимости врагов.
- Версия рассчитана на локальные боты и зафиксированную сборку CS:GO; CS2 и подключения к серверам в её область не входят.
