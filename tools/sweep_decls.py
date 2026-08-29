#!/usr/bin/env python3
"""sweep_decls.py -- try the declaration levers on every parked function.

WHY THIS EXISTS

Three functions came out of the parked set in two rounds, and all three were
parked with notes that ruled out something the note never tried. The pattern
is always the same: the diff sits next to some feature of the C -- a pointer,
a script variable, a constant -- and the search goes there, while the actual
lever is a prototype somewhere else in the file.

    LoadStatusIcon    "the order does not move"  -- three attempts, none of
                      them the declaration of the call that was wrong
    Func_8078948      cited LoadStatusIcon's note as the same class
    OvlFunc_945_200d068
                      three attempts, all about the script variables

There are two declaration levers and they pull in opposite directions:

  * the PRECEDING call's return type decides whether r0 is live across it, and
    so whether the next call fills r0 first or last;
  * the MISMATCHING call's own declaration decides the order gcc fills THAT
    call's argument registers.

Neither is guessable from the diff, both are one line of C, and the search
space per file is small -- one variant per declared callee. That is exactly
the shape of thing to do mechanically rather than by judgement.

WHAT IT DOES

For each parked .c, screens the file as-is, then screens one variant per
`extern`-declared function with that declaration REMOVED. Reports any variant
that comes back OK, and any that merely gets shorter -- a variant that drops
the instruction delta is worth a human look even when it does not match.

It also tries the other direction, which turns out to be the productive one.
Most parked files already call most of their callees implicitly -- that is the
house style in this tree -- so there are only about 1.5 declarations per file
to remove. Adding one does NOT need a parameter signature:

    extern void Foo();

declares the RETURN TYPE and leaves the parameters unspecified, and the return
type is the whole of the first lever. An implicitly declared function returns
`int`, so gcc keeps r0 live across the call; `void` releases it. That is one
generated line per implicitly called name, and no inference.

Variants that fail to compile -- because the return value is actually used --
simply screen as non-matches, so no filtering is needed.

RUN IT INSIDE THE CONTAINER, once, rather than one container per file:

    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        python3 tools/sweep_decls.py

A reported match is a SCREEN, not a verdict -- it still needs a split and
`make compare`. See tools/rescreen_park.py for why the flags come from the
--ref path and not from the parked file's own location.

TRUSTING A ZERO

This sweep's useful answer is usually "nothing", and a tool that reports
nothing is indistinguishable from a tool that is broken. So it carries a
self-test built from the one case known to work -- OvlFunc_945_200d068, which
matches only when __MapActor_SetBehavior is declared:

    docker run ... python3 tools/sweep_decls.py --selftest

That case also establishes that the generated `extern void Foo();` form is
enough. It was checked against the full prototype `extern void Foo(int,
unsigned char *);` and both produce a byte-identical match, which is what
makes direction 2 legitimate without inferring parameters.

Run the self-test before believing a clean sweep.
"""
import glob
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = re.compile(r"Source asm:\s*goldensun/(\S+\.s)")
# an extern that declares a FUNCTION -- name followed by an open paren.
# Deliberately not matching extern data declarations, which have no effect
# on argument fill order and would only add noise.
DECL = re.compile(r"^\s*extern\b.*?\b(\w+)\s*\(")
RESULT = re.compile(r"^\s*(OK|XX)\s+(\S+)(.*)$", re.M)
DELTA = re.compile(r"rom (\d+) lines, ours (\d+)")

TMP = "/tmp/sweep_decls"
# things that look like a call but are not one
KEYWORDS = {"if", "for", "while", "switch", "return", "sizeof", "do",
            "int", "char", "short", "long", "unsigned", "signed", "void",
            "struct", "union", "enum", "static", "const", "volatile",
            "u8", "u16", "u32", "s8", "s16", "s32", "fx32", "asm", "__asm__"}


def screen(path, ref):
    r = subprocess.run(
        [sys.executable, os.path.join(ROOT, "tools", "tryc.py"),
         path, "--ref", ref, "--quiet"],
        capture_output=True, text=True, cwd=ROOT)
    return r.stdout + r.stderr


def verdict(out):
    """(matched, delta) -- delta is |rom - ours| or None if unparseable."""
    m = RESULT.search(out)
    if not m:
        return False, None
    if m.group(1) == "OK":
        return True, 0
    d = DELTA.search(out)
    return False, abs(int(d.group(1)) - int(d.group(2))) if d else None


SELFTEST_C = "src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_a_a_b.c"
SELFTEST_FN = "OvlFunc_945_200d068"
SELFTEST_REF = "asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_a_a.s"
SELFTEST_COMMIT = "0dcb12d^"


