#!/usr/bin/env python3
"""blocked_cse.py -- find functions blocked by pool/computed-constant CSE.

Batch 111 established the rule (docs/elevation.md, "Pool-constant CSE: the
complete rule"): a constant that costs a pool load OR two instructions to build,
used twice with one use DOMINATING the other, is hoisted by gcc-2.96 into a
callee-saved register. The ROM often reloads it. Getting the reload needs BOTH a
control-flow boundary between the uses AND -fno-rerun-cse-after-loop. With no
boundary -- no label between the two uses -- nothing reaches it.

So a function is blocked when the same materialised constant appears twice
inside one label-free region. This finds them, so candidate picking can avoid
them and so the size of the problem is known.

Constants counted, because all three are what gcc CSEs:
    ldr rN, =V                      -> V        (pool load)
    mov rN, #K ... lsl rN, #S       -> K<<S     (two-instruction build)
    mov rN, #K ... neg rN, rN       -> -K       (two-instruction build)

    python3 tools/blocked_cse.py [--band LO HI] [--list N]

HOW MUCH TO TRUST IT. The test is "the same materialised constant appears twice
with no label between", which is a proxy for "the first use dominates the second
inside one region". It is right about the shape and approximate about the
domination -- a label between two uses might be a loop head rather than a real
boundary, and gcc's CSE regions are basic blocks rather than label-to-label
spans.

Validated positively: it flags src/non_matching/rom_7d30e0/2009838.c, which is
parked on exactly this and where six CSE-related flags were measured.

Read the output as an upper bound. The subset with ZERO labels anywhere is
certain -- there is no boundary to be had -- and that is 98 functions and 2% of
the remaining mass. The full flagged set is 630 functions and 49%. The truth is
between, nearer the top for long straight-line functions and nearer the bottom
for branchy ones.
"""
import argparse, glob, re, sys

START = re.compile(r"^\.thumb_func_start (\S+)")
END = re.compile(r"^\.func_end")
INSN = re.compile(r"^\t([a-z][a-z0-9.]*)")
LAB = re.compile(r"^\.L\w+:")
POOL = re.compile(r"^\tldr\t(r\d+), =(0x[0-9a-f]+|\d+)$")
MOVI = re.compile(r"^\tmov\t(r\d+), #(0x[0-9a-f]+|\d+)$")
LSL = re.compile(r"^\tlsl\t(r\d+), #(0x[0-9a-f]+|\d+)$")
NEG = re.compile(r"^\tneg\t(r\d+), (r\d+)$")


def num(s):
    return int(s, 16) if s.startswith("0x") else int(s)


def scan(path):
    name = None
    for line in open(path, errors="replace"):
        line = line.rstrip("\n")
        m = START.match(line)
        if m:
            name, n, labs, pend, seen, bad = m.group(1), 0, 0, {}, set(), []
            continue
        if name is None:
            continue
        if END.match(line):
            yield name, n, labs, bad
            name = None
            continue
        if LAB.match(line):
            labs += 1
            seen, pend = set(), {}
        m = POOL.match(line)
        if m:
            v = num(m.group(2))
            (bad.append(v) if v in seen else seen.add(v))
        m = MOVI.match(line)
        if m:
            pend[m.group(1)] = num(m.group(2))
        m = LSL.match(line)
        if m and m.group(1) in pend:
            v = pend.pop(m.group(1)) << num(m.group(2))
            (bad.append(v) if v in seen else seen.add(v))
        m = NEG.match(line)
        if m and m.group(2) in pend:
            v = -pend.pop(m.group(2))
            (bad.append(v) if v in seen else seen.add(v))
        m = INSN.match(line)
        if m and m.group(1) != "align":
            n += 1


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--band", nargs=2, type=int, default=[0, 10 ** 9])
    ap.add_argument("--list", type=int, default=0)
    a = ap.parse_args()
    lo, hi = a.band
    tot = ti = 0
    blocked = []
    for f in glob.glob("asm/**/*.s", recursive=True):
        for name, n, labs, bad in scan(f):
            if not (lo <= n <= hi):
                continue
            tot += 1
            ti += n
            if bad:
                blocked.append((n, len(bad), name, f))
    bi = sum(b[0] for b in blocked)
    print("band %d-%d: %d functions, %d instructions" % (lo, hi, tot, ti))
    print("blocked by constant CSE with no boundary: %d functions (%.0f%%), "
          "%d instructions (%.0f%% of the band)"
          % (len(blocked), 100.0 * len(blocked) / max(tot, 1), bi,
             100.0 * bi / max(ti, 1)))
    for b in sorted(blocked, reverse=True)[:a.list]:
        print("  %4d insns  %2d repeats  %-26s %s" % b)
    return 0


if __name__ == "__main__":
    sys.exit(main())
