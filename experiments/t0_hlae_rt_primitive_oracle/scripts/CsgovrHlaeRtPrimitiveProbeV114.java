// CSGOVR T0 exact-HLAE render-target primitive census v1.1.4
// @category CSGOVR
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import java.io.*;
import java.util.*;

public class CsgovrHlaeRtPrimitiveProbeV114 extends GhidraScript {
    private static class Hit {
        Function f;
        int totalCalls;
        int setRt;
        int getRt;
        int stretch;
        int createTexture;
        int createQuery;
        int issue;
        int getData;
        int score;
        ArrayList<Instruction> relevant = new ArrayList<Instruction>();
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

    private boolean pushEligible(Hit h) {
        // Exact source PushRT does GetRT + SetRT, but does not create its own texture
        // and does not StretchRect. v1.1.3 omitted !CreateTexture and therefore
        // incorrectly ranked the R32F render-target helper at +0x36150 above PushRT.
        return h.setRt > 0 && h.getRt > 0 && h.stretch == 0 && h.createTexture == 0;
    }

    private boolean popEligible(Hit h) {
        return h.setRt > 0 && h.getRt > 0 && h.stretch > 0;
    }

    private boolean waitEligible(Hit h) {
        return h.setRt == 0 && h.getRt == 0 && h.issue > 0 && h.getData > 0 &&
               h.totalCalls <= 16 && h.f.getBody().getNumAddresses() <= 128;
    }

    private String role(Hit h) {
        if (popEligible(h)) return "RT_POP_MEMBER_FINALIST";
        if (pushEligible(h)) return "RT_PUSH_MEMBER_FINALIST";
        if (waitEligible(h)) return "GPU_WAIT_MEMBER_FINALIST";
        if (h.setRt > 0 && h.getRt > 0 && h.createTexture > 0 && h.stretch == 0)
            return "RT_SWITCH_WITH_CREATE_TEXTURE_NOT_PUSH";
        if (h.createQuery > 0) return "QUERY_CREATE_CANDIDATE";
        if (h.createTexture > 0) return "CREATE_TEXTURE_CANDIDATE";
        if (h.stretch > 0) return "STRETCHRECT_CANDIDATE";
        if (h.setRt > 0 || h.getRt > 0) return "RT_STATE_CANDIDATE";
        return "OTHER";
    }

    private void score(Hit h) {
        int s = 0;
        if (h.setRt > 0) s += 45;
        if (h.getRt > 0) s += 45;
        if (h.stretch > 0) s += 18;
        if (h.createTexture > 0) s += 8;
        if (h.createQuery > 0) s += 18;
        if (pushEligible(h)) {
            s += 180;
            if (h.f.getBody().getNumAddresses() <= 256) s += 35;
            if (h.totalCalls <= 10) s += 30;
        }
        if (popEligible(h)) {
            s += 210;
            if (h.f.getBody().getNumAddresses() <= 384) s += 25;
        }
        if (waitEligible(h)) {
            s += 240;
            if (h.f.getBody().getNumAddresses() <= 80) s += 30;
        }
        // Strong anti-pattern discovered by exact-source review: texture-creation
        // helpers can look like PushRT merely because they switch RT0 later.
        if (h.setRt > 0 && h.getRt > 0 && h.createTexture > 0 && h.stretch == 0) s -= 260;
        h.score = s;
    }

    private void dumpCandidate(PrintWriter out, Hit h, Address base, Listing listing,
                               ReferenceManager rm, FunctionManager fm, DecompInterface di) {
        long rva = h.f.getEntryPoint().subtract(base);
        long body = h.f.getBody().getNumAddresses();
        int sigLen = (int)Math.min(32L, Math.max(6L, body));
        out.println("================================================================================");
        out.println("RVA=0x" + Long.toHexString(rva));
        out.println("Entry=" + h.f.getEntryPoint());
        out.println("Name=" + h.f.getName());
        out.println("Role=" + role(h));
        out.println("Score=" + h.score);
        out.println("BodyBytes=" + body);
        out.println("TotalCalls=" + h.totalCalls);
        out.println("SetRT=" + h.setRt + " GetRT=" + h.getRt + " StretchRect=" + h.stretch +
                    " CreateTexture=" + h.createTexture + " CreateQuery=" + h.createQuery +
                    " QueryIssue=" + h.issue + " QueryGetData=" + h.getData);
        out.println("EntrySigLen=" + sigLen);
        out.println("EntrySigHex=" + bytesHex(h.f.getEntryPoint(), sigLen));
        out.println("RELEVANT_CALLS");
        for (Instruction ins : h.relevant) out.println("  " + ins.getAddress() + "  " + text(ins));
        out.println("CALLERS");
        int shown = 0;
        for (ReferenceIterator ri = rm.getReferencesTo(h.f.getEntryPoint()); ri.hasNext();) {
            Reference r = ri.next();
            if (!r.getReferenceType().isCall()) continue;
            Function cf = fm.getFunctionContaining(r.getFromAddress());
            out.println("  from=" + r.getFromAddress() + " caller=" +
                        (cf == null ? "<none>" : cf.getEntryPoint() + "/" + cf.getName()));
            if (++shown >= 30) break;
        }
        out.println("DECOMPILE_BEGIN");
        out.println(decompile(di, h.f));
        out.println("DECOMPILE_END");
    }

