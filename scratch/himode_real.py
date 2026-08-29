import os, re
PARKS = sorted(os.path.join(r, f) for r, _, fs in os.walk('src/non_matching')
               for f in fs if f.endswith('.c'))
HDR = re.compile(r'^\s*/?\*?\s*((?:Ovl)?Func_\w+|[A-Za-z][A-Za-z0-9_]+)\s*(?:\[[^\]]*\])?\s*(?:--|@)')
# the blocker sentence must be about a halfword CONSTANT, not just contain strh
REAL = re.compile(r'(pool\w*[^.]{0,60}(halfword|HImode|0xffff))|'
                  r'((halfword|HImode)[^.]{0,60}pool)|'
                  r'(HImode literal)|(narrow\w*[^.]{0,40}short)', re.I)
INTERM = re.compile(r'int intermediate|intermediate.{0,20}int', re.I)
DIFF = re.compile(r'\b(\d+)\s+differ', re.I)
rows = []
for p in PARKS:
    txt = open(p, errors='replace').read()
    if not REAL.search(txt) or INTERM.search(txt):
        continue
    m = next((HDR.match(l) for l in txt.split('\n')[:3] if HDR.match(l)), None)
    if not m: continue
    d = DIFF.search(txt)
    rows.append((int(d.group(1)) if d else 999, m.group(1), p))
print(f"parks where a halfword CONSTANT is described as the blocker, int-intermediate untried: {len(rows)}")
for d, fn, p in sorted(rows):
    print(f"  {(d if d != 999 else '?'):>4} differ  {fn:26} {p}")
