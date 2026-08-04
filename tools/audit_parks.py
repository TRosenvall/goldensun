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


def audit(path, ref):
    r = subprocess.run(
        [sys.executable, os.path.join(ROOT, "tools", "tryc.py"),
         path, "--ref", ref, "--full"],
        capture_output=True, text=True, cwd=ROOT)
    out = r.stdout + r.stderr
    h = HEAD.search(out)
    if not h:
        return None
    same_len = h.group(2) == h.group(3)
    labels = []
    for line in out.split("\n"):
        m = DIFF.match(line)
        if m and (LABEL.match(m.group(1).strip()) or LABEL.match(m.group(2).strip())):
            labels.append((m.group(1).strip(), m.group(2).strip()))
    return h.group(1), same_len, labels


def main():
    strong, weak, n = [], [], 0
    for p in sorted(glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"),
                              recursive=True)):
        rel = os.path.relpath(p, ROOT)
        m = SRC.search(open(p, errors="replace").read())
        if not m or not os.path.exists(os.path.join(ROOT, m.group(1))):
            continue
        n += 1
        res = audit(rel, m.group(1))
        if not res or not res[2]:
            continue
        (strong if res[1] else weak).append((rel, res[0], res[2]))

    print(f"screened {n} parked files\n")
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
