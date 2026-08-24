// CSGOVR T0 exact-HLAE native stream capture seam oracle v1.0.0
// @category CSGOVR
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import java.io.*;
import java.util.*;

public class CsgovrHlaeCaptureSeamProbe extends GhidraScript {
    private static class Hit {
        Function f;
        int totalCalls;
        int getRtData;
        int stretchRect;
        int createRenderTarget;
        int createOffscreen;
        int getDeviceSurface;
        int score;
        ArrayList<Instruction> relevant = new ArrayList<Instruction>();
    }

    private String text(Instruction ins) {
        StringBuilder sb = new StringBuilder(ins.getMnemonicString());
        for (int i=0; i<ins.getNumOperands(); ++i) {
            sb.append(i==0 ? " " : ", ");
            sb.append(ins.getDefaultOperandRepresentation(i));
        }
        return sb.toString().replace("\r"," ").replace("\n"," ");
    }

    private boolean hasDisp(String s, int disp) {
        String x=s.toLowerCase().replace(" ","");
        String h=Integer.toHexString(disp).toLowerCase();
        return x.contains("+0x"+h) || x.contains("+"+h+"h") ||
               x.contains("[0x"+h+"]") || x.contains("["+h+"h]");
    }

    private String bytesHex(Address start, int wanted) {
        try {
            int n=Math.max(0,Math.min(wanted,64));
            byte[] b=new byte[n];
            int got=currentProgram.getMemory().getBytes(start,b);
            if(got<=0) return "NONE";
            StringBuilder sb=new StringBuilder(got*2);
            for(int i=0;i<got;++i) sb.append(String.format("%02X",b[i]&0xff));
            return sb.toString();
        } catch(Throwable t) { return "ERROR"; }
    }

    private String decompile(DecompInterface di, Function f) {
        try {
            DecompileResults dr=di.decompileFunction(f,90,monitor);
            if(dr!=null && dr.decompileCompleted() && dr.getDecompiledFunction()!=null)
                return dr.getDecompiledFunction().getC();
            return "DECOMPILE_FAILED " + (dr==null?"null":dr.getErrorMessage());
        } catch(Throwable t) { return "DECOMPILE_EXCEPTION "+t; }
    }

    private void score(Hit h) {
        int s=0;
        // IDirect3DDevice9 slots on x86:
        // GetRenderTargetData=32 => 0x80
        // StretchRect=34 => 0x88
        // CreateRenderTarget=28 => 0x70
        // CreateOffscreenPlainSurface=36 => 0x90
        if(h.getRtData>0) s += 120 + 30*Math.min(h.getRtData,2);
        if(h.stretchRect>0) s += 150;
        if(h.getRtData>0 && h.stretchRect>0) s += 300;
        // Exact pinned source CAfxD3D9Capture::Capture contains both branches,
        // while its helper methods own RT/offscreen creation. Penalize creators.
        if(h.createRenderTarget>0) s -= 80;
        if(h.createOffscreen>0) s -= 80;
        long body=h.f.getBody().getNumAddresses();
        if(h.getRtData>0 && h.stretchRect>0 && body>=80 && body<=900) s += 80;
        if(h.totalCalls>=4 && h.totalCalls<=40) s += 30;
        h.score=s;
    }

    private void dump(PrintWriter out, Hit h, Address base, FunctionManager fm,
                      ReferenceManager rm, DecompInterface di) {
        long rva=h.f.getEntryPoint().subtract(base);
        long body=h.f.getBody().getNumAddresses();
        int sigLen=(int)Math.min(32L,Math.max(8L,body));
        out.println("================================================================================");
        out.println("RVA=0x"+Long.toHexString(rva));
        out.println("Entry="+h.f.getEntryPoint());
        out.println("Name="+h.f.getName());
        out.println("BodyBytes="+body);
        out.println("TotalCalls="+h.totalCalls);
        out.println("GetRenderTargetData="+h.getRtData);
        out.println("StretchRect="+h.stretchRect);
        out.println("CreateRenderTarget="+h.createRenderTarget);
        out.println("CreateOffscreenPlainSurface="+h.createOffscreen);
        out.println("Score="+h.score);
        out.println("EntrySigHex="+bytesHex(h.f.getEntryPoint(),sigLen));
        out.println("RELEVANT_CALLS");
        for(Instruction ins:h.relevant) out.println("  "+ins.getAddress()+"  "+text(ins));

        out.println("DIRECT_CALLEES");
        HashSet<Address> seen=new HashSet<Address>();
        InstructionIterator ii=currentProgram.getListing().getInstructions(h.f.getBody(),true);
        while(ii.hasNext()) {
            Instruction ins=ii.next();
            if(!ins.getFlowType().isCall()) continue;
            Address[] flows=ins.getFlows();
            if(flows==null || flows.length!=1) continue;
            Address dst=flows[0];
            if(!seen.add(dst)) continue;
            Function cf=fm.getFunctionAt(dst);
            if(cf!=null) {
                long crva=cf.getEntryPoint().subtract(base);
                out.println("  callsite="+ins.getAddress()+" calleeRVA=0x"+Long.toHexString(crva)+
                            " name="+cf.getName()+" sig="+bytesHex(cf.getEntryPoint(),16));
            }
        }

        out.println("CALLERS");
        int shown=0;
        for(ReferenceIterator ri=rm.getReferencesTo(h.f.getEntryPoint());ri.hasNext();) {
            Reference r=ri.next();
            if(!r.getReferenceType().isCall()) continue;
            Function cf=fm.getFunctionContaining(r.getFromAddress());
            out.println("  from="+r.getFromAddress()+" caller="+
                        (cf==null?"<none>":cf.getEntryPoint()+"/"+cf.getName()));
            if(++shown>=30) break;
        }
        out.println("DECOMPILE_BEGIN");
        out.println(decompile(di,h.f));
        out.println("DECOMPILE_END");
    }

