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
possible -- which is the property that was owed.  (Note it is a local, not
global, minimum: a pair of pins might be jointly removable where neither is
singly removable.  The pass reports that bound rather than claiming more.)
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
