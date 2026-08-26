#!/usr/bin/env python3
"""tryc.py -- compile a candidate .c and diff it against the ROM's assembly.

Runs INSIDE the build container (gcc-2.96 lives at /opt/gcc296 and there is no
Darwin host for it):

    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        python3 tools/tryc.py src/rom_9000/rom_ca2c_a.c

Compiles with the production flags, then compares the instruction stream
against the hand-written asm/ counterpart. Comparing text rather than bytes is
deliberate: a byte diff of an unlinked object is dominated by relocation
placeholders, which is the trap tools/asmdiff.py fell into and why NEXT.md says
not to trust it alone. Mnemonic+operand equality after label normalisation is
the thing that actually predicts a match.

It is still a SCREEN, not a verdict. `make compare` in the container is the
only authority, and a green screen here should always be followed by one.

Exit 0 on an exact instruction-stream match, 1 otherwise.
"""
import os
import re
import subprocess
import sys

ROOT = "/work" if os.path.isdir("/work/asm") else \
    os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GCC = os.environ.get("GCC296_DIR", "/opt/gcc296")
CFLAGS = ["-B" + GCC + "/", "-O2", "-mthumb", "-mthumb-interwork",
          "-mcpu=arm7tdmi", "-fno-builtin", "-nostdinc", "-ffreestanding",
          "-fcall-used-r4", "-I" + os.path.join(ROOT, "include")]


# Thumb-1's data-processing ops are destructive, so `lsl r2, r2, #2` and
# `lsl r2, #2` are the SAME instruction -- the ROM disassembly uses the
# two-operand shorthand and gcc emits the three-operand form. Without folding
# these together every single function reports a false diff on its first
# arithmetic instruction, which is exactly what the first run of this did.
DESTRUCTIVE = re.compile(
    r"^(adc|add|and|asr|bic|eor|lsl|lsr|mul|mvn|neg|orr|ror|sbc|sub)"
    r"(\s+)(\w+), \3, (.+)$")


# `=` covers literal-pool loads. The ROM disassembly writes `ldr r0, =1`
# and gcc writes `ldr r0, =0x1`; identical instructions, and left
# unnormalised they read as a mismatch. This hid OvlFunc_971_2009050,
# whose nine instructions were otherwise exact -- and it would hide any
# function in the pool-tell class, which is the one most often worked on.
# A symbol (`=gState`) has no digits after the `=` and is untouched, and
# an assembler `.set foo = 5` has a space, which the pattern does not allow.
NUM = re.compile(r"(?<![\w.])(#|\.word\s+|=)(-?(?:0[xX][0-9a-fA-F]+|\d+))\b")


def _hex(m):
    """One spelling for one value.

    The ROM's hand-written asm writes `mov r2, #0x91` where gcc writes
    `mov r3, #145`, and gcc writes `.word -2080365184` for a pool entry the
    disassembly shows as 0x84010000. Both are the same instruction. Left
    unnormalised, every function carrying a constant reports a false diff --
    and worse, a REAL constant difference (0x5a vs 0x4a) becomes impossible to
    spot in the noise.
    """
    return m.group(1) + hex(int(m.group(2), 0) & 0xFFFFFFFF)


# ARM register aliases. gcc emits `sl`, the ROM disassembly writes `r10`, and
# they are the same register -- LoadMoveIcon and LoadOldMoveIcon differed by
# nothing else. sp/lr/pc are deliberately NOT normalised: both sides already
# spell those the same way, so touching them only risks new noise.
# `lr` IS `r14`, and the two sides do NOT agree on how to spell it. The comment
# that used to sit here said sp/lr/pc were left alone because "both sides
# already spell those the same way". That is true inside a push/pop list, where
# both write `lr`, and false everywhere else: the ROM disassembly writes
# `mov r14, r1` where gcc writes `mov lr, r3`. OvlFunc_883_200834c -- a
# thirteen-member family -- reported two differences for that alone.
# sp and pc stay out: both sides really do agree on those, and touching them
# only risks new noise.
ALIAS = re.compile(r"\b(sl|fp|ip|lr)\b")
_ALIAS = {"sl": "r10", "fp": "r11", "ip": "r12", "lr": "r14"}

# An `al` condition suffix is "always", which is the default, so `pushal` and
# `push` assemble to the SAME halfword -- verified against the assembler:
#
#     pushal {r5, r6, lr}   ->  b560
#     push   {r5, r6, lr}   ->  b560
#
# The disassembly that produced asm/ spells one instruction in the whole tree
# that way (asm/overlays/common/common1_c_a_c_c_a.s), and without this a
# byte-perfect function reads as "1 differ" and gets parked. Only the forms that
# actually occur are rewritten; a blanket "strip a trailing al" would also eat
# real mnemonics such as `bal` -> `b` (harmless) but is not worth the risk on
# names this tool does not control.
CONDAL = re.compile(r"^(push|pop)al\b")


