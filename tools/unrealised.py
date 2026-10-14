"""unrealised.py -- parks whose HEADER mentions a BETTER figure than the one installed.

    python3 tools/unrealised.py
    python3 tools/unrealised.py --min-gain 3

WHY THIS EXISTS, and it cost five batches to learn.

Batch 340 measured a body at 6 on `FieldMove_NoTarget`, wrote **"NOT installed"**
beside it, and told the next reader to start from there.  Five batches went by.
Batch 350 brief D found it by reading the header, reproduced it, and improved the
park 7 -> 6 -- the lever was never a crossing in the negative list, it was a
POSITIVE that had been recorded and left.

That is a distinct failure from the recorded-negative problem this project has
been mining.  A stale NEGATIVE wastes a search.  **A stale POSITIVE wastes the
answer.**  And unlike a negative, it is findable mechanically: the park's own
prose names a smaller number than the park's own claim line.

## WHAT IT DOES

For each park: take the CLAIM (the current figure, which the brief template
requires first in the header) and scan the rest of the header for any other
"N of M" against the SAME M.  Report every N smaller than the claim.

## WHAT IT CANNOT DO, so read the rows rather than trusting them

A smaller figure in a header is not automatically an improvement waiting to be
installed.  All of these are common and legitimate:

  * it is a PINNED or DEVICE-CARRYING figure, deliberately not shipped under the
    owner's prefer-pin-free policy (batch 349 parked a byte-identical 0 for
    exactly this reason);
  * it is measured under a per-file FLAG that the park is not built with;
  * it is a figure for a DIFFERENT function the header discusses -- a landed twin
    in the same piece, say, which is a documented thing to cite;
  * it is on the WRONG CODEGEN SHAPE: batch 350 brief C declined a 4 over an
    installed 5 because the 4 replaced a construct the installed body shares with
    the ROM.  A lower figure is not the goal; the ROM's sequence is.

So this tool RANKS CANDIDATES FOR READING.  It prints the surrounding line so the
reason is usually visible without opening the file.
"""
import argparse
import glob
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CLAIM = re.compile(
    r"(\d+)\s+(?:differing\s+)?(?:[A-Za-z]+\s+){0,3}encodings?\s+of\s+(\d+)", re.I)
CLAIM2 = re.compile(r"(\d+)\s+of\s+(\d+)\s+(?:[A-Za-z]+\s+){0,3}encodings?", re.I)
# MATCH ONLY WHAT IS DEMONSTRABLY AN objcmp FIGURE.
#
# The first two versions of this tool tried to EXCLUDE the other metrics a park
# header carries, and that is the wrong design: a header legitimately holds
# "N of M" from aligncmp's ALIGNED-EQUAL count (where HIGHER IS BETTER, the exact
# reverse), from hunk counts, from percentages, from instruction counts, and from
# figures about other functions.  Excluding them one pattern at a time leaked
# every time -- the second version's `ALIGN\b` did not even match "ALIGNED", so
# parks whose best row is an "ALIGNED RESIDUE 57 of 252" line were still reported
# as hiding a 57.
#
# So require POSITIVE EVIDENCE instead: the pair must sit directly against
# objcmp's own vocabulary -- "encodings", "differ", or "of M encodings" -- in the
# same clause.  A number that cannot be shown to be a differing-encoding count is
# not reported at all.  That trades recall for trustworthiness, which is the
# right trade for a tool whose output is a read-first list.
PAIR = re.compile(
    r"\b(\d+)\s+(?:differing\s+)?(?:[A-Za-z]+\s+){0,2}of\s+(\d+)\s+"
    r"(?:[A-Za-z]+\s+){0,2}encodings?\b"
    r"|\b(\d+)\s+of\s+(\d+)\s+differ", re.I)


def pair_of(m):
    """The (n, total) from whichever alternative matched."""
    if m.group(1) is not None:
        return int(m.group(1)), int(m.group(2))
    return int(m.group(3)), int(m.group(4))


# A line mentioning any of these is usually explaining why a smaller figure was
# NOT installed, so it is reported but flagged as probably-accounted.
EXCUSED = re.compile(r"pin|device|fakematch|NOT install|not shipped|flag|CFLAGS|"
                     r"volatile|asm|clobber|barrier|twin|landed|wrong shape|"
                     # INSTRUMENT is this tree's word for a body measured to prove
                     # a mechanism and never meant to ship -- 80216b4's "THE
                     # INSTRUMENT: 4 of 24, AND IT TAKES A THREE-WAY CROSS" was
                     # flagged READ THIS on the first run for want of it.
                     r"owner|instrument|diagnostic|probe|unshippable|"
                     # and the two shapes that make a smaller figure meaningless
                     r"NOT a distance|counts PHASE", re.I)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--min-gain", type=int, default=1)
    a = ap.parse_args()

    rows = []
    for p in sorted(glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"),
                              recursive=True)):
        t = open(p, errors="replace").read()
        flat = t.replace("*", " ")
        cm = CLAIM.search(flat) or CLAIM2.search(flat)
        if not cm:
            continue
        claim, total = int(cm.group(1)), int(cm.group(2))
        best, where = claim, None
        for m in PAIR.finditer(flat):
            n, tot = pair_of(m)
            if tot != total or n >= best:
                continue
            # EXCLUDE aligncmp's FIGURE, WHICH MEANS THE OPPOSITE.
            # `aligned-equal 661 of 1416` counts encodings that MATCH, so HIGHER
            # IS BETTER -- the reverse of objcmp's differing count.  Both are
            # spelled "N of M", so the first version of this tool reported every
            # park carrying an aligncmp line as having an unrealised gain, and at
            # the top of the list those false rows dominated: a park claiming 1263
            # of 1416 was reported as hiding a "661", which is simply its
            # aligned-equal count.  Two metrics with one textual shape.
            line = flat[max(0, m.start() - 120):m.end() + 120]
            line = re.sub(r"\s+", " ", line).strip()
            best, where = n, line
        if where and claim - best >= a.min_gain:
            rows.append((claim - best, claim, best, total,
                         os.path.relpath(p, ROOT), where))

    rows.sort(reverse=True)
    print(f"{len(rows)} park(s) mention a SMALLER figure than they claim\n")
    for gain, claim, best, total, rel, line in rows:
        flag = "  [probably accounted]" if EXCUSED.search(line) else "  <-- READ THIS"
        print(f"{claim:>4} -> {best:<4} of {total:<5} (gain {gain:>3})  "
              f"{rel.replace('src/non_matching/', '')}{flag}")
        print(f"        ...{line[:150]}...")
    unaccounted = [r for r in rows if not EXCUSED.search(r[5])]
    print(f"\n{len(unaccounted)} of {len(rows)} have no pin/device/flag/twin word "
          f"nearby -- read those first.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
