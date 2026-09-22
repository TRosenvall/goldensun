# Batch 280 — nineteen functions, the large-function band opened, and two parks that are about measurement

Gated on a clean `make clean && make -j8 && make compare`, green at the target SHA1
`5c4695205413df7db52b9a184815a07783999971`. **All nineteen verified at their exact ROM addresses in
the freshly linked overlay ELFs** (19 checked, 0 mismatches). `git status` clean.

| | |
|---|---|
| elevated | **19** |
| parked | **8** |
| `.sym` entries added | **1** (`_AREA_6d`) — three more reported and withheld |
| whole-file conversions | **4** (no split, no linker change) |
| **fakematch debt added** | 19 |

**Seven subagents.** The user asked for six at 4–5 functions each, then amended to seven mid-turn.

| agent | assignment | landed |
|---|---|---|
| A | overlay 943 whole file | **3** of 4 — and one target was my duplicate-assignment error |
| B | overlay 896 | **3** of 3 |
| C | overlay 881 whole file + singles | **3** of 4 |
| D | overlay 899 | **3** of 4 |
| F | overlays 917/918/923/932/944 | **4** of 4 |
| G | overlays 969 + common1 | **2** of 5 |
| E | `rom_8a000` / `rom_b5000` | **0** of 4 — one at 5 encodings |

The headline is the band. **`OvlFunc_881_200acb4` landed byte-exact at 397 instructions and 1148
bytes**, the largest function ever elevated in this project, and four files converted whole. The band
is **more tractable than the 100-instruction one, not less** — but only once you stop reading the
difference count as a distance, which is what the two most useful failures of the batch are about.

## A NORMALISED DIFFERENCE COUNT IS NOT A DISTANCE TO EXACT WHEN THE POOL HAS MOVED

This cost two parks and it is the lesson worth carrying out of the batch.

`OvlFunc_969_200bbc8` was reported to me at **"3 differing"**. Under `objcmp` it measures **218 of
295**. Both numbers are correct. The instruction counts are 295 against 293 — a delta of two — so
roughly **216 of the 218 are the same instructions with shifted `ldr [pc, #N]` offsets**, and the −4
bytes is one pool word. That is `tryc`'s documented blind spot #1 seen from the other side.

Three *normalised* differences with a displaced pool is a **much worse** position than three real
ones, because the pool is one structural fact that has to be fixed before any of the three can be
trusted. I nearly published the 3.

The corollary, now written into both parks: **quote `objcmp`, and quote the reference count beside the
difference count.** `OvlFunc_common1_5e4` was reported at "202 of 277" and measures 232 of **263** —
the reference count itself disagreed, which is exactly how a mismatched baseline announces itself if
you are printing it. `OvlFunc_951_2008ac8` was reported at 102 and measures 129, the gap being a
`-fno-rerun-loop-opt` row that does not exist plus three symbols that were not added.

## The count is a poor search signal at this size, because defects cascade

| function | root cause | lines it spent |
|---|---|---|
| `OvlFunc_881_20086ec` | two extra pool words | **twelve conditional branches** |
| `OvlFunc_881_200acb4` | 31 CSE'd constants | **111 of 129** |
| `OvlFunc_881_200a8e8` | constant CSE | eight high-register copies, four register roles |
| `Func_808e680` | one interior pool | **216, against 5 with an end pool** |

Intermediate readings lie outright: one candidate scored 221 against another's 223 while the 223 was
strictly better in the window under test. **Navigate by windows, not counts** — which first requires
normalising pool offsets and branch targets in the diff. The raw diff for one 397-instruction
candidate was 129 lines of which ~100 were `ldr [pc, #N]` offsets.

## The dominant blocker at this size is CSE OF REPEATED POOL CONSTANTS, not allocation

These are long straight-line cutscene scripts, so the same emotion id / speed pair / coordinate
recurs 2–5 times **inside a single basic block** — calls do not end a block. gcc commons every one;
the ROM commons none. This was 111/129 on `OvlFunc_881_200acb4` and ~110/169 on
`OvlFunc_881_200a8e8`, and argument pins are its only cure in this tree's idiom.

**So the pin count scales with the script's repetition, not with any defect in the reading** — which
is why those two ship 25 and 13 pins and that is the correct number, not a smell. Every flag probe was
worse, confirming the axis: the commoning is cse1/gcse, not cse2
(`-fno-rerun-cse-after-loop` 183, `-fno-gcse` 180, `-fno-cse-follow-jumps` 169 inert,
`-fno-expensive-optimizations` 179, against a 169 baseline).

## PINNING AND UN-PINNING ARE BOTH LEVERS AND THE LADDER MUST TEST BOTH DIRECTIONS

