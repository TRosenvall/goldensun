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
# ANY macro whose body declares pins, not just ones literally named PIN<digit>.
# This used to be `(PIN\d)`, so a file naming its macros R0..R3 reported FOUR pins
# when it carried NINETY-SIX across 36 blocks -- batch 341 brief D found it after
# I briefed that file as "4 pins".  The pin count is the authoritative input to
# target selection AND to the directory's headline total, so an under-report here
# propagates everywhere.
#
# Matched by SHAPE: a #define whose body contains a `register ... __asm__("rN")`
# declaration. That cannot over-match, because a macro without such a declaration
# contributes no pins however it is named.
PIN_DEF = re.compile(r"^\s*#define\s+(\w+)\b(.*(?:\\\n.*)*)$", re.M)
DECL    = re.compile(r"register\s+[^;=]*?__asm__\s*\(\s*\"(\w+)\"\s*\)")
EQU     = re.compile(r"__asm__\s*(?:volatile\s*)?\(\s*\"\.equ\s+([A-Za-z_]\w*)")
# Three spellings of "a volatile asm barrier" reach THREE DIFFERENT passes, decided
# by the RTX code of the insn's pattern -- see docs/elevation.md, "A BARE BARRIER
# DOES NOT FLUSH cse's TABLE".  They are counted separately because they are
# different devices carrying different pass-3 debt, and because counting only the
# first (which this tool did until batch 336) reports zero pins for a body that
# ships either of the other two.
BARRIER = re.compile(r"__asm__\s*volatile\s*\(\s*\"\"\s*\)")              # ASM_INPUT: sched only
CLOBBER = re.compile(r"__asm__\s*volatile\s*\(\s*\"\"\s*:{3}\s*\"memory\"") # PARALLEL: + combine
PLUSR   = re.compile(r"__asm__\s*(?:volatile\s*)?\(\s*\"\"\s*:\s*\"\+r\"")
# AN UNDEFINED EXTERN TAKEN BY ADDRESS IS A DEVICE, and it was a blind spot.
# This tool counted register-asm declarations, `.equ` shims and empty
# `asm volatile` -- but batch 342 brief E found a park reported as 0 PINS AND 0
# DEVICES that carries `extern int _FILE_18;` read as `(int)&_FILE_18`, with
# _FILE_18 defined NOWHERE in the tree: not in file_table.sym, not anywhere.
# That stands in for a pooled constant and is worth 160 encodings on that park,
# so a clean shimcount row there was badly misleading.
#
# It is exactly the class the owner's symbol-table ruling governs, so it must be
# visible. Detected as: an `extern` declaration whose symbol is taken by ADDRESS
# in the same file and which no `.sym` file defines.
EXTERN_DECL = re.compile(r"^\s*extern\s+[\w \t\*]+?\b(\w+)\s*(?:\[[^\]]*\])?\s*;", re.M)


def sym_defined():
    """Every symbol the tree DEFINES anywhere -- .sym values, asm labels and
    .globals, and C definitions (cached).

    CHECKING ONLY .sym FILES WAS WRONG and flagged 26 landed files on its first
    run.  A symbol can perfectly well be defined as a LABEL in asm/ --
    gOvl_0200e3f4, gScript_882__0200cec8, ActorCmd_ARRAY_918__02009c00 are all
    real overlay data -- and those are not devices.  The decisive argument is
    that a LANDED file's externs all resolve by definition, because the build
    links; so anything flagged in one is a false positive by construction.

    Only a symbol defined NOWHERE is the device class: it cannot link, which is
    precisely why it only ever appears in a park, standing in for a constant.
    """
    global _SYMDEF
    try:
        return _SYMDEF
    except NameError:
        pass
    import glob as _g
    _SYMDEF = set()
    for s in _g.glob(os.path.join(ROOT, "*.sym")):
        for line in open(s, errors="replace"):
            line = line.split("/*")[0].split("//")[0].strip()
            m = re.match(r"([A-Za-z_]\w*)\s*=", line)
            if m:
                _SYMDEF.add(m.group(1))
    # SYMBOLS DEFINED THROUGH AN ASSEMBLER MACRO.  This is the class that made the
    # first two versions of this check wrong, twice.  `.incdata Data_80ec8, ...`
    # expands (include/macros.inc:46-51) to `.global \sym` plus `\sym:`, so
    # Data_80ec8 IS defined -- and batch 343 brief E found this tool calling it an
    # undefined-extern DEVICE on a park whose LANDED sibling takes the same address
    # the same way.  Batch 342 had already hit it once with _FIXUP_RAM_CODE_SIZE
    # from `.func_end_emit_size`, and restricting the class to parks only hid the
    # defect rather than fixing it.
    #
    # So parse macros.inc for which PARAMETER POSITIONS a macro turns into a
    # symbol, then read those arguments out of every invocation.  Note
    # .func_end_emit_size globalises its SECOND parameter, so a position-blind
    # rule would miss it.
    macsym = {}
    mi = os.path.join(ROOT, "include/macros.inc")
    if os.path.exists(mi):
        for blk in re.finditer(r"^\s*\.macro\s+(\.?\w+)([^\n]*)\n(.*?)^\s*\.endm",
                               open(mi, errors="replace").read(), re.M | re.S):
            name, params, body = blk.group(1), blk.group(2), blk.group(3)
            names = [x.split(":")[0].split("=")[0].strip()
                     for x in params.split(",") if x.strip()]
            idxs = {i for i, pn in enumerate(names)
                    if re.search(r"\.glob(?:a)?l\s+\\" + re.escape(pn) + r"\b", body)
                    or re.search(r"^\\" + re.escape(pn) + r"\s*:", body, re.M)}
            if idxs:
                macsym[name.lstrip(".")] = idxs
    inv = re.compile(r"^\s*\.(\w+)\s+([^\n]+)$", re.M)
    lbl = re.compile(r"^\s*(?:\.global\s+|\.globl\s+)?([A-Za-z_][\w.]*)\s*:", re.M)
    glb = re.compile(r"^\s*\.glob(?:a)?l\s+([A-Za-z_][\w.]*)", re.M)
    for s in _g.glob(os.path.join(ROOT, "asm/**/*.s"), recursive=True):
        txt = open(s, errors="replace").read()
        _SYMDEF.update(lbl.findall(txt))
        _SYMDEF.update(glb.findall(txt))
        for m in inv.finditer(txt):
            want = macsym.get(m.group(1))
            if not want:
                continue
            args = [a.strip() for a in m.group(2).split(",")]
            for i in want:
                if i < len(args) and re.match(r"^[A-Za-z_]\w*$", args[i]):
                    _SYMDEF.add(args[i])
    cdef = re.compile(r"^[A-Za-z_][\w \t\*]*?\b(\w+)\s*(?:\([^;{]*?\)\s*\{|\[|=)", re.M)
    for s in _g.glob(os.path.join(ROOT, "src/**/*.c"), recursive=True):
        if "non_matching" in s:
            continue
        _SYMDEF.update(cdef.findall(open(s, errors="replace").read()))
    return _SYMDEF
