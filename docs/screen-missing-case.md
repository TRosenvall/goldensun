# The missing-case recognition recipe, and the re-screen it makes possible

Batch 308, brief C.  Everything here is MEASURED in this session; the two
probe programs are `probe_thresh.c` (regenerable, see below) and `probe_loop.c`.

## 1. The two constants this rests on, both probed in THIS build

* **`case_values_threshold()` = 5.**  A `switch` on an `unsigned int` with 3 or
  4 dense case labels compiles to a DECISION TREE; with 5 or more it compiles to
  a `casesi` JUMP TABLE.  Probed with ten functions `T3..T12`, each `switch (x)`
  over `case 0..n-1` with distinct bodies: `T3`, `T4` gave trees, `T5`..`T12`
  gave `lsl r3,#2 / ldr r3,[r3,r2] / mov pc,r3` plus a `.word` table.
* **The tree/table test in `stmt.c`'s `expand_end_case`** is therefore

      tree  <=  count < 5  ||  (unsigned) range > 10 * count
      table <=  otherwise                    (range = maxval - minval)

  where `count` is the number of case NODES after `group_case_nodes` has merged
  contiguous same-label nodes into ranges.

## 2. The recipe, in the order to apply it

**STEP 0 -- is it even a gcc tree?**  `emit_case_nodes` tests an interior node
with `beq` and then RE-COMPARES THE SAME REGISTER AGAINST THE SAME CONSTANT for
the range branch:

    cmp rN, #K / b<eq> ..  /  cmp rN, #K / b<hi|cc|cs|ls|gt|lt> ..

A hand-written `if`/`else if` ladder never repeats a compare.  That doubled
compare is the signature, and it is what the screen script keys on.

**STEP 1 -- READ THE BRANCH MNEMONIC.**  `bcc`/`bcs`/`bhi`/`bls` mean the
selector is UNSIGNED; `bgt`/`blt`/`bge`/`ble` mean SIGNED.  This decides whether
a lowest node's test can collapse, and whether `case -1` sorts first or last.

**STEP 2 -- COUNT THE NODES, then split into the two cases below.**
Collect only the constants inside the doubled-compare cluster; a neighbouring
`if`'s `cmp rN,#0` sitting four instructions away will otherwise inflate the
count and produce a false positive (it did, twice, in this session).

### CASE A -- 3 or 4 nodes, span dense: THE TREE IS EXPLAINED BY THE THRESHOLD
Nothing is hidden.  Batch 307's lever still applies to the SHAPE -- with four
nodes `{0,1,2,3}` the bisect `i = (4 + 0 + 1) / 2 = 2` moves the head once, the
root becomes node 1, and for an UNSIGNED selector node 0 is bounded below so its
test collapses into the entry `bcc`.  So `bcc` on an unsigned selector over a
lopsided `{K, K, K+1}` test run IS four nodes and the lowest one IS unwritten.
But that lowest case is the fourth of four, not a fifth: do not go looking for an
out-of-range value as well.

VERIFIED HERE, two ways:
* `asm/rom_15000/rom_17e88_a_a_c.s` BufferString main switch, sub-list
  `{0,1,2,3}`: `cmp #1/beq .. / cmp #1/bcc .. / cmp #2/beq .. / cmp #3/beq ..`,
  and `case 0: case 2:` share one label while `case 1: case 3:` share another.
* `asm/rom_b5000/rom_b9b30_c_c_c.s:851` Func_80bae40 is the SIGNED counterpart
  and the trap batch 307 flagged.  It is nodes `{1,2,3,4}`, root 2, left `{1}`,
  right `{3,4}` -- a FOUR-node split, not the three-node one the recon guessed,
  and node 1 keeps an explicit `beq` precisely because `bgt` says signed and a
  signed selector has no tight lower bound to collapse against.  Count 4 < 5 is
  the whole reason it is a tree.  NOTHING IS MISSING THERE.

