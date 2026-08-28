import os, re, sys
sys.path.insert(0, 'tools')
import pool

PARKS = sorted(os.path.join(r, f) for r, _, fs in os.walk('src/non_matching')
               for f in fs if f.endswith('.c'))
NAME = re.compile(r'^\s*/?\*?\s*((?:Ovl)?Func_\w+|[A-Z][A-Za-z0-9_]+)\s*(?:\[[^\]]*\])?\s*(?:--|@)')

seen = pool.parked_names()          # what pool.py actually excludes today
real, missed = set(), []
for p in PARKS:
    for line in open(p, errors='replace').read().split('\n')[:3]:
        m = NAME.match(line)
        if m:
            n = m.group(1); real.add(n)
            if n not in seen:
                missed.append((n, p))
            break
print(f"function parks found by header      : {len(real)}")
print(f"names pool.py currently excludes    : {len(seen)}")
print(f"PARKED BUT NOT EXCLUDED (the leak)  : {len(missed)}")
for n, p in sorted(missed)[:15]:
    print(f"    {n:30} {p}")
if len(missed) > 15:
    print(f"    ... and {len(missed)-15} more")
