#!/usr/bin/env python3
"""realign.py -- re-rank the parked set on ALIGNED distance, not positional.

WHY THIS EXISTS. Batch 180 found Func_8020b64 recorded in its park as "47 of
61" and actually SIX instructions away, with every register already in the
ROM's place. The 47 was a positional count: the two streams differ in length,
so a position-by-position comparison reports everything after the first
insertion as differing. tryc.py --align reports disagreeing REGIONS instead and
gives 6.

That is not a rounding error, it is a different verdict. A park quoting a
positional count on a length mismatch is overstating its distance, sometimes by
an order of magnitude, and the parked set has been ranked on those numbers for
most of this project's life. This tool re-measures.

    python3 tools/realign.py                    # every park, cheapest first
    python3 tools/realign.py --slice 0 40       # a slice, for parallel runs
    python3 tools/realign.py --max-insns 60

Each park is a .c named for its address. The function name and its reference
.s are recovered from asm/ by that address, the park is screened inside the
build container, and the aligned count is printed. Parks whose C no longer
compiles, or whose function has since been elevated, are reported as SKIP with
the reason rather than silently dropped.

This is a SCREEN. `make compare` remains the authority for anything acted on.
"""
import glob
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DOCKER = ["docker", "run", "--rm", "-v", ROOT + ":/work", "-w", "/work",
          "goldensun-build", "sh", "-c"]
START = re.compile(r"^\.thumb_func_start\s+(\S+)(?:\s+@\s+0x0*([0-9a-fA-F]+))?")
# Overlay .s files mostly carry NO address comment on the marker -- the address
# lives in the symbol itself, OvlFunc_<overlay>_<addr>. Indexing only on the
# comment made 33 of 60 overlay parks look like they had no assembly left, i.e.
# like stale parks for already-elevated functions. They were not.
NAMEADDR = re.compile(r"^(?:OvlFunc_\d+|Func|sub)_([0-9a-fA-F]{6,8})$")


def index_asm():
    """address -> (function name, .s path) for everything still in asm/."""
    out = {}
    for s in glob.glob(os.path.join(ROOT, "asm", "**", "*.s"), recursive=True):
        try:
            for line in open(s, errors="replace"):
                m = START.match(line)
                if not m:
                    continue
                fn = m.group(1)
                rel = os.path.relpath(s, ROOT)
                if m.group(2):
                    out[int(m.group(2), 16)] = (fn, rel)
                nm = NAMEADDR.match(fn)
                if nm:
                    out.setdefault(int(nm.group(1), 16), (fn, rel))
        except OSError:
            pass
    return out


def parks():
    out = []
    for p in glob.glob(os.path.join(ROOT, "src", "non_matching", "**", "*.c"),
                       recursive=True):
        b = os.path.basename(p)[:-2]
        try:
            out.append((int(b, 16), os.path.relpath(p, ROOT)))
        except ValueError:
            pass          # preheader_load_merge.c and friends: named, not addressed
    return sorted(out)


def main():
    lo, hi, cap = 0, 10 ** 9, 10 ** 9
    for i, a in enumerate(sys.argv):
        if a == "--slice":
            lo, hi = int(sys.argv[i + 1]), int(sys.argv[i + 1]) + int(sys.argv[i + 2])
        if a == "--max-insns":
            cap = int(sys.argv[i + 1])

    idx = index_asm()
    rows = []
    todo = parks()[lo:hi]
    for n, (addr, park) in enumerate(todo, 1):
        # Park filenames drop the region prefix: 916b0.c is 0x080916b0 and
        # 4cc.c is an overlay offset. Try the bare value, then the main-ROM and
        # overlay bases, and finally the overlay load address.
        hit = None
        for cand in (addr, 0x08000000 | addr, 0x02000000 | addr,
                     0x02008000 + addr):
            hit = idx.get(cand)
            if hit:
                addr = cand
                break
        if not hit:
            print("SKIP %-46s no asm for 0x%x (elevated?)" % (park, addr))
            continue
        fn, spath = hit
        ref = os.path.join("scratch", "realign_%08x.s" % addr)
        subprocess.run(
            ["awk", "-v", "n=" + fn,
             r'BEGIN{s="^\.thumb_func_start "n"([ \t]|$)"; e="^\.func_end "n"[ \t]*$"} $0~s,$0~e',
             os.path.join(ROOT, spath)],
            stdout=open(os.path.join(ROOT, ref), "w"), check=False)
        if os.path.getsize(os.path.join(ROOT, ref)) == 0:
            print("SKIP %-46s could not extract %s" % (park, fn))
            continue
        r = subprocess.run(
            DOCKER + ["python3 tools/tryc.py %s --ref %s --align" % (park, ref)],
            capture_output=True, text=True)
        txt = r.stdout + r.stderr
        m = re.search(r"(\d+) instruction\(s\) in disagreeing regions, of (\d+)", txt)
        if r.returncode == 0 and " OK " in txt:
            print("MATCH?? %-43s %s -- park may already be stale" % (park, fn))
            rows.append((0, fn, park))
        elif m:
            bad, tot = int(m.group(1)), int(m.group(2))
            if tot <= cap:
                print("%5d /%4d   %-30s %s" % (bad, tot, fn, park))
                rows.append((bad, fn, park))
        else:
            first = next((l for l in txt.splitlines() if "error" in l.lower()), "")
            print("SKIP %-46s does not compile  %s" % (park, first[:60]))
        sys.stdout.flush()

    print("\n=== cheapest first ===")
    for bad, fn, park in sorted(rows)[:40]:
        print("%5d  %-30s %s" % (bad, fn, park))


if __name__ == "__main__":
    main()
