#!/usr/bin/env python3
"""find_shape.py -- find functions with the SAME INSTRUCTION SHAPE as a solved one.

    python3 tools/find_shape.py <asm/path/file.s> <SolvedFunctionName>
    python3 tools/find_shape.py --git <FunctionName>        (recover the .s from git)

WHY THIS EXISTS

`tools/find_twins.py` finds functions that are BYTE-IDENTICAL up to symbol
names. That is a narrow net. Batch 86 solved three door-opening scripts and
batch 87 found the remaining three by hand-writing a regex over the solved
function's instruction stream with its constants as capture groups -- they
differ in NINE constants each, so find_twins.py cannot see them, and all three
screened clean on the first attempt once the template was filled in.

This automates that. Given a function you have already matched, it builds a
pattern from its instruction stream in which

    every immediate      #0x2c, #7        -> a capture group
    every pool value     =0x122c, =gFoo   -> a capture group
    every branch target  bl __Foo, b .L1  -> a capture group
    every local label    .L1782:          -> a capture group

and everything else -- mnemonics, register names, offsets in `[rN, #imm]`
addressing, the order and count of instructions -- has to match exactly. Then it
reports every function in `asm/` that fits, with the captured values.

Fill those into the C you already wrote and screen each one. Batch 87's three
took about ten minutes for all three.

WHAT IT IS NOT

It is not a similarity score. A candidate either has the identical instruction
sequence or it does not, so a one-instruction difference in the middle rejects
it. That is deliberate: the point is to find functions the SAME C TEMPLATE will
produce, and a template that emits a different instruction is a different
template. Use `--allow N` to permit up to N differing lines when you want to see
near misses, but read those by hand -- they will not fill in mechanically.

Addressing offsets are NOT wildcarded. `[r5, #0x50]` and `[r5, #0x4c]` are
different struct members and therefore different C; only bare `#imm` operands
and pool values become holes.
"""
import os
import re
import subprocess
import sys

ROOT = "/work" if os.path.isdir("/work/asm") else \
    os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))

# re.M IS LOAD-BEARING. Without it `findall` only ever matches at offset 0,
# so every file reported exactly one function (or none) and the tool said
# "0 with this shape" for families that had members. Caught by checking a
# known sibling pair line by line and finding that every line DID match.
FUNC_START = re.compile(r"^\.thumb_func_start (\S+)", re.M)
LOCAL_LABEL = re.compile(r"^(\.L[0-9a-fA-F_]+):$")


def body(text, name):
    """The instruction lines of one function, comments and blanks dropped."""
    out, on = [], False
    for line in text.split("\n"):
        if line.startswith(".thumb_func_start " + name + " ") or \
           line.rstrip() == ".thumb_func_start " + name:
            on = True
            continue
        if on and line.startswith(".func_end "):
            return out
        if on:
            s = line.strip()
            if s and not s.startswith("@"):
                out.append(s)
    return out


def all_functions(path):
    """[(name, [insn, ...]), ...] for one .s file."""
    text = open(path, errors="replace").read()
    names = FUNC_START.findall(text)
    return [(n, body(text, n)) for n in names]


BRANCH = {"b", "bl", "beq", "bne", "bgt", "blt", "bge", "ble", "bhi", "bls",
          "bcc", "bcs", "bmi", "bpl", "bvc", "bvs"}


def to_pattern(line):
    """One instruction line -> (regex, n_holes).

    THE SEPARATOR IS A TAB, not a space, and the first version of this got that
    wrong: its branch-target rule matched `bl\\ (\\S+)` and so never fired, which
    made every function with a branch in it fail to match and the tool report
    zero for four families that had members. Tokenise on whitespace rather than
    pattern-matching escaped text.
    """
    m = LOCAL_LABEL.match(line)
    if m:
        return r"(\.L[0-9a-fA-F_]+):", 1

    parts = line.split(None, 1)
    mnem = parts[0]
    rest = parts[1] if len(parts) > 1 else ""

    # a branch's target is a hole; a push/pop register list is not
    if mnem in BRANCH and rest and not rest.startswith("{"):
        return re.escape(mnem) + r"\s+(\S+)", 1

    if not rest:
        return re.escape(line), 0

    # addressing offsets stay exact -- [r5, #0x50] and [r5, #0x4c] are different
    # struct members and therefore different C
    kept = []

    def keep(mm):
        kept.append(mm.group(0))
        return "\x00%d\x00" % (len(kept) - 1)

    rest = re.sub(r"\[[^\]]*\]", keep, rest)

    holes = 0
    out, pos = [], 0
    for mm in re.finditer(r"(?<![\w.])(#|=)(\S+)", rest):
        out.append(re.escape(rest[pos:mm.start()]))
        out.append(re.escape(mm.group(1)) + r"(\S+)")
        holes += 1
        pos = mm.end()
    out.append(re.escape(rest[pos:]))
    pat = re.escape(mnem) + re.escape(line[len(mnem):len(line) - len(parts[1])]) + "".join(out)

    for i, k in enumerate(kept):
        pat = pat.replace(re.escape("\x00%d\x00" % i), re.escape(k))
        pat = pat.replace("\x00%d\x00" % i, re.escape(k))
    return pat, holes


