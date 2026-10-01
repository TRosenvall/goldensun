"""Classify BACKWARD EDGES, not comparison mnemonics.

A raw mnemonic census counts COMPARISONS; loop form is decided only by what the
backward edges close on.  A backward edge is a branch whose target label is
DEFINED EARLIER in the function than the branch itself.
"""
import re,sys,collections
BR=re.compile(r'^\s+(b|beq|bne|blt|ble|bgt|bge|bhi|bls|bcc|bcs|bmi|bpl)\s+(\.L\w+)\s*$')
LBLDEF=re.compile(r'^(\.L\w+):')
for n in sys.argv[1:]:
    lines=open('scratch_elev/b313g/body_%s.s'%n).read().split('\n')
    pos={}
    for i,l in enumerate(lines):
        m=LBLDEF.match(l)
        if m: pos[m.group(1)]=i
    back=[]
    for i,l in enumerate(lines):
        m=BR.match(l)
        if m and m.group(2) in pos and pos[m.group(2)] < i:
            back.append((i+1,m.group(1),m.group(2),pos[m.group(2)]+1))
    c=collections.Counter(op for _,op,_,_ in back)
    print("### %s   BACKWARD EDGES = %d"%(n,len(back)))
    for ln,op,tgt,tl in back:
        print("    line %-5d %-4s -> %-12s (defined line %d)"%(ln,op,tgt,tl))
    print("    closes on: %s"%dict(c))
    eqne=c.get('bne',0)+c.get('beq',0)
    print("    == / != closures: %d of %d   signed closures: %d   unconditional: %d"
          %(eqne,len(back),sum(v for k,v in c.items() if k in('blt','ble','bgt','bge')),c.get('b',0)))
    print()
