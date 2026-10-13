"""autosweep.py -- the JUDGMENT-FREE half of elevating a park, run from bash.

    python3 tools/autosweep.py src/non_matching/rom_b5000/80b6d30.c
    python3 tools/autosweep.py <park.c> --flags      # per-file flag groups only
    python3 tools/autosweep.py <park.c> --anchor     # anchor placements only

WHAT THIS IS FOR.  Most of elevating a park needs judgment: reading the
disassembly, forming a hypothesis, reading `.00.rtl` or `.18.greg` to name the
pass.  None of that is scriptable.  Two axes ARE mechanical and need no
hypothesis, so this tool runs them for free:

  * THE FLAG AXIS.  Ten per-file flag groups exist in the Makefile.  Trying the
    UNCHANGED body under each is pure measurement -- no source edit, nothing to
    decide.
  * THE ANCHOR AXIS.  Inserting `do { } while (0);` at each top-level statement
    boundary is text manipulation.  Func_801fd34 landed on one such placement
    after a hand sweep of 82; Field_Move and Func_80ab21c landed the same shape
    with a barrier, and the anchor is strictly preferable (same reach, emits
    nothing, not counted by shimcount -- see docs/elevation.md).

## MEASURED HIT RATE, AND IT IS LOW -- READ THIS BEFORE EXPECTING LANDINGS

This file's first version claimed the flag axis "produced a landing in batch
337", citing OvlFunc_common1_588's -O1 match.  **That was wrong, and the batch's
own report says so**: the -O1 match needed the flag CROSSED WITH A SOURCE CHANGE
(two locals instead of the park's reused one), and
reports/batch-337.md records the inversion -- "the parked body is the only one of
ten in that corpus group that does not improve at -O1, so a flag sweep run on
the park alone finds nothing."  A blind flag sweep would NOT have found it.

Measured since, on parks no agent had swept:

    FLAG axis    2 LANDINGS and 3 improvements in 10 parks  (batches 340-342)
    ANCHOR axis  1 landing and several improvements in ~20 parks

THE FLAG AXIS RATE WAS BADLY UNDERSTATED, and the reason matters for sequencing.
The original "0 in 6" came from six parks that had ALREADY BEEN HAND-SWEPT by
earlier briefs, so the cheap answers were gone before this tool ran.  Batch 342
brief A drew four parks that were batch-319 backfills NOBODY had ever swept and
THREE of the four improved on the flag axis -- 23 -> 11 on -fno-gcse (which then
landed), 25 -> 17 on -O1, 23 -> 19 on -ffixed-r7.  Batch 342 brief D's whole
landing WAS a flag: OvlFunc_924_2009db4 is byte-identical under
-fno-rerun-loop-opt with the park's body unchanged, and its park had recorded
"NO FLAG GROUP HELPS" -- a bound stated from six of the eight groups.

So: on a park nobody has swept, run this FIRST.  On one that has been swept by
hand, expect little.

THE ANCHOR AXIS HAS NOW PAID ONCE.  Brief A's Func_80a7440 came to 0 on
`do { } while (0);` after `s = iwram_3001f2c;`, found by this tool while the
agent was mid-derivation of the same answer -- and the park it refuted had
arithmetic that was entirely CORRECT: both chains at 3+P, the class rung at
haifa-sched.c:4069-4094 reproduced.  Its verdict ("nothing semantics-preserving
reaches it") was wrong anyway, because you do not win that tie, you empty the
ready list.  So the sequencing advice is now: RUN THIS AND READ THE TABLE BEFORE
forming a hypothesis, not after.

Placement is predictable for the anchor, which makes a full sweep slightly
wasteful but still cheap: the notes hang on the FOLLOWING insn
(haifa-sched.c:3735-3736), so an anchor fences immediately before the next insn.
See docs/elevation.md, "THE ANCHOR IS NOT A STRONGER BARRIER".

Do not expect a landing per park.  **The rest of this tool's value is the
recorded NEGATIVE**, which is worth real tokens for a different reason: an agent that
knows both cheap axes are closed does not spend part of its round re-screening
them, and every brief so far has burned some of its round on exactly that.  The
secondary value is as a CROSSING PARTNER -- a flag that is inert alone may not be
inert against a changed body, which is how the -O1 landing actually happened, and
knowing which flags are inert on the parked body tells an agent which ones are
worth re-crossing after it edits the source.

So this is a BEFORE-THE-AGENT tool, not an instead-of-the-agent tool.

WHAT IT CANNOT DO, stated plainly so nobody mistakes a clean sweep for a verdict:
it does not read dumps, cannot invent a spelling, cannot write a body for a
prose park, and cannot correct a park's diagnosis.  A park that survives this
sweep has had its two cheapest axes closed -- which is genuinely useful to a
brief, as a recorded negative -- and nothing more.

The anchor variants are device-free.  `--barrier` also exists for completeness
but a bare `__asm__ volatile ("")` is fakematch-class and carries pass-3 debt,
so prefer the anchor and reach for the barrier only to confirm a mechanism.
"""
import argparse
import glob
import os
import re
import shutil
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
# SPLIT AT THE IMAGE NAME DELIBERATELY.  Everything before it is a docker flag;
# everything after is the container's command.  The first version of this file
# appended `-e OBJCMP_EXTRA=...` to the full list, i.e. AFTER the image name, so
# docker treated `-e` as the command to run and the flag never reached objcmp --
# and the sweep reported all eight flag groups as "no verdict", which reads
# exactly like a clean negative.  Verified by hand against a direct objcmp run
# that DID report 20 of 76 with -fno-gcse.
DOCKER_PRE = ["docker", "run", "--rm", "--security-opt", "seccomp=unconfined",
              "-v", f"{ROOT}:/work", "-w", "/work"]
