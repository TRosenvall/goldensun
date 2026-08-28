import os, re
PARKS = sorted(os.path.join(r, f) for r, _, fs in os.walk('src/non_matching')
               for f in fs if f.endswith('.c'))
HDR = re.compile(r'^\s*/?\*?\s*((?:Ovl)?Func_\w+|[A-Za-z][A-Za-z0-9_]+)\s*(?:\[[^\]]*\])?\s*(?:--|@)')
RAW = re.compile(r'\*\s*\(\s*(unsigned\s+)?(char|short|int|u8|u16|u32|s8|s16|s32)\s*\*\s*\)\s*\(|\w+\s*\[\s*0x[0-9a-f]{2}\s*\]')
CAND = re.compile(r'scratch/(\S+\.c)')
DIFF = re.compile(r'\b(\d+)\s+differ', re.I)
rows = []
for p in PARKS:
    txt = open(p, errors='replace').read()
    m = next((HDR.match(l) for l in txt.split('\n')[:3] if HDR.match(l)), None)
    if not m or not RAW.search(txt):
        continue
    c = CAND.search(txt)
    cand = 'scratch/' + c.group(1) if c else None
    live = cand and os.path.exists(cand)
    d = DIFF.search(txt)
    rows.append((int(d.group(1)) if d else 999, m.group(1), p, cand if live else '-'))
print(f"parks touching memory via raw access: {len(rows)}")
print(f"{'differ':>7}  {'function':26} candidate")
for d, fn, p, c in sorted(rows)[:18]:
    print(f"{(d if d!=999 else '?'):>7}  {fn:26} {c}")
