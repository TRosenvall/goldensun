#!/usr/bin/env python3
"""census.py -- how many functions are left, by size and by status.

    python3 tools/census.py            # the table
    python3 tools/census.py --list 1 20   # name the AVAILABLE ones in a band

WHY THIS IS A TOOL AND NOT A ONE-LINER. This count was got wrong three times in
one day, each time by a fresh throwaway script, and each time the number was
plausible enough to publish. The three failures are all encoded below as tests
the code must not regress:

1. COUNT FUNCTIONS BY `.thumb_func_start`, NOT BY `.func_end`. Hand-written .s
   files do not reliably close their functions -- asm/rom_9000/rom_92b8.s has 9
   starts and 6 ends. A parser that emits a function only when it sees an end
   silently drops the rest, which undercounted hand-written assembly as 59 when
   it is 76.

2. A PARK IS NOT ONE FILE PER FUNCTION. Three shapes break the obvious mapping:
   CLASS parks (arg_interleave_flat.c, tiny_reg_order.c) cover many functions at
   once; some parks are named for a different address than their subject
   (Func_80f0008's neighbourhood is parked in 20095d4.c); and some are named for
   a source-file stem (rom_79c30.c). Matching park FILENAMES to addresses found
   464 parked functions when the true figure is 659.

3. MATCH THE NAME AS A SUBSTRING. An overlay park cites its callees with the
   thunk prefix, `__Func_808b868`, and `\bFunc_` does not match that -- `_` is a
   word character, so there is no boundary before `F`. A word-boundary regex
   also misses every park whose subject has a real name (CanRemoveItem,
   UpdateScreenShake) rather than an address.

   Substring matching can in principle over-count, by treating a park's citation
   of some OTHER unsolved function as if that function were parked. Measured
   against a hand check of the 1-20 band, it does not: it predicted 1 available
   and the hand check found exactly 1. Prefer it, and re-run the hand check
   below if you change anything here.

5. MATCH THE DIRECTIVE CASE-INSENSITIVELY. Four functions in the ROM are
   declared `.thumb_Func_start` or `.thumb_func_Start` with the wrong case --
   GAS accepts them and the bytes are identical. README.md has said since the
   rom_8a000 miscount that "any tool that walks these files needs a
   case-insensitive match", and this file did not have one, so it reported TOTAL
   1260 where funcindex.py (which uses re.I, and is right) reported 1262. The
   two still in asm/ are Func_a1f74 and Func_97f80.

   Found in batch 277 the same way the earlier four were: a subagent read a file
   this tool had told me held four functions and found five. THE CROSS-CHECK
   THAT CATCHES IT is census TOTAL against funcindex's "still in asm" count --
   they must agree, and they had silently disagreed by two.

4. AN ARM FUNCTION IS NOT ATTEMPTABLE. This build has no ARM compile path at
   all -- every one of the 935 gcc-2.96 rules passes -mthumb, and NONE of the
   3,517 solved files is ARM. So a `.arm_func_start` function cannot be
   elevated without first establishing a build path that does not exist, and
   there is no precedent for one.

   That is a structural fact, not a heuristic, which is why ARM gets its own
   column rather than being folded into hand-asm. The 12 ARM functions here
   were being reported as AVAILABLE, and they are the reason the 1-20 band
   looked like it still had work in it. Checked by hand, all six of the ARM
   functions under 40 instructions are plainly hand-written: three-way width
   selection by predication (ldrccb/ldreqh/ldrgt), `rrx`, constant tables read
   via adr+ldm rather than a literal pool, and a routine that rewrites Thumb BL
   pairs in memory to relocate code copied to RAM.

6. A PARK'S SUBJECT IS WHAT IT DEFINES, NOT EVERY NAME IT MENTIONS. This file
   used `name in parks` against the concatenated text of every park, which
   counts a function as parked when some OTHER park merely cites it -- in an
   `extern` prototype for a callee, or in prose about a file-mate ("Anim_Haunt
   is function 4 of 5; Anim_PlanetDiver is 3", ".rodata belongs to
   BaseAnim_Breath, not to this function"). That reported 677 parked and 417
   available when the figures are 522 and 572: OVER 150 FUNCTIONS THAT NOBODY
   HAS EVER ATTEMPTED WERE BEING COUNTED AS ALREADY TRIED.

   Filename matching (the version before it) found 464 -- an undercount, which
   is why substring matching was adopted. BOTH WERE WRONG AND THE FIX IS THE
   UNION, not a choice between them: park_subjects() plus match_stem() combine
   four signals, and the filename signal is load-bearing (it is the only thing
   that sees rom_c9000/80ccaec.c parking Func_80cd52c's neighbour, whose header
   opens with a batch note rather than a subject line).

   HAND-CHECKED two ways before the change. Ten disputed names sampled at
   random were all citations -- four `extern` prototypes, six prose mentions of
   a neighbour -- and none was a park. Then the ORPHAN-PARK INVARIANT: every
   park file should have at least one function attributed to it, and only 11 of
   531 do not. Re-run both if you touch this.

   THE ORPHAN CHECK ALSO FOUND FOUR STALE PARKS whose subject has since been
   elevated and which should have been retired at landing: Func_80f6038,
   Func_80f4100, Func_80a22f4, OvlFunc_881_2009888. Those are a separate defect
   from the counting, and they are why the invariant is worth keeping.

   Found in batch 284 because tools/pickable.py and this file DISAGREED about
   seven functions, pickable calling them available. Two tools disagreeing is
   the cheapest bug detector this project has; prefer it to trusting either.

VERIFY BEFORE QUOTING. `--list` prints the available functions in a band; for a
small band, grep each name in src/non_matching/ by hand. If any listed function
turns up there, this file has a bug -- fix it here rather than in a new script.
"""
import os, re, sys, glob

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from filtered import hand_written, generated

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
START = re.compile(r"^\s*\.(thumb|arm)_[Ff]unc_[Ss]tart(?:_noalign)?\s+(\S+)")
END = re.compile(r"^\s*\.func_end\b")
DIRECTIVE = re.compile(r"^\s*\.")
LABEL = re.compile(r"^\s*\S+:")
BUCKETS = [(1, 20), (21, 40), (41, 60), (61, 100),
           (101, 200), (201, 400), (401, 800), (801, 10 ** 9)]