IMAGE = "goldensun-build"
DOCKER = DOCKER_PRE + [IMAGE]

# The distinguishing flag of each per-file group in the Makefile.  Kept as data
# rather than parsed, because the groups are named by their effect and a regex
# over the Makefile would pick up the prose discussing them -- the
# prose-as-machinery trap this tree keeps hitting.
FLAGS = [
    ("-O1",                       "O1_CFLAGS"),
    ("-fno-gcse",                 "GCSE_CFLAGS"),
    ("-fno-schedule-insns2",      "SCHED2_CFLAGS"),
    ("-ffixed-r7",                "FIXEDR7_CFLAGS"),
    ("-fno-strict-aliasing",      "ALIAS_CFLAGS"),
    ("-fno-strength-reduce",      "STRENGTH_CFLAGS"),
    ("-fno-rerun-cse-after-loop", "CSE_CFLAGS"),
    ("-fno-rerun-loop-opt",       "RERUNLOOP_CFLAGS"),
]

# BOTH WORD ORDERS.  Parks write "13 differing encodings of 55" and "13 of 55
# encodings" about equally, and this file carried only the first -- so it read
# FOUR OF FIVE of batch 338 brief D's targets as having no figure at all.
# tools/parkcheck.py already solved this with a chain of five patterns; the two
# that matter here are its CLAIM and CLAIM5.
CLAIM = re.compile(
    r"(\d+)\s+(?:differing\s+)?(?:[A-Za-z]+\s+){0,3}encodings?\s+of\s+(\d+)", re.I)
CLAIM_MIRROR = re.compile(
    r"(\d+)\s+of\s+(\d+)\s+(?:[A-Za-z]+\s+){0,3}encodings?", re.I)
DEF = re.compile(r"^[A-Za-z_][A-Za-z0-9_ \t\*]*?\b(\w+)\s*\([^;{]*?\)\s*\{", re.M | re.S)
SUBJ = re.compile(r"\s*/\*+\s*((?:Ovl)?Func_\w+|[A-Z][A-Za-z0-9_]*)")


def resolve(park):
    """-> (reference .s, function name).  Same rules as backfill.py / piece.py."""
    t = open(os.path.join(ROOT, park), errors="replace").read()
    ref = None
    for m in re.finditer(r"(asm/[A-Za-z0-9_/]+\.s)", t):
        if os.path.exists(os.path.join(ROOT, m.group(1))):
            ref = m.group(1)
            break
    code = re.sub(r"/\*.*?\*/", " ", t, flags=re.S)
    code = re.sub(r"//[^\n]*", " ", code)
    # THE SUBJECT COMES FIRST, NOT THE FIRST DEFINITION.  This used to take
    # DEF.search(code), i.e. whichever function the file happens to define
    # earliest -- so a park carrying `static inline` helpers above its subject
    # swept the WRONG FUNCTION.  Batch 340 brief E hit it on AnimEnd, which
    # swept as SetRegAnimDest and reported "BASE DOES NOT MEASURE": a
    # clean-looking "too far gone" verdict on a target where the anchor axis was
    # worth 12 encodings.  Fourth false-clean-negative in this file.
    #
    # Prefer the header subject, validated against the reference's own
    # .thumb_func_start list, and fall back to a definition only if that fails.
    names = []
    if ref:
        body = open(os.path.join(ROOT, ref), errors="replace").read()
        names = re.findall(r"^\s*\.(?:thumb|arm)_func_start(?:_noalign)?\s+(\S+)",
                           body, re.M)
    sm = SUBJ.match(t)
    fn = None
    if sm and (not names or sm.group(1) in names):
        fn = sm.group(1)
    if fn is None:
        for m in DEF.finditer(code):
            if not names or m.group(1) in names:
                fn = m.group(1)
                break
    return ref, fn


def aligned_residue(path, ref, fn, extra=""):
    """aligncmp's position-tolerant residue -- the ONLY number comparable across a
    length change.

    objcmp's ndiff compares encodings index by index with no alignment, so one
    missing or extra instruction poisons every later index and the count
    saturates.  aligncmp runs an LCS over the two encoding streams, so an
    inserted or deleted instruction no longer does that.

    This exists because batch 345 brief A found that autosweep's phase guard,
    while it correctly FIRES on a length-changing row, still prints that row's
    lower ndiff where a reader takes it for progress.  Measured on `GiveItemTo`:
    -fno-rerun-cse-after-loop reads 49 against a base of 58 and takes the ALIGNED
    residue 42 -> 44, i.e. it is strictly worse.  A verdict needs the aligned
    number, not a caveat next to a misleading one.

    aligncmp honours OBJCMP_EXTRA because it calls objcmp.cflags_for, which
    applies it -- so a flag variant is measurable on this scale.
    """
    cmd = DOCKER_PRE[:]
    if extra:
        cmd += ["-e", f"OBJCMP_EXTRA={extra}"]
    cmd += [IMAGE, "python3", "tools/aligncmp.py", path, ref, fn]
    out = subprocess.run(cmd, capture_output=True, text=True).stdout
    m = re.search(r"differing/ins/del (\d+) in (\d+) hunks", out)
    return (int(m.group(1)), int(m.group(2))) if m else None


