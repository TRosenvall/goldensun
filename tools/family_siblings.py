#!/usr/bin/env python3
"""family_siblings.py -- rank remaining functions by how much of their own
.s family is already elevated.

Batches 172 and 173 both closed on a lever that no ranking offered. In each
case the answer was sitting in an already-solved .c in the SAME `_a/_b/_c`
family as the target:

  Sprite_DeleteLayerIndex  <- Sprite_DeleteLayer supplied its entire second half
  Func_809b364             <- rom_9ad70_c_c_a.c supplied `g = gState;` with the
                              offset left in the expression, AND `_CONST_1` for
                              a pooled 1, both against the very same halfword

`fuzzy_solved.py` cannot see these. It scores whole-skeleton similarity, and a
sibling that shares one idiom while differing everywhere else scores near zero.
But the tree's splits keep genuinely related routines adjacent, so family
adjacency is a real signal that whole-body similarity throws away.

What a sibling gives is NOT a lever -- docs/elevation.md already had every lever
both of those functions needed. It is the INSTANTIATION: which symbol name,
which of several near-equivalent spellings, and confirmation that a particular
field is handled a particular way. A general rule tells you a lever exists; a
sibling tells you what to type.

    python3 tools/family_siblings.py [--limit N] [--max-insns N]

Columns: how many solved .c files share the family stem, how many of those
mention the same idioms the target's assembly asks for, the target's size, the
files to read, and -- added after batch 175 -- whether the target REPEATS an
expensive constant. That last column is a hard skip, not a warning: batch 175
parked three functions on it in one round, `-fno-gcse` was inert on all three
(it is cse.c's local constant CSE, which no flag reaches), and one of the three
was a cutscene script with no branches and no loops that looked completely
trivial. Read the DUP-CONST column before you read the function. Sorted by solved-sibling count, because a family that is
mostly done is a family whose conventions are already settled.

The idiom columns are the point of the tool. A family with twelve solved
siblings is not useful in itself; a family with one solved sibling that already
spells the exact global the target loads is worth more.
"""
import glob
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import filtered
import pickable
import shapesib

HIGH = re.compile(r"\br(?:8|9|10|11)\b")
# Symbols the target's assembly names, which a sibling may already spell.
SYMBOL = re.compile(r"=\s*([A-Za-z_][A-Za-z0-9_]*)")
POOLED_SMALL = re.compile(r"\bldr\s+r\d+,\s*=(0x[0-9a-f]{1,2}|\d{1,3})\b")


def family(path):
    """The stem shared by every split of one original .s."""
    stem = os.path.basename(path)[:-2]
    return os.path.dirname(path), re.sub(r"(_[abc])+$", "", stem)


def solved_in_family(asm_path):
    d, stem = family(asm_path)
    src_dir = d.replace("asm/", "src/", 1)
    return sorted(glob.glob(os.path.join(src_dir, stem + "*.c")))


def main():
    limit = 20
    max_insns = 90
    for i, a in enumerate(sys.argv):
        if a == "--limit":
            limit = int(sys.argv[i + 1])
        if a == "--max-insns":
            max_insns = int(sys.argv[i + 1])

    parked = pickable.parked()
    rows = []
    for s in glob.glob("asm/**/*.s", recursive=True):
        if os.path.exists(s[:-2].replace("asm/", "src/", 1) + ".c"):
            continue
        sibs = solved_in_family(s)
        if not sibs:
            continue
        text = ""
        for c in sibs:
            try:
                text += open(c, errors="replace").read()
            except OSError:
                pass
        for name, body in shapesib.functions(s):
            if name in parked:
                continue
            ins = [l for l in body if l.strip()
                   and not l.strip().startswith((".", "@", "/*"))]
            n = len(ins)
            if not (12 <= n <= max_insns):
                continue
            if any(HIGH.search(l) for l in ins):
                continue
            syms = {m for l in ins for m in SYMBOL.findall(l)
                    if not m.startswith("L")}
            hit = sorted(x for x in syms if x in text)
            # A pooled small constant is the `_CONST_n` tell; a sibling that
            # already declares one shows which name the tree uses.
            pooled = bool(any(POOLED_SMALL.search(l) for l in ins))
            const_sib = "_CONST_" in text
            # Batch 175: three parks in one batch, all duplicate-constant CSE,
            # and one of them (a cutscene script -- thirteen calls, no branches,
            # no loops) looked completely trivial. -fno-gcse is inert on all
            # three because it is cse.c's LOCAL constant CSE, so there is no
            # flag and no spelling. Check this BEFORE reading the shape.
            vals = filtered.expensive_constants(ins)
            dup = sorted({v for v in vals if vals.count(v) > 1})
            rows.append((len(sibs), len(hit), n, name, s, hit, pooled,
                         const_sib, dup))

    rows.sort(key=lambda r: (-r[1], -r[0], r[2]))
    print("%d remaining functions live in a family with solved siblings\n"
          % len(rows))
    for sibs, nhit, n, name, s, hit, pooled, const_sib, dup in rows[:limit]:
        note = ""
        if dup:
            note += "  <- DUP-CONST %s" % ", ".join(dup[:2])
        if pooled:
            note += "  POOLED-SMALL" + (" (sibling declares _CONST_*)"
                                        if const_sib else "")
        print("  %2d solved  %d shared-sym  %3di  %-28s %s%s"
              % (sibs, nhit, n, name, s, note))
        if dup:
            print("             SKIP unless everything else is exhausted: "
                  "cse.c shares this and no flag or spelling separates it")
        if hit:
            print("             shares: %s" % ", ".join(hit[:6]))
        for c in solved_in_family(s)[:3]:
            print("             read: %s" % c)


if __name__ == "__main__":
    main()
