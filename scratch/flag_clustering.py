import re, os, collections

rules = collections.defaultdict(list)   # flag group -> [target .o]
cur = None
lines = open('Makefile').read().split('\n')
for i, l in enumerate(lines):
    m = re.match(r'^(asm/\S+\.o|asm/\S*%\S*\.o):\s', l)
    if m:
        cur = m.group(1)
        continue
    if cur:
        g = re.search(r'\$\((\w+_CFLAGS)\)', l)
        if g:
            rules[g.group(1)].append(cur)
            cur = None

print("group             files   distinct dirs   top directories")
for g, ts in sorted(rules.items(), key=lambda kv: -len(kv[1])):
    dirs = collections.Counter(os.path.dirname(t) for t in ts)
    top = ", ".join(f"{os.path.basename(d)}({n})" for d, n in dirs.most_common(3))
    print(f"{g:17} {len(ts):5}   {len(dirs):6}        {top}")
