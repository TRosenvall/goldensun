#!/usr/bin/env python3
"""Triage the unattempted functions on the axes that actually predict difficulty.

WHY THIS EXISTS.  Two rankings have been tried and both were wrong:

  * INSTRUCTION COUNT.  Batch 311 ranked five functions by length and the order was
    wrong -- one target was 115 instructions LONGER than another yet much easier,
    because it had no aggregates.  Length is not the axis.
  * BRANCH DENSITY.  Batch 310 found 14 of 18 branch targets were POOL SKIPS, so the
    count is mostly noise.

What does predict, and what this script measures:

  frame / movsp   THE FRAME TRIAD.  `sub sp,#imm` is only the first of three greps;
                  `mov rX,sp` reveals aggregates the first grep cannot see.  Each
                  aggregate is a quantity whose slot you must place, and aggregate
                  order is REVERSED relative to the scalars, so aggregates cost far
                  more than instructions do.  A function with NO frame has no spill
                  map at all and none of the declaration-order material applies.
  hi              high-register mentions (r8-r11/sl/fp).  *** THIS MEASURES
                  HIGH-REGISTER PRESSURE, WHATEVER ITS CAUSE -- NOT the
                  wide-constant reuse fraction. ***  Batch 313: the top-ranked
                  function at 191 mentions turned out to be dominated by ONE
                  memory-loaded pointer and its 28 reads, and another with 58
                  copies had ZERO constant-rooted ones, so the pin pass had no
                  domain there at all.  Use tools/hipartition.py, which traces
                  each copy back to the ROOT of its value and is VALIDATED against
                  this document's hand-computed table on four functions.
  maxrl / distc   the pooled-constant multiset -- the most-reloaded value's count,
                  and how many distinct values are pooled.  *** maxrl
                  UNPARTITIONED IS ACTIVELY MISLEADING, NOT MERELY WEAK. ***  The
                  highest maxreload in batch 313 was 23, and all 23 were ONE ARRAY
                  BASE ADDRESS (an 84-byte table read `ldrb [base,index]` at 23
                  sites).  Partitioned, that function had the LOWEST true-numeric
                  ratio of its group -- 15 of 89 pool refs -- making it the
                  CLOSEST to a pure rebuild, the exact opposite of what the figure
                  suggests.  Partition pool references into NUMERIC CONSTANTS
                  against SYMBOL/ARRAY-BASE ADDRESSES before reading anything off
                  this column.
  reuse           `mov rlo,rhigh` copies.

  maxrl + reuse together are the PURE-REBUILD vs MIXED discriminator (batch 311):
  nothing reloaded more than ~3x with few reuse copies means PURE REBUILD, where one
  blanket pin pass over every literal argument outside 0..255 is step 1 and took
  OvlFunc_969_200cbec from -28/-14 to both axes exact in a single step.  A value
  reloaded 7-8 times WITH ~30 reuse copies means MIXED, which that pass cannot
  finish, because a pin forces the REGISTER not the REBUILD and it fights naming at
  exactly the sites where both apply.

  bne / sgn       a COMPARISON census -- and *** IT DOES NOT COUNT LOOPS. ***
                  Batch 313 classified every BACKWARD EDGE in a function this
                  column called signed-dominant (48 signed against 16 bne) and
                  found ELEVEN LOOPS, TEN CLOSING ON bne/beq: the signed compares
                  were clamps, signed-division corrections and frame-phase tests,
                  none of them loop closures.  The `!=` lever's applicability
                  INVERTED.  Classify backward edges (target address below the
                  branch) and read what each closes on; this column is a hint
                  about arithmetic, not about loop form.  It MUST be read PER
                  FUNCTION.
                  Anim_Gaia's loops are all `bne` and spelling every loop `!=` was
                  worth 23 encodings; its sibling Anim_ScreenShatter has 48 signed
                  comparisons, where the same change would corrupt 48 sites.
  jt              `.word .L` entries, i.e. real jump tables.  Do not infer these from
                  what a function appears to do: batch 311 predicted switch material
                  would pay on five menu/field functions that had ZERO jump tables
                  between them, while the one target WITH dispatches had the FEWEST
                  high-register mentions.  The two axes were inversely related.

The name list is hardcoded from `tools/census.py --list`; refresh it when the
available set changes.  Ranking is by (maxrl + reuse, frame + 100*aggregates), i.e.
purest-rebuild and simplest-frame first.
"""

