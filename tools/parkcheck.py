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
# WIDENED, batch 336.  This wanted `differing` to sit DIRECTLY against
# `encodings`, so the house style this project now asks briefs to write --
# "10 differing STREAM encodings of 39", the current figure first and in
# stream terms -- was INVISIBLE to it.  On rom_a1000/80ae99c.c it skipped the
# leading 10 and matched an older "18 differing encodings of 39" further down,
# then reported MISMATCH against a figure the park was not claiming.  The
# checker was reading history as the claim.  tools/piece.py had the identical
# defect and is fixed the same way; allow a few intervening words.
CLAIM  = re.compile(
    r"(\d+)\s+(?:differing\s+)?(?:[A-Za-z]+\s+){0,3}encodings?\s+of\s+(\d+)", re.I)
# THE MIRROR WORD ORDER, added batch 326.  CLAIM above wants
# "4 differing encodings of 103"; agents write "4 of 103 encodings" and
# "PARK HELD AT 4 of 103 encodings" just as naturally, and three of brief E's
# headers came back NO CLAIM with correct measurements sitting in them.  That is
# the same defect as the batch-324 backfill, where six parks read NO CLAIM for
# the same reason -- a claim nothing can parse cannot be caught lying.
#
# It is SPECIFIC, not a fallback: the word "encodings" must FOLLOW the pair, so
# it cannot swallow a bare "4 of 103" appearing in prose about something else.
# That is what keeps it safe to rank alongside CLAIM rather than after CLAIM4.
CLAIM5 = re.compile(r"(\d+)\s+of\s+(\d+)\s+(?:differing\s+)?encodings?", re.I)
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
    # A RECIPE CAN SIT IN THE SECOND COMMENT BLOCK.  header_of used to return
    # only the first /* ... */, so a brief that prepends a SEPARATE block -- a
    # correction, a re-measurement note -- strands the `Verify with:` recipe in
    # block 2 and every such park reports UNCHECKABLE with a correct figure
    # sitting in it.  FOUR parks hit this in batch 328 alone, and it is a
    # recurrence of batch 311's prepended-note failure, which was fixed only for
    # leading `//` lines and blank lines.
    #
    # So: take the leading RUN of comment blocks, not just the first.  A park's
    # header is everything before its first declaration, and that is what a
    # reader sees as the header too.  Stop at the first non-comment,
    # non-whitespace line so a recipe quoted inside the BODY cannot be mistaken
    # for the park's own.
    blocks, rest = [], s
    while True:
        mm = re.match(r"\s*(/\*.*?\*/)", rest, re.S)
        if not mm:
            break
        blocks.append(mm.group(1))
        rest = rest[mm.end():]
    if not blocks:
        return ""
    m = type("M", (), {"group": lambda self, _=0: "\n".join(blocks)})()
    # A HEADER CLOSED INSIDE ITS OWN PROSE.  Found in batch 324 on StartRain.c,
    # whose header contained the text `int/void*` immediately followed by
    # `/undeclared`; the star-slash pair inside that closed the comment, the rest
    # of the prose became code, and the body would not compile.  The park was
    # therefore UNMEASURABLE BY EVERY TOOL -- it carried no figure, frontier.py
    # could not rank it, and parkcheck reported "no recipe" because the recipe was
    # below the accidental terminator.  It measured 4 of 104 with relocations
    # identical once fixed: a near-landing invisible for want of two characters.
    #
    # The signal is exact and was validated against all 780 parks with ZERO false
    # positives: a LEGITIMATE terminator has nothing after it on its line, because
    # anything there would have to be valid C.  Prose after it means the comment
    # ended early.
    #
    # Reported as its own verdict, not as UNCHECKABLE: the park is not
    # unverifiable, it is BROKEN, and the repair is mechanical.
    j = s.find("*/")
    eol = s.find("\n", j)
    tail = s[j + 2:eol if eol > 0 else len(s)].strip()
    if tail:
        return "\x00HEADERCUT\x00" + tail[:60]
    return m.group(0)


