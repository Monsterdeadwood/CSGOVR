@echo off
setlocal EnableExtensions
cd /d "%~dp0"
chcp 65001 >nul 2>&1
set "SELF=%~f0"
title CSGOVR FX FIRE OWNERSHIP CLOSURE v1.2.0 COLD OMNI

echo ============================================================
echo CSGOVR FX FIRE OWNERSHIP CLOSURE v1.2.0
echo COLD ONLY - NO GAME - NO HLAE - NO OPENXR - NO INJECTION
echo ============================================================
echo.
echo MISSING FACT: exact local-shot ownership across every frozen FX_FireBullets caller.
echo WHY NOT COLD: N/A; frozen binaries + caller census + RTTI + VEngineClient014 can be mined offline.
echo PASS: all direct callers, caller families, ECX/EDX/stack evidence, local-player interface evidence, special caller evidence.
echo FAIL: hash mismatch, running target, incomplete caller census, or wrong-object caller silently promoted.
echo BASELINE MUST HOLD: exact frozen hashes; no game/HLAE/OpenXR/injection/debugger; no mutation; no LIVE ticket.
echo.

powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "$p=[IO.File]::ReadAllText($env:SELF);$m='###__CSGOVR_PS1_PAYLOAD__###';$i=$p.IndexOf($m);if($i-lt0){Write-Error 'payload marker missing';exit 2};$s=$p.Substring($i+$m.Length);$tmp=Join-Path $env:TEMP ('CSGOVR_FIRE_OWNERSHIP_'+[guid]::NewGuid().ToString('N')+'.ps1');[IO.File]::WriteAllText($tmp,$s,[Text.UTF8Encoding]::new($false));try{& powershell.exe -NoProfile -ExecutionPolicy Bypass -File $tmp;exit $LASTEXITCODE}finally{Remove-Item -LiteralPath $tmp -Force -ErrorAction SilentlyContinue}"
set "EC=%ERRORLEVEL%"
echo.
echo ExitCode=%EC%
echo Send CSGOVR_FX_FIRE_OWNERSHIP_CLOSURE_RESULT_*.zip back to ChatGPT.
pause
exit /b %EC%

###__CSGOVR_PS1_PAYLOAD__###
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$ProgressPreference='SilentlyContinue'

$BatPath=$env:SELF
$Root=Split-Path -Parent $BatPath
$ExpectedClient='183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03'
$ExpectedServer='699757BB5C8DDB71193F68DABA22F59F697491DDBD768BC8656AC2CE435C4DB8'

Write-Host '[MODE] COLD OWNERSHIP ONLY. No game/HLAE/OpenXR/injection/debugger.' -ForegroundColor Cyan

$bad=@()
foreach($n in @('csgo','HLAE','hlae','csgovr_m32b_xr_host_v110','csgovr_xr_host_v071')){
    try{$bad += @(Get-Process -Name $n -ErrorAction SilentlyContinue)}catch{}
}
$bad=@($bad|Sort-Object Id -Unique)
if($bad.Count -gt 0){
    $ids=($bad|ForEach-Object{"$($_.ProcessName):$($_.Id)"}) -join ', '
    throw "COLD gate: target/VR processes are running: $ids"
}

function Test-GameRoot([string]$g){
    if([string]::IsNullOrWhiteSpace($g)){return $false}
    return (Test-Path -LiteralPath (Join-Path $g 'csgo\bin\client.dll') -PathType Leaf) -and
           (Test-Path -LiteralPath (Join-Path $g 'csgo\bin\server.dll') -PathType Leaf)
}

$candidates=New-Object System.Collections.Generic.List[string]
if($env:CSGOVR_GAME){$candidates.Add($env:CSGOVR_GAME)}
$candidates.Add('C:\Program Files (x86)\Steam\steamapps\common\csgo legacy')
$candidates.Add('C:\Program Files (x86)\Steam\steamapps\common\Counter-Strike Global Offensive')
$Game=$null
foreach($g in @($candidates|Select-Object -Unique)){if(Test-GameRoot $g){$Game=$g;break}}
if(-not$Game){throw 'Frozen CS:GO not found. Set CSGOVR_GAME to the folder containing csgo\bin\client.dll.'}

$Client=Join-Path $Game 'csgo\bin\client.dll'
$Server=Join-Path $Game 'csgo\bin\server.dll'
$Engine=$null
foreach($e in @((Join-Path $Game 'bin\engine.dll'),(Join-Path $Game 'csgo\bin\engine.dll'))){
    if(Test-Path -LiteralPath $e -PathType Leaf){$Engine=$e;break}
}
if(-not$Engine){throw 'engine.dll not found under game root; VEngineClient014 cold closure requires it.'}

