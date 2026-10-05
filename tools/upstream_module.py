#!/usr/bin/env python3
"""upstream_module.py -- which UPSTREAM MODULE does a function belong to?

This project subdivided the upstream disassembly's objects. Upstream
(`gsret/goldensun`, our `upstream` remote) ships **521 `.s` files**, one per
object, and its `stage1.ld` lists them one `.o` at a time inside named output
sections:

    rom_1b70 : {
        rom_c0/src/rom_1b70.o(.text)
        rom_c0/src/rom_2544.o(.text)
        ...
    }

`tools/split_s.py` then bisected those objects into `_a`/`_b`/`_c`-suffixed
pieces so individual functions could be converted. We now have **5,203** `.s`
files where upstream has 521, a mean of **24.4 pieces per upstream module**, and
`overlays/ovl_30.s` alone has become 2,476 pieces.

WHAT THIS COST, AND WHAT IT DID NOT.  Measured 2026-10-05:

  * NOTHING IN THE ROM.  `make compare` passes and the SHA1 matches.
    Subdividing an object and relinking the pieces in the same order is
    byte-neutral here.
  * NO INFORMATION.  Every one of the 5,203 `.s` files, and 4,461 of our 4,468
    landed `.c` files, maps onto EXACTLY ONE upstream module -- zero crossings
    and zero ambiguity. The splits are strictly nested subdivisions, so the
    upstream boundary is recoverable by filename prefix, which is what this
    tool does.

WHAT IT DID COST is grouping knowledge *in practice*: **150 upstream modules are
now spread across more than one of our `.c` files, a mean of 29.6 each, and only
19 upstream modules have been landed as a single whole `.c`.** When a function is
converted alone it becomes its own translation unit, and the original compiler
saw its module's functions together -- which is the information a TU-merging pass
needs and which nothing in the tree currently surfaces.

HOW MUCH TO TRUST AN UPSTREAM BOUNDARY.  It is upstream's model, not proven
ground truth: upstream's own README lists *"Isolate modules further. Modules
should be partially linked and combined in a final link"* as a ROADMAP ITEM, so
upstream does not claim its objects are the original translation units either.
Treat a module boundary as **better evidence than our bisections and coarser than
the truth might be** -- a hypothesis about grouping, in the same way a park's
figure is evidence and its diagnosis is not.

    python3 tools/upstream_module.py Func_801d9d4      # one function
    python3 tools/upstream_module.py src/rom_15000/x.c # or a path
    python3 tools/upstream_module.py --fragmented      # modules split across many .c
    python3 tools/upstream_module.py --module rom_15000/rom_1ca1c
"""
import collections, os, re, subprocess, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
UPSTREAM = "upstream/master"


def _ls(ref, path, ext):
    r = subprocess.run(["git", "ls-tree", "-r", "--name-only", ref, "--", path],
                       capture_output=True, text=True, cwd=ROOT)
    if r.returncode != 0:
        sys.exit(f"git ls-tree failed for {ref}; is the `upstream` remote fetched?\n"
                 f"  git fetch upstream")
    return [f for f in r.stdout.split() if f.endswith(ext)]


def upstream_modules():
    """{bank: {module basename}} from the upstream disassembly."""
    out = collections.defaultdict(set)
    for f in _ls(UPSTREAM, ".", ".s"):
        out[f.split("/")[0]].add(os.path.basename(f)[:-2])
    return out


def module_of(path, up):
    """The upstream module a tree path belongs to, by longest prefix."""
    parts = path.split("/")
    bank = None
    for p in parts:
        if p in up:
            bank = p
            break
    if bank is None:
        return None
    name = os.path.basename(path).rsplit(".", 1)[0]
    cands = [b for b in up[bank] if name == b or name.startswith(b + "_")]
    return (bank, max(cands, key=len)) if cands else None