WILDCARD_HITS = set()


def makefile_flags(src_rel):
    """Which flag set the BUILD uses for this source, from the Makefile itself.

    Several translation units -- overlay stems mostly -- are built at -O1
    rather than -O2, and overlays/common/common2_c* drops -mthumb-interwork.
    Screening one of those at -O2 reports a clean match that then fails the
    build. That happened to OvlFunc_964_2009348: the .c was written, the
    overlay compare failed, and the .c had to be reverted.

    Returns a set of adjustments rather than guessing: "O1", "no-interwork",
    or nothing.

    WILDCARD_HITS records, as a side effect, whether any rule that fired was a
    `%` PATTERN rather than an explicit target. That distinction matters: a
    pattern is anchored on a name prefix, and the split chain that produces
    those prefixes is NOT a translation-unit boundary, so a pattern can spread
    one TU's flag choice onto a neighbouring TU that merely shares a prefix.
    See the narrowed rom_7b7f1c rule in the Makefile for the case that cost a
    round -- at the wrong -O the diff was four lines of argument fill order,
    indistinguishable from a real blocker.
    """
    p = os.path.join(ROOT, "Makefile")
    if not os.path.exists(p):
        return set()
    lines = open(p, errors="replace").read().split("\n")
    out = set()
    for i, l in enumerate(lines):
        m = re.match(r"^(asm/\S+)\.o:\s*(src/\S+)\.c\s*$", l)
        if not m:
            continue
        pat = m.group(2) + ".c"
        # make's % is a single wildcard; anchor it so a pattern cannot match
        # a path in a different directory
        # re.escape does NOT escape '%', so replacing the escaped form finds
        # nothing and every pattern silently fails to match -- which is how
        # this first shipped, reporting no per-file rules at all.
        rx = "^" + re.escape(pat).replace("%", ".*") + "$"
        if not re.match(rx, src_rel):
            continue
        recipe = "\n".join(lines[i + 1:i + 4])
        # Record the pattern ONLY when it actually contributes a non-default
        # flag. The generic `src/%.c` rule is a wildcard too, so recording
        # every match would fire the hint on every mismatch and make it noise.
        hit = False
        if "O1_CFLAGS" in recipe:
            out.add("O1")
            hit = True
        if "COMMON2_CFLAGS" in recipe:
            out.add("no-interwork")
            hit = True
        if hit and "%" in pat:
            WILDCARD_HITS.add(pat)
    return out


def _asm_constants():
    """`.set NAME, VALUE` definitions from the .inc files.

    Hand-written asm writes `ldr r3, =REG_DMA3SAD`; gcc writes
    `ldr r3, =0x40000d4`. gba.inc defines that name with `.set`, so the
    assembler resolves it to the same word and the two are the SAME
    instruction -- but they compare unequal as text, which reported
    Func_80a22f4 as differing at instruction zero when that line was fine.
    """
    out = {}
    for name in ("gba.inc", "macros.inc"):
        p = os.path.join(ROOT, "include", name)
        if not os.path.exists(p):
            continue
        for line in open(p, errors="replace"):
            m = re.match(r"\s*\.set\s+(\w+)\s*,\s*(0[xX][0-9a-fA-F]+|\d+)", line)
            if m:
                out[m.group(1)] = hex(int(m.group(2), 0) & 0xFFFFFFFF)
    # Linker-script fragments define absolute symbols the same way, and the
    # message ids are the ones that matter: the C refers to `&_MSG_c9b` where
    # the disassembly shows the bare 0xc9b it resolves to. Same pool word,
    # different spelling.
    for name in ("message.sym", "wram.sym", "file_table.sym", "area.sym"):
        p = os.path.join(ROOT, name)
        if not os.path.exists(p):
            continue
        for line in open(p, errors="replace"):
            m = re.match(r"\s*(\w+)\s*=\s*(0[xX][0-9a-fA-F]+|\d+)\s*;", line)
            if m:
                out[m.group(1)] = hex(int(m.group(2), 0) & 0xFFFFFFFF)
    return out


ASM_CONST = _asm_constants()
ASM_CONST_RE = re.compile(r"=(" + "|".join(map(re.escape, ASM_CONST)) + r")\b") \
    if ASM_CONST else None


