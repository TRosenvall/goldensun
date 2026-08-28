import os, re, subprocess, sys
sys.path.insert(0, 'tools')
import pool

PARKS = sorted(os.path.join(r, f) for r, _, fs in os.walk('src/non_matching')
               for f in fs if f.endswith('.c'))
HDR = re.compile(r'^\s*/?\*?\s*((?:Ovl)?Func_\w+|[A-Za-z][A-Za-z0-9_]+)\s*(?:\[[^\]]*\])?\s*(?:--|@)')
INTER = re.compile(r'interleav|argument[- ]setup|arg[- ]interleave', re.I)

rows = []
for p in PARKS:
    txt = open(p, errors='replace').read()
    if not INTER.search(txt):
        continue
    m = next((HDR.match(l) for l in txt.split('\n')[:3] if HDR.match(l)), None)
    if not m:
        continue
    fn = m.group(1)
    out = subprocess.run([sys.executable, 'tools/showfunc.py', fn],
                         capture_output=True, text=True).stdout
    if '.thumb_func_start' not in out:
        continue
    lines = out.split('\n')
    start = next(i for i, l in enumerate(lines) if l.startswith('.thumb_func_start'))
    end = next((i for i, l in enumerate(lines) if l.startswith('.func_end')), len(lines))
    body = lines[start + 1:end]
    r = pool.measure(fn, p, body, lines, start)
    if r:
        rows.append((r['site'], r['unguarded'], r['n'], fn, p))

g = [r for r in rows if r[0] > 0]
u = [r for r in rows if r[0] == 0]
print(f"interleave parks measured: {len(rows)}")
print(f"  with a GUARDED site (branch BEFORE the site) : {len(g)}  <- lever has a dominating block")
print(f"  no guarded site                              : {len(u)}")
print("\nGUARDED SITES, smallest function first:")
for site, ung, n, fn, p in sorted(g, key=lambda t: t[2]):
    print(f"   {n:4} insns  {site} guarded  {ung} unguarded  {fn:28} {p}")
