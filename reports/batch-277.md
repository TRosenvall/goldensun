# Batch 277 -- nineteen functions, a duplicate-group selector, and the allocation formula

Gated on a clean `make clean && make -j8 && make compare`, green at the target SHA1
`5c4695205413df7db52b9a184815a07783999971`. All nineteen addresses checked against
the freshly linked ELF, along with every function the fourteen splits left in
assembly. Four control symbols came back absent.

| | |
|---|---|
| elevated | **19** |
| parked | 5 new, 1 standing park advanced, **4 retired** |
| tool fixes | **1** (`census.py`, a long-standing undercount) |
| `.sym` entries added | 0 (two comment-only corrections to `const.sym`) |
| splits | 14 |
| **fakematch debt added** | **1** |

**Six screening subagents, twenty-one targets.** Nineteen functions landed from
fifteen solved targets -- the difference is duplicate groups, where one solution
converts two functions.

| agent | assignment | landed |
|---|---|---|
| 2 | small duplicate groups | **8** of 5 targets |
| 6 | overlay singles + the `Field` pair | **5** of 4 targets |
| 4 | `rom_a1814` cluster | **3** of 3 |
| 3 | `rom_b8228` cluster | **2** of 4 |
| 5 | `rom_1de5c` save-data cluster | **1** of 4 |
| 1 | the x15 duplicate group | 0 of 1 (101 → 42) |

## Targeting: two selectors, one of which was sitting unused

Batch 276 concluded that an elevated same-stem NEIGHBOUR predicts convergence
better than instruction count. Applied here by scoring every remaining function on
how many `_`-components its stem shares with an elevated `.c` in the same
directory, it held: **the 61--100 band now has only two functions with a twin,
against dozens in 101--200**, and the round's landings came almost entirely from
the larger band.

The second selector was already in the tree and unused. **`tools/dupfuncs.py`
reports 16 duplicate groups covering 46 remaining functions, 30 of which would come
free.** Three pairs landed off it in one round.

Two rules, learned immediately:

1. **Verify the twin, don't assume it.** Of three pairs, two were identical and the
   third differed in **exactly one constant** -- a loop bound, 0x540 against 0x600.
   That one landed as two files, not a shared body.
2. **`objcmp` each member against its own reference.** One command, and it is the
   difference between a checked landing and a guess.

The biggest group, `OvlFunc_883_20088c0` at **x15** with every host holding exactly
one function, did not land -- but went from 101 differing to **42 at full length**,
and turned up two problems in its own park: its candidate C had never been updated
to implement its own recorded levers, and its update block's symbol names belonged
to a *different* member of the group. **In a duplicate group, check which copy a
note was written against**; numbers from two copies are not comparable if the
source was not. Two *solved* siblings in another overlay carry the same
abs-and-shift block and settle its geometry table as 1-D rather than `int[][4]`.

## What landed

