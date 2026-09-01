#!/usr/bin/env python3
"""name_evidence.py -- collect naming evidence for every function we elevated.

WHY THIS EXISTS

Renaming is deferred until elevation is finished, which is the right call:
function names do not survive into the ROM (objcopy -O binary drops the symbol
table), so a bulk rename is byte-neutral and `make compare` proves it. Names
chosen with a whole overlay in view beat names chosen one function at a time.

What is NOT free to defer is the EVIDENCE.

Two things consume it as we work. Our own reading of a function lands as prose
in a .c header, one file at a time. And the ROM's own annotation for an overlay
function lives in the `.s` comment block -- which elevating that function
DELETES. Both are recoverable from git, neither is usable as a table.

WHY THE BASIS IS RECORDED AND NOT JUST THE NAME

docs/attribution.md records that the annotation corpus gets mechanism right and
purpose WRONG often enough to matter -- Func_80b7e7c does not take the
arguments it was documented with, corrected in batch 01. A name taken from an
annotation and a name taken from reading the code are not equally trustworthy,
and recording only the name would launder a guess into a fact.

    read         we described the behaviour ourselves while elevating it
    named        the annotation proposes an actual identifier (e.g. TalkStaged)
    call-trace   the annotation says outright that it is "a CALL TRACE rather
                 than a description" -- it lists callees and claims nothing more
    annotation   prose asserting purpose. USE, BUT VERIFY.
    none         nothing beyond the address

Only `read` and `named` should be renamed without a second look, and `named`
only because it is checkable against the body in seconds.

KEYING

Overlay annotations are keyed by (overlay directory, offset within the
overlay), while our symbols carry the ABSOLUTE address. The overlay's load
address comes from its own overlay.ld -- it is 0x02008000, not 0x02000000, and
using the wrong one silently misses every single lookup rather than failing.
"""
import json
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
# The commit that adopted Coaltergeist's tree wholesale. Everything after it is
# ours, which is exactly the set that goes back upstream.
BASE = "851f208"

NAME_LINE = re.compile(r"^[A-Za-z_][A-Za-z0-9_]{3,}$")
DEFN = re.compile(r"^[A-Za-z_][A-Za-z0-9_ \t*]*?\b([A-Za-z_]\w*)\s*\([^;]*$", re.M)
# Header lines that are provenance boilerplate, not a description. NOTE: never
# put "" in here -- str.startswith("") is True for every string, which silently
# discards the entire header.
BOILER = ("Cluster", "Total .text", "Preserves the original", "asm/", "goldensun/",
          "The .s held", "For porting", "Verified against",
          # continuation lines of the wrapped provenance sentences above
          "name and its slot", "the neighbours", "the _a and _c", "listed in")


DOC_HEADER = """# Naming evidence

**Generated — do not hand-edit.** Regenerate with:

    python3 tools/name_evidence.py --doc > docs/names.md

Renaming is deliberately deferred until elevation is finished. Function names
do not survive into the ROM — `objcopy -O binary` drops the symbol table — so a
bulk rename is byte-neutral and `make compare` proves it. Nothing is gained by
paying for it early, and names chosen with a whole overlay in view beat names
chosen one function at a time.

What is **not** free to defer is the evidence, which is what this table holds.
Working out what a function does happens during elevation; without capturing
it, the rename pass would re-read all of it.

## Read the `Basis` column before trusting a name

`docs/attribution.md` records that the inherited annotation corpus gets
mechanism right and **purpose wrong** often enough to matter — `Func_80b7e7c`
does not take the arguments it was documented with, corrected in batch 01. A
name from an annotation and a name from reading the code are not equally
trustworthy, so each row carries where it came from:

| Basis | Meaning | Rename without re-checking? |
|---|---|---|
| `read` | we described the behaviour ourselves while elevating it | yes |
| `named` | the annotation proposes an actual identifier | yes — checkable against the body in seconds |
| `call-trace` | the annotation says outright it is a call trace, not a description | no — it claims nothing about purpose |
| `annotation` | inherited prose asserting purpose | **no — verify first** |
| `none` | nothing beyond the address | no |

Recording only the name would launder a guess into a fact.
"""


def added_sources():
    """The .c files we added, straight from git rather than from a hand list."""
    out = subprocess.run(
        ["git", "diff", "--name-only", "--diff-filter=A", f"{BASE}..HEAD", "--", "src"],
        cwd=ROOT, capture_output=True, text=True).stdout.split()
    return [p for p in out if p.endswith(".c") and "/non_matching/" not in p]