$csha=(Get-FileHash -LiteralPath $Client -Algorithm SHA256).Hash.ToUpperInvariant()
$ssha=(Get-FileHash -LiteralPath $Server -Algorithm SHA256).Hash.ToUpperInvariant()
$esha=(Get-FileHash -LiteralPath $Engine -Algorithm SHA256).Hash.ToUpperInvariant()
Write-Host ('[GAME] '+$Game)
Write-Host ('[HASH] client='+$csha)
Write-Host ('[HASH] server='+$ssha)
Write-Host ('[HASH] engine='+$esha)
if($csha-ne$ExpectedClient){throw 'client.dll frozen hash mismatch'}
if($ssha-ne$ExpectedServer){throw 'server.dll frozen hash mismatch'}

$Python=$null;$PyArgs=@()
foreach($n in @('python.exe','python3.exe','py.exe')){
    $cmd=Get-Command $n -ErrorAction SilentlyContinue
    if($cmd){
        $Python=$cmd.Source
        if([IO.Path]::GetFileName($Python)-ieq'py.exe'){$PyArgs=@('-3')}
        break
    }
}
if(-not$Python){throw 'Python 3 not found.'}

$Disasm=$null;$DisasmKind=$null
$dc=New-Object System.Collections.Generic.List[string]
foreach($n in @('llvm-objdump.exe','objdump.exe')){
    $cmd=Get-Command $n -ErrorAction SilentlyContinue
    if($cmd){$dc.Add($cmd.Source)}
}
foreach($p in @(
    'C:\Program Files\Git\mingw64\bin\objdump.exe',
    'C:\Program Files\Git\usr\bin\objdump.exe',
    'C:\Program Files\LLVM\bin\llvm-objdump.exe',
    'C:\Program Files (x86)\LLVM\bin\llvm-objdump.exe'
)){
    if(Test-Path -LiteralPath $p -PathType Leaf){$dc.Add($p)}
}
foreach($p in @($dc|Select-Object -Unique)){
    try{
        & $p --version *> $null
        if($LASTEXITCODE-eq0){
            $Disasm=$p
            $DisasmKind=if([IO.Path]::GetFileName($p).ToLowerInvariant()-like'llvm-*'){'LLVM'}else{'GNU'}
            break
        }
    }catch{}
}
if(-not$Disasm){throw 'No objdump/llvm-objdump found. Git for Windows normally supplies objdump.exe.'}
Write-Host ('[DISASM] '+$Disasm+' kind='+$DisasmKind) -ForegroundColor Green

$Stamp=Get-Date -Format 'yyyyMMdd_HHmmss'
$Safe=Join-Path 'C:\CSGOVR_TMP' ('FIRE_OWNERSHIP_'+$Stamp)
$Input=Join-Path $Safe 'input'
$Out=Join-Path $Safe 'RESULT'
$CallerDir=Join-Path $Out 'ALL_CALLER_DISASM'
$EngineDir=Join-Path $Out 'ENGINE_INTERFACE_DISASM'
New-Item -ItemType Directory -Force -Path $Input,$Out,$CallerDir,$EngineDir|Out-Null

function Run-RangeDisasm([string]$Label,[string]$Bin,[uint64]$StartVA,[uint64]$StopVA,[string]$Dst){
    if($StopVA-le$StartVA){throw "$Label invalid disassembly range"}
    $start=('0x{0:X}' -f $StartVA);$stop=('0x{0:X}' -f $StopVA)
    if($DisasmKind-eq'LLVM'){
        & $Disasm -d --x86-asm-syntax=intel --start-address=$start --stop-address=$stop $Bin 2>&1 | Out-File -FilePath $Dst -Encoding ascii
    }else{
        & $Disasm -d -M intel --start-address=$start --stop-address=$stop $Bin 2>&1 | Out-File -FilePath $Dst -Encoding ascii
    }
    if($LASTEXITCODE-ne0){throw "$Label disassembly exit=$LASTEXITCODE"}
}

$PyCode=@'
#!/usr/bin/env python3
import argparse, hashlib, json, re, struct
from pathlib import Path

EXPECTED_CLIENT="183D42FAC8A4ECC24DC02EDF5F09122F37FE9E7533CA093A867718B25CFDEB03"
EXPECTED_SERVER="699757BB5C8DDB71193F68DABA22F59F697491DDBD768BC8656AC2CE435C4DB8"
TARGETS={"client":0x0044DC90,"server":0x004C4650}
SPECIAL={"client":{0x003F6989:"KNOWN_TE_PROOF_CALLER"},"server":{0x00258521:"SPECIAL_258521"}}

def u16(b,o): return struct.unpack_from("<H",b,o)[0]
def u32(b,o): return struct.unpack_from("<I",b,o)[0]
def i32(b,o): return struct.unpack_from("<i",b,o)[0]
def p32(x): return struct.pack("<I",x & 0xffffffff)