def aligned_report(rom, ours, name):
    """Report disagreeing REGIONS instead of disagreeing positions.

    WHY THIS EXISTS. The positional diff compares instruction i to instruction
    i, which is right for a function that is the same length and wrong the
    moment one side has an extra instruction: everything after shifts by one and
    reports as different. On a twenty-instruction function that is survivable --
    the listing is short enough to read. On a 140-instruction one the header
    said "132 differ" when the two streams actually disagreed in six small
    places totalling 36 instructions, and every variant screened afterwards
    also said "132 differ", so the number could not rank them.

    That is not a cosmetic problem. It makes the ONE number the screen prints
    useless on exactly the functions where reading the whole listing is least
    practical, and 45% of the remaining work is in functions over 400
    instructions.

    difflib on the instruction lists gives the regions directly. The count to
    watch is the LAST line: instructions inside disagreeing regions, which does
    fall as a candidate improves.
    """
    import difflib
    sm = difflib.SequenceMatcher(None, rom, ours, autojunk=False)
    bad = 0
    print(f"  ~~ {name}  (rom {len(rom)}, ours {len(ours)})")
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            continue
        bad += max(i2 - i1, j2 - j1)
        print(f"     {tag.upper()} at rom[{i1}:{i2}] ours[{j1}:{j2}]")
        for x in rom[i1:i2]:
            print(f"          rom  {x}")
        for y in ours[j1:j2]:
            print(f"          ours {y}")
    print(f"     {bad} instruction(s) in disagreeing regions, of {len(rom)}")
    return bad


def pool_is_inline(path):
    """True if the reference .s dumps its literal pool inside the function body.

    Two markers: the `.pool_aligned` macro, or a `.word` sitting before the
    function's `.func_end`. Either means gcc had to branch over the pool, and a
    single-function translation unit will not reproduce that placement.
    """
    if not path or not os.path.exists(path):
        return False
    txt = open(path, errors="replace").read()
    for m in re.finditer(r"\.thumb_func_start.*?\.func_end", txt, re.S):
        body = m.group(0)
        if ".pool_aligned" in body or re.search(r"^\s*\.word\s", body, re.M):
            return True
    return False


def canon(s):
    s = CONDAL.sub(r"\1", s)
    s = ALIAS.sub(lambda m: _ALIAS[m.group(1)], s)
    if ASM_CONST_RE:
        s = ASM_CONST_RE.sub(lambda m: "=" + ASM_CONST[m.group(1)], s)
    return NUM.sub(_hex, DESTRUCTIVE.sub(r"\1\2\3, \4", s))


