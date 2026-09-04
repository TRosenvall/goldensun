#!/usr/bin/env python3
"""checkaddr.py -- verify a batch report's addresses against the linked ELFs.

    python3 tools/checkaddr.py reports/batch-189.md
    python3 tools/checkaddr.py --syms          # rebuild the symbol cache first

Run it INSIDE the build container, or run --syms there once and the check
anywhere. Exits non-zero if any address is wrong or any symbol is missing, so
it can gate a batch report the way `make compare` gates a commit.

WHY THIS IS A TOOL. reports/batch-188.md asserted that "every address above was
checked against the linked ELF". It had not been: four of its fourteen were
wrong. The four were exactly the functions with REAL NAMES -- DrawSmallText,
CheckPartyItem, MapActor_SetPos, MapActor_SetPos3D -- while all ten
`Func_<addr>` / `OvlFunc_<n>_<addr>` entries were right.

That distribution is the whole lesson. An address-named symbol carries its own
answer, so "checking" it is a tautology that always passes. A named symbol is
the only case where the check can fail, and it is the case where the address
gets guessed from the .s file's stem or from a neighbouring function. So the
habit produced a verification record that was strongest where it was useless
and absent where it mattered. `0x08091f14` was published for MapActor_SetPos
while still belonging to the unelevated Func_8091f14, and only a later
collision surfaced it.

A claim of verification is worth nothing unless the check can fail. This one
can.
"""
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CACHE = os.path.join(ROOT, "scratch", "addrcheck", "syms.txt")
# a report row:  | 7 | `Name` | `0x080923e4` | ...
ROW = re.compile(r"\|\s*\d+\s*\|\s*`([A-Za-z_]\w*)`\s*\|\s*`(0x[0-9a-fA-F]+)`")


def build_syms():
    elves = [os.path.join(ROOT, "goldensun.elf")]
    ovl = os.path.join(ROOT, "overlays")
    if os.path.isdir(ovl):
        for d in sorted(os.listdir(ovl)):
            e = os.path.join(ovl, d, "overlay.elf")
            if os.path.exists(e):
                elves.append(e)
    out = []
    for e in elves:
        if not os.path.exists(e):
            continue
        r = subprocess.run(["arm-none-eabi-nm", e], capture_output=True, text=True)
        if r.returncode == 0:
            out.append(r.stdout)
    if not out:
        sys.exit("checkaddr: no ELFs found -- build first, and run this in the container")
    os.makedirs(os.path.dirname(CACHE), exist_ok=True)
    with open(CACHE, "w") as f:
        f.write("".join(out))
    return "".join(out)


def load_syms():
    if os.path.exists(CACHE) and "--syms" not in sys.argv:
        return open(CACHE, errors="ignore").read()
    return build_syms()


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    syms = load_syms()
    table = {}
    for line in syms.splitlines():
        p = line.split()
        if len(p) == 3 and p[1] in ("T", "t"):
            table.setdefault(p[2], p[0])
    if not args:
        print("usage: checkaddr.py <report.md> [...]")
        return 0
    bad = 0
    for path in args:
        rows = ROW.findall(open(path, errors="ignore").read())
        if not rows:
            print("%s: no address rows found" % path)
            continue
        print("%s: %d addresses" % (path, len(rows)))
        for name, claimed in rows:
            actual = table.get(name)
            if actual is None:
                print("  MISSING  %-26s claimed %s" % (name, claimed))
                bad += 1
            elif int(actual, 16) != int(claimed, 16):
                print("  WRONG    %-26s claimed %s  actual 0x%s" % (name, claimed, actual))
                bad += 1
            else:
                print("  ok       %-26s %s" % (name, claimed))
    if bad:
        print("\n%d address(es) wrong -- fix the report before publishing." % bad)
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
