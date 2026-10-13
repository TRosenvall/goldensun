"""harvest.py -- which parks are CLOSEST TO LANDING, ranked by a figure that is
actually a distance.

    python3 tools/harvest.py                  # the ranked list
    python3 tools/harvest.py --max-figure 5   # only the tightest
    python3 tools/harvest.py --unswept        # only parks with no recorded sweep

WHY THIS EXISTS.  Elevating a park has TWO STAGES and this project built an
instrument for only the first one:

  STAGE 1  close the length gap.  Expensive -- it needs a hypothesis, dumps, and
           usually an agent-round.  `tools/gapscan.py` ranks this stage.
  STAGE 2  land from equal length.  Cheap -- the figure is a real distance, the
           residue is local, and the two mechanical axes often finish it.

Batches 344-347 spent every agent slot on stage 1, which is correct work and
closed 22 net gaps.  But each closure DEPOSITS A PARK INTO STAGE 2, and nothing
was draining it.  Measured when this was written:

    222 parks at EQUAL instruction length
     71 of them at a REAL figure <= 10
     60 of those device-free
     31 of those NEVER SWEPT

Thirty-one device-free parks at a real distance of ten or less, with both cheap
axes unclosed.  Batch 345's free pass landed TWO parks from this band for zero
tokens, and batch 338's landed two more.  That is the pace problem: not that the
tools are wrong, but that the cheap half of the pipeline had no instrument
pointing at it, so it silted up while agents did the expensive half.

## WHY RANKING ON THE FIGURE IS ONLY VALID HERE

A park's figure is a POSITIONAL count, so it measures distance ONLY when the
16-bit instruction counts agree -- otherwise it is counting phase offset (see
docs/elevation.md, "A PARK'S FIGURE IS NOT A DISTANCE").  This tool therefore
refuses to rank anything whose counts differ: that park belongs to gapscan, not
here.  It also requires equal BYTE SIZE and equal POOL COUNT, because those are
two further independent length dimensions and a park failing either is still
partly out of phase.

So every row this prints has all three lengths equal, which is exactly the
condition under which "N differing encodings" means "N things to fix".

## WHAT THE COLUMNS MEAN

    figure    differing encodings; a REAL distance on every row here
    dev       devices from shimcount (pins, .equ shims, address devices).
              A park with devices is not a candidate for a clean landing.
    swept     whether the header records an autosweep result.  BLANK IS THE
              OPPORTUNITY -- it means the two mechanical axes have never been
              tried, and those axes have landed six parks across four batches
              with no agent involved.
"""
import argparse
import glob
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DOCKER = ["docker", "run", "--rm", "--security-opt", "seccomp=unconfined",
          "-v", f"{ROOT}:/work", "-w", "/work", "goldensun-build"]
# The continuation line of a park's verify command begins with a COMMENT
# ASTERISK, so the separator class must allow it -- the omission once hid 550 of
# 606 parks from gapscan and read as a clean negative.
VERIFY = re.compile(r"objcmp\.py[\s\\*]+(src/\S+\.c)[\s\\*]+(asm/\S+\.s)"
                    r"(?:[\s\\*]+--func\s+(\S+))?")
SWEPT = re.compile(r"autosweep|anchor axis|cheap axes|FLAG AXIS", re.I)


def measure(park):
    """-> (figure, equal_lengths, detail) or None when it did not measure."""
    m = VERIFY.search(open(park, errors="replace").read())
    if not m:
        return None
    cmd = DOCKER + ["python3", "tools/objcmp.py", m.group(1), m.group(2)]
    if m.group(3):
        cmd += ["--func", m.group(3)]
    out = subprocess.run(cmd, capture_output=True, text=True).stdout
    if re.search(r"^\s+OK\b", out, re.M):
        return (0, True, "ALREADY BYTE-IDENTICAL -- check why it is still parked")
    fm = re.search(r"differ in (\d+) place", out)
    if not fm:
        return None
    im = re.search(r"instructions are (\d+)/(\d+)", out)
    if not im or im.group(1) != im.group(2):
        return (int(fm.group(1)), False, "instruction counts differ")
    # Byte size and pool count are two further independent length dimensions.
    if re.search(r"XX SIZE", out):
        return (int(fm.group(1)), False, "byte size differs")
    if re.search(r"XX POOL", out):
        return (int(fm.group(1)), False, "pool/32-bit entry count differs")
    return (int(fm.group(1)), True, "")


def devices(parks):
    """{park: n devices} from shimcount, which prints findings under each path."""
    if not parks:
        return {}
    out = subprocess.run([sys.executable, os.path.join(ROOT, "tools/shimcount.py")]
                         + parks, capture_output=True, text=True).stdout
    res, cur = {}, None
    for ln in out.splitlines():
        if ln.startswith("src/"):
            cur = ln.strip()
            res[cur] = 0
        elif ln.strip() and cur:
            res[cur] += 1
    return res


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--max-figure", type=int, default=10)
    ap.add_argument("--unswept", action="store_true",
                    help="only parks whose header records no autosweep result")
    a = ap.parse_args()

    rows, skipped = [], 0
    for p in sorted(glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"),
                              recursive=True)):
        rel = os.path.relpath(p, ROOT)
        r = measure(rel)
        if r is None:
            continue
        fig, equal, why = r
        if not equal:
            skipped += 1
            continue
        if fig > a.max_figure:
            continue
        rows.append((fig, rel, why))

    dev = devices([r[1] for r in rows])
    rows.sort()
    out = []
    for fig, rel, why in rows:
        swept = bool(SWEPT.search(open(os.path.join(ROOT, rel),
                                      errors="replace").read()))
        if a.unswept and swept:
            continue
        out.append((fig, rel, dev.get(rel, 0), swept, why))

    print(f"{len(out)} park(s) at ALL THREE LENGTHS EQUAL and figure "
          f"<= {a.max_figure}")
    print(f"({skipped} more are still out of phase on some dimension -- those "
          f"belong to gapscan.py)\n")
    print(f"{'figure':>6} {'dev':>4} {'swept':>6}  park")
    for fig, rel, d, swept, why in out:
        print(f"{fig:>6} {d:>4} {'yes' if swept else '':>6}  "
              f"{rel.replace('src/non_matching/', '')}"
              + (f"   {why}" if why else ""))
    ready = [r for r in out if r[2] == 0 and not r[3]]
    print(f"\n{len(ready)} are DEVICE-FREE AND NEVER SWEPT -- run autosweep on "
          f"these first.")
    print("That route has landed six parks across four batches with no agent "
          "involved.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
