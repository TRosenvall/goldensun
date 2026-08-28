import os, re, sys
sys.path.insert(0, 'tools')
import pool
PARKS = sorted(os.path.join(r, f) for r, _, fs in os.walk('src/non_matching')
               for f in fs if f.endswith('.c'))
HDR = re.compile(r'^\s*/?\*?\s*((?:Ovl)?Func_\w+|[A-Za-z][A-Za-z0-9_]+)\s*(?:\[[^\]]*\])?\s*(?:--|@)')
real = set()
for p in PARKS:
    for line in open(p, errors='replace').read().split('\n')[:3]:
        m = HDR.match(line)
        if m: real.add(m.group(1)); break
seen = pool.parked_names()
print(f"header-named parks : {len(real)}")
print(f"pool now excludes  : {len(seen)}   (was 84)")
print(f"still leaking      : {len(real - seen)}")
for n in sorted(real - seen)[:8]: print("    missed:", n)
# over-match control: every excluded name should look like a real symbol
bad = [n for n in seen if not re.fullmatch(r'(Ovl)?Func_\w+|[A-Za-z]\w{2,}', n)]
print(f"malformed names captured (should be 0): {len(bad)} {bad[:5]}")
