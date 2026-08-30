#!/usr/bin/env python3
"""whodoesthis.py -- find ALREADY-MATCHING functions whose generated assembly
contains a given instruction shape, so you can read the C that produced it.

WHY THIS EXISTS

Three times in recent rounds a residue was declared unreachable on the strength
of reasoning about what gcc "must" do, and three times that was wrong. The
correction each time came from the same place: look for code in this tree that
ALREADY emits the shape, then read its source.

A corpus count over the remaining assembly tells you what the original authors
wrote. A corpus count over the GENERATED assembly tells you what this compiler
will actually do from C -- which is the only question that matters when you are
stuck.

    python3 tools/whodoesthis.py 'add\\tr3, r3, #39'
    python3 tools/whodoesthis.py --multiline 'mov\\t(r\\d+), r\\d+\\n\\tadd\\t\\1, \\1, #(\\d+)\\n\\tldrb\\t\\1, \\[\\1\\]'

Prints the .c file for every generated .s that matches, with the matched text.
Only files under src/ that are NOT in non_matching/ are considered, so every hit
is a function that compiles byte-identically today.

TWO THINGS IT WILL NOT TELL YOU

It reports the shape, not the reason. The C it points at may differ from yours
in several ways and only one of them matters -- on Func_808e0b0 the matching
function's loop body was wholly wrong to copy (29 differing) while its loop
GUARD was the entire fix.

And a count of zero is weak evidence, not proof. Generated assembly is a sample
of what people have managed to write so far, not of what the compiler allows;
the mov+lsl version of one search returned zero while the pooled version of the
same search returned seventeen.
"""
import re
import subprocess
import sys


def main():
    args = [a for a in sys.argv[1:]]
    multiline = "--multiline" in args
    if multiline:
        args.remove("--multiline")
    if not args:
        sys.exit(__doc__.strip().split("\n\n")[0])
    pat = re.compile(args[0].encode().decode("unicode_escape"),
                     re.M | (re.S if multiline else 0))

    files = subprocess.run(["git", "ls-files", "src"],
                           capture_output=True, text=True).stdout.split()
    hits = 0
    for c in files:
        if not c.endswith(".c") or "non_matching" in c:
            continue
        s = "asm/" + c[len("src/"):-2] + ".s"
        try:
            t = open(s, errors="ignore").read()
        except OSError:
            continue
        m = pat.search(t)
        if m:
            hits += 1
            frag = " / ".join(x.strip() for x in m.group(0).split("\n") if x.strip())
            print(f"{c}\n    {frag[:100]}")
    print(f"\n{hits} matching function(s) emit this shape")
    if hits == 0:
        print("Zero is weak evidence: it may mean nobody has written it yet,")
        print("not that gcc cannot. Try a looser pattern before concluding.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
