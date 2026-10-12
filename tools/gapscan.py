"""gapscan.py -- parks that are a FEW INSTRUCTIONS from matching, whatever their figure says.

    python3 tools/gapscan.py                 # ranked by |gap|, then figure
    python3 tools/gapscan.py --max-gap 1     # only the one-instruction parks
    python3 tools/gapscan.py --dir shorter   # only "ours is SHORTER"

WHY THIS EXISTS, and it is the most useful thing measured in batch 344.

A park's figure is a POSITIONAL count over the encoding stream.  When the two
sides' 16-bit instruction counts differ, the streams are compared OUT OF PHASE
and the figure counts the offset -- so a park that is ONE INSTRUCTION from the
reference can read 109, 143, even 159 differing encodings.  Every targeting
instrument in this tree ranks on that figure, so those parks have been sorted to
the bottom while being structurally the CLOSEST work available.

Measured over the whole tree when this was written -- 558 parks carrying a
verify recipe:

    200  equal instruction length
    358  OUT OF PHASE
           100  within 1 instruction
           156  within 2
           192  within 3
           244  within 5

**192 parks are within three instructions and their MEDIAN FIGURE IS 134**,
ranging from 17 to 1934.  A hundred of them are at a ONE-instruction gap.

That matters because the LENGTH-GAP CRITERION is this tree's most productive
targeting rule by a wide margin -- four batches run on it closed 8 of 15, 7 of
25, 8 of 25 and 10 of 32, and LOCATED the missing-or-extra instruction on
essentially every target.  It had been applied only to parks whose figure was
already small, which is a tiny and now-exhausted slice of the parks it fits.

## THE DIRECTION IS THE WHOLE KEY

Batch 332's brief E called splitting a batch by gap direction "the whole key on
all three", so this tool reports the SIGN:

  * ours SHORTER -- gcc is not emitting something the original did.  Look for a
    missing statement, but do not assume it IS a statement: batch 339 brief C
    found three of four missing instructions were a `pop {r5}` (allocation
    class), a `mov r12, r3` (one extra long-lived value forcing a high register)
    and a `b` over a literal pool.
  * ours LONGER -- gcc failed an optimisation the original made, so the search is
    what lets it REUSE a value, never what statement to add.

And COUNT EACH REGION SEPARATELY: on one park a net -1 was really -3 before the
loop and +2 inside it, two independent mechanisms cancelling in the total.

## NO POSITIONAL FIGURE MEANS ANYTHING UNTIL THE LENGTHS AGREE

Close the gap first, then read the figure.  A park at a 1-instruction gap whose
figure is 143 is not "143 away"; it is one instruction away from being
measurable.
"""
import argparse
import glob
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DOCKER = ["docker", "run", "--rm", "--security-opt", "seccomp=unconfined",
          "-v", f"{ROOT}:/work", "-w", "/work", "goldensun-build"]
# The separator class MUST allow `*`.  A park writes its verify command across
# several lines of a BLOCK COMMENT, so the continuation begins " *     src/...".
# Without `*` this regex matched only the 56 parks that fit the command on one
# line and silently skipped 550 -- a clean negative over 90% of the tree.
VERIFY = re.compile(r"objcmp\.py[\s\\*]+(src/\S+\.c)[\s\\*]+(asm/\S+\.s)"
                    r"(?:[\s\\*]+--func\s+(\S+))?")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--max-gap", type=int, default=3)
    ap.add_argument("--dir", choices=["shorter", "longer"])
    a = ap.parse_args()

    rows = []
    for p in sorted(glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"),
                              recursive=True)):
        txt = open(p, errors="replace").read()
        m = VERIFY.search(txt)
        if not m:
            continue
        rel = os.path.relpath(p, ROOT)
        cmd = DOCKER + ["python3", "tools/objcmp.py", m.group(1), m.group(2)]
        if m.group(3):
            cmd += ["--func", m.group(3)]
        out = subprocess.run(cmd, capture_output=True, text=True).stdout
        im = re.search(r"instructions are (\d+)/(\d+)", out)
        if not im:
            continue
        ref, ours = int(im.group(1)), int(im.group(2))
        if ref == ours:
            continue
        gap = ours - ref
        if abs(gap) > a.max_gap:
            continue
        if a.dir == "shorter" and gap > 0:
            continue
        if a.dir == "longer" and gap < 0:
            continue
        fm = re.search(r"differ in (\d+) place\(s\)", out)
        rows.append((abs(gap), gap, int(fm.group(1)) if fm else None, rel))

    rows.sort()
    print(f"{len(rows)} park(s) within {a.max_gap} instruction(s) of the reference\n")
    print(f"{'gap':>4} {'figure':>7}  park")
    for _, gap, fig, rel in rows:
        print(f"{gap:+4} {str(fig) if fig is not None else '  -':>7}  "
              f"{rel.replace('src/non_matching/', '')}")
    print("\nThe figure is a PHASE artifact on every row above. Close the gap first.")
    print("ours SHORTER (-) = a missing instruction; ours LONGER (+) = a reuse gcc missed.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
