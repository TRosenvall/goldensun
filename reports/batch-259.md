# Batch 259 -- six functions, six parks, and two recorded rules that turned out to be wrong

Gated on a clean `make clean && make -j8 && make compare`, green at the target
SHA1 `5c4695205413df7db52b9a184815a07783999971`. Every address checked against the
linked ELF with `tools/checkaddr.py`.

| | |
|---|---|
| elevated | **6** |
| parks retired | 5 |
| whole-file conversions | **6 of 6** -- no splits at all |
| build-input changes | 2 Makefile rules (the eighth and ninth) |
| fakematch debt added | 4 functions |

A six-agent round. **Every one of the six came out of the park pile**, and every
one converted its file WHOLE -- the first batch with no split.

## What landed

| | function | address | source | note |
|---|---|---|---|---|
| 1 | `OvlFunc_881_200811c` | `0x0200811c` | [ovl_30_a_a_a_c_a_c.c](src/overlays/rom_77a7c8/ovl_30_a_a_a_c_a_c.c) | no pins |
| 2 | `OvlFunc_968_2009780` | `0x02009780` | [ovl_30_…_a_a_c_a.c](src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c_a_a_c_a.c) | +`-O2` rule; 1 pin |
| 3 | `OvlFunc_964_2009fdc` | `0x02009fdc` | [ovl_30_c_c_c_a_a_a_a.c](src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a_a_a.c) | +`-O2` rule; pins |
| 4 | `OvlFunc_964_200a040` | `0x0200a040` | [ovl_30_c_c_c_a_a_a_a.c](src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a_a_a.c) | file-mate |
| 5 | `OvlFunc_928_2008d0c` | `0x02008d0c` | [ovl_314_c_c_a_c_c_c_a.c](src/overlays/rom_7b6668/ovl_314_c_c_a_c_c_c_a.c) | no pins |
| 6 | `OvlFunc_932_200ad58` | `0x0200ad58` | [ovl_30_…_c_c_c_a.c](src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_c_c_c_a.c) | 1 pin |

## Two recorded rules were wrong, and both failures were productive

**1. The `-fno-schedule-insns2` sign rule has a counterexample.** The rule --
*improves ⇒ sched2 owns the residue and alias may reach it; regresses ⇒ sched2 is
right and alias is the wrong axis* -- had held across a dozen functions. On
`OvlFunc_928_2008d0c` it **regresses 2 → 21**, and alias was the only axis that
worked.

The cause is scope. Every supporting case had sched2 owning a multi-instruction
window; here sched2 gets **93 of 95 encodings right** and exactly one tie wrong,
so turning it off measures the 93 it was getting right.

> The sign rule is diagnostic when the residue is a REGION. For a single adjacent
> pair it measures the wrong thing -- read the ready list instead, via
> `-fsched-verbose=5` on the `.23.sched2` dump.

`OvlFunc_932_200ad58` sharpens the same edge from the other side: there the flag
also regressed, and the correct reading was not "done" but *the fix lives
upstream, in the chain order sched2 consumes*. sched2's answer was right for the
chain it was given; only the LUIDs were wrong.

**2. "A straight-line repeated constant needs a dominating branch" is not true.**
The park on `2009fdc` and a sibling file both declared two functions out of reach
because neither has a conditional branch anywhere, citing the measured fact that
`goto L; L:` is deleted in `02.jump`. The branch is not needed: a **bare hard
-register declaration removes the pseudo the constant would flow through**, so
cse1 has nothing to common -- and the same pin fixes the r0-in-the-middle
argument interleave in the same edit. One lever, two blocker classes.

The entry for this already exists as *"try the BARE register pin before the
barrier"*, but it is filed under the fakematch idiom and never connected to either
class, which is why two rounds walked past it.

## Also settled

**This tree never runs sched1.** No `.21.sched` dump is produced, which is why
`-fno-schedule-insns` measures inert everywhere. Every scheduling question here is
sched2 -- so a park describing "pre-reload scheduling" is describing something
that does not happen.

**A reload-generated insn is spliced in immediately before its use**, so it can
never carry a LUID below an insn that already precedes it. That is why no
statement order, named local, pin or barrier can win a `rank_for_schedule` tie
against one. The only reachable variable is the READY TIME, and only a memory
dependence moves that.

**To CREATE that dependence, the aggregate must CONTAIN the store's type.** A
`COMPONENT_REF` load takes the aggregate's alias set and `record_component_aliases`
makes each member a subset, so a conflict against an `int` store needs an `int`
member. Measured: `union{u16}` 2, `union{u16,char[2]}` 2, `union{int,u16}`
**EXACT**. A `char` member is not enough. This is the mirror of the batch-250
device, which uses two distinct named sets to REMOVE a dependence.