class PE:
    def __init__(self,path):
        self.path=Path(path); self.b=self.path.read_bytes()
        if self.b[:2]!=b"MZ": raise RuntimeError(f"{path}: not MZ")
        pe=u32(self.b,0x3c)
        if self.b[pe:pe+4]!=b"PE\0\0": raise RuntimeError(f"{path}: no PE")
        nsec=u16(self.b,pe+6); optsz=u16(self.b,pe+20); opt=pe+24
        if u16(self.b,opt)!=0x10b: raise RuntimeError(f"{path}: expected PE32")
        self.base=u32(self.b,opt+28); self.size_image=u32(self.b,opt+56)
        self.timestamp=u32(self.b,pe+8)
        sec0=opt+optsz; self.sections=[]
        for i in range(nsec):
            o=sec0+i*40
            name=self.b[o:o+8].split(b"\0",1)[0].decode("ascii","replace")
            vs=u32(self.b,o+8); va=u32(self.b,o+12); rawsz=u32(self.b,o+16); raw=u32(self.b,o+20); chars=u32(self.b,o+36)
            self.sections.append(dict(name=name,va=va,vsize=vs,raw=raw,rawsize=rawsz,chars=chars))
    def sha(self): return hashlib.sha256(self.b).hexdigest().upper()
    def sec_for_rva(self,rva):
        for s in self.sections:
            if s["va"] <= rva < s["va"]+max(s["vsize"],s["rawsize"]): return s
        return None
    def rva_to_off(self,rva):
        s=self.sec_for_rva(rva)
        if not s: return None
        d=rva-s["va"]
        if d>=s["rawsize"]: return None
        return s["raw"]+d
    def va_to_off(self,va): return self.rva_to_off(va-self.base)
    def read_u32_va(self,va):
        o=self.va_to_off(va)
        if o is None or o+4>len(self.b): return None
        return u32(self.b,o)
    def read_bytes_va(self,va,n):
        o=self.va_to_off(va)
        if o is None: return b""
        return self.b[o:o+n]
    def is_exec_va(self,va):
        s=self.sec_for_rva(va-self.base)
        return bool(s and (s["chars"] & 0x20000000))
    def is_mapped_va(self,va): return self.sec_for_rva(va-self.base) is not None
    def data_sections(self): return [s for s in self.sections if s["rawsize"] and not (s["chars"] & 0x20000000)]
    def exec_sections(self): return [s for s in self.sections if s["rawsize"] and (s["chars"] & 0x20000000)]
    def find_bytes(self,needle,sections=None):
        out=[]
        for s in sections or self.sections:
            blob=self.b[s["raw"]:s["raw"]+s["rawsize"]]; pos=0
            while True:
                j=blob.find(needle,pos)
                if j<0: break
                out.append(self.base+s["va"]+j); pos=j+1
        return out

def scan_direct_calls(pe,target_rva):
    ans=[]
    for s in pe.exec_sections():
        blob=pe.b[s["raw"]:s["raw"]+s["rawsize"]]
        for j in range(0,max(0,len(blob)-4)):
            if blob[j]!=0xE8: continue
            rel=i32(blob,j+1); call_rva=s["va"]+j; dst=(call_rva+5+rel)&0xffffffff
            if dst==target_rva:
                call_va=pe.base+call_rva; sec_lo=pe.base+s["va"]; sec_hi=sec_lo+s["rawsize"]
                ans.append(dict(call_rva=call_rva,call_va=call_va,return_va=call_va+5,target_rva=target_rva,target_va=pe.base+target_rva,
                    window_start_va=max(sec_lo,call_va-0x1800),window_stop_va=min(sec_hi,call_va+0x500)))
    return sorted(ans,key=lambda x:x["call_rva"])

def strong_starts(pe,call_rva):
    s=pe.sec_for_rva(call_rva)
    if not s: return []
    lo=max(s["va"],call_rva-0x5000); a=pe.rva_to_off(lo); b=pe.rva_to_off(call_rva)
    if a is None or b is None: return []
    blob=pe.b[a:b]; hits=[]
    for pat,label in [(b"\x55\x8b\xec","55 8B EC"),(b"\x55\x89\xe5","55 89 E5"),(b"\x56\x8b\xf1","56 8B F1"),(b"\x53\x8b\xdc","53 8B DC")]:
        pos=0
        while True:
            j=blob.find(pat,pos)
            if j<0: break
            rva=lo+j; hits.append(dict(rva=rva,va=pe.base+rva,distance=call_rva-rva,pattern=label)); pos=j+1
    for m in re.finditer(b"\xCC{3,}",blob):
        j=m.end()
        while j<len(blob) and blob[j] in (0x90,0xCC): j+=1
        if j<len(blob):
            rva=lo+j; d=call_rva-rva
            if 0<d<=0x5000: hits.append(dict(rva=rva,va=pe.base+rva,distance=d,pattern="AFTER_CC_PADDING"))
    d={}
    for h in hits:d[(h["va"],h["pattern"])]=h
    return sorted(d.values(),key=lambda x:x["distance"])[:32]

