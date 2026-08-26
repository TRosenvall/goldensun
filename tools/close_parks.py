import glob, os, re, subprocess, sys
SRC = re.compile(r"Source asm:\s*goldensun/(\S+\.s)")
HEAD = re.compile(r"XX (\S+)\s+\(rom (\d+) lines, ours (\d+)[^,]*, first diff at (\d+), (\d+) differ")
out, seen = [], 0
for p in sorted(glob.glob("src/non_matching/**/*.c", recursive=True)):
    t = open(p, errors="replace").read()
    m = SRC.search(t)
    if not m or not os.path.exists(m.group(1)):
        continue
    seen += 1
    r = subprocess.run([sys.executable, "tools/tryc.py", p, "--ref", m.group(1)],
                       capture_output=True, text=True)
    for line in (r.stdout + r.stderr).split("\n"):
        h = HEAD.search(line)
        if h and int(h.group(5)) <= 6:
            out.append((int(h.group(5)), h.group(1), int(h.group(2)), int(h.group(3)), p))
for n, fn, a, b, p in sorted(out):
    print(f"{n:3} differ  {fn:28} rom {a:3} ours {b:3}  {p}")
print(f"\n{len(out)} of {seen} screened parks are within 6 instructions")
