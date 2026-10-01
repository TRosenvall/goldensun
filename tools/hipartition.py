import re, sys
f,s,e,name = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), sys.argv[4]
lines = open(f).read().split('\n')[s-1:e]
ins=[]
for i,l in enumerate(lines):
    if l.lstrip().startswith('.call_via'):
        ins.append((i,'.call_via',l.split()[-1])); continue
    m=re.match(r'^\s+([a-z][a-z0-9]*)\s*(.*)$', l)
    if m and not l.lstrip().startswith('.'):
        ins.append((i,m.group(1),m.group(2).split('@')[0].strip()))
HI={'r8','r9','r10','r11','sl','fp'}
DEFOPS={'mov','ldr','add','sub','lsl','lsr','asr','and','orr','eor','mul','neg','mvn','ldrb','ldrh','ldrsh','ldrsb','adc','sbc','ror','bic','tst'}
def ops_of(x): return [p.strip() for p in x.split(',')]
def find_def(k,reg,depth=0):
    """return root class of value in reg at instruction index k (exclusive)"""
    if depth>8: return 'deep'
    for j in range(k-1,-1,-1):
        i,op,o=ins[j]
        p=ops_of(o)
        if op in ('bl','.call_via') and reg in ('r0','r1','r2','r3'):
            return 'callret'
        if op in DEFOPS and p and p[0]==reg:
            src = p[1] if len(p)>1 else ''
            if op=='ldr' and src.startswith('='):
                return 'POOL-CONST' if re.match(r'=(0x|\d)',src) else 'POOL-SYM'
            if op in ('ldr','ldrb','ldrh','ldrsh','ldrsb'):
                return 'MEM'
            if op=='mov' and src.startswith('#'):
                return 'IMM8'
            if op=='mov' and re.match(r'r\d+$|sl$|fp$',src):
                return find_def(j,src,depth+1)
            if op in ('lsl','lsr','asr','neg','mvn'):
                # unary: root = operand (src reg if 3-operand, else same reg)
                if len(p)>=3 and re.match(r'r\d+$',p[1]):
                    r0 = find_def(j,p[1],depth+1)
                else:
                    r0 = find_def(j,reg,depth+1)
                # a shift/neg APPLIED to an immediate is a MULTI-INSTRUCTION CONSTANT,
                # not an imm8.  This is the class the doc's partition measures.
                if r0 in ('IMM8','MULTI-INSN-CONST'): return 'MULTI-INSN-CONST'
                if r0 == 'POOL-CONST': return 'MULTI-INSN-CONST'
                return r0
            if op in ('add','sub','mul','and','orr','eor','bic','adc','sbc'):
                # binary accumulate into reg: root of reg itself + other operand
                if len(p)==2 and p[1].startswith('#'):
                    return find_def(j,reg,depth+1)
                regs=[x for x in p[1:] if re.match(r'r\d+$|sl$|fp$',x)]
                if p[0]=='sp' or 'sp' in p: return 'SP'
                roots=set()
                base = p[1] if len(p)>=3 else reg
                roots.add(find_def(j,base,depth+1))
                if len(p)>=3 and re.match(r'r\d+$',p[2]): roots.add(find_def(j,p[2],depth+1))
                elif len(p)==2 and regs: roots.add(find_def(j,regs[0],depth+1))
                if 'MEM' in roots or 'callret' in roots: return 'COMPUTED(mem)'
                if roots<= {'IMM8','POOL-CONST','deep'}: return 'MULTI-INSN-CONST'
                return 'COMPUTED(%s)'%'/'.join(sorted(roots))
            return 'other:'+op
    return 'arg/prologue'
cnt={}; det=[]
for k,(i,op,o) in enumerate(ins):
    if op!='mov': continue
    p=ops_of(o)
    if len(p)!=2: continue
    d,sr=p
    if d in HI or sr not in HI: continue
    c=find_def(k,sr)
    cnt[c]=cnt.get(c,0)+1
    det.append((i+s,sr,c))
print('===',name,'| mov rlo,rhigh =',sum(cnt.values()))
for c,n in sorted(cnt.items(),key=lambda x:-x[1]): print('   %-22s %d'%(c,n))
# per high register
per={}
for ln,sr,c in det: per.setdefault(sr,{}).setdefault(c,0); per[sr][c]+=1
for r in sorted(per): print('   ',r,per[r])
