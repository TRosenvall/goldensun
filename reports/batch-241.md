# Batch 241 — a split collapsed the day it was made

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_926_200aad0` | `0x0200aad0` | [ovl_314_…_c_c_b.c](src/overlays/rom_7b2078/ovl_314_c_c_c_c_c_c_a_a_c_c_b.c) |
| 2 | `OvlFunc_896_2008f8c` | `0x02008f8c` | [ovl_314_c_c_a_c_a_a_c_a.c](src/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a_c_a.c) |
| 3 | `OvlFunc_969_200be9c` | `0x0200be9c` | [ovl_314_…_a_b.c](src/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_c_c_a_b.c) |
| 4 | `OvlFunc_953_2009298` | `0x02009298` | [ovl_30_c_c_c_a_a_c_a_a.c](src/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_c_a_a.c) |
| 5 | `OvlFunc_953_20091c4` | `0x020091c4` | [ovl_30_c_c_c_a_a_c_a_a.c](src/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_c_a_a.c) |
| 6 | `OvlFunc_960_2008594` | `0x02008594` | [ovl_314_c_a_c_c_c_c_c_c_a.c](src/overlays/rom_7eaf28/ovl_314_c_a_c_c_c_c_c_c_a.c) |
| 7 | `OvlFunc_924_200ba64` | `0x0200ba64` | [ovl_35b8_a_a_c_a_c_a.c](src/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_a.c) |
| 8 | `OvlFunc_924_200bb24` | `0x0200bb24` | [ovl_35b8_a_a_c_a_c_a.c](src/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_a.c) |

Entry 1 is now the largest function in the project: **5000 bytes, 1958
encodings, 170 pins**. Entry 5 was not a target — it was the other function
in a file opened for entry 4, the fifth such this week.

## Parked

| function | address | file | blocker |
|---|---|---|---|
| `OvlFunc_956_2008ba4` | `0x02008ba4` | [2008ba4.c](src/non_matching/ovl_7e0928/2008ba4.c) | pool-load ordering, one adjacent swap — **and 2-of-75 requires `-fno-gcse`** |

That park needs reading carefully. At the tree default it is **72 of 75**
with the relocation list four bytes short; reaching 2 needs `GCSE_CFLAGS`.
So closing it costs a Makefile entry as well as the residue, and the park
says so rather than burying a flag dependency in a promising number.

## A SPLIT COLLAPSED THE DAY IT WAS MADE

Entries 7–8 are the last two functions of the four-function `.s` that batch
240 split by hand hours earlier. Both matched on the **first candidate**,
with no pins and no named constants, so the split had nothing left to do:
all four functions are one TU again and the two `overlay.ld` lines fold back
into the one they replaced. Third collapse in six batches.

Verified by arithmetic, since `objcmp --func` cannot isolate a function in a
multi-function candidate: 884 = 232+284+192+176 bytes, 407 encodings, 27
relocations, with `nm` placing each function at the running sum and every
relocation landing at its own function's base. The two functions landed in
batch 240 were **re-screened from the combined text**, not assumed.

## THE DECISIVE LEVER ON THE LARGEST FUNCTION WAS NOT A PIN

Entry 1's two `GetActor(0)->f5a` sites must evaluate the call **once** and
leave it **anonymous**, and only a compound assignment does both:

| spelling | differing |
|---|---|
| `X = 0xfe & X`, X the call expression | 398 — emits **two `bl`s** |
| naming the pointer (the recorded failure mode) | 400 |
| `&=` | took the candidate 428 → 34 |

The recorded rule says "do not name it". The two-call trap is the half it
does not spell out.

## THE RELOCATION PARTITION NEEDED A THIRD BOUND, AND THIS ONE IS THE MECHANISM

Batch 239 recorded the discriminator and gave its mechanism as *"a pure
argument transposition moves no byte offset at all"*. Entry 1 is a
counter-example to that mechanism.

Site 166's constant is used **exactly once**, the encoding count is unchanged
at 1958, and the only defect is a two-instruction transposition — yet
dropping the pin measures **25 differing with RELOCATIONS DIFFERING**, this
function's CSE-band floor. Moving the pool-loading `ldr` two bytes makes gcc
dump the second literal pool one instruction earlier, which shifts every
following `bl`.

**The relocation line sorts pin jobs by EFFECT, not by cause.** Read the
encoding count too.

That is the third bound this rule has taken in three batches — after the
per-change bound (aggregate measurements sit inside the CSE band with
relocations silent) and the scale bound. The bands themselves have now held
across 308 drops with nothing in 4–9, so the rule is real; but it is
accumulating caveats faster than confidence, and the honest summary is that
it is a fast triage signal, not a classifier.

## ONE MATERIALISATION IS NOT ONE VARIABLE

The exact converse of the recorded "a re-loaded immediate after a join is TWO
locals". Entry 2's ROM has a single `mov r5,#0` serving two byte stores *and*
arriving at the loop as the counter — which reads as one variable and is not:

