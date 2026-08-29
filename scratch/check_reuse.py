import os, sys, subprocess
sys.path.insert(0, 'tools')
import pool

def body_of(fn):
    """Prefer asm/; fall back to the saved oneref for functions since elevated."""
    out = subprocess.run([sys.executable, 'tools/showfunc.py', fn],
                         capture_output=True, text=True).stdout
    if '.thumb_func_start' not in out:
        p = os.path.join('scratch', fn + '.s')
        out = open(p, errors='replace').read()
    b = out[out.index('.thumb_func_start'):]
    return b[:b.index('.func_end')].split('\n')

cases = [
    ('OvlFunc_966_2009090', 'PARKED constant_reuse (0x100 x4)', True),
    ('OvlFunc_965_2008eac', 'PARKED constant_reuse (-1 x3)',    True),
    ('OvlFunc_974_2008bb8', 'MATCHED exactly (from saved ref)', False),
    ('OvlFunc_944_2008468', 'PARKED interleave, not reuse',     False),
    ('OvlFunc_969_200db90', 'PARKED scheduling, not reuse',     False),
    ('Func_80218dc',        'PARKED arg order, not reuse',      False),
]
bad = 0
for fn, note, expect in cases:
    r = pool.reuse(body_of(fn))
    ok = (r > 0) == expect
    bad += not ok
    print("%s reuse=%2d  expect%s  %-22s %s"
          % ('ok ' if ok else 'BAD', r, '>0' if expect else '=0', fn, note))
print("\nDETECTOR IS WRONG\n" if bad else "\nall agree with the recorded outcomes\n")