def recover_rtti(pe):
    classes=[]; name_hits=[]
    for s in pe.data_sections():
        blob=pe.b[s["raw"]:s["raw"]+s["rawsize"]]
        for m in re.finditer(rb"\.\?A[UV][A-Za-z0-9_?$@.]{2,180}@@\x00",blob):
            name=m.group()[:-1].decode("ascii","replace"); name_va=pe.base+s["va"]+m.start(); name_hits.append((name,name_va-8))
    seen=set()
    for name,td_va in name_hits:
        if (name,td_va) in seen: continue
        seen.add((name,td_va))
        for ref_va in pe.find_bytes(p32(td_va),pe.data_sections()):
            col_va=ref_va-12; sig=pe.read_u32_va(col_va); off=pe.read_u32_va(col_va+4); ptd=pe.read_u32_va(col_va+12); pchd=pe.read_u32_va(col_va+16)
            if sig not in (0,1) or ptd!=td_va or off is None or off>0x100000 or pchd is None or not pe.is_mapped_va(pchd): continue
            for vr in pe.find_bytes(p32(col_va),pe.data_sections()):
                vt=vr+4; entries=[]
                for i in range(256):
                    f=pe.read_u32_va(vt+i*4)
                    if f is None or not pe.is_exec_va(f): break
                    entries.append(f)
                if entries: classes.append(dict(name=name,type_descriptor_va=td_va,col_va=col_va,vtable_va=vt,entry_count=len(entries),entries=entries))
    d={}
    for c in classes:d[(c["name"],c["vtable_va"])]=c
    return list(d.values())

def classify_call(pe,rec,classes):
    starts=strong_starts(pe,rec["call_rva"]); rec["strong_start_candidates"]=starts
    by_func={}
    for c in classes:
        for idx,f in enumerate(c["entries"]):by_func.setdefault(f,[]).append((c["name"],idx,c["vtable_va"]))
    exact=[]
    for st in starts:
        if st["va"] in by_func:
            exact.append(dict(function_va=st["va"],distance=st["distance"],start_pattern=st["pattern"],classes=[dict(name=n,slot=i,vtable_va=v) for n,i,v in by_func[st["va"]]]))
    nearest=[]
    for f,owners in by_func.items():
        dist=rec["call_va"]-f
        if 0<=dist<=0x5000: nearest.append(dict(function_va=f,distance=dist,classes=[dict(name=n,slot=i,vtable_va=v) for n,i,v in owners]))
    nearest=sorted(nearest,key=lambda x:x["distance"])[:16]
    rec["rtti_exact_start_matches"]=exact; rec["rtti_nearest_vfunc_candidates"]=nearest
    names=[]
    for x in exact:names += [y["name"] for y in x["classes"]]
    if not names and nearest and nearest[0]["distance"]<=0x800:names += [y["name"] for y in nearest[0]["classes"]]
    joined=" ".join(names).lower()
    if "tefirebullets" in joined: kind="NETWORK_TE"; policy="REJECT"
    elif "weapon" in joined: kind="WEAPON_FAMILY"; policy="CANDIDATE_REQUIRES_LOCAL_PLAYER"
    else: kind="UNKNOWN"; policy="REJECT_PENDING"
    rec["class_names"]=sorted(set(names)); rec["caller_kind"]=kind; rec["policy"]=policy
    return rec

def resolve_create(pe,fn_va,depth=0):
    if depth>3 or not pe.is_exec_va(fn_va): return dict(kind="UNRESOLVED")
    raw=pe.read_bytes_va(fn_va,64)
    if len(raw)>=5 and raw[0]==0xE9:
        dst=fn_va+5+struct.unpack_from("<i",raw,1)[0]; r=resolve_create(pe,dst,depth+1); r["via_jmp"]=dst; return r
    for i in range(min(40,max(0,len(raw)-5))):
        if raw[i]==0xB8:
            imm=struct.unpack_from("<I",raw,i+1)[0]
            if pe.is_mapped_va(imm): return dict(kind="MOV_EAX_IMM",operand_va=imm,object_va=imm)
        if raw[i]==0xA1:
            g=struct.unpack_from("<I",raw,i+1)[0]; val=pe.read_u32_va(g)
            if val and pe.is_mapped_va(val): return dict(kind="MOV_EAX_MEM",operand_va=g,object_va=val)
        if raw[i:i+2]==b"\x8D\x05" and i+6<=len(raw):
            imm=struct.unpack_from("<I",raw,i+2)[0]
            if pe.is_mapped_va(imm): return dict(kind="LEA_EAX_ABS",operand_va=imm,object_va=imm)
    return dict(kind="UNRESOLVED",prefix_hex=raw[:48].hex().upper())

