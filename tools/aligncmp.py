#!/usr/bin/env python3
"""aligncmp.py -- position-tolerant residue view for LONG reconstructions.

    python3 tools/aligncmp.py <cand.c> <ref.s> <FuncName> [-v]

WHY THIS EXISTS.  objcmp is the authority on byte-exactness and nothing else
settles that -- but its difference COUNT compares encodings index by index with
NO alignment, so as soon as one instruction is missing or extra, every index
after it differs and the count saturates.

Measured on BaseAnim_SpecialAttack (3,070 instructions): three materially
different candidates -- a revision, a declaration-reorder variant, and a
two-declaration swap probe -- ALL read exactly 3250 of 3380 differing, despite
hundreds of changed spill-slot offsets between them.  The count could not see any
of it.  On the scale below the same candidates read 49.7%, 49.6% and 54.6%.

So use objcmp for the verdict and for any function with a true distance (size AND
instruction count equal, where its count IS meaningful), and use this when a long
reconstruction is still short or long by a few instructions and you need to know
whether a change helped.

MASKING RULES: NONE.  Raw 16/32-bit encodings as objdump -dz prints them are
compared verbatim; the only tolerance is ALIGNMENT (an LCS over the two encoding
streams), so an inserted or deleted instruction no longer poisons every later
index.  A PC-relative load whose pool offset moved STILL COUNTS AS A DIFFERENCE.
The single place this is looser than objcmp: relocated words (bl targets, .word
symbol) read as 0 in both streams and so compare equal.

That paragraph is not decoration.  The first normalizer written for this problem
masked registers, branch targets AND pool constants, and reported 90.7% "match"
on a reconstruction whose encodings were 96% wrong.  Any helper metric here must
state its masking rules beside every number it prints, which is why they are in
this docstring and in the output header.
"""

import difflib, os, subprocess, sys, tempfile, re
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import objcmp

cand, ref, name = sys.argv[1], sys.argv[2], sys.argv[3]
tmp = tempfile.mkdtemp()
flags, _ = objcmp.cflags_for(ref)
co = os.path.join(tmp, "c.o")
subprocess.run([objcmp.GCC + "/xgcc", "-B" + objcmp.GCC + "/"] + flags +
               [os.path.join("-I" + ROOT, "include"), "-S", "-o", os.path.join(tmp, "c.s"), cand], check=True)
subprocess.run(objcmp.AS + [os.path.join(tmp, "c.s"), "-o", co], check=True)
rs = os.path.join(tmp, "r.s")
open(rs, "w").writelines(objcmp.one_function(ref, name))
ro = os.path.join(tmp, "r.o")
subprocess.run(objcmp.AS + [rs, "-o", ro], check=True)

def dis(obj):
    d = subprocess.run(["arm-none-eabi-objdump", "-dz", obj], capture_output=True, text=True).stdout
    enc, txt = [], []
    for l in d.splitlines():
        m = objcmp.ENC.match(l)
        if m:
            enc.append(m.group(1).strip())
            txt.append(l.split("\t", 2)[2].strip() if l.count("\t") >= 2 else "")
    return enc, txt

# our object may hold more than the one function; cut by symbol
byf = objcmp.dump_by_function(co)
ours = None
for n, e in byf:
    if n == name: ours = e
oenc, otxt = dis(co)
renc, rtxt = dis(ro)
# restrict ours to the function's encodings
if ours: oenc = ours; otxt = otxt[:len(ours)]
sm = difflib.SequenceMatcher(None, renc, oenc, autojunk=False)
eq = 0; hunks = 0; delta = 0
for tag, i1, i2, j1, j2 in sm.get_opcodes():
    if tag == "equal": eq += i2 - i1
    else:
        hunks += 1
        delta += max(i2 - i1, j2 - j1)
print("ref %d encodings, ours %d" % (len(renc), len(oenc)))
print("aligned-equal %d  (%.1f%% of ref)   differing/ins/del %d in %d hunks"
      % (eq, 100.0 * eq / len(renc), delta, hunks))
if len(sys.argv) > 4:
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal": continue
        print("--- hunk %s ref[%d:%d] ours[%d:%d]" % (tag, i1, i2, j1, j2))
        for k in range(i1, min(i2, i1 + 6)): print("    ref  %-10s %s" % (renc[k], rtxt[k]))
        for k in range(j1, min(j2, j1 + 6)): print("    ours %-10s %s" % (oenc[k], otxt[k]))
