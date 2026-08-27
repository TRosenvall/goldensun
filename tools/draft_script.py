#!/usr/bin/env python3
"""draft_script.py -- produce a FIRST DRAFT of the C body for a straight-line
cutscene function, by simulating the register file across argument setup.

WHY

53 functions of 1000+ instructions remain, and the largest single group of them
is cutscene script: a few hundred calls, four instructions of argument setup
each, and almost no control flow. OvlFunc_883_200b4c8 is 1024 instructions with
264 calls and THREE labels.

Transcribing 264 calls by hand is not hard, it is just long -- and long is
exactly where transcription errors come from. One wrong immediate in call 180
looks identical, in the screen output, to a subtle codegen difference. This
emits the obvious reading of each call so the work becomes REVIEW rather than
typing.

WHAT IT DOES AND DOES NOT DO

It tracks a symbolic value per register through `mov`, `neg`, `lsl`, `add`,
`sub` and `ldr =`, records stack stores, and prints one line per `bl`. It does
NOT understand control flow: a label ends the simulation for the registers that
could differ on the other path, and it says so rather than guessing.

    docker run --rm -v "$PWD:/work" -w /work goldensun-build \\
        python3 tools/draft_script.py <function-name>

THE OUTPUT IS A DRAFT, NOT AN ANSWER. Three things it gets wrong by design:

  * ARITY is guessed from which of r0-r3 were written since the previous call,
    plus any stack slot written since. A callee that ignores r3, or one whose
    r2 happens to be left over from the previous call, will be mis-typed. Every
    call site should be checked against the real signature.
  * VALUES HELD ACROSS CALLS in callee-saved registers print as `r5` etc. Those
    are the ones that usually want a named local, and deciding that is the
    reader's job -- see the survives-a-call note in docs/elevation.md, which is
    necessary but not sufficient.
  * ANYTHING AFTER THE FIRST LABEL is printed with a marker and unknown
    registers, because the value could have come from either path.

Screen the result like anything else. The point is to arrive at the first
screen with the arithmetic already right.
"""
import re
import subprocess
import sys

MOVI = re.compile(r"^\tmov\t(r\d+|sl|fp|ip|lr), #(0x[0-9a-f]+|\d+)$")
MOVR = re.compile(r"^\tmov\t(r\d+|sl|fp|ip|lr), (r\d+|sl|fp|ip|lr|sp)$")
NEG = re.compile(r"^\tneg\t(r\d+), (r\d+)$")
LSL2 = re.compile(r"^\tlsl\t(r\d+), #(0x[0-9a-f]+|\d+)$")
LSL3 = re.compile(r"^\tlsl\t(r\d+), (r\d+), #(0x[0-9a-f]+|\d+)$")
ADDI = re.compile(r"^\tadd\t(r\d+), #(0x[0-9a-f]+|\d+)$")
SUBI = re.compile(r"^\tsub\t(r\d+), #(0x[0-9a-f]+|\d+)$")
LDRE = re.compile(r"^\tldr\t(r\d+), =(\S+)$")
STRSP = re.compile(r"^\tstr\t(r\d+), \[sp(?:, #(0x[0-9a-f]+|\d+))?\]$")
BL = re.compile(r"^\tbl\t(\S+)$")
LABEL = re.compile(r"^(\.L\w+):")
OTHER = re.compile(r"^\t[a-z]")


def num(s):
    return int(s, 16) if s.startswith("0x") else int(s)


def hexs(v):
    if isinstance(v, str):
        return v
    if -9 <= v <= 9:
        return str(v)
    return hex(v) if v >= 0 else "-" + hex(-v)


