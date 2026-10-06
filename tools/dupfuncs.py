#!/usr/bin/env python3
"""dupfuncs.py -- find remaining functions that are DUPLICATES of each other.

Overlays in this ROM share a lot of code by copying it. Normalising away
function names, .L label numbers and pool operands, 101 of the 2110 remaining
THUMB functions fall into 27 identical groups -- so 74 of them would come free
the moment their representative is solved.

The distribution is very top-heavy. Three groups account for 50 functions:

    x18  OvlFunc_883_20080c4
    x17  OvlFunc_883_200834c
    x15  OvlFunc_883_20088c0

Two of those are already parked, one at SEVEN differing lines of 176.

    docker run --rm -v "$PWD:/work" -w /work goldensun-build \\
        python3 tools/dupfuncs.py

WHAT "DUPLICATE" MEANS HERE. Bodies are compared after replacing every
`Func_`/`OvlFunc_` symbol with a placeholder, every `.L` label with a
placeholder, and every `=operand` pool reference with a placeholder. So two
members of a group differ only in which data tables and callees they name --
`OvlFunc_957_20088c0` and `OvlFunc_964_20088c0` differ in exactly two `ldr =`
operands. One C file per group, with the labels as parameters, matches all of
them.

Groups smaller than 8 instructions are skipped; short leaf functions collide
trivially and are not worth the churn.
"""
import collections
import hashlib
import os
import re
import subprocess
import sys

FSTART = re.compile(r"^\.(thumb|arm)_func_start\s+(\S+)", re.M)


def normalise(body):
    n = re.sub(r"(Ovl)?Func_\w+", "F", body)
    n = re.sub(r"\.L\w+", "L", n)
    n = re.sub(r"=\S+", "=X", n)
    return "\n".join(l for l in n.split("\n") if l.startswith("\t"))


# --- folds for comparing HAND-WRITTEN .s against GCC-GENERATED .s -------------
#
# The two halves of the tree spell the same instruction differently, so the
# default normalise() can never match across them.  Three differences, all
# mechanical, all verified against asm/overlays/rom_7f6e64/..._a_a_a.s (hand)
# and asm/overlays/rom_7b0400/ovl_314_c_c_c_c_a.s (generated):
#
#   register alias   hand `mov r6, r10`      generated `mov r6, sl`
#   operand count    hand `add r6, #0x64`    generated `add r6, r6, #100`
#   immediate radix  hand `#0x64`            generated `#100`
#
ALIAS = {"sl": "r10", "sb": "r9", "fp": "r11", "ip": "r12", "lr": "r14",
         "sp": "r13", "pc": "r15"}


def _fold_imm(m):
    return "#" + str(int(m.group(1), 0))


def normalise_x(body):
    """normalise(), plus the three cross-half folds, INSTRUCTIONS ONLY.

    Directive lines are dropped here and not in normalise().  Two reasons, both
    cross-half:

    1.  BOUNDARY.  A generated function has no `.func_end`; it ends at
        `.size NAME,.LfeN-NAME`, and its literal pool sits BEFORE that.  Slicing
        to the next function start therefore drags in the pool and the next
        function's `.global`/`.align`/`.type` -- the same defect the default
        mode documents for the hand-written half.  Dropping directives makes the
        slice end irrelevant.

    2.  POOL REPRESENTATION.  The halves do not agree on what a pooled operand
        even looks like: hand-written `.s` writes `ldr r0, =gThing` (which
        normalise() placeholders to `=X`), while gcc writes `ldr r0, .L3` plus a
        separate `.word` entry.  No fold reconciles those, so the pool cannot
        participate in a cross-half comparison at all.
    """
    n = normalise(body)
    out = []
    for l in n.split("\n"):
        if l.startswith("\t.") or l.strip().startswith("."):
            continue
        # register aliases -> rN
        l = re.sub(r"\b(sl|sb|fp|ip|lr|sp|pc)\b", lambda m: ALIAS[m.group(1)], l)
        # immediates -> decimal
        l = re.sub(r"#(0x[0-9a-fA-F]+|\d+)", _fold_imm, l)
        # three-operand destructive -> two-operand:  op rD, rD, X  ->  op rD, X
        l = re.sub(r"^(\t\w+\t)(r\d+), \2, ", r"\1\2, ", l)
        # `movs`/`adds`/... -> `mov`/`add`/...  (flag-setting suffix is not a
        # different instruction for shape purposes on Thumb-1, where almost
        # every data-processing op sets flags regardless of spelling)
        l = re.sub(r"^\t(add|sub|mov|and|orr|eor|lsl|lsr|asr|neg|mul|bic)s\t",
                   r"\t\1\t", l)
        out.append(l)
    return "\n".join(out)


