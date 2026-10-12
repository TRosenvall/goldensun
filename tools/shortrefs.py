"""shortrefs.py -- rank the PROSE parks by how big the function actually is.

    python3 tools/shortrefs.py                 # ranked, shortest first
    python3 tools/shortrefs.py --max 40        # only functions under 40 insns
    python3 tools/shortrefs.py --emit          # one objcmp recipe line per park

THE PROBLEM THIS SOLVES.  675 park files hold 170 with no figure, and 144 of
those are PROSE parks: a header, a diagnosis, sometimes a plan, and **no
compilable body**.  Nothing can measure them -- `objcmp` needs a body, so
`parkcheck` reports NO CLAIM, `backfill.py` skips them, and the figure bands
cannot rank them.  They are the largest unmeasured pool left in the tree and
they are completely invisible to every targeting instrument.

But the thing that decides whether one is cheap to attempt is not in the park at
all -- it is **how long the reference function is**.  Writing a candidate for a
12-instruction function is a few minutes; for a 700-instruction one it is a
batch.  That length is sitting in the hand-written `.s`, free to read, needing no
compile and no container.

So this tool does not measure parks.  It measures **what attempting one would
cost**, and ranks accordingly.  A prose park in front of a 15-instruction
function is the cheapest unclaimed work in the tree; the same park in front of a
600-instruction function is not worth a brief's round.

WHAT COUNTS AS AN INSTRUCTION.  Lines inside the function's span that are not
labels, directives, comments or blank -- i.e. what the assembler will emit.
`.word` entries ARE counted separately and reported, because a function whose
span is mostly pool words is shorter than its line count suggests.
"""
import argparse
import glob
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CLAIM = re.compile(
    r"(\d+)\s+(?:differing\s+)?(?:[A-Za-z]+\s+){0,3}encodings?\s+of\s+(\d+)", re.I)
CLAIM2 = re.compile(r"(\d+)\s+of\s+(\d+)\s+(?:[A-Za-z]+\s+){0,3}encodings", re.I)
DEF = re.compile(r"^[A-Za-z_][A-Za-z0-9_ \t\*]*?\b(\w+)\s*\([^;{]*?\)\s*\{", re.M | re.S)
SUBJ = re.compile(r"\s*/\*+\s*((?:Ovl)?Func_\w+|[A-Z][A-Za-z0-9_]*)")
START = re.compile(r"^\s*\.(?:thumb|arm)_func_start(?:_noalign)?\s+(\S+)", re.M)


def strip_comments(s):
    s = re.sub(r"/\*.*?\*/", " ", s, flags=re.S)
    return re.sub(r"//[^\n]*", " ", s)


_INDEX = None


def func_index():
    """{function name: .s path} built from EVERY .thumb_func_start in asm/.

    A park's named `.s` cannot be trusted: splits consume files, so 65 of the 144
    prose parks named either a path that no longer exists or one that no longer
    holds their function.  Looking the NAME up across the tree is definitive and
    costs one pass, so it is the primary resolver and the park's own text is only
    a fallback.
    """
    global _INDEX
    if _INDEX is None:
        _INDEX = {}
        for s in glob.glob(os.path.join(ROOT, "asm/**/*.s"), recursive=True):
            rel = os.path.relpath(s, ROOT)
            for m in START.finditer(open(s, errors="replace").read()):
                _INDEX.setdefault(m.group(1), rel)
    return _INDEX


def reference_of(text, fn=None):
    if fn:
        hit = func_index().get(fn)
        if hit:
            return hit
    for m in re.finditer(r"(asm/[A-Za-z0-9_/]+\.s)", text):
        if os.path.exists(os.path.join(ROOT, m.group(1))):
            return m.group(1)
    return None


def span_of(body, fn):
    """Lines of fn's span in the .s: from its .thumb_func_start to the next one."""
    lines = body.split("\n")
    first = None
    for i, l in enumerate(lines):
        m = START.match(l)
        if m:
            if m.group(1) == fn:
                first = i
            elif first is not None:
                return lines[first + 1:i]
    return lines[first + 1:] if first is not None else None


def measure_span(lines):
    """-> (instructions, pool words).  An instruction is what the assembler emits."""
    insn = pool = 0
    for l in lines:
        s = l.strip()
        if not s or s.startswith("@"):
            continue
        if s.startswith(".word") or s.startswith(".short") or s.startswith(".byte"):
            pool += 1
            continue
        if s.startswith(".") or s.endswith(":"):
            continue
        insn += 1
    return insn, pool


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--max", type=int, default=0, help="only functions this short")
    ap.add_argument("--emit", action="store_true")
    a = ap.parse_args()

    rows, broken = [], []
    for p in sorted(glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"),
                              recursive=True)):
        t = open(p, errors="replace").read()
        if CLAIM.search(t[:9000]) or CLAIM2.search(t[:9000]):
            continue                              # already has a figure
        if DEF.search(strip_comments(t)):
            continue                              # has a body: backfill.py's job
        rel = os.path.relpath(p, ROOT)
        # VALIDATE THE SUBJECT AGAINST THE INDEX.  SUBJ matches the first
        # capitalised identifier on line 1, which is the house format -- but a
        # park whose note was PREPENDED above its subject line gives up a prose
        # word instead, and this tool duly reported `BATCH not found in ...`.
        # A real subject is a name that some .s actually declares, so check.
        m = SUBJ.match(t)
        fn = m.group(1) if m else None
        if fn and fn not in func_index():
            # fall back to any function the park's own text names that the tree
            # does declare -- recovers a park whose line 1 was displaced
            for cand in re.findall(r"\b((?:Ovl)?Func_\w+|[A-Z][A-Za-z0-9_]{3,})\b",
                                   t[:1200]):
                if cand in func_index():
                    fn = cand
                    break
        ref = reference_of(t, fn)
        if not ref or not fn:
            broken.append((rel, "no reference .s on disk" if not ref
                           else "no subject on line 1"))
            continue
        body = open(os.path.join(ROOT, ref), errors="replace").read()
        lines = span_of(body, fn)
        if lines is None:
            broken.append((rel, f"{fn} not found in {ref}"))
            continue
        insn, pool = measure_span(lines)
        rows.append((insn, pool, fn, rel, ref))

    rows.sort()
    shown = [r for r in rows if not a.max or r[0] <= a.max]
    if a.emit:
        for insn, pool, fn, rel, ref in shown:
            print(f"# {fn}  {insn} instructions")
            print(f'docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" '
                  f"-w /work goldensun-build python3 tools/objcmp.py {rel} {ref} "
                  f"--func {fn}")
        return 0

    print(f"{len(rows)} PROSE parks resolved (no body, so unmeasurable until one "
          f"is written)\n")
    print(f"{'insns':>6} {'pool':>5}  {'function':30} park")
    for insn, pool, fn, rel, ref in shown:
        print(f"{insn:>6} {pool:>5}  {fn:30} {rel.replace('src/non_matching/','')}")
    if a.max:
        print(f"\n{len(shown)} of {len(rows)} are at or under {a.max} instructions.")
    buckets = [(0, 20), (21, 50), (51, 120), (121, 300), (301, 10 ** 9)]
    print("\ncost-to-attempt distribution:")
    for lo, hi in buckets:
        n = sum(1 for r in rows if lo <= r[0] <= hi)
        label = f"{lo}-{hi}" if hi < 10 ** 9 else f"{lo}+"
        print(f"  {label:>9} instructions : {n:>4}")
    if broken:
        print(f"\n{len(broken)} could not be resolved:")
        for rel, why in broken[:8]:
            print(f"  {rel.replace('src/non_matching/','')}: {why}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