def measure(path, ref, fn, extra=""):
    """One objcmp run.  -> (ndiff, total, dinsn, first) or None if it got no verdict.

    `dinsn` is ours-minus-ref 16-bit INSTRUCTION COUNT and `first` is the index
    of the first differing encoding.  Both are returned because NDIFF ALONE IS
    NOT COMPARABLE ACROSS VARIANTS: it is a POSITIONAL count, so a variant that
    changes the instruction count is measured out of phase and its ndiff is
    partly counting offset.  Batch 344 brief E found THREE OF ITS FOUR targets
    carried a FALSE "BETTER" flag row for exactly this reason -- two rows reading
    37 against a base of 49 both RAISED the real (LCS) residue from 9 to 12.
    """
    cmd = DOCKER_PRE[:]
    if extra:
        cmd += ["-e", f"OBJCMP_EXTRA={extra}"]       # BEFORE the image name
    cmd += [IMAGE, "python3", "tools/objcmp.py", path, ref, "--func", fn]
    out = subprocess.run(cmd, capture_output=True, text=True).stdout
    if re.search(r"^\s+OK\b", out, re.M):
        return (0, 0, 0, -1)
    m = re.search(r"differ in (\d+) place\(s\) \(ref (\d+)", out)
    if not m:
        return None
    fi = re.search(r"first at index (\d+)", out)
    first = int(fi.group(1)) if fi else -1
    im = re.search(r"instructions are (\d+)/(\d+)", out)
    dinsn = (int(im.group(2)) - int(im.group(1))) if im else 0
    # RELOCATIONS ARE ON THE SAME OUTPUT and a figure alone hides them: brief D's
    # 20087b0 read "14 of 71" AND "RELOCATIONS differ", two pool words transposed,
    # so its 14 was never a plain distance.  Asking for both instruction counts
    # catches the phase trap; it does not catch this one.
    if re.search(r"RELOCATIONS differ", out):
        # NOT NECESSARILY A DEFECT.  Batch 340 brief D found three of four such
        # banners false: _CONST_2 (const.sym:53) and _AREA_36 (area.sym:71) are
        # ABSOLUTE symbols in .sym files that stage1.ld INCLUDEs, so an
        # R_ARM_ABS32 against them links to the literal value and the ROM's
        # bare constant is reproduced exactly.  Several already-landed,
        # byte-identical files spell the same pooled constants the same way.
        # So this is a flag to CHECK, never a verdict -- and the check is
        # whether the symbol is defined by value in a .sym file.
        globals().setdefault("_RELOC_WARN", set()).add(path)
    return (int(m.group(1)), int(m.group(2)), dinsn, first)


def body_span(text, fn):
    """Character span of fn's body, braces balanced.  -> (open_idx, close_idx)."""
    m = re.search(r"^[A-Za-z_][A-Za-z0-9_ \t\*]*?\b" + re.escape(fn) +
                  r"\s*\([^;{]*?\)\s*\{", text, re.M | re.S)
    if not m:
        return None
    i = text.index("{", m.start())
    depth = 0
    for j in range(i, len(text)):
        if text[j] == "{":
            depth += 1
        elif text[j] == "}":
            depth -= 1
            if depth == 0:
                return (i, j)
    return None


def _skip_blanks(text, p, window=400):
    """The next real token after `p`, skipping whitespace AND comments.

    Every lookahead test in boundaries() needs this: a declaration in these park
    bodies is routinely annotated (`vec3_t posA;   /* sp+0xac */`), so the raw
    text after a `;` is a comment and a test for `int` never sees the `int`.
    """
    s = text[p:p + window]
    while True:
        t = s.lstrip()
        if t.startswith("/*"):
            e = t.find("*/")
            if e < 0:
                return ""
            s = t[e + 2:]
        elif t.startswith("//"):
            e = t.find("\n")
            if e < 0:
                return ""
            s = t[e + 1:]
        else:
            return t


_DECL_MACROS = {}


def decl_macros(text):
    """Names `#define`d in this file whose expansion DECLARES something.

    A pin macro (`#define R0 register int q0 __asm__("r0")`) is a declaration
    wearing a statement's clothes, so an anchor placed in front of one lands in
    front of a declaration and C89 rejects it.  Matched by SHAPE, not by the name
    `PIN\\d` -- the same correction `shimcount.py` needed, where macros called
    R0..R3 made it report 4 pins where 96 existed.

    Continuation lines are joined first, since these macros are routinely written
    across several backslashed lines.
    """
    key = id(text)
    if key in _DECL_MACROS:
        return _DECL_MACROS[key]
    joined = re.sub(r"\\\n", " ", text)
    out = set()
    for m in re.finditer(r"^\s*#\s*define\s+(\w+)(?:\([^)]*\))?\s+(.+)$",
                         joined, re.M):
        if re.search(r"\b(?:register|const|static|volatile|unsigned|signed|"
                     r"struct|union|enum|void|char|short|int|long|float|double)\b",
                     m.group(2)):
            out.add(m.group(1))
    _DECL_MACROS[key] = out
    return out


