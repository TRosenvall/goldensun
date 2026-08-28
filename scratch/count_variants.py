import os, re
START = re.compile(r'^\s*\.(thumb_func_start(?:_noalign)?|arm_func_start)\s+(\S+)')
GCCFN = re.compile(r'^\s*\.thumb_func\b')

tot = skipped_tu = 0
names_all, names_skipped = set(), set()
gen = 0
for root, _, files in os.walk('asm'):
    for fn in files:
        if not fn.endswith('.s'):
            continue
        p = os.path.join(root, fn)
        has_c = os.path.exists(p.replace('asm/', 'src/', 1)[:-2] + '.c')
        txt = open(p, errors='ignore').read()
        if GCCFN.search(txt) and not START.search(txt):
            gen += 1          # gcc-generated intermediate, no ROM-notation funcs
        found = [m.group(2) for m in (START.match(l) for l in txt.split('\n')) if m]
        tot += len(found)
        names_all.update(found)
        if has_c:
            skipped_tu += len(found)
        else:
            names_skipped.update(found)

print("A  all .thumb_func_start in asm/            :", tot)
print("B  distinct names                            :", len(names_all))
print("C  excluding TUs that have a mirrored .c     :", tot - skipped_tu)
print("D  distinct names, excluding those TUs       :", len(names_skipped))
print("   (gcc-generated .s files with no ROM funcs :", gen, ")")
