#!/usr/bin/env python3
"""Static integrity gate for the minimal Quest projection child.

This is intentionally narrower than a compiler. It proves the repository still
contains the exact audited parent and exact prepared child, and rejects known
v0.6.1 oracle-only behavior if it leaks into the child.
"""

from __future__ import annotations

import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PARENT = ROOT / "runtime/golden_v0.5.5/src/eye_patch.c"
INJECTOR = ROOT / "runtime/golden_v0.5.5/src/injector.c"
CHILD = ROOT / "experiments/quest_projection_child/eye_patch_quest_projection.c"

PARENT_SHA256 = "3844B2C323F1077A901581A0A82908A174F9D1DF3F4213C5EB68980B7E7D148F"
INJECTOR_SHA256 = "E6133CEFC23460BA3AD82350803251C604AA24425474B224BEF7FEE01EA5183D"
CHILD_SHA256 = "F9B66C35327EC5F0F355ECD74DFA21CB4CD1A3D91CEE3B843AD1BBC95C5EB628"

REQUIRED = [
    "#define VIEW_BYTES 0x198u",
    "#define OFF_CUSTOM_PROJ_BOOL 0x068u",
    "#define OFF_CUSTOM_PROJ_MATRIX 0x06Cu",
    "#define QUEST_M00 0.902738278f",
    "#define QUEST_M02_ABS 0.242512644f",
    "#define QUEST_M11 0.835478849f",
    "#define QUEST_M12 -0.193187454f",
    "apply_quest_asymmetric_projection(localView, pass)",
    "localView[OFF_CUSTOM_PROJ_BOOL] = 1;",
    "pass == 1 || pass == 2",
    "pass 3 forceCache=0 = normal on-screen game view",
    "exact -insecure",
]

FORBIDDEN = [
    "g_frozenOrigin",
    "g_frozenAngles",
    "g_frozenFov",
    "g_frozenAspect",
    "g_frozenZNear",
    "g_frozenZFar",
    "freeze_center_camera_once",
    "custom_projection_mode.ini",
    "projection_control_thread",
    "PROJ_X_POS",
    "PROJ_X_NEG",
    "PROJ_Y_POS",
    "PROJ_Y_NEG",
    "PROJ_X_WIDE",
    "PROJ_Y_WIDE",
    "PROJ_SHIFT",
    "PROJ_WIDE_SCALE",
    "140.0f",
]


def sha(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def fail(message: str) -> int:
    print(f"FAIL: {message}", file=sys.stderr)
    return 1


def main() -> int:
    for path in (PARENT, INJECTOR, CHILD):
        if not path.is_file():
            return fail(f"missing file: {path.relative_to(ROOT)}")

    checks = [
        (PARENT, PARENT_SHA256, "parent"),
        (INJECTOR, INJECTOR_SHA256, "injector"),
        (CHILD, CHILD_SHA256, "child"),
    ]
    for path, expected, label in checks:
        actual = sha(path)
        if actual != expected:
            return fail(f"{label} SHA256 expected={expected} actual={actual}")
        print(f"PASS {label}_sha256 {actual}")

    text = CHILD.read_text(encoding="utf-8")
    for token in REQUIRED:
        if token not in text:
            return fail(f"required child token missing: {token!r}")
    print(f"PASS required_tokens {len(REQUIRED)}")

    for token in FORBIDDEN:
        if token in text:
            return fail(f"forbidden v0.6.1 oracle behavior leaked into child: {token!r}")
    print(f"PASS forbidden_tokens_absent {len(FORBIDDEN)}")

    if text.count("{") != text.count("}"):
        return fail("rough brace balance failed")
    print("PASS brace_balance")

    # Parent scheduling/ownership seam must still exist literally in the child.
    parent_text = PARENT.read_text(encoding="utf-8")
    invariants = [
        "int currentCount = *(volatile int*)((uint8_t*)afxStreamsThis + OFF_DO_RENDER_COUNT);",
        "int forceCache = *(volatile uint8_t*)((uint8_t*)afxStreamsThis + OFF_FORCE_CACHE) ? 1 : 0;",
        "if (view && forceCache && (pass == 1 || pass == 2))",
        "g_original(afxStreamsThis, fn, gameThis, gameEdx,\n               view, hudView, nClearFlags, whatToDraw);",
    ]
    for invariant in invariants:
        if invariant not in parent_text or invariant not in text:
            return fail(f"parent scheduling invariant changed/missing: {invariant!r}")
    print(f"PASS parent_schedule_invariants {len(invariants)}")

    print("QUEST_PROJECTION_CHILD_STATIC_GATE=PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
