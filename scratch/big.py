import re, glob
FS = re.compile(r"^\.thumb_func_start (\S+)", re.M)
CODE = re.compile(r"^\t[a-z]")
out = []
for p in glob.glob("asm/**/*.s", recursive=True):
    t = open(p, errors="replace").read()
    parts = FS.split(t)
    for i in range(1, len(parts), 2):
        body = parts[i+1].split(".func_end")[0]
        n = sum(1 for l in body.split("\n") if CODE.match(l) and ".word" not in l)
        if n >= 1001:
            calls = len(re.findall(r"^\tbl\t", body, re.M))
            labels = len(re.findall(r"^\.L\w+:", body, re.M))
            out.append((n, calls, labels, parts[i], p))
for n, c, l, name, p in sorted(out)[:12]:
    print(f"{n:6}  calls={c:4} labels={l:4}  {name:26} {p}")
print(f"\n{len(out)} functions of 1001+ instructions")