def all_sources():
    """EVERY elevated .c, not just the ones we added.

    added_sources() is scoped to what goes back upstream, which is the right
    set for the evidence table. The NAMING universe is larger: 2404 functions
    came in with Coaltergeist's tree before BASE, most already named but some
    still `Func_`. --subsystems counts against this set instead.
    """
    out = []
    for root, _, files in os.walk(os.path.join(ROOT, "src")):
        rel = os.path.relpath(root, ROOT).replace(os.sep, "/")
        if "/non_matching" in "/" + rel or rel.startswith("src/lib"):
            continue
        for f in files:
            if f.endswith(".c"):
                out.append(rel + "/" + f)
    return sorted(out)


PLACEHOLDER = re.compile(r"^(Func_|OvlFunc_|sub_)")


def naming_universe():
    """(still placeholder-named, already named) across every elevated .c."""
    place = named = 0
    for p in all_sources():
        for f in defined_functions(p):
            if PLACEHOLDER.match(f):
                place += 1
            else:
                named += 1
    return place, named


def defined_functions(path):
    body = open(os.path.join(ROOT, path), errors="replace").read()
    body = re.sub(r"/\*.*?\*/", "", body, flags=re.S)
    return [m.group(1) for m in DEFN.finditer(body)
            if not m.group(0).lstrip().startswith(("extern", "typedef"))]


def overlay_origin(ov):
    """Load address of an overlay, from its own linker script."""
    p = os.path.join(ROOT, "overlays", ov, "overlay.ld")
    if not os.path.exists(p):
        return None
    m = re.search(r"overlay\s*\([^)]*\)\s*:\s*ORIGIN\s*=\s*(0x[0-9a-fA-F]+)", open(p).read())
    return int(m.group(1), 0) if m else None


def annotations():
    """(overlay, offset) -> text for overlays; symbol -> text for the main ROM."""
    p = os.path.join(ROOT, "ANNOTATIONS.json")
    if not os.path.exists(p):
        return {}, {}
    ov, main = {}, {}
    for entries in json.load(open(p)).values():
        for e in entries:
            if not e.get("text"):
                continue
            if e.get("overlay") and e.get("addr") is not None:
                ov[(e["overlay"], e["addr"])] = e["text"]
            elif e.get("symbol"):
                main[e["symbol"]] = e["text"]
    return ov, main


PROPOSALS = os.path.join(ROOT, "tools", "name_proposals.tsv")


def proposals():
    """function -> {proposed, basis, subsystem, home, why} from the TSV.

    The TSV is the accumulating rename decision. It OUTRANKS an annotation's
    proposed identifier, because a row here was written with the body in view
    and with its family beside it, which is the whole argument for deferring
    the rename in the first place.
    """
    out = {}
    if not os.path.exists(PROPOSALS):
        return out
    for line in open(PROPOSALS, errors="replace"):
        if line.startswith("#") or not line.strip():
            continue
        f = line.rstrip("\n").split("\t")
        if len(f) < 6:
            continue
        out[f[0]] = {"proposed": f[1], "basis": f[2], "subsystem": f[3],
                     "home": f[4], "why": f[5]}
    return out


def own_description(path):
    """The prose WE wrote in the file header, minus the provenance boilerplate."""
    head = open(os.path.join(ROOT, path), errors="replace").read().split("*/")[0]
    kept = []
    for line in head.split("\n"):
        line = line.strip(" *\t/")
        if line and not line.startswith(BOILER):
            kept.append(line)
    return " ".join(kept)