**A third owner: reload.** Batch 258 said to check whether `local_alloc` or
`global_alloc` owns a quantity before doing `QTY_CMP_PRI` arithmetic. There is a
third answer -- neither. A Thumb `ldrsh`/`ldrsb` zero offset is the
`(clobber (scratch:SI))` of `*thumb_extendhisi2_insn`; it never becomes an allocno,
so no ref-count adjuster, live-range split or pin can reach it. Reload fills it
with the lowest hard register free AT THAT INSTRUCTION, and the only handle is the
liveness of the competing register. `.18.greg` says `Using reg N for reload 0`;
when you see that line, stop counting references.

**A cheap constant argument can never be repositioned by source spelling.** On
thumb `rtx_cost(CONST_INT, SET)` is 0 below 256 and
`precompute_register_parameters`' gate is `> 2`, with `SMALL_REGISTER_CLASSES`
always live. So every shifted argument is precomputed in argument order and every
small literal is emitted later -- the cheap argument always carries the HIGHEST
LUID of the group. Naming it is inert (combine folds it back); naming the
expensive one moves it earlier, and it can never move later because
`update_equiv_regs`' move-to-use path needs `REG_BASIC_BLOCK < 0`.

**The bare pin can beat the barrier, and the barrier can be actively harmful.** On
`200ad58`: bare pin EXACT, `__asm__ volatile ("" : "+r"(y))` **3 -- worse than
baseline**, because the barrier blocks a `mov r2` hoist the ROM needs.

**The r0 pin is a SCHEDULING lever, not an allocation one.** The pinned value goes
to r0 either way; the pin changes when the move is emitted, which is what sched2
breaks ties on.

## Not closed, but proved

`Func_80b0a20` stays parked at **16 of 34** under `-fno-strict-aliasing`, size and
encoding count exact. What changed is that the residue is now *proved unreachable
from C* rather than merely unbeaten, so the next reader should not re-run the
spelling sweep:

1. **Order is arithmetic.** `add_minipool_forward_ref` sorts by
   `address + pool_range` and merges only on value AND mode.
2. **The head bound is measurable.** `.26.mach` prints
   `;; Emitting minipool after insn 229; address 92`. Writing the zero as a symbol
   makes every entry SImode with head range 1024 and moves the pool past the
   epilogue -- so the `const.sym` reading is disqualified **by construction**, not
   by score.
3. **Nothing can supply a narrow `0xffff`.** `force_const_mem` never fires for an
   integer on thumb, and a HImode `& 0xffff` is all-ones for the mode and folds
   away at both tree and RTL level.

That third point generalises: the halfword-exception lever runs ONE WAY. It can
narrow `0x1ff`, `0xfe00` or a zero used as an insert keep-mask -- never `0xffff`.

## Three things I got wrong

**I briefed an agent off a measurement artifact.** I screened `2009fdc`'s park and
read *"54 differing, ours 84 instructions against the ROM's 44"*, wrote it up as a
wrong-shape problem, and told the agent to rewrite from the disassembly.
`objcmp --func` and `olevel.py` filter only the REFERENCE and compile the WHOLE
candidate, so pointing them at a two-function park file compares 44 reference
instructions against 88 candidate ones. The park was never twice the ROM's length;
per function it was **14 of 44**. The agent measured properly, ignored the framing
and closed both functions. Recorded in `docs/elevation.md` -- the failure is
silent, because a mismatched scope returns a plausible number instead of an error.

**I told an agent the `ALIAS_CFLAGS` rule would be the fifth.** The Makefile
carries **19**. I had repeated that number from a batch-258 agent report without
checking it.

**`park_status.py` was wrong a fourth time.** It scored `80b0a20` at 1 off a
sentence about a *different* function -- *"Func_809b0dc, the other function with
this symptom, goes from 1 differing"*. Fixed; the "6 differing or fewer" list
dropped from 80 to 75, so five more targets were not as close as they looked. Four
distinct failure modes now, each patched with another regex over prose that was
never written to be machine-read. It is adequate for ranking and should not be
trusted for any individual park.

## A fourth park whose exemplar was misread

`2008d0c`'s park named a matching function as emitting the constant first from the
same expression shape, and built its framing on it. That function's generated `.s`
emits the LOAD first -- and its ROM does too. It was never a counter-example.

> A "function X does this from the same source shape" claim is about X's OUTPUT.
> Read X's generated `.s` before believing it, not X's source.

## State

| | |
|---|---|
| matched | **4,296** (76.0% of the 5,655 elevatable) |
| remaining, hand-written thumb | 1,359 |
| &nbsp;&nbsp;parked | 465 |
| &nbsp;&nbsp;UNATTEMPTED | 894 |
| ARM, never elevatable | 51 |
| park files | 519, resolving to 489 distinct subjects |

Pool went **1,365 → 1,359**, exactly -6.

## Process

Six agents, scratch directories named for their targets, no collisions. All six
were barred from `src/`, `asm/`, the Makefile and the linker scripts and from
running `make`; every landing was done here and gated, with a
`grep -q 'Generated by gcc'` guard on each staged `.s`.

Worth noting what the agents caught that I did not: one corrected my measurement
artifact, one corrected my `ALIAS_CFLAGS` count, and one produced the sign-rule
counterexample by ignoring what the rule told it and measuring the axis anyway.
