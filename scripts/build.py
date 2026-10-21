#!/usr/bin/env python3
"""Build only the new H2 native x86 module. The accepted R4 host is NEVER rebuilt here."""
import pathlib, shutil, subprocess, json, sys
root=pathlib.Path(__file__).resolve().parents[1]
clang=shutil.which('clang++') or r'C:\Program Files\LLVM\bin\clang++.exe';lld=shutil.which('lld-link') or r'C:\Program Files\LLVM\bin\lld-link.exe'
if not clang or not lld:raise SystemExit('clang++ and lld-link required')
out=root/'_build';out.mkdir(exist_ok=True)
if any(a not in ('--scene-only','--measured','--universal') for a in sys.argv[1:]):raise SystemExit('Only --scene-only, --measured or --universal is supported')
measured='--measured' in sys.argv[1:]
universal='--universal' in sys.argv[1:]
commands=[]
def run(args):
    args=list(map(str,args));commands.append(args)
    p=subprocess.run(args,capture_output=True,text=True)
    if p.stdout:print(p.stdout)
    if p.stderr:print(p.stderr)
    if p.returncode:raise SystemExit(f'BUILD_FAILED cmd={" ".join(args)} exit={p.returncode}')
    return p
kernel={
'GetProcAddress':8,'LoadLibraryW':4,'GetModuleHandleW':4,'GetModuleFileNameW':12,'GetEnvironmentVariableW':12,
'GetCurrentProcessId':0,'GetCurrentThreadId':0,'GetTickCount64':0,'Sleep':4,'VirtualProtect':16,'VirtualQuery':12,
'QueryPerformanceCounter':4,'QueryPerformanceFrequency':4,
'VirtualAlloc':16,'VirtualFree':12,'CreateFileW':28,'WriteFile':20,'FlushFileBuffers':4,'CloseHandle':4,
'CreateFileMappingW':24,'OpenFileMappingW':12,'MapViewOfFile':20,'UnmapViewOfFile':4,'OpenProcess':12,
'QueryFullProcessImageNameW':16,'GetProcessTimes':20,'VirtualAllocEx':20,'VirtualFreeEx':16,'WriteProcessMemory':20,
'CreateRemoteThread':28,'WaitForSingleObject':8,'GetExitCodeThread':8,'CreateThread':24,
'DisableThreadLibraryCalls':4,'GetLastError':0,'CreateToolhelp32Snapshot':8,'Module32FirstW':8,'Module32NextW':8,
'GetFileInformationByHandle':8,'FlushInstructionCache':12,'ExitProcess':4,'OutputDebugStringA':4 }
libs={'kernel32':kernel,'msvcrt':{x:None for x in ['_snprintf','memcpy','memset','sin','cos','tan','atan2','sqrt']},
'd3d11':{'D3D11CreateDevice':44},'dxgi':{'CreateDXGIFactory1':8}}
for arch,target,source,file,dll in [('x86','i686-pc-windows-msvc','scene32.cpp','nova_scene32.dll',True),
 ('x86','i686-pc-windows-msvc','inject32.cpp','nova_inject32.exe',False),
 ('x86','i686-pc-windows-msvc','control32.cpp','nova_control32.exe',False),
 ('x64','x86_64-pc-windows-msvc','xr64.cpp','nova_xr64.exe',False)]:
    if source!="scene32.cpp":continue
    if measured:file='nova_scene32_measured.dll'
    if universal:file='nova_scene32_universal.dll'
    build=out/arch;build.mkdir(exist_ok=True)
    needed=['kernel32','msvcrt']+(['d3d11','dxgi'] if arch=='x64' else [])
    for lib in needed:
        defs=build/(lib+'.def')
        defs.write_text('LIBRARY '+lib+'.dll\nEXPORTS\n'+'\n'.join(libs[lib])+'\n')
        run([lld,'/lib','/def:'+str(defs),'/machine:'+arch,'/out:'+str(build/(lib+'.lib'))])
    obj=build/(source+'.obj')
    flags=['--target='+target,'-std=c++17','-O2','-ffreestanding','-fno-builtin','-fno-omit-frame-pointer','-fno-exceptions','-fno-rtti','-fno-threadsafe-statics','-fno-stack-protector',
           '-fms-extensions','-mno-stack-arg-probe','-Wall','-Wextra','-Werror','-Wno-missing-field-initializers','-Wno-unused-parameter','-I'+str(root/'src'),'-I'+str(root/'deps')]
    if measured:flags+=['-DNOVA_LAB_PERF']
    if universal:flags+=['-DNOVA_LAB_PERF','-DNOVA_LAB_UNIVERSAL']
    run([clang,*flags,'-c',root/'src'/source,'-o',obj])
    link=[lld,'/nodefaultlib','/machine:'+arch,'/dynamicbase','/nxcompat','/Brepro','/out:'+str(root/'bin'/file),str(obj),'/stack:2097152,2097152']
    if dll:link+=['/dll','/entry:DllMain@12']
    else:link+=['/subsystem:console','/entry:mainCRTStartup']
    link +=[str(build/(lib+'.lib')) for lib in needed]
    if arch=='x86':
        for name,size in kernel.items(): link+=['/alternatename:__imp__'+name+'@'+str(size)+'=__imp__'+name]
        # DllMain is stdcall while functions in the import definition are just extern DLL exports.
    run(link)
(out/'commands.json').write_text(json.dumps(commands,indent=2))
print("HANDCORE H2 build complete; runtime NOT verified")
