import os, re
PARKS = sorted(os.path.join(r, f) for r, _, fs in os.walk('src/non_matching')
               for f in fs if f.endswith('.c'))
HDR = re.compile(r'^\s*/?\*?\s*((?:Ovl)?Func_\w+|[A-Za-z][A-Za-z0-9_]+)\s*(?:\[[^\]]*\])?\s*(?:--|@)')
ROLE = re.compile(r'register[- ]roles?|occupy each other|roles? .{0,15}swap|'
                  r'registers exchanged|r2/r3 exchange|each other.s registers', re.I)
DIFF = re.compile(r'(\d+)\s+(?:of\s+\d+\s+)?differ|(\d+)\s+of\s+(\d+)', re.I)
WORDS = {'TWO':2,'THREE':3,'FOUR':4,'FIVE':5,'SIX':6,'SEVEN':7,'TEN':10,'ELEVEN':11,'FIFTEEN':15}

rows = []
for p in PARKS:
    txt = open(p, errors='replace').read()
    if not ROLE.search(txt):
        continue
    m = next((HDR.match(l) for l in txt.split('\n')[:3] if HDR.match(l)), None)
    if not m:
        continue
    d = None
    # "N differing" / "N differ" is authoritative; only fall back to "N of M"
    # when no explicit differ-count exists, and take M-of-N's FIRST number only
    # if the sentence does not also say "lines".
    n = re.search(r'\b(\d+)\s+differ', txt, re.I)
    if n:
        d = int(n.group(1))
    else:
        w = re.search(r'\b(TWO|THREE|FOUR|FIVE|SIX|SEVEN|TEN|ELEVEN|FIFTEEN)\s+differ', txt, re.I)
        if w:
            d = WORDS[w.group(1).upper()]
        else:
            n2 = re.search(r'\b(\d+)\s+of\s+\d+(?!\s+lines)', txt)
            if n2: d = int(n2.group(1))
    rows.append((d if d is not None else 999, m.group(1), p))

print(f"parks whose notes describe a register-ROLE swap: {len(rows)}\n")
print(f"{'differ':>7}  function                      file")
for d, fn, p in sorted(rows):
    print(f"{(d if d != 999 else '?'):>7}  {fn:28}  {p}")
known = [d for d, _, _ in rows if d != 999]
print(f"\nwith a recorded distance: {len(known)}, median {sorted(known)[len(known)//2] if known else '-'}")
