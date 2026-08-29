#!/usr/bin/env python3
"""pool.py -- select remaining functions by shape, for the levers that fit them.

WHY THIS EXISTS

The same candidate query has been rebuilt inline eight or nine times across
these batches, each time slightly differently, and twice with a bug that made it
report zero. Two of those bugs are recorded in docs/elevation.md; both cost a
round. This puts the query in one place with the corrections already applied.

PARK EXCLUSION IS BY FUNCTION NAME, NOT ADDRESS

Park files are named for the low address (`2008950.c`), and overlay functions
from DIFFERENT overlays share that suffix -- every overlay loads at 0x02000000.
`OvlFunc_971_200808c` was being excluded by a park written for
`OvlFunc_881_200808c`. So the exclusion set is built from the function names in
the park headers, not from the filenames.

SHAPES

  flags       >=4 Get/Set/ClearFlag calls.  Requiring them to be a MAJORITY
              of the calls cut 169 candidates to 3, which is too strict to be
              useful -- a dispatcher usually does a little work per arm.
              Flag dispatchers; several have matched on the first screen with no
              lever at all.
  dense       call-dense scripts: calls*3 >= instructions and mem*3 <= them.
              DENSITY, NOT SIZE, is the selector -- two rounds picking the
              smallest candidates produced zero matches across eleven functions.
  interleave  a single-instruction argument emitted INSIDE another argument's
              split build (mov+lsl or mov+neg), with a conditional branch
              before the site. Both halves are required: the lever needs a split
              build to move things around AND a dominating block to
              rematerialise from.

Columns include `br` (conditional branches) and `flag2` (one id feeding both a
Get and a Set/Clear). Read them before writing C:

  * br == 0     -> neither naming lever can work; only the flag group is left.
  * flag2       -> screen with --no-rerun-cse from the start.

    python3 tools/pool.py <shape> [minsize] [maxsize]
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# Makefile pattern rules with a non-default flag group.  tryc.py compiles a
# candidate with the flags for its OWN path, and a file that does not exist yet
# has no path in the Makefile -- so a screen can be exact at -O2 while the build
# uses -O1 and the overlay differs.  That cost a green screen and a red build on
# OvlFunc_885_20080dc.  This column warns before the C is written.
def wildcards():
    out = []
    txt = open(os.path.join(ROOT, "Makefile"), errors="ignore").read().split("\n")
    for i, l in enumerate(txt):
        m = re.match(r"^(asm/\S+?)%\.o: \S+%\.c$", l)
        if not m or i + 1 >= len(txt):
            continue
        g = re.search(r"\$\((\w+_CFLAGS)\)", txt[i + 1])
        if g and g.group(1) != "GCC296_CFLAGS":
            out.append((m.group(1), g.group(1)))
    return out


WILDCARDS = wildcards()


def wildcard_for(asm_path):
    """flag group a new .c under this .s's name would silently inherit"""
    stem = asm_path[:-2] if asm_path.endswith(".s") else asm_path
    hits = {g for pre, g in WILDCARDS if stem.startswith(pre)}
    return "/".join(sorted(g.replace("_CFLAGS", "") for g in hits)) if hits else "-"
START = re.compile(r"^\.thumb_func_start(?:_noalign)? (\S+)")
END = re.compile(r"^\.func_end")
BL = re.compile(r"^\tbl\t(\S+)$")
FLAGCALL = re.compile(r"^\tbl\t_*(Get|Set|Clear)Flag$")
MEM = re.compile(r"^\t(ldr|str|ldrb|strb|ldrh|strh|ldrsh|ldrsb)\t\w+, \[")
COND = re.compile(r"^\tb(eq|ne|ge|gt|le|lt|hi|ls|cs|cc|mi|pl)\b")
SETUP = re.compile(r"^\t(mov|lsl|neg|ldr|add|sub|asr|lsr)\t(r[0-3])\b")
MOVI3 = re.compile(r"^\tmov\t(r[1-3]), #")
MOVR0 = re.compile(r"^\tmov\tr0, #(0x[0-9a-f]+|\d+)$")
SPLIT = re.compile(r"^\t(lsl|neg)\t(r[1-3]),(?: \2,)?(?: #)?")