def scan_interface(pe,name="VEngineClient014"):
    strings=pe.find_bytes((name+"\0").encode("ascii"),pe.data_sections()); out=dict(name=name,string_vas=strings,candidates=[])
    for sva in strings:
        for ref in pe.find_bytes(p32(sva),pe.data_sections()):
            reg=ref-4; create=pe.read_u32_va(reg); nameptr=pe.read_u32_va(reg+4); nxt=pe.read_u32_va(reg+8)
            if nameptr!=sva or not create or not pe.is_exec_va(create): continue
            rr=resolve_create(pe,create); obj=rr.get("object_va"); vt=pe.read_u32_va(obj) if obj else None; slots={}
            if vt and pe.is_mapped_va(vt):
                for idx in (12,18,19):
                    f=pe.read_u32_va(vt+idx*4); slots[str(idx)]=dict(function_va=f,is_exec=bool(f and pe.is_exec_va(f)))
            out["candidates"].append(dict(interface_reg_va=reg,create_fn_va=create,next_va=nxt,resolver=rr,object_va=obj,vtable_va=vt,slots=slots))
    d={}
    for c in out["candidates"]:d[(c["interface_reg_va"],c["create_fn_va"])]=c
    out["candidates"]=list(d.values()); return out

INS=re.compile(r'^\s*([0-9A-Fa-f]+):\s+(?:(?:[0-9A-Fa-f]{2})\s+)+\s*([A-Za-z][A-Za-z0-9_.]*)\s*(.*?)\s*$')
def parse_disasm(path):
    out=[]
    for line in Path(path).read_text(encoding="ascii",errors="ignore").splitlines():
        m=INS.match(line)
        if m: out.append(dict(addr=int(m.group(1),16),mn=m.group(2).lower(),op=m.group(3).strip(),raw=line))
    return out

def last_reg_writer(tail,reg):
    rr=reg.lower()
    pats=[re.compile(rf'^\s*{rr}\s*,'),re.compile(rf'\b{rr}\b')]
    for x in reversed(tail):
        op=x["op"].lower()
        if x["mn"].startswith(("mov","lea","xor","pop")) and pats[0].search(op): return x["raw"]
    return None

def caller_dataflow(facts,caller_dir,outdir):
    rows=[]; txt=[]; caller_dir=Path(caller_dir)
    for side in ("client","server"):
        txt.append(f"[{side.upper()}]")
        for rec in facts["callers"][side]:
            rva=int(rec["call_rva"]); call_va=int(rec["call_va"]); p=caller_dir/f"{side.upper()}_CALL_{rva:08X}.txt"
            row=dict(side=side,call_rva=rva,call_va=call_va,caller_kind=rec.get("caller_kind"),policy=rec.get("policy"),classes=rec.get("class_names",[]),special_label=rec.get("special_label"))
            if not p.exists(): row["error"]="DISASM_MISSING"; rows.append(row); continue
            ins=parse_disasm(p); tail=[x for x in ins if x["addr"]<call_va][-500:]
            row["ecx_last_writer"]=last_reg_writer(tail,"ecx"); row["edx_last_writer"]=last_reg_writer(tail,"edx")
            pushes=[]; espwrites=[]
            for x in reversed(tail[-220:]):
                if x["mn"]=="push": pushes.append(x["raw"])
                if x["mn"].startswith("mov") and re.search(r'\[esp(?:\+0x[0-9a-f]+)?\]',x["op"].lower()): espwrites.append(x["raw"])
                if len(pushes)>=16 and len(espwrites)>=16: break
            row["nearest_pushes_reverse"]=pushes[:16]; row["nearest_esp_writes_reverse"]=espwrites[:16]
            rows.append(row)
            txt.append(f"0x{rva:08X} KIND={row['caller_kind']} POLICY={row['policy']} CLASSES={','.join(row['classes']) or '-'} SPECIAL={row.get('special_label') or '-'}")
            txt.append("  ECX="+(row.get("ecx_last_writer") or "NOT_FOUND")); txt.append("  EDX="+(row.get("edx_last_writer") or "NOT_FOUND"))
            txt.append("  NEAREST_PUSHES_REVERSE:")
            for q in pushes[:14]: txt.append("    "+q)
            txt.append("  ESP_WRITES_REVERSE:")
            for q in espwrites[:14]: txt.append("    "+q)
        txt.append("")
    Path(outdir,"ALL_CALLERS_DATAFLOW.json").write_text(json.dumps(rows,indent=2),encoding="utf-8")
    Path(outdir,"ALL_CALLERS_DATAFLOW.txt").write_text("\n".join(txt)+"\n",encoding="utf-8")

def selftest():
    assert p32(0x12345678)==b"\x78\x56\x34\x12"
    assert (0x1005+struct.unpack("<i",struct.pack("<i",-5))[0])==0x1000
    print("SELFTEST=PASS")

