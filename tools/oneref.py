#!/usr/bin/env python3
"""oneref.py -- extract ONE function into a standalone .s for tryc.py --ref.

WHY THIS EXISTS

tryc.py only runs its .text size check when the reference holds a single
function. Against a multi-function .s it prints

    [size check skipped: ref has 4 functions]

and compares instructions only. That note means the candidate has NOT been
size-checked, and batch 123 shipped a screen that said OK while the .text was
0x9c against the reference's 0x98 -- an instruction-for-instruction match that
fails `make compare` because it shifts everything after it in the TU.

Most targets live inside a multi-function .s, so the size check was silently off
for most screens. This makes turning it back on one command:

    python3 tools/oneref.py <function>            # writes scratch/<fn>.s
    docker run ... tools/tryc.py cand.c --ref scratch/<fn>.s

It copies the file's `.include` preamble so the result assembles, then the
function's block verbatim. It does NOT copy any trailing `.section .data` --
that belongs to the file, not the function, and including it would make the size
check compare the wrong thing.

STILL NOT A VERDICT. A passing size check is necessary, not sufficient:
OvlFunc_903_2008fc8 passed it with identical objdump section sizes and the
linked overlay still differed in 58 bytes, because pool loads normalise to
`=value` and a pool holding different values at the same distance compares
equal. `make compare` remains the authority. See docs/elevation.md,
"Fakematches: what the screen cannot settle".
"""
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def main():
    if len(sys.argv) < 2:
        sys.exit("usage: oneref.py <function> [out.s]")
    fn = sys.argv[1]
    out = sys.argv[2] if len(sys.argv) > 2 else os.path.join(ROOT, "scratch", fn + ".s")

    hit = subprocess.run(["grep", "-rl", r"^\.thumb_func_start\(_noalign\)\? " + fn + r"\b",
                          os.path.join(ROOT, "asm")], capture_output=True, text=True)
    paths = [p for p in hit.stdout.split("\n") if p.strip()]
    if not paths:
        sys.exit("not found in asm/: " + fn)
    if len(paths) > 1:
        sys.exit("ambiguous, name the file yourself:\n  " + "\n  ".join(paths))
    src = paths[0]

    lines = open(src, errors="ignore").read().split("\n")
    pre = [l for l in lines[:12] if l.strip().startswith(".include")]
    start = re.compile(r"^\.thumb_func_start(?:_noalign)? " + re.escape(fn) + r"\b")
    end = re.compile(r"^\.func_end " + re.escape(fn) + r"\b")
    body, on = [], False
    for l in lines:
        if start.match(l):
            on = True
        if on:
            body.append(l)
        if on and end.match(l):
            break
    if not body:
        sys.exit("could not delimit " + fn + " in " + src)

    os.makedirs(os.path.dirname(out), exist_ok=True)
    open(out, "w").write("\n".join(pre + [""] + body) + "\n")
    print(f"{out}   ({src}, {len(body)} lines)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
