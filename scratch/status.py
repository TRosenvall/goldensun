import re, glob, os, collections
FS = re.compile(r"^\.thumb_func_start (\S+)", re.M)
CODE = re.compile(r"^\t[a-z]")
sizes = []
for p in glob.glob("asm/**/*.s", recursive=True):
    t = open(p, errors="replace").read()
    parts = FS.split(t)
    for i in range(1, len(parts), 2):
        body = parts[i+1].split(".func_end")[0]
        n = sum(1 for l in body.split("\n") if CODE.match(l) and ".word" not in l)
        sizes.append((n, parts[i], p))
bands = [(1,10),(11,20),(21,30),(31,50),(51,100),(101,200),(201,500),(501,1000),(1001,10**9)]
print(f"{'band':>14} {'count':>7} {'insns':>10}  cumulative %")
tot = len(sizes); tins = sum(s for s,_,_ in sizes); run = 0
for lo, hi in bands:
    sel = [s for s,_,_ in sizes if lo <= s <= hi]
    run += len(sel)
    lbl = f"{lo}-{hi}" if hi < 10**9 else f"{lo}+"
    print(f"{lbl:>14} {len(sel):7} {sum(sel):10}  {100*run/tot:5.1f}%")
print(f"{'TOTAL':>14} {tot:7} {tins:10}")
print()
print("median size:", sorted(s for s,_,_ in sizes)[tot//2])