def main():
    ov_ann, main_ann = annotations()
    prop = proposals()
    rows = []
    sources = all_sources() if "--subsystems" in sys.argv else added_sources()
    for path in sources:
        m = re.search(r"src/overlays/(rom_[0-9a-f]+|common)/", path)
        ovdir = m.group(1) if m else None
        origin = overlay_origin(ovdir) if ovdir else None

        for fn in defined_functions(path):
            am = re.search(r"_(2[0-9a-f]{6}|8[0-9a-f]{6})$", fn)
            addr = int(am.group(1), 16) if am else None

            ann = None
            if addr is not None and origin is not None:
                ann = ov_ann.get((ovdir, addr - origin))
            if ann is None:
                ann = main_ann.get(fn)

            proposed, basis, note = "", "none", ""
            if ann:
                first = ann.split("\n")[0].strip().strip("@ ")
                if "CALL TRACE rather than a description" in ann:
                    basis = "call-trace"
                    # the callee list is the only real content
                    tail = [l.strip(" @\t") for l in ann.split("\n")[1:] if l.strip(" @\t")]
                    note = " ".join(tail)
                elif NAME_LINE.match(first):
                    basis, proposed = "named", first
                    note = " ".join(ann.split("\n")[1:]).strip()
                else:
                    basis, note = "annotation", first

            mine = own_description(path)
            if mine:
                # our own reading outranks anything inherited, but a proposed
                # identifier is kept either way -- it costs nothing to carry
                basis = "read" if basis in ("none", "call-trace", "annotation") else basis
                note = mine

            pr = prop.get(fn)
            subsystem = home = why = ""
            if pr:
                proposed = pr["proposed"]
                basis = pr["basis"]
                subsystem, home, why = pr["subsystem"], pr["home"], pr["why"]

            rows.append({"function": fn,
                         "address": f"0x{addr:08x}" if addr else "",
                         "file": path, "proposed": proposed,
                         "basis": basis, "note": note,
                         "subsystem": subsystem, "home": home, "why": why})

    if "--doc" in sys.argv:
        print(DOC_HEADER)
    if "--json" in sys.argv:
        print(json.dumps(rows, indent=1))
        return

    if "--subsystems" in sys.argv:
        emit_subsystems(rows)
        return

    order = {"named": 0, "read": 1, "call-trace": 2, "annotation": 3, "none": 4}
    rows.sort(key=lambda r: (order.get(r["basis"], 9), r["function"]))
    counts = {}
    for r in rows:
        counts[r["basis"]] = counts.get(r["basis"], 0) + 1

    print("| Function | Address | Proposed | Basis | Evidence |")
    print("|---|---|---|---|---|")
    for r in rows:
        note = r["note"].replace("|", "\\|").replace("\n", " ")[:140]
        print(f"| `{r['function']}` | `{r['address']}` | "
              f"{('`' + r['proposed'] + '`') if r['proposed'] else '—'} | "
              f"{r['basis']} | {note} |")
    print(f"\n{len(rows)} functions: "
          + ", ".join(f"{v} {k}" for k, v in sorted(counts.items())))


SUB_HEADER = """# Name proposals by subsystem

**Generated — do not hand-edit.** Regenerate with:

    python3 tools/name_evidence.py --subsystems > docs/name-proposals.md

The source of truth is `tools/name_proposals.tsv`. `docs/names.md` holds the
EVIDENCE for every elevated function; this holds the DECISION for those that
have one, grouped the way a rename pass would actually be run — by subsystem,
so that a family sharing a body gets one name rather than one name per batch.

Style follows this tree's own conventions: `PascalCase` for a plain routine,
`Subsystem_Verb` where a module owns it (`Actor_TravelTo`, `MapActor_SetIdle`,
`UI_SellMenu`, `Anim_Judgment`), `g*` for globals. The upstream decomp's `src/`
was not read.

Read the `Basis` column before trusting a row; `docs/attribution.md` records
that the inherited annotation corpus gets mechanism right and purpose wrong
often enough to matter. Where the code does not establish an identity — which
status effect a counter belongs to, say — the name carries an `Unk<offset>`
suffix and the `Why` column says so. That is deliberate: a guess dressed as a
fact is worse than an honest placeholder.
"""


def emit_subsystems(rows):
    """docs/name-proposals.md: only rows that HAVE a proposal, by subsystem."""
    have = [r for r in rows if r.get("subsystem")]
    print(SUB_HEADER)
    groups = {}
    for r in have:
        groups.setdefault(r["subsystem"], []).append(r)
    done = len(have)
    place, already = naming_universe()
    print("%d elevated functions carry a real name already; **%d still carry a "
          "`Func_`/`OvlFunc_` placeholder**, and those are the naming job.\n"
          % (already, place))
    print("This file proposes **%d** of them (%.1f%%).\n"
          % (done, 100.0 * done / place if place else 0))
    for sub in sorted(groups):
        g = sorted(groups[sub], key=lambda r: r["function"])
        print("## %s — %d function%s\n" % (sub, len(g), "" if len(g) == 1 else "s"))
        print("| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |")
        print("|---|---|---|---|---|---|---|")
        for r in g:
            why = r["why"].replace("|", "\\|")
            print("| `%s` | `%s` | `%s` | %s | %s | %s | `%s` |"
                  % (r["function"], r["address"], r["proposed"], r["basis"],
                     r["subsystem"], why, r["home"]))
        print()


if __name__ == "__main__":
    main()