def boundaries(text, i, j):
    """EVERY statement boundary inside the body, at any nesting depth.

    Offsets just after a `;`, plus just after each `{` that opens a nested block,
    so an anchor can be placed as the first statement of an `if` or loop body.
    Declarations are included -- placing an anchor among them is exactly how
    Field_Move's 0 was found (after a load, not before).

    A `;` INSIDE PARENTHESES IS NOT A STATEMENT BOUNDARY.  The two semicolons of
    a `for (init; cond; step)` header sit inside the parens, and inserting there
    produces `for (;do { } while (0);  i != 11; i++)`, a parse error.  So EVERY
    `for` LOOP IN THE BODY CONTRIBUTED TWO VARIANTS THAT COULD NOT COMPILE --
    which is why batch 345 brief B saw exactly 4 "produced NO ROW" on all four of
    its targets, each having two `for` loops.  Measured over the clean 45-park
    sweep: 9 parks affected, 20 dead variants.

    The tool was RIGHT to report that slice as untested rather than inert, and
    that report is what found this.  But those variants were never going to
    measure, so the honest count of a swept axis was always short by twice the
    `for`-loop count.

    THIS USED TO BE `depth == 1` ONLY, which is a false-clean-negative bug and
    the third in this file.  Statements inside any `if` or loop were never
    anchored, so a park whose residue lives in a nested block got the full
    "Both cheap axes are CLOSED" verdict when half of one axis had never been
    tried.  Batch 338 brief D found it the expensive way: THREE of its five
    targets were in that state, and BOTH of its anchor-axis wins came from
    placements this function could not generate -- OvlFunc_971_20087b0's residue
    is inside `if (v < 0)` (14 -> 4) and Func_8096ddc's is inside its `for` body
    (11 -> 10).
    """
    out, depth, paren = [], 0, 0
    for k in range(i, j):
        c = text[k]
        if c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
        elif c == "(":
            paren += 1
        elif c == ")":
            paren -= 1
        elif c == ";" and depth >= 1 and paren == 0:
            # SKIP A BOUNDARY INSIDE THE DECLARATION PROLOGUE.  gcc-2.96 is C89,
            # so a statement among declarations does not compile -- which is why
            # 3 of 8 variants on one park produced no row at all.  If the next
            # thing after this `;` opens with a type keyword, an anchor here
            # would illegally precede a declaration.
            # LOOK PAST COMMENTS, not just whitespace.  These park bodies
            # annotate their declarations with the stack slot --
            #
            #     vec3_t posA;                 /* sp+0xac */
            #     vec3_t vin;                  /* sp+0xa0 */
            #
            # so the text right after the `;` is a COMMENT and every test below
            # saw `/* sp+0xac */` instead of the declaration that follows it.
            # That is why `parse error before 'int'` survived a guard whose whole
            # job is to match `int`.  Twenty-nine dead variants across four parks,
            # found only by compiling every variant rather than trusting the
            # guard.  The window is wide because a comment eats most of it.
            nxt = _skip_blanks(text, k + 1)
            if re.match(r"(?:const|static|volatile|register|unsigned|signed|"
                        r"struct|union|enum|void|char|short|int|long|float|"
                        r"double|u8|u16|u32|s8|s16|s32|vu8|vu16|vu32|vs32)\b",
                        nxt):
                continue
            # A DECLARATION USING A TYPEDEF IS STILL A DECLARATION, and the
            # keyword list above cannot see one.  This is the THIRD cause of the
            # dead-variant slice: on Anim_CriticalHit all 12 failures were
            # `vec3_t vb;`, `DrawFn *fp;`, `CopyFn cp;`, `ClearFn cl;` -- local
            # typedefs, so the keyword test passed them through and the anchor
            # landed among declarations, which C89 rejects.
            #
            # Test STRUCTURALLY instead of by vocabulary: two space-separated
            # identifiers before a `;`, `,` or `=` is a declaration, whatever the
            # first one is called.  `a = 1;` has only one identifier before the
            # `=` and `foo(x);` has none before the `;`, so neither matches.
            #
            # A false positive here only DROPS a boundary, and the tool reports
            # the boundary count it actually swept -- so the failure mode is a
            # smaller honest axis, not a silent one.
            if re.match(r"[A-Za-z_]\w*(?:\s+[A-Za-z_]\w*)*\s+\**[A-Za-z_]\w*"
                        r"\s*(?:\[[^\];]*\])?\s*[;,=]", nxt):
                continue
            # A PIN MACRO EXPANDS TO A DECLARATION, so a boundary in front of one
            # is a boundary in front of a declaration.  FOURTH and last cause of
            # the dead-variant slice: a pinned park's body reads
            #
            #     { R0; R1; R2; q0 = A; ... }
            #
            # and `R0` is `#define R0 register int q0 __asm__("r0")`.  Neither the
            # keyword list nor the two-identifier test can see that, because at
            # this point in the text `R0` is just a name followed by `;`.
            #
            # So read the macros out of THIS FILE and skip the ones whose own
            # expansion declares something.  Matching by SHAPE rather than by the
            # name `PIN\d` is the same correction shimcount.py needed, where
            # macros called R0..R3 reported 4 pins against 96 real ones.
            if any(re.match(re.escape(m) + r"\b", nxt) for m in decl_macros(text)):
                continue
            # SKIP A `;` THAT IS NOT A STATEMENT TERMINATOR BUT A CLAUSE
            # SEPARATOR.  Two cases, and both produce a parse error:
            #
            #   if (c) x = 1; <HERE> else ...      -> "parse error before `else'"
            #   do x = 1; while (c);               -> the `while` is the loop's
            #
            # This is the SECOND cause of the dead-variant slice.  Fixing the
            # for-header case cleared only 3 of the 9 affected parks in the
            # 45-park sweep; this clears the rest.  Measured on
            # overlays/20082f0.c: 4 failures, all four "parse error before
            # `else'", from an un-braced and a braced else arm each.
            #
            # The general rule, now stated in one place: A `;` IS A BOUNDARY ONLY
            # WHEN WHAT FOLLOWS IT CAN LEGALLY BE A NEW STATEMENT.  Three things
            # can follow one and not be that -- a declaration (C89), another
            # clause of the same construct, and a closing paren of a `for`.
            if re.match(r"(?:else|while)\b", nxt):
                continue
            out.append(k + 1)
    return out

