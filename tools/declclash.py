"""Which extern declarations disagree, and which modules that blocks.

PASS 2.5's BLOCKER IS NOT "DUPLICATE" DECLARATIONS -- C PERMITS A REPEATED
IDENTICAL `extern`.  It is CONFLICTING declarations: two landed pieces of one
module declaring the same symbol at different types, each type chosen because it
made that piece's own function match.  Separate TUs hide the disagreement; the
merge is simply where it surfaces.

Scope matters and the two numbers are different:
  --modules  (default) INTRA-module clashes -- what actually blocks pass 2.5,
             since the merge only brings one module's pieces together.
  --tree     tree-wide clashes -- what blocks hoisting a symbol into a header,
             which is a bigger job and arguably pass 3/4.

THE TYPES ARE FACTS, NOT PREFERENCES.  Where one side is a pointer and another
is not, the question is whether the ROM dereferences twice, and the hand-written
asm answers it:

    grep -rn -A2 '=iwram_3001ebc' asm --include='*.s'

    ldr r5, =iwram_3001ebc     <- address of the symbol
    ldr r3, [r5]               <- loads the WORD STORED THERE, then offsets
    mov r2, #0xe0                 0xe0/0xe4/0xec off it

so iwram_3001ebc HOLDS A POINTER, and `extern unsigned char iwram_3001ebc;`
used as `&iwram_3001ebc` is the wrong shape however well it matched in
isolation.  Resolving a clash is a measurement, not a vote.

usage: declclash.py [--modules | --tree] [--sym NAME] [--top N]
"""
import re, os, sys, collections

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DECL = re.compile(
    r"^\s*extern\s+([A-Za-z_][\w \t\*]*?)\s*(\**)\s*(\w+)\s*(\[[^\]]*\])?\s*;", re.M)


def strip_comments(s):
    s = re.sub(r"/\*.*?\*/", " ", s, flags=re.S)
    return re.sub(r"//[^\n]*", " ", s)


def module_of(path):
    """A module is the piece prefix: rom_2e00_c_c_b.c -> rom_2e00.

    Pass 2 splits a module into pieces by appending _a/_b/_c/_d chains, so
    stripping that chain recovers the module the merge has to rebuild.
    """
    b = os.path.basename(path)[:-2]
    b = re.sub(r"(_[abcd])+$", "", b)
    return os.path.join(os.path.dirname(path), b)


def scan():
    """-> (per_module, per_tree, files_of_module).  Landed sources only."""
    per_mod = collections.defaultdict(lambda: collections.defaultdict(set))
    per_tree = collections.defaultdict(lambda: collections.defaultdict(set))
    modfiles = collections.defaultdict(set)
    for root, _d, fs in os.walk(os.path.join(ROOT, "src")):
        if "non_matching" in root:
            continue
        for f in sorted(fs):
            if not f.endswith(".c"):
                continue
            p = os.path.join(root, f)
            rel = os.path.relpath(p, ROOT)
            m = module_of(rel)
            modfiles[m].add(rel)
            code = strip_comments(open(p, errors="replace").read())
            for mm in DECL.finditer(code):
                ty = (mm.group(1).strip() + " " + mm.group(2)).strip()
                if "(" in ty:                 # function declarations
                    continue
                sym = mm.group(3)
                per_mod[m][sym].add(ty)
                per_tree[sym][ty].add(rel)
    return per_mod, per_tree, modfiles


def clashes(per_mod, modfiles):
    """symbol -> number of multi-piece modules it blocks."""
    out = collections.Counter()
    detail = collections.defaultdict(list)
    for m, syms in per_mod.items():
        if len(modfiles[m]) < 2:
            continue
        for s, ts in syms.items():
            if len(ts) > 1:
                out[s] += 1
                detail[s].append((m, sorted(ts)))
    return out, detail


def main():
    per_mod, per_tree, modfiles = scan()
    argv = sys.argv[1:]
    top = 15
    if "--top" in argv:
        top = int(argv[argv.index("--top") + 1])

    if "--sym" in argv:
        s = argv[argv.index("--sym") + 1]
        print(f"{s}  -- tree-wide declarations:\n")
        for ty, fl in sorted(per_tree[s].items(), key=lambda kv: -len(kv[1])):
            print(f"  {len(fl):4} file(s)  extern {ty} {s};")
            for f in sorted(fl)[:3]:
                print(f"            {f}")
        _, detail = clashes(per_mod, modfiles)
        if detail.get(s):
            print(f"\n  blocks {len(detail[s])} multi-piece module(s):")
            for m, ts in sorted(detail[s])[:12]:
                print(f"    {m:46} {ts}")
        if any(t.endswith("*") for t in per_tree[s]) and \
           any(not t.endswith("*") for t in per_tree[s]):
            print(f"\n  POINTER-vs-VALUE clash.  Settle it from the ROM:")
            print(f"    grep -rn -A2 '={s}' asm --include='*.s' | head")
        return 0

    tree_clash = sum(1 for s, t in per_tree.items() if len(t) > 1)
    multi = {m: v for m, v in modfiles.items() if len(v) > 1}
    cl, detail = clashes(per_mod, modfiles)
    blocked = len({m for s in detail for m, _ in detail[s]})
    tot = sum(cl.values())

    print(f"landed modules (by piece prefix)      {len(modfiles)}")
    print(f"  holding 2+ landed pieces            {len(multi)}")
    print(f"  with an INTRA-MODULE type clash     {blocked}"
          f"  ({100 * blocked // max(len(multi), 1)}% of multi-piece modules)")
    print(f"  clashes, summed over modules        {tot}"
          f"  across {len(cl)} distinct symbols")
    print(f"tree-wide symbols declared 2+ ways    {tree_clash}")
    print()
    print("THE DEBT IS CONCENTRATED -- resolve these and most of it goes:")
    run = 0
    for i, (s, c) in enumerate(cl.most_common(top), 1):
        run += c
        ptr = ""
        if any(t.endswith("*") for t in per_tree[s]) and \
           any(not t.endswith("*") for t in per_tree[s]):
            ptr = "  [ptr-vs-value: ask the ROM]"
        print(f"  {i:2}. {s:24} blocks {c:3} modules   cum {100*run//tot:3}%{ptr}")
    vp = sum(1 for s in detail
             if any(t.endswith("*") for t in per_tree[s])
             and any(not t.endswith("*") for t in per_tree[s])
             for _ in detail[s])
    print()
    print(f"pointer-vs-value clashes: {vp} of {tot} ({100*vp//max(tot,1)}%)"
          f" -- each one a FACT about the ROM, not a preference")
    print("\n  python3 tools/declclash.py --sym iwram_3001ebc")
    return 0


if __name__ == "__main__":
    sys.exit(main())