| | function | address | source |
|---|---|---|---|
| 1 | `Field_Lift` | `0x08098954` | [rom_97b54_a_c_a_c_c_a_b.c](../src/rom_8a000/rom_97b54_a_c_a_c_c_a_b.c) |
| 2 | `Field_Carry` | `0x0809a294` | [rom_97b54_c_c_c_a_b.c](../src/rom_8a000/rom_97b54_c_c_c_a_b.c) *(twin)* |
| 3 | `OvlFunc_882_200bce4` | `0x0200bce4` | [ovl_30_c_c_c_c_a_a_a_c_c_c_c_c.c](../src/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_c_c_c.c) |
| 4 | `OvlFunc_950_20083dc` | `0x020083dc` | [ovl_30_c_c_a_c_a_a_c_a_c_a_a.c](../src/overlays/rom_7d5838/ovl_30_c_c_a_c_a_a_c_a_c_a_a.c) |
| 5 | `OvlFunc_949_2008728` | `0x02008728` | [ovl_30_..._a_b.c](../src/overlays/rom_7d4af4/ovl_30_c_c_a_c_c_c_c_c_c_c_c_a_b.c) *(fakematch)* |
| 6 | `Func_801b36c` | `0x0801b36c` | [rom_1aeec_a_a_c_a_c_a_b.c](../src/rom_15000/rom_1aeec_a_a_c_a_c_a_b.c) *(park closed)* |
| 7 | `Func_80b0694` | `0x080b0694` | [rom_b0070_a_a_c_a_c_c_c_a_a_b.c](../src/rom_b0000/rom_b0070_a_a_c_a_c_c_c_a_a_b.c) *(twin, park closed)* |
| 8 | `Func_809088c` | `0x0809088c` | [rom_8d9a4_c_c_c_a_a_a_c_a_b.c](../src/rom_8a000/rom_8d9a4_c_c_c_a_a_a_c_a_b.c) *(park closed)* |
| 9 | `Func_80f2ebc` | `0x080f2ebc` | [rom_f2028_c_c_a_a_a_b.c](../src/rom_f2000/rom_f2028_c_c_a_a_a_b.c) *(near-twin, park closed)* |
| 10 | `Func_80f6038` | `0x080f6038` | [rom_f6008_c_a_a.c](../src/rom_f6000/rom_f6008_c_a_a.c) |
| 11 | `Func_80f4100` | `0x080f4100` | [rom_f4008_c_c_b.c](../src/rom_f4000/rom_f4008_c_c_b.c) *(twin)* |
| 12 | `Func_807a498` | `0x0807a498` | [rom_79460_..._a_b.c](../src/rom_77000/rom_79460_c_c_c_c_a_c_c_c_c_a_b.c) |
| 13 | `Func_8026fa8` | `0x08026fa8` | [rom_23178_a_a_a_a_c_a_a_b.c](../src/rom_15000/rom_23178_a_a_a_a_c_a_a_b.c) |
| 14 | `Func_80b90f8` | `0x080b90f8` | [rom_b8228_..._c_b.c](../src/rom_b5000/rom_b8228_c_a_c_c_a_c_a_c_b.c) |
| 15 | `Func_80b9324` | `0x080b9324` | [rom_b8228_..._c_c_b.c](../src/rom_b5000/rom_b8228_c_a_c_c_a_c_a_c_c_b.c) |
| 16 | `Func_80a1d08` | `0x080a1d08` | [rom_a1814_..._c_b.c](../src/rom_a1000/rom_a1814_c_a_a_c_a_c_a_a_c_b.c) |
| 17 | `Func_80a1e38` | `0x080a1e38` | [rom_a1814_..._c_c_b.c](../src/rom_a1000/rom_a1814_c_a_a_c_a_c_a_a_c_c_b.c) |
| 18 | `Func_80a1fd4` | `0x080a1fd4` | [rom_a1814_..._c_c_c_b.c](../src/rom_a1000/rom_a1814_c_a_a_c_a_c_a_a_c_c_c_b.c) |
| 19 | `Func_801ef68` | `0x0801ef68` | [rom_1de5c_..._c_c_b.c](../src/rom_15000/rom_1de5c_c_c_c_c_a_a_a_c_c_b.c) |

## The headline finding: register permutations are arithmetic

The largest blocked class in this corpus is a register permutation with the
structure already exact. Six parks sit on one. The advice has been "read
`.18.greg` and hope". **The formula is confirmed:**

    priority = floor_log2(REG_N_REFS) * REG_N_REFS / REG_LIVE_LENGTH

**Both inputs are printed verbatim by `-da` in `.17.lreg`** as `Register N used R
times across L insns`. `REG_N_REFS` is LOOP-WEIGHTED (`+= loop_depth + 1`), and a
def counts as a ref. On `Func_801ef68` it predicted `.18.greg`'s
`;; 19 regs to allocate:` line **in full, every allocno, in all five variants
checked**.

Its value is knowing **when to stop** and **what to change**. `Func_801ef68` sat at
32 differing with three locals permuted across r0/r5/r6, and eleven spellings
measured exactly 32:

| local | refs | live length | priority |
|---|---|---|---|
| `i` | 20 | 112 | **0.714** |
| `p` | 12 | 64 | **0.5625** |
| `y` | 7 | 25 | **0.560** |

Two of the three were tied to **0.4%**. No respelling was going to separate them,
and the formula says so before the round is spent. Conversely `Func_801f088`'s park
now states its next step as arithmetic: the masked value needs one more depth-2
reference, or a live length under 94, to overtake at 0.287 against 0.229.

### Two levers and one trap that come with it

- **`i = n;` as a MERGE-POINT COPY** is the cheapest known way to shorten a loop
  index's live range: 12 refs / 96 insns became 12 / 40, moving it from 11th to 4th
  in the allocation order. An `if/else` writing the index in both arms is 80
  differing; a ternary costs three instructions.
- **A "DEAD" INITIALISER CANNOT BUY PRIORITY.** A bare `y = 0;` before a loop is
  deleted outright, and moving the extra reference *after* the loop ballooned the
  live length from 25 to 78 and made it worse. The reference must be INSIDE the
  loop.
