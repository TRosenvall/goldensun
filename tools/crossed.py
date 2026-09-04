#!/usr/bin/env python3
"""crossed.py -- flag functions whose ROM crosses mov order against shift order.

    python3 tools/crossed.py OvlFunc_922_2009154 [more names...]

WHY. gcc emits the `mov`s that feed an argument fill IN THE ORDER THEIR
CONSUMING SHIFTS APPEAR. That is measured, on a minimal reproducer, and it is
not about the constants being equal -- `q1 = 0x81` against `q2 = 0x80`
transposes exactly as two identical values do. See "the same-value movs class
is really MOV ORDER SLAVED TO SHIFT ORDER" in docs/elevation.md.

The consequence is a shape no source reaches. Where the ROM has

    mov r1 / mov r2 / lsl r2 / ... / lsl r1

the mov order says r1 first and the shift order says r2 first, and those two
cannot be set independently: writing the third argument inline flips the mov
pair and takes the tail with it. Two mutually exclusive reachable states, the
ROM a third.

CORRECTED: THE SHAPE IS REACHABLE AND THIS TOOL NO LONGER REJECTS ANYTHING.
Both functions this was built to catch are now elevated, and so is a third it
reported AVOID. One line closes the site -- a volatile asm on the FIRST mov the
ROM issues, placed straight after its assignment:

    q1 = 0xdc; __asm__ volatile ("" : : "r" (q1)); q2 = 0x9d; q2 <<= 3; ...

It consumes the register, so the mov must be materialised before it, and it
produces nothing, so gcc has no value to copy forward in place of the immediate.
The mov order was never decided by the source, which is why every operand-level
spelling tied; it is decided in the post-reload scheduler, and a scheduling
barrier is the thing that reaches it. See "CORRECTED AGAIN: a VOLATILE ASM ON
THE FIRST MOV reaches the crossed case" in docs/elevation.md.

So a BARRIER verdict is a ROUTE, NOT A REJECTION: it says this function has a
site that will sit two instructions from done until the barrier goes in, and
where to put it. Run it before choosing a target for that reason, not to skip
the candidate. A CLEAN result is not a promise the function will match; it only
means this particular site is absent.

The exit status is 1 when any crossed site was found. That is a "there is one
here" signal for scripting; it is NOT a recommendation to skip, and nothing
should treat it as one.

Validated against the two functions this was built from,
src/overlays/rom_7c460c/ovl_314_c_a_c_a.c and
src/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_a_c_a_c_a.c, both of which it flags
and both of which now match. An earlier version reported both CLEAN because it split
call blocks on `startswith('bl ')` and the disassembly separates the mnemonic
with a TAB, so nothing ever split and the whole function collapsed into one
block. A filter that passes the cases it exists to catch is worse than no
filter; check any change here against those two names.
"""
import re
import sys
import glob
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def call_blocks(body):
    """Split a function body into the run of instructions before each call."""
    cur = []
    for line in body:
        s = line.strip()
        cur.append(s)
        if re.match(r"bl\b", s):
            yield cur
            cur = []


def crossed_sites(body):
    n = 0
    for blk in call_blocks(body):
        movs, lsls = [], []
        for s in blk:
            m = re.match(r"mov\s+r([0-3]),\s*#", s)
            if m:
                movs.append(m.group(1))
            # THE CONSUMING OP IS NOT ALWAYS A SHIFT. What orders the movs is
            # whichever instruction consumes each one first, and for a negative
            # argument that is `neg`, not `lsl`. Scanning for `lsl` alone missed
            # the __Func_80933f8 site in OvlFunc_909_200979c and the
            # __Func_8012330 site in OvlFunc_931_20087b8 -- both real crossings
            # in the negation form, both needing the same barrier, and both
            # reported CLEAN. `asr` and `lsr` are included for the same reason.
            m = re.match(r"(?:lsl|lsr|asr|neg)\s+r([0-3]),", s)
            if m:
                lsls.append(m.group(1))
        shifted = [r for r in movs if r in lsls]
        first = []
        for r in shifted:
            if r not in first:
                first.append(r)
        if len(first) >= 2:
            order = [r for r in lsls if r in first]
            if order[:len(first)] != first:
                n += 1
    return n


def find(name):
    # A .s path is accepted in place of a function name. This exists so the tool
    # stays testable: every function it was validated against has since been
    # elevated, its listing removed from asm/, and a bare name lookup now reports
    # NOT FOUND for all three. Recover one and pass the file:
    #
    #   git show 182f93e4^:asm/overlays/rom_7eaf28/ovl_314_c_a_c_c_c_c_c_c_c_c_c_c_c_a.s > /tmp/a.s
    #   git show e336f4f1^:asm/overlays/rom_7c460c/ovl_314_c_a_c_a.s                      > /tmp/b.s
    #   git show e336f4f1^:asm/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_a_c_a_c_a.s        > /tmp/c.s
    #   python3 tools/crossed.py /tmp/a.s /tmp/b.s /tmp/c.s     # all three BARRIER
    #
    # A filter that passes the cases it exists to catch is worse than no filter,
    # and an earlier version of this file did exactly that. Check any change here
    # against those three before trusting it.
    if name.endswith(".s") and os.path.exists(name):
        return name
    for s in glob.glob(os.path.join(ROOT, "asm/**/*.s"), recursive=True):
        txt = open(s, errors="ignore").read()
        if ".thumb_func_start " + name in txt:
            return s
    return None


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    bad = 0
    for name in sys.argv[1:]:
        path = find(name)
        if not path:
            print("%-28s NOT FOUND (already elevated, or a typo)" % name)
            continue
        lines = open(path, errors="ignore").read().split("\n")
        i = next(k for k, l in enumerate(lines)
                 if ".thumb_func_start " in l) if name.endswith(".s") else \
            next(k for k, l in enumerate(lines)
                 if ".thumb_func_start " + name in l)
        j = next((k for k in range(i + 1, len(lines))
                  if ".func_end" in lines[k]), len(lines))
        n = crossed_sites(lines[i + 1:j])
        bad += 1 if n else 0
        print("%-28s crossed-sites=%d  %s" % (name, n, "CLEAN" if not n else "BARRIER"))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
