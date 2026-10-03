#!/usr/bin/env python3
"""frontier.py -- rank the parked frontier by VERIFIED distance, and group it by blocker.

    docker run ... goldensun-build python3 tools/parkcheck.py > /tmp/audit.txt
    python3 tools/frontier.py /tmp/audit.txt                 # ranked table
    python3 tools/frontier.py /tmp/audit.txt --md reports/frontier.md
    python3 tools/frontier.py /tmp/audit.txt --group         # by blocker keyword

WHY.  Targets were being chosen off park PROSE, and batch 319's audit showed why
that is unsafe: of 819 parks, 342 had a recipe and ALL 342 agreed with their
headers, while 428 had no recipe and had never been measured by anything.  Every
wrong figure this project has found sat in that second group -- including one park
claiming "78 of 85" that measures **4 of 85**, which had been near the top of the
frontier for batches while looking 74 encodings further away than it was.

So: rank on parkcheck's verified OK figures and nothing else.  A park with no
recipe does not appear here, and that absence is the point -- it is a work item
(write a recipe, measure it), not a target.

GROUPING MATTERS AS MUCH AS RANKING.  Measured per-landing token cost over batches
317-318: briefs whose targets shared ONE wall ran at 58k-83k per landing, mixed
assignments at ~95k, and a brief carrying one hard decline at 162k.  Grouping by
mechanism pays the diagnosis once and spends it three or four times, so `--group`
buckets the frontier by the blocker keywords the parks themselves use.  Those
buckets are a STARTING HYPOTHESIS for batching, not a classification -- a park's
stated blocker has been wrong roughly forty times out of forty-two in pass two.
"""
import argparse, glob, os, re, sys, collections

# the blocker vocabulary this tree actually uses, most specific first
BLOCKERS = [
    ("sched2-tie",      r"rank_for_schedule|dependent count|INSN_LUID|sched2 tie|scheduling tie"),
    ("alloc-order",     r"allocno_compare|local-alloc|global_alloc|register (allocation|permutation)"
                        r"|REG_N_REFS|live length"),
    ("reload",          r"\breload\b|find_dummy_reload|choose_reload_regs|spill"),
    ("pool-order",      r"pool (word )?order|minipool|literal pool|max_address"),
    ("cse-gcse",        r"\bcse1?\b|gcse|PRE |pre_insert_copies|rerun-cse"),
    ("alias-set",       r"alias set|DIFFERENT_ALIAS_SETS_P|true_dependence|anti_dependence"),
    ("frame",           r"\bframe\b|sub sp|push \{|prologue|epilogue"),
    ("arg-interleave",  r"arg(ument)? interleave|calls\.c|precompute"),
    ("symbol-table",    r"\.sym\b|relocation must|pooled (zero|symbol)"),
    ("tu-shape",        r"nested function|static chain|whole-file|TU shape|original TU"),
]


def parse_audit(path):
    rows = []
    for l in open(path, errors="replace"):
        m = re.match(r"\s+OK\s+(\S+)\s+(\d+)(.*)$", l)
        if m:
            rows.append((int(m.group(2)), m.group(1), m.group(3).strip()))
    return rows


def park_meta(p):
    s = open(p, errors="replace").read()
    i = s.find("*/")
    hdr = s[:i if i > 0 else 3000]
    tot = re.search(r"of\s+(\d+)\s+encodings|encodings?\s+of\s+(\d+)|of\s+(\d+)\b", hdr)
    total = next((g for g in (tot.groups() if tot else []) if g), None)
    fn = re.search(r"--func\s+(\w+)", hdr)
    kinds = [k for k, pat in BLOCKERS if re.search(pat, hdr, re.I)]
    flagged = bool(re.search(r"OBJCMP_EXTRA|NOT a production-flag", hdr))
    return (int(total) if total else None, fn.group(1) if fn else "?", kinds, flagged)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("audit")
    ap.add_argument("--md")
    ap.add_argument("--group", action="store_true")
    ap.add_argument("--max", type=int, default=50, help="only parks at or below this figure")
    a = ap.parse_args()

    rows = parse_audit(a.audit)
    enriched = []
    for d, p, note in rows:
        total, fn, kinds, flagged = park_meta(p)
        enriched.append(dict(d=d, path=p, total=total, func=fn, kinds=kinds,
                             flagged=flagged or "production-flag" in note))
    enriched.sort(key=lambda r: (r["d"], r["path"]))

    out = []
    out.append(f"# Verified parked frontier\n")
    out.append(f"{len(enriched)} parks with a parkcheck-verified figure. "
               f"Parks with no recipe are ABSENT and are a work item, not a target.\n")
    buckets = collections.Counter()
    for r in enriched:
        for k in (r["kinds"] or ["unclassified"]):
            buckets[k] += 1
    out.append("\n## Blocker buckets (a park can appear in several)\n")
    for k, n in buckets.most_common():
        out.append(f"- **{k}** — {n}")
    out.append(f"\n## Ranked, figure <= {a.max}\n")
    out.append("| figure | of | park | blocker(s) |")
    out.append("|---:|---:|---|---|")
    for r in enriched:
        if r["d"] > a.max:
            continue
        flag = " ⚑flag" if r["flagged"] else ""
        out.append(f"| **{r['d']}**{flag} | {r['total'] or '?'} | "
                   f"`{r['path'].split('non_matching/')[-1]}` | {', '.join(r['kinds']) or '—'} |")
    txt = "\n".join(out) + "\n"
    if a.md:
        open(a.md, "w").write(txt)
        print(f"wrote {a.md}  ({len(enriched)} verified parks)")
    else:
        print(txt)
    if a.group:
        print("\n## Suggested mechanism-grouped batches (hypotheses, not classifications)\n")
        by = collections.defaultdict(list)
        for r in enriched:
            if r["d"] <= a.max:
                by[(r["kinds"] or ["unclassified"])[0]].append(r)
        for k, v in sorted(by.items(), key=lambda kv: -len(kv[1])):
            v.sort(key=lambda r: r["d"])
            names = ", ".join(f"{x['path'].split('/')[-1]}({x['d']})" for x in v[:6])
            print(f"  {k:<16} {len(v):>3} parks   closest: {names}")


if __name__ == "__main__":
    main()
