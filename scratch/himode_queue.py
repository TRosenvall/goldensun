import os, re, subprocess
PARKS = sorted(os.path.join(r, f) for r, _, fs in os.walk('src/non_matching')
               for f in fs if f.endswith('.c'))
HDR = re.compile(r'^\s*/?\*?\s*((?:Ovl)?Func_\w+|[A-Za-z][A-Za-z0-9_]+)\s*(?:\[[^\]]*\])?\s*(?:--|@)')
HI = re.compile(r'HImode|halfword|strh|ldrh', re.I)
INTERM = re.compile(r'int intermediate|intermediate.{0,20}int', re.I)
rows = []
for p in PARKS:
    txt = open(p, errors='replace').read()
    if not HI.search(txt):
        continue
    m = next((HDR.match(l) for l in txt.split('\n')[:3] if HDR.match(l)), None)
    if not m: continue
    date = subprocess.run(['git','log','--diff-filter=A','--format=%ad','--date=short','-1','--',p],
                          capture_output=True, text=True).stdout.strip()
    rows.append((date, m.group(1), bool(INTERM.search(txt)), p))
print(f"parks mentioning halfword/HImode: {len(rows)}")
print(f"{'parked':10} {'int-interm tried':17} function")
for d, fn, tried, p in sorted(rows):
    print(f"{d:10} {'yes' if tried else 'NO':17} {fn:26} {p}")
