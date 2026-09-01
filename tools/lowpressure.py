#!/usr/bin/env python3
"""lowpressure.py -- rank remaining functions by how little register pressure
they carry, because pressure is what the surviving walls are made of.

By batch 176 every ranking in this tree was returning candidates that screen
close and then stop on the same three things:

  * a value copied into r12 or r14 and read back (rom_9000/80120dc.c,
    rom_15000/801965c.c, ovl_7fa4ec/20092ac.c)
  * a value copied into a callee-saved register the source cannot name
    (rom_b5000/80b90ac.c and two others)
  * a sum allocated straight into a long-lived register where the ROM went
    through a scratch and copied (rom_77000/8077cb8.c)

All three are the register allocator disagreeing with the original build, and
all three get MORE likely as pressure rises. The other rankings score shape
(fuzzy_solved), adjacency (family_siblings) or lever-applicability (pickable) --
none of them scores the thing that is actually stopping the work.

This one does, from three signals visible in the .s without any analysis:

  PUSH WIDTH.  `push {lr}` or `push {r5, lr}` means the function never needed
  more than a couple of long-lived values. Every park listed above pushes three
  or four callee-saved registers.

  HIGH REGISTERS.  r12 (ip) or r14 (lr) holding a value is a real wall -- no C
  expresses either. r8-r11 are NOT, and treating them as one cost this tree a
  long time: gcc-2.96 allocates r8-r11 freely once r4-r7 are committed, and
  relaxing the reject produced three elevations in one round out of 109
  candidates it had been hiding. r8-r11 is reported in a column.

  LIVE-ACROSS-CALL COUNT, approximated by the number of calls: each `bl` forces
  everything live to a callee-saved register, so calls are what convert a wide
  value set into pressure. Zero or one call is the sweet spot.

Also rejects a repeated expensive constant, per batch 175 -- that is cse.c's
local sharing and no flag or spelling reaches it.

    python3 tools/lowpressure.py [--limit N] [--max-insns N] [--min-insns N]

Sorted by push width then size, so the least-constrained functions come first.
This is a bet, not a proof: low pressure does not guarantee a match, it just
avoids the class of blocker that has been ending rounds.
"""
import glob
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import filtered
import pickable
import shapesib

# r12 (ip) and r14 (lr) holding a VALUE are a real wall -- the source has no
# way to ask for either. r8-r11 are NOT: gcc-2.96 allocates them freely once
# r4-r7 are committed, and three functions were elevated out of this class in
# one round after the reject was relaxed (Func_80a1814, Func_800d924,
# Func_800d98c). It is reported as a column, not rejected.
HIGH = re.compile(r"\br(?:12|14)\b")
HI811 = re.compile(r"\br(?:8|9|10|11)\b")
PUSH = re.compile(r"^\s*push\s+\{([^}]*)\}")


def push_regs(ins):
    """Callee-saved registers in the FIRST push, lr excluded."""
    for l in ins:
        m = PUSH.search(l)
        if m:
            regs = [r.strip() for r in m.group(1).split(",")]
            return [r for r in regs if r not in ("lr", "r14")]
    return []


WRITE = re.compile(r"^\s*(?:mov|add|sub|ldr|ldrb|ldrh|ldrsb|ldrsh|lsl|lsr|asr|and|orr|eor|neg|mul|bic|mvn)\s+(r[0-9]+)")
READ_ONLY = re.compile(r"\br([4-7])\b")


def reads_unset_callee_saved(ins):
    """True if the body reads r4-r7 before ever writing it and never pushes it.

    The m4a engine bodies under asm/rom_f9000 do this -- no prologue at all, and
    r4/r5 arrive as implicit inputs from the caller. They are hand-written
    assembly with a private calling convention and NO C signature can express
    them, which is why the census keeps `audio` as its own class. Every scan in
    this tree kept offering them until this check existed.
    """
    if any(re.search(r"^\s*push\b", l) for l in ins):
        return False
    written = set()
    for l in ins:
        m = WRITE.match(l)
        ops = l.split(None, 1)[1] if " " in l.strip() else ""
        for r in READ_ONLY.findall(ops):
            if "r" + r not in written:
                return True
        if m:
            written.add(m.group(1))
    return False


def main():
    limit, lo, hi = 25, 20, 120
    for i, a in enumerate(sys.argv):
        if a == "--limit":
            limit = int(sys.argv[i + 1])
        if a == "--min-insns":
            lo = int(sys.argv[i + 1])
        if a == "--max-insns":
            hi = int(sys.argv[i + 1])

    parked = pickable.parked()
    rows = []
    for s in glob.glob("asm/**/*.s", recursive=True):
        if os.path.exists(s[:-2].replace("asm/", "src/", 1) + ".c"):
            continue
        for name, body in shapesib.functions(s):
            if name in parked:
                continue
            ins = [l for l in body if l.strip()
                   and not l.strip().startswith((".", "@", "/*"))]
            n = len(ins)
            if not (lo <= n <= hi):
                continue
            # the push/pop lines legitimately name high registers; every OTHER
            # mention is the wall already present in the reference
            body_only = [l for l in ins
                         if not re.search(r"^\s*(push|pop)\b", l)]
            if any(HIGH.search(l) for l in body_only):
                continue
            if reads_unset_callee_saved(ins):
                continue
            vals = filtered.expensive_constants(ins)
            if len(vals) != len(set(vals)):
                continue
            calls = sum(1 for l in ins if re.search(r"\bbl\s+", l))
            regs = push_regs(ins)
            nhi = sum(1 for l in body_only if HI811.search(l))
            rows.append((len(regs), calls, n, name, s, ",".join(regs) or "-",
                         nhi))

    rows.sort()
    print("%d candidates, sorted by register pressure\n" % len(rows))
    for nreg, calls, n, name, s, regs, nhi in rows[:limit]:
        print("  %d saved (%-11s) %2d bl %3di  %-28s %s%s"
              % (nreg, regs, calls, n, name, s,
                 "  [%d hi]" % nhi if nhi else ""))


if __name__ == "__main__":
    main()