# a bare ASM_OPERANDS pattern -- the only form that reaches cse.c:5742-5745.
# Matches an operand list that is NOT "+r" (counted above) and NOT ::: "memory".
OPERAND = re.compile(r"__asm__\s*volatile\s*\(\s*\"\"\s*:(?!::)\s*(?!\"\+r\")[^;]*?\)\s*;")


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
        own = len(DECL.findall(body))
        chained = sum(arity.get(t, 0) for t in re.findall(r"\b\w+\b", body)
                      if t in arity)
        if own or chained:
            arity[name] = own + chained

    body = PIN_DEF.sub("", code)          # drop the #define lines themselves
    pins = len(DECL.findall(body))        # standalone declarations
    blocks = []
    for m in re.finditer(r"\b(\w+)\b", body):
        nm = m.group(1)
        if nm in arity:
            blocks.append(nm)
            pins += arity[nm]
    return {
        "pins": pins, "pin_blocks": blocks,
        "equ": EQU.findall(body),
        "barriers": len(BARRIER.findall(body)),
        "clobber": len(CLOBBER.findall(body)),
        "operand": len(OPERAND.findall(body)),
        # NOT added into `pins`: brief D found the single `"+r"` asm being counted
        # once as a barrier and again as a declaration.
        "plus_r": len(PLUSR.findall(body)),
        # ONLY A PARK CAN CARRY THIS DEVICE, and that is the decisive test rather
        # than any definition index.  A LANDED file's externs all resolve by
        # construction -- the build links -- so a flag there is a false positive
        # whatever the index says.  Two rounds of widening the index proved the
        # point the hard way: .sym values alone flagged 26 landed files, adding
        # asm labels and C definitions left 5, and those 5 are symbols defined
        # through assembler MACROS (_FIXUP_RAM_CODE_SIZE comes from
        # `.func_end_emit_size` at asm/rom_c0/rom_2544.s:619).  Chasing definition
        # syntaxes is the wrong move; the link is the authority.
        "addr_dev": sorted({n for n in EXTERN_DECL.findall(body)
                            if "non_matching" in path   # see sym_defined()
                            and re.search(r"&\s*" + re.escape(n) + r"\b", body)
                            and n not in sym_defined()}),
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
        total = (r["pins"] + len(r["equ"]) + r["plus_r"]
                 + r["clobber"] + r["operand"] + len(r["addr_dev"]))
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
        if r["clobber"]:
            print('    asm + memory  : %d  (PARALLEL -- fences combine, NOT cse;'
                  ' fakematch-class)' % r["clobber"])
        if r["addr_dev"]:
            print('    addr device   : %d  (%s)  <-- UNDEFINED extern taken by ADDRESS;'
                  ' stands in for a constant, and no .sym defines it'
                  % (len(r["addr_dev"]), ", ".join(r["addr_dev"])))
            problems += 1
        if r["operand"]:
            print('    asm + operand : %d  (bare ASM_OPERANDS -- the only form that'
                  ' flushes cse; fakematch-class)' % r["operand"])
        if ((r["pins"] or r["plus_r"] or r["clobber"] or r["operand"])
                and "non_matching" not in rel and not booked(rel)):
            print("    *** has a fakematch-class shim and NO fakematch.txt row")
            problems += 1
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())
