#!/usr/bin/env python3
"""filtered.py -- the selection filter from docs/elevation.md, as a tool.

The filter was described in the doc ("A SELECTION FILTER that works") and used
once, but never committed as code, so every round has had to re-derive it. It
rejects a candidate if ANY of these hold:

  * under 40 instructions   -- no register pressure for the levers to act on
  * over 120 instructions   -- too many independent residues to converge on
  * uses r8-r11             -- allocation-priority residues
  * repeats an expensive constant (a pooled value, or a mov+lsl pair) anywhere
                            -- CSE if the uses are close, PRE hoisting if one
                               dominates the other; neither yields to spelling
  * fewer than 8 calls      -- arithmetic-heavy bodies hit instruction
                               selection rather than the documented levers

Detector note carried over from the doc: a `mov rN, #imm` followed by a later
`lsl rN, #k` counts as ONE constant even when other instructions sit between
them. Requiring adjacency is what let the first version pass the very function
whose PRE hoisting motivated the filter.

Scans must use \\s+ and never a literal space: .s files put a TAB between
mnemonic and operands, and a literal space silently returns zero hits.
"""
import glob
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pickable
import shapesib

HIGH = re.compile(r"\br(?:8|9|10|11)\b")
POOL = re.compile(r"\bldr\s+r\d+,\s*=(\S+)")
MOVI = re.compile(r"\bmov\s+(r\d+),\s*#(0x[0-9a-f]+|\d+)")
LSL = re.compile(r"\blsl\s+(r\d+),\s*#(0x[0-9a-f]+|\d+)")
WRITE = re.compile(r"\b(?:mov|add|sub|ldr|ldrb|ldrh|lsl|lsr|asr|and|orr|eor|neg|mul|bic)\s+(r\d+)")


def expensive_constants(body):
    """Pooled values, plus mov+lsl builds, as a list of value keys."""
    vals = []
    pend = {}
    for l in body:
        m = POOL.search(l)
        if m:
            vals.append("pool:" + m.group(1))
        m = MOVI.search(l)
        if m:
            pend[m.group(1)] = int(m.group(2), 0)
            continue
        m = LSL.search(l)
        if m and m.group(1) in pend:
            vals.append("built:%d" % (pend.pop(m.group(1)) << int(m.group(2), 0)))
            continue
        m = WRITE.search(l)
        if m:
            pend.pop(m.group(1), None)
    return vals


def passes(body):
    ins = [l for l in body if l.strip() and not l.strip().startswith((".", "@", "/*"))]
    n = len(ins)
    if not (40 <= n <= 120):
        return None
    if any(HIGH.search(l) for l in ins):
        return None
    calls = sum(1 for l in ins if re.search(r"\bbl\s+", l))
    if calls < 8:
        return None
    vals = expensive_constants(ins)
    if len(vals) != len(set(vals)):
        return None
    return n, calls


if __name__ == "__main__":
    parked = pickable.parked()
    rows = []
    for s in glob.glob("asm/**/*.s", recursive=True):
        if os.path.exists(s[:-2].replace("asm/", "src/", 1) + ".c"):
            continue
        for name, body in shapesib.functions(s):
            if name in parked:
                continue
            r = passes(body)
            if r:
                rows.append((r[1], r[0], name, s, bool(shapesib.kin(s))))
    rows.sort(key=lambda r: (-r[0], r[1]))
    print("%d candidates pass the filter\n" % len(rows))
    for calls, n, name, s, haskin in rows[:25]:
        print("  %2d calls %3di %-28s %s%s" % (calls, n, name, s, "  [kin]" if haskin else ""))
