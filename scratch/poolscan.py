import os, re
START = re.compile(r'^\s*\.(?:thumb_func_start(?:_noalign)?|arm_func_start)\s+(\S+)')
BR = re.compile(r'^\tb\t\.L\w+$')
blocked, total = [], 0
for root, _, files in os.walk('asm'):
    for fn in files:
        if not fn.endswith('.s'):
            continue
        p = os.path.join(root, fn)
        if os.path.exists(p.replace('asm/', 'src/', 1)[:-2] + '.c'):
            continue
        L = open(p, errors='ignore').read().split('\n')
        cur = None
        for i, l in enumerate(L):
            m = START.match(l)
            if m:
                cur = m.group(1); total += 1; continue
            if cur and l.strip() == '.pool_aligned':
                # a mid-function pool: preceded (within 3 lines) by an unconditional b
                if any(BR.match(L[j]) for j in range(max(0, i-3), i)):
                    blocked.append(cur); cur = None
print(f"remaining functions scanned            : {total}")
print(f"with a BRANCH-OVER-POOL (unreachable)  : {len(blocked)}")
print(f"  = {100.0*len(blocked)/total:.1f}% of what is left")
for n in blocked[:12]:
    print("   ", n)
if len(blocked) > 12:
    print(f"    ... and {len(blocked)-12} more")
