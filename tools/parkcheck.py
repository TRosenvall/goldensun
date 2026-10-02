#!/usr/bin/env python3
"""parkcheck.py -- verify that each park's HEADER CLAIM matches what its BODY measures.

    docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
        goldensun-build python3 tools/parkcheck.py [path ...]

WHY THIS EXISTS. A park is a candidate .c plus a header stating how far it got. The
two are written in the same commit and nothing checked that they agreed, so they
drifted -- twice, in consecutive batches, both times because a header was updated
with the improved numbers while the BODY was left as the older candidate:

  * batch 282 advanced Func_80a96d8 from 10 to 4 encodings.  The header said 4; the
    body still measured 10, because the one-line edit the header described was never
    actually written into the C below it.
  * batch 283 advanced BaseAnim_Tackle from 47 to 12.  Same shape: header 12, body 47.

Both were caught by an agent re-measuring a park it had been told to start from --
i.e. by the next reader paying for the error.  Batch 281 had already recorded the
lesson ("read a park's header against its body before committing -- the body is
evidence about the header") and it recurred anyway, which is the argument for a tool
rather than a habit.

HOW IT FINDS THE REFERENCE.  Parks carry a verify recipe in their header:

    Verify with:
      python3 tools/objcmp.py <park path> <reference .s> [--func NAME]

That line is the contract this tool reads.  A park without one is reported as
UNCHECKABLE rather than silently skipped -- an unverifiable claim is a defect too.

HOW IT READS THE CLAIM.  The first "N ... of M" in the header, where M matches the
reference's own encoding count.  Size-only and prose-only parks report NO CLAIM.

RUN IT IN THE CONTAINER.  objcmp.py needs /opt/gcc296/xgcc, which exists only inside
goldensun-build.  On the host objcmp dies with FileNotFoundError, and until batch 284
this tool read that empty stdout as "objcmp produced no encoding line" and reported
UNCHECKABLE -- i.e. IT BLAMED EVERY PARK FOR A MISSING COMPILER.  An agent lost time
to that, reasonably concluding the corpus was unverifiable when the corpus was fine.
It now refuses to run at all when the compiler is absent, and reports a per-park
objcmp failure as TOOLING rather than folding it in with real park defects.

A TOOL THAT CANNOT RUN MUST SAY SO INSTEAD OF REPORTING FAILURES, because a blanket
"UNCHECKABLE: 531" is indistinguishable from a real finding and invites exactly the
wrong conclusion. The distinction matters for a live number: 485 of these parks
genuinely lack a recipe and 46 genuinely carry one, and on the host the old code
printed the same verdict for both.
"""
import os, re, subprocess, sys, glob

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GCC = os.path.join(os.environ.get("GCC296_DIR", "/opt/gcc296"), "xgcc")
# `\` line continuations are allowed before the .s AND before --func: a recipe
# split as `... REF.s \` / `--func NAME` used to lose the --func and fail on
# every multi-function .s ("holds N functions; pass --func NAME").
# Tokens may be separated by any run of whitespace and line continuations. The recipe
# is now written as the DOCKER invocation -- objcmp needs the in-container compiler --
# which wraps across three lines with a `\` after objcmp.py ITSELF. Earlier patterns
# allowed a continuation only between the two paths, so they captured the backslash as
# the candidate path and reported every such park UNCHECKABLE.
#
# GROUP 1 IS CAPTURED AND DISCARDED: check() runs objcmp against the park's OWN path, not
# against whatever the recipe names there. That is why four parks get away with the literal
# placeholder `<this>`, and why this group must stay permissive -- requiring it to look like
# a .c path broke exactly those four. It only has to not swallow a bare continuation.
VERIFY = re.compile(r"objcmp\.py[\s\\]+([^\s\\]+)[\s\\]+(\S+\.s)(?:[\s\\]+--func\s+(\S+))?", re.M)
CLAIM  = re.compile(r"(\d+)\s+(?:differing\s+)?encodings?\s+of\s+(\d+)", re.I)
CLAIM2 = re.compile(r"NON-MATCHING,\s*(\d+)\s+of\s+(\d+)", re.I)
# A third accepted phrasing, anchored the way CLAIM2 is.  Agents naturally write
# "PARKED at 20 of 76" and neither pattern above matched it, so such a park
# reported NO CLAIM and its figure went UNVERIFIED -- the checker's own
# silent-unverifiability class.  Of 15 parks matching this, 11 already matched
# CLAIM/CLAIM2 (no verdict change) and FOUR were being skipped.
CLAIM3 = re.compile(r"PARKED\s+(?:AT\s+)?(\d+)\s+of\s+(\d+)", re.I)
# A fourth phrasing, and it MUST STAY LAST IN THE CHAIN.  Parks write their
# figure as "NOT MATCHING, 4 of 19", "DOES NOT LAND -- 8 of 17", and other
# variants none of the three patterns above match, so they reported NO CLAIM and
# went unverified -- nineteen of them.
#
# MEASURED BEFORE ADDING, and the measurement is the reason for the ordering:
# of the parks this pattern matches, 19 match it ALONE (pure gain), 162 also
# match an earlier pattern and AGREE, and **11 also match an earlier pattern and
# DISAGREE** -- it is greedier and picks up a different number in those headers.
# So it is a FALLBACK ONLY.  Promote it ahead of CLAIM/CLAIM2/CLAIM3 and you
# silently change the verified figure of eleven parks.
CLAIM4 = re.compile(r"(?:NOT\s+MATCHING|DOES\s+NOT\s+LAND|NON-?MATCHING|PARKED)"
                    r"[^0-9\n]{0,40}(\d+)\s+of\s+(\d+)", re.I)