import re, os, subprocess, sys, collections

names = """Func_8090a5c OvlFunc_880_20083cc OvlFunc_951_2008e5c BaseAnim_Tiamat Anim_Kirin
OvlFunc_882_200b1ac OvlFunc_884_20097c8 Anim_ScreenShatter BaseAnim_Bite_Sting Func_80a2680
Anim_Boreas OvlFunc_969_20092c8 Func_8023178 Func_80be378 OvlFunc_968_200b068 Anim_Cybele
OvlFunc_899_200b6f8 Anim_Neptune Anim_Procne BaseAnim_Meteor Func_8026080 BaseAnim_ParticleSpray
Anim_Thor Func_8027114 Func_80f6440 OvlFunc_969_200a360 OvlFunc_897_2009410 OvlFunc_883_20095dc
LuckyDiceMain OvlFunc_883_200bfb0 OvlFunc_896_200a7f8 OvlFunc_957_20093f8 Anim_Judgment
Func_80bbb0c OvlFunc_913_2008d3c OvlFunc_911_20088ec UpdateActors""".split()

# index every .thumb_func_start in asm/
loc = {}
for root, dirs, files in os.walk("asm"):
    for fn in files:
        if not fn.endswith(".s"): continue
        p = os.path.join(root, fn)
        try: txt = open(p, errors="replace").read()
        except: continue
        for m in re.finditer(r"^\.thumb_func_start\s+(\S+)", txt, re.M):
            loc.setdefault(m.group(1), []).append(p)

def slice_fn(path, name):
    txt = open(path, errors="replace").read().split("\n")
    out, on = [], False
    for ln in txt:
        if re.match(r"^\.thumb_func_start\s+%s\b" % re.escape(name), ln):
            on = True; continue
        if on and re.match(r"^\.(thumb_func_start|arm_func_start)\b", ln): break
        if on and re.match(r"^\.func_end", ln): break
        if on: out.append(ln)
    return out

