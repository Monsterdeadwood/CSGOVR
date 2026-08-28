#!/usr/bin/env python3
from __future__ import annotations
import tempfile
from pathlib import Path
import research_doc_integrity as rd

def die(s): raise SystemExit("DOC_SELFTEST_FAIL: "+s)
def neederr(n,e):
    if not e: die(n+" incorrectly accepted")
def clean(n,e):
    if e: die(n+" unexpectedly failed: "+" | ".join(e))

def main():
    with tempfile.TemporaryDirectory() as td:
        root=Path(td); p=root/"research"/"X.md"; p.parent.mkdir(parents=True)
        claims={"CL":{"status":"PENDING","evidence_ids":["EV"]}}
        evidence={"EV":{"id":"EV","repo_path":"research/X.md"}}
        contract={"pending_evidence_refs":[]}

        p.write_text("# no identity\n",encoding="utf-8")
        e=[]; rd.validate_doc("research/X.md",p,contract,claims,evidence,False,e); neederr("missing class",e)

        p.write_text("RESEARCH_DOC_CLASS: NOTE\n# note\n",encoding="utf-8")
        e=[]; rd.validate_doc("research/X.md",p,contract,claims,evidence,False,e); clean("plain note",e)

        p.write_text("RESEARCH_DOC_CLASS: EVIDENCE\n# ids missing\n",encoding="utf-8")
        e=[]; rd.validate_doc("research/X.md",p,contract,claims,evidence,False,e); neederr("evidence without ids",e)

        p.write_text("RESEARCH_DOC_CLASS: EVIDENCE\nEVIDENCE_ID: EV\nCLAIM_ID: CL\n",encoding="utf-8")
        e=[]; rd.validate_doc("research/X.md",p,contract,claims,evidence,False,e); neederr("unconverged evidence",e)

        pending={"pending_evidence_refs":["research/X.md"]}
        e=[]; rd.validate_doc("research/X.md",p,pending,claims,evidence,False,e); clean("pending evidence",e)

        claims_active={"CL":{"status":"ACTIVE","evidence_ids":["EV"]}}
        e=[]; rd.validate_doc("research/X.md",p,pending,claims_active,evidence,False,e); neederr("pending doc on active claim",e)

        e=[]; rd.validate_doc("research/X.md",p,contract,claims_active,evidence,True,e); clean("same-change state convergence",e)

        p.write_text("RESEARCH_DOC_CLASS: CONVERGENCE\nEVIDENCE_ID: EV\nCLAIM_ID: CL\n",encoding="utf-8")
        e=[]; rd.validate_doc("research/X.md",p,pending,claims,evidence,False,e); clean("pending convergence",e)

    print("RESEARCH_DOC_INTEGRITY_SELFTEST=PASS"); return 0

if __name__=="__main__": raise SystemExit(main())