# A third accepted phrasing, anchored the same way CLAIM2 is anchored.  Agents
# naturally write "PARKED at 20 of 76" and neither pattern above matches it, so
# the park reported NO CLAIM and its figure went UNVERIFIED -- the same
# silent-unverifiability class this checker exists to catch, in the checker.
# Measured before widening: of 15 parks whose headers match this, 11 already
# matched CLAIM or CLAIM2 (no verdict change) and FOUR were being skipped.
CLAIM3 = re.compile(r"PARKED\s+(?:AT\s+)?(\d+)\s+of\s+(\d+)", re.I)


def header_of(path):
    s = open(path, errors="replace").read()
    # Skip leading blank and `//` lines before looking for the /* ... */ block.
    # A stray `// fakematch` above the header used to report "no header comment",
    # i.e. UNCHECKABLE -- the same silent-unverifiability failure as a note
    # prepended ABOVE the `Verify with:` line (batch 311, two parks).  An
    # UNCHECKABLE park's figure can never be caught lying, so this must not be
    # reachable by accident.
    lines = s.split("\n")
    i = 0
    while i < len(lines) and (not lines[i].strip() or lines[i].lstrip().startswith("//")):
        i += 1
    s = "\n".join(lines[i:])
    m = re.match(r"/\*.*?\*/", s, re.S)
    return m.group(0) if m else ""


