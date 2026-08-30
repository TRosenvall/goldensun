import os, re, subprocess, sys, json
idx = {}
for root, _, files in os.walk('asm'):
    for f in files:
        if not f.endswith('.s'):
            continue
        p = os.path.join(root, f)
        try:
            t = open(p, errors='ignore').read()
        except OSError:
            continue
        for m in re.finditer(r'^\.thumb_func_start\s+(\S+)', t, re.M):
            idx.setdefault(m.group(1), p)
DECL = re.compile(r"^extern\s+void\s+([A-Za-z_]\w*)\s*\([^;]*\);\s*$", re.M)
FN = re.compile(r"^(?:\w[\w \*]*?)\b(\w+)\s*\([^;]*\)\s*$", re.M)
rows=[]
for c in sorted(open('scratch/_cands.txt').read().split()):
    if not os.path.exists(c):
        continue
    src = open(c).read()
    names = [n for n in FN.findall(src) if n in idx]
    if not names:
        continue
    ref = idx[names[0]]
    if not DECL.search(src):
        continue
    def run(text):
        open('scratch/_pl.c','w').write(text)
        o = subprocess.run(['python3','tools/tryc.py','scratch/_pl.c','--ref',ref,'--quiet'],
                           capture_output=True,text=True).stdout
        l = (o.strip().splitlines() or ['(none)'])[0]
        if ' OK ' in l: return 0, l.strip()
        m = re.search(r'(\d+) differ', l)
        return (int(m.group(1)) if m else 10**9), l.strip()
    a,_ = run(src)
    b,lb = run(DECL.sub('', src))
    flag = 'BETTER' if b < a else ''
    print(f"{c:<34} {names[0]:<28} {a:>7} -> {b:>7}  {flag}", flush=True)