### CASE B -- 5 or more nodes and span <= 10 * count: A CASE VALUE IS MISSING
This is the new tell.  The same source, holding only the visible values, would
have compiled to a jump table; a tree means the case list carries at least one
value far enough outside the span to push `range` past `10 * count`.  For an
unsigned selector `case -1:` (0xFFFFFFFF) always suffices, and it is what
BufferString has -- in BOTH of its switches.

**Where the extra node hides.**  0xFFFFFFFF sorts LAST for an unsigned selector,
so it lands as the right child of the highest real node, and
`emit_case_nodes`' single-valued/right-child-only path finishes with
`do_jump_if_equal(index, -1, that case's code_label)`.  Then:

* if `case -1:` has a BODY, or the switch has a `default` body, you see it:
  `mov r2,#1 / neg r2,r2 / cmp r7,r2 / bne .. / b ..`  (BufferString's main
  switch, at the `.L1830a` node);
* if `case -1:` is EMPTY and the switch has NO `default` body, its label
  collapses onto the end of the switch, which is also the default label, and
  jump.c deletes a conditional jump whose target equals the following
  unconditional jump's.  All that is left is `cmp #0x1e / beq body / b end`
  -- INDISTINGUISHABLE from the highest node having no right child
  (BufferString's reduced switch, at `.L18136`).

So in CASE B the absence of any visible out-of-range test is NOT evidence
against the missing case.  The node count and the span are the evidence.

## 3. The re-screen, run over the whole tree

`screen_hidden_case.py` (gap 14) and `screen_tight.py` (gap 6) walk every `.s`
under `asm/`, split it into functions -- handling BOTH `.thumb_func_start` and
gcc's own column-0 label plus `.size` form -- find doubled-compare clusters, and
flag CASE B.

    docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
      goldensun-build python3 scratch_elev/b308c/screen_tight.py

RESULT, tight run:

    HAND-WRITTEN (candidates): 2 sites -- BOTH are BufferString's two switches
    COMPILER-GENERATED (controls): 0 sites

So **BufferString was the only CASE B function in the tree**, and it is now
solved.  That is a negative result for the other 34 functions batch 307 listed
and it is worth stating plainly: their lopsided dispatches are CASE A, tree
because 4 < 5, and re-screening them for an out-of-range case will find nothing.
What batch 307's lever does buy on those 34 is the FOURTH, LOWEST case -- which
is a real and separate win.

The loose run (gap 14) additionally reported BuildDraw2DFuncEx (count 7,
span 12) and, among generated files, Func_80a9c18 (count 5, span 4).  BOTH WERE
CHECKED AND BOTH ARE FALSE POSITIVES OF THE SAME KIND -- the cluster absorbed a
neighbouring bound compare:

* `asm/rom_c9000/rom_ed408.s` BuildDraw2DFuncEx: the flagged value set
  `{0,1,2,3,4,8,0xc}` merges the `cmp r3,#4 / #8 / #0xc` at lines 56-64 with the
  genuine `{0,1,2,3}` trees at lines 108-114 and 135-141.  Its switches are
  CASE A.
* `asm/rom_a1000/rom_a8604_c_c_a_c_a_a.s` Func_80a9c18 is a LANDED, MATCHING
  generated file and its source, `src/rom_a1000/rom_a8604_c_c_a_c_a_a.c:34`,
  holds `case 1,2,3,4` -- four nodes, below the threshold.  The screen counted a
  fifth from an adjacent `cmp r3,#0`.

A correction worth recording about this script's own history: its FIRST run
reported "COMPILER-GENERATED (controls): 0 sites" and that number was a
DETECTOR BUG, not a validation -- the function splitter only knew
`.thumb_func_start`, which gcc's own `.s` does not emit, so no generated file was
ever entered.  The fix is in the script and commented there.

## 4. Regenerating the threshold probe

    python3 - <<'EOF' > /tmp/probe_thresh.c
    print("extern void S(int);")
    for n in range(3, 13):
        print("void T%d(unsigned int x) {" % n)
        print("  switch (x) {")
        for k in range(n):
            print("  case %d: S(%d); break;" % (k, k*7+1))
        print("  }")
        print("}")
    EOF

then compile with the production flags and look for `mov pc, r3`.
