#!/usr/bin/env python3
"""shimcount.py -- count the shims in a candidate's CODE, not in its prose.

    python3 tools/shimcount.py src/rom_c0/rom_49a8_b.c ...
    python3 tools/shimcount.py --all          # every landed .c, flagging unbooked shims

WHY THIS EXISTS. A shim count decides whether a landed file needs a `fakematch.txt` row,
and getting it wrong in either direction is a real defect: an unbooked shim is an
undocumented fakematch, and a phantom one books a row for a file that does not need it.

Counting with grep does not work, and this is not a hypothetical. In one session SIX
separate counts were wrong, every time for one of three reasons:

  1. A park or landing header DESCRIBES its shims ("SHIMS: one `register ... __asm__`
     declaration") or tabulates every spelling tried. A file-wide grep counts that prose.
     Stripping only the LEADING comment is not enough -- these headers run to several
     blocks, and the tables are usually in a later one.
  2. A header sentence saying there are NO shims matches a grep for shims. Twice a file
     reported "0 pins" and grep answered 1, which was the sentence itself.
  3. Pins hide in MACROS. `#define PIN4 PIN3; register int q3 __asm__("r3")` puts the
     declaration on a `#define` line, so an anchored `^\\s*register` pattern finds none
     while the function body is full of `{ PIN4; ... }`. One file read as 0 pins and
     actually carries nine pinned blocks plus three standalone declarations.

So: strip EVERY block comment, then expand the PIN macros' own arity, then count.

THE TWO CLASSES ARE COUNTED SEPARATELY because they are different objects. A
`register ... __asm__` declaration pins a hard register. An `__asm__(".equ NAME, VALUE")`
declares a symbol and is usually a MEASUREMENT shim that must be stripped before landing --
`message.sym` or `area.sym` is where the row belongs. Eight of those were caught in files
reported as having none.

An empty `__asm__ volatile ("")` is reported separately again, and deliberately: the tree
does NOT treat it as a fakematch (5 of 6 landed files using one carry no row), whereas
`__asm__ ("" : "+r" (x))` IS classed as one at docs/elevation.md:4159.
"""
import os, re, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PIN_DEF = re.compile(r"^\s*#define\s+(PIN\d)\b(.*)$", re.M)
DECL    = re.compile(r"register\s+[^;=]*?__asm__\s*\(\s*\"(\w+)\"\s*\)")
EQU     = re.compile(r"__asm__\s*(?:volatile\s*)?\(\s*\"\.equ\s+([A-Za-z_]\w*)")
BARRIER = re.compile(r"__asm__\s*volatile\s*\(\s*\"\"\s*\)")
PLUSR   = re.compile(r"__asm__\s*(?:volatile\s*)?\(\s*\"\"\s*:\s*\"\+r\"")


def strip_comments(s):
    """Every block comment, and line comments too -- not just the leading header."""
    s = re.sub(r"/\*.*?\*/", "", s, flags=re.S)
    return re.sub(r"//[^\n]*", "", s)


def count(path):
    src = open(path, errors="replace").read()
    code = strip_comments(src)

    # how many registers each PIN macro reserves, resolved through its own chaining
    arity = {}
    for name, body in PIN_DEF.findall(code):
        arity[name] = len(DECL.findall(body)) + sum(
            arity.get(t, 0) for t in re.findall(r"\bPIN\d\b", body))

    body = PIN_DEF.sub("", code)          # drop the #define lines themselves
    pins = len(DECL.findall(body))        # standalone declarations
    blocks = []
    for m in re.finditer(r"\bPIN(\d)\b", body):
        nm = "PIN" + m.group(1)
        blocks.append(nm)
        pins += arity.get(nm, int(m.group(1)))
    return {
        "pins": pins, "pin_blocks": blocks,
        "equ": EQU.findall(body),
        "barriers": len(BARRIER.findall(body)),
        "plus_r": len(PLUSR.findall(body)),
    }


def booked(func_or_path):
    fm = os.path.join(ROOT, "fakematch.txt")
    if not os.path.exists(fm):
        return False
    return any(func_or_path in l for l in open(fm, errors="replace"))


def main():
    args = [a for a in sys.argv[1:] if a != "--all"]
    if "--all" in sys.argv:
        args = []
        for root, _d, fs in os.walk(os.path.join(ROOT, "src")):
            if "non_matching" in root:
                continue
            args += [os.path.join(root, f) for f in sorted(fs) if f.endswith(".c")]
    if not args:
        sys.exit(__doc__.strip().splitlines()[0] +
                 "\n\nusage: shimcount.py <file.c>... | --all")

    problems = 0
    for p in args:
        r = count(p)
        rel = os.path.relpath(p, ROOT)
        total = r["pins"] + len(r["equ"]) + r["plus_r"]
        if "--all" in sys.argv and total == 0 and not r["barriers"]:
            continue
        print(f"{rel}")
        if r["pins"]:
            print("    register pins : %d%s" % (
                r["pins"], ("  via " + ", ".join(r["pin_blocks"])) if r["pin_blocks"] else ""))
        if r["equ"]:
            print("    .equ shims    : %d  (%s)  <-- MEASUREMENT ONLY; must not land"
                  % (len(r["equ"]), ", ".join(r["equ"])))
            problems += 1
        if r["plus_r"]:
            print('    "+r" barriers : %d  (classed as a fakematch)' % r["plus_r"])
        if r["barriers"]:
            print("    empty asm     : %d  (NOT treated as a fakematch in this tree)"
                  % r["barriers"])
        if (r["pins"] or r["plus_r"]) and "non_matching" not in rel and not booked(rel):
            print("    *** has a fakematch-class shim and NO fakematch.txt row")
            problems += 1
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())
