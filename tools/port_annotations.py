#!/usr/bin/env python3
"""port_annotations.py -- move our prose annotations onto this tree's asm/.

Our annotations were written against a tree that named everything
`Func_<offset>`. This one uses semantic names and carries `aliases.txt`, so
references inside the prose are rewritten where the alias is unambiguous.

Rules, in order of how much they matter:

  1. Only substitute when the alias is UNAMBIGUOUS. `aliases.txt` maps
     name -> address; if two names share an address, neither is used. A wrong
     substitution silently corrupts prose, which is worse than leaving a stale
     symbol name in place.
  2. Never touch a function that already has a comment block above it.
  3. Comments assemble to nothing, so this cannot change the ROM -- but the
     build is still verified afterwards, because "cannot" is not "did not".

Usage:
    python3 tools/port_annotations.py --dry-run [--limit N]
    python3 tools/port_annotations.py --apply [--limit N]
"""
import json, re, sys, os, glob
from collections import defaultdict

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def load_aliases():
    """address -> name, from the linked ELF's symbol table.

    NOT from aliases.txt -- that file says "residual symbol names, for
    reference only" and holds data symbols; it contains no function names at
    all, which is why an earlier version of this substituted nothing.

    The ELF is authoritative: it has every symbol the build actually produced,
    at its real address. Addresses carrying more than one name are skipped --
    a wrong substitution silently corrupts prose.
    """
    import subprocess
    by_addr = defaultdict(list)
    r = subprocess.run(["arm-none-eabi-nm", os.path.join(ROOT, "goldensun.elf")],
                       capture_output=True, text=True)
    for line in r.stdout.splitlines():
        f = line.split()
        if len(f) == 3 and f[1] in "tT":
            by_addr[int(f[0], 16) & 0xFFFFFF].append(f[2])
    # prefer a semantic name over a Func_/sub_ placeholder at the same address
    def pick(names):
        real = [n for n in names
                if not re.fullmatch(r"_?(?:Func|sub|Sub)_[0-9a-fA-F]+", n)]
        return real[0] if len(real) == 1 else None
    unambiguous = {}
    for a, names in by_addr.items():
        n = pick(names)
        if n:
            unambiguous[a] = n.lstrip("_")
    return unambiguous, by_addr


def rewrite(text, alias):
    """Func_xxxx -> semantic name where the alias is unambiguous."""
    subs = 0
    def repl(m):
        nonlocal subs
        a = int(m.group(1), 16)
        n = alias.get(a)
        if n and not re.fullmatch(r"(?:sub|Func|Sub)_[0-9a-fA-F]+", n):
            subs += 1
            return n
        return m.group(0)
    return re.sub(r"\bFunc_([0-9a-f]{2,6})\b", repl, text), subs


def their_functions():
    """address -> (symbol, path, line index of the .thumb_func_start)."""
    out = {}
    for p in glob.glob(os.path.join(ROOT, "asm", "**", "*.s"), recursive=True):
        L = open(p, errors="replace").read().split("\n")
        for i, l in enumerate(L):
            m = re.match(r"\s*\.thumb_func_start\s+(\w+)\s*@\s*(0x0?8[0-9a-fA-F]+)", l)
            if m:
                out[int(m.group(2), 16) & 0xFFFFFF] = (m.group(1), p, i)
    return out


def main():
    apply = "--apply" in sys.argv
    limit = None
    if "--limit" in sys.argv:
        limit = int(sys.argv[sys.argv.index("--limit") + 1])

    alias, by_addr = load_aliases()
    ambiguous = sum(1 for v in by_addr.values() if len(v) > 1)
    print(f"aliases: {len(alias)} unambiguous, {ambiguous} addresses skipped as ambiguous")

    ann = json.load(open(os.path.join(ROOT, "ANNOTATIONS.json")))
    ours = {}
    for items in ann.values():
        for it in items:
            if it["addr"] is not None and not it["overlay"]:
                ours[it["addr"]] = it["text"]

    theirs = their_functions()
    print(f"our main-ROM annotations: {len(ours)}   their asm/ functions: {len(theirs)}")

    # group work by file so each file is rewritten once
    per_file = defaultdict(list)
    matched = skipped_existing = 0
    total_subs = 0
    for addr, text in ours.items():
        if addr not in theirs:
            continue
        sym, path, idx = theirs[addr]
        per_file[path].append((idx, addr, sym, text))
        matched += 1

    done = 0
    for path in sorted(per_file):
        L = open(path, errors="replace").read().split("\n")
        ins = {}
        for idx, addr, sym, text in per_file[path]:
            j = idx - 1
            while j >= 0 and not L[j].strip():
                j -= 1
            if j >= 0 and L[j].lstrip().startswith("@"):
                skipped_existing += 1
                continue
            new, n = rewrite(text, alias)
            total_subs += n
            ins[idx] = ["@ " + x if x else "@" for x in new.split("\n")]
            done += 1
            if limit and done >= limit:
                break
        if ins and apply:
            out = []
            for i, line in enumerate(L):
                if i in ins:
                    out.extend(ins[i])
                out.append(line)
            open(path, "w").write("\n".join(out))
        if limit and done >= limit:
            break

    print(f"\nannotations placed: {done}")
    print(f"already annotated (left alone): {skipped_existing}")
    print(f"Func_xxxx references rewritten to semantic names: {total_subs}")
    print(f"files touched: {len(per_file) if apply else 0}")
    if not apply:
        print("\n(dry run -- pass --apply to write)")


if __name__ == "__main__":
    main()
