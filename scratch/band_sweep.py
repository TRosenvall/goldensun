import os, re, subprocess, sys
sys.path.insert(0, 'tools')
import pool

PARKS = sorted(os.path.join(r, f) for r, _, fs in os.walk('src/non_matching')
               for f in fs if f.endswith('.c'))
HDR = re.compile(r'^\s*/?\*?\s*((?:Ovl)?Func_\w+|[A-Za-z][A-Za-z0-9_]+)\s*(?:\[[^\]]*\])?\s*(?:--|@)')
LEVER = re.compile(r'interleav|argument[- ]setup|arg[- ]order|arg order|stack argument|'
                   r'spill|halfword|HImode|divsi3|modsi3|sub sp', re.I)

rows = []
for p in PARKS:
    txt = open(p, errors='replace').read()
    m = next((HDR.match(l) for l in txt.split('\n')[:3] if HDR.match(l)), None)
    if not m:
        continue
    fn = m.group(1)
    out = subprocess.run([sys.executable, 'tools/showfunc.py', fn],
                         capture_output=True, text=True).stdout
    if '.thumb_func_start' not in out:
        continue
    L = out.split('\n')
    st = next(i for i, l in enumerate(L) if l.startswith('.thumb_func_start'))
    en = next((i for i, l in enumerate(L) if l.startswith('.func_end')), len(L))
    r = pool.measure(fn, p, L[st+1:en], L, st)
    if not r or not (21 <= r['n'] <= 40):
        continue
    date = subprocess.run(['git','log','--diff-filter=A','--format=%ad','--date=short','-1','--',p],
                          capture_output=True, text=True).stdout.strip()
    rows.append((date, r['n'], r['site'], r['unguarded'], r['reuse'],
                 bool(LEVER.search(txt)), fn, p))

print(f"parks in the 21-40 instruction band with a live .s: {len(rows)}\n")
print(f"{'parked':11}{'insns':>6}{'site':>5}{'ungd':>5}{'reuse':>6}  lever-class  function")
for d, n, site, ung, reuse, lev, fn, p in sorted(rows):
    print(f"{d:11}{n:6}{site:5}{ung:5}{reuse:6}  {'YES' if lev else '-  ':11}  {fn:26} {p}")