def check(path):
    hdr = header_of(path)
    if not hdr:
        return ("UNCHECKABLE", "no header comment", None, None)
    vm = VERIFY.search(hdr.replace("*", " "))
    if not vm:
        # A TRIAGE park has no candidate, so there is no figure to check and no
        # recipe to run.  That is a legitimate state and must NOT share a verdict
        # with the dangerous one -- a park that DOES claim an "N of M" figure but
        # gives no way to re-measure it, whose number can never be caught lying.
        # Thirteen parks once hid in exactly that verdict.  Only report NOFIGURE
        # when the header both declares the absence AND claims no figure.
        flat = hdr.replace("*", " ")
        declares_none = re.search(
            r"NO\s+(?:objcmp\s+)?(?:FIGURE|CANDIDATE)|"
            r"NOT\s+RECONSTRUCTED|TRIAGE\s+ONLY|NO\s+CANDIDATE\s+WRITTEN",
            flat, re.I)
        claims = (CLAIM.search(flat) or CLAIM2.search(flat)
                  or CLAIM3.search(flat) or CLAIM4.search(flat)) or CLAIM3.search(flat)
        if declares_none and not claims:
            return ("NOFIGURE", "triage park, no candidate and no figure claimed",
                    None, None)
        # A park can be UNSCORABLE BY objcmp FOR A STRUCTURAL REASON and carry its
        # own measurement recipe instead.  The known case is a gcc NESTED-FUNCTION
        # whole-TU candidate: the nested functions are emitted as local symbols
        # (`Func_80b9554.0`), so objcmp has no mode for it, and the park measures by
        # assembling both sides and diffing normalised `objdump -d` instead.
        # That is a legitimate state and must NOT share a verdict with a park whose
        # figure simply cannot be re-measured -- UNCHECKABLE is the DANGEROUS bucket
        # and keeping it clean is the whole point of having it.
        # Widened in batch 316: a park that honestly declares itself unscorable earns
        # ALTVERIFY, but only with one of the phrasings below.  80bd424.c wrote
        # "objcmp cannot isolate a nested parent" and came back UNCHECKABLE, which
        # reads as a defect in the park rather than a property of the measurement.
        if re.search(r"objcmp(?:\.py)?\s+(?:CANNOT|CAN'T|CAN\s+NOT)\s+(?:SCORE|ISOLATE|MEASURE)"
                     r"|objcmp\.py\s+has\s+no\s+mode", flat, re.I):
            return ("ALTVERIFY",
                    "objcmp cannot score this by construction; park carries its own recipe",
                    None, None)
        return ("UNCHECKABLE", "no `Verify with: objcmp.py ...` recipe in header", None, None)
    ref, func = vm.group(2), vm.group(3)
    # A recipe wrapped in `sh -c '...'` leaves the closing quote glued to the
    # function name, and objcmp then reports "NAME' not found" -- a TOOLING
    # verdict on a recipe a human can run as written (batch 311, two parks).
    if func:
        func = func.strip("'\"")
    ref = ref.strip("'\"")
    if not os.path.exists(os.path.join(ROOT, ref)):
        return ("UNCHECKABLE", f"reference {ref} not found", None, None)
    cm = (CLAIM.search(hdr) or CLAIM2.search(hdr)
          or CLAIM3.search(hdr) or CLAIM4.search(hdr)) or CLAIM3.search(hdr)
    # A park with NO FUNCTION BODY still produces a number: objcmp compiles the
    # empty translation unit and reports every one of the reference's encodings as
    # differing.  That number is meaningless but indistinguishable from a real
    # measurement, so a claim near it would be "verified" by nothing at all.
    # Same family as a missing input printing "ENCODINGS EXACT" -- the input
    # exists here, it is just empty.  Found on a batch-312 triage park that
    # honestly claimed no figure and was measured at 2602 regardless.
    nocomment = re.sub(r"/\*.*?\*/", " ", open(path, errors="replace").read(), flags=re.S)
    nocomment = re.sub(r"//[^\n]*", " ", nocomment)
    if not re.search(r"\)\s*\{", nocomment):
        return ("NOBODY", "no function definition in the file -- nothing to measure",
                None, None)
    cmd = [sys.executable, os.path.join(ROOT, "tools", "objcmp.py"), path, ref]
    if func:
        cmd += ["--func", func]
    # HONOUR A PER-PARK FLAG DECLARED IN THE PARK'S OWN RECIPE.
    #
    # Some parks' figures are reachable only under one non-production flag, and
    # the honest ones SAY SO in the `Verify with:` line as
    # `-e OBJCMP_EXTRA=-fno-gcse`.  Measured at the tree default those parks came
    # back MISMATCH -- "a park's header is lying about its own body" -- which is
    # exactly backwards: the header was telling the truth and this checker was
    # measuring something the header never claimed.  Found on
    # OvlFunc_956_2008ba4, whose figure is 2 under -fno-gcse and 72 without it,
    # and whose park states that in capitals.
    #
    # So: if the recipe declares OBJCMP_EXTRA, measure with it, and SAY SO in the
    # verdict so the figure is never mistaken for a production-flag distance.
    # A park that needs a flag and does NOT declare it still reads MISMATCH,
    # which is the right answer -- an undeclared flag dependency is a defect.
    env = dict(os.environ)
    xm = re.search(r"OBJCMP_EXTRA=(\S+)", hdr)
    if xm:
        env["OBJCMP_EXTRA"] = xm.group(1)
    r = subprocess.run(cmd, capture_output=True, text=True, cwd=ROOT, env=env)
    out = r.stdout
    if r.returncode != 0 and "differ" not in out and " OK " not in out:
        first = (r.stderr.strip().splitlines() or ["no stderr"])[-1]
        return ("TOOLING", f"objcmp failed to run: {first}", None, None)
    if " OK " in out:
        got = (0, None)
    else:
        m = re.search(r"ENCODINGS differ in (\d+) place\(s\) \(ref (\d+)", out)
        if not m:
            return ("UNCHECKABLE", "objcmp produced no encoding line", None, None)
        got = (int(m.group(1)), int(m.group(2)))
    if not cm:
        return ("NO CLAIM", f"measures {got[0]}", None, got[0])
    claimed = int(cm.group(1))
    if claimed == got[0]:
        note = f"{claimed}"
        if xm:
            note += f"  [under {xm.group(1)} -- NOT a production-flag figure]"
        return ("OK", note, claimed, got[0])
    return ("MISMATCH", f"header claims {claimed}, body measures {got[0]}", claimed, got[0])


def main():
    if not os.path.exists(GCC):
        sys.stderr.write(
            f"parkcheck: {GCC} not found -- objcmp cannot compile anything here.\n"
            "This tool must run INSIDE the build container, or every park would be\n"
            "reported UNCHECKABLE for a reason that has nothing to do with the parks:\n\n"
            '  docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \\\n'
            "      goldensun-build python3 tools/parkcheck.py [path ...]\n")
        return 2
    paths = sys.argv[1:] or sorted(glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"), recursive=True))
    paths = [os.path.relpath(p, ROOT) for p in paths]
    tally = {}
    bad = []
    for p in paths:
        status, detail, _, _ = check(p)
        tally[status] = tally.get(status, 0) + 1
        if status in ("MISMATCH", "UNCHECKABLE", "TOOLING"):
            bad.append((status, p, detail))
        print(f"  {status:<12} {p}  {detail}")
    print()
    for k in sorted(tally):
        print(f"{k}: {tally[k]}")
    if any(s == "MISMATCH" for s, _, _ in bad):
        print("\nMISMATCHES -- a park's header is lying about its own body:")
        for s, p, d in bad:
            if s == "MISMATCH":
                print(f"  {p}: {d}")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
