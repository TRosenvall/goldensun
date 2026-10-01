import re,sys
f,s,e,name=sys.argv[1],int(sys.argv[2]),int(sys.argv[3]),sys.argv[4]
lines=open(f).read().split('\n')[s-1:e]
ins=[]
for i,l in enumerate(lines):
    if l.lstrip().startswith('.call_via'): ins.append((i+s,'.call_via',l.split()[-1])); continue
    m=re.match(r'^\s+([a-z][a-z0-9]*)\s*(.*)$',l)
    if m and not l.lstrip().startswith('.'): ins.append((i+s,m.group(1),m.group(2).split('@')[0].strip()))
HI={'r8','r9','r10','r11'}
def val(k,reg,d=0):
    if d>8: return '?'
    for j in range(k-1,-1,-1):
        i,op,o=ins[j]
        p=[x.strip() for x in o.split(',')]
        if not p: continue
        if op in ('bl','.call_via') and reg in ('r0','r1','r2','r3'): return 'CALLRET'
        if p[0]!=reg: continue
        if op=='ldr' and len(p)>1 and p[1].startswith('='): return 'POOL '+p[1][1:]
        if op.startswith('ldr'): return 'MEM'
        if op=='mov' and p[1].startswith('#'): return 'IMM '+p[1][1:]
        if op=='mov' and re.match(r'r\d+$',p[1]): return val(j,p[1],d+1)
        if op in ('lsl','lsr','asr'):
            base=p[1] if len(p)>=3 and re.match(r'r\d+$',p[1]) else reg
            inner=val(j,base,d+1); sh=p[-1]
            if inner.startswith('IMM '):
                try: return 'BUILT 0x%x (%s<<%s)'%(int(inner[4:],0)<<int(sh.lstrip('#'),0),inner[4:],sh)
                except: return 'BUILT '+inner
            return 'BUILT('+inner+')'
        if op=='add' and len(p)==2 and p[1].startswith('#'): return 'ADD'+p[1]+'->'+val(j,reg,d+1)
        return 'OTHER:'+op
    return 'ARG'
reads={}
for k,(i,op,o) in enumerate(ins):
    p=[x.strip() for x in o.split(',')]
    if op=='mov' and len(p)==2 and p[1] in HI and re.match(r'r[0-7]$',p[0]): reads[p[1]]=reads.get(p[1],0)+1
defs={}
for k,(i,op,o) in enumerate(ins):
    p=[x.strip() for x in o.split(',')]
    if len(p)<2: continue
    if p[0] in HI and op in ('mov','add','sub') and re.match(r'r[0-7]$|#',p[1]):
        v = val(k,p[1]) if re.match(r'r[0-7]$',p[1]) else 'IMM '+p[1].lstrip('#')
        defs.setdefault((p[0],op,v),[]).append(i)
print("=== %s  (reads of high regs: %s)"%(name,reads))
for (r,op,v),ls in sorted(defs.items(), key=lambda x:(x[0][0],-len(x[1]))):
    print("  %-4s %-4s <- %-34s x%-3d first lines %s"%(r,op,v,len(ls),ls[:5]))
