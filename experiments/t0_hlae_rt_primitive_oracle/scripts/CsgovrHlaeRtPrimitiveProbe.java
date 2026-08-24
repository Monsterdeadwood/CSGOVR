// CSGOVR T0 exact-HLAE render-target primitive census
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
        boolean setRt;
        boolean getRt;
        boolean stretch;
        boolean createTexture;
        boolean createQuery;
        ArrayList<Instruction> calls = new ArrayList<Instruction>();
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
        // Ghidra operand rendering varies across analyzers / syntax. Keep this
        // deliberately tolerant, then rely on decompile + source semantics for promotion.
        return x.contains("+0x" + h) || x.contains("+" + h + "h") ||
               x.contains("[0x" + h + "]") || x.contains("[" + h + "h]");
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

        out.println("CSGOVR_T0_HLAE_RT_PRIMITIVE_SCAN");
        out.println("Program=" + pg.getName());
        out.println("ImageBase=" + base);
        out.println("DoRenderViewRVA=0x22340");
        out.println("D3D9_VTABLE_SET_RT=slot37/disp0x94");
        out.println("D3D9_VTABLE_GET_RT=slot38/disp0x98");
        out.println("D3D9_VTABLE_STRETCHRECT=slot34/disp0x88");
        out.println("D3D9_VTABLE_CREATETEXTURE=slot23/disp0x5c");
        out.println("D3D9_VTABLE_CREATEQUERY=slot118/disp0x1d8");

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
                String t = text(ins);
                boolean relevant = false;
                if (hasDisp(t, 0x94)) { h.setRt = true; relevant = true; }
                if (hasDisp(t, 0x98)) { h.getRt = true; relevant = true; }
                if (hasDisp(t, 0x88)) { h.stretch = true; relevant = true; }
                if (hasDisp(t, 0x5c)) { h.createTexture = true; relevant = true; }
                if (hasDisp(t, 0x1d8)) { h.createQuery = true; relevant = true; }
                if (relevant) h.calls.add(ins);
            }
            if (h.setRt) h.score += 45;
            if (h.getRt) h.score += 45;
            if (h.stretch) h.score += 18;
            if (h.createTexture) h.score += 12;
            if (h.createQuery) h.score += 18;
            if (h.setRt && h.getRt) h.score += 50;
            if (h.setRt && h.getRt && h.stretch) h.score += 20;
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

        out.println("CandidateCount=" + hits.size());
        int n = 0;
        for (Hit h : hits) {
            n++;
            if (n > 40) break;
            long rva = h.f.getEntryPoint().subtract(base);
            out.println("================================================================================");
            out.println("[CANDIDATE " + n + "]");
            out.println("Entry=" + h.f.getEntryPoint());
            out.println("RVA=0x" + Long.toHexString(rva));
            out.println("Name=" + h.f.getName());
            out.println("Score=" + h.score);
            out.println("SetRT=" + h.setRt + " GetRT=" + h.getRt + " StretchRect=" + h.stretch +
                        " CreateTexture=" + h.createTexture + " CreateQuery=" + h.createQuery);
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
                out.println("  from=" + r.getFromAddress() + " caller=" + (cf == null ? "<none>" : cf.getEntryPoint()+"/"+cf.getName()));
                callers++;
                if (callers >= 30) break;
            }

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
        println("CSGOVR T0 HLAE RT primitive scan complete");
    }
}
