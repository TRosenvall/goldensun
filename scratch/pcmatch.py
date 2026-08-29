import os, re, sys
sys.path.insert(0,'tools'); import census
GEN = re.compile(r'^\t\.thumb_func\n\t\.type\t +(\S+),function\n\1:', re.M)
hits=[]
for root,_,fs in os.walk('asm'):
    for f in fs:
        if not f.endswith('.s'): continue
        p=os.path.join(root,f)
        c=p.replace('asm/','src/',1)[:-2]+'.c'
        if not os.path.exists(c): continue
        t=open(p,errors='ignore').read()
        st=list(GEN.finditer(t))
        for k,m in enumerate(st):
            end=st[k+1].start() if k+1<len(st) else len(t)
            body=t[m.start():end]
            if census.precompute(body):
                n=len([l for l in body.split('\n') if l.startswith('\t')])
                hits.append((n,m.group(1),c))
hits.sort()
print(f"{len(hits)} MATCHING functions have the precompute shape\n")
for n,name,c in hits[:8]: print(f"  {n:4d}  {name:26s} {c}")
