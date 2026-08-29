import os
ROOT = '.'
n = 0
for root, _, files in os.walk('asm'):
    for fn in files:
        if not fn.endswith('.s'):
            continue
        p = os.path.join(root, fn)
        # skip TUs that have been elevated (a .c exists at the mirrored src path)
        if os.path.exists(p.replace('asm/', 'src/', 1)[:-2] + '.c'):
            continue
        with open(p, errors='ignore') as f:
            for l in f:
                if l.startswith('.thumb_func_start') or l.startswith('.arm_func_start'):
                    n += 1
print(n)
