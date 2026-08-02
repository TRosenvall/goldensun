#!/usr/bin/env python3
"""checkfuncs.py -- per-function pass/fail against baserom.gba.

A whole-ROM diff is useless during a toolchain migration: one function that
compiles four bytes too long displaces everything after it, and a handful of
real faults look like 500 KB of breakage.

This compares each function INDIVIDUALLY. Our naming convention encodes the
address (Func_488c lives at 0x488c), so the ROM's bytes for a function can be
read directly, and the built ELF's bytes for the same symbol compared against
them -- regardless of where the link actually placed it.

Usage (inside the build container):
    python3 tools/checkfuncs.py [--verbose] [--only PREFIX]
"""
import re, subprocess, sys, os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ELF = os.path.join(ROOT, "goldensun.elf")
ROM = os.path.join(ROOT, "baserom.gba")


def c_defined_funcs():
    """Func_xxxx symbols we build from C, mapped to their .c file."""
    out = {}
    for dirpath, _, files in os.walk(ROOT):
        if "/tools/" in dirpath or "/.git" in dirpath:
            continue
        for fn in files:
            if not fn.endswith(".c"):
                continue
            p = os.path.join(dirpath, fn)
            try:
                src = open(p, errors="replace").read()
            except OSError:
                continue
            # a definition, not a declaration: name(...) followed by a brace
            for m in re.finditer(r"\b(Func_[0-9a-f]+)\s*\([^;{]*\)\s*\{", src):
                out[m.group(1)] = os.path.relpath(p, ROOT)
    return out


def elf_symbols():
    """name -> (vaddr, size) from the linked ELF."""
    r = subprocess.run(["arm-none-eabi-nm", "-S", ELF],
                       capture_output=True, text=True)
    syms = {}
    for line in r.stdout.splitlines():
        f = line.split()
        if len(f) == 4 and f[3].startswith("Func_"):
            syms[f[3]] = (int(f[0], 16), int(f[1], 16))
    return syms


def elf_bytes(vaddr, size):
    """Read size bytes at vaddr out of the ELF's loaded image."""
    r = subprocess.run(
        ["arm-none-eabi-objcopy", "-O", "binary",
         "--only-section=.text", ELF, "/tmp/_elf.bin"],
        capture_output=True, text=True)
    return None  # replaced below; kept for clarity


def main():
    verbose = "--verbose" in sys.argv
    only = None
    if "--only" in sys.argv:
        only = sys.argv[sys.argv.index("--only") + 1]

    if not os.path.exists(ELF):
        print("goldensun.elf missing -- run make first", file=sys.stderr)
        return 2

    rom = open(ROM, "rb").read()
    # Flatten the ELF once: objcopy to a raw image keyed by ROM offset.
    subprocess.run(["arm-none-eabi-objcopy", "-O", "binary", ELF, "/tmp/_built.bin"],
                   check=True)
    built = open("/tmp/_built.bin", "rb").read()

    cfuncs = c_defined_funcs()
    syms = elf_symbols()

    ok, bad, missing = [], [], []
    for name, cfile in sorted(cfuncs.items()):
        if only and not cfile.startswith(only):
            continue
        want_addr = int(name.split("_")[1], 16)
        if name not in syms:
            missing.append((name, cfile))
            continue
        vaddr, size = syms[name]
        got_off = vaddr & 0xFFFFFF
        if size == 0:
            missing.append((name, cfile))
            continue
        want = rom[want_addr:want_addr + size]
        got = built[got_off:got_off + size]
        if want == got:
            ok.append((name, cfile))
        else:
            nd = sum(1 for x, y in zip(want, got) if x != y)
            bad.append((name, cfile, size, nd, got_off != want_addr))

    print(f"matching   {len(ok)}")
    print(f"differing  {len(bad)}")
    if missing:
        print(f"no symbol  {len(missing)}")
    if bad:
        print()
        print(f"{'function':<16} {'size':>5} {'bad':>5}  moved  source")
        for name, cfile, size, nd, moved in sorted(bad, key=lambda r: -r[3]):
            print(f"{name:<16} {size:>5} {nd:>5}  {'yes' if moved else ' - ':>5}  {cfile}")
    if verbose and missing:
        print("\nno symbol in ELF:")
        for name, cfile in missing:
            print(f"  {name:<16} {cfile}")
    return 0 if not bad else 1


if __name__ == "__main__":
    sys.exit(main())
