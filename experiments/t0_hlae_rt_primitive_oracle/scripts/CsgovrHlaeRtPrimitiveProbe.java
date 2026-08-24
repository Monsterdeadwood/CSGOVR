// CSGOVR T0 exact-HLAE render-target primitive census v1.1.3
// @category CSGOVR
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import java.io.*;
import java.util.*;

public class CsgovrHlaeRtPrimitiveProbe extends GhidraScript {
    private static final long DORENDERVIEW_RVA = 0x22340L;

    private static class Hit {
        Function f;
        int score;
        int totalCalls;
        int setRtCount;
        int getRtCount;
        int stretchCount;
        int createTextureCount;
        int createQueryCount;
        int queryIssueCount;
        int queryGetDataCount;
        ArrayList<Instruction> calls = new ArrayList<Instruction>();

        boolean setRt() { return setRtCount > 0; }
        boolean getRt() { return getRtCount > 0; }
        boolean stretch() { return stretchCount > 0; }
        boolean createTexture() { return createTextureCount > 0; }
        boolean createQuery() { return createQueryCount > 0; }
        boolean queryIssue() { return queryIssueCount > 0; }
        boolean queryGetData() { return queryGetDataCount > 0; }
    }

    private String text(Instruction ins) {
        StringBuilder sb = new StringBuilder(ins.getMnemonicString());
        for (int i = 0; i < ins.getNumOperands(); i++) {
            sb.append(i == 0 ? " " : ", ");
            sb.append(ins.getDefaultOperandRepresentation(i));
        }
        return sb.toString().replace("\r", " ").replace("\n", " ");
    }

    private boolean hasDisp(String s, int disp) {
        String x = s.toLowerCase().replace(" ", "");
        String h = Integer.toHexString(disp).toLowerCase();
        return x.contains("+0x" + h) || x.contains("+" + h + "h") ||
               x.contains("[0x" + h + "]") || x.contains("[" + h + "h]");
    }

    private int callCount(Listing listing, Function f) {
        int n = 0;
        InstructionIterator ii = listing.getInstructions(f.getBody(), true);
        while (ii.hasNext()) if (ii.next().getFlowType().isCall()) n++;
        return n;
    }

    private String bytesHex(Address start, int wanted) {
        try {
            int n = Math.max(0, Math.min(wanted, 64));
            byte[] b = new byte[n];
            int got = currentProgram.getMemory().getBytes(start, b);
            if (got <= 0) return "NONE";
            StringBuilder sb = new StringBuilder(got * 2);
            for (int i = 0; i < got; i++) sb.append(String.format("%02X", b[i] & 0xff));
            return sb.toString();
        } catch (Throwable t) {
            return "ERROR";
        }
    }

    private void dumpFunctionInstructions(PrintWriter out, Listing listing, Function f, int maxInstructions) {
        InstructionIterator ii = listing.getInstructions(f.getBody(), true);
        int n = 0;
        while (ii.hasNext() && n < maxInstructions) {
            Instruction ins = ii.next();
            out.println("      " + ins.getAddress() + "  " + text(ins));
            n++;
        }
        if (ii.hasNext()) out.println("      ... instruction dump truncated ...");
    }

    private void ctx(PrintWriter out, Listing listing, Address center, int before, int after) {
        Instruction c = listing.getInstructionAt(center);
        if (c == null) return;
        ArrayList<Instruction> pre = new ArrayList<Instruction>();
        Instruction p = c;
        for (int i = 0; i < before; i++) {
            p = p.getPrevious();
            if (p == null) break;
            pre.add(p);
        }
        Collections.reverse(pre);
        for (Instruction x : pre) out.println("    " + x.getAddress() + "  " + text(x));
        out.println("  > " + c.getAddress() + "  " + text(c));
        Instruction n = c;
        for (int i = 0; i < after; i++) {
            n = n.getNext();
            if (n == null) break;
            out.println("    " + n.getAddress() + "  " + text(n));
        }
    }