# For the reuse column: a constant that costs MORE THAN ONE instruction to
# materialise, and is needed more than once, is the shape gcc hoists into a
# callee-saved register while the ROM rebuilds it in place. See the reuse note
# in docs/elevation.md.
MOVIMM = re.compile(r"^\tmov\t(r\d+), #(0x[0-9a-f]+|\d+)$")
LSLIMM = re.compile(r"^\tlsl\t(r\d+),(?: (r\d+),)? #(0x[0-9a-f]+|\d+)$")
NEGREG = re.compile(r"^\tneg\t(r\d+), (r\d+)$")
POOLLD = re.compile(r"^\tldr\t(r\d+), =(\S+)$")
WRITES = re.compile(r"^\t(mov|lsl|lsr|asr|neg|add|sub|ldr|ldrb|ldrh|mul|orr|and)\t(r\d+)")
LDRE0 = re.compile(r"^\tldr\tr0, =(\S+)$")
MOVI0 = re.compile(r"^\tmov\tr0, #(0x[0-9a-f]+|\d+)$")
LSL0 = re.compile(r"^\tlsl\tr0,(?: r0,)? #(0x[0-9a-f]+|\d+)$")
# Park headers name the function, and they do it in FIVE shapes that accumulated
# over the life of the tree:
#     Func_X -- 0xADDR         Func_X  [dir]  --  0xADDR      Func_X @ 0xADDR
#     Func_X -- asm/path.s     Func_X  --  NOT MATCHING
# Matching only the first shape excluded 84 of 253 parked functions and let the
# other 169 come back round as "fresh" candidates -- OvlFunc_881_2009c08 was
# investigated from scratch one day after it was parked.
#
# Two passes, unioned, because they serve different jobs. PARKNAME scans the
# whole head and needs an address or an asm path after the name: that is strict
# enough not to catch prose, and it deliberately also catches a twin named in a
# cross-reference, since both twins are blocked by the same thing. PARKHEAD
# reads only the first few lines and accepts anything after the dash, which is
# what picks up the "NOT MATCHING" style headers.
PARKNAME = re.compile(
    r"\b((?:Ovl)?Func_\w+|[A-Za-z]\w+)\s*(?:\[[^\]]*\])?\s*(?:--|@)\s*"
    r"(?:0x[0-9a-fA-F]+|asm/\S+\.s)")
PARKHEAD = re.compile(
    r"^\s*/?\*?\s*((?:Ovl)?Func_\w+|[A-Za-z][A-Za-z0-9_]+)\s*(?:\[[^\]]*\])?\s*(?:--|@)\s")


def parked_names():
    out = set()
    for root, _, files in os.walk(os.path.join(ROOT, "src/non_matching")):
        for fn in files:
            if not fn.endswith(".c"):
                continue
            head = open(os.path.join(root, fn), errors="ignore").read(2000)
            out |= set(PARKNAME.findall(head))
            for line in head.split("\n")[:3]:
                m = PARKHEAD.match(line)
                if m:
                    out.add(m.group(1))
                    break
    return out


def flagid(body, k):
    """id fed to r0 for the call at index k -- pool load OR mov+lsl build."""
    win = body[max(0, k - 5):k]
    for y in win:
        m = LDRE0.match(y)
        if m:
            return m.group(1)
    base = sh = None
    for y in win:
        m = MOVI0.match(y)
        if m:
            base, sh = int(m.group(1), 0), 0
        m = LSL0.match(y)
        if m and base is not None:
            sh = int(m.group(1), 0)
    return hex(base << (sh or 0)) if base is not None else None


def main():
    shape = sys.argv[1] if len(sys.argv) > 1 else "flags"
    lo = int(sys.argv[2]) if len(sys.argv) > 2 else 20
    hi = int(sys.argv[3]) if len(sys.argv) > 3 else 120
    skip = parked_names()
    rows = []
    for root, _, files in os.walk(os.path.join(ROOT, "asm")):
        for fn in files:
            if not fn.endswith(".s"):
                continue
            p = os.path.join(root, fn)
            if os.path.exists(p.replace("/asm/", "/src/", 1)[:-2] + ".c"):
                continue
            lines = [l.rstrip("\n") for l in open(p, errors="ignore")]
            cur, start, body = None, 0, []
            for i, l in enumerate(lines):
                m = START.match(l)
                if m:
                    cur, start, body = m.group(1), i, []
                    continue
                if cur is None:
                    continue
                if END.match(l):
                    if cur not in skip:
                        rows.append(measure(cur, p, body, lines, start))
                    cur = None
                    continue
                body.append(l)
    rows = [r for r in rows if r and lo <= r["n"] <= hi and keep(shape, r)]
    rows.sort(key=lambda r: r["n"])
    print(f"{len(rows)} candidates, shape={shape}, {lo}-{hi} instructions")
    print("insns calls mem  br flag2 site reuse wildcard  name")
    for r in rows[:20]:
        print(f"{r['n']:5} {r['calls']:5} {r['mem']:3} {r['br']:3} "
              f"{'Y' if r['flag2'] else '-':>5} {r['site']:>4} {r['reuse']:>5} "
              f"{r['wild']:>8}  "
              f"{r['name']:28} {r['path']}")
    return 0