def scan(args):
    out=Path(args.out); out.mkdir(parents=True,exist_ok=True)
    client=PE(args.client); server=PE(args.server); engine=PE(args.engine)
    if client.sha()!=EXPECTED_CLIENT: raise RuntimeError("client frozen hash mismatch")
    if server.sha()!=EXPECTED_SERVER: raise RuntimeError("server frozen hash mismatch")
    modules={"client":client,"server":server}; census={}; rtti={}
    for side,pe in modules.items():
        classes=recover_rtti(pe)
        rtti[side]=dict(class_count=len(classes),interesting=[c for c in classes if any(k in c["name"].lower() for k in ("firebullet","weaponm3","weaponnova","weaponsawed","weaponxm1014","weaponcsbase"))][:160])
        recs=scan_direct_calls(pe,TARGETS[side])
        for r in recs:r["special_label"]=SPECIAL.get(side,{}).get(r["call_rva"]);classify_call(pe,r,classes)
        census[side]=recs
    ei=scan_interface(engine,"VEngineClient014")
    result=dict(
        frozen=dict(client_sha256=client.sha(),server_sha256=server.sha(),engine_sha256=engine.sha(),client_timestamp=client.timestamp,server_timestamp=server.timestamp,engine_timestamp=engine.timestamp),
        targets=dict(client_rva=TARGETS["client"],server_rva=TARGETS["server"]),
        caller_counts=dict(client=len(census["client"]),server=len(census["server"])),callers=census,rtti=rtti,vengineclient014=ei,
        source_candidates=dict(GetLocalPlayer_slot=12,GetViewAngles_slot=18,SetViewAngles_slot=19,late_abi="ECX playerIndex; EDX weapon*; stack1 econ*; stack2 origin; stack3 angles",tag="SOURCE_INFERRED_UNTIL_FROZEN_REVIEW"))
    (out/"OWNERSHIP_COLD_FACTS.json").write_text(json.dumps(result,indent=2),encoding="utf-8")
    lines=["CSGOVR FX FIRE OWNERSHIP COLD FACTS",f"CLIENT_SHA256={client.sha()}",f"SERVER_SHA256={server.sha()}",f"ENGINE_SHA256={engine.sha()}",f"CLIENT_DIRECT_CALLERS={len(census['client'])}",f"SERVER_DIRECT_CALLERS={len(census['server'])}",""]
    for side in ("client","server"):
        lines.append("["+side.upper()+" CALLERS]")
        for r in census[side]:lines.append(f"RVA=0x{r['call_rva']:08X} RETURN=0x{r['return_va']:08X} KIND={r['caller_kind']} POLICY={r['policy']} SPECIAL={r.get('special_label') or '-'} CLASSES={','.join(r.get('class_names') or []) or '-'}")
        lines.append("")
    lines.append("[VEngineClient014]");lines.append(f"STRING_COUNT={len(ei['string_vas'])}");lines.append(f"INTERFACE_REG_CANDIDATES={len(ei['candidates'])}")
    for i,c in enumerate(ei["candidates"]):
        s12=c.get("slots",{}).get("12",{});s19=c.get("slots",{}).get("19",{})
        lines.append(f"CANDIDATE={i} REG=0x{c['interface_reg_va']:08X} CREATE=0x{c['create_fn_va']:08X} OBJECT={('0x%08X'%c['object_va']) if c.get('object_va') else 'UNRESOLVED'} VTABLE={('0x%08X'%c['vtable_va']) if c.get('vtable_va') else 'UNRESOLVED'} SLOT12={('0x%08X'%s12.get('function_va')) if s12.get('function_va') else 'UNRESOLVED'} SLOT19={('0x%08X'%s19.get('function_va')) if s19.get('function_va') else 'UNRESOLVED'}")
    (out/"CALLER_CENSUS.txt").write_text("\n".join(lines)+"\n",encoding="utf-8")
    fails=[]
    if not census["client"]:fails.append("CLIENT_NO_DIRECT_CALLERS")
    if not census["server"]:fails.append("SERVER_NO_DIRECT_CALLERS")
    if not any(r["call_rva"]==0x003F6989 for r in census["client"]):fails.append("CLIENT_003F6989_MISSING")
    if not any(r["call_rva"]==0x005A7B09 for r in census["server"]):fails.append("SERVER_005A7B09_MISSING")
    te=next((r for r in census["client"] if r["call_rva"]==0x003F6989),None)
    if te and te["caller_kind"]!="NETWORK_TE":fails.append("CLIENT_003F6989_NOT_RTTI_TE")
    if not ei["candidates"]:fails.append("VENGINECLIENT014_NOT_RESOLVED")
    (out/"FAILED_INVARIANTS_OWNERSHIP.txt").write_text(("NONE\n" if not fails else "\n".join(fails)+"\n"),encoding="utf-8")
    (out/"OWNERSHIP_GATE_POLICY.txt").write_text("NETWORK_TE=REJECT\nUNKNOWN=REJECT_PENDING\nWEAPON_FAMILY=CANDIDATE_REQUIRES_LOCAL_PLAYER\nLOCAL_PLAYER_SOURCE_CANDIDATE=VEngineClient014_SLOT12\nNO_LIVE_TICKET=1\n",encoding="utf-8")
    print("SCAN=PASS");print(f"CLIENT_CALLERS={len(census['client'])}");print(f"SERVER_CALLERS={len(census['server'])}");print("FAILED="+("|".join(fails) if fails else "NONE"))

