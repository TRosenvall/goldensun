import os, re
GEN = re.compile(r'^\t\.thumb_func\n\t\.type\t +(\S+),function\n\1:', re.M)
WORD = re.compile(r'^\t\.word\t(.+)$')
CODE = re.compile(r'^\t[a-z]')
tables=0; consts=0; examples=[]
for root,_,fs in os.walk('asm'):
    for f in fs:
        if not f.endswith('.s'): continue
        p=os.path.join(root,f)
        if not os.path.exists(p.replace('asm/','src/',1)[:-2]+'.c'): continue
        t=open(p,errors='ignore').read()
        st=list(GEN.finditer(t))
        for k,m in enumerate(st):
            end=st[k+1].start() if k+1<len(st) else len(t)
            lines=t[m.start():end].split('\n')
            seen=False; kinds=set()
            for l in lines:
                w=WORD.match(l)
                if w:
                    seen=True
                    kinds.add('label' if w.group(1).startswith('.L') else 'const')
                elif seen and CODE.match(l):
                    if 'const' in kinds:
                        consts+=1
                        if len(examples)<4: examples.append((m.group(1),p))
                    else: tables+=1
                    break
print(f"MATCHING functions with mid-body .word followed by code:")
print(f"   jump tables only : {tables}")
print(f"   LITERAL CONSTANTS: {consts}")
for n,p in examples: print(f"      {n}  {p}")
