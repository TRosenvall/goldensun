import os, re
BLOCK = re.compile(r"((?:\t(?:mov|lsl|neg|ldr|add|sub)\t[^\n]*\n)+)\tbl\t")
CHEAP = re.compile(r"\tmov\t(r[0-3]), #")
EXP   = re.compile(r"\t(?:lsl|neg)\t(r[0-3])|\tldr\t(r[0-3]), =")
def strict(body):
    """HANDOFF's rule: cheap constants mixed with TWO OR MORE expensive values,
    and a cheap one is not last."""
    for m in BLOCK.finditer(body):
        lines=[l for l in m.group(1).split('\n') if l]
        exp=set(); cheap_idx=[]
        for i,l in enumerate(lines):
            e=EXP.search(l)
            if e: exp.add(e.group(1) or e.group(2))
            c=CHEAP.match(l)
            if c: cheap_idx.append(i)
        if len(exp)>=2 and cheap_idx and cheap_idx[-1] != len(lines)-1:
            return True
    return False
GEN = re.compile(r'^\t\.thumb_func\n\t\.type\t +(\S+),function\n\1:', re.M)
SRC = re.compile(r'^\s*\.(?:thumb_func_start(?:_noalign)?|arm_func_start)\s+(\S+)', re.M)
def scan(generated):
    hit=tot=0
    for root,_,fs in os.walk('asm'):
        for f in fs:
            if not f.endswith('.s'): continue
            p=os.path.join(root,f)
            if os.path.exists(p.replace('asm/','src/',1)[:-2]+'.c') != generated: continue
            t=open(p,errors='ignore').read()
            st=list((GEN if generated else SRC).finditer(t))
            for k,m in enumerate(st):
                end=st[k+1].start() if k+1<len(st) else len(t)
                tot+=1
                if strict(t[m.start():end]): hit+=1
    return hit,tot
h1,t1=scan(False); h2,t2=scan(True)
print(f"STRICT precompute rule:")
print(f"  REMAINING flagged: {h1} of {t1}  ({100.0*h1/t1:.1f}%)")
print(f"  MATCHING  flagged: {h2} of {t2}  ({100.0*h2/t2:.1f}%)  <- false positives")

# print the matching counterexamples
ex=[]
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
            b=t[m.start():end]
            if strict(b):
                ex.append((len([l for l in b.split('\n') if l.startswith('\t')]), m.group(1), c))
ex.sort()
print("\nsmallest MATCHING functions with the strict shape:")
for n,name,c in ex[:5]: print(f"  {n:4d}  {name:26s} {c}")