def n_insn(lines):
    n = 0
    for l in lines:
        s = l.strip()
        if not s or s.startswith(("@", "/*", "*")):
            continue
        if DIRECTIVE.match(l) or LABEL.match(l):
            continue
        n += 1
    return n


DEFN = re.compile(r"^[A-Za-z_][\w\s\*]*?\b([A-Za-z_]\w*)\s*\([^;]*?\)\s*\{", re.M)


def park_subjects():
    """The set of function names that are actually PARKED.

    NOT a substring search over park text -- see lesson 6 in the module
    docstring. A park's subject is what it DEFINES or what its header names,
    never every identifier it happens to mention.

    Three signals, union:
      * a function DEFINED in the park's code (comments stripped first)
      * a function named in the park header's first two lines, which is where
        this corpus puts the subject ("/* NAME -- NON-MATCHING, N of M")
      * for a CLASS park -- one sitting at the top level of src/non_matching/
        rather than in a bank directory -- every function it names, because
        covering many functions at once is what a class park is for
    """
    subjects, stems = set(), []
    for path in glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"),
                          recursive=True):
        text = open(path, errors="ignore").read()
        code = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
        code = re.sub(r"//[^\n]*", "", code)
        subjects |= {m.group(1) for m in DEFN.finditer(code)}
        head = "\n".join(text.split("\n")[:2])
        subjects |= {w for w in re.findall(r"[A-Za-z_]\w*", head)}
        if os.path.dirname(path) == os.path.join(ROOT, "src/non_matching"):
            subjects |= set(re.findall(r"[A-Za-z_]\w*", text))
        stems.append(os.path.basename(path)[:-2])
    return subjects, stems


