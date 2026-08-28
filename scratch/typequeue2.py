import os, re
PARKS = sorted(os.path.join(r, f) for r, _, fs in os.walk('src/non_matching')
               for f in fs if f.endswith('.c'))
HDR = re.compile(r'^\s*/?\*?\s*((?:Ovl)?Func_\w+|[A-Za-z][A-Za-z0-9_]+)\s*(?:\[[^\]]*\])?\s*(?:--|@)')
# the BLOCKER sentence must be about memory access, not argument order
BLK = re.compile(r'BLOCKER[^\n]*\n(?:\s*\*[^\n]*\n){0,8}')
MEM = re.compile(r'\b(str|ldr|strb|ldrb|strh|ldrh|ldrsh)\b|address|pointer|offset|base', re.I)
ARG = re.compile(r'argument[- ]setup|interleav|arg order', re.I)
DIFF = re.compile(r'\b(\d+)\s+differ', re.I)
rows = []
for p in PARKS:
    txt = open(p, errors='replace').read()
    m = next((HDR.match(l) for l in txt.split('\n')[:3] if HDR.match(l)), None)
    if not m: continue
    b = BLK.search(txt)
    if not b: continue
    blk = b.group(0)
    if not MEM.search(blk) or ARG.search(blk):
        continue
    d = DIFF.search(txt)
    rows.append((int(d.group(1)) if d else 999, m.group(1), p))
print(f"parks whose BLOCKER is memory-shaped: {len(rows)}\n")
for d, fn, p in sorted(rows)[:16]:
    print(f"{(d if d!=999 else '?'):>5} differ  {fn:26} {p}")
