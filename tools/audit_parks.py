#!/usr/bin/env python3
"""audit_parks.py -- find parked functions whose C is WRONG, not merely unmatched.

WHY

A park note records a blocker. The danger is that the blocker is real AND the C
is also incorrect, because then the diagnosis explains the diff and nobody looks
further. That has happened twice:

  OvlFunc_931_2008360  (batch 20)  a flag guard enclosed one statement too many;
                                   the screen reported a clean match because it
                                   had dropped label definitions.
  OvlFunc_909_200828c  (batch 25)  __ActorMessage and __SetFlag sat inside an
                                   `if` that the ROM runs unconditionally. The
                                   park diagnosed constant-CSE, which was also
                                   true, and the wrong control flow hid behind
                                   it for several rounds.

Both times the tell was the same: A LABEL IN A DIFFERENT POSITION. Instructions
can differ for a dozen harmless codegen reasons, but a branch target landing
somewhere else means the two functions have different control flow -- which is a
statement about behaviour, not about register allocation.

WHAT THIS DOES

Screens every parked .c with `tryc.py --full` and reports the ones where a
differing line is a LABEL rather than an instruction. Those are the files to
re-read before trusting their notes.

    docker run --rm -v "$PWD:/work" -w /work goldensun-build \\
        python3 tools/audit_parks.py

WHAT A HIT DOES AND DOES NOT MEAN

Not every label displacement is a semantic error. If our stream is a different
LENGTH from the ROM's, every label after the first extra instruction shifts by
one and shows up here -- that is the same codegen difference reported twice.
The signal to act on is a label difference in a function whose two streams are
the SAME length, which is why that is reported separately and first.

RESULT OF THE FIRST RUN, 83 parked files, recorded so the two hits are not
re-investigated from scratch:

  DecFlagByte      BENIGN. Our stream loads the byte once and reuses it on the
                   path where the store did not happen; the ROM reloads
                   unconditionally after the label. Semantically identical --
                   gcc proved the value unchanged on that path. Codegen, not
                   control flow.
  Func_80b09fc     BENIGN. The label is a POOL-SKIP target, `b .L / <pool> / .L:`,
                   the same placement question as
                   src/non_matching/rom_15000/rom_1c154.c. Nothing to do with
                   the source's branches.

                   Worth noting separately: that function loads 0 from the pool
                   (`ldr r6, =0x0`) where a `mov r6, #0` would do, which is the
                   pool tell -- the operand was a SYMBOL whose value is zero.
                   That is a live lead for it and is unrelated to this audit.

So no parked file currently carries the batch-25 failure mode. Re-run this after
any round that adds parks.
"""
import glob
import os
import re
import subprocess
import sys

ROOT = "/work" if os.path.isdir("/work/asm") else \
    os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = re.compile(r"Source asm:\s*goldensun/(\S+\.s)")
HEAD = re.compile(r"^\s+XX (\S+)\s+\(rom (\d+) lines, ours (\d+)")
DIFF = re.compile(r"^\s+-> rom (.*?)\s{2,}ours (.*)$")
LABEL = re.compile(r"^L\d+:$")
# a bare register operand -- these differing is ordinary allocation noise
REG = re.compile(r"^(r\d+|sp|lr|pc)$")


