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


NUM = re.compile(r"(?<![\w.])(#|\.word\s+)(-?(?:0[xX][0-9a-fA-F]+|\d+))\b")


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
ALIAS = re.compile(r"\b(sl|fp|ip)\b")
_ALIAS = {"sl": "r10", "fp": "r11", "ip": "r12"}


def makefile_flags(src_rel):
    """Which flag set the BUILD uses for this source, from the Makefile itself.

    Several translation units -- overlay stems mostly -- are built at -O1
    rather than -O2, and overlays/common/common2_c* drops -mthumb-interwork.
    Screening one of those at -O2 reports a clean match that then fails the
    build. That happened to OvlFunc_964_2009348: the .c was written, the
    overlay compare failed, and the .c had to be reverted.

    Returns a set of adjustments rather than guessing: "O1", "no-interwork",
    or nothing.
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
        if "O1_CFLAGS" in recipe:
            out.add("O1")
        if "COMMON2_CFLAGS" in recipe:
            out.add("no-interwork")
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
    for name in ("message.sym", "wram.sym", "file_table.sym", "unknown_id.sym"):
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


def canon(s):
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
        lab, off = m.group(2), int(m.group(3) or "0", 0)
        w = words.get(lab)
        if not w or off % 4 or off // 4 >= len(w):
            return m.group(0)
        used.add(lab)
        # Resolve an absolute symbol to its value HERE, not in canon(): the
        # pool entry only becomes visible as `=NAME` after this substitution,
        # so canon() has already run by then and never sees it.
        val = w[off // 4]
        return f"{m.group(1)}=" + ASM_CONST.get(val, val)

    out = [re.sub(r"^(ldr\s+\w+, )(\.?L[0-9a-fA-F]+)(?:\+(0[xX][0-9a-fA-F]+|\d+))?$", deref, s) for s in body]

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
    """Number the labels that SURVIVED pool resolution, then drop definitions.

    Must run after resolve_pools, not before: gcc's pool labels only exist on
    one side of the comparison, so numbering with them present offsets every
    branch target relative to the ROM's.
    """
    labels = {}

    def norm(m):
        if m.group(0) not in labels:
            labels[m.group(0)] = "L%d" % len(labels)
        return labels[m.group(0)]

    body = [LABEL.sub(norm, s) for s in body]
    return [s for s in body if not re.match(r"^L\d+:$", s)]


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
        body.append(canon(s))
    if cur is not None:
        out.append((cur, body))
    out = [(n, renumber(resolve_pools(b))) for n, b in out]
    if want:
        out = [(n, b) for n, b in out if n in want]
    return out


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    quiet = "--quiet" in sys.argv
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
    if adjust and not quiet:
        print(f"  (built with: {', '.join(sorted(adjust))})")
    cflags = ["-O1" if (a == "-O2" and "O1" in adjust) else a for a in CFLAGS]
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
            print(f"  OK {name}  ({len(got)} lines)")
            continue
        ok = False
        # first divergence, with a little context -- enough to see whether it
        # is a scheduling difference or a genuinely different lowering
        i = next((k for k in range(max(len(got), len(exp)))
                  if k >= len(got) or k >= len(exp) or got[k] != exp[k]), 0)
        print(f"  XX {name}  (rom {len(exp)} lines, ours {len(got)}, "
              f"first diff at {i})")
        if not quiet:
            # short functions print whole: the useful signal is usually
            # WHERE the two streams re-converge, which a keyhole around the
            # first divergence hides
            n = max(len(got), len(exp))
            lo, hi = (0, n) if n <= 40 else (max(0, i - 3), min(n, i + 8))
            for k in range(lo, hi):
                a = exp[k] if k < len(exp) else ""
                b = got[k] if k < len(got) else ""
                print(f"      {'  ' if a == b else '->'} rom {a:<34} ours {b}")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
