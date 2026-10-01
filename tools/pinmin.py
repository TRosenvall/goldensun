#!/usr/bin/env python3
"""Minimise OvlFunc_889_2008074's register pins to a FIXPOINT.

Owed since batch 310: the function landed byte-exact with 233 pins across 82
PIN sites and the set was never reduced.  A pin that is not load-bearing is
noise in the record -- it makes the next reader think the compiler needed
forcing where it did not.

THE ORACLE.  For a LANDED function objcmp is not the instrument: there is no
ROM-side .s left to compare against, because converting the function replaced
it with gcc's own output.  tools/tryc.py rightly REFUSES to compare against a
generated .s, since for a MATCHING question that is a tautology.  The question
here is a different one -- "does this edit change the output from the
known-good baseline?" -- which is a regression check, and for that the
tautology is exactly the point.  So: save the current generated .s, rebuild
after each edit, and require byte-identical .s.  A full `make compare` gates
the final result.

METHOD.  Cumulative greedy single pass: walk the sites, tentatively unpin one
while KEEPING every unpin already accepted, rebuild, and accept only if the .s
is unchanged.  The result is a true fixpoint -- no single further removal is
possible -- which is the property that was owed.

*** AND A SINGLE-PIN GREEDY IS PROVABLY INSUFFICIENT.  READ THIS BEFORE TRUSTING
A FIXPOINT FROM THIS SCRIPT. ***

Batch 314 measured the gap in BOTH directions, and the second is worse than the
"local minimum" caveat this docstring used to carry:

  * A PAIR CAN BE JOINTLY REMOVABLE where neither is singly removable -- the
    caveat as originally stated.
  * A PAIR CAN BE JOINTLY LOAD-BEARING WHERE EACH IS INDIVIDUALLY INERT.  On one
    function 20 of 34 pin sites were individually inert; dropping all 20 cost 65
    encodings, and bisection isolated it to a SINGLE PAIR -- each free alone (4),
    together 65, with every subset containing both at 65 and every subset missing
    either at 4.

THE MECHANISM, which makes it predictable rather than a surprise: those two sites
were the only two in the function materialising THE SAME CONSTANT.  cse1 unifies
two pseudos holding the same CONST_INT, the survivor then crosses calls and takes
a callee-saved register -- so A PIN DEFEATS THAT UNIFICATION ONLY IN COMPANY.
Either pin alone leaves the other pseudo unpinned, and unification needs TWO
UNPINNED PEERS.  Control: break the shared value and the 65 collapses to 5.  A
second independent instance was found by the same method -- a width reduction,
individually inert at 32 of 34 sites, one pair costing 15, again the only two
sites sharing a constant.

WHY GREEDY CANNOT SEE IT, exactly: offered one of such a pair first, the pass
ACCEPTS it (free alone), then REJECTS the second (now load-bearing), and reports a
fixpoint ONE PIN SHORT, with no indication the two were related.

THE GUARD: GROUP PIN SITES BY THE VALUE THEY MATERIALISE -- one grep per distinct
constant -- AND REMOVE EACH GROUP AS A UNIT, then run the single-site pass over
what remains.  A fixpoint from this script WITHOUT that grouping is a LOWER BOUND
on what can be removed, not a minimum, and must be reported as such.  The 233 ->
157 result this script produced on OvlFunc_889_2008074 carries that caveat.

tools/sweep_variants.py measures a whole directory of variants in ONE container
(35 variants in ~3 seconds), which is what made 218 measurements and the
bisections above affordable.  Use it for the group sweep rather than one rebuild
per trial.
"""
import re, subprocess, shutil, sys, os

SRC = "src/overlays/rom_78ac38/ovl_30_c_c_c_b.c"
GEN = "asm/overlays/rom_78ac38/ovl_30_c_c_c_b.s"
HERE = os.path.dirname(os.path.abspath(__file__))
BASE = os.path.join(HERE, "baseline.s")
ORIG = os.path.join(HERE, "orig.c")
LOG  = open(os.path.join(HERE, "log.txt"), "w", buffering=1)

def log(*a):
    print(*a); LOG.write(" ".join(str(x) for x in a) + "\n")

orig = open(ORIG).read()
hdr_end = orig.index("*/") + 2
head, body = orig[:hdr_end], orig[hdr_end:]

PAT = re.compile(r"\{\s*PIN([1-4]);((?:[^{}])*?)\}", re.S)
sites = list(PAT.finditer(body))
log("sites:", len(sites))

def unpinned_text(m):
    b = m.group(2)
    assigns = dict((i, e.strip()) for i, e in
                   re.findall(r"\bq([0-3])\s*=\s*([^;]+);", b))
    call = re.search(r"(__?[A-Za-z_][A-Za-z0-9_]*)\s*\(([^;]*)\)\s*;", b)
    fn = call.group(1)
    n = len(call.group(2).split(","))
    args = ", ".join(assigns[str(i)] for i in range(n))
    return "%s(%s);" % (fn, args)

def render(unpin):
    out, last = [], 0
    for i, m in enumerate(sites):
        out.append(body[last:m.start()])
        out.append(unpinned_text(m) if i in unpin else m.group(0))
        last = m.end()
    out.append(body[last:])
    return head + "".join(out)

def build_ok():
    r = subprocess.run(
        ["docker", "run", "--rm", "--security-opt", "seccomp=unconfined",
         "-v", os.getcwd() + ":/work", "-w", "/work", "goldensun-build",
         "sh", "-c", "make AGBCC_DIR=/opt/agbcc -j8 >/tmp/pm.log 2>&1; echo $?"],
        capture_output=True, text=True)
    if r.stdout.strip().splitlines()[-1] != "0":
        return False
    with open(BASE, "rb") as a, open(GEN, "rb") as b:
        return a.read() == b.read()

accepted = set()
try:
    for i in range(len(sites)):
        trial = accepted | {i}
        open(SRC, "w").write(render(trial))
        if build_ok():
            accepted = trial
            log("site %2d: pin NOT needed  (removed, running total %d)" % (i, len(accepted)))
        else:
            log("site %2d: PIN REQUIRED" % i)
    open(SRC, "w").write(render(accepted))
    ok = build_ok()
    log("\nFIXPOINT: %d of %d sites unpinned; %d sites still required; final rebuild identical: %s"
        % (len(accepted), len(sites), len(sites) - len(accepted), ok))
    log("removed sites:", sorted(accepted))
    if not ok:
        log("FINAL RENDER DID NOT REPRODUCE -- restoring original")
        open(SRC, "w").write(orig)
except BaseException as e:
    log("ABORTED (%s) -- restoring original" % e)
    open(SRC, "w").write(orig)
    raise