# NOTE ON WHAT IS NOT INSERTED.  An earlier version also inserted just after each
# nested `{`, to anchor as the first statement of an `if` or loop body.  That can
# produce invalid C -- a brace opening an initializer or an aggregate is
# indistinguishable from one opening a block by text alone -- and on one park 6
# variants yielded only 3 rows, silently.  Dropping it loses nothing: a `;` at any
# depth already lands inside every nested block, which is where brief D's two
# wins were (inside `if (v < 0)`, and inside a `for` body).


def path_in_reloc_warn(p):
    return p in globals().get("_RELOC_WARN", set())


def main():
    # THIS TOOL SHELLS OUT TO `docker` ITSELF, so it must run on the HOST.  Run
    # inside the build container it dies AFTER printing the park's claims, which
    # reads as a measurement failure rather than a usage error -- batch 341 brief
    # D and batch 342 brief E each lost time to it.
    if not shutil.which("docker"):
        print("autosweep.py must run on the HOST, not inside the build container:")
        print("  it invokes `docker run` itself, and there is no docker here.")
        print("  (If you are in the container, exit and run it from the repo root.)")
        return 2
    ap = argparse.ArgumentParser()
    ap.add_argument("park")
    ap.add_argument("--flags", action="store_true")
    ap.add_argument("--anchor", action="store_true")
    ap.add_argument("--barrier", action="store_true")
    ap.add_argument("--max", type=int, default=40,
                    help="cap on placement variants (default 40)")
    a = ap.parse_args()
    if not (a.flags or a.anchor or a.barrier):
        a.flags = a.anchor = True          # the two device-free axes

    park = os.path.relpath(os.path.abspath(a.park), ROOT)
    ref, fn = resolve(park)
    if not ref or not fn:
        print(f"cannot resolve: reference={ref} function={fn}")
        return 1
    text = open(os.path.join(ROOT, park), errors="replace").read()
    claimed = CLAIM.search(text[:9000]) or CLAIM_MIRROR.search(text[:9000])
    print(f"park      {park}")
    print(f"reference {ref}")
    print(f"function  {fn}")
    print(f"claims    {claimed.group(1) + ' of ' + claimed.group(2) if claimed else '(no figure)'}")

    base = measure(park, ref, fn)
    if base is None:
        print("\nBASE DOES NOT MEASURE -- objcmp stopped before the encoding compare.")
        print("Nothing here is meaningful until the body reaches the reference's length.")
        return 1
    print(f"base      {base[0]} of {base[1]}"
          + (f"   INSTRUCTION GAP {base[2]:+} (ours - ref) -- THIS NDIFF IS PHASE,"
             f" NOT A DISTANCE" if base[2] else "   instruction counts EQUAL"))
    if path_in_reloc_warn(park):
        print("          !! RELOCATIONS ALSO DIFFER -- CHECK, do not assume a defect.")
        print("             If the differing words are absolute symbols defined by")
        print("             value in a .sym file (const.sym, area.sym, size.sym,")
        print("             message.sym), they link to the literal and the figure IS")
        print("             a plain distance -- landed files already do this.  Only a")
        print("             genuinely unresolved symbol makes the figure unreadable.")
    print()
    best = [(base[0], "base", "")]

    if a.flags:
        print("FLAG AXIS -- unchanged body, one per-file flag group at a time")
        for flag, group in FLAGS:
            r = measure(park, ref, fn, extra=flag)
            v = f"{r[0]} of {r[1]}" if r else "NO VERDICT -- length changed, or a parse miss; CHECK BY HAND"
            # THE SAME GUARD THE ANCHOR AXIS ALREADY CARRIES, and the flag axis
            # went without it until batch 344 brief E measured the consequence:
            # three of its four targets showed a FALSE "BETTER" flag row, because
            # a lower NDIFF on a variant whose instruction count moved is phase,
            # not progress.  A lower count only means something when the length
            # is unchanged AND the first differing index moved.
            mark = ""
            # A FLAG THAT READS *EQUAL* WHILE CHANGING THE BODY STAYS INVISIBLE,
            # and batch 346 brief C caught one hiding in exactly that gap:
            # -fno-strength-reduce on Func_80bac6c reads the SAME ndiff 35 at the
            # same equal stream length 57/57, while actually producing 52
            # instructions against the base's 55 and taking the ALIGNED residue
            # 12 -> 30.  Reading lower is not the only way a row lies; reading
            # EQUAL while the instruction count moves is the other way.
            #
            # So measure the aligned residue whenever the row is not strictly
            # worse OR the instruction count moved -- not only when ndiff fell.
            if r and (r[0] <= base[0] or r[2] != base[2]) and r[0] != 0:
                ab = globals().setdefault("_ALIGN_BASE",
                                          aligned_residue(park, ref, fn))
                av = aligned_residue(park, ref, fn, extra=flag)
                if ab and av and av[0] > ab[0]:
                    mark = (f"  <-- ALIGNED RESIDUE IS WORSE ({ab[0]} -> {av[0]})"
                            + (f" despite ndiff {r[0]} vs base {base[0]}"
                               if r[0] <= base[0] else "")
                            + "; NOT progress")
                elif ab and av and av[0] < ab[0]:
                    mark = f"  <-- ALIGNED RESIDUE IMPROVES ({ab[0]} -> {av[0]})"
                if r[2] != base[2] and mark:
                    mark += f"  [instruction count moved to {r[2]:+}]"
            if r and r[0] < base[0] and not mark:
                # ANY lower row gets the ALIGNED verdict attached, because the raw
                # count is not comparable across a length change and a caveat
                # beside a misleading number still reads as progress (345 brief A).
                ab = globals().setdefault("_ALIGN_BASE", aligned_residue(park, ref, fn))
                av = aligned_residue(park, ref, fn, extra=flag)
                if ab and av and av[0] > ab[0]:
                    mark = (f"  <-- lower count but ALIGNED RESIDUE IS WORSE "
                            f"({ab[0]} -> {av[0]}); this row is NOT progress")
                elif ab and av and av[0] == ab[0]:
                    mark = (f"  <-- lower count, ALIGNED RESIDUE UNCHANGED at "
                            f"{ab[0]}; the drop is phase, not distance")
                elif r[2] != base[2]:
                    mark = (f"  <-- lower count, instruction count moved to "
                            f"{r[2]:+}"
                            + (f", ALIGNED {ab[0]} -> {av[0]}" if ab and av else
                               "; aligncmp did not measure -- CHECK BY HAND"))
                elif r[3] == base[3]:
                    mark = (f"  <-- lower count but `first` UNMOVED at {r[3]}; "
                            f"CHECK with aligncmp before believing it")
                else:
                    mark = ("  <-- BETTER"
                            + (f" (ALIGNED {ab[0]} -> {av[0]})" if ab and av else ""))
            if r and r[0] == 0:
                mark = "  <-- *** ZERO ***"
            print(f"  {flag:28} {group:20} {v}{mark}")
            if r:
                # Only a row that passes the guard may enter BEST. A phase row
                # ranking above the base is how a false lead gets propagated.
                if r[0] == 0 or (r[2] == base[2] and r[3] != base[3]
                                 and "NOT progress" not in mark
                                 and "phase, not distance" not in mark):
                    best.append((r[0], f"flag {flag}", group))
        print()

    span = body_span(text, fn)
    if (a.anchor or a.barrier) and span:
        i, j = span
        pts = boundaries(text, i, j)
        # ALSO SWEEP THE static inline HELPERS THE SUBJECT CALLS.  boundaries()
        # used to see the subject's body ONLY, so a lever living inside a helper
        # was unreachable -- and that is exactly where batch 340 brief E's
        # AnimEnd answer was (an anchor after a queue-pointer assignment INSIDE
        # the inline, 14 -> 2), and where both AnimStart parks' whole lever lives
        # (worth 5 each, batch 341 brief A).  Fifth false-clean-negative class in
        # this file: the tool could report an axis closed while never looking at
        # the code that mattered.
        #
        # Only helpers the subject actually CALLS, so the sweep does not wander
        # into unrelated statics.
        called = set(re.findall(r"\b(\w+)\s*\(", text[i:j]))
        for hm in re.finditer(r"^\s*static\s+(?:__inline__\s+|inline\s+)?[\w \t\*]*?"
                              r"\b(\w+)\s*\([^;{]*?\)\s*\{", text, re.M | re.S):
            if hm.group(1) not in called:
                continue
            hs = body_span(text, hm.group(1))
            if hs and not (hs[0] >= i and hs[1] <= j):
                extra = boundaries(text, *hs)
                if extra:
                    print(f"  (+{len(extra)} boundaries inside static inline "
                          f"{hm.group(1)}, which the subject calls)")
                    pts = sorted(pts + extra)
        for label, snippet, dev in (("ANCHOR", "\n    do { } while (0);", False),
                                    ("BARRIER", '\n    __asm__ volatile ("");', True)):
            if label == "ANCHOR" and not a.anchor:
                continue
            if label == "BARRIER" and not a.barrier:
                continue
            base_first = base[3]
            capped = len(pts) > a.max
            print(f"{label} AXIS -- {min(len(pts), a.max)} of {len(pts)} statement "
                  f"boundaries{'  (DEVICE: fakematch-class, pass-3 debt)' if dev else '  (device-free)'}")
            if capped:
                globals()["_CAPPED"] = True
                # THE CAP IS A FALSE-CLEAN-NEGATIVE GENERATOR, and the fourth in
                # this file.  --max defaults to 40, and batch 341 brief C had FOUR
                # targets exceeding it (88, 94, 124 and 179 boundaries) -- two of
                # which improved on a placement PAST boundary 40 (Func_80a112c at
                # 58, Anim_Djinni at 44).  The old line printed no denominator and
                # the verdict still said "CLOSED", so every default run on a long
                # park recorded a TRUNCATED sweep as a closed axis.
                print(f"     !! CAPPED at --max {a.max}: boundaries "
                      f"{a.max}..{len(pts) - 1} are UNTESTED, not inert.  Re-run with "
                      f"--max {len(pts)} before recording this axis as closed.")
            d = tempfile.mkdtemp(prefix="autosweep_", dir=os.path.join(ROOT, "scratch_elev"))
            try:
                # CARRY THE PARK'S SIBLING HEADERS INTO THE VARIANT DIRECTORY.
                # A variant is written here, not beside the park, so a
                # `#include "decls.h"` naming a header that lives in the PARK'S
                # OWN directory cannot be found and EVERY variant fails to
                # compile.  The axis then reports "all N produced NO ROW", which
                # is honest but means the park has no anchor axis at all.
                #
                # Seven parks are in that state -- the rom_c9000 Anim family
                # (ParticleCloud, Heal, StatUp, TitanBlade, SpecialAttack,
                # RapidSlash, Nova), all including src/non_matching/rom_c9000/decls.h.
                #
                # Copying is right rather than adding -I: the park's directory
                # also holds the OTHER parks' .c files, and putting it on the
                # include path would let a variant pick up a header by a name it
                # only resolves there, which is a different compile from the one
                # objcmp performs.
                for h in glob.glob(os.path.join(os.path.dirname(
                        os.path.join(ROOT, park)), "*.h")):
                    shutil.copy(h, d)
                names = []
                for n, p in enumerate(pts[:a.max]):
                    v = text[:p] + snippet + text[p:]
                    nm = f"{label.lower()}{n:02d}.c"
                    open(os.path.join(d, nm), "w").write(v)
                    names.append(nm)
                rel = os.path.relpath(d, ROOT)
                out = subprocess.run(
                    DOCKER + ["python3", "tools/sweep_variants.py", park, ref, fn, rel],
                    capture_output=True, text=True).stdout
                # sweep_variants.py prints `NAME  ndiff=N  first=N  dsize=N  rel=ok`,
                # NOT a bare column -- parsing f[1] as an int silently matched
                # nothing and the axis reported no rows at all, which reads like a
                # clean sweep.  Parse the key.
                parsed = 0
                min_nd = None
                nident = 0
                for line in out.splitlines():
                    f = line.split()
                    m = re.search(r"ndiff=(\d+)", line)
                    if f and f[0].endswith(".c") and m:
                        nd = int(m.group(1))
                        tag = ""
                        if nd < base[0]:
                            # ONLY "BETTER" IF THE LENGTHS AGREE.  A variant that
                            # is still N instructions off is compared OUT OF
                            # PHASE, so a lower positional count is misalignment
                            # moving, not distance closing.  Batch 339 brief D
                            # was handed two such rows (82->81 and 149->145) and
                            # had to establish that neither was an improvement;
                            # dsize is sweep_variants' length delta.
                            # dsize=0 IS NOT ENOUGH.  Batch 343 brief C showed a
                            # row reading 29 -> 28 at dsize=0 that aligncmp scores
                            # 110/22 BEFORE AND AFTER -- only the hunk count moved.
                            # A positional count can fall while nothing closes,
                            # because an equal-length stream can still be out of
                            # phase when the instruction counts differ and a PAD
                            # absorbs the difference.  So a win is only credible
                            # when `first` moves too; flag the rest for checking
                            # rather than calling them improvements.
                            dm = re.search(r"dsize=(-?\d+)", line)
                            fm = re.search(r"first=(-?\d+)", line)
                            if dm and dm.group(1) != "0":
                                tag = ("  <-- lower count but dsize=" + dm.group(1)
                                       + ": MISALIGNMENT, not an improvement")
                            elif fm and base_first is not None and \
                                    fm.group(1) == str(base_first):
                                tag = ("  <-- lower count but `first` UNMOVED at "
                                       + fm.group(1) + ": may be a phase artifact,"
                                       " check with aligncmp before believing it")
                            else:
                                tag = "  <-- BETTER"
                        if nd == 0:
                            tag = "  <-- *** ZERO ***"
                        if tag:
                            # PRINT THE SOURCE LINE, not just anchorNN.  Brief D
                            # had to import boundaries() to recover which
                            # placement a winning row meant.
                            n = re.match(r"(?:anchor|barrier)(\d+)", f[0])
                            where = ""
                            if n and int(n.group(1)) < len(pts):
                                off = pts[int(n.group(1))]
                                where = ("  @ after: ..."
                                         + text[max(i, off - 46):off].strip()
                                         .replace("\n", " ")[-46:])
                            print(f"  {line}{tag}{where}")
                        parsed += 1
                        min_nd = nd if min_nd is None else min(min_nd, nd)
                        # SURFACE BIT-IDENTITY.  sweep_variants now fingerprints
                        # each variant's OBJECT (encodings + relocations), and a
                        # row identical to the base measured NOTHING -- gcc
                        # canonicalised the difference away before the back end.
                        # "all N inert" and "all N measured nothing" are very
                        # different claims, and only the second one tells an agent
                        # to stop re-spelling.  One park and one earlier batch
                        # spent ~22 rows re-spelling an expression gcc normalises.
                        if "BIT-IDENTICAL TO BASE" in line or "bit-identical to" in line:
                            nident += 1
                        # A ROW THE GUARD JUST REJECTED MUST NOT ENTER `BEST`.
                        # The flag axis was given this refusal in batch 345; the
                        # anchor axis was not, so it kept appending every row and
                        # the closing summary read `Improved 739 -> 700` off a row
                        # tagged "MISALIGNMENT, not an improvement" two lines
                        # above it.  Measured on three Anim parks the moment they
                        # got an anchor axis at all: ParticleCloud 739 -> 700,
                        # Heal 824 -> 810, StatUp 844 -> 842, EVERY ONE of them
                        # from a dsize != 0 row.  Printing the caveat and then
                        # contradicting it in the verdict is worse than not
                        # printing it, because the verdict is what gets copied
                        # into a park header.
                        if nd == 0 or not tag or tag == "  <-- BETTER":
                            best.append((nd, f"{label.lower()} @ {f[0]}", ""))
                written = min(len(pts), a.max)
                if parsed and parsed < written:
                    # A variant that produces no row did not compile or did not
                    # reach the encoding compare.  Say so: silently measuring
                    # fewer variants than were written reads like a clean sweep.
                    print(f"  !! {written - parsed} of {written} variant(s) produced"
                          f" NO ROW -- they did not compile or did not measure;"
                          f" that part of the axis is UNTESTED, not inert")
                if not parsed:
                    print(f"  !! NO ROWS PARSED from sweep_variants -- do NOT read this"
                          f" as a clean sweep; check its output by hand")
                elif min_nd >= base[0]:
                    if nident == parsed and parsed:
                        print(f"  !! ALL {parsed} VARIANT(S) WERE BIT-IDENTICAL TO "
                              f"THE BASE -- this axis measured NOTHING.")
                        print(f"     gcc canonicalises these spellings away before "
                              f"the back end; they are")
                        print(f"     NOT negatives and must not be recorded as "
                              f"levers tried.")
                    else:
                        print(f"  all {parsed} inert or worse (best {min_nd}, "
                              f"base {base[0]})"
                              + (f"  -- BUT {nident} OF THEM WERE BIT-IDENTICAL TO "
                                 f"THE BASE and measured nothing" if nident else ""))
            finally:
                shutil.rmtree(d, ignore_errors=True)
            print()

    best.sort()
    print("BEST")
    for nd, how, grp in best[:5]:
        print(f"  {nd:5}  {how} {grp}")
    if best[0][0] == 0:
        print("\n*** A ZERO WAS FOUND BY SWEEP ALONE -- verify it, then land it. ***")
    elif best[0][0] < base[0]:
        print(f"\nImproved {base[0]} -> {best[0][0]} with no judgment applied.")
    else:
        if globals().get("_CAPPED"):
            print("\nNo improvement found -- but the anchor axis was CAPPED, so it is")
            print("NOT closed.  Re-run with --max set to the full boundary count before")
            print("recording anything about that axis.")
        else:
            # SAY WHICH AXES ACTUALLY RAN.  With --flags or --anchor alone this
            # printed "Both cheap axes are CLOSED", which is a false completeness
            # claim and exactly the kind of sentence that gets copied into a park
            # header as a recorded negative (347 brief E).  A closed axis is only
            # the axis that was swept.
            ran = [n for n, on in (("flag", a.flags),
                                   ("anchor", a.anchor),
                                   ("barrier", a.barrier)) if on]
            if len(ran) >= 2:
                print(f"\nThe {' and '.join(ran)} axes are CLOSED on this park."
                      f"  That is a recorded")
                print("negative worth putting in its header before an agent starts.")
            else:
                print(f"\nThe {ran[0] if ran else 'requested'} axis is CLOSED on this"
                      f" park -- AND ONLY THAT ONE.")
                print("The other cheap axis was NOT RUN, so do not record it as closed;")
                print("re-run without the axis flag to sweep both.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