def selftest():
    """Prove direction 2 can find a match that is known to exist.

    Takes the elevated OvlFunc_945_200d068, strips the one declaration that
    makes it match, and checks the sweep puts it back. If this fails, a clean
    sweep means nothing.
    """
    os.makedirs(TMP, exist_ok=True)
    ref = os.path.join(TMP, "selftest_ref.s")
    r = subprocess.run(["git", "show", f"{SELFTEST_COMMIT}:{SELFTEST_REF}"],
                       capture_output=True, text=True, cwd=ROOT)
    if r.returncode:
        print("SELFTEST SKIPPED: cannot recover the pre-split assembly")
        return 0
    open(ref, "w").write(r.stdout)

    lines = open(os.path.join(ROOT, SELFTEST_C), errors="replace").read().split("\n")
    stripped = [l for l in lines if "__MapActor_SetBehavior(" not in l
                or not l.startswith("extern")]
    if len(stripped) == len(lines):
        print("SELFTEST BROKEN: the declaration it strips is no longer there")
        return 1
    var = os.path.join(TMP, "selftest.c")
    open(var, "w").write("\n".join(stripped))

    if verdict(screen(var, ref))[0]:
        print("SELFTEST FAILED: matches WITHOUT the declaration, so this case "
              "no longer discriminates and proves nothing")
        return 1
    at = next(i for i, l in enumerate(stripped) if l.startswith("extern"))
    open(var, "w").write("\n".join(
        stripped[:at] + ["extern void __MapActor_SetBehavior();"] + stripped[at:]))
    if not verdict(screen(var, ref))[0]:
        print("SELFTEST FAILED: cannot re-find a match that is known to exist")
        return 1
    print(f"selftest OK -- {SELFTEST_FN} lost and re-found by direction 2")
    return 0


def main():
    if "--selftest" in sys.argv:
        return selftest()
    only = [a for a in sys.argv[1:] if not a.startswith("-")]
    os.makedirs(TMP, exist_ok=True)
    hits, better, checked = [], [], 0

    for p in sorted(glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"),
                              recursive=True)):
        rel = os.path.relpath(p, ROOT)
        if only and not any(o in rel for o in only):
            continue
        text = open(p, errors="replace").read()
        m = SRC.search(text)
        if not m or not os.path.exists(os.path.join(ROOT, m.group(1))):
            continue
        ref = m.group(1)
        lines = text.split("\n")

        base_ok, base_delta = verdict(screen(rel, ref))
        if base_ok:
            hits.append((rel, "AS-IS", ref))
            print(f"  MATCH AS-IS  {rel}")
            continue

        var = os.path.join(TMP, os.path.basename(rel))
        found = False

        def try_variant(body, label):
            """Screen one generated variant; record a match or an improvement."""
            nonlocal found, checked
            checked += 1
            open(var, "w").write(body)
            ok, delta = verdict(screen(var, ref))
            if ok:
                hits.append((rel, label, ref))
                print(f"  MATCH        {rel}   {label}")
                found = True
                return True
            if (delta is not None and base_delta is not None
                    and delta < base_delta):
                better.append((rel, label, base_delta, delta))
                print(f"  shorter      {rel}   {label}: "
                      f"{base_delta} -> {delta}")
            return False

        # direction 1: drop an existing declaration
        for i, line in enumerate(lines):
            d = DECL.match(line)
            if not d:
                continue
            if try_variant("\n".join(lines[:i] + lines[i + 1:]),
                           f"drop decl of {d.group(1)}"):
                break

        # direction 2: declare an implicitly called function `void`
        if not found:
            declared = {d.group(1) for d in
                        (DECL.match(l) for l in lines) if d}
            called = set(re.findall(r"\b(\w+)\s*\(", text))
            defined = set(re.findall(r"^\w[\w \t*]*?\b(\w+)\s*\([^;]*$",
                                     text, re.M))
            # `insert` goes above the first extern so the declaration is in
            # scope at every call, wherever in the body the call sits
            at = next((i for i, l in enumerate(lines)
                       if l.startswith("extern")), None)
            if at is not None:
                for name in sorted(called - declared - defined - KEYWORDS):
                    if try_variant(
                            "\n".join(lines[:at] + [f"extern void {name}();"]
                                      + lines[at:]),
                            f"declare {name} void"):
                        break

    print(f"\n{checked} variants screened across "
          f"{len(set(h[0] for h in hits)) + len(better)} interesting files")
    print(f"{len(hits)} match, {len(better)} shorter-but-not-matching")
    return 0


if __name__ == "__main__":
    sys.exit(main())
