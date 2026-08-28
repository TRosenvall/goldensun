#!/usr/bin/env python3
"""structmap.py -- cross-reference the structs the elevated corpus declares.

WHY

274 elevated .c files declare a local struct, under names like A, S, E, Ent, Pk
and Actor, while only 93 include include/actor.h. Many of those describe the SAME
game object, discovered independently, one function at a time. That is duplicated
work in two directions: the same layout is re-derived, and a field pinned down in
one file is not available to the next.

This emits two tables into docs/structs.md.

  BY NAME    every struct NAME, how many DIFFERENT layouts have been declared
             under it, and the union of every field those layouts pin down. That
             union is the header worth building: `Actor` is declared 67 times and
             each declaration knows a few offsets, so the union knows all of them.

             Grouping by field offsets instead was tried first and is wrong -- it
             collapses unrelated objects (a DmaQueue and a SpriteSlot both have a
             2-byte field at 0) and it buries the actual finding, which is that
             one NAME has many layouts.

  BY FILE    every file that declares a struct, the shapes it uses, and the
             levers its own header comment names. The lever column is read from
             the file, not inferred, so it says what was actually needed rather
             than what might be.

Offsets are computed by walking the declaration: `unsigned char padN[0xNN]`
advances by its size, a typed field occupies its width. A shape is the tuple of
(offset, width) pairs for the NAMED fields only -- padding names differ between
files and carry no information.

    python3 tools/structmap.py            # regenerate docs/structs.md
    python3 tools/structmap.py --check    # print without writing
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, "src")

# Three declaration forms are in use, and the anonymous typedef is the most
# common (219 of them). Matching only `struct Name {` found 48 files where
# 274 declare one -- the undercount is the whole reason to parse rather than
# grep.
STRUCT = re.compile(r"(?:^|\n)\s*(?:typedef\s+)?struct\s*(\w*)\s*\{", re.M)
FIELD = re.compile(r"^\s*(?:const\s+)?(unsigned char|signed char|char|unsigned short|short|"
                   r"unsigned int|int|unsigned long|long|u8|s8|u16|s16|u32|s32|fx32|vec3_t|void)\s*"
                   r"(\*?)\s*(\w+)\s*(?:\[([^]]*)\])?\s*;", re.M)
WIDTH = {"unsigned char": 1, "signed char": 1, "char": 1, "u8": 1, "s8": 1,
         "unsigned short": 2, "short": 2, "u16": 2, "s16": 2,
         "unsigned int": 4, "int": 4, "u32": 4, "s32": 4,
         "unsigned long": 4, "long": 4, "fx32": 4, "vec3_t": 12, "void": 4}
LEVER = [
    (re.compile(r"name[d]? .{0,30}offset", re.I), "offset-named"),
    (re.compile(r"dominating block", re.I), "interleave-naming"),
    (re.compile(r"int intermediate", re.I), "int-intermediate"),
    (re.compile(r"CSE_CFLAGS", re.I), "CSE_CFLAGS"),
    (re.compile(r"LEFT UNDECLARED|drop\w* .{0,20}prototype", re.I), "no-prototype"),
    (re.compile(r"by value", re.I), "struct-by-value"),
    (re.compile(r"stack argument|spilled", re.I), "stack-pair-naming"),
    (re.compile(r"_AREA_|area\.sym", re.I), "area-symbol"),
    (re.compile(r"_MSG_|message\.sym", re.I), "message-symbol"),
    (re.compile(r"fakematch", re.I), "FAKEMATCH"),
]


def arrsize(expr):
    """Array sizes are written as expressions -- `pad[0x55 - 0x23 - 1]` -- so a
    literal-only match silently reads them as zero and every later offset in the
    struct comes out wrong. Evaluate the arithmetic, refuse anything else."""
    expr = expr.strip()
    if not expr:
        return None
    if not re.fullmatch(r"[0-9a-fA-FxX+\-*/() ]+", expr):
        return None
    try:
        return int(eval(expr, {"__builtins__": {}}, {}))
    except Exception:
        return None


def parse_structs(text):
    """[(name, [(field, offset, width)])] for each struct declared in text."""
    out = []
    for m in STRUCT.finditer(text):
        name, i, depth, body = m.group(1), m.end(), 1, []
        while i < len(text) and depth:
            if text[i] == "{":
                depth += 1
            elif text[i] == "}":
                depth -= 1
                if not depth:
                    break
            body.append(text[i])
            i += 1
        # an anonymous typedef names itself after the closing brace: `} Name;`
        tail = text[i:i + 64]
        tn = re.match(r"\}\s*(\w+)\s*;", tail)
        if not name and tn:
            name = tn.group(1)
        if not name:
            name = "<anon>"
        off, fields = 0, []
        for f in FIELD.finditer("".join(body)):
            ty, ptr, fname, arr = f.group(1), f.group(2), f.group(3), f.group(4)
            w = 4 if ptr else WIDTH.get(ty, 4)
            if arr is None:
                n = 1
            else:
                n = arrsize(arr)
                if n is None:
                    # an unparseable size makes every later offset a lie;
                    # drop the struct rather than publish wrong offsets
                    fields = None
                    break
            size = w * n
            if not fname.lower().startswith("pad"):
                fields.append((fname, off, size))
            off += size
        if fields:
            out.append((name, tuple(fields)))
    return out


def levers_in(text):
    return sorted({tag for rx, tag in LEVER if rx.search(text)})


def main():
    shapes, byfile = {}, []
    for root, _, files in os.walk(SRC):
        if "non_matching" in root:
            continue
        for fn in sorted(files):
            if not fn.endswith(".c"):
                continue
            p = os.path.join(root, fn)
            text = open(p, errors="replace").read()
            got = parse_structs(text)
            if not got:
                continue
            rel = os.path.relpath(p, ROOT)
            lv = levers_in(text)
            names = []
            for name, sig in got:
                key = tuple((o, w) for _, o, w in sig)
                shapes.setdefault(key, {"names": set(), "files": [], "fields": sig})
                shapes[key]["names"].add(name)
                shapes[key]["files"].append(rel)
                names.append(name)
            byfile.append((rel, names, lv))

    lines = []
    lines.append("# Struct map\n")
    lines.append("GENERATED by `tools/structmap.py` -- do not edit by hand; "
                 "rerun it instead.\n")

    # group by NAME: the same object described independently many times
    byname = {}
    for key, v in shapes.items():
        for n in v["names"]:
            e = byname.setdefault(n, {"layouts": set(), "files": set(), "fields": {}})
            e["layouts"].add(key)
            e["files"].update(v["files"])
            for fname, o, w in v["fields"]:
                e["fields"].setdefault(o, (fname, w))

    lines.append(f"{len(byfile)} elevated files declare a struct under "
                 f"{len(byname)} distinct names.\n")
    lines.append("A name declared under SEVERAL layouts is the same object "
                 "described independently,\nonce per function. The union column "
                 "is what a shared header for it would contain.\n")

    lines.append("\n## By name -- where a shared header would pay\n")
    lines.append("| name | layouts | files | union of fields pinned down (offset:width) |")
    lines.append("|---|---|---|---|")
    for n, e in sorted(byname.items(), key=lambda kv: (-len(kv[1]["layouts"]), kv[0])):
        if len(e["files"]) < 2:
            continue
        fl = ", ".join(f"{o:#x}:{w}" for o, (fn, w) in sorted(e["fields"].items())[:10])
        if len(e["fields"]) > 10:
            fl += ", ..."
        lines.append(f"| {n} | {len(e['layouts'])} | {len(e['files'])} | {fl} |")

    single = sum(1 for e in byname.values() if len(e["files"]) < 2)
    lines.append(f"\n{single} names are used by a single file and are omitted.\n")

    # the same LAYOUT under different names: one object, described twice
    byshape = {}
    for key, v in shapes.items():
        k = tuple((o, w) for _, o, w in v["fields"])
        e = byshape.setdefault(k, {"names": set(), "files": set()})
        e["names"].update(v["names"])
        e["files"].update(v["files"])

    lines.append("\n## Same layout, different name -- one object described twice\n")
    lines.append("Only layouts pinning down TWO OR MORE fields are listed. A single "
                 "shared field\nis not evidence: five unrelated structs each having "
                 "one 4-byte field at offset 0\ncollide by coincidence, not because "
                 "they are the same object.\n")
    lines.append("| files | names given to it | fields |")
    lines.append("|---|---|---|")
    for k, e in sorted(byshape.items(), key=lambda kv: -len(kv[1]["files"])):
        if len(e["names"]) < 2 or len(k) < 2:
            continue
        fl = ", ".join(f"{o:#x}:{w}" for o, w in k[:8])
        if len(k) > 8:
            fl += ", ..."
        lines.append(f"| {len(e['files'])} | {', '.join(sorted(e['names']))} | {fl} |")

    lines.append("\n## By file -- structs declared and levers the file records\n")
    lines.append("| file | structs | levers named in its header |")
    lines.append("|---|---|---|")
    for rel, names, lv in byfile:
        lines.append(f"| `{rel}` | {', '.join(names)} | {', '.join(lv) or '-'} |")

    out = "\n".join(lines) + "\n"
    if "--check" in sys.argv:
        print(out[:3000])
    else:
        with open(os.path.join(ROOT, "docs", "structs.md"), "w") as f:
            f.write(out)
        shared = sum(1 for e in byname.values() if len(e["files"]) > 1)
        print(f"docs/structs.md: {len(byfile)} files, {len(byname)} names, "
              f"{shared} declared by 2+ files")
    return 0


if __name__ == "__main__":
    sys.exit(main())