def operand_mismatch(a, b):
    """True if two instructions share a mnemonic but differ in a NON-register operand.

    That means the C names a different constant or a different symbol than the
    ROM does, which is a bug in the source rather than a codegen difference.

    Six parked functions have now been found this way, three of them in one
    session, all previously filed as "logic faithful" or "reg-alloc divergence":

      OvlFunc_911_20081ac   compared against _AREA_38, ROM wants 0x26
      HeightTile_7          index written param_2 * 16 + param_3, ROM has them
                            transposed
      OvlFunc_907_20080dc   TWO wrong symbols -- a made-up SpecialExitTag for an
                            area id, and a .L label where the ROM returns a real
                            global

    A wrong constant produces a well-formed single-instruction diff that reads
    exactly like allocation noise, which is why they survived so long. Registers
    are deliberately excluded: r3-versus-r4 is the noise this is trying to see
    past.
    """
    ta, tb = a.split(None, 1), b.split(None, 1)
    if len(ta) < 2 or len(tb) < 2 or ta[0] != tb[0]:
        return None
    oa = [x.strip() for x in ta[1].replace("[", "").replace("]", "").split(",")]
    ob = [x.strip() for x in tb[1].replace("[", "").replace("]", "").split(",")]
    if len(oa) != len(ob):
        return None
    for x, y in zip(oa, ob):
        if x != y and not (REG.match(x) and REG.match(y)):
            return (x, y)
    return None


def audit(path, ref):
    r = subprocess.run(
        [sys.executable, os.path.join(ROOT, "tools", "tryc.py"),
         path, "--ref", ref, "--full"],
        capture_output=True, text=True, cwd=ROOT)
    out = r.stdout + r.stderr
    h = HEAD.search(out)
    if not h:
        return None
    rom_text, our_text = [], []
    for line in out.split("\n"):
        m = re.match(r"^\s+(?:->)?\s*rom (.*?)\s{2,}ours (.*)$", line)
        if m:
            rom_text.append(m.group(1).strip())
            our_text.append(m.group(2).strip())
    rom_text, our_text = " | ".join(rom_text), " | ".join(our_text)
    same_len = h.group(2) == h.group(3)
    labels, operands = [], []
    for line in out.split("\n"):
        m = DIFF.match(line)
        if not m:
            continue
        a, b = m.group(1).strip(), m.group(2).strip()
        if LABEL.match(a) or LABEL.match(b):
            labels.append((a, b))
            continue
        om = operand_mismatch(a, b)
        # A TRANSPOSITION also produces differing operands at the same index --
        # both instructions exist on both sides, just swapped. That is ordinary
        # scheduling, not a wrong constant, and it was 6 of the first 8 hits.
        # Only report an operand that appears NOWHERE on the other side.
        # NOTE the crossing: the ROM's operand is checked against OUR stream and
        # vice versa. Testing each against the stream it came from is vacuously
        # true and makes this report nothing, which looks exactly like a filter
        # that is working.
        if om and (om[0] not in our_text or om[1] not in rom_text):
            operands.append((a, b, om))
    return h.group(1), same_len, labels, operands


def main():
    strong, weak, opnd, n = [], [], [], 0
    for p in sorted(glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"),
                              recursive=True)):
        rel = os.path.relpath(p, ROOT)
        m = SRC.search(open(p, errors="replace").read())
        if not m or not os.path.exists(os.path.join(ROOT, m.group(1))):
            continue
        n += 1
        res = audit(rel, m.group(1))
        if not res:
            continue
        if res[3]:
            opnd.append((rel, res[0], res[3]))
        if res[2]:
            (strong if res[1] else weak).append((rel, res[0], res[2]))

    print(f"screened {n} parked files\n")
    print("=== OPERAND VALUE DIFFERS -- likely a bug in the C, check first ===")
    for rel, fn, ops in opnd:
        print(f"  {fn}  {rel}")
        for a, b, (x, y) in ops[:2]:
            print(f"      rom {a!r}\n      ours {b!r}     ({x} vs {y})")
    if not opnd:
        print("  (none)")
    print()
    print("=== SAME LENGTH, label displaced -- re-read these first ===")
    for rel, fn, labs in strong:
        print(f"  {fn}  {rel}")
        for a, b in labs[:3]:
            print(f"      rom {a!r}  ours {b!r}")
    if not strong:
        print("  (none)")
    print(f"\n=== different length, label shift is probably codegen ({len(weak)}) ===")
    for rel, fn, _ in weak:
        print(f"  {fn}  {rel}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