def main():
    if len(sys.argv) < 2:
        sys.exit("usage: draft_script.py <function>")
    fn = sys.argv[1]
    out = subprocess.run([sys.executable, "tools/showfunc.py", fn],
                         capture_output=True, text=True).stdout
    body = out[out.index(".thumb_func_start"):]
    body = body[:body.index(".func_end")]

    reg = {}          # register -> int value or opaque string
    stack = {}        # byte offset -> value
    dirty = set()     # registers written since the last bl
    sdirty = set()    # stack offsets written since the last bl
    ncalls = 0
    after_label = False

    for line in body.split("\n"):
        m = LABEL.match(line)
        if m:
            print(f"  /* ---- {m.group(1)}: control flow joins here;"
                  f" register state below is UNRELIABLE ---- */")
            reg.clear()
            stack.clear()
            after_label = True
            continue
        m = BL.match(line)
        if m:
            ncalls += 1
            args = []
            top = -1
            for i in range(4):
                if f"r{i}" in dirty:
                    top = i
            for i in range(top + 1):
                args.append(hexs(reg.get(f"r{i}", f"r{i}?")))
            for off in sorted(sdirty):
                args.append(hexs(stack.get(off, f"sp+{off}?")))
            print(f"    {m.group(1)}({', '.join(args)});")
            reg = {k: v for k, v in reg.items() if k not in ("r0", "r1", "r2", "r3")}
            reg["r0"] = f"<{m.group(1)}>"
            dirty.clear()
            sdirty.clear()
            continue
        for rx, fn_ in ((MOVI, "movi"), (MOVR, "movr"), (NEG, "neg"),
                        (LSL3, "lsl3"), (LSL2, "lsl2"), (ADDI, "addi"),
                        (SUBI, "subi"), (LDRE, "ldre"), (STRSP, "strsp")):
            m = rx.match(line)
            if not m:
                continue
            if fn_ == "movi":
                reg[m.group(1)] = num(m.group(2)); dirty.add(m.group(1))
            elif fn_ == "movr":
                reg[m.group(1)] = reg.get(m.group(2), m.group(2))
                dirty.add(m.group(1))
            elif fn_ == "neg":
                v = reg.get(m.group(2))
                reg[m.group(1)] = -v if isinstance(v, int) else f"-({v})"
                dirty.add(m.group(1))
            elif fn_ == "lsl3":
                v = reg.get(m.group(2))
                n = num(m.group(3))
                reg[m.group(1)] = (v << n) if isinstance(v, int) else f"({v} << {n})"
                dirty.add(m.group(1))
            elif fn_ == "lsl2":
                v = reg.get(m.group(1))
                n = num(m.group(2))
                reg[m.group(1)] = (v << n) if isinstance(v, int) else f"({v} << {n})"
                dirty.add(m.group(1))
            elif fn_ == "addi":
                v = reg.get(m.group(1))
                n = num(m.group(2))
                reg[m.group(1)] = (v + n) if isinstance(v, int) else f"({v} + {n})"
                dirty.add(m.group(1))
            elif fn_ == "subi":
                v = reg.get(m.group(1))
                n = num(m.group(2))
                reg[m.group(1)] = (v - n) if isinstance(v, int) else f"({v} - {n})"
                dirty.add(m.group(1))
            elif fn_ == "ldre":
                reg[m.group(1)] = m.group(2); dirty.add(m.group(1))
            elif fn_ == "strsp":
                off = num(m.group(2)) if m.group(2) else 0
                stack[off] = reg.get(m.group(1), m.group(1))
                sdirty.add(off)
            break
        else:
            if OTHER.match(line) and not line.startswith("\tbl"):
                op = line.strip().split("\t")[0]
                if op not in ("push", "pop", "bx", "b", "nop"):
                    print(f"    /* ?? {line.strip()} */")
                    for r in re.findall(r"r\d+", line.split(",")[0]):
                        reg.pop(r, None)
                        dirty.add(r)

    print(f"\n  /* {ncalls} calls drafted"
          f"{'; SOME AFTER A LABEL -- recheck those' if after_label else ''} */")
    return 0


if __name__ == "__main__":
    sys.exit(main())