def match_stem(name, stems):
    """Does a park FILENAME name this function?

    The signal census used before substring matching, and insufficient ALONE
    (it found 464) but necessary in the union: src/non_matching/rom_c9000/
    80cd52c.c parks Func_80cd52c and its header opens with a batch note rather
    than the subject line, so nothing else here sees it. Stems are either a
    bare address (80cd52c, 2008c1c) or address_Name (d82b0_Drain).
    """
    low = name.lower()
    for st in stems:
        for part in st.lower().split("_"):
            if len(part) >= 4 and low.endswith(part):
                return True
    return False


def survey():
    """[(insns, name, hand_written, parked)] for every function still in asm/."""
    parked, stems = park_subjects()
    rows = []
    for root, _, files in os.walk(os.path.join(ROOT, "asm")):
        for fn in sorted(files):
            if not fn.endswith(".s"):
                continue
            path = os.path.join(root, fn)
            lines = open(path, errors="ignore").readlines()
            # a .s sitting beside a solved .c is compiler OUTPUT, not a
            # target. NOT a substring search -- hand-written prose mentions
            # the string, and that silently hid 126 functions here too.
            if generated(path, lines):
                continue
            hw = hand_written(path)
            starts = [(i, m.group(1), m.group(2)) for i, l in enumerate(lines)
                      if (m := START.match(l))]
            for k, (i, kind, name) in enumerate(starts):
                stop = starts[k + 1][0] if k + 1 < len(starts) else len(lines)
                for j in range(i + 1, stop):
                    if END.match(lines[j]):
                        stop = j
                        break
                rows.append((n_insn(lines[i + 1:stop]), name, hw,
                             name in parked or match_stem(name, stems),
                             kind == "arm"))
    return rows


def main():
    rows = survey()
    if "--list" in sys.argv:
        k = sys.argv.index("--list")
        lo, hi = int(sys.argv[k + 1]), int(sys.argv[k + 2])
        sel = sorted(r for r in rows
                     if lo <= r[0] <= hi and not r[2] and not r[3] and not r[4])
        for c, name, _, _, _ in sel:
            print(f"{c:4d}  {name}")
        print(f"\n{len(sel)} available in {lo}-{hi}")
        return
    lab = lambda a, b: f"{a}-{b}" if b < 10 ** 9 else "800+"
    hdr = (f"{'size':>9} | {'total':>6} | {'hand-asm':>8} | {'ARM':>4} | "
           f"{'parked':>6} | {'AVAILABLE':>9}")
    rule = "-" * 9 + "-+-" + "-" * 6 + "-+-" + "-" * 8 + "-+-" + "-" * 4 + "-+-" \
           + "-" * 6 + "-+-" + "-" * 9
    print(hdr); print(rule)
    t = [0, 0, 0, 0, 0]
    for a, b in BUCKETS:
        s = [r for r in rows if a <= r[0] <= b]
        hw = sum(1 for r in s if r[2])
        arm = sum(1 for r in s if r[4] and not r[2])
        pk = sum(1 for r in s if r[3] and not r[2] and not r[4])
        av = len(s) - hw - arm - pk
        t[0] += len(s); t[1] += hw; t[2] += arm; t[3] += pk; t[4] += av
        print(f"{lab(a, b):>9} | {len(s):6d} | {hw:8d} | {arm:4d} | {pk:6d} | {av:9d}")
    print(rule)
    print(f"{'TOTAL':>9} | {t[0]:6d} | {t[1]:8d} | {t[2]:4d} | {t[3]:6d} | {t[4]:9d}")
    solved = (len(glob.glob(os.path.join(ROOT, "src/**/*.c"), recursive=True))
              - len(glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"), recursive=True)))
    print(f"\nmatched .c files in src/ (excl. parks): {solved}")


if __name__ == "__main__":
    main()
