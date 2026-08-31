#!/usr/bin/env python3
"""protolever.py -- retry a parked candidate with callee prototypes deleted.

WHY THIS EXISTS

`mov r0` landing at the START of a call's argument setup where the ROM has it at
the END was read as a scheduler residue for many rounds, and parked as one 69
times.  It is not.  Calling a function through C's implicit `int f()` -- no
declaration at all -- moves that `mov r0` to the end.  OvlFunc_954_20095e0
recorded this in batch 138 and it was not generalised; OvlFunc_956_200a4d0,
OvlFunc_955_20092f0 and OvlFunc_955_2009424 all responded to it in one round
once it was.

An EMPTY PARAMETER LIST is not the same lever.  `extern void f();` was screened
on three of those functions and moved nothing.  The declaration has to be gone.

    python3 tools/protolever.py scratch/H1234_best.c --ref asm/.../x.s

Screens the candidate as written, then with every void-returning callee's
prototype deleted, then with each one deleted on its own, and prints the
differing count for each.  Deleting a prototype for a callee whose RESULT is
used would change the type of the call, so only `extern void` declarations are
touched.
"""
import os
import re
import subprocess
import sys

DECL = re.compile(r"^extern\s+void\s+([A-Za-z_]\w*)\s*\([^;]*\);\s*$", re.M)


def screen(text, ref, tag):
    tmp = f"scratch/_protolever_{os.getpid()}.c"
    open(tmp, "w").write(text)
    out = subprocess.run(["python3", "tools/tryc.py", tmp, "--ref", ref,
                          "--quiet"], capture_output=True, text=True).stdout
    os.unlink(tmp)
    line = out.strip().splitlines()[0] if out.strip() else "(no output)"
    print(f"  {tag:<34} {line.strip()}")
    # tryc prints "  OK <name> (N lines)" INDENTED. `out.strip()` removes that
    # indent, so a test for " OK " with a leading space never fires and a MATCH
    # scores 10**9 -- the worst possible result. That hid a match on
    # Actor_SetAnimAndSpeed: the sweep printed two OK rows and then reported
    # "best: 4 differing, as written". Match on the token, not on surrounding
    # whitespace.
    ok = line.split(None, 1)[0] == "OK" if line else False
    if ok:
        return 0
    m = re.search(r"(\d+) differ", line)
    return int(m.group(1)) if m else 10 ** 9


def main():
    path = sys.argv[1]
    ref = sys.argv[sys.argv.index("--ref") + 1]
    src = open(path).read()
    names = DECL.findall(src)
    print(f"{path}: {len(names)} void-returning callees declared\n")
    best = screen(src, ref, "as written")
    if not names:
        return 0

    def drop(text, keep):
        return DECL.sub(lambda m: "" if m.group(1) in keep else m.group(0), text)

    screen(drop(src, set(names)), ref, "all prototypes deleted")

    # Greedy hill-climb: try each deletion ALONE, keep the single best, repeat.
    # Deleting them all at once is usually worse -- over the 62 saved candidates
    # with a reference it made 37 worse, 21 unchanged and 4 better -- because the
    # lever is per call site. Deleting one at a time is what found the fix on
    # OvlFunc_952_20085a4, whose park note had named the wrong call.
    dropped, cur = [], best
    while True:
        pick, pick_score = None, cur
        for n in names:
            if n in dropped:
                continue
            score = screen(drop(src, set(dropped + [n])), ref,
                           "drop " + ", ".join(dropped + [n]))
            if score < pick_score:
                pick, pick_score = n, score
        if pick is None:
            break
        dropped.append(pick)
        cur = pick_score
        if cur == 0:
            break
    print(f"\nbest: {cur} differing"
          + (f", deleting {', '.join(dropped)}" if dropped else ", as written"))
    return 0


if __name__ == "__main__":
    sys.exit(main())