`OvlFunc_881_200acb4`, in three steps:

    129 -> 18   PINNING 31 repeated-pool-constant sites in one step
     18 ->  6   UN-PINNING q1 at nine of them, so the mov/lsl pair stays expanded and
                sched2 can slot the pinned `mov r0` BETWEEN the mov and the lsl
      6 ->  0   REMOVING the last pin block entirely -- the second __Func_80933f8 call
                is byte-exact written PLAIN, and five pinned orders all fail

**A pin that is inert is not free — one of them was actively wrong.** Two greedy rounds found 8
individually-inert pins; dropping all 8 *jointly* broke it (the `rom_7d95dc` shape), and a proper
greedy kept 4 of the 8.

## Five more levers, each closing or nearly closing a function

- **A POOLED ZERO CAN BE A HImode CONSTANT RATHER THAN A SYMBOL.** `OvlFunc_943_200a618` loads `0`
  *from the pool* for two byte stores while storing 1 and 2 at the same offsets with `movs`, and its
  ROM splits the pool into **three** chunks with `b`s only 248 and 356 bytes apart. `*thumb_movhi_insn`
  alt 1 prints `ldrh` (GAS → a two-byte pc-relative `ldr`) and carries `pool_range 64` against
  `movsi`'s 1020, while `MINIPOOL_FIX_SIZE` still rounds it to a word — **one such fix clamps
  `max_address` for the whole pool.** The spelling is the **bare literal**; `int zero = 0;` destroys it
  (69 differing) and a `_CONST_0` symbol gives the right `ldr` but the wrong range. **Check for a
  halfword store before inventing a symbol.** That one edit took 69 → 0 and dissolved an r5/r6 swap a
  full `.17.lreg`/`.18.greg` analysis had priced as a 5× unreachable gap.
- **SWITCH vs IF-CHAIN IS VISIBLE IN THE BRANCH POLARITY.** `cmp #0xa / beq` is a `switch`; `bne` is
  an `if`/`else if`. Worth all 25 branch-structure differences at once on `OvlFunc_924_200a318`.
- **A SUBSET PIN IS ORDER-SENSITIVE WHERE A FULL PIN IS NOT**, qualifying batch 273's "write every
  pinned fill uniformly ascending". With q0 left to expand, ascending `q1,q2,q3` **fails** at 4 and
  `q2,q1,q3` is exact. The rule holds when the pin set is q0-first-and-complete.
- **ELEVEN-ARGUMENT CALLS: NAME THE STACK ARGUMENTS.** Eleven literals emit seven `mov`/`str` pairs
  alternating through r3 because anti-dependences pin the order; `v1..v7` as named locals let
  local-alloc give each its own register, CSE share the two `4`s, and sched2 group the movs ahead of
  the strs — the ROM's shape. Worth 20; nothing else reached it.
- **THE POOL-PLACEMENT RULE IN `docs/elevation.md` WAS HALF A RULE, and the missing half is the useful
  one at size.** The file recorded only that a wide first entry is harmful when the ROM's pool is
  mid-body. **When the ROM's pool is at the END, the `int` local is REQUIRED** — 216 differing with an
  interior pool against 5 with an end pool, same C except one constant. And it needs pairing: the
  `int` local *alone* was worse (5 → 10), paying only once hoisted above the dominating branch.
  Amended in place.

## MERGING AND SPLITTING LIVE RANGES ARE BOTH LEVERS, decided per value by whether it crosses a call

Three instances, and **the priority formula predicted one before it was tried.** On `Func_808e23c`,
merging the outer-loop counter with a call result lifts `REG_N_REFS` 7→10 and `REG_LIVE_LENGTH` 54→60,
so priority goes 0.259 → 0.5, passing two competitors; the predicted order
`ptr r5 > j r6 > best r7 > size r8` is exactly the ROM's. 50 → 37, and a 60-instruction search loop
went exact. `Func_80ba2c0` confirms the same lever negatively — splitting two such values costs 130
against 51.

**The reverse defect is in the same function.** Reusing one variable for the inner count *and* a call
result made the count cross a call, excluding it from r4 in `find_reg` PASS 0 and producing a
caller-save pair plus a third spill slot. Separating them removed both (92 → 72).

## New park class: REDUNDANT-COPY PRESSURE, where the ROM is LESS optimal than gcc

`Func_80ba2c0`. The ROM spills a parameter and reloads it nine times; ours keeps it in r11. The cause
is exactly one register of pressure: **the ROM's loop carries a redundant copy of a context pointer
into r8** — three instructions per iteration where two suffice — which takes the fourth high register
and forces the parameter out. gcc, being correct, makes no such copy.

Reproducing it needs a construct that makes gcc hold a *second* live copy of a pointer it already has,
across a loop, with nothing else added. Nine spellings; **gcc coalesces or reverts every one**, and
coalescing is correct. Recorded as a class so it is not re-attacked as a spelling hunt.

## Two things that should have been standard long before batch 280

