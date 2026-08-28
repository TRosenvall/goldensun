import os, re, collections

PARKS = sorted(os.path.join(r, f) for r, _, fs in os.walk('src/non_matching')
               for f in fs if f.endswith('.c'))

# A park names its function in the FIRST comment line, in one of four shapes:
#   Func_X -- 0xADDR      Func_X  [dir]  --  0xADDR
#   Func_X @ 0xADDR       Func_X -- asm/path.s
NAME = re.compile(r'^\s*/?\*?\s*((?:Ovl)?Func_\w+|[A-Z][A-Za-z0-9_]+)\s*(?:\[[^\]]*\])?\s*(?:--|@)')

def names_in(path, txt):
    """Functions this file is the park FOR (header line only, not cross-refs)."""
    for line in txt.split('\n')[:3]:
        m = NAME.match(line)
        if m and not line.strip().startswith('* Twin of'):
            return [m.group(1)]
    return []

CLASSES = [
    ('interleave / arg-setup order', r'interleav|argument[- ]setup|arg(ument)? order'),
    ('constant reuse', r'constant[- ]reuse|commons|commoned|rebuilds? (it|the)'),
    ('register-role swap', r'register[- ]role|roles? .{0,12}swap|registers exchanged'),
    ('scheduling / placement', r'schedul|hoist'),
    ('symbol base / fold', r'symbol[- ]base|displacement|relocation'),
    ('HImode / halfword literal', r'HImode|halfword constant'),
    ('division helper symbol', r'divsi3'),
]

cls = collections.Counter(); cls_files = collections.defaultdict(list)
owner = collections.defaultdict(list); classdocs = []

for p in PARKS:
    txt = open(p, errors='replace').read()
    ns = names_in(p, txt)
    if not ns:
        classdocs.append(p); continue
    owner[ns[0]].append(p)
    hit = [c for c, pat in CLASSES if re.search(pat, txt, re.I)]
    for c in (hit or ['unclassified']):
        cls[c] += 1; cls_files[c].append(p)

print("== VALIDATION (must all say ok) ==")
checks = [("Func_80ab1f4 parked twice", len(owner.get('Func_80ab1f4', [])) == 2),
          ("free parked twice",         len(owner.get('free', [])) == 2),
          ("names found for most files", len(owner) > 0.7 * (len(PARKS) - len(classdocs)))]
for label, ok in checks:
    print(f"  {'ok ' if ok else 'BAD'} {label}")

print(f"\n{len(PARKS)} files = {sum(len(v) for v in owner.values())} function parks"
      f" + {len(classdocs)} blocker-class documents")
print(f"{len(owner)} distinct functions parked\n")

print("BLOCKER CLASS  (a park may carry several)")
for c, n in cls.most_common():
    print(f"{n:5}  {c}")

print("\nDUPLICATE PARKS (same function, >1 file):")
for f, ps in sorted(owner.items()):
    if len(ps) > 1:
        print(f"  {f}")
        for q in ps: print(f"      {q}")