def check(path):
    hdr = header_of(path)
    if hdr.startswith("\x00HEADERCUT\x00"):
        return ("HEADERCUT",
                "header comment closed INSIDE its prose, so the body does not "
                "compile and no tool can measure this park -- prose after the "
                f"terminator: {hdr.split(chr(0))[2]!r}", None, None)
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
        claims = (CLAIM.search(flat) or CLAIM5.search(flat) or CLAIM2.search(flat)
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
    cm = (CLAIM.search(hdr) or CLAIM5.search(hdr) or CLAIM2.search(hdr)
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
    # SCOPE: THE RECIPE'S OWN COMMAND, NOT THE WHOLE HEADER.
    #
    # The paragraph above says "if the RECIPE declares OBJCMP_EXTRA" and the code
    # used to search `hdr` -- the entire header. So a park DISCUSSING the flag in
    # prose was measured under it. Batch 330 found three parks in rom_77000 that
    # each record `OBJCMP_EXTRA=-fno-schedule-insns2` as a REFUTED route, with
    # the refuted numbers written out beside it; parkcheck harvested the flag and
    # reported two of them MISMATCH at exactly those refuted numbers, and the
    # third TOOLING because prose had glued a trailing colon to the token. The
    # accusation MISMATCH makes -- "a park's header is lying about its own body"
    # -- was false in all three cases. The headers were right.
    #
    # A real declaration is shell: `OBJCMP_EXTRA=-flag python3 tools/objcmp.py
    # ...`, so it sits just before `objcmp.py` in the SAME command. Prose does
    # not. Scope the search to that span, and require the capture to look like a
    # flag after trailing punctuation is stripped.
    env = dict(os.environ)
    span = hdr[max(0, vm.start() - 400):vm.start()]
    xm = re.search(r"OBJCMP_EXTRA=(\S+)", span)
    extra_note = ""
    if xm:
        tok = xm.group(1).rstrip(":,.;)\"'")
        if re.fullmatch(r"-[A-Za-z0-9][-A-Za-z0-9=_+.]*", tok):
            env["OBJCMP_EXTRA"] = tok
        else:
            extra_note = f" [ignored a malformed OBJCMP_EXTRA token {xm.group(1)!r}]"
    elif "OBJCMP_EXTRA=" in hdr:
        # Mentioned somewhere in the header but not in the recipe's command. Do
        # NOT apply it -- an undeclared flag dependency must still read MISMATCH,
        # which is the right answer -- but say so, because a flag the park really
        # needs and misplaced would otherwise look like an ordinary mismatch.
        extra_note = " [header mentions OBJCMP_EXTRA outside the recipe; NOT applied]"
    r = subprocess.run(cmd, capture_output=True, text=True, cwd=ROOT, env=env)
    out = r.stdout
    if r.returncode != 0 and "differ" not in out and " OK " not in out:
        first = (r.stderr.strip().splitlines() or ["no stderr"])[-1]
        return ("TOOLING", f"objcmp failed to run: {first}", None, None)
    # STREAM INFLATION.  A park's figure is a POSITIONAL count over the encoding
    # STREAM, which includes one entry per pool word and one per 2-byte pad.  So
    # when the two sides' instruction counts differ, the streams are compared out
    # of phase and the figure counts the phase shift, not wrong instructions.
    #
    # This is NOT only the 18 parks whose claim exceeds their own length.  Batch
    # 331 brief A found OvlFunc_932_20086a0 claiming a perfectly coherent "10 of
    # 27" whose real content was TWO differing instructions out of 24 -- the rest
    # was phase.  A park can therefore look four or five times further away than
    # it is, which is exactly the kind of park nobody picks up.
    #
    # `insn` below is objcmp's own INSTRUCTION COUNT line (pool and pads
    # EXCLUDED).  When it is present the two sides' lengths disagree, so the
    # verdict says INFLATED and names both numbers.  parkcheck still verifies the
    # park's own claim against the stream figure -- that contract is unchanged.
    im = re.search(r"INSTRUCTION COUNT\s+ref (\d+), ours (\d+)", out)
    insn = (int(im.group(1)), int(im.group(2))) if im else None
    sm = re.search(r"ENCODINGS differ in \d+ place\(s\) \(ref (\d+), ours (\d+)\)", out)
    ref_stream_equal = bool(sm) and sm.group(1) == sm.group(2)
    if " OK " in out:
        got = (0, None)
    else:
        m = re.search(r"ENCODINGS differ in (\d+) place\(s\) \(ref (\d+)", out)
        if not m:
            return ("UNCHECKABLE", "objcmp produced no encoding line", None, None)
        got = (int(m.group(1)), int(m.group(2)))
    # DISTINGUISH THE TWO CASES, because they are not the same defect.
    #
    # `insn` is the 16-BIT instruction count; `got[1]` is the STREAM length,
    # which includes 32-bit entries (pool words AND `bl` calls) and pads.  The
    # first draft of this flag fired on an instruction-count difference alone
    # and called every such park PHASE.  Batch 334 brief C showed that is wrong
    # for Func_8093af8: its 16-bit counts are 116/115 but its STREAM lengths are
    # 124/124, a 32-bit entry absorbing the difference -- and its residue
    # decomposes into real causes.  So:
    #
    #   stream lengths DIFFER  -> the comparison really is out of phase
    #   stream lengths AGREE   -> SOMETHING absorbs the difference, but it does
    #                             NOT follow that positions line up
    #
    # CORRECTED, batch 343 brief C, on the SAME function this note was written
    # for.  Two things about Func_8093af8 were wrong here: the absorber is a PAD
    # HALFWORD, not a 32-bit entry (those are 8 against 8), and EQUAL STREAM
    # LENGTH WITH UNEQUAL INSTRUCTION COUNTS DOES NOT LINE POSITIONS UP.  A
    # trailing pad cannot compensate positionally for an instruction missing
    # MID-STREAM: aligncmp locates the missing instruction at ref 97-103 and the
    # pad at our index 117, so everything between them is shifted, and the real
    # residue is 110 aligned-equal against 22 in seven hunks.
    #
    # So the flag reports what it measured and names the alignment tool rather
    # than asserting a conclusion either way.
    if insn:
        if ref_stream_equal:
            extra_note += ("  [16-bit instruction counts differ (ref %d, ours %d) but"
                           " STREAM LENGTHS AGREE -- a 32-bit entry OR A PAD absorbs"
                           " it.  THIS DOES NOT MEAN POSITIONS LINE UP: a trailing pad"
                           " cannot compensate for an instruction missing mid-stream."
                           " Run aligncmp to locate both ends before reading this"
                           " figure as a distance]" % insn)
        else:
            extra_note += ("  *** INFLATED: instruction counts differ (ref %d, ours %d)"
                           " AND stream lengths differ, so this positional figure"
                           " counts PHASE, not distance" % insn)
        # AND CROSS-CHECK THE HEADER'S OWN PROSE AGAINST WHAT WAS JUST MEASURED.
        # objcmp has printed a note about this for some time; what it cannot do is
        # notice that the PARK SAYS THE OPPOSITE.  Batch 346 found three of brief
        # E's four targets and one of brief A's asserting "this figure IS a true
        # distance" on the grounds that "N == N encodings and no SIZE line" --
        # reading the STREAM figure as an instruction count while
        # `XX INSTRUCTION COUNT ref N, ours N+1` sat on the line directly above.
        #
        # Swept tree-wide when this was added: 256 parks assert a distance in
        # prose and 100 OF THEM ARE OUT OF PHASE.  That is a class, not a few
        # slips, and it matters because the claim is load-bearing -- it is the
        # sentence that tells the next agent the figure can be trusted, and every
        # targeting instrument here ranks on that figure.
        # MATCH THE ASSERTION, NOT ITS NEGATION.  The first version of this check
        # fired on `NOT A TRUE DISTANCE` -- a header that is CORRECT and says so.
        # Batch 347 brief A was handed such a park as a target and found its line
        # 6 had read "NOT A TRUE DISTANCE", correctly, since batch 335.  The
        # same bug inflated the coordinator's tree-wide count: 256 files matched
        # the bare phrase, 224 survived a naive negation filter, and only 180 are
        # claims about THIS park's figure at all -- the rest were prose about
        # other parks ("312 brief C spent its budget reaching a real distance on
        # its other two").  The honest out-of-phase count is 53, not the 100
        # first reported.
        #
        # So: require the claim to be affirmative AND to be about this park's own
        # figure, and look back far enough to span a comment asterisk.
        flat_hdr = re.sub(r"[\s*]+", " ", hdr)
        claimed_distance = False
        for dm in re.finditer(r"(?:true|real|plain|genuine)\s+distance|"
                              r"IS\s+a\s+distance|a\s+distance\s+(?:now|at\s+last)",
                              flat_hdr, re.I):
            before = flat_hdr[max(0, dm.start() - 70):dm.start()]
            if re.search(r"(?:\bnot\b|\bnever\b|\bno longer\b|\bisn\b|\bwas\b|"
                         r"\bstops?\b|\bcannot\b|\bfails?\b|\buntil\b|"
                         r"\breach(?:ing)?\b|\bspent\b)[^.;!]{0,60}$",
                         before, re.I):
                continue
            claimed_distance = True
            break
        if claimed_distance:
            extra_note += ("  !! CHECK: the header contains an AFFIRMATIVE"
                           " distance claim while the instruction counts above"
                           " disagree.  THIS IS A FLAG, NOT A VERDICT -- two"
                           " legitimate shapes trip it: prose QUOTING the phrase"
                           " (a header discussing a brief's wording), and a claim"
                           " about a DIFFERENT body (a flag variant or an"
                           " instrument-only state reached at equal length). If"
                           " the claim really is about this park's own figure,"
                           " fix the header before anyone targets on it")
    if not cm:
        return ("NO CLAIM", f"measures {got[0]}{extra_note}", None, got[0])
    claimed = int(cm.group(1))
    if claimed == got[0]:
        note = f"{claimed}"
        if "OBJCMP_EXTRA" in env and env.get("OBJCMP_EXTRA"):
            note += f"  [under {env['OBJCMP_EXTRA']} -- NOT a production-flag figure]"
        return ("OK", note + extra_note, claimed, got[0])
    return ("MISMATCH",
            f"header claims {claimed}, body measures {got[0]}{extra_note}",
            claimed, got[0])


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