def main():
    ap=argparse.ArgumentParser();ap.add_argument("--selftest",action="store_true");ap.add_argument("--client");ap.add_argument("--server");ap.add_argument("--engine");ap.add_argument("--out");ap.add_argument("--dataflow",action="store_true");ap.add_argument("--facts");ap.add_argument("--caller-dir")
    a=ap.parse_args()
    if a.selftest:selftest();return
    if a.dataflow:
        facts=json.loads(Path(a.facts).read_text(encoding="utf-8"));caller_dataflow(facts,a.caller_dir,a.out);print("DATAFLOW=PASS");return
    scan(a)
if __name__=="__main__":main()
'@

try{
    $StageClient=Join-Path $Input 'client.dll'
    $StageServer=Join-Path $Input 'server.dll'
    $StageEngine=Join-Path $Input 'engine.dll'
    Copy-Item -LiteralPath $Client -Destination $StageClient -Force
    Copy-Item -LiteralPath $Server -Destination $StageServer -Force
    Copy-Item -LiteralPath $Engine -Destination $StageEngine -Force
    if((Get-FileHash $StageClient -Algorithm SHA256).Hash.ToUpperInvariant()-ne$ExpectedClient){throw 'staged client hash mismatch'}
    if((Get-FileHash $StageServer -Algorithm SHA256).Hash.ToUpperInvariant()-ne$ExpectedServer){throw 'staged server hash mismatch'}
    Write-Host '[SAFE STAGE] exact copies verified.' -ForegroundColor Green

    $Analyzer=Join-Path $Safe 'csgovr_fire_ownership.py'
    [IO.File]::WriteAllText($Analyzer,$PyCode,[Text.UTF8Encoding]::new($false))
    Copy-Item -LiteralPath $Analyzer -Destination (Join-Path $Out 'ANALYZER_SOURCE.py')

    & $Python @PyArgs $Analyzer --selftest | Tee-Object -FilePath (Join-Path $Out 'STATIC_SELFTEST.txt')
    if($LASTEXITCODE-ne0){throw "analyzer selftest exit=$LASTEXITCODE"}

    & $Python @PyArgs $Analyzer --client $StageClient --server $StageServer --engine $StageEngine --out $Out | Tee-Object -FilePath (Join-Path $Out 'OWNERSHIP_ANALYZER_OUTPUT.txt')
    if($LASTEXITCODE-ne0){throw "ownership analyzer exit=$LASTEXITCODE"}

    $FactsPath=Join-Path $Out 'OWNERSHIP_COLD_FACTS.json'
    $Facts=Get-Content -LiteralPath $FactsPath -Raw|ConvertFrom-Json

    foreach($side in @('client','server')){
        $bin=if($side-eq'client'){$StageClient}else{$StageServer}
        foreach($r in @($Facts.callers.$side)){
            $rva=[uint64]$r.call_rva
            $dst=Join-Path $CallerDir (("{0}_CALL_{1:X8}.txt" -f $side.ToUpperInvariant(),$rva))
            Run-RangeDisasm (($side.ToUpperInvariant())+'_CALL_'+('{0:X8}' -f $rva)) $bin ([uint64]$r.window_start_va) ([uint64]$r.window_stop_va) $dst
        }
    }

    & $Python @PyArgs $Analyzer --dataflow --facts $FactsPath --caller-dir $CallerDir --out $Out | Tee-Object -FilePath (Join-Path $Out 'ALL_CALLERS_DATAFLOW_OUTPUT.txt')
    if($LASTEXITCODE-ne0){throw "caller dataflow exit=$LASTEXITCODE"}

    $idx=0
    if($Facts.vengineclient014){
        foreach($c in @($Facts.vengineclient014.candidates)){
            if($c.create_fn_va){
                $f=[uint64]$c.create_fn_va
                $start=if($f-gt0x100){$f-0x100}else{$f}
                Run-RangeDisasm ('ENGINE_CREATEFN_'+$idx) $StageEngine ([uint64]$start) ([uint64]($f+0x500)) (Join-Path $EngineDir (("CREATEFN_{0:D2}_{1:X8}.txt" -f $idx,$f)))
            }
            foreach($slot in @('12','18','19')){
                $s=$c.slots.$slot
                if($s -and $s.function_va -and $s.is_exec){
                    $f=[uint64]$s.function_va;$start=if($f-gt0x100){$f-0x100}else{$f}
                    Run-RangeDisasm ('ENGINE_SLOT_'+$slot) $StageEngine ([uint64]$start) ([uint64]($f+0x600)) (Join-Path $EngineDir (("VENGINE014_SLOT{0}_{1:X8}.txt" -f $slot,$f)))
                }
            }
            $idx++
        }
    }

    @(
      'MISSING FACT: exact local-shot ownership across every frozen FX_FireBullets caller.',
      'WHY NOT COLD: N/A; frozen binaries + caller census + RTTI + VEngineClient014 can be mined offline.',
      'PASS: all direct callers, caller families, ECX/EDX/stack evidence, local-player interface evidence, special caller evidence.',
      'FAIL: hash mismatch, running target, incomplete caller census, or wrong-object caller silently promoted.',
      'BASELINE MUST HOLD: exact frozen hashes; no game/HLAE/OpenXR/injection/debugger; no mutation; no LIVE ticket.'
    )|Set-Content -LiteralPath (Join-Path $Out 'PREFLIGHT.txt') -Encoding utf8

    @(
      'BASE_PACKAGE=CSGOVR_FX_FIRE_ABI_SLOT_CLOSURE_v1.1.1_ABI_FAMILY_COLD.zip',
      'BASELINE_REUSED=exact hashes + objdump tool resolution + read-only staged DLL analysis + result ZIP packaging',
      'ONLY_DELTA=full caller census + RTTI caller family + all-caller ECX/EDX evidence + VEngineClient014 slot12/18/19 closure',
      'GAME_EXECUTION=DENIED',
      'LIVE_TICKET=NONE'
    )|Set-Content -LiteralPath (Join-Path $Out 'LINEAGE.txt') -Encoding utf8

    @(
      'SOURCE_CANDIDATE_ONLY:',
      'VEngineClient014 slot12 = GetLocalPlayer',
      'VEngineClient014 slot18 = GetViewAngles',
      'VEngineClient014 slot19 = SetViewAngles',
      'Current v1.8.1 lineage already uses this same VEngineClient014 interface and slot19.',
      'Promotion requires frozen InterfaceReg/vtable/body review.',
      '',
      'LATE_ABI_CANDIDATE_ONLY:',
      'ECX=iPlayerIndex',
      'EDX=CBaseCombatWeapon*',
      '[ESP+04]=CEconItemView*',
      '[ESP+08]=vOrigin',
      '[ESP+0C]=vAngles'
    )|Set-Content -LiteralPath (Join-Path $Out 'SOURCE_CANDIDATES.txt') -Encoding utf8

    $ownFail=(Get-Content -LiteralPath (Join-Path $Out 'FAILED_INVARIANTS_OWNERSHIP.txt') -Raw).Trim()
    @(
      'CSGOVR FX FIRE OWNERSHIP CLOSURE v1.2.0 COLD OMNI',
      'COLD_ONLY=1',
      ('GAME_ROOT='+$Game),
      ('CLIENT_SHA256='+$csha),
      ('SERVER_SHA256='+$ssha),
      ('ENGINE_SHA256='+$esha),
      ('CLIENT_DIRECT_CALLERS='+$Facts.caller_counts.client),
      ('SERVER_DIRECT_CALLERS='+$Facts.caller_counts.server),
      'NETWORK_TE_POLICY=REJECT',
      'UNKNOWN_POLICY=REJECT_PENDING',
      'WEAPON_FAMILY_POLICY=CANDIDATE_REQUIRES_LOCAL_PLAYER',
      'LOCAL_PLAYER_SOURCE_CANDIDATE=VEngineClient014_SLOT12',
      ('FAILED_INVARIANTS='+$ownFail.Replace("`r",' ').Replace("`n",' | ')),
      'NO_OFFSET_ONLY_REPAIR=1',
      'LIVE_TICKET=NONE'
    )|Set-Content -LiteralPath (Join-Path $Out 'STATUS.txt') -Encoding utf8

    $ResultZip=Join-Path $Root ('CSGOVR_FX_FIRE_OWNERSHIP_CLOSURE_RESULT_'+$Stamp+'.zip')
    if(Test-Path -LiteralPath $ResultZip){Remove-Item -LiteralPath $ResultZip -Force}
    Add-Type -AssemblyName System.IO.Compression.FileSystem
    [IO.Compression.ZipFile]::CreateFromDirectory($Out,$ResultZip,[IO.Compression.CompressionLevel]::Optimal,$false)
    $zsha=(Get-FileHash -LiteralPath $ResultZip -Algorithm SHA256).Hash
    Write-Host ''
    Write-Host '[COLD OWNERSHIP CAPTURE COMPLETE]' -ForegroundColor Green
    Write-Host ('RESULT='+$ResultZip)
    Write-Host ('SHA256='+$zsha)
    exit 0
}
finally{
    try{Remove-Item -LiteralPath $Safe -Recurse -Force -ErrorAction SilentlyContinue}catch{}
}