| spelling | result |
|---|---|
| one variable | 485 insns, 373 differing |
| two variables | **exact** |

The long live range drops the counter below the actor pointer in allocno
priority, gcc gives it a **high** register, and `strb`, `add #imm` and
`cmp #imm` all need a low one, so every use pays a reload copy. Written as
two, gcc **still emits only one `mov`** — both sit in one 174-instruction
block and it coalesces the copy away. The extra local's type is inert and
splitting into three ties: only the **count** matters.

## A CSE KILL THAT SHORTENS THE FUNCTION IS A PRESSURE SIGNAL

On entry 4, the only residue at one stage was a two-site CSE worth 12 of 334.
Pinning it cost **thirty** — because removing the pseudo dropped register
pressure, a held address moved out of a high register into a low one, and
three reload pairs collapsed. Six instructions short.

The cure was to restore the pressure *honestly*, by splitting a recycled
local. **Diagnostic: when a pin makes the output shorter, something that
should be live is missing.**

## REPETITION ALONE FORCES A BASE INTO A REGISTER

Entry 3 refutes a recorded "never". The ROM does `add r6, #2` off a held
`0x80<<6`; the notebook reads that shape as a positive tell for a named
local, says two independent `CONST_INT`s can never produce it, and requires a
runtime use of the base. None applies — both are plain literals in separate
calls' argument lists, with no variable anywhere.

Isolated on a three-function probe: base repeated four times gives a held
register plus `add #2` and no pool; base used once, or absent, pools the
derived constant. **Repetition alone is enough. No runtime consumer and no
mutated variable are required.**

Entry 3 also needed the largest hole yet: **22 of its 39 CSE-nominated sites
must stay bare**, because they carry the ROM's own held values. Pinning all
39 collapses the prologue and costs 318 of 358 at twelve bytes short.

## THE INLINE-ARGUMENT RULE IS A PLACEMENT LEVER

The doc states it as a prohibition — do not pass a computed value as an
inline's argument. Entry 6's ROM computes the value **above** the interrupt
guard, so it must be the argument; building it inside costs 212 encodings,
four bytes, and every relocation moved. Same shape, opposite verdict, and the
discriminator is where the ROM puts the computation.

Entry 6 also contradicts its own template's summary of a recorded rule. That
template reads `push {lr}` as "nothing survives a call, so only pins work";
this ROM pushes **eight** registers, keeps seven ordinary locals live across
calls, and its **first 92 of 249 instructions are exact from plain C with no
pin at all**. Reading the prologue by content is the recorded discipline —
here doing it says the opposite of what the template asserted.

## AN UNFLUSHED WRITE CAN SCORE A CANDIDATE WRONGLY

Four tools did `open(p, "w").write(s)` and handed `p` straight to a
subprocess: `objcmp.py`, `tryc.py`, `protolever.py`, `sweep_decls.py`.
CPython refcounting usually closes at once, which is why this has almost
always worked — but "almost" is the wrong guarantee for the authority tool,
where a short read yields a plausible wrong score and sends someone to park a
function that already matches.

Not hypothetical. An agent reproduced it in its own harness — identical
source scoring 12 differing on the first compile and 0 on the next two in one
process — and separately saw `objcmp` report 124 differing once on a function
that then returned OK on six consecutive re-runs.

All five sites now use context managers, and `objcmp` was re-verified against
the real `asm/` path on both functions landed from that agent's work.

## An unresolved contradiction, flagged not decided

`CLAUDE.md` lists *"commit compiler-generated `.s` files from `asm/`"* under
**Never**. `docs/elevation.md` has a section headed *"The generated `.s`
beside the `.c` IS tracked — commit it"*, which explicitly anticipates and
rebuts that reading.

The repository sides with `elevation.md`: **3,511 tracked `.s` under `asm/`
carry gcc's banner**, and `.gitignore` has no `*.s` rule. This session has
followed `CLAUDE.md`, so roughly 30 functions landed today have no tracked
generated `.s` where ~3,500 others do.

Nothing is broken — the `.s` regenerates from the `.c`, `make clean && make
&& make compare` is green and the tree is clean either way. It is a
consistency question, cheaply fixable in either direction, and it is left for
the maintainer rather than settled by one session's reading.
