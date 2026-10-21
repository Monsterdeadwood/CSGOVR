#!/usr/bin/env python3
"""Build additive XR Lab hosts. Preserves all original NOVA binaries.

Requires Python 3, clang++ and lld-link (LLVM on Windows). No game is started.
"""
from pathlib import Path
import hashlib
import json
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[1]
LLVM = Path(r"C:\Program Files\LLVM\bin")
CC = shutil.which("clang++") or str(LLVM / "clang++.exe")
LINK = shutil.which("lld-link") or str(LLVM / "lld-link.exe")
BUILD = ROOT / "_build" / "lab-hosts"
BUILD.mkdir(parents=True, exist_ok=True)
commands = []

def run(args):
    args = list(map(str, args))
    commands.append(args)
    subprocess.run(args, check=True)

kernel = "GetProcAddress LoadLibraryW GetModuleHandleW GetModuleFileNameW GetEnvironmentVariableW GetCurrentProcessId GetCurrentThreadId GetTickCount64 QueryPerformanceCounter QueryPerformanceFrequency Sleep VirtualProtect VirtualQuery VirtualAlloc VirtualFree CreateFileW ReadFile WriteFile FlushFileBuffers CloseHandle CreateFileMappingW OpenFileMappingW MapViewOfFile UnmapViewOfFile OpenProcess QueryFullProcessImageNameW GetProcessTimes VirtualAllocEx VirtualFreeEx WriteProcessMemory CreateRemoteThread WaitForSingleObject GetExitCodeThread CreateThread DisableThreadLibraryCalls GetLastError CreateToolhelp32Snapshot Module32FirstW Module32NextW GetFileInformationByHandle ExitProcess OutputDebugStringA".split()
libs = {"kernel32": kernel, "msvcrt": "_snprintf memcpy memset sin cos tan atan2 sqrt printf".split(),
        "d3d11": ["D3D11CreateDevice"], "dxgi": ["CreateDXGIFactory1"]}
for name, exports in libs.items():
    definition = BUILD / (name + ".def")
    definition.write_text("LIBRARY " + name + ".dll\nEXPORTS\n" + "\n".join(exports) + "\n")
    run([LINK, "/lib", "/def:" + str(definition), "/machine:x64", "/out:" + str(BUILD / (name + ".lib"))])

targets = [(ROOT, "nova_xr64_sim_palm.exe", ["NOVA_LAB_SIM", "NOVA_LAB_PERF", "NOVA_LAB_UNIVERSAL"]),
           (ROOT, "nova_xr64_palm.exe", ["NOVA_LAB_PERF", "NOVA_LAB_UNIVERSAL"])]
hashes = {}
for profile, output, defines in targets:
    obj = BUILD / (profile.name + "_" + output + ".obj")
    run([CC, "--target=x86_64-pc-windows-msvc", "-std=c++17", "-O2", "-ffreestanding", "-fno-builtin",
         "-fno-omit-frame-pointer", "-fno-exceptions", "-fno-rtti", "-fno-threadsafe-statics", "-fno-stack-protector",
         "-fms-extensions", "-mno-stack-arg-probe", "-Wall", "-Wextra", "-Werror", "-Wno-empty-body", "-Wno-missing-field-initializers", "-Wno-unused-parameter",
         *("-D" + d for d in defines), "-I" + str(profile / "src"), "-I" + str(profile / "deps"),
         "-c", profile / "src/xr64.cpp", "-o", obj])
    binary = profile / "bin" / output
    run([LINK, "/nodefaultlib", "/machine:x64", "/dynamicbase", "/nxcompat", "/Brepro", "/subsystem:console",
         "/entry:mainCRTStartup", "/stack:2097152,2097152", "/out:" + str(binary), obj,
         *(BUILD / (name + ".lib") for name in libs)])
    hashes[str(binary.relative_to(ROOT)).replace("\\", "/")] = hashlib.sha256(binary.read_bytes()).hexdigest()
(BUILD / "build.json").write_text(json.dumps({"commands": commands, "binaries": hashes, "game_launched": False}, indent=2))
diagnostic_hashes = dict(hashes)
for relative in ("diagnostics/xr-sim/xrsim.dll", "diagnostics/xr-tape/xrtape_layer.dll"):
    diagnostic_hashes[relative] = hashlib.sha256((ROOT / relative).read_bytes()).hexdigest()
for relative in ("profiles/graphics3k/bin/nova_scene32_adaptive.dll", "bin/nova_scene32_measured.dll", "bin/nova_scene32_universal.dll"):
    if (ROOT / relative).is_file():
        diagnostic_hashes[relative] = hashlib.sha256((ROOT / relative).read_bytes()).hexdigest()
for folder in ("bin", "profiles/graphics3k/bin"):
    for name in ("msvcp140.dll", "vcruntime140.dll", "vcruntime140_1.dll"):
        relative=folder+"/"+name
        if (ROOT / relative).is_file():
            diagnostic_hashes[relative] = hashlib.sha256((ROOT / relative).read_bytes()).hexdigest()
(ROOT / "DIAGNOSTICS_SHA256.json").write_text(json.dumps(diagnostic_hashes, indent=2))
print(json.dumps({"built": hashes, "runtime_verified_after_changes": False}, indent=2))
