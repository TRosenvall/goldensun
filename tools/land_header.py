#!/usr/bin/env python3
"""land_header.py CAND.c DEST.c HEADER.txt -- write DEST.c as HEADER + CAND's body.

Strips CAND's own leading /* ... */ comment (an agent's park-style header) and
prepends HEADER (the landing header: address, what was split or converted, and
the lever that closed it). Used for every landing in batches 286-289.

Caveat learned the hard way: if CAND starts with a `// fakematch` line comment
instead of a block comment, nothing is stripped and the result has two headers
-- use the candidate as-is in that case.
"""
import sys

cand, dest, header = sys.argv[1:4]
s = open(cand).read().lstrip()
if s.startswith("/*"):
    s = s[s.index("*/") + 2:].lstrip("\n")
open(dest, "w").write(open(header).read().rstrip("\n") + "\n" + s)