- **Spill slots read as a declaration order but are not a lever.** gcc assigns them
  ascending pseudo number to descending `sp` offset, so a ROM's layout is legible --
  but reordering declarations to match moved nothing (144 against 140). The slot
  order is a consequence of the allocation.

## Two parks had talked themselves into scaffolding they did not need

Both retired, both exact with ordinary C.

`Func_80b0694`'s park read the ROM's two reads of one field as a CSE problem and
concluded *"`volatile` would do it and is a fakematch"*, booking itself as future
debt. **`loop.c`'s `duplicate_loop_exit_test` COPIES the exit test to the loop
front and runs at pass 08, AFTER cse1 and gcse**, so the copy is never available to
fold and LICM then hoists it. The park's guarded `do/while` is 16 differing; a
plain `while (i != b->n)` is exact.

`Func_80f2ebc`'s park concluded the C was finished, that a flag group AND a
per-object symbol rename were both required, and instructed **"Do not re-try the
function-pointer route."** That instruction was the error. `extern int
divsi3_RAM(int, int);` with the pointer assigned **inside** the guard emits the
ROM's preheader and body exactly, under default flags. Checked that the
declaration is honest rather than a convenient alias: `divsi3_RAM` at `0x03000380`
and `__divsi3` at `0x080022ec` are two distinct real symbols.

## The strongest case yet against one-lever-at-a-time search

`Func_80a1e38`. **Twenty-one single local changes all left 110 differing** -- five
declaration orders, four increment spellings, five positions for a `= 0`, three
pointer-copy variants, a `volatile` parameter, an address-of. Nesting the retry
loop as a real inner loop went to **0**, and fixed three unrelated-looking residues
at once: a reference-depth change that spilled a parameter to the ROM's `sp+0xc`,
`j` becoming a genuine biv so `strength_reduce` emitted the giv init after the LICM
hoists, and an r2/r3 role swap in a compare.

**When a residue spans register allocation AND a preheader order AND an
argument-register swap at once, the loop STRUCTURE is the common cause.**

## Other levers, each of which closed a function

- **cse1 substitutes the nearest branch-proven zero for a `const_int 0` store**,
  which extends that variable's live range across a call so
  `ALLOCNO_CALLS_CROSSED > 0` excludes every call-used register -- r1 *and* r4
  under `-fcall-used-r4`. The fix is a NEARER zero, not a block: a named `int z = 0;`
  is merged into the other variable's qty and deleted, and `volatile` fails. The
  tell is a `str <callee-saved reg>` where the ROM has `str r0` after `mov r0,#0`,
  plus a push list one register too wide.
- **Reusing the source variable as the accumulator** inverts `local_alloc` priority
  (roughly refs over live length), worth 29 of 54. When a ROM reuses a dead value's
  register as an accumulator, suspect one variable rather than two.
- **Two sequential loops share ONE counter -- and the spill pairs are EVIDENCE for
  it.** Worth 135 differing to 0. The shared counter lands in r4, call-clobbered
  under `-fcall-used-r4`, producing the ROM's `str r4,[sp,#0]` pairs around three
  calls; a separate counter takes a free r7 and both vanish.
- **A byte index past the 5-bit offset wants the INDEX named, not the address.**
  Fixed a length in one edit, 38 differing to 3.
- **`fold_truthop` folds a three-term comparison chain into a range test, and
  `unsigned char` does not block it** (gcc folds in QImode instead). A ROM shape of
  N forward branches into a shared tail assignment that the success path jumps over
  needs explicit `goto`s.
- **A HImode constant store pools; the same store through a struct field does not.**
  And **pool entries sort HImode BEFORE SImode**, which is how you read whether a
  `strb` is sharing a halfword pool word rather than wanting a symbol.
- **A local array spelled directly survives dead-store elimination** where the same
  array through a pointer does not -- worth 79 to 54 and the exact length.
- **`if (c) {X} else {loop; X}` and `if (!c) {loop} X` are different block
  layouts**, which explains an `add rN, sp, #K` that appears duplicated in two
  successors.
- **The "three named locals" constant lever is amended in both directions**: it
  fires on many sites, not three, and where the constants DIFFER (the non-pin cure
  for the `mov r0` interleave). Its precondition is the whole test -- the
  assignments must sit in a block a branch DOMINATES, and **block-0 constants must
  be left BARE**, where naming them widens the push list.

## A tool bug that had been quietly wrong the whole time

`README.md` has said since an earlier miscount that four ROM functions are declared
`.thumb_Func_start` or `.thumb_func_Start` with the wrong case, that GAS accepts
them, and that **"any tool that walks these files needs a case-insensitive
match."** `census.py` did not have one.

