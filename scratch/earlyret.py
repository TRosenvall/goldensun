import os, re
hits=[]
for root,_,fs in os.walk('src/non_matching'):
    for f in fs:
        if not f.endswith('.c'): continue
        p=os.path.join(root,f)
        t=open(p,errors='ignore').read()
        code=re.sub(r'/\*.*?\*/','',t,flags=re.S)
        if not re.search(r'\w',code): continue
        # an early guard returning a constant, with more code after it
        m=re.search(r'if\s*\([^)]*==\s*0\s*\)\s*\n?\s*return\s+0\s*;', code)
        if m and len(code[m.end():].strip()) > 80:
            hits.append(p)
print(f"{len(hits)} parks have an early `if (x == 0) return 0;` guard with work after it\n")
for p in hits: print("  "+p)