def keep(shape, r):
    if shape == "flags":
        return r["flagcalls"] >= 4
    if shape == "dense":
        return r["calls"] * 3 >= r["n"] and r["mem"] * 3 <= r["n"]
    if shape == "interleave":
        return r["site"] > 0 and r["unguarded"] == 0
    sys.exit("shape must be one of: flags, dense, interleave")


def reuse(body):
    """How many distinct EXPENSIVE constants this function needs more than once.

    Expensive means it cannot be reached by a single `mov rD, #imm`: a shifted
    or negated build, or a pooled load. Those are the ones gcc-2.96 as invoked
    here hoists and keeps alive across calls, adding a push the ROM does not
    have, while the original rebuilds them at each use.

    A nonzero value predicts the constant-reuse blocker BEFORE any C is written,
    which is the whole point -- two functions were transcribed correctly and
    lost to it in a single round before this column existed.
    """
    import collections
    pending, seen = {}, collections.Counter()
    for x in body:
        m = MOVIMM.match(x)
        if m:
            pending[m.group(1)] = int(m.group(2), 0)
            continue
        m = LSLIMM.match(x)
        if m:
            d, src = m.group(1), m.group(2) or m.group(1)
            if src in pending:
                seen[pending[src] << int(m.group(3), 0)] += 1
            pending.pop(d, None)
            continue
        m = NEGREG.match(x)
        if m:
            if m.group(2) in pending:
                seen[-pending[m.group(2)]] += 1
            pending.pop(m.group(1), None)
            continue
        m = POOLLD.match(x)
        if m:
            seen[m.group(2)] += 1
            pending.pop(m.group(1), None)
            continue
        m = WRITES.match(x)
        if m:
            pending.pop(m.group(2), None)
    return sum(1 for v in seen.values() if v > 1)


def measure(name, path, body, lines, start):
    import collections
    n = sum(1 for x in body if x.startswith("\t") and not x.startswith("\t."))
    if not n:
        return None
    calls = sum(1 for x in body if BL.match(x))
    ids = collections.Counter()
    for k, x in enumerate(body):
        if FLAGCALL.match(x):
            v = flagid(body, k)
            if v:
                ids[v] += 1
    guarded = unguarded = 0
    for i in range(start, start + len(body) + 1):
        if i >= len(lines) or not BL.match(lines[i]):
            continue
        j, blk = i - 1, []
        while j >= 0 and SETUP.match(lines[j]):
            blk.append(lines[j])
            j -= 1
        blk.reverse()
        for zi, x in enumerate(blk):
            if not MOVR0.match(x):
                continue
            started = {MOVI3.match(y).group(1) for y in blk[:zi] if MOVI3.match(y)}
            if any(SPLIT.match(y) and SPLIT.match(y).group(2) in started
                   for y in blk[zi + 1:]):
                if any(COND.match(lines[k]) for k in range(start, i)):
                    guarded += 1
                else:
                    unguarded += 1
            break
    return {"name": name, "path": os.path.relpath(path, ROOT), "n": n,
            "calls": calls, "mem": sum(1 for x in body if MEM.match(x)),
            "br": sum(1 for x in body if COND.match(x)),
            "flagcalls": sum(1 for x in body if FLAGCALL.match(x)),
            "flag2": any(v > 1 for v in ids.values()),
            "wild": wildcard_for(os.path.relpath(path, ROOT)),
            "site": guarded, "unguarded": unguarded, "reuse": reuse(body)}


if __name__ == "__main__":
    sys.exit(main())
