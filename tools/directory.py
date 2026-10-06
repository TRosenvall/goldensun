#!/usr/bin/env python3
"""directory.py -- generate docs/directory/, the per-function work index.

    python3 tools/directory.py            # regenerate docs/directory/
    python3 tools/directory.py --summary  # print the counts only

WHAT IT IS FOR.  Three questions get asked repeatedly and have never had one
answer in the tree:

  * which functions are PARKED, how close each is, and which module it sits in;
  * which landed functions need DEPINNING (pass 3) -- they match, but only with a
    register pin, a barrier, a device or a per-file flag;
  * which landed functions need HUMANIZING (pass 4) -- they match and are clean,
    but are still named `Func_<addr>` in a split-named file.

A function can be in exactly one of the first two states and may be in the third
as well: depinning is about ARTIFACTS, humanizing is about NAMES AND STRUCTURE,
and a file can need both.

WHERE EACH NUMBER COMES FROM, because this project has been bitten repeatedly by
a count taken the convenient way:

  * PIN AND BARRIER COUNTS COME FROM tools/shimcount.py, never from grep.
    docs/elevation.md says so in terms ("COUNT SHIMS WITH tools/shimcount.py,
    NEVER WITH grep"), because a header that DESCRIBES its pins, a sentence
    saying there are NONE, and pins hidden inside PIN macros each defeat a
    pattern.  Six counts in one session were wrong that way.
  * PARK FIGURES come from the park header's own claim, parsed with the same
    regexes tools/parkcheck.py uses, and parkcheck is what verifies them against
    objcmp.  A figure here is a CLAIM; parkcheck is the authority.
  * MODULES come from tools/upstream_module.py, which resolves a park by its
    RECIPE SYMBOL rather than its filename -- parks are address-named and
    landings are split-named, and a filename match finds every landing and no
    park.
  * DEVICE AND SYMBOL-REFERENCE counts are grep-derived and LABELLED AS
    INDICATIVE in the output, because there is no authority tool for them yet.

A FUNCTION IS NOT LISTED AS NEEDING DEPINNING JUST BECAUSE ITS FILE CARRIES AN
ARTIFACT.  A file can hold several functions after a split, and the artifact
belongs to one of them.  The index is therefore FILE-SCOPED for pass-3 work and
function-scoped for the rest, and says which it is on every table.
"""
import collections, json, os, re, subprocess, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, "docs", "directory")

CLAIM = [
    re.compile(r"(\d+)\s+(?:differing\s+)?encodings?\s+of\s+(\d+)", re.I),
    re.compile(r"(\d+)\s+of\s+(\d+)\s+(?:differing\s+)?encodings?", re.I),
    re.compile(r"NON-MATCHING,\s*(\d+)\s+of\s+(\d+)", re.I),
    re.compile(r"PARKED\s+(?:AT\s+)?(\d+)\s+of\s+(\d+)", re.I),
]
VERIFY = re.compile(r"objcmp\.py[\s\\]+\S+[\s\\]+(\S+\.s)(?:[\s\\]+--func\s+(\S+))?")
DEFN = re.compile(r"^[\w \t\*]*?\b(\w+)\s*\([^;]*\)\s*\{", re.M)


def sh(*a):
    return subprocess.run(a, capture_output=True, text=True, cwd=ROOT).stdout


def tracked(pat):
    return [f for f in sh("git", "ls-files", pat).split() if f.endswith(".c")]


def header_and_body(path):
    s = open(os.path.join(ROOT, path), errors="replace").read()
    # the leading RUN of comment blocks is the header, matching parkcheck
    blocks, rest, n = [], s, 0
    while True:
        m = re.match(r"\s*(/\*.*?\*/)", rest, re.S)
        if not m:
            break
        blocks.append(m.group(1))
        n += m.end()
        rest = rest[m.end():]
    return "\n".join(blocks), rest, s


