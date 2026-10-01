#!/usr/bin/env python3
"""Variant sweep in ONE container. Fails loudly.
Usage: sweep.py BASE REF FUNC VARIANTDIR   -- measures BASE then every .c in VARIANTDIR
Prints: NAME ndiff first size reloc
"""
import os, sys, subprocess, tempfile, glob
# Import THE AUTHORITY, tools/objcmp.py, not a copy of it.
#
# This line used to read `sys.path.insert(0, "/work/scratch_elev/b314e")` and
# import a 403-line FORK of objcmp.py living in a GITIGNORED scratch directory.
# It worked only while that directory happened to exist, would break on a fresh
# clone, and -- worse -- meant figures came from a SECOND copy of the authority
# that could drift from it silently.  The fork differed by exactly two
# environment hooks, which are now in tools/objcmp.py itself, defaulting to the
# previous behaviour.
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__))))
os.environ.setdefault("OBJCMP_ROOT", "/work")
import objcmp as O

base, ref, func, vdir = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4]
for p in (base, ref):
    if not os.path.exists(p): sys.exit("HARNESS FAIL: missing %s" % p)
if not os.path.isdir(vdir): sys.exit("HARNESS FAIL: no variant dir %s" % vdir)

t = tempfile.mkdtemp()
rs = os.path.join(t, "r.s")
if func:
    open(rs, "w").writelines(O.one_function(ref, func))
else:
    open(rs, "w").write(open(ref, errors="ignore").read())
refo = os.path.join(t, "r.o")
p = subprocess.run(O.AS + ["-I" + os.path.dirname(os.path.abspath(ref)), "-o", refo, rs],
                   capture_output=True, text=True)
if p.returncode: sys.exit("HARNESS FAIL: ref assemble\n" + p.stderr)
A_enc, A_rel, A_sz = O.dump(refo)
flags, _ = O.cflags_for(ref)

def measure(src):
    cs = os.path.join(t, "c.s"); co = os.path.join(t, "c.o")
    p = subprocess.run([os.path.join(O.GCC, "xgcc")] + flags + ["-S", "-o", cs, src],
                       capture_output=True, text=True)
    if p.returncode: return ("COMPILEFAIL", None, None, None)
    open(cs, "a").write("\n\t.text\n\t.align\t2, 0\n")
    p = subprocess.run(O.AS + ["-o", co, cs], capture_output=True, text=True)
    if p.returncode: return ("ASMFAIL", None, None, None)
    B_enc, B_rel, B_sz = O.dump(co)
    k = sum(1 for x, y in zip(A_enc, B_enc) if x != y) + abs(len(A_enc) - len(B_enc))
    first = next((i for i, (x, y) in enumerate(zip(A_enc, B_enc)) if x != y), -1)
    rb, _ = O.reloc_diff(A_rel, B_rel)
    return (k, first, B_sz - A_sz, "RELOCDIFF" if rb else "ok")

r = measure(base)
print("%-34s ndiff=%-5s first=%-5s dsize=%-4s rel=%s" % ("BASE", r[0], r[1], r[2], r[3]))
sys.stdout.flush()
for v in sorted(glob.glob(os.path.join(vdir, "*.c"))):
    r = measure(v)
    print("%-34s ndiff=%-5s first=%-5s dsize=%-4s rel=%s" % (os.path.basename(v), r[0], r[1], r[2], r[3]))
    sys.stdout.flush()
