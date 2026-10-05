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
    """Map every module to the tree files that implement it."""
    up = upstream_modules()
    landed, parks, asm = collections.defaultdict(list), collections.defaultdict(list), collections.defaultdict(list)
    for f in _ls("HEAD", "src/", ".c"):
        m = module_of(f, up)
        if not m:
            continue
        (parks if "/non_matching/" in f else landed)[m].append(f)
    for f in _ls("HEAD", "asm/", ".s"):
        m = module_of(f, up)
        if m:
            asm[m].append(f)
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