Two of the four are still in assembly (`Func_a1f74`, `Func_97f80`), so **every
TOTAL that tool has printed -- including the figures published in these reports --
was two low.** `funcindex.py` uses `re.I` and was right.

**The cross-check is census TOTAL against funcindex's "still in asm" count: they
must agree.** They had silently disagreed by two; both now read 1261. That check is
recorded in `census.py`'s docstring as its fifth numbered counting lesson.

It surfaced because a subagent read a file the tool said held four functions and
found five -- and my own targeting grep was case-sensitive too, so I had passed the
wrong count to the agent. The split around `Func_a1f74` was arranged correctly once
known, and it is verified intact at `0x080a1f74`. One of the two is a
39-instruction target in a band the table had been reporting as empty.

## Parked: five new, all itemised

| function | differing | class |
|---|---|---|
| `Func_802106c` | **23 of 194** | one `.sym` entry + an r8 reload split |
| `Func_80b9470` | 37 of 105, **size exact** | an 8th allocno for 7 registers |
| `PrepareSaveHeader` | 69 of 181 | CSE base canonicalisation |
| `Func_80b920c` | 116 of 131, **stream identical** | one refused LICM movable |
| `Func_801f088` | 140 of 175 | `global_alloc`, priced with the formula |

`Func_80b920c`'s count is the extreme case of count-is-not-a-difference: the
instruction stream is identical one-for-one, every mnemonic and offset, and only
register NAMES differ plus five instructions. One root cause renaming almost
everything.

Two results say **stop** rather than **try harder**, with the evidence:

- **A pooled `sfp+K` temp address cannot be kept uncommoned.** Two at expand, one
  after cse1, always; and once commoned, LICM's arithmetic makes the hoist
  unavoidable. **Eight spellings intended to prevent it were byte-identical to each
  other.** Nineteen flags singly and six in pairs did not beat the default.
- **`move_movables` on a call-free Thumb inner loop: threshold < 31**, measured
  from a refused `life 1, savings 1` movable in a 31-insn pass-2 loop. With the
  recorded 23 for a loop *with* a call, that brackets the threshold for the next
  function in this class.

## `const.sym`, annotated twice

Comment-only, and each prevents a wrong future action.

- The halfword exception gains its **second attested instance**: `Func_80b9470`
  pools a genuine `0xf` (one of only two small-value pool words in the tree) and a
  plain `& 0xf` on an `ldrh` result reproduces it exactly. No `_CONST_f` was added.
- `_CONST_1f`'s own *"near miss, not close enough to count"* objection is corrected
  in place. It rested on the spelling printing `ldrh` -- but **Thumb-1 has no
  PC-relative `ldrh`**, so gas assembles it to the identical halfword. That entry is
  unnecessary; a `(u16)` cast before the mask is what reproduces the pool. Batch 276
  found this and I had recorded it only in a park.

## State

**4,449 from C / 1,261 in asm** against batch 276's 4,430 / 1,280 -- exactly
**+19/-19**, the **twelfth consecutive batch** where the delta reconciles.

`census.py` TOTAL 1,261, now agreeing with funcindex exactly. 481 park files
(480 - 4 retired + 5 new, reconciling). `fakematch.txt` 459 rows, **+1** -- one
four-pin function, whose pin ladder is on file and which needed a pin at exactly
one of its twelve call sites where the neighbouring fakematch spends three.

The 101--200 band holds **218 available**, and this round drew from it
successfully. 61--100 is down to 26, with only two of those carrying a same-stem
twin -- so the neighbour selector now points almost entirely at the larger band,
and `dupfuncs.py` points at 27 more functions that come free with a partner.

## Open, for the user

**Three build-input entries, now deliberately grouped as one decision.** Each has
verified evidence; none COMPLETES its function, which is the batch-272 rule I have
been applying, so adding any one alone would be drift:

- **`_MSG_2080 = 0x2080`** (new this batch) -- the STRONG structural argument:
  0x2080 is shiftable (`0x41 << 7`, verified), so gcc builds it and was measured
  doing so, while the ROM pools it. Worth 73 → 67 on `Func_802106c`, which stalls
  at 23.
- **`_SIZE_80f0024 = 0x230`** (batch 275) -- arithmetic verified, same class as the
  `_SIZE_8015430` added in batch 276. The strongest of the three.
- **`_TBL_7a828`** in `label.sym` (batch 275) -- `.L7a828` verified `.global`.

Also still open: the fakematch convention question (52 bare-pin files unregistered),
and `OvlFunc_948_2009308`'s one pin from batch 272.
