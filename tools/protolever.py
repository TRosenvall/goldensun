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
import re
import subprocess
import sys

DECL = re.compile(r"^extern\s+void\s+([A-Za-z_]\w*)\s*\([^;]*\);\s*$", re.M)


def screen(text, ref, tag):
    open("/tmp/_pl.c", "w").write(text)
    out = subprocess.run(["python3", "tools/tryc.py", "/tmp/_pl.c", "--ref", ref,
                          "--quiet"], capture_output=True, text=True).stdout
    line = out.strip().splitlines()[0] if out.strip() else "(no output)"
    print(f"  {tag:<34} {line.strip()}")
    m = re.search(r"(\d+) differ", line)
    return 10 ** 9 if m is None and " OK " not in line else (0 if " OK " in line else int(m.group(1)))


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

    allgone = drop(src, set(names))
    score = screen(allgone, ref, "all prototypes deleted")
    if score < best:
        best = score
    for n in names:
        score = screen(drop(src, {n}), ref, f"without {n}")
        if score < best:
            best = score
    print(f"\nbest: {best} differing")
    return 0


if __name__ == "__main__":
    sys.exit(main())
