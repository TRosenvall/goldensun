import os, re, subprocess, sys

PARKS = sorted(os.path.join(r, f) for r, _, fs in os.walk('src/non_matching')
               for f in fs if f.endswith('.c'))
HDR = re.compile(r'^\s*/?\*?\s*((?:Ovl)?Func_\w+|[A-Za-z][A-Za-z0-9_]+)\s*(?:\[[^\]]*\])?\s*(?:--|@)')
INTER = re.compile(r'interleav|argument[- ]setup|arg[- ]interleave', re.I)
COND = re.compile(r'^\tb(eq|ne|ge|gt|le|lt|hi|ls|cs|cc|mi|pl)\b')

guarded, straight, gone = [], [], []
for p in PARKS:
    txt = open(p, errors='replace').read()
    if not INTER.search(txt):
        continue
    m = None
    for line in txt.split('\n')[:3]:
        m = HDR.match(line)
        if m: break
    if not m:
        continue
    fn = m.group(1)
    out = subprocess.run([sys.executable, 'tools/showfunc.py', fn],
                         capture_output=True, text=True).stdout
    if '.thumb_func_start' not in out:
        gone.append((fn, p)); continue
    body = out[out.index('.thumb_func_start'):]
    body = body[:body.index('.func_end')] if '.func_end' in body else body
    n = sum(1 for l in body.split('\n') if COND.match(l))
    (guarded if n else straight).append((fn, n, p))

print(f"interleave-class parks with a live .s : {len(guarded) + len(straight)}")
print(f"  GUARDED   (has a conditional branch): {len(guarded)}   <- new lever should reach these")
print(f"  STRAIGHT  (no branch at all)        : {len(straight)}   <- no known lever")
print(f"  no live .s (already elevated/moved) : {len(gone)}")
print("\nGUARDED, smallest first:")
for fn, n, p in sorted(guarded, key=lambda t: t[1])[:14]:
    print(f"   {n:3} branches  {fn:28} {p}")
