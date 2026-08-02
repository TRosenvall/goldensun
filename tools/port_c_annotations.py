#!/usr/bin/env python3
"""port_c_annotations.py -- put our prose on the functions already in C.

The two earlier porters targeted `.s` files. What is left are annotations for
functions this tree has already decompiled, so the anchor is a C function
definition rather than a `.thumb_func_start`, and the comment syntax is C.

Mapping runs address -> symbol -> definition site:

  * the linked ELF gives symbol -> address for everything actually built
  * our annotations are keyed by address
  * the definition is found by scanning src/ for `<name>(` followed by a brace

Care taken:

  * their .c files already open with a generated header describing the TU's
    layout. That header is left in place; our prose is inserted immediately
    above the function it describes, which in a multi-function TU is not the
    top of the file.
  * a function that already carries a block comment directly above it is left
    alone.
  * `Func_<offset>` references in the prose are rewritten to this tree's
    semantic names where unambiguous, matching what the .s porters did.
"""
import json, re, sys, os, glob, subprocess
from collections import defaultdict

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
from port_annotations import load_aliases, rewrite


def elf_addr_to_name():
    out = {}
    r = subprocess.run(["arm-none-eabi-nm", os.path.join(ROOT, "goldensun.elf")],
                       capture_output=True, text=True)
    for line in r.stdout.splitlines():
        f = line.split()
        if len(f) == 3 and f[1] in "tT":
            out.setdefault(int(f[0], 16) & 0xFFFFFF, []).append(f[2])
    # Most "ambiguous" addresses are not really ambiguous: a function shares its
    # address with a section label (Exports_c0) or the end-marker of the
    # preceding region (DecompressLZ1_ROM_End). Discard those, then accept the
    # address if exactly one real name is left. Anything still plural is
    # genuinely undecidable and skipped.
    def real(names):
        keep = [n for n in names
                if not n.endswith("_End")
                and not n.startswith("Exports_")
                and not n.endswith("_ROM_End")]
        # Func_X and _Func_X are the same symbol wearing an export alias
        uniq = {n.lstrip("_") for n in keep}
        return keep, uniq
    resolved = {}
    for a, names in out.items():
        keep, uniq = real(names)
        if len(uniq) == 1:
            resolved[a] = keep[0].lstrip("_")
    return resolved


def c_definitions():
    """symbol -> (path, line index of the definition's first line)."""
    out = {}
    for p in glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True):
        L = open(p, errors="replace").read().split("\n")
        for i, l in enumerate(L):
            # a definition: name(...) with a brace on this line or the next
            m = re.match(r"^\s*(?:[A-Za-z_]\w*[\s*]+)+?(\w+)\s*\([^;]*$", l)
            if not m:
                continue
            nxt = L[i+1] if i + 1 < len(L) else ""
            if "{" in l or "{" in nxt or l.rstrip().endswith(")"):
                out.setdefault(m.group(1), (p, i))
    return out


def main():
    apply = "--apply" in sys.argv

    alias, _ = load_aliases()
    addr2name = elf_addr_to_name()
    defs = c_definitions()

    # MAIN ROM ONLY. Overlay annotations are keyed by offset-within-overlay
    # (0x30, 0xC4, ...), which collide with low main-ROM addresses. Mixing them
    # here made 2296 annotations look unmappable when they were simply being
    # looked up in the wrong address space.
    ann = json.load(open(os.path.join(ROOT, "ANNOTATIONS.json")))
    ours = {}
    for items in ann.values():
        for it in items:
            if it["addr"] is not None and not it["overlay"]:
                ours.setdefault(it["addr"], it["text"])

    print(f"our annotations: {len(ours)}")
    print(f"ELF single-symbol addresses: {len(addr2name)}")
    print(f"C definitions found in src/: {len(defs)}")

    per_file = defaultdict(list)
    no_symbol = no_def = named_already = 0
    for addr, text in ours.items():
        name = addr2name.get(addr)
        if not name:
            no_symbol += 1
            continue
        # ONLY annotate functions this tree has NOT already named.
        #
        # Where they have a semantic name, theirs wins and ours must not be
        # pasted alongside it. Measured on 100 addresses where both sides
        # expressed a recognisable domain, 35 disagreed -- and spot-checking
        # showed the pattern: our prose gets the MECHANISM right and the
        # PURPOSE wrong. 0x543c is described accurately as "allocate a scratch,
        # DMA3-copy a routine into it, call it there" but labelled
        # decompression, when their BlitFade_Add (and the BlitFade_Add_ROM
        # twin at 0x1fb8) shows it is a blit. 0x6910 is worse: our prose
        # describes sound channels on what is plainly IdentifyFlash.
        #
        # That is what comes of characterising functions by shape rather than
        # by tracing them. The prose still has value on functions nobody has
        # named, which is where the .s porters put it.
        if not re.fullmatch(r"(?:Func|sub|Sub)_[0-9a-fA-F]+", name):
            named_already += 1
            continue
        d = defs.get(name)
        if not d:
            no_def += 1
            continue
        per_file[d[0]].append((d[1], name, text))

    placed = skipped = subs = 0
    for path in sorted(per_file):
        L = open(path, errors="replace").read().split("\n")
        ins = {}
        for idx, name, text in per_file[path]:
            j = idx - 1
            while j >= 0 and not L[j].strip():
                j -= 1
            if j >= 0 and (L[j].rstrip().endswith("*/") or
                           L[j].lstrip().startswith("//")):
                skipped += 1
                continue
            new, n = rewrite(text, alias)
            subs += n
            lines = new.split("\n")
            # DROP our first line when it is just our name for the function.
            # This tree already has a name, and theirs is better sourced --
            # BlitFade_Div2_ROM, DecompressLZ1_ROM, cam4aSoundMain read like
            # recovered SDK symbols where ours were inferred from reading the
            # code. Ours disagreed with theirs at 506 of 515 shared addresses;
            # mostly naming convention, but at 0x6910 we had called
            # IdentifyFlash "LoadInstrumentForChannel", which is simply wrong.
            # The behavioural prose below the name line is what has value here.
            if re.fullmatch(r"[A-Za-z_]\w{2,50}", lines[0].strip()):
                lines = lines[1:]
                while lines and not lines[0].strip():
                    lines = lines[1:]
            if not lines:
                skipped += 1
                continue
            block = ["/* " + lines[0]] + \
                    [" * " + x if x else " *" for x in lines[1:]] + \
                    [" */"]
            ins[idx] = block
            placed += 1
        if ins and apply:
            out = []
            for i, line in enumerate(L):
                if i in ins:
                    out.extend(ins[i])
                out.append(line)
            open(path, "w").write("\n".join(out))

    print(f"\nannotations placed: {placed}")
    print(f"already commented (left alone): {skipped}")
    print(f"no single ELF symbol at that address: {no_symbol}")
    print(f"already named by this tree (theirs wins, skipped): {named_already}")
    print(f"symbol found but no C definition (still assembly): {no_def}")
    print(f"references rewritten to semantic names: {subs}")
    print(f"files touched: {len(per_file) if apply else 0}")
    if not apply:
        print("\n(dry run -- pass --apply to write)")


if __name__ == "__main__":
    main()