def asm_files(landed):
    """Tracked .s files under asm/.  `landed` picks which half.

    A piece is LANDED when a sibling .c exists beside it in src/ -- converting a
    function deletes its hand-written .s and the next build writes a generated
    one back to the same path, so both halves have .s files and only the sibling
    .c tells them apart.
    """
    out = []
    for f in subprocess.run(["git", "ls-files", "asm"],
                            capture_output=True, text=True).stdout.split():
        if not f.endswith(".s") or "/m4a" in f or not os.path.exists(f):
            continue
        if os.path.exists("src/" + f[len("asm/"):-2] + ".c") == landed:
            out.append(f)
    return out


# A GENERATED .s HAS NO `.thumb_func_start`.  gcc emits `.thumb_func` followed
# by `.type NAME,function` and a `NAME:` label, so FSTART matches nothing in the
# landed half -- which is why the first run of --vs-landed reported "0 landed".
GSTART = re.compile(r"^\t\.type\s+(\S+?),\s*function\s*$", re.M)


def bodies(files, cross=False, gen=False):
    """{normalised-hash: [(func, .s path)]} for every THUMB function in `files`.

    `gen`   -- parse gcc-GENERATED .s (`.type NAME,function`) instead of
               hand-written .s (`.thumb_func_start NAME`).
    `cross` -- normalise with the cross-half folds so a hand-written body and a
               generated one can hash equal.

    THESE ARE TWO SEPARATE SWITCHES ON PURPOSE.  A cross-half comparison needs
    `cross` on BOTH halves but `gen` on only the landed one; conflating them
    makes the hand-written half parse with the generated parser and silently
    yield nothing.
    """
    groups = collections.defaultdict(list)
    total = 0
    for f in files:
        t = open(f, errors="ignore").read()
        if gen:
            starts = [(m.end(), "thumb", m.group(1)) for m in GSTART.finditer(t)]
        else:
            starts = [(m.start(), m.group(1), m.group(2)) for m in FSTART.finditer(t)]
        for i, (off, kind, name) in enumerate(starts):
            if kind != "thumb":
                continue
            end = starts[i + 1][0] if i + 1 < len(starts) else len(t)
            fe = t.find("\n.func_end", off)
            if fe != -1 and fe < end:
                end = fe
            body = (normalise_x if cross else normalise)(t[off:end])
            total += 1
            if body.count("\n") < 8:
                continue
            groups[hashlib.md5(body.encode()).hexdigest()].append((name, f))
    return groups, total