rows = []
for n in names:
    paths = loc.get(n)
    if not paths:
        rows.append((n, "NOT FOUND", 0,0,0,0,0,0,0,0,0,0,0,0.0,0,0,0,0,0)); continue
    p = paths[0]
    body = slice_fn(p, n)
    txt = "\n".join(body)
    insns = sum(1 for l in body if re.match(r"^\t[a-z]", l))
    # THE FRAME, and the first grep is NOT enough.
    #
    # Thumb-1 `sub sp,#imm` caps at 508 bytes, so a function with a LARGER frame
    # cannot use that form at all and builds it via a register instead:
    #     ldr r5, =0xfffffddc / add sp, r5      (= -548)
    # With only the `sub sp,#imm` grep such a function reports frame 0 and reads
    # as FRAMELESS.  That is exactly backwards -- it happened on
    # OvlFunc_880_20083cc in batch 312, whose 548-byte frame with six aggregates
    # makes it the HARDEST frame of its group while being SHORTER than its
    # siblings, and the tool ranked it easiest.  ANY FRAME OVER 508 BYTES WAS
    # INVISIBLE.
    m = re.search(r"sub\s+sp,\s*#(0x[0-9a-f]+|\d+)", txt)
    frame = int(m.group(1), 0) if m else 0
    if not frame and re.search(r"(?:add|sub)\s+sp,\s*r[0-9]+", txt):
        # register-built frame: recover the size from the negative constant the
        # register is loaded with.  Reported as a lower bound if not found.
        for mm in re.finditer(r"=\s*(0x[fF][fF][0-9a-fA-F]{6})", txt):
            v = int(mm.group(1), 16)
            if v > 0x7fffffff:
                frame = 0x100000000 - v
                break
        if not frame:
            frame = -1   # register-built, size not recovered
    # AGGREGATES.  `mov rX, sp` is only ONE of the two forms and is absent from
    # 20083cc entirely; `add rX, sp, #K` is the other and is the one it uses.
    # Count both (batch 312 correction).
    # ...BUT a raw count of `add rX,sp,#K` OVER-REPORTS aggregates badly.
    # Thumb-1 has NO sp-relative ldrh/strh/ldrb/strb, so EVERY SUB-WORD STACK
    # SCALAR must materialise a base register too, and looks identical to an
    # aggregate at this grep.  Measured on four functions in batch 313, the raw
    # count gave 11/12/9/1 where the true aggregate counts are 1/4/0/0 -- and the
    # difficulty ranking inverted, because I had assigned work by that number.
    # This resolves SOME of them by FIRST USE of the materialised register -- a
    # sub-word load/store means a sub-word scalar -- but the column is only an
    # UPPER BOUND and must be read as one.  Checked against hand-measurement on
    # the same four functions, this heuristic still gave 10/11/4/0 where careful
    # REGION READING gave 1/4/0/0.  A mechanical first-use scan cannot see a
    # base register that is re-used across blocks, passed on, or walked, so
    # A TRUE AGGREGATE COUNT NEEDS THE REGION READ.  Use this column to decide
    # WHERE to look, never to rank difficulty on its own -- which is exactly the
    # mistake it was introduced to fix.
    #
    # The scan MUST be BLOCK-AWARE.  An agent's first-use scan crossed a loop
    # head and misclassified a walking pointer as a scalar; stop at any label or
    # branch rather than running on.  Note also that this listing writes small
    # immediates in DECIMAL (`#8`, not `#0x8`).
    SUBWORD = re.compile(r"^\t(ldr|str)(h|b|sh|sb)\b")
    BLOCKEND = re.compile(r"^(\.L\w+:|\t(b|bl|bx|beq|bne|blt|ble|bgt|bge|bhi|bls|bcc|bcs)\b)")
    lines = txt.split("\n")
    aggr, subword, unresolved = 0, 0, 0
    for i, ln in enumerate(lines):
        m = re.match(r"\tadd\s+(r\d+),\s*sp,\s*#", ln)
        if not m:
            continue
        reg = m.group(1)
        verdict = None
        for j in range(i + 1, min(i + 40, len(lines))):
            if BLOCKEND.match(lines[j]):
                break
            if re.search(r"\b%s\b" % reg, lines[j]):
                verdict = "sub" if SUBWORD.match(lines[j]) else "agg"
                break
        if verdict == "sub":
            subword += 1
        elif verdict == "agg":
            aggr += 1
        else:
            unresolved += 1
    movsp = len(re.findall(r"mov\s+r\d+,\s*sp", txt)) + aggr
    addsp = len(re.findall(r"add\s+r\d+,\s*sp", txt))
    # OUTGOING ARGUMENT SPACE, a FOURTH check the triad lacked: `str rX,[sp]`
    # with no matching load is argument space for a 5+-argument call, and it is
    # invisible to the other three because offset 0 forms no address.  Pairing
    # against loads is the discriminator; the raw store count is the screen.
    sp0 = len(re.findall(r"str\s+r\d+,\s*\[sp\]", txt))
    # LABEL COUNT -- separates straight-line from branch-dense, which decides
    # WHICH LEVER SET applies (band-800plus.md section 1).  Three of batch 312's
    # nine targets were assigned straight-line constant-reuse material and were
    # branch-dense (61, 76 and 79 labels); it is one grep and was missing here.
    # NOTE a pool skip is still a basic-block boundary to every per-block pass,
    # so these are NOT discountable the way the batch-310 branch-target count was.
    labels = len(re.findall(r"^\.L\w+:", txt, re.M))
    # high registers
    hi = len(re.findall(r"\b(r8|r9|r10|r11|sl|fp)\b", txt))
    # pooled constants: ldr rX, =VALUE  -> multiset
    pool = re.findall(r"ldr\s+r\d+,\s*=(\S+)", txt)
    cnt = collections.Counter(pool)
    maxreload = max(cnt.values()) if cnt else 0
    distinct = len(cnt)
    # reuse copies: mov rlo, rhigh
    reuse = len(re.findall(r"mov\s+r[0-7],\s*(r8|r9|r10|r11|sl|fp)\b", txt))
    # loop comparison census
    bne = len(re.findall(r"\bbne\b", txt))
    signed = len(re.findall(r"\b(blt|ble|bgt|bge)\b", txt))
    # jump tables
    # `.word .L` counts jump-table ENTRIES, not TABLES.  Calling this column "jt"
    # cost a brief a bad premise ("33 jump tables" was 2 tables of 22 and 11
    # entries).  Count dispatch SITES separately.
    jt = len(re.findall(r"\.word\s+\.L", txt))
    tables = len(re.findall(r"^\t(?:mov|ldr|add)\s+pc\b", txt, re.M))
    # WORK DENSITY -- the axis that orders the call-script population, which the
    # high-register axis cannot (batch 312: high-reg ran 17/19/16 across three
    # targets and the HARDEST had the FEWEST).  These are cutscene scripts at ~3
    # instructions per call where almost everything is argument-fill, so a
    # function that barely computes has no wide constants to reuse.  Approximated
    # as: instructions that are neither a call nor a write to an argument
    # register r0-r3.
    #
    # THIS COLUMN IS A PROXY AND IT IS TOO CRUDE.  Measured against a DATAFLOW
    # classifier (constant-origin set per basic block) it gave 3.4/4.0/4.5/4.9
    # where the truth is 2.9/6.8/6.3/6.4 -- INVERTING the ranking: the function it
    # called easiest carries 142 work instructions against another's 30.  The
    # defect is an ADJACENCY assumption: these scripts INTERLEAVE argument fills,
    #     mov r1,#imm / mov r2,#imm / lsl r1,#8 / lsl r2,#7 / bl
    # so no `lsl` sits beside its own `mov`, and 173 of one function's 196 `lsl`
    # read as "work".  The `reuse` column has the same defect -- genuine
    # non-constant copies were 42/14/44/28 where this tool listed the FEWEST for
    # the function that has the MOST.
    #
    # THE AXIS THAT ACTUALLY RANKS THE CALL-SCRIPT POPULATION IS UNRESOLVED DRAFT
    # LINES: what tools/draft_script.py cannot resolve (memory operations and
    # over-guessed arities), confirmed independently twice, 44/117/171/214 across
    # one brief's four.  It is a COUNT OF WORK TO DO rather than a proxy for it.
    #
    # Every mechanical proxy built here has been defeated by a shape it could not
    # see, and each replacement was itself too crude one layer down.  USE THESE
    # COLUMNS TO DECIDE WHERE TO LOOK; DO NOT RANK DIFFICULTY ON THEM.
    bl = len(re.findall(r"^\t(bl|blx)\b", txt, re.M))
    argfill = len(re.findall(r"^\t[a-z]+\s+r[0-3],", txt, re.M))
    work = max(insns - bl - argfill, 0)
    wd = (100.0 * work / insns) if insns else 0.0
    rows.append((n, p, insns, frame, movsp, addsp, hi, maxreload, distinct, reuse, bne, signed, jt, wd, sp0, labels, subword, unresolved, tables))

print("%-26s %5s %6s %5s %4s %5s %5s %5s %4s %4s %4s %6s %4s %4s %5s %3s %4s" %
      ("function","insn","frame","agg<=","hi","maxrl","distc","reuse","bne","sgn","jt","work%","sp0","lbl","subw","?","tbl"))
# rank: pure-rebuild candidates first (low maxreload, low reuse), small frame, no aggregates
def key(r):
    if r[1]=="NOT FOUND": return (9,0)
    return (r[7] + r[9], (r[3] if r[3] > 0 else 600) + 100*r[4])
for r in sorted(rows, key=key):
    if r[1]=="NOT FOUND":
        print("%-26s  NOT FOUND" % r[0]); continue
    n,p,insns,frame,movsp,addsp,hi,maxrl,distc,reuse,bne,sgn,jt,wd,sp0,labels,subw,unres,tables = r
    fs = "reg?" if frame == -1 else hex(frame)
    print("%-26s %5d %6s %5d %4d %5d %5d %5d %4d %4d %4d %5.1f %4d %4d %5d %3d %4d" %
          (n, insns, fs, movsp, hi, maxrl, distc, reuse, bne, sgn, jt, wd, sp0, labels, subw, unres, tables))