- **VERIFY A SYMBOL TELL AGAINST A SYMBOLISED COPY OF THE REFERENCE.** `objcmp` compares an unlinked
  object against a hand-written `.s` that spells the symbol as its literal, so a *correct* candidate
  reports "N encodings + M relocations differ." Three of four targets in one agent's set did. Copy the
  reference and rewrite `ldr r0, =0x3a` to `=_AREA_3a`; that turns "2 places differ" into `OK`.
- **READ THE COMPILER'S OWN SOURCE — IT IS IN THE IMAGE AND HAD NEVER BEEN OPENED.** Both hardest
  levers this batch came from reading `arm.md`, `arm.c` and `loop.c` in the build container. It turned
  "gcc will not split this pool" from a dead end into a measurement that matched the ROM to within 4
  bytes. 279 batches of it sitting unused.

## Corrections I owe the log

**I told seven agents and the user that no batch had attempted the 201–400 instruction band, and that
the largest landing was ~400 bytes. Both false.** Agent F found a landed byte-exact 334-instruction
function. The truth: **148 landed functions exceed 200 instructions, 90 exceed 300, and the largest is
1,939.** Root cause: **a landed function's `.s` is gcc output and carries `.type NAME,function`, not
`.thumb_func_start`** — so every scan I wrote could see only functions still in assembly and returned
zero, which I read as "never done". Fixed in `d66b38a2` with the full distribution. The lesson —
**when a tool says something has never been done, check that the tool can see it** — is the third
instance of this shape, so it is now a standing check rather than an anecdote.

**I assigned `OvlFunc_944_2008af8` to two agents.** Agent F landed it; Agent A solved it independently
and found it already committed. That wasted part of a budget and it was my error in building the
target list. The consolation is genuine: both reached the same 15 pin sites and the same loop shape,
which is unusual corroboration for a heavily-pinned function — but I should not have needed it that
way.

**A container `cpp` crash dumped a coredump over the repo-root file `core`, which is tracked.**
Restored with `git restore` before anything was staged. Worth knowing that this file exists and is
clobberable.

## State

**4,498 from C / 1,212 in asm** against batch 279's 4,479 / 1,231 — exactly **+19/−19**, the
**fifteenth consecutive reconciling batch**. `census.py` TOTAL 1212, agreeing with `funcindex` exactly.
**494 park files** (+8). `fakematch.txt` **488 rows** (+19). `dupfuncs.py` now reports 8 groups
covering 29 functions with 21 free — **and that remains a FLOOR**, since it groups on exact identity
and near-twins are invisible to it.

The census now shows where the work is, and it is not where the batches have been: **511 available**,
of which **141 are in 201–400**, **103 in 401–800** and **61 above 800**. Only 25 remain in 61–100.
The large bands are the project now.

## Open, and all of these are the user's call

- **Three `.sym` entries reported with their evidence and withheld** because none **completes** its
  function, which is this tree's line: `_MSG_2644` (`OvlFunc_881_200a8e8`), `_MSG_920`
  (`Func_808e680`), `_MSG_970` (`Func_808d9a4`). Each park records the in-function control.
- **A fourth group marked UNVERIFIED:** `_MSG_e43`/`_MSG_e49`/`_MSG_e4c` for `OvlFunc_951_2008ac8`.
  The control message.sym's criterion requires was never recorded with the report, so these are
  explicitly weaker than the three above rather than passed along at equal weight.
- **Weaker still, and flagged as such:** `0x1000` in `Func_808d9a4` is a `const.sym` candidate with
  **no in-function control**. Do not add on this evidence.
- **Three long-standing entries:** `_MSG_2080`, `_SIZE_80f0024`, `_TBL_7a828`.
- **Three per-file flag rows**, each a change to the build's claim about the original compilation:
  `-fno-rerun-loop-opt` for `OvlFunc_951_2008ac8`, `-fcall-saved-r4` for `OvlFunc_970_2008f80`,
  `ALIAS_CFLAGS` for `OvlFunc_882_200c41c`.
- **The fakematch convention question**, quantified at about 20 of 181 PIN-using files unbooked.
- ~~**One permanent fakematch row nobody can remove:** `Func_808d9a4`'s byte table is a global symbol
  whose name starts with a dot (`.L9e680`), which no C identifier can name.~~ **RETRACTED in batch
  281 — this was false.** `extern const unsigned char tbl[] __asm__(".L9e680");` names it, the park's
  own C had been doing exactly that all along, and the construct appears in six landed files where it
  is not consistently booked as fakematch debt. Verified independently on `.Lb4146` and `.Lb4ab2`,
  where `objcmp` confirms the relocation matches byte-for-byte. Nothing about `Func_808d9a4` is
  blocked by naming.
- `OvlFunc_948_2009308`'s one pin, from batch 272.
- **One target never attempted:** `OvlFunc_945_200aff0` (agent G ran out of budget).
