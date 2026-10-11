"""backfill.py -- measure parks that carry NO FIGURE, and rank what comes back.

    python3 tools/backfill.py            # list the measurable no-claim parks
    python3 tools/backfill.py --emit     # emit an objcmp command per park

WHY.  A park with no parseable claim is invisible to every targeting instrument
in this tree: it is absent from the figure bands, parkcheck reports NO CLAIM, and
reports/frontier.md cannot rank it.  675 park files hold 505 with a claim and
**170 without**, so a quarter of the remaining work has never been measured --
and an unmeasured park could be at 2 or at 900, with nothing to say which.

Of those 170, **144 are prose/triage/class parks with no compilable body**: they
need a candidate written before anything can be measured, which is agent work.
The other **26 have a body**, which means a figure is one compile away and needs
no agent at all.  That is the cheap half, and this tool finds it.

The precedent is batch 324's backfill, which measured six NO-CLAIM parks and
found figures sitting in them unread.  parkcheck's own header records the same
class: "a claim nothing can parse cannot be caught lying."

HOW IT RESOLVES A REFERENCE.  These parks have no `Verify with:` recipe either,
so the `.s` comes from the first `asm/**.s` path in the file that still EXISTS on
disk -- the same rule tools/piece.py uses, and for the same reason: several parks
name an `.s` that a later split consumed.
"""
import glob
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CLAIM = re.compile(
    r"(\d+)\s+(?:differing\s+)?(?:[A-Za-z]+\s+){0,3}encodings?\s+of\s+(\d+)", re.I)
CLAIM2 = re.compile(r"(\d+)\s+of\s+(\d+)\s+(?:[A-Za-z]+\s+){0,3}encodings", re.I)
DEF = re.compile(r"^[A-Za-z_][A-Za-z0-9_ \t\*]*?\b(\w+)\s*\([^;{]*?\)\s*\{", re.M | re.S)
SUBJ = re.compile(r"\s*/\*+\s*((?:Ovl)?Func_\w+|[A-Z][A-Za-z0-9_]*)")


def strip_comments(s):
    s = re.sub(r"/\*.*?\*/", " ", s, flags=re.S)
    return re.sub(r"//[^\n]*", " ", s)


def reference_of(path, text):
    """First asm/**.s named in the file that exists on disk."""
    for m in re.finditer(r"(asm/[A-Za-z0-9_/]+\.s)", text):
        if os.path.exists(os.path.join(ROOT, m.group(1))):
            return m.group(1)
    return None


def subject_of(path, text, code):
    """The function this park is about: its first DEFINITION, else the header subject."""
    m = DEF.search(code)
    if m:
        return m.group(1)
    m = SUBJ.match(text)
    return m.group(1) if m else None


def scan():
    out = []
    for p in sorted(glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"),
                              recursive=True)):
        t = open(p, errors="replace").read()
        if CLAIM.search(t[:9000]) or CLAIM2.search(t[:9000]):
            continue                      # already has a figure
        code = strip_comments(t)
        if not DEF.search(code):
            continue                      # bodyless: needs a candidate first
        rel = os.path.relpath(p, ROOT)
        out.append((rel, reference_of(rel, t), subject_of(rel, t, code),
                    len(t.splitlines())))
    return out


def main():
    rows = scan()
    emit = "--emit" in sys.argv
    print(f"{len(rows)} park(s) with a BODY and NO FIGURE -- each one compile away\n")
    ok = 0
    for rel, ref, subj, n in rows:
        if ref and subj:
            ok += 1
            if emit:
                print(f"docker run --rm --security-opt seccomp=unconfined "
                      f'-v "$PWD:/work" -w /work goldensun-build python3 '
                      f"tools/objcmp.py {rel} {ref} --func {subj}")
            else:
                print(f"  {subj:28} {n:>5} lines  {rel}")
                print(f"  {'':28} {'':>5}        {ref}")
        else:
            print(f"  !! {rel}: "
                  f"{'no reference .s on disk' if not ref else 'no subject found'}")
    if not emit:
        print(f"\n{ok} of {len(rows)} are ready to measure. "
              f"Run with --emit to get the commands, or:")
        print("  python3 tools/backfill.py --emit | sh")
    return 0


if __name__ == "__main__":
    sys.exit(main())
