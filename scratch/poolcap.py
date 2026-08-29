import os, re
GEN = re.compile(r'^\t\.thumb_func\n\t\.type\t +(\S+),function\n\1:', re.M)
POOL = re.compile(r'^\s*\.pool(_aligned)?\s*$')
gen_pool = 0; gen_total = 0; longest = 0; longname=''
for root,_,fs in os.walk('asm'):
    for f in fs:
        if not f.endswith('.s'): continue
        p=os.path.join(root,f)
        if not os.path.exists(p.replace('asm/','src/',1)[:-2]+'.c'): continue
        t=open(p,errors='ignore').read()
        if POOL.search(t): gen_pool += 1
        st=list(GEN.finditer(t))
        for k,m in enumerate(st):
            end=st[k+1].start() if k+1<len(st) else len(t)
            body=t[m.start():end]
            gen_total += 1
            n=len([l for l in body.split('\n') if l.startswith('\t')])
            if n>longest: longest, longname = n, m.group(1)
print(f"GENERATED .s files containing a .pool directive anywhere: {gen_pool}")
print(f"generated functions: {gen_total}, longest = {longest} insns ({longname})")