    @Override public void run() throws Exception {
        String[] args=getScriptArgs();
        if(args.length<1) throw new IllegalArgumentException("output path required");
        Program pg=currentProgram;
        Listing listing=pg.getListing();
        FunctionManager fm=pg.getFunctionManager();
        ReferenceManager rm=pg.getReferenceManager();
        Address base=pg.getImageBase();
        PrintWriter out=new PrintWriter(new OutputStreamWriter(new FileOutputStream(args[0]),"UTF-8"));
        out.println("CSGOVR_T0_HLAE_NATIVE_STREAM_CAPTURE_SEAM_V1_0_0");
        out.println("ImageBase="+base);
        out.println("SOURCE_MODEL=CAfxD3D9Capture::Capture uses tracked RT; MSAA branch StretchRect then GetRenderTargetData; non-MSAA direct GetRenderTargetData");
        out.println("ROLE_POLICY=GetRenderTargetData(0x80)+StretchRect(0x88), helpers with CreateRT/CreateOffscreen penalized");

        ArrayList<Hit> hits=new ArrayList<Hit>();
        FunctionIterator fit=fm.getFunctions(true);
        while(fit.hasNext()) {
            Function f=fit.next();
            Hit h=new Hit(); h.f=f;
            InstructionIterator ii=listing.getInstructions(f.getBody(),true);
            while(ii.hasNext()) {
                Instruction ins=ii.next();
                if(!ins.getFlowType().isCall()) continue;
                h.totalCalls++;
                String t=text(ins);
                boolean rel=false;
                if(hasDisp(t,0x80)) { h.getRtData++; rel=true; }
                if(hasDisp(t,0x88)) { h.stretchRect++; rel=true; }
                if(hasDisp(t,0x70)) { h.createRenderTarget++; rel=true; }
                if(hasDisp(t,0x90)) { h.createOffscreen++; rel=true; }
                if(rel) h.relevant.add(ins);
            }
            score(h);
            if(h.getRtData>0 || h.stretchRect>0) hits.add(h);
        }
        Collections.sort(hits,new Comparator<Hit>() {
            public int compare(Hit a, Hit b) {
                int c=Integer.compare(b.score,a.score);
                return c!=0?c:a.f.getEntryPoint().compareTo(b.f.getEntryPoint());
            }
        });

        Hit best=null;
        for(Hit h:hits) {
            if(h.getRtData>0 && h.stretchRect>0) { best=h; break; }
        }
        out.println("================================================================================");
        out.println("SUMMARY");
        if(best==null) {
            out.println("BestCaptureRVA=NONE");
            out.println("BestCaptureSigHex=NONE");
            out.println("SemanticVerdict=AMBIGUOUS");
        } else {
            int sigLen=(int)Math.min(32L,Math.max(8L,best.f.getBody().getNumAddresses()));
            out.println("BestCaptureRVA=0x"+Long.toHexString(best.f.getEntryPoint().subtract(base)));
            out.println("BestCaptureSigHex="+bytesHex(best.f.getEntryPoint(),sigLen));
            out.println("BestGetRenderTargetDataCalls="+best.getRtData);
            out.println("BestStretchRectCalls="+best.stretchRect);
            out.println("SemanticVerdict=BOUNDED_CAPTURE_FINALIST_PENDING_SOURCE_REVIEW");
        }

        DecompInterface di=new DecompInterface();
        di.toggleCCode(true); di.toggleSyntaxTree(true); di.openProgram(pg);
        int n=0;
        for(Hit h:hits) {
            dump(out,h,base,fm,rm,di);
            if(++n>=12) break;
        }
        di.dispose();
        out.println("================================================================================");
        out.println("SCAN_COMPLETE=YES");
        out.close();
        println("CSGOVR native stream capture seam scan complete");
    }
}