    private void summary(PrintWriter out, String prefix, Hit h, Address base) {
        if (h == null) {
            out.println(prefix + "RVA=NONE");
            out.println(prefix + "SigHex=NONE");
            return;
        }
        long body = h.f.getBody().getNumAddresses();
        int sigLen = (int)Math.min(32L, Math.max(6L, body));
        out.println(prefix + "RVA=0x" + Long.toHexString(h.f.getEntryPoint().subtract(base)));
        out.println(prefix + "SigHex=" + bytesHex(h.f.getEntryPoint(), sigLen));
        out.println(prefix + "Score=" + h.score);
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

        out.println("CSGOVR_T0_HLAE_RT_PRIMITIVE_SCAN_V1_1_4");
        out.println("ImageBase=" + base);
        out.println("ROLE_POLICY=EXACT_SOURCE_GUIDED_NO_AUTOPROMOTION");
        out.println("PUSH_POLICY=SetRT+GetRT+NoStretchRect+NoCreateTexture");
        out.println("KNOWN_V113_FALSE_POSITIVE=R32F_HELPER_AT_0x36150");

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
                boolean rel = false;
                if (hasDisp(t, 0x94)) { h.setRt++; rel = true; }
                if (hasDisp(t, 0x98)) { h.getRt++; rel = true; }
                if (hasDisp(t, 0x88)) { h.stretch++; rel = true; }
                if (hasDisp(t, 0x5c)) { h.createTexture++; rel = true; }
                if (hasDisp(t, 0x1d8)) { h.createQuery++; rel = true; }
                if (hasDisp(t, 0x18)) { h.issue++; rel = true; }
                if (hasDisp(t, 0x1c)) { h.getData++; rel = true; }
                if (rel) h.relevant.add(ins);
            }
            score(h);
            if (h.score > 0 || !role(h).equals("OTHER")) hits.add(h);
        }

        Collections.sort(hits, new Comparator<Hit>() {
            public int compare(Hit a, Hit b) {
                int c = Integer.compare(b.score, a.score);
                return c != 0 ? c : a.f.getEntryPoint().compareTo(b.f.getEntryPoint());
            }
        });

        Hit bestPush = null, bestPop = null, bestWait = null;
        for (Hit h : hits) {
            if (bestPush == null && pushEligible(h)) bestPush = h;
            if (bestPop == null && popEligible(h)) bestPop = h;
            if (bestWait == null && waitEligible(h)) bestWait = h;
        }

        out.println("================================================================================");
        out.println("ROLE_SUMMARY_HEURISTIC_ONLY");
        summary(out, "BestPushMember", bestPush, base);
        summary(out, "BestPopMember", bestPop, base);
        summary(out, "BestGpuWaitMember", bestWait, base);
        out.println("BoundedCoreFinalists=" + ((bestPush != null && bestPop != null && bestWait != null) ? "YES" : "NO"));
        out.println("SemanticVerdict=" + ((bestPush != null && bestPop != null && bestWait != null) ? "BOUNDED_FINALISTS_PENDING_SOURCE_REVIEW" : "AMBIGUOUS"));

        DecompInterface di = new DecompInterface();
        di.toggleCCode(true);
        di.toggleSyntaxTree(true);
        di.openProgram(pg);

        HashSet<Function> emitted = new HashSet<Function>();
        for (Hit h : new Hit[] { bestPush, bestPop, bestWait }) {
            if (h != null && emitted.add(h.f)) dumpCandidate(out, h, base, listing, rm, fm, di);
        }
        int extra = 0;
        for (Hit h : hits) {
            if (emitted.contains(h.f)) continue;
            String r = role(h);
            if (r.equals("RT_SWITCH_WITH_CREATE_TEXTURE_NOT_PUSH") || r.contains("CANDIDATE")) {
                dumpCandidate(out, h, base, listing, rm, fm, di);
                if (++extra >= 20) break;
            }
        }

        di.dispose();
        out.println("================================================================================");
        out.println("SCAN_COMPLETE=YES");
        out.close();
        println("CSGOVR T0 HLAE RT primitive scan v1.1.4 complete");
    }
}