def index():
    """Map every module to the tree files that implement it.

    A PARK CANNOT BE RESOLVED BY ITS FILENAME, and assuming otherwise made this
    tool report `our parks: 0` for a module that has nine of them.  Found by
    batch 327 brief J on ovl_7ac2d8: landings are SPLIT-NAMED
    (src/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_a.c) while parks are ADDRESS-NAMED
    (src/non_matching/ovl_7ac2d8/200cfcc.c), so a prefix match against the
    upstream module `ovl_35b8` finds every landing and no park.  The landed side
    was right the whole time, which is what made the zero look plausible.

    So a park is resolved by its RECIPE SYMBOL: take the `--func` out of its
    header, find the asm piece that defines that symbol, and take THAT file's
    module.  Falling back to the filename only when a park has no recipe.

    This is the fourth instance in this project of a NAME CHECK STANDING IN FOR A
    DEFINITION CHECK, and the third of them in my own tooling, after
    park_bodies.py counting a top-level `extern` as a definition and then its own
    sibling bug of reading an indented call as a K&R definition.  A false
    NEGATIVE is the expensive direction: it tells the reader not to look.
    """
    up = upstream_modules()
    landed, parks, asm = collections.defaultdict(list), collections.defaultdict(list), collections.defaultdict(list)
    # which asm piece defines which symbol -> that piece's module
    sym_mod = {}
    for f in _ls("HEAD", "asm/", ".s"):
        m = module_of(f, up)
        if not m:
            continue
        asm[m].append(f)
        # A FILE IN HEAD IS NOT NECESSARILY A FILE ON DISK.  `git ls-tree HEAD`
        # lists what the last commit holds, and install_batch's splits phase
        # DELETES the pre-split .s before anything is committed -- so during a
        # batch this loop hit a path that no longer exists and the whole tool
        # died with FileNotFoundError, for every function.  Reported by batch 327
        # brief F, which had been told to run this tool early; any "no landed
        # sibling" conclusion from that batch was reached without it.
        #
        # My own fix introduced this, and I introduced it while ten agents were
        # running -- the same mid-batch-tool-change discipline breach another
        # brief flagged one batch earlier.
        fp = os.path.join(ROOT, f)
        if not os.path.exists(fp):
            continue
        for sym in re.findall(r"thumb_func_start\s+(\w+)",
                              open(fp, errors="replace").read()):
            sym_mod[sym] = m
    VERIFY = re.compile(r"objcmp\.py[\s\\]+\S+[\s\\]+(\S+\.s)(?:[\s\\]+--func\s+(\S+))?")
    for f in _ls("HEAD", "src/", ".c"):
        if "/non_matching/" not in f:
            m = module_of(f, up)
            if m:
                landed[m].append(f)
            continue
        # Same reason as above: a RETIRED park is still in HEAD and gone from
        # disk until the retirement is committed.
        fp = os.path.join(ROOT, f)
        if not os.path.exists(fp):
            continue
        txt = open(fp, errors="replace").read()
        i = txt.find("*/")
        hdr = (txt[:i] if i > 0 else txt[:3000]).replace("*", " ")
        vm = VERIFY.search(hdr)
        m = None
        if vm:
            # prefer the reference .s the park itself names
            m = module_of(vm.group(1).strip("'\""), up)
            if not m and vm.group(2):
                m = sym_mod.get(vm.group(2).strip("'\""))
        if not m:
            fn = re.search(r"--func\s+([A-Za-z_]\w*)", hdr)
            if fn:
                m = sym_mod.get(fn.group(1))
        if not m:
            m = module_of(f, up)          # last resort: the old filename guess
        if m:
            parks[m].append(f)
    return up, landed, parks, asm


def funcs_in(path):
    p = os.path.join(ROOT, path)
    if not os.path.exists(p):
        return []
    txt = open(p, errors="replace").read()
    if path.endswith(".s"):
        return re.findall(r"thumb_func_start\s+(\w+)", txt) or \
               re.findall(r"\.type\s+(\w+),function", txt)
    txt = re.sub(r"/\*.*?\*/", " ", txt, flags=re.S)
    return [m.group(1) for m in re.finditer(r"^[\w \t\*]*?\b(\w+)\s*\([^;]*\)\s*\{", txt, re.M)]


def report_module(key, landed, parks, asm):
    bank, mod = key
    print(f"\n=== upstream module {bank}/{mod}.s ===")
    print(f"  our landed .c : {len(landed.get(key, []))}")
    print(f"  our parks     : {len(parks.get(key, []))}")
    print(f"  our .s pieces : {len(asm.get(key, []))}")
    for label, files in (("LANDED", landed.get(key, [])), ("PARKED", parks.get(key, [])),
                         ("STILL ASM", asm.get(key, []))):
        if not files:
            continue
        print(f"  --- {label}")
        for f in sorted(files):
            fs = funcs_in(f)
            print(f"      {f}" + (f"   [{', '.join(fs[:4])}{'...' if len(fs) > 4 else ''}]" if fs else ""))


def main():
    args = sys.argv[1:]
    if not args:
        print(__doc__)
        sys.exit(2)
    up, landed, parks, asm = index()

    if args[0] == "--fragmented":
        rows = sorted(((len(v), k) for k, v in landed.items() if len(v) > 1), reverse=True)
        print(f"upstream modules landed across MORE THAN ONE .c: {len(rows)}")
        print(f"modules landed as exactly one whole .c         : {sum(1 for v in landed.values() if len(v) == 1)}")
        print("\n  .c  upstream module")
        for n, (bank, mod) in rows[:40]:
            print(f"{n:5d}  {bank}/{mod}.s")
        return

    if args[0] == "--module":
        bank, mod = args[1].split("/")[-2], os.path.basename(args[1]).rsplit(".", 1)[0]
        report_module((bank, mod), landed, parks, asm)
        return

    for a in args:
        if os.path.exists(os.path.join(ROOT, a)):
            key = module_of(a, up)
            if not key:
                print(f"\n{a}: no upstream module matched")
                continue
            print(f"\n{a}  ->  upstream {key[0]}/{key[1]}.s")
            report_module(key, landed, parks, asm)
            continue
        # treat as a function name: find the file defining or holding it
        found = None
        for table in (landed, parks, asm):
            for key, files in table.items():
                for f in files:
                    if a in funcs_in(f):
                        found = (a, f, key)
                        break
                if found:
                    break
            if found:
                break
        if not found:
            print(f"\n{a}: not found in any mapped file")
            continue
        _, f, key = found
        print(f"\n{a}  in {f}  ->  upstream {key[0]}/{key[1]}.s")
        report_module(key, landed, parks, asm)


if __name__ == "__main__":
    main()
