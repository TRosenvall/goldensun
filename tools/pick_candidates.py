#!/usr/bin/env python3
"""pick_candidates.py -- rank functions by how likely they are to MATCH.

WHY THIS IS SEPARATE FROM elevation_candidates.py

That tool ranks by tractability -- size, call count, branch count -- and filters
out functions whose blocker is already recorded against them by name. It answers
"what is small and unattempted".

This one answers a different question: "what is unlikely to hit a blocker I
already know about". Two consecutive rounds produced no elevations because
candidates were picked on size alone and landed on shapes that were already
characterised as blocked. Every filter below was paid for by a round.

THE FILTERS, AND WHAT EACH ONE COST

  --whole-file      One function in the .s and no data, so no split, no linker
                    script edit, no chance of stranding a label. Not a
                    correctness filter, just the cheapest kind of win.

  loop-free         Rejects any backward branch. The un-rotated loop shape needs
                    the goto lever to get close and then usually lands on the
                    pre-header load merge -- see
                    src/non_matching/preheader_load_merge.c, three members, all
                    short by exactly one instruction, no known fix.

  no _call_via_rN   Indirect calls are matchable (four are elevated) but each
                    needs its own read of the pointer's return type, so they are
                    their own project rather than a quick win.

  no repeated       THE ONE THAT MATTERS MOST. A pooled constant loaded twice in
  pooled constant   one function is the constant-CSE blocker: gcc hoists it into
                    a callee-saved register and pays a push/pop to save one pool
                    load. OvlFunc_899_200852c is 38 instructions against 36 --
                    LONGER than the ROM -- for exactly this.

                    Note this is a heuristic, not a proof. The blocked case is a
                    constant repeated on ONE path; repeated on MUTUALLY
                    EXCLUSIVE arms it is fine, because gcc never has both live.
                    OvlFunc_922_20085b8 matched with two ids repeated that way.

                    CONFIRMED AGAIN IN BATCH 52, and this time the mechanical
                    search for the constant-CSE shape is what flagged it.
                    OvlFunc_967_20084b0 loads 0x9a7 for two __GetFlag calls --
                    the exact shape -- but they sit on opposite arms of an area
                    test. It matches at plain -O2 and needed no Makefile rule.
                    So the search for this class must be read as a WORKLIST, not
                    a verdict: check whether the two uses can be live at once
                    before adding a per-file flag.
                    So this filter throws away some good candidates, which is
                    the right trade while there are hundreds left.

                    AND THE REJECTS ARE A WORKLIST. Run with --allow-repeat and
                    screen the tagged rows with `tryc.py --no-rerun-cse`: where
                    the repetitions are separated by a CALL, that flag matches
                    them. Five tagged candidates in batch 26, three matched
                    immediately. See CSE_CFLAGS in the Makefile.

  not m4a            asm/rom_f9000/ is the stock m4a / "Sappy" sound engine.
                     The Makefile already knows it is different: src/lib/m4a/
                     is built with old_agbcc and -D M4A_SIGNED_CHAR, NOT
                     Camelot's gcc296. tryc.py screens with gcc296 flags, so a
                     screen there answers the wrong question even when the
                     function really was C.

  not hand-assembly  A prologue of `mov r12, lr` with a matching `bx r12` is
                     not a shape gcc-2.96 emits for Thumb -- it saves lr with
                     `push {lr}` and returns through a popped register. Fifteen
                     functions in the tree use the r12 convention and all of
                     them are in the m4a engine, whose hand-written half is
                     assembly by ORIGIN and was never compiled from anything.

                     These two cost most of a round: eight of the top sixteen
                     candidates at `--min-calls 1 --max-insn 20` were `ply_*`
                     sound commands, which look ideal -- eight instructions,
                     one call, a tiny shared helper -- and cannot be elevated
                     at all. Small in absolute terms, dominant at the top of
                     the list, which is the only place that matters.

  no arg-interleave  A `mov r0` landing INSIDE another argument's construction
                     -- between `mov rN, #imm` and its `lsl rN` -- is the
                     arg-interleave blocker, and neither declaration lever
                     reaches it. It cost two functions and eight screens before
                     it was recognised, because the symptom is indistinguishable
                     from the fill-order class the levers DO retire.

                     Unlike the others this one is read off the ROM's own
                     instruction stream, so it is exact rather than heuristic:
                     if the reference interleaves, the C cannot.

WHAT IT DOES NOT KNOW

Constants re-materialised with `mov rN, #imm / neg rN, rN` rather than pooled.
OvlFunc_945_200c13c builds -1 three times that way and is constant-CSE for the
same reason as the pooled cases, but the `no repeated pooled constant` filter
does not see it. Check for repeated small-constant construction by eye until
that is added.

    python3 tools/pick_candidates.py --whole-file
    python3 tools/pick_candidates.py --min-calls 5 --max-insn 30
"""
import collections
import glob
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
from asmfacts import functions, carries_data           # noqa: E402