def parks():
    """[{func, park, figure, total, module, bank, checkable}]"""
    sys.path.insert(0, os.path.join(ROOT, "tools"))
    try:
        import upstream_module as um
        _up, _landed, pk, _asm = um.index()
        mod_of = {p: k for k, ps in pk.items() for p in ps}
    except Exception as e:
        print(f"  (upstream_module unavailable: {e})", file=sys.stderr)
        mod_of = {}
    out = []
    for p in tracked("src/non_matching/**"):
        hdr, body, _ = header_and_body(p)
        flat = " ".join(hdr.replace("*", " ").split())
        m = next((x for x in (c.search(flat) for c in CLAIM) if x), None)
        v = VERIFY.search(hdr.replace("*", " "))
        fn = (v.group(2) if v and v.group(2) else None)
        if not fn:
            fm = re.search(r"--func\s+([A-Za-z_]\w*)", hdr)
            fn = fm.group(1) if fm else None
        if not fn:
            d = DEFN.search(re.sub(r"/\*.*?\*/", " ", body, flags=re.S))
            fn = d.group(1) if d else "?"
        ref = v.group(1).strip("'\"") if v else None
        key = mod_of.get(p)
        out.append({
            "func": fn, "park": p,
            "figure": int(m.group(1)) if m else None,
            "total": int(m.group(2)) if m else None,
            "module": f"{key[0]}/{key[1]}.s" if key else None,
            "bank": p.split("non_matching/")[1].split("/")[0],
            "has_body": bool(re.search(r"\)\s*\{", body)),
            "checkable": bool(ref and os.path.exists(os.path.join(ROOT, ref))),
        })
    return out


def shims(paths=None):
    """{file: {kind: count}} from tools/shimcount.py, THE AUTHORITY.

    With no argument this is `--all`, i.e. the landed sources.  Given explicit
    paths it scores those instead, which is how the PARKED index gets its pin
    column -- and that column is not decoration.  Batch 328 brief A caught me
    ranking a park at "2" as the closest in its bank when the body carries TEN
    pins and its pin-free figure is in the hundreds; under owner standard 3 it
    cannot be a pass-2 landing even at 0.  RANK LOW-BAND PARKS BY (figure, pins),
    NEVER BY FIGURE.
    """
    cmd = [sys.executable, os.path.join(ROOT, "tools", "shimcount.py")]
    cmd += list(paths) if paths else ["--all"]
    r = subprocess.run(cmd, capture_output=True, text=True, cwd=ROOT)
    out, cur, unbooked = collections.defaultdict(dict), None, set()
    for line in r.stdout.splitlines():
        if line.startswith("src/"):
            cur = line.strip()
            continue
        if "NO fakematch.txt row" in line and cur:
            unbooked.add(cur)
            continue
        m = re.match(r"\s+(register pins|\"\+r\" barriers|empty asm|\.equ shims?)\s*:\s*(\d+)", line)
        if m and cur:
            out[cur][m.group(1)] = int(m.group(2))
    return out, unbooked


def indicative(files):
    """grep-derived signals, LABELLED indicative in the output."""
    pats = {
        "volatile cast": re.compile(r"\(\s*volatile\b"),
        "symbol ref": re.compile(r"\(int\)\s*&\s*_(?:CONST|MSG|AREA|FILE|SIZE|LABEL)"),
        "do-while barrier": re.compile(r"while\s*\(\s*0\s*\)"),
    }
    out = collections.defaultdict(dict)
    for f in files:
        s = open(os.path.join(ROOT, f), errors="replace").read()
        s = re.sub(r"/\*.*?\*/", " ", s, flags=re.S)          # strip prose first
        s = re.sub(r"//[^\n]*", " ", s)
        for k, rx in pats.items():
            n = len(rx.findall(s))
            if n:
                out[f][k] = n
    return out


def flag_groups():
    mk = open(os.path.join(ROOT, "Makefile"), errors="replace").read()
    groups = re.findall(r"^([A-Z0-9_]+_CFLAGS)\s*[:+]?=", mk, re.M)
    objs = collections.defaultdict(list)
    for g in set(groups):
        for m in re.finditer(r"^(\S+\.o):.*\$\(" + re.escape(g) + r"\)", mk, re.M):
            objs[g].append(m.group(1))
    return sorted(set(groups)), objs


def names(files):
    """function-scoped: which landed definitions are still Func_<addr>."""
    addr = re.compile(r"^(?:Ovl)?Func_[0-9a-f]+$|^OvlFunc_\d+_[0-9a-f]+$")
    anon, named = [], []
    for f in files:
        s = open(os.path.join(ROOT, f), errors="replace").read()
        s = re.sub(r"/\*.*?\*/", " ", s, flags=re.S)
        for m in DEFN.finditer(s):
            n = m.group(1)
            if n in ("if", "for", "while", "switch", "do", "return", "sizeof"):
                continue
            (anon if addr.match(n) else named).append((n, f))
    return anon, named


