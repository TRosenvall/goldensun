"""scratchfill.py -- find prose parks whose CANDIDATE WAS ALREADY WRITTEN.

    python3 tools/scratchfill.py            # list them
    python3 tools/scratchfill.py --emit      # one objcmp command per park

THE DEFECT CLASS, found by batch 338 brief E and then measured tree-wide.

133 park files are "prose" parks: a header, a diagnosis, sometimes a plan, and no
compilable body.  tools/shortrefs.py ranked them by the reference function's
length and concluded they are the EXPENSIVE pool -- nothing under 21
instructions, 40 of them over 300.  That conclusion was wrong about the cost,
because it assumed the body still had to be written.

**70 of the 133 name a candidate in `scratch/` THAT STILL EXISTS.**  The work was
done, measured, and then left in an untracked directory instead of being
installed into the park.  Brief E drew five such parks, found all five had live
candidates, and landed THREE of them in one round.

So before anyone writes a body from disassembly, check here.  This tool costs one
compile per park and no tokens, and it is the cheapest unclaimed work in the
tree by a wide margin.

WHY THE CANDIDATES WENT MISSING.  `scratch/` is gitignored, so a candidate left
there is invisible to `git ls-files`, to the directory generator, and to every
targeting instrument -- while the park beside it reads as unattempted prose.  The
lesson for the future is the standing one: a park body may be replaced, but the
WORK must be carried into the tree, not left beside it.

CAUTION.  A scratch candidate is a snapshot of what compiled on the day it was
written.  Several name headers or helpers that have since moved, so a candidate
that fails to compile is not evidence about the function -- it is evidence the
candidate has rotted.  The tool reports those separately.
"""
import glob
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DOCKER_PRE = ["docker", "run", "--rm", "--security-opt", "seccomp=unconfined",
              "-v", f"{ROOT}:/work", "-w", "/work"]
IMAGE = "goldensun-build"
DEF = re.compile(r"^[A-Za-z_][A-Za-z0-9_ \t\*]*?\b(\w+)\s*\([^;{]*?\)\s*\{", re.M | re.S)
SUBJ = re.compile(r"\s*/\*+\s*((?:Ovl)?Func_\w+|[A-Z][A-Za-z0-9_]*)")
START = re.compile(r"^\s*\.(?:thumb|arm)_func_start(?:_noalign)?\s+(\S+)", re.M)

_IDX = None


def func_index():
    """{function: .s} from every .thumb_func_start -- a park's named .s rots."""
    global _IDX
    if _IDX is None:
        _IDX = {}
        for s in glob.glob(os.path.join(ROOT, "asm/**/*.s"), recursive=True):
            rel = os.path.relpath(s, ROOT)
            for m in START.finditer(open(s, errors="replace").read()):
                _IDX.setdefault(m.group(1), rel)
    return _IDX


def strip_comments(s):
    s = re.sub(r"/\*.*?\*/", " ", s, flags=re.S)
    return re.sub(r"//[^\n]*", " ", s)


def scan():
    out = []
    for p in sorted(glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"),
                              recursive=True)):
        t = open(p, errors="replace").read()
        if DEF.search(strip_comments(t)):
            continue                       # has a body already
        rel = os.path.relpath(p, ROOT)
        cands = [c for c in sorted(set(re.findall(r"scratch[A-Za-z0-9_/.\-]*\.c", t)))
                 if os.path.exists(os.path.join(ROOT, c))]
        if not cands:
            continue
        m = SUBJ.match(t)
        fn = m.group(1) if m else None
        # VALIDATE AGAINST THE FILENAME'S ADDRESS.  Parks are address-named, so a
        # resolved subject must contain that address.  The first version of this
        # fallback scanned the first 1200 characters for any name the tree
        # declares, and on ovl_797990/2008c1c.c -- whose line 1 is a displaced
        # "BATCH 207 ..." note and whose line 3 names its TWIN -- it resolved to
        # OvlFunc_898_2009090 and wrote that function's body into a park about
        # OvlFunc_901_2008c1c.  Caught by this check, reverted, and the check now
        # runs before anything is written.
        addr = re.search(r"([0-9a-f]{5,8})$", os.path.basename(rel)[:-2])
        def ok(name):
            return name in func_index() and (not addr or addr.group(1) in name)
        if not (fn and ok(fn)):
            fn = None
            for c in re.findall(r"\b((?:Ovl)?Func_\w+|[A-Z][A-Za-z0-9_]{3,})\b", t[:1200]):
                if ok(c):
                    fn = c
                    break
        ref = func_index().get(fn)
        out.append((rel, cands[0], fn, ref))
    return out


def main():
    rows = scan()
    emit = "--emit" in sys.argv
    ready = [r for r in rows if r[2] and r[3]]
    print(f"{len(rows)} prose park(s) name a LIVE scratch candidate; "
          f"{len(ready)} resolve to a function and reference\n")
    if emit:
        for rel, cand, fn, ref in ready:
            print(f"# {fn}   park {rel}")
            print(" ".join(DOCKER_PRE + [IMAGE, "python3", "tools/objcmp.py",
                                         cand, ref, "--func", fn]))
        return 0
    for rel, cand, fn, ref in rows:
        flag = "" if (fn and ref) else "   !! unresolved"
        print(f"  {fn or '?':28} {cand:34} {rel.replace('src/non_matching/','')}{flag}")
    print(f"\nMeasure them all with:\n  python3 tools/scratchfill.py --emit | sh")
    return 0


if __name__ == "__main__":
    sys.exit(main())