FUNC = re.compile(r"\.thumb_func_start\s+(\S+)[^\n]*\n(.*?)\.func_end", re.S)
LABEL = re.compile(r"(\.L\w+):")
BRANCH = re.compile(r"\tb\w*\t(\.L\w+)")
POOL = re.compile(r"ldr\s+r\d+,\s*=(\S+)")


def has_loop(body):
    """True if any branch targets a label defined earlier -- i.e. a back edge."""
    lines = body.split("\n")
    pos = {m.group(1): i for i, l in enumerate(lines) if (m := LABEL.match(l))}
    for i, l in enumerate(lines):
        m = BRANCH.search(l)
        if m and pos.get(m.group(1), len(lines)) < i:
            return True
    return False


MOVIMM = re.compile(r"^\tmov\t(r\d+), #")
LSL = re.compile(r"^\tlsl\t(r\d+),")
MOVR0 = re.compile(r"^\tmov\tr0,")


def has_arg_interleave(body):
    """True if a `mov r0` sits between a `mov rN, #imm` and that rN's `lsl`.

    The ROM scheduled r0 into the middle of another argument's construction.
    gcc emits r0 before or after the whole block and neither declaration lever
    moves it into the gap, so the function cannot match. Read straight off the
    reference, so it is exact for the shape it covers.

    A WIDER RULE WAS TRIED AND IS WRONG. OvlFunc_924_2008ffc has the same defect
    with no shift in sight --

        ldr r2, =0x3333 / mov r0, #0 / ldr r1, =0x6666

    -- so the obvious generalisation is "r0 written with some other argument
    register written both before and after it". That rejects THREE functions
    that actually matched (OvlFunc_940_2008224, OvlFunc_936_20083d8,
    OvlFunc_922_20085b8), because r0-r3 are also ordinary scratch registers and
    nothing in the text distinguishes argument setup from a range check that
    happens to use r2 and r3. Position alone is not the discriminator.

    So this stays narrow and misses cases like OvlFunc_924_2008ffc, which is the
    right trade: a filter that rejects good candidates is worse than one that
    lets a few bad ones through, because the bad ones cost one screen and the
    good ones are never seen again.
    """
    pending = {}
    for line in body.split("\n"):
        # r0 FIRST. `mov r0, #0xf` matches MOVIMM too, and testing that first
        # silently swallowed the r0 write -- which made this miss
        # OvlFunc_899_2008428, one of the two functions it exists to catch.
        # A filter that rejects nothing looks exactly like one that works.
        if MOVR0.match(line):
            for r in pending:
                pending[r] = True
            continue
        m = MOVIMM.match(line)
        if m:
            pending[m.group(1)] = False
            continue
        m = LSL.match(line)
        if m and pending.get(m.group(1)):
            return True
        if m:
            pending.pop(m.group(1), None)
        if line.startswith("\tbl\t"):
            pending.clear()
    return False


ARGW = re.compile(r"^\t(?:mov|ldr|lsl|lsr|neg|add|sub)\t(r[0-3])\b")


def r0_mid_blocks(body):
    """Argument blocks where r0 is written with other arg registers either side.

    NOT a filter -- reported as a tag. Batch 26 established that this shape is
    SOMETIMES reachable by declaring the callee, so rejecting it would throw
    away good candidates. But it is the "no" answer in three of the four cases
    measured, so it is worth seeing before spending a screen:

      OvlFunc_959_20092e0   mov r2 / mov r0 / mov r1                REACHABLE
      OvlFunc_899_2008428   mov r1,#imm / mov r0 / lsl r1 / mov r2  no
      OvlFunc_924_2008ffc   ldr r2,= / mov r0 / ldr r1,=            no
      OvlFunc_950_2008898   mov r2 / mov r0 / ldr r1,=              no

    The one that works has all plain `mov`s around r0; the tag reports whether
    that is the case here, so the odds are visible rather than guessed.
    """
    out = []
    for block in re.split(r"^\tbl\t.*$", body, flags=re.M):
        seq = [m.group(1) for l in block.split("\n") if (m := ARGW.match(l))]
        kinds = [l.split("\t")[1] for l in block.split("\n")
                 if ARGW.match(l)]
        if "r0" not in seq:
            continue
        i = seq.index("r0")
        if i == 0 or i == len(seq) - 1:
            continue
        out.append("all-mov" if all(k == "mov" for k in kinds) else "mixed")
    return out