SHAPE_IMM = re.compile(r"(?<![\w.])(#|=)\S+")
SHAPE_LBL = re.compile(r"\.L[0-9a-fA-F_]+")
SHAPE_SYM = re.compile(r"(?<=\t)(?:__|Ovl|Func_|_)[\w.]+$")


def shape(ins):
    """A canonical signature: the instruction stream with every constant,
    label and call target replaced by a placeholder.

    This is what `--clusters` groups on. It is deliberately the SAME abstraction
    the pattern builder uses, so a cluster of size N means N functions one
    template would produce -- not N functions that merely look alike.
    """
    out = []
    for line in ins:
        m = LOCAL_LABEL.match(line)
        if m:
            out.append("L:")
            continue
        parts = line.split(None, 1)
        mnem = parts[0]
        rest = parts[1] if len(parts) > 1 else ""
        if mnem in BRANCH and rest and not rest.startswith("{"):
            out.append(mnem + " T")
            continue
        kept = []
        rest = re.sub(r"\[[^\]]*\]", lambda mm: kept.append(mm.group(0)) or "\x00", rest)
        rest = SHAPE_IMM.sub(lambda mm: mm.group(1) + "K", rest)
        rest = SHAPE_LBL.sub("L", rest)
        for k in kept:
            rest = rest.replace("\x00", k, 1)
        out.append(mnem + " " + rest)
    return "\n".join(out)


def clusters(min_size, min_insn):
    import collections, glob
    groups = collections.defaultdict(list)
    for f in sorted(glob.glob(os.path.join(ROOT, "asm/**/*.s"), recursive=True)):
        if "Generated by gcc" in open(f, errors="replace").read(200):
            continue
        for n, ins in all_functions(f):
            if len(ins) < min_insn:
                continue
            groups[shape(ins)].append((n, os.path.relpath(f, ROOT), len(ins)))
    gs = [g for g in groups.values() if len(g) >= min_size]
    gs.sort(key=lambda g: -(len(g) - 1) * g[0][2])
    print(f"{'n':>3} {'insn':>5} {'payoff':>7}  representative")
    for g in gs:
        n, rel, ln = g[0]
        print(f"{len(g):3d} {ln:5d} {(len(g)-1)*ln:7d}  {n}  {rel}")
    print(f"\n{len(gs)} shape clusters, "
          f"{sum(len(g) - 1 for g in gs)} functions reachable by solving one each")
    return 0


def main():
    a = sys.argv[1:]
    if not a or a[0] in ("-h", "--help"):
        print(__doc__)
        return 2
    if a[0] == "--clusters":
        mn = int(a[a.index("--min") + 1]) if "--min" in a else 2
        mi = int(a[a.index("--min-insn") + 1]) if "--min-insn" in a else 20
        return clusters(mn, mi)
    allow = int(a[a.index("--allow") + 1]) if "--allow" in a else 0
    a = [x for x in a if x != "--allow" and not x.isdigit()] if "--allow" in a else a

    if a[0] == "--git":
        name = a[1]
        hit = subprocess.run(["git", "grep", "-l", "-F", ".thumb_func_start " + name,
                              "HEAD~40", "--", "asm/"], capture_output=True, text=True,
                             cwd=ROOT).stdout.split("\n")[0]
        if not hit:
            print(f"could not find {name} in git history"); return 1
        rev, path = hit.split(":", 1)
        text = subprocess.run(["git", "show", f"{rev}:{path}"], capture_output=True,
                              text=True, cwd=ROOT).stdout
        src, ref = body(text, name), path
    else:
        ref, name = a[0], a[1]
        src = body(open(os.path.join(ROOT, ref), errors="replace").read(), name)

    if not src:
        print(f"{name} not found in {ref}"); return 1
    pats = [to_pattern(l) for l in src]
    print(f"{name}: {len(src)} instructions, "
          f"{sum(h for _, h in pats)} constants wildcarded  ({ref})\n")

    import glob
    found = 0
    for f in sorted(glob.glob(os.path.join(ROOT, "asm/**/*.s"), recursive=True)):
        if "Generated by gcc" in open(f, errors="replace").read(200):
            continue
        for n, ins in all_functions(f):
            if n == name or len(ins) != len(src):
                continue
            caps, bad = [], 0
            for (p, _), line in zip(pats, ins):
                m = re.fullmatch(p, line)
                if m:
                    caps.extend(m.groups())
                else:
                    bad += 1
                    if bad > allow:
                        break
            if bad > allow:
                continue
            found += 1
            rel = os.path.relpath(f, ROOT)
            print(f"  {n:26s} {rel}" + (f"   [{bad} line(s) differ]" if bad else ""))
            print(f"      {' '.join(caps)}")
    print(f"\n{found} function(s) with this shape")
    return 0


if __name__ == "__main__":
    sys.exit(main())
