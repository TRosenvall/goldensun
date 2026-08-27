#!/usr/bin/env python3
"""label_false_negatives.py -- find parks whose screen is a LABEL cascade.

Batch 112: OvlFunc_898_2008a4c was parked on a 25-of-50 screen and its parked C
was already byte-perfect. Every differing position cascaded from ONE redundant
label -- gcc emitted the pool-skip label immediately before the `if`s own join
label, two definitions at the same address. A label emits no bytes.

tools/tryc.py keeps branched-to label definitions in the stream deliberately,
which is correct; but one extra label shifts every later position, so the
positional count says "25 differ" about a function that differs in nothing.

This screens every park and reports the ones whose FIRST differing line is a
label definition on either side. Those are the candidates for a byte check --
and, if the bytes match, for unparking with their C unchanged.

    python3 tools/label_false_negatives.py [--jobs N]
"""
import argparse, glob, os, re, subprocess, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ARROW = re.compile(r"^\s+-> rom (.*?)\s{2,}ours (.*)$")
LABEL = re.compile(r"^L\d+:$")
FUNC = re.compile(r"^(?:void|int|unsigned|short|char|struct|static|u8|u16|u32|s8|s16|s32)[\w \*]*?(\w+)\s*\(", re.M)


def screen(path, flags):
    cmd = ["docker", "run", "--rm", "-v", "%s:/work" % ROOT, "-w", "/work",
           "goldensun-build", "python3", "tools/tryc.py", path, "--full"]
    if flags:
        cmd += ["--cflags", flags]
    r = subprocess.run(cmd, capture_output=True, text=True, timeout=900)
    return r.stdout


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--flags", default="")
    a = ap.parse_args()
    parks = sorted(glob.glob("src/non_matching/**/*.c", recursive=True))
    hits = []
    for p in parks:
        try:
            out = screen(p, a.flags)
        except subprocess.TimeoutExpired:
            continue
        if " OK " in out and "XX" not in out:
            print("ALREADY OK   %s" % p)
            continue
        for line in out.split("\n"):
            m = ARROW.match(line)
            if not m:
                continue
            rom, ours = m.group(1).strip(), m.group(2).strip()
            if LABEL.match(rom) or LABEL.match(ours):
                hits.append((p, rom, ours))
                print("LABEL FIRST  %-52s rom %-14s ours %s" % (p, rom, ours))
            break
    print("\n%d parks screened, %d open on a label" % (len(parks), len(hits)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
