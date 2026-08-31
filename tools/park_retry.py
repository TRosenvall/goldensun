#!/usr/bin/env python3
"""park_retry.py -- rank parks for RE-ATTACK by levers found after they were written.

WHY

OvlFunc_946_20092b4 sat parked at TWO differing on the orr-destination residue.
Its park listed two spellings tried and rejected, and neither was the documented
fix -- `unsigned char m = 2;` matched outright. The park simply predated the
lever. OvlFunc_896_200c260 was parked at 78 of 85 as an unreachable register
rotation and came to 4 once two locals were merged into one.

A park is a snapshot of what was known the day it was written. The inventory
grows every batch, so the park corpus is a candidate pool that quietly refills.

The counter-example matters as much: OvlFunc_947_2009fd4 is also at 2 differing
on an orr residue and its park HAS tried the narrow local, at 18 differing,
along with five other spellings. A thorough park stays parked. This tool ranks
by "close, and does not mention lever X" -- it does not claim the lever works,
only that it was never written down as tried.

Read the park before re-screening, always.
"""
import glob
import os
import re
import sys

# (name, residue pattern that suggests it, phrases meaning it was already tried)
LEVERS = [
    ("merge-vars",  r"register[- ]role|rotation|exchanged|swap",
                    r"merg|one variable|same variable|single local|one register"),
    # only the ORR-DESTINATION shape: a mov'd constant next to the orr, with
    # the registers exchanged. A bare `orr` anywhere in a quoted listing is not
    # this shape, and matching it produced false positives on the first sweep.
    ("uchar-orr",   r"mov\s+r\d+,\s*#0x?[0-9a-f]+\s*/\s*orr\s+r\d+,\s*r\d+",
                    r"unsigned char"),
    ("int-and",     r"mov\s+r\d+,\s*#0x?[0-9a-f]+\s*/\s*and\s+r\d+,\s*r\d+",
                    r"int m\b|`int` local|int local|int mask"),
    ("no-proto",    r"argument (?:fill )?order|fill order|arguments? .*reversed",
                    r"prototype|implicit|declared|declaration"),
    ("addr-local",  r"ldr r\d+, ?\[r\d+\]|address",
                    r"address-only|delete the local|only holds an address"),
    ("volatile",    r"re-?read|reload|loads? (?:it )?twice",
                    r"volatile"),
]
NUM = re.compile(r"(\d+)\s+(?:of|differing)")


def best_count(text):
    m = NUM.search(text)
    return int(m.group(1)) if m else None


if __name__ == "__main__":
    limit = int(sys.argv[1]) if len(sys.argv) > 1 else 25
    rows = []
    for f in glob.glob("src/non_matching/**/*.c", recursive=True):
        t = open(f, errors="ignore").read()
        n = best_count(t)
        if n is None or n > 20:
            continue
        untried = [name for name, residue, tried in LEVERS
                   if re.search(residue, t, re.I) and not re.search(tried, t, re.I)]
        if untried:
            rows.append((n, f, untried))
    rows.sort()
    print("%d close parks (<=20 differing) with an untried lever\n" % len(rows))
    for n, f, u in rows[:limit]:
        print("  %3d  %-46s %s" % (n, f.replace("src/non_matching/", ""), ",".join(u)))
