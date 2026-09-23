#!/usr/bin/env python3
"""parkcheck.py -- verify that each park's HEADER CLAIM matches what its BODY measures.

    docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
        goldensun-build python3 tools/parkcheck.py [path ...]

WHY THIS EXISTS. A park is a candidate .c plus a header stating how far it got. The
two are written in the same commit and nothing checked that they agreed, so they
drifted -- twice, in consecutive batches, both times because a header was updated
with the improved numbers while the BODY was left as the older candidate:

  * batch 282 advanced Func_80a96d8 from 10 to 4 encodings.  The header said 4; the
    body still measured 10, because the one-line edit the header described was never
    actually written into the C below it.
  * batch 283 advanced BaseAnim_Tackle from 47 to 12.  Same shape: header 12, body 47.

Both were caught by an agent re-measuring a park it had been told to start from --
i.e. by the next reader paying for the error.  Batch 281 had already recorded the
lesson ("read a park's header against its body before committing -- the body is
evidence about the header") and it recurred anyway, which is the argument for a tool
rather than a habit.

HOW IT FINDS THE REFERENCE.  Parks carry a verify recipe in their header:

    Verify with:
      python3 tools/objcmp.py <park path> <reference .s> [--func NAME]

That line is the contract this tool reads.  A park without one is reported as
UNCHECKABLE rather than silently skipped -- an unverifiable claim is a defect too.

HOW IT READS THE CLAIM.  The first "N ... of M" in the header, where M matches the
reference's own encoding count.  Size-only and prose-only parks report NO CLAIM.
"""
import os, re, subprocess, sys, glob

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
VERIFY = re.compile(r"objcmp\.py\s+(\S+)\s+\\?\s*\n?\s*(\S+\.s)(?:\s+--func\s+(\S+))?", re.M)
CLAIM  = re.compile(r"(\d+)\s+(?:differing\s+)?encodings?\s+of\s+(\d+)", re.I)
CLAIM2 = re.compile(r"NON-MATCHING,\s*(\d+)\s+of\s+(\d+)", re.I)


def header_of(path):
    s = open(path, errors="replace").read()
    m = re.match(r"/\*.*?\*/", s, re.S)
    return m.group(0) if m else ""


def check(path):
    hdr = header_of(path)
    if not hdr:
        return ("UNCHECKABLE", "no header comment", None, None)
    vm = VERIFY.search(hdr.replace("*", " "))
    if not vm:
        return ("UNCHECKABLE", "no `Verify with: objcmp.py ...` recipe in header", None, None)
    ref, func = vm.group(2), vm.group(3)
    if not os.path.exists(os.path.join(ROOT, ref)):
        return ("UNCHECKABLE", f"reference {ref} not found", None, None)
    cm = CLAIM.search(hdr) or CLAIM2.search(hdr)
    cmd = [sys.executable, os.path.join(ROOT, "tools", "objcmp.py"), path, ref]
    if func:
        cmd += ["--func", func]
    out = subprocess.run(cmd, capture_output=True, text=True, cwd=ROOT).stdout
    if " OK " in out:
        got = (0, None)
    else:
        m = re.search(r"ENCODINGS differ in (\d+) place\(s\) \(ref (\d+)", out)
        if not m:
            return ("UNCHECKABLE", "objcmp produced no encoding line", None, None)
        got = (int(m.group(1)), int(m.group(2)))
    if not cm:
        return ("NO CLAIM", f"measures {got[0]}", None, got[0])
    claimed = int(cm.group(1))
    if claimed == got[0]:
        return ("OK", f"{claimed}", claimed, got[0])
    return ("MISMATCH", f"header claims {claimed}, body measures {got[0]}", claimed, got[0])


def main():
    paths = sys.argv[1:] or sorted(glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"), recursive=True))
    paths = [os.path.relpath(p, ROOT) for p in paths]
    tally = {}
    bad = []
    for p in paths:
        status, detail, _, _ = check(p)
        tally[status] = tally.get(status, 0) + 1
        if status in ("MISMATCH", "UNCHECKABLE"):
            bad.append((status, p, detail))
        print(f"  {status:<12} {p}  {detail}")
    print()
    for k in sorted(tally):
        print(f"{k}: {tally[k]}")
    if any(s == "MISMATCH" for s, _, _ in bad):
        print("\nMISMATCHES -- a park's header is lying about its own body:")
        for s, p, d in bad:
            if s == "MISMATCH":
                print(f"  {p}: {d}")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