def census_landed():
    """What tools/census.py says is landed -- the authority on ROM function counts."""
    try:
        out = sh(sys.executable, os.path.join(ROOT, "tools", "census.py"))
        m = re.search(r"TOTAL\s*\|\s*(\d+)", out)
        return 5710 - int(m.group(1)) if m else "?"
    except Exception:
        return "?"


BARRIER_KEY = '"+r" barriers'


def main():
    summary_only = "--summary" in sys.argv
    pk = parks()
    landed = [f for f in tracked("src/**") if "/non_matching/" not in f]
    sh_map, unbooked = shims()
    park_sh, _ = shims([p["park"] for p in pk]) if pk else ({}, set())
    for r in pk:
        r["pins"] = park_sh.get(r["park"], {}).get("register pins", 0)
    ind = indicative(landed)
    groups, group_objs = flag_groups()
    anon, named = names(landed)
    split_named = [f for f in landed
                   if re.search(r"/(?:rom|ovl)[0-9a-z_]*_[0-9a-f]+(?:_[a-z])+\.c$", f)]

    depin_files = sorted(set(sh_map) | set(ind),
                         key=lambda f: -(sum(sh_map.get(f, {}).values())
                                         + sum(ind.get(f, {}).values())))
    figs = sorted(p["figure"] for p in pk if p["figure"] is not None)

    # Reconcile against tools/census.py, which is the authority on how many ROM
    # FUNCTIONS exist.  This tool counts DEFINITIONS found in landed sources, and
    # that is a larger set: it includes static and inline helpers that are not
    # separate ROM functions.  Labelling it "landed functions" would overstate
    # progress, which is the one number nobody should have to double-check.
    S = {
        "definitions in landed sources": len(anon) + len(named),
        "  (census ROM functions landed)": census_landed(),
        "landed .c files": len(landed),
        "park FILES (not functions)": len(pk),
        "parked with a figure": len(figs),
        "parked bodyless": sum(1 for p in pk if not p["has_body"]),
        "parked unverifiable (no live recipe)": sum(1 for p in pk if not p["checkable"]),
        "files needing DEPIN": len(depin_files),
        "register pins (shimcount)": sum(v.get("register pins", 0) for v in sh_map.values()),
        "fakematch-class barriers": sum(v.get(BARRIER_KEY, 0) for v in sh_map.values()),
        "unbooked fakematch-class": len(unbooked),
        "per-file flag groups": len(groups),
        "parks carrying register pins": sum(1 for r in pk if r.get("pins")),
        "functions still Func_<addr>": len(anon),
        "functions with a real name": len(named),
        "split-named files": len(split_named),
    }
    for k, v in S.items():
        print(f"  {k:38s} {v}")
    if summary_only:
        return 0

    os.makedirs(OUT, exist_ok=True)

    # ---------------- README ----------------
    with open(os.path.join(OUT, "README.md"), "w") as f:
        f.write(f"""# The work directory

Generated by `tools/directory.py`. **Regenerate it rather than editing it** —
every number here is derived, and a hand-edit will be silently overwritten.

    git add -A && python3 tools/directory.py

## ⚠ STAGE THE BATCH FIRST — an unstaged landing is INVISIBLE here

This tool and `tools/dupfuncs.py` both enumerate through **`git ls-files`**, so a
landing whose new `.c` has not been `git add`ed does not exist as far as either is
concerned. The failure is silent and the output looks entirely plausible: after
batch 329's eleven landings a pre-staging run still reported the *old* 5,006
definitions and 4,484 landed `.c` files, while `census.py` — which walks the
filesystem — had already moved to 4,894. Four counters, two reading git and two
reading disk, and only the disk pair was right.

Run `git add -A` before regenerating, and if `definitions in landed sources` has
not moved after a batch that landed something, that is the bug — not a plateau.

## The three states

| file | question | scope |
|---|---|---|
| [`01-parked.md`](01-parked.md) | which functions do not match yet, and how close | per function |
| [`02-depin.md`](02-depin.md) | which landed files match only with an artifact (**pass 3**) | **per file** |
| [`03-humanize.md`](03-humanize.md) | which landed functions are still unnamed, in split-named files (**pass 4**) | per function |

A function is in **exactly one** of the first two states. It may be in the third
as well: depinning is about **artifacts**, humanizing is about **names and
structure**, and a file can need both.

## Counts

| | |
|---|---|
""")
        for k, v in S.items():
            f.write(f"| {k} | **{v:,}** |\n")
        f.write(f"""
## Why two function counts

`definitions in landed sources` counts every function **defined** in a landed
`.c`, which includes `static` and `inline` helpers that are not separate ROM
functions. `tools/census.py` is the authority on **ROM functions**, and its
figure is the one to quote for progress. The gap between them is real code, not
an error.

## Where the numbers come from

- **Pin and barrier counts come from `tools/shimcount.py`, never from grep.**
  `docs/elevation.md` says so in terms, because a header that *describes* its
  pins, a sentence saying there are *none*, and pins hidden inside `PIN` macros
  each defeat a pattern. Six counts in one session were wrong that way.
- **Park figures are the park's own CLAIM**, parsed with the same regexes
  `tools/parkcheck.py` uses. **`parkcheck` is the authority** — it re-measures
  each claim with `objcmp`. A figure here has not been re-verified by this tool.
- **Modules come from `tools/upstream_module.py`**, which resolves a park by its
  **recipe symbol** rather than its filename: parks are address-named and
  landings are split-named, so a filename match finds every landing and no park.
- **Device and symbol-reference counts are grep-derived and INDICATIVE.** There
  is no authority tool for them yet. Block comments are stripped before matching,
  which removes the prose-counting failure but not every one.

## Caveat that matters for pass 3

`02-depin.md` is **file-scoped**. After a split a file can hold several
functions, and an artifact belongs to one of them — so a file appearing there
does not mean every function in it needs work. Resolving artifact to function is
the first task of pass 3, not something this index can do.
""")

    # ---------------- 01 parked ----------------
    with open(os.path.join(OUT, "01-parked.md"), "w") as f:
        f.write("# Parked functions\n\n")
        f.write(f"**{len(pk)} park files**, {len(figs)} carrying a parseable figure, "
                f"{sum(1 for p in pk if not p['has_body'])} bodyless (prose/triage/class parks), "
                f"{sum(1 for p in pk if not p['checkable'])} with no live verify recipe.\n\n")
        f.write("A figure is the park's own claim. `tools/parkcheck.py` is what verifies it.\n\n")
        bands = [(0, 0), (1, 2), (3, 5), (6, 10), (11, 20), (21, 50),
                 (51, 100), (101, 300), (301, 10**9)]
        f.write("## Distribution\n\n| differing encodings | parks |\n|---|---|\n")
        for lo, hi in bands:
            n = sum(1 for x in figs if lo <= x <= hi)
            lbl = str(lo) if lo == hi else (f"{lo}–{hi}" if hi < 10**9 else f"{lo}+")
            f.write(f"| {lbl} | {n} |\n")
        if figs:
            f.write(f"\nMedian **{figs[len(figs)//2]}**.\n")
        npin = sum(1 for r in pk if r.get("pins"))
        f.write(f"""
## By figure

**{npin} of these parks carry register pins.** A pinned body's figure is not
comparable with a pin-free one: owner standard 3 prefers a pin-free body, so a
park at 2 with ten pins is further from a pass-2 landing than a park at 20 with
none. **Rank by (figure, pins).** Pin counts are `shimcount`'s.

| fig | pins | of | function | module | park |
|---|---|---|---|---|---|
""")
        for p in sorted(pk, key=lambda r: (r["figure"] is None, r["figure"] or 0,
                                           r.get("pins", 0), r["func"])):
            if p["figure"] is None:
                continue
            pins = p.get("pins", 0)
            f.write(f"| {p['figure']} | {pins if pins else ''} | {p['total'] or '?'} | "
                    f"`{p['func']}` | {p['module'] or '—'} | "
                    f"[{p['park']}](../../{p['park']}) |\n")
        nof = [p for p in pk if p["figure"] is None]
        if nof:
            f.write(f"\n## No parseable figure ({len(nof)})\n\n"
                    "Prose, triage and class parks, plus any whose claim phrasing "
                    "`parkcheck` cannot read.\n\n| function | park |\n|---|---|\n")
            for p in sorted(nof, key=lambda r: r["park"]):
                f.write(f"| `{p['func']}` | [{p['park']}](../../{p['park']}) |\n")

    # ---------------- 02 depin ----------------
    with open(os.path.join(OUT, "02-depin.md"), "w") as f:
        f.write("# Pass 3 — depinning\n\n**File-scoped.** See the caveat in the README.\n\n")
        f.write(f"**{len(depin_files)} landed files** carry at least one matching artifact: "
                f"**{S['register pins (shimcount)']:,} register pins**, "
                f"{S['fakematch-class barriers']} fakematch-class barriers, "
                f"{sum(v.get('empty asm',0) for v in sh_map.values())} bare empty-asm barriers "
                f"(**not** a fakematch in this tree), "
                f"{sum(v.get('.equ shims',0)+v.get('.equ shim',0) for v in sh_map.values())} `.equ` shims.\n\n")
        if unbooked:
            f.write("## ⚠ Fakematch-class shim with NO `fakematch.txt` row\n\n"
                    "An unbooked shim is an undocumented fakematch. These need a row or a removal.\n\n")
            for u in sorted(unbooked):
                f.write(f"- [{u}](../../{u})\n")
            f.write("\n")
        f.write("## Per-file artifact load\n\n"
                "`pins`, `+r` and `empty asm` are from `shimcount` (authoritative). "
                "`volatile`, `symbol` and `do-while` are **indicative** greps.\n\n"
                "| pins | +r | empty | volatile | symbol | do-while | file |\n"
                "|---|---|---|---|---|---|---|\n")
        BAR = '"+r" barriers'
        for fl in depin_files:
            sm, im = sh_map.get(fl, {}), ind.get(fl, {})
            cells = [sm.get("register pins", ""), sm.get(BAR, ""), sm.get("empty asm", ""),
                     im.get("volatile cast", ""), im.get("symbol ref", ""),
                     im.get("do-while barrier", "")]
            row = " | ".join(str(c) if c else "" for c in cells)
            f.write("| " + row + " | [" + fl + "](../../" + fl + ") |\n")
        f.write(f"\n## Per-file flag groups ({len(groups)})\n\n"
                "A per-file flag group asserts something about how the original object was "
                "built. `docs/owner-decisions.md` entry 2 declined one on exactly that "
                "ground.\n\n")
        for g in groups:
            f.write(f"- **`{g}`** — {len(group_objs.get(g, []))} object(s)\n")

    # ---------------- 03 humanize ----------------
    with open(os.path.join(OUT, "03-humanize.md"), "w") as f:
        f.write("# Pass 4 — humanizing\n\n")
        f.write(f"**{len(anon):,} landed functions are still named `Func_<addr>`** against "
                f"{len(named):,} with a real name, across **{len(split_named):,} split-named "
                f"files** of {len(landed):,} landed.\n\n")
        f.write("""See `docs/humanization.md` for the method and the artifact taxonomy. Two
structural facts bound this work:

- **1,543 local labels have been promoted to `.global`** purely because file
  splits put a label's user in another object. The original build never exported
  one. That is invisible in the ROM — a symbol table is not linked in — but it is
  the clearest measure of how far the tree has drifted from plausible original
  source.
- **150 upstream modules are spread across more than one of our `.c` files**, a
  mean of 29.6 each, and only 19 have been landed as a single whole `.c`. See
  `tools/upstream_module.py --fragmented`.

## Still named `Func_<addr>`

""")
        bybank = collections.defaultdict(list)
        for n, fl in anon:
            bybank[fl.split("/")[1]].append((n, fl))
        f.write("| bank | unnamed functions |\n|---|---|\n")
        for b, v in sorted(bybank.items(), key=lambda kv: -len(kv[1])):
            f.write(f"| {b} | {len(v)} |\n")
        f.write("\n<details><summary>Full list</summary>\n\n| function | file |\n|---|---|\n")
        for n, fl in sorted(anon):
            f.write(f"| `{n}` | [{fl}](../../{fl}) |\n")
        f.write("\n</details>\n")

    print(f"\nwrote {OUT}/README.md, 01-parked.md, 02-depin.md, 03-humanize.md")
    return 0


if __name__ == "__main__":
    sys.exit(main())
