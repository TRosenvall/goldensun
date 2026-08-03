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


def canon(s):
    s = ALIAS.sub(lambda m: _ALIAS[m.group(1)], s)
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
        return f"{m.group(1)}=" + w[off // 4]

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
    # Some translation units -- several overlay stems already, per the
    # per-file rules in the Makefile -- only byte-match at -O1. Screening at
    # -O2 alone reports those as failures that no rewriting of the C can fix.
    cflags = ["-O1" if (a == "-O2" and "--O1" in sys.argv) else a for a in CFLAGS]
    src = [a for a in sys.argv[1:] if a.endswith(".c")][0]
    src = os.path.join(ROOT, src) if not src.startswith("/") else src

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