def resolve_pools(body):
    """Fold gcc's explicit literal pool into `ldr rD, =value` form.

    gcc emits `ldr r3, .L14+4` with the constants spelled out as `.word` under
    a `.L14:` label; the ROM's hand-written asm uses the assembler's `=value`
    shorthand and lets gas build the pool. Identical machine code, completely
    different text -- so without this every pool-using function looks like a
    total mismatch from instruction zero, which is what the first run of this
    reported for Func_80ad5f4 and gfree.

    Applied to BOTH sides, since some hand-written files also spell their
    pools out.
    """
    words, cur = {}, None
    for s in body:
        m = re.match(r"^(\.?L[0-9a-fA-F]+):$", s)
        if m:
            cur = m.group(1)
            words.setdefault(cur, [])
            continue
        m = re.match(r"^\.word (\S+)$", s)
        if m and cur is not None:
            words[cur].append(m.group(1))
            continue
        cur = None

    used = set()

    def deref(m):
        # groups: 1 = "ldr", 2 = " rD, ", 3 = label, 4 = optional +offset
        lab, off = m.group(3), int(m.group(4) or "0", 0)
        w = words.get(lab)
        if not w or off % 4 or off // 4 >= len(w):
            return m.group(0)
        used.add(lab)
        # Resolve an absolute symbol to its value HERE, not in canon(): the
        # pool entry only becomes visible as `=NAME` after this substitution,
        # so canon() has already run by then and never sees it.
        val = w[off // 4]
        return f"{m.group(1)}{m.group(2)}=" + ASM_CONST.get(val, val)

    # `ldrh`/`ldrb` ARE THE SAME INSTRUCTION HERE, and missing that cost a
    # four-member family several rounds. Thumb-1 has no pc-relative `ldrh` or
    # `ldrb` -- only `LDR Rd, [PC, #imm8*4]` exists -- so when gcc's HImode
    # pattern prints
    #
    #     ldrh r2, .L0
    #
    # gas assembles the very same halfword the ROM's `ldr r2, =0x1f` does,
    # 0x4a0d. Matching only `ldr` here left that line reported as a difference
    # in every screen of OvlFunc_914_2008b24 and its three twins, all four of
    # which are byte-identical to the ROM. The mnemonic is rewritten to `ldr`
    # so the two sides can compare at all.
    #
    # Only a BARE LABEL operand matches, so `ldrh r2, [r3, #4]` is untouched.
    out = [re.sub(r"^(ldr)[bh]?(\s+\w+, )(\.?L[0-9a-fA-F]+)(?:\+(0[xX][0-9a-fA-F]+|\d+))?$",
                  deref, s) for s in body]

    # drop the pool itself, but only the labels actually dereferenced -- a
    # `.word` under an unreferenced label is data (a jump table), not a pool
    keep, skipping = [], False
    for s in out:
        m = re.match(r"^(\.?L[0-9a-fA-F]+):$", s)
        if m:
            skipping = m.group(1) in used
            if skipping:
                continue
        if skipping and re.match(r"^(\.word |\.align)", s):
            continue
        skipping = False
        keep.append(s)
    return keep


LABEL = re.compile(r"\.?\bL[0-9a-fA-F]+\b")


def renumber(body):
    """Number the labels that SURVIVED pool resolution, and KEEP definitions.

    Must run after resolve_pools, not before: gcc's pool labels only exist on
    one side of the comparison, so numbering with them present offsets every
    branch target relative to the ROM's.

    LABEL DEFINITIONS ARE KEPT IN THE STREAM. They used to be dropped, on the
    reasoning that "their position is implied by branch order". It is not.

    OvlFunc_931_2008360 compared equal on every instruction and then differed
    from the ROM by ONE BYTE -- a `beq` whose offset was 0x02 in the ROM and
    0x06 in ours. Same mnemonic, same normalised target name, different
    distance, because the label sat two instructions further along. With the
    definitions dropped there was nothing left in either stream to disagree
    about, and the screen reported a clean match on a function that fails
    `make compare`.

    Keeping `L<n>:` as a token makes the position part of the comparison,
    which is what a branch actually encodes.
    """
    labels = {}

    def norm(m):
        if m.group(0) not in labels:
            labels[m.group(0)] = "L%d" % len(labels)
        return labels[m.group(0)]

    # A definition nothing branches to carries no information -- gcc leaves
    # such labels behind after pool resolution and the ROM's disassembly does
    # not. Keep only the ones some instruction actually references, which is
    # what makes the POSITION of a real branch target part of the comparison
    # without importing the two sides' bookkeeping differences.
    #
    # DROP THEM BEFORE NUMBERING, NOT AFTER. Numbering first hands an index to
    # a label that is then thrown away, so every later label on that side is
    # off by one and the two streams disagree about names they agree about the
    # position of. OvlFunc_914_2008b24 -- byte-identical to the ROM -- reported
    # `ble L1` against `ble L2` for exactly this, because gcc's discarded pool
    # label had consumed L1 on our side and nothing had on the ROM's.
    defn = re.compile(r"^\.?L[0-9a-fA-F]+:$")
    used_raw = set()
    for line in body:
        if not defn.match(line):
            used_raw.update(LABEL.findall(line))
    body = [x for x in body if not defn.match(x) or x[:-1] in used_raw]
    return [LABEL.sub(norm, s) for s in body]


def instructions(text, want=None):
    """Ordered [(name, [insn, ...])] from a .s body.

    Labels are normalised to L<n> in first-appearance order so that gcc's
    .L12/.L13 and the disassembly's .L8ca4c compare equal, and literal-pool
    entries are kept as `.word <sym>` because a pool that differs in CONTENT
    is a real mismatch even when the instruction that loads it is identical.
    """
    out, cur, body = [], None, []

    # The two sides spell a function start differently and both must be read:
    # hand-written asm uses the macros.inc `.thumb_func_start NAME` macro,
    # while gcc emits a bare `.thumb_func` directive followed by `NAME:` on
    # the next line. Missing the second form made this report nothing at all.
    pending = False
    for raw in text.split("\n"):
        l = raw.split("@")[0].rstrip()
        m = re.match(r"\s*\.(?:thumb_func_start(?:_noalign)?|arm_func_start)\s+(\S+)", l,
                     re.IGNORECASE)
        if m:
            if cur is not None:
                out.append((cur, body))
            cur, body, labels = m.group(1), [], {}
            continue
        if re.match(r"\s*\.(thumb_func|arm)\s*$", l):
            pending = True
            continue
        if pending:
            m = re.match(r"^(\w+):", l)
            if m:
                if cur is not None:
                    out.append((cur, body))
                cur, body, pending, labels = m.group(1), [], False, {}
                continue
            if l.strip().startswith("."):
                continue  # .type/.size between the directive and the label
            pending = False
        if re.match(r"\s*\.(func_end|size)\b", l) and cur is not None:
            out.append((cur, body))
            cur, body = None, []
            continue
        if cur is None:
            continue
        s = l.strip()
        if not s or s.startswith("@"):
            continue
        if s.startswith(".") and not s.startswith(".word"):
            # .align/.global/.type/.size/.func_end carry no code
            if s.startswith(".thumb") or s.startswith(".arm"):
                continue
            if re.match(r"\.L?\w+:", s):
                pass  # a label definition; falls through to normalisation
            else:
                continue
        # Label definitions are KEPT through parsing so resolve_pools can see
        # which .word runs belong to which pool label; the bare ones are
        # dropped afterwards, since their position is implied by branch order.
        s = re.sub(r"\s+", " ", s)
        # A FOURTH SPELLING DIFFERENCE, and the only one that is a typo rather
        # than a convention: a handful of lines in the inherited disassembly
        # have no space after the operand comma --
        #
        #     ldr r0,=.L3058          (asm/overlays/rom_7a8c8c/...)
        #     ldr r0, =.L3130         (every other line in the same function)
        #
        # gcc always emits the spaced form, so one of these in a function makes
        # a byte-exact translation report one differing position, in the middle
        # of an otherwise clean diff -- which reads exactly like a wrong symbol.
        # Collapsing runs of whitespace does not fix it; there is no whitespace
        # to collapse.
        s = re.sub(r",\s*", ", ", s)
        # A FIFTH SPELLING. `ldrb r3, [r3]` and `ldrb r3, [r3, #0]` are the same
        # instruction -- the zero-offset form is an alias -- and the ROM's
        # disassembly writes one while gcc writes the other. Fold them, the same
        # way the destructive-form and comma-spacing differences are folded.
        #
        # Only the bare single-register form: `[r3, r2]` is a REGISTER offset
        # and a different instruction, so it must not be touched.
        s = re.sub(r"\[(r\d+|sp|pc)\]", r"[\1, #0x0]", s)
        body.append(canon(s))
    if cur is not None:
        out.append((cur, body))
    out = [(n, renumber(resolve_pools(b))) for n, b in out]
    if want:
        out = [(n, b) for n, b in out if n in want]
    return out


def text_size(asm_text, cflags_unused=None):
    """Bytes of .text an assembler produces for this listing, or None.

    WHY THIS EXISTS. The instruction comparison above resolves literal-pool
    loads to their VALUES, which is what lets it see through the ROM's
    `ldr r0, =0x242` versus gcc's `ldr r0, .L8` + `.word 578`. That
    normalisation is correct and necessary, and it means the comparison cannot
    see how many bytes the pool actually occupies or where it lands.

    OvlFunc_931_2008360 matched on every instruction and every pool word, in
    order, and produced an object with 0x74 bytes of .text where the function
    and its pool are 0x5A. The overlay differed at the first byte past the
    function. Seventeen batches of screening passed before a function fell into
    that gap.

    So: assemble both sides and compare the size. It is cheap, it is the only
    check that sees padding and alignment, and a mismatch here means the
    instruction comparison is telling the truth about a listing that will still
    produce different bytes.
    """
    import tempfile
    with tempfile.TemporaryDirectory() as d:
        sp, op = os.path.join(d, "t.s"), os.path.join(d, "t.o")
        open(sp, "w").write(asm_text)
        r = subprocess.run(["arm-none-eabi-as", "-mcpu=arm7tdmi",
                            "-mthumb-interwork", "-I", os.path.join(ROOT, "include"),
                            "-o", op, sp], capture_output=True, text=True)
        if r.returncode:
            return None
        r = subprocess.run(["arm-none-eabi-objdump", "-h", op],
                           capture_output=True, text=True)
        m = re.search(r"\.text\s+([0-9a-f]+)", r.stdout)
        return int(m.group(1), 16) if m else None


KNOWN_OPTS = {"--align", "--cflags", "--full", "--no-rerun-cse", "--no-sched2",
              "--quiet", "--ref", "--O1"}


def check_opts(argv):
    """Reject unrecognised --options instead of ignoring them.

    WHY THIS EXISTS. `--func <name>` was passed to this tool for many rounds. It
    is not an option and never was; every function in the .c is compared against
    the reference by NAME, so there is nothing to select. It was harmless only
    because scratch files hold one function -- until one held eight, reported a
    single result, and the same name eight times.

    A flag that silently does nothing is worse than one that errors: it reads
    like a filter that is working.
    """
    takes_value = {"--cflags", "--ref"}
    i, bad = 0, []
    args = argv[1:]
    while i < len(args):
        a = args[i]
        if a.startswith("--"):
            if a not in KNOWN_OPTS:
                bad.append(a)
            elif a in takes_value:
                i += 1
        i += 1
    if bad:
        sys.exit(f"tryc.py: unknown option(s): {' '.join(bad)}\n"
                 f"known: {' '.join(sorted(KNOWN_OPTS))}")


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    check_opts(sys.argv)
    quiet = "--quiet" in sys.argv
    ref_path = None
    src = [a for a in sys.argv[1:] if a.endswith(".c")][0]
    src = os.path.join(ROOT, src) if not src.startswith("/") else src

    # Take the build's OWN per-file rules rather than assuming -O2. Several
    # translation units are built at -O1, and overlays/common/common2_c* drops
    # -mthumb-interwork; screening one of those at -O2 reports a clean match
    # that then fails the build. --O1 still forces it by hand, which is needed
    # when the candidate is a scratch file the Makefile has no rule for.
    adjust = makefile_flags(os.path.relpath(src, ROOT))
    # A scratch or parked .c does not sit where the build would put it, so the
    # Makefile has no rule for its path and the per-file flags are lost. The
    # --ref assembly DOES identify the real translation unit, so take the flags
    # from there too and union them in.
    #
    # This is not hypothetical. src/non_matching/ovl_7ed0a0/2009348.c screened
    # from its parked path reported a CLEAN MATCH; its real TU builds at -O1
    # (rule asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a%.o) and at -O1 it does not
    # match at all. The parked note had warned about exactly this and the sweep
    # walked into it anyway, because the warning was prose and this was not.
    if "--ref" in sys.argv:
        _r = sys.argv[sys.argv.index("--ref") + 1]
        _r = os.path.relpath(_r, ROOT) if _r.startswith("/") else _r
        # the rules are written against the src/ side of the pair
        adjust |= makefile_flags(re.sub(r"^asm/", "src/", _r)[:-2] + ".c")
    if "--O1" in sys.argv:
        adjust.add("O1")
    # --no-sched2 is NOT the same thing as --O1, and assuming it was cost a
    # park. The Makefile comment on the O1_CFLAGS rules says -O1 is
    # "equivalently -O2 -fno-schedule-insns2"; for Func_809a44c that is false.
    # -O1 diverges at instruction 4 of 27 while -O2 -fno-schedule-insns2 is a
    # clean match, because -O1 also changes register allocation and expression
    # ordering, not just the post-reload scheduler.
    if "--no-sched2" in sys.argv:
        adjust.add("no-sched2")
    # --no-rerun-cse is the constant-CSE probe. gcc-2.96 runs CSE a second time
    # after loop optimisation, and that second pass is what hoists a repeated
    # pooled constant into a callee-saved register. -fno-gcse,
    # -fno-cse-follow-jumps, -fno-cse-skip-blocks and
    # -fno-expensive-optimizations all leave the hoist in place; only this one
    # removes it.
    if "--no-rerun-cse" in sys.argv:
        adjust.add("no-rerun-cse")
    if adjust and not quiet:
        print(f"  (built with: {', '.join(sorted(adjust))})")
    cflags = ["-O1" if (a == "-O2" and "O1" in adjust) else a for a in CFLAGS]
    if "no-sched2" in adjust:
        cflags = cflags + ["-fno-schedule-insns2"]
    if "no-rerun-cse" in adjust:
        cflags = cflags + ["-fno-rerun-cse-after-loop"]
    if "no-interwork" in adjust:
        cflags = [a for a in cflags if a != "-mthumb-interwork"]
    # --cflags "<extra>" appends arbitrary compiler flags, so a hypothesis
    # about the original invocation can be tested against a known-failing
    # function without editing this file each time.
    if "--cflags" in sys.argv:
        cflags += sys.argv[sys.argv.index("--cflags") + 1].split()

    # --ref lets a scratch .c be tested against any .s, which is what makes
    # the order PROVE FIRST, SPLIT SECOND possible. Most targets sit inside a
    # multi-function .s that has to be split into _a/_b/_c and wired into the
    # linker script before it can be built; doing that for a candidate that
    # then fails to match is a lot of churn for nothing.
    if "--ref" in sys.argv:
        ref = sys.argv[sys.argv.index("--ref") + 1]
        ref_path = ref if os.path.isabs(ref) else os.path.join(ROOT, ref)
        ref = ref if ref.startswith("/") else os.path.join(ROOT, ref)
    else:
        ref = src.replace("/src/", "/asm/")[:-2] + ".s"
    if not os.path.exists(ref):
        sys.exit("no asm counterpart: " + ref)

    # A .s that gcc produced is not a reference -- it IS this compiler's
    # output, so comparing against it always passes and means nothing. That
    # happens the moment a .c lands: the build writes the generated .s to the
    # same path the hand-written one occupied. It reported a match for a
    # function whose build had just failed.
    with open(ref, errors="replace") as f:
        if "Generated by gcc" in (f.readline() or ""):
            sys.exit(f"REFUSING: {os.path.relpath(ref, ROOT)} is GENERATED, not "
                     f"the ROM's assembly.\nComparing against it is a "
                     f"tautology. Recover the original with:\n"
                     f"    git show HEAD:{os.path.relpath(ref, ROOT)}")

    r = subprocess.run([os.path.join(GCC, "xgcc")] + cflags + ["-S", "-o", "-", src],
                       capture_output=True, text=True)
    if r.returncode:
        print("COMPILE FAILED")
        print(r.stderr.strip()[:2000])
        return 1

    ours = instructions(r.stdout)
    theirs = dict(instructions(open(ref, errors="replace").read()))

    ok = True
    for name, got in ours:
        exp = theirs.get(name)
        if exp is None:
            print(f"  ?? {name}: no such function in {os.path.relpath(ref, ROOT)}")
            ok = False
            continue
        if got == exp:
            # The instruction streams agree. Now check that the ASSEMBLED sizes
            # do too -- see text_size() for why that is a separate question.
            reftxt = open(ref, errors="replace").read()
            # Only meaningful when the reference holds exactly ONE function.
            # A multi-function .s assembles to the whole file's .text, and its
            # literal pool may be shared between functions, so there is no
            # honest per-function size to compare against. In that case the
            # check is skipped and reported as skipped -- re-screen against the
            # _b.s after splitting, where it does apply. Reporting a size
            # mismatch there would be a false positive on every split
            # candidate, which is worse than the miss this guards.
            nrefs = len(re.findall(r"\.(?:thumb|arm)_func_start", reftxt, re.I))
            mine = refsz = None
            if nrefs == 1:
                mine = text_size(r.stdout + "\n\t.text\n\t.align\t2, 0\n")
                refsz = text_size(reftxt)
            if mine is not None and refsz is not None and mine != refsz:
                print(f"  !! {name}: instructions match but .text differs -- "
                      f"ours 0x{mine:x}, reference 0x{refsz:x}")
                print(f"     The pool or its padding differs. This WILL fail "
                      f"make compare; see")
                print(f"     src/non_matching/ovl_7b8cb0/2008360.c.")
                ok = False
                continue
            note = "" if nrefs == 1 else "  [size check skipped: ref has "
            note += "" if nrefs == 1 else f"{nrefs} functions]"
            print(f"  OK {name}  ({len(got)} lines){note}")
            # A CLEAN INSTRUCTION STREAM IS NOT A CLEAN FUNCTION when the ROM
            # keeps its literal pool INSIDE the function body. Pool loads are
            # normalised to `ldr rD, =value` on both sides, so a pool sitting at
            # a different distance compares equal here and still changes every
            # PC-relative offset in the emitted bytes.
            #
            # OvlFunc_962_200816c and its twin OvlFunc_967_2008234 both screened
            # OK and both failed `make compare` with ~36 differing bytes, all of
            # them `ldr rN, [pc, #imm]` and branch displacements. gcc puts the
            # pool after the epilogue in a single-function TU; the ROM has it
            # mid-function behind a `.pool_aligned`.
            #
            # This is the THIRD false-positive class in this tool. Warn rather
            # than fail: a mid-function pool does not always move, and the build
            # is still the authority.
            #
            # THE WARNING USED TO LIVE ONLY HERE, ON THE `OK` PATH, and that was
            # a hole. Func_801edec screened XX with ONE differing line -- and
            # that line was only this tool printing a symbol name where the
            # reference prints its value, i.e. cosmetically identical. No
            # warning fired, and `make compare` failed by 323,730 bytes because
            # the TU came out a different SIZE and everything after it shifted.
            # A near-match with an inline pool is exactly as unproven as a
            # clean one, so warn_inline_pool() is now called on both paths.
            if pool_is_inline(ref_path):
                print(f"     !! {name}: the reference keeps its literal pool "
                      f"INSIDE the function (.pool_aligned / mid-body .word).")
                print(f"        Pool loads normalise to `=value`, so a pool at a "
                      f"different distance still compares equal here.")
                print(f"        VERIFY WITH make compare -- this screen cannot "
                      f"see PC-relative offsets.")
            # THIS `continue` IS LOAD-BEARING AND WAS LOST ONCE. Adding the
            # inline-pool warning to this branch in batch 75 displaced it, and
            # from then until batch 81 every CLEAN screen printed its `OK` line
            # and then fell straight into the mismatch report below, emitting
            #
            #     OK  Func_x  (21 lines)
            #     XX  Func_x  (rom 21 lines, ours 21, first diff at 0, 0 differ)
            #
            # and returning failure. "0 differ" is the tell that it is this bug
            # and not a real mismatch. Anything reading the exit status, or
            # grepping for `^  XX`, was told a match had failed.
            continue
        ok = False
        # first divergence, with a little context -- enough to see whether it
        # is a scheduling difference or a genuinely different lowering
        i = next((k for k in range(max(len(got), len(exp)))
                  if k >= len(got) or k >= len(exp) or got[k] != exp[k]), 0)
        # Report the TOTAL number of differing positions, not just where the
        # first one is. Over 40 lines the listing below is a keyhole around the
        # first divergence, and reading "one -> in the window" as "one
        # difference in the function" sent a round down the wrong path: a
        # 45-line function showed a single mismatched `bl` and looked like a
        # symbol-naming problem, while two register-allocation differences sat
        # nine lines further down, outside the window.
        ndiff = sum(1 for k in range(max(len(got), len(exp)))
                    if (exp[k] if k < len(exp) else None)
                    != (got[k] if k < len(got) else None))
        print(f"  XX {name}  (rom {len(exp)} lines, ours {len(got)}, "
              f"first diff at {i}, {ndiff} differ)")
        # THE DIVISION HELPER IS A LINK-TIME QUESTION, NOT A SOURCE ONE, and it
        # is worth saying so here because the diff looks like a wrong symbol.
        # gcc-2.96 emits `__divsi3` for `/` with no flag to rename it; overlay
        # code calls the RAM-resident copy `_divsi3_RAM` through the stub
        # .export_func makes. Deliberately NOT normalised away: 17 call sites
        # inside asm/overlays/ do call `__divsi3` directly, so equating the two
        # names would hide a real difference in those. A hint costs nothing and
        # hides nothing.
        # ALL FOUR HELPERS, not just __divsi3. The first version of this hint
        # matched the literal string "divsi3" and so said nothing about
        # OvlFunc_882_2008064, whose single differing line was
        # `bl __umodsi3` against `bl _umodsi3_RAM`.
        hlp = next((h for h in ("divsi3", "udivsi3", "modsi3", "umodsi3")
                    for a, b in zip(exp, got)
                    if a != b and h in a and h in b), None)
        if hlp:
            print(f"     -- `__{hlp}` vs `_{hlp}_RAM` is the LINKER ALIAS, not "
                  f"the C: add")
            print(f"        `__{hlp} = _{hlp}_RAM;` to this overlay's "
                  f"overlay.ld. See src/overlays/rom_7a5214/ovl_17ec_c_b.c.")
        # A NEAR-MISS WITH AN INLINE POOL IS AS UNPROVEN AS A CLEAN ONE.
        # Func_801edec screened XX with ONE differing line -- and that line was
        # only this tool printing a symbol name where the reference prints its
        # value, i.e. cosmetically identical. The warning below lived only on
        # the OK path, so nothing fired, and `make compare` failed by 323,730
        # bytes: the TU came out a different SIZE and everything after it
        # shifted. Printed here IN ADDITION to the diff, never instead of it --
        # the first attempt at this fix used an `elif ... continue` and
        # suppressed the entire mismatch report, which is far worse than the
        # hole it closed.
        if pool_is_inline(ref_path):
            print(f"     !! the reference keeps its literal pool INSIDE the "
                  f"function. Even a difference that looks cosmetic can mean a "
                  f"different")
            print(f"        translation-unit SIZE, which shifts everything after "
                  f"it. VERIFY WITH make compare.")
        # A NON-DEFAULT FLAG SET THAT CAME FROM A WILDCARD RULE IS A SUSPECT,
        # not a fact. The rule is anchored on a name prefix, and the _a/_b/_c
        # split chain that produces those prefixes is not a translation-unit
        # boundary -- so the rule may be describing a NEIGHBOURING TU. This is
        # printed only on a mismatch, because on a match the flags are right by
        # construction.
        if adjust and WILDCARD_HITS and not quiet:
            print(f"     ?? flags {sorted(adjust)} came from a WILDCARD rule "
                  f"({', '.join(sorted(WILDCARD_HITS))}).")
            print("        That rule may belong to a neighbouring TU that only "
                  "shares a name prefix.")
            print("        Re-screen with the default flags before believing "
                  "this diff:  --cflags \"-O2\"")
        # --align replaces the positional listing with a region-aligned one.
        # `ndiff` above counts POSITIONS, so a single extra instruction makes
        # every later position report as different; on a 140-instruction
        # function that read "132 differ" for streams that disagreed in six
        # places totalling 36 instructions, and it read the same for every
        # variant tried afterwards. Use this on anything long enough that the
        # listing cannot be eyeballed.
        if "--align" in sys.argv:
            aligned_report(exp, got, name)
            continue
        if not quiet:
            # short functions print whole: the useful signal is usually
            # WHERE the two streams re-converge, which a keyhole around the
            # first divergence hides
            n = max(len(got), len(exp))
            # --full disables the keyhole. Needed by tools/audit_parks.py, which
            # looks for LABEL positions among the differing lines: a label in
            # the wrong place means the CONTROL FLOW is wrong, not the codegen,
            # and that has twice hidden behind a correct-sounding blocker
            # diagnosis (OvlFunc_931_2008360 in batch 20, OvlFunc_909_200828c in
            # batch 25). The keyhole is what let the second one hide.
            full = "--full" in sys.argv
            lo, hi = (0, n) if (n <= 40 or full) else (max(0, i - 3), min(n, i + 8))
            for k in range(lo, hi):
                a = exp[k] if k < len(exp) else ""
                b = got[k] if k < len(got) else ""
                print(f"      {'  ' if a == b else '->'} rom {a:<34} ours {b}")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
