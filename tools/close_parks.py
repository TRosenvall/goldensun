#!/usr/bin/env python3
"""close_parks.py -- rank parked functions by how few instructions they are from
matching, so the cheap work in src/non_matching/ can be found rather than guessed at.

WHY

A park note records a blocker, and reading the notes gives no sense of scale: a
file that is one instruction away and a file that is half wrong look identical
in a directory listing. The first run of this, in batch 93 over the 146 parks
that still name a live .s, found 52 within SIX instructions and five within
three. That is a third of the park corpus a handful of instructions from done,
which is not the impression the notes give.

The diffs also cluster into a few recognisable shapes -- argument-move
rotations, and/orr operand destinations, pool-load ordering, register naming --
so working the list in order tends to turn up one lever that moves several files
at once. Batch 93's prototype lever came out of exactly that.

USAGE

    docker run --rm -v "$PWD:/work" -w /work goldensun-build \\
        python3 tools/close_parks.py

Takes about ten minutes; it compiles every park once. Files whose recorded
`Source asm:` path no longer exists -- the .s was split or elevated since the
note was written -- are skipped rather than reported, so a shrinking denominator
is expected and is not a failure.
"""
import glob
import os
import re
import subprocess
import sys

SRC = re.compile(r"Source asm:\s*goldensun/(\S+\.s)")
HEAD = re.compile(r"XX (\S+)\s+\(rom (\d+) lines, ours (\d+)[^,]*, "
                  r"first diff at (\d+), (\d+) differ")
THRESHOLD = 6


def main():
    out, seen = [], 0
    for p in sorted(glob.glob("src/non_matching/**/*.c", recursive=True)):
        m = SRC.search(open(p, errors="replace").read())
        if not m or not os.path.exists(m.group(1)):
            continue
        seen += 1
        r = subprocess.run([sys.executable, "tools/tryc.py", p, "--ref", m.group(1)],
                           capture_output=True, text=True)
        for line in (r.stdout + r.stderr).split("\n"):
            h = HEAD.search(line)
            if h and int(h.group(5)) <= THRESHOLD:
                out.append((int(h.group(5)), h.group(1),
                            int(h.group(2)), int(h.group(3)), p))
    for n, fn, a, b, p in sorted(out):
        print(f"{n:3} differ  {fn:28} rom {a:3} ours {b:3}  {p}")
    print(f"\n{len(out)} of {seen} screened parks are within "
          f"{THRESHOLD} instructions")
    return 0


if __name__ == "__main__":
    sys.exit(main())
