#!/usr/bin/env python3
"""park_bodies.py -- which parks actually DEFINE a body for a function?

This answers the one question you must settle before crossing two parks:
does the other park contain a RIVAL BODY, or does it merely mention the
function?  Getting it wrong costs an agent a round, and in batch 324 it cost
THREE, because the coordinator's ad-hoc detector counted a top-level

    extern int Func_8019944(int key, int flag);

as a definition.  Briefs C, D and G were each told to "measure both bodies and
cross them" against a park that declares their function and defines something
else.  Brief G's "other park" defines a different 248-encoding function that
merely CALLS the target and happens to share a reference .s.

    python3 tools/park_bodies.py Func_8019944 HeightTile_B ...
    python3 tools/park_bodies.py --all            # every multi-body function
    python3 tools/park_bodies.py --plan FILE.json # check a batch plan's claims

WHY A DECLARATION IS EASY TO MISTAKE FOR A DEFINITION.  Both start at column 0
with a type, both carry the function name followed by `(`, and both end the
line with `;` or `{` only AFTER a parameter list that may wrap across lines.
A regex anchored on `^type name (` matches both.  The discriminator is what
follows the CLOSING paren -- `{` is a definition, `;` is a declaration -- and
finding the closing paren means balancing parens, not matching a line.

THE FOUR OUTCOMES for a function with two real bodies, all four observed:
  1. true redundancy -- the bodies are md5-identical;
  2. a BODYLESS PHANTOM claiming the better figure (two of the first three
     instances found; this tool exists to make that visible for free);
  3. a stale pair -- one predates a landed dependency;
  4. two PARTIAL answers that need CROSSING -- batch 323, worth 18 of 20.
Only outcome 4 rewards crossing, and only outcomes 1-4 are possible at all
when there really are two bodies.  A phantom is not an outcome, it is a
mis-read.
"""
import glob, json, os, re, sys, hashlib

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def strip_comments(s):
    s = re.sub(r"/\*.*?\*/", lambda m: "\n" * m.group(0).count("\n"), s, flags=re.S)
    return re.sub(r"//[^\n]*", "", s)


def definitions(src):
    """Return {name: line} for every function DEFINED in this C text.

    A definition is a name followed by a balanced parameter list and then `{`.
    Balancing the parens is what separates this from the regex that caused the
    batch-324 confusion: a declaration's closing paren is followed by `;`.
    """
    out = {}
    code = strip_comments(src)
    for m in re.finditer(r"\b([A-Za-z_]\w*)\s*\(", code):
        name = m.group(1)
        if name in ("if", "for", "while", "switch", "return", "sizeof", "do"):
            continue
        i = m.end() - 1
        depth = 0
        while i < len(code):
            if code[i] == "(":
                depth += 1
            elif code[i] == ")":
                depth -= 1
                if depth == 0:
                    break
            i += 1
        else:
            continue
        # THE DEFINITION MUST START AT COLUMN 0.  Without this, an indented CALL
        # whose result feeds an expression is mistaken for a K&R definition:
        # BufferString.c:435 has
        #     DecompressString(Func_8019944(4, mode) + 0x333, strbuf, 0x18);
        # and a parser that looks past `)` for the next `{` finds the following
        # block and reports a body.  That false positive contradicted brief C's
        # own reading of the same file, which was correct.  Every function
        # definition in this corpus begins at column 0; every call is indented.
        ls = code.rfind("\n", 0, m.start()) + 1
        if code[ls:m.start()] != code[ls:m.start()].lstrip():
            continue
        j = i + 1
        while j < len(code) and code[j] in " \t\r\n":
            j += 1
        if j < len(code) and code[j] == "{":
            out[name] = code.count("\n", 0, m.start()) + 1
    return out


def body_hash(path, name):
    src = strip_comments(open(path, errors="replace").read())
    m = re.search(r"\b" + re.escape(name) + r"\s*\(", src)
    if not m:
        return None
    i = src.find("{", m.end())
    if i < 0:
        return None
    depth, j = 0, i
    while j < len(src):
        if src[j] == "{":
            depth += 1
        elif src[j] == "}":
            depth -= 1
            if depth == 0:
                break
        j += 1
    return hashlib.md5(" ".join(src[i:j + 1].split()).encode()).hexdigest()[:12]


def scan():
    parks, defs, mentions = {}, {}, {}
    for p in sorted(glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"), recursive=True)):
        rel = os.path.relpath(p, ROOT)
        src = open(p, errors="replace").read()
        parks[rel] = src
        for name, line in definitions(src).items():
            defs.setdefault(name, []).append((rel, line))
        code = strip_comments(src)
        for name in set(re.findall(r"\b([A-Za-z_]\w*)\s*\(", code)):
            mentions.setdefault(name, []).append(rel)
    return parks, defs, mentions


def report(names, defs, mentions):
    bad = 0
    for name in names:
        d = defs.get(name, [])
        m = [f for f in mentions.get(name, []) if f not in [x[0] for x in d]]
        print(f"\n=== {name} ===")
        if not d:
            print("  NO PARK DEFINES A BODY for it (it may have landed, or be triage-only)")
        for rel, line in d:
            print(f"  BODY    {rel}:{line}   md5={body_hash(os.path.join(ROOT, rel), name)}")
        for rel in m:
            print(f"  mention {rel}   <- DECLARES OR CALLS ONLY, nothing to cross")
        if len(d) > 1:
            hs = {body_hash(os.path.join(ROOT, r), name) for r, _ in d}
            print(f"  >>> {len(d)} RIVAL BODIES."
                  f"  {'IDENTICAL (outcome 1, true redundancy)' if len(hs) == 1 else 'DIFFERENT -- measure both, then cross with tools/crossfire.py'}")
        elif len(d) == 1 and m:
            print("  >>> ONE body. The other file(s) are PHANTOMS -- do not cross, do not retire them for this function.")
            bad += 1
    return bad


def main():
    args = sys.argv[1:]
    parks, defs, mentions = scan()
    if not args:
        print(__doc__)
        sys.exit(2)
    if args[0] == "--all":
        multi = sorted(n for n, v in defs.items() if len(v) > 1)
        print(f"{len(parks)} parks scanned; {len(defs)} functions defined in them")
        print(f"functions with MORE THAN ONE park body: {len(multi)}")
        report(multi, defs, mentions)
    elif args[0] == "--plan":
        plan = json.load(open(args[1]))
        names = []
        for v in (plan.values() if isinstance(plan, dict) else plan):
            for t in (v.get("targets", []) if isinstance(v, dict) else [v]):
                names.append(t["func"] if isinstance(t, dict) else t)
        n = report(names, defs, mentions)
        print(f"\n{n} target(s) whose 'other park' is a PHANTOM -- fix the brief before sending it.")
    else:
        report(args, defs, mentions)


if __name__ == "__main__":
    main()