def vs_landed():
    """Report REMAINING functions whose body already exists as a LANDED one.

    WHY THIS MODE EXISTS.  The default mode's universe is remaining-vs-remaining
    (see asm_files): it excludes every piece that has a sibling .c.  So a park
    whose duplicate has ALREADY LANDED is invisible to it -- and that is the
    cheapest landing in the tree, because the landed .c is a working body and
    the port is a rename.

    Found by batch 329 brief F the hard way: it reached OvlFunc_925_200b460 by
    reading a park's module siblings, then ported that landed twelve-line body
    onto TWO parks on the first try.  dupfuncs could not have told it to look.

    THE RESULT, AND WHY AN EMPTY ONE IS EVIDENCE.  As of batch 329 this reports
    NOTHING: no remaining function hashes to a landed body.  That is a real
    negative, not a broken tool, because the normaliser passes two controls:

      control 1  the 7 known hand-vs-hand duplicate groups all survive the
                 cross folds, 7/7 -- so the folds are not lossy
      control 2  the same folds find 197 duplicate groups covering 756 of the
                 4,883 LANDED bodies -- so they work on generated .s as well

    Re-run both before concluding the tool has rotted.

    WHAT IT STILL CANNOT SEE, which is the thing worth knowing.  Equality here
    is EXACT after normalisation, and normalise() placeholders only Func_/
    OvlFunc_ names, .L labels and `=` pool operands.  A pair differing in a
    DATA symbol, a non-Func callee or a single immediate does not hash equal.
    Batch 329 brief F's case is exactly that: it ported the landed body of
    OvlFunc_925_200b460 onto two parks, and the asm is 42 instructions against
    41 -- a near-twin with an extra call, not a duplicate.  So "no free
    rename-only ports exist" is established; "no free ports exist" is NOT.
    Finding near-twins needs a shape instrument, not a hash.
    """
    rem, nrem = bodies(asm_files(landed=False), cross=True, gen=False)
    lan, nlan = bodies(asm_files(landed=True), cross=True, gen=True)
    print(f"{nrem} remaining THUMB functions, {nlan} landed")
    shared = sorted(set(rem) & set(lan))
    if not shared:
        print("\nno remaining function hashes to a landed body.")
        print("This is a REAL negative -- the normaliser passes two controls")
        print("(see the docstring).  It means there are no free RENAME-ONLY")
        print("ports from landed bodies.  It does NOT rule out near-twins that")
        print("differ in a data symbol, a callee or one immediate: those need a")
        print("shape instrument, not a hash.  See brief F's OvlFunc_925_200b460.")
        return
    n = sum(len(rem[h]) for h in shared)
    print(f"\n{len(shared)} body/bodies shared; {n} remaining function(s) "
          f"would come free\n")
    for h in shared:
        for name, f in rem[h]:
            print(f"  {name}  ({f})")
        for name, f in lan[h]:
            src = "src/" + f[len("asm/"):-2] + ".c"
            print(f"      LANDED BODY: {name}  ->  {src}")
        print()


def main():
    if "--vs-landed" in sys.argv:
        return vs_landed()
    files = asm_files(landed=False)
    groups = collections.defaultdict(list)
    total = 0
    for f in files:
        t = open(f, errors="ignore").read()
        starts = [(m.start(), m.group(1), m.group(2)) for m in FSTART.finditer(t)]
        for i, (off, kind, name) in enumerate(starts):
            if kind != "thumb":
                continue
            end = starts[i + 1][0] if i + 1 < len(starts) else len(t)
            # END AT `.func_end`, NOT AT THE NEXT FUNCTION START.
            #
            # Slicing to the next function start drags in everything BETWEEN the
            # two functions -- the literal pool and, decisively, the trailing
            # `.global` directives for DATA symbols.  Those lines begin with a
            # tab, so they survive the filter in normalise(), and names like
            # `gOvl_0200a8f4`, `gScript_921__0200a4f4` and `gTable_921__0200a3f0`
            # are NOT placeholdered (only Func_/OvlFunc_/.L are).  Two copies of
            # one routine in different overlays therefore differ in ~39 lines of
            # symbol names and NEVER GROUP.
            #
            # That is how the tree's two CLOSEST parks -- 1 of 199 and 2 of 199,
            # the same 192-instruction routine in two overlays, identical after
            # name/label/pool normalisation -- were missed, with neither park
            # referencing the other.  A pin present in one and absent in the
            # other was the whole of their 2-vs-1 difference.
            fe = t.find("\n.func_end", off)
            if fe != -1 and fe < end:
                end = fe
            body = normalise(t[off:end])
            total += 1
            if body.count("\n") < 8:
                continue
            groups[hashlib.md5(body.encode()).hexdigest()].append((name, f))

    dups = {k: v for k, v in groups.items() if len(v) > 1}
    covered = sum(len(v) for v in dups.values())
    print(f"{total} remaining THUMB functions")
    print(f"{len(dups)} duplicate groups covering {covered} functions; "
          f"{covered - len(dups)} would come free")
    for v in sorted(dups.values(), key=len, reverse=True):
        if len(v) < 2:
            continue
        print(f"\n  x{len(v)}  {v[0][0]}")
        for name, f in v[:4]:
            print(f"        {name:<24} {f}")
        if len(v) > 4:
            print(f"        ... and {len(v) - 4} more")
    return 0


if __name__ == "__main__":
    sys.exit(main())