    private String decompile(DecompInterface di, Function f) {
        try {
            DecompileResults dr = di.decompileFunction(f, 90, monitor);
            if (dr != null && dr.decompileCompleted() && dr.getDecompiledFunction() != null)
                return dr.getDecompiledFunction().getC();
            return "DECOMPILE_FAILED " + (dr == null ? "null" : dr.getErrorMessage());
        } catch (Throwable t) {
            return "DECOMPILE_EXCEPTION " + t.toString();
        }
    }

    private String roleHint(Hit h) {
        if (h.setRt() && h.getRt() && h.stretch()) return "RT_POP_MEMBER_FINALIST";
        if (h.setRt() && h.getRt() && !h.stretch()) return "RT_PUSH_MEMBER_FINALIST";
        if (!h.setRt() && !h.getRt() && h.queryIssue() && h.queryGetData() && h.totalCalls <= 16)
            return "GPU_WAIT_MEMBER_FINALIST";
        if (h.createQuery()) return "QUERY_CREATE_CANDIDATE";
        if (h.createTexture()) return "CREATE_TEXTURE_CANDIDATE";
        if (h.stretch()) return "STRETCHRECT_CANDIDATE";
        if (h.setRt() || h.getRt()) return "RT_STATE_CANDIDATE";
        return "OTHER";
    }