def parked_names():
    """Functions already parked, so they are not offered again as candidates.

    Without this the top of the list fills up with functions that have already
    been attempted and characterised -- four of the first five rows on the first
    run after a break, each costing a read to recognise. The parked note is the
    place to go for those, not the candidate list.
    """
    out = set()
    for p in glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"),
                       recursive=True):
        txt = open(p, errors="replace").read()
        out.update(re.findall(r"\b((?:Ovl)?Func_\w+|[A-Z]\w+)\s*\(", txt))
        m = re.match(r"/\*\s*(\S+)", txt)
        if m:
            out.add(m.group(1))
        # AND EVERY FUNCTION NAMED IN THE HEADER COMMENT. A park that covers a
        # CLASS lists its members as bare names -- src/non_matching/overlays/
        # pool_load_first.c names four -- and only one of them is also a
        # definition. Matching on "name followed by (" therefore skipped three
        # of the four, and two came back to the top of this list the round
        # after they were parked, costing a read each. That is exactly what
        # this filter exists to prevent.
        head = txt.split("*/", 1)[0]
        out.update(re.findall(r"\b((?:Ovl)?Func_\w+)\b", head))
    return out


def scan(whole_file, min_calls, min_insn, max_insn, allow_repeat,
         skip_inline_pool=False):
    skip = parked_names()
    rows = []
    for p in sorted(glob.glob(os.path.join(ROOT, "asm/**/*.s"), recursive=True)):
        rel = os.path.relpath(p, ROOT)
        # m4a: a different compiler entirely -- see the docstring.
        if rel.startswith("asm/rom_f9000/"):
            continue
        fns = functions(rel)
        if not fns:
            continue
        if whole_file and (len(fns) != 1 or carries_data(rel)):
            continue
        txt = open(p, errors="replace").read()
        for m in FUNC.finditer(txt):
            name, body = m.group(1), m.group(2)
            insn = len([l for l in body.split("\n") if l.startswith("\t")])
            if not (min_insn <= insn <= max_insn):
                continue
            if name in skip:
                continue
            calls = body.count("\tbl\t")
            if calls < min_calls or "_call_via" in body or has_loop(body):
                continue
            if has_arg_interleave(body):
                continue
            # HAND-WRITTEN ASSEMBLY, not compiler output. gcc-2.96 never saves
            # lr in r12 for Thumb; it pushes lr and returns through a popped
            # register. Nothing to elevate -- these were never C.
            if re.search(r"^\tmov\tr12, lr", body, re.M) or \
                    re.search(r"^\tbx\tr12", body, re.M):
                continue
            # AN INLINE LITERAL POOL IS NOT A DISQUALIFICATION. This used to
            # `continue` here, on the batch-30 reasoning that gcc puts the pool
            # after the epilogue while the ROM keeps it mid-body behind a
            # `.pool_aligned`, so every PC-relative offset differs even when the
            # instruction stream matches. That cost two functions, and the rule
            # it produced then cost four more.
            #
            # gcc reproduces a mid-body pool perfectly well, `b` around it and
            # all. It does so when the pool reference is NARROW: arm.md gives a
            # HImode pool load a pool_range of 32-60 bytes against SImode's
            # 1020, so it cannot wait for the barrier at the end of the function
            # and dump_table manufactures a jump over an early pool. The whole
            # OvlFunc_914_2008b24 family -- four byte-identical functions -- was
            # excluded from this list by the old rule and is now elevated.
            #
            # The real hazard is unchanged and belongs to tryc.py, which warns
            # about it on both the OK and the near-miss paths: a screen cannot
            # see PC-relative offsets, so an inline-pool reference must be taken
            # to `make compare` before it is believed. `--no-inline-pool`
            # restores the old exclusion for anyone who wants a safer worklist.
            if skip_inline_pool and (".pool_aligned" in body
                                     or re.search(r"^\s*\.word\s", body, re.M)):
                continue
            pooled = collections.Counter(POOL.findall(body))
            dupes = [k for k, v in pooled.items() if v > 1]
            if dupes and not allow_repeat:
                continue
            rows.append((-calls, insn, name, rel, len(fns), dupes,
                         r0_mid_blocks(body)))
    return sorted(rows)


def main():
    a = sys.argv[1:]

    def opt(flag, default):
        return int(a[a.index(flag) + 1]) if flag in a else default

    rows = scan("--whole-file" in a, opt("--min-calls", 3),
                opt("--min-insn", 10), opt("--max-insn", 40),
                "--allow-repeat" in a, "--no-inline-pool" in a)
    print(f"{'call':>4} {'insn':>5} {'fns':>4}  name / file")
    for c, insn, name, rel, nf, dupes, mid in rows[:opt("--limit", 20)]:
        tag = f"   [repeats {','.join(dupes)}]" if dupes else ""
        if mid:
            tag += f"   [r0-mid: {','.join(mid)}]"
        print(f"{-c:4d} {insn:5d} {nf:4d}  {name}  {rel}{tag}")
    print(f"\n{len(rows)} candidates")
    return 0


if __name__ == "__main__":
    sys.exit(main())
