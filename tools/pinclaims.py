#!/usr/bin/env python3
"""pinclaims.py -- cross-check a header's CLAIMED pin count against shimcount.

WHY THIS EXISTS.  `tools/parkcheck.py` re-measures a park's FIGURE with objcmp
on every run, so a stale figure cannot survive.  Nothing re-measures a PIN
COUNT.  So a header sentence like

    shimcount.py: 4 register pins (the two `register ... __asm__` operands ...)

is a CACHED NUMBER that reads as authoritative because it names the tool, and it
goes stale the moment the body gains or loses a pin.  Batch 329 found exactly
that on `src/non_matching/ovl_7ac2d8/200d5c0.c`: the header said 4 and
enumerated four, having itself introduced a fifth (`q2 __asm__("r2")`) further
up the same header.

`docs/directory/` is NOT affected -- it calls shimcount itself.  Only prose
drifts.  That bounds the damage: this tool checks DOCUMENTATION, not the match.

    python3 tools/pinclaims.py            # whole tree
    python3 tools/pinclaims.py src/a.c    # named files

Exit 1 if any mismatch survives triage, 0 otherwise.

## READ THIS BEFORE "FIXING" A MISMATCH

A mismatch is **not** automatically a stale number.  A header legitimately
discusses pin counts that are not the file's own:

  * a REJECTED or NOT-INSTALLED pinned candidate -- `2009818.c` records a body
    that is byte-exact at two pins and is parked pin-free per owner policy, so
    "2 register pins" is true of the candidate and false of the file;
  * a TABLE OF ALTERNATIVES -- `ovl_30_a_a_a_c_c.c` lists "EXACT, 3 register
    pins" against "EXACT, 0 pins" and ships the second.

Both are good writing. **Read the sentence before touching it.**

## AND A WARNING FROM THIS TOOL'S OWN FIRST DRAFT

The first pattern here was `(\d+)\s+register pins`, which matched the `2` in
*"the r3/r2 register pins"* and the `3` in *"the p0..p3 register pins"* -- the
digit of a REGISTER NAME.  That reported two landed files as understating their
pins when neither did, and it is the same defect this project keeps finding:
**a screening tool reading prose as machinery.**  Hence NUM below refuses a
digit preceded by a letter, digit, `/`, `_` or `%`.  If you widen this pattern,
re-check it against those two files first.
"""
import glob
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# A claimed count must not be the tail of a register or parameter token.
NUM = r"(?<![A-Za-z0-9/_%])(\d+)"
PAT_NAMED = re.compile(rf"shimcount(?:\.py)?\s*:?\s*{NUM}\s+register pins", re.I)
PAT_BARE = re.compile(rf"{NUM}\s+register pins", re.I)

# Headers known to discuss a pin count that is deliberately not the file's own.
# Keep the REASON with the path; an entry without one is indistinguishable from
# a silenced bug.
EXPECTED = {
    "src/non_matching/ovl_7b4558/2009818.c":
        "records a byte-exact body at 2 pins, parked pin-free per owner policy",
    "src/overlays/rom_77a7c8/ovl_30_a_a_a_c_c.c":
        "tabulates a 3-pin and a 0-pin alternative, both exact; ships the 0-pin one",
}


def claimed(paths):
    out = {}
    for f in paths:
        try:
            t = open(f, errors="replace").read(20000)
        except OSError:
            continue
        m = PAT_NAMED.search(t) or PAT_BARE.search(t)
        if m:
            out[f] = int(m.group(1))
    return out


def actual(paths):
    """Pin counts from shimcount, THE AUTHORITY.  Never re-implement it here."""
    if not paths:
        return {}
    r = subprocess.run([sys.executable, os.path.join(ROOT, "tools", "shimcount.py")]
                       + list(paths), capture_output=True, text=True, cwd=ROOT)
    out, cur = {}, None
    for l in r.stdout.splitlines():
        if l and not l.startswith("    "):
            cur = l.strip()
            out.setdefault(cur, 0)
        elif cur:
            m = re.search(r"register pins\s*:\s*(\d+)", l)
            if m:
                out[cur] = int(m.group(1))
    return out


def main():
    args = sys.argv[1:]
    paths = args or sorted(glob.glob(os.path.join(ROOT, "src/**/*.c"), recursive=True))
    paths = [os.path.relpath(p, ROOT) for p in paths]
    cl = claimed(paths)
    ac = actual(list(cl))
    bad, known = [], []
    for f, c in sorted(cl.items()):
        a = ac.get(f, 0)
        if a == c:
            continue
        (known if f in EXPECTED else bad).append((f, c, a))
    print(f"{len(cl)} file(s) state a pin count in their header")
    if known:
        print(f"\n{len(known)} expected (a count that is deliberately not the file's own):")
        for f, c, a in known:
            print(f"  says {c:4}  has {a:4}   {f}\n        {EXPECTED[f]}")
    if not bad:
        print("\nno unexplained mismatch.")
        return 0
    print(f"\n{len(bad)} UNEXPLAINED MISMATCH(ES) -- read the sentence before editing it:")
    for f, c, a in bad:
        print(f"  says {c:4}  has {a:4}   {f}")
    return 1


if __name__ == "__main__":
    sys.exit(main())