    private void printThinCallers(PrintWriter out, Listing listing, FunctionManager fm,
                                  ReferenceManager rm, Function target, Address base,
                                  DecompInterface di) {
        out.println("THIN_CALLER_CANDIDATES");
        int shown = 0;
        HashSet<Address> seen = new HashSet<Address>();
        for (ReferenceIterator ri = rm.getReferencesTo(target.getEntryPoint()); ri.hasNext();) {
            Reference r = ri.next();
            if (!r.getReferenceType().isCall()) continue;
            Function cf = fm.getFunctionContaining(r.getFromAddress());
            if (cf == null || seen.contains(cf.getEntryPoint())) continue;
            seen.add(cf.getEntryPoint());
            long bytes = cf.getBody().getNumAddresses();
            int calls = callCount(listing, cf);
            if (bytes <= 160 && calls <= 4) {
                long rva = cf.getEntryPoint().subtract(base);
                int sigLen = (int)Math.min(32L, Math.max(6L, bytes));
                String sig = bytesHex(cf.getEntryPoint(), sigLen);
                out.println("  RVA=0x" + Long.toHexString(rva) +
                            " Entry=" + cf.getEntryPoint() +
                            " Name=" + cf.getName() +
                            " BodyBytes=" + bytes + " Calls=" + calls +
                            " RefFrom=" + r.getFromAddress() +
                            " SigLen=" + sigLen + " SigHex=" + sig);
                out.println("  THIN_INSTRUCTIONS_BEGIN");
                dumpFunctionInstructions(out, listing, cf, 80);
                out.println("  THIN_INSTRUCTIONS_END");
                out.println("  THIN_DECOMPILE_BEGIN");
                out.println(decompile(di, cf));
                out.println("  THIN_DECOMPILE_END");
                shown++;
                if (shown >= 12) break;
            }
        }
        out.println("ThinCallerCountShown=" + shown);
    }

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 1) throw new IllegalArgumentException("output path required");

        Program pg = currentProgram;
        Listing listing = pg.getListing();
        FunctionManager fm = pg.getFunctionManager();
        ReferenceManager rm = pg.getReferenceManager();
        Address base = pg.getImageBase();
        PrintWriter out = new PrintWriter(new OutputStreamWriter(new FileOutputStream(args[0]), "UTF-8"));

        out.println("CSGOVR_T0_HLAE_RT_PRIMITIVE_SCAN_V1_1_3");
        out.println("Program=" + pg.getName());
        out.println("ImageBase=" + base);
        out.println("DoRenderViewRVA=0x22340");
        out.println("D3D9_VTABLE_SET_RT=slot37/disp0x94");
        out.println("D3D9_VTABLE_GET_RT=slot38/disp0x98");
        out.println("D3D9_VTABLE_STRETCHRECT=slot34/disp0x88");
        out.println("D3D9_VTABLE_CREATETEXTURE=slot23/disp0x5c");
        out.println("D3D9_VTABLE_CREATEQUERY=slot118/disp0x1d8");
        out.println("D3D9_QUERY_ISSUE=slot6/disp0x18");
        out.println("D3D9_QUERY_GETDATA=slot7/disp0x1c");
        out.println("ROLE_POLICY=SOURCE_GUIDED_HEURISTIC_ONLY_NO_AUTOPROMOTION");

        ArrayList<Hit> hits = new ArrayList<Hit>();
        FunctionIterator fit = fm.getFunctions(true);
        while (fit.hasNext()) {
            Function f = fit.next();
            Hit h = new Hit();
            h.f = f;
            InstructionIterator ii = listing.getInstructions(f.getBody(), true);
            while (ii.hasNext()) {
                Instruction ins = ii.next();
                if (!ins.getFlowType().isCall()) continue;
                h.totalCalls++;
                String t = text(ins);
                boolean relevant = false;
                if (hasDisp(t, 0x94)) { h.setRtCount++; relevant = true; }
                if (hasDisp(t, 0x98)) { h.getRtCount++; relevant = true; }
                if (hasDisp(t, 0x88)) { h.stretchCount++; relevant = true; }
                if (hasDisp(t, 0x5c)) { h.createTextureCount++; relevant = true; }
                if (hasDisp(t, 0x1d8)) { h.createQueryCount++; relevant = true; }
                if (hasDisp(t, 0x18)) { h.queryIssueCount++; relevant = true; }
                if (hasDisp(t, 0x1c)) { h.queryGetDataCount++; relevant = true; }
                if (relevant) h.calls.add(ins);
            }

            if (h.setRt()) h.score += 45;
            if (h.getRt()) h.score += 45;
            if (h.stretch()) h.score += 18;
            if (h.createTexture()) h.score += 12;
            if (h.createQuery()) h.score += 18;
            if (h.setRt() && h.getRt()) h.score += 80;
            if (h.setRt() && h.getRt() && h.stretch()) h.score += 90;
            if (h.setRt() && h.getRt() && !h.stretch()) h.score += 70;
            if (h.queryIssue() && h.queryGetData() && h.totalCalls <= 16) h.score += 120;
            if (h.score > 0) hits.add(h);
        }

        Collections.sort(hits, new Comparator<Hit>() {
            public int compare(Hit a, Hit b) {
                int c = Integer.compare(b.score, a.score);
                if (c != 0) return c;
                return a.f.getEntryPoint().compareTo(b.f.getEntryPoint());
            }
        });

        DecompInterface di = new DecompInterface();
        di.toggleCCode(true);
        di.toggleSyntaxTree(true);
        di.openProgram(pg);

        Hit bestPush = null, bestPop = null, bestWait = null;
        for (Hit h : hits) {
            String role = roleHint(h);
            if (bestPush == null && role.equals("RT_PUSH_MEMBER_FINALIST")) bestPush = h;
            if (bestPop == null && role.equals("RT_POP_MEMBER_FINALIST")) bestPop = h;
            if (bestWait == null && role.equals("GPU_WAIT_MEMBER_FINALIST")) bestWait = h;
        }

        out.println("================================================================================");
        out.println("ROLE_SUMMARY_HEURISTIC_ONLY");
        if (bestPush != null)
            out.println("BestPushMemberRVA=0x" + Long.toHexString(bestPush.f.getEntryPoint().subtract(base)) +
                        " Entry=" + bestPush.f.getEntryPoint() + " Score=" + bestPush.score);
        else out.println("BestPushMemberRVA=NONE");
        if (bestPop != null)
            out.println("BestPopMemberRVA=0x" + Long.toHexString(bestPop.f.getEntryPoint().subtract(base)) +
                        " Entry=" + bestPop.f.getEntryPoint() + " Score=" + bestPop.score);
        else out.println("BestPopMemberRVA=NONE");
        if (bestWait != null)
            out.println("BestGpuWaitMemberRVA=0x" + Long.toHexString(bestWait.f.getEntryPoint().subtract(base)) +
                        " Entry=" + bestWait.f.getEntryPoint() + " Score=" + bestWait.score);
        else out.println("BestGpuWaitMemberRVA=NONE");
        out.println("BoundedCoreFinalists=" + ((bestPush != null && bestPop != null) ? "YES" : "NO"));
        out.println("SemanticVerdict=" + ((bestPush != null && bestPop != null) ? "BOUNDED_FINALISTS" : "AMBIGUOUS"));

        ArrayList<Hit> ordered = new ArrayList<Hit>();
        for (Hit h : hits) if (!roleHint(h).equals("OTHER") && roleHint(h).contains("FINALIST")) ordered.add(h);
        for (Hit h : hits) if (!ordered.contains(h)) ordered.add(h);

        out.println("CandidateCount=" + hits.size());
        int n = 0;
        for (Hit h : ordered) {
            n++;
            if (n > 50) break;
            long rva = h.f.getEntryPoint().subtract(base);
            out.println("================================================================================");
            out.println("[CANDIDATE " + n + "]");
            out.println("Entry=" + h.f.getEntryPoint());
            out.println("RVA=0x" + Long.toHexString(rva));
            out.println("Name=" + h.f.getName());
            out.println("Score=" + h.score);
            out.println("SourceRoleHint=" + roleHint(h));
            out.println("TotalCalls=" + h.totalCalls);
            out.println("SetRT=" + h.setRtCount + " GetRT=" + h.getRtCount +
                        " StretchRect=" + h.stretchCount +
                        " CreateTexture=" + h.createTextureCount +
                        " CreateQuery=" + h.createQueryCount +
                        " QueryIssue=" + h.queryIssueCount +
                        " QueryGetData=" + h.queryGetDataCount);
            out.println("BodyBytes=" + h.f.getBody().getNumAddresses());

            out.println("RELEVANT_CALLS");
            for (Instruction ins : h.calls) {
                out.println("  " + ins.getAddress() + "  " + text(ins));
                ctx(out, listing, ins.getAddress(), 6, 6);
            }

            out.println("CALLERS");
            int callers = 0;
            for (ReferenceIterator ri = rm.getReferencesTo(h.f.getEntryPoint()); ri.hasNext();) {
                Reference r = ri.next();
                if (!r.getReferenceType().isCall()) continue;
                Function cf = fm.getFunctionContaining(r.getFromAddress());
                out.println("  from=" + r.getFromAddress() + " caller=" +
                            (cf == null ? "<none>" : cf.getEntryPoint()+"/"+cf.getName()));
                callers++;
                if (callers >= 30) break;
            }
            printThinCallers(out, listing, fm, rm, h.f, base, di);

            out.println("DECOMPILE_BEGIN");
            out.println(decompile(di, h.f));
            out.println("DECOMPILE_END");
        }

        Address doRv = base.add(DORENDERVIEW_RVA);
        out.println("================================================================================");
        out.println("DORENDERVIEW_ANCHOR");
        out.println("VA=" + doRv);
        Function doF = fm.getFunctionAt(doRv);
        if (doF != null) {
            out.println("Function=" + doF.getName());
            out.println("DECOMPILE_BEGIN");
            out.println(decompile(di, doF));
            out.println("DECOMPILE_END");
        } else {
            out.println("FunctionAtAnchor=NONE");
        }

        out.println("================================================================================");
        out.println("SYMBOL_NAME_HARVEST");
        String[] needles = new String[] {
            "AfxD3d9PushRenderTargetEx", "AfxD3d9PopRenderTarget", "AfxGetRenderTargetSurface",
            "AfxGetDirect3DDevice9", "AfxD3D_WaitForGPU", "DrawingThread_SetRenderTarget",
            "DrawingThread_UnsetRenderTarget", "DrawingThread_GetOrCreateRenderTargetSurface"
        };
        SymbolIterator si = pg.getSymbolTable().getAllSymbols(true);
        int symbolHits = 0;
        while (si.hasNext()) {
            Symbol s = si.next();
            String sn = s.getName(true);
            for (String needle : needles) {
                if (sn.toLowerCase().contains(needle.toLowerCase())) {
                    out.println("  " + s.getAddress() + "  " + sn + " source=" + s.getSource());
                    symbolHits++;
                    break;
                }
            }
        }
        out.println("SymbolHits=" + symbolHits);

        di.dispose();
        out.println("SCAN_COMPLETE=YES");
        out.close();
        println("CSGOVR T0 HLAE RT primitive scan v1.1.3 complete");
    }
}
