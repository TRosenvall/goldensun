import os, re, subprocess, sys
idx = {}
for root, _, files in os.walk('asm'):
    for f in files:
        if f.endswith('.s'):
            p = os.path.join(root, f)
            for m in re.finditer(r'^\.thumb_func_start\s+(\S+)', open(p, errors='ignore').read(), re.M):
                idx.setdefault(m.group(1), p)
DECL = re.compile(r"^extern\s+void\s+([A-Za-z_]\w*)\s*\([^;]*\);\s*$", re.M)
FN = re.compile(r"^(?:\w[\w \*]*?)\b(\w+)\s*\([^;]*\)\s*$", re.M)

def score(text, ref):
    open('scratch/_pl.c', 'w').write(text)
    o = subprocess.run(['python3', 'tools/tryc.py', 'scratch/_pl.c', '--ref', ref, '--quiet'],
                       capture_output=True, text=True).stdout
    l = (o.strip().splitlines() or ['(none)'])[0]
    if ' OK ' in l:
        return 0
    m = re.search(r'(\d+) differ', l)
    return int(m.group(1)) if m else 10 ** 9

for c in sys.argv[1:]:
    src = open(c).read()
    names = [n for n in FN.findall(src) if n in idx]
    if not names:
        print(f"{c}: no reference found"); continue
    ref = idx[names[0]]
    cur, dropped = score(src, ref), []
    pool = DECL.findall(src)
    while True:
        best, pick = cur, None
        for n in pool:
            if n in dropped:
                continue
            t = DECL.sub(lambda m: '' if m.group(1) in dropped + [n] else m.group(0), src)
            s = score(t, ref)
            if s < best:
                best, pick = s, n
        if pick is None:
            break
        dropped.append(pick); cur = best
        if cur == 0:
            break
    tag = 'MATCH' if cur == 0 else ''
    print(f"{c:<26} {names[0]:<26} -> {cur:>6} drop={','.join(dropped) or '-'} {tag}", flush=True)
