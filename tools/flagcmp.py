"""GUARD ADDED ON INSTALL (batch 307).
flagcmp exists because objcmp and aligncmp take their flags from the Makefile row, so a
flag with NO row cannot otherwise be screened.  That makes it a SCREENING tool only.

IT MUST NEVER SUPPLY A PARK CLAIM LINE.  parkcheck re-measures the claim with objcmp at
PRODUCTION flags; a per-flag figure written there reads as a park lying about its own
body, which has now happened four times in this tree (three aligncmp figures in batch 301
and one -fno-rerun-cse-after-loop figure in batch 303).  Quote a flagcmp number anywhere
else in a header, labelled with the flag.

And a flag that helps is not a landing route unless it becomes a Makefile row -- check the
CSE_CFLAGS precondition first, because where a save-flag id is read ONCE the cse flags are
byte-identical to the default and no row should be written.
"""
#!/usr/bin/env python3
"""flagcmp.py -- aligncmp + size/count, with EXTRA CFLAGS injected.

    python3 scratch_elev/b307g/flagcmp.py <cand.c> <ref.s> <Func> [--extra "-fno-x -ffixed-r8"]

Exists only because objcmp/aligncmp take their flags from the Makefile row for
the reference, so a flag that has no row yet cannot be screened.  The park's
CLAIM LINE must still come from plain objcmp at production flags; this is for
ranking candidates during the search.
"""
import difflib, os, subprocess, sys, tempfile
ROOT = "/work" if os.path.isdir("/work/asm") else \
    os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import objcmp

cand, ref, name = sys.argv[1], sys.argv[2], sys.argv[3]
extra = []
if "--extra" in sys.argv:
    extra = sys.argv[sys.argv.index("--extra") + 1].split()

tmp = tempfile.mkdtemp()
flags, _ = objcmp.cflags_for(ref)
flags = flags + extra
co, cs = os.path.join(tmp, "c.o"), os.path.join(tmp, "c.s")
subprocess.run([objcmp.GCC + "/xgcc", "-B" + objcmp.GCC + "/"] + flags +
               ["-I" + os.path.join(ROOT, "include"), "-S", "-o", cs, cand], check=True)
subprocess.run(objcmp.AS + [cs, "-o", co], check=True)
rs, ro = os.path.join(tmp, "r.s"), os.path.join(tmp, "r.o")
open(rs, "w").writelines(objcmp.one_function(ref, name))
subprocess.run(objcmp.AS + [rs, "-o", ro], check=True)


def dis(obj):
    d = subprocess.run(["arm-none-eabi-objdump", "-dz", obj],
                       capture_output=True, text=True).stdout
    enc, txt = [], []
    for l in d.splitlines():
        m = objcmp.ENC.match(l)
        if m:
            enc.append(m.group(1).strip())
            txt.append(l.split("\t", 2)[2].strip() if l.count("\t") >= 2 else "")
    return enc, txt


byf = objcmp.dump_by_function(co)
ours = None
for n, e in byf:
    if n == name:
        ours = e
oenc, otxt = dis(co)
renc, rtxt = dis(ro)
if ours:
    oenc, otxt = ours, otxt[:len(ours)]
_, _, rsz = objcmp.dump(ro)
_, _, csz = objcmp.dump(co)
sm = difflib.SequenceMatcher(None, renc, oenc, autojunk=False)
eq = hunks = delta = 0
for tag, i1, i2, j1, j2 in sm.get_opcodes():
    if tag == "equal":
        eq += i2 - i1
    else:
        hunks += 1
        delta += max(i2 - i1, j2 - j1)
raw = sum(1 for i in range(min(len(renc), len(oenc))) if renc[i] != oenc[i]) \
    + abs(len(renc) - len(oenc))
print("flags extra: %s" % (" ".join(extra) or "(none)"))
print("size  ref %s ours %s   count ref %d ours %d   raw-unaligned %d"
      % (rsz, csz, len(renc), len(oenc), raw))
print("aligned-equal %d  (%.1f%% of ref)   differing/ins/del %d in %d hunks"
      % (eq, 100.0 * eq / len(renc), delta, hunks))
if "-v" in sys.argv:
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            continue
        print("--- hunk %s ref[%d:%d] ours[%d:%d]" % (tag, i1, i2, j1, j2))
        for k in range(i1, min(i2, i1 + 6)):
            print("    ref  %-10s %s" % (renc[k], rtxt[k]))
        for k in range(j1, min(j2, j1 + 6)):
            print("    ours %-10s %s" % (oenc[k], otxt[k]))
