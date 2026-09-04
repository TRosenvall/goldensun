# Batch 218

Six elevated, two parked, and one new tool. The batch's centre is a blocker
class this tree had never seen — **a function whose instructions and size both
match and which still fails `make compare`** — and the tool added to catch it.

## Function breakdown

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_880_2008154` | `0x02008154` | [ovl_30_c_c_a_a_a_b.c](src/overlays/rom_7795e8/ovl_30_c_c_a_a_a_b.c) |
| 2 | `OvlFunc_928_2008500` | `0x02008500` | [ovl_314_a_a_c_c_c_b.c](src/overlays/rom_7b6668/ovl_314_a_a_c_c_c_b.c) |
| 3 | `OvlFunc_897_200935c` | `0x0200935c` | [ovl_30_c_c_a_c_a_a_b.c](src/overlays/rom_791794/ovl_30_c_c_a_c_a_a_b.c) |
| 4 | `OvlFunc_911_200a6cc` | `0x0200a6cc` | [ovl_30_c_c_a_b.c](src/overlays/rom_79e5c0/ovl_30_c_c_a_b.c) |
| 5 | `Func_808bd24` | `0x0808bd24` | [rom_8ba38_a_a_a_c_a_b.c](src/rom_8a000/rom_8ba38_a_a_a_c_a_b.c) |
| 6 | `Func_80903bc` | `0x080903bc` | [rom_8d9a4_…_a_b.c](src/rom_8a000/rom_8d9a4_c_c_c_a_a_a_a_a_b.c) |

Parked: `Func_80982dc` (pool ORDER), `Func_80b5a0c` (94 of 97, induction-variable
form).

Gated on a clean `make clean && make compare`, every address verified against
the linked ELF with `tools/checkaddr.py`.

## `tools/objcmp.py`: WHAT tryc.py STRUCTURALLY CANNOT SEE

`tryc.py` compares INSTRUCTION STREAMS, and to stay robust against where a
literal pool happens to land it normalises every PC-relative load to `=value`.
That is the right call for screening, and it makes it blind to two real things.
Both showed up in this batch, in opposite directions:

**A false PASS.** `Func_80982dc` screens clean — 86 instructions against 86,
196 bytes against 196, ZERO differing mnemonics — and fails `make compare`. Its
nine pool words are the ROM's nine, **rotated**: gcc emitted the last one first.
The ROM's `.s` reaches constants with `ldr rX, =value`, so the ASSEMBLER pools
them in instruction order; gcc emits its own `.word` list in its own order.
Every `ldr [pc, #N]` then points four bytes further and the normaliser hides it.

**A false FAIL.** `OvlFunc_928_2008500` screens at "6 differ" and is
byte-identical. gcc put its pool-dump target and its epilogue label at the same
address, and the ROM's disassembly can only show one label there.

`tools/objcmp.py` compares at the object level — size, every instruction
encoding, every relocation — and gets both right. It is not a replacement for
`tryc`: `tryc` tells you *which instruction* is wrong while you iterate, this
tells you whether you are actually done. **Run it whenever `tryc`'s own `!!`
warning says the reference keeps its pool inside the function**, which marks
exactly the cases where the normalisation hides something. It is also not a
replacement for `make compare`, which still catches layout and linker-script
mistakes it cannot see.

The tool self-tests on this batch: it rejects the parked function and accepts
all six elevated ones.

## THE COST OF NOT HAVING IT, recorded honestly

Two functions were written into the build in one step, and the build went red.
Neither could be blamed from the screen, because both screened clean, so it took
a bisect — restoring one to assembly and rebuilding — to learn which. That is
the whole argument for the tool, and also for a discipline this batch confirms:
**write and build ONE function at a time.** Batching two saves a minute and
costs a bisect the moment either is wrong.

## THE TWO NAMING RULES ARE ONE RULE

Batch 216 added "do not name an intermediate that is consumed immediately — the
name is what buys it a register". `OvlFunc_897_200935c` is the exact inverse:
each of its four switch arms stores through the same global, and written
directly the address is computed into a scratch register *after* the call, which
makes every arm's store textually identical to the early-return path's store —
so cross-jumping merges them and the function comes out two lines short. A named
pointer per arm buys the address a callee-saved register, materialised before
the call, and the merge stops happening.

So the rule is one rule with a test: **name a value that must SURVIVE
something** — a call, or a register choice you need — **and do not name one
whose only role is to be consumed immediately.**

## A POOLED SMALL CONSTANT IS NOT AUTOMATICALLY A SYMBOL

`OvlFunc_928_2008500`'s `ldr r2, .L58c @ 0` reads exactly like the
pooled-constant-means-linker-symbol tell, and `(int)&_AREA_00` reproduces the
pool TEXT. It is wrong. A bare literal `0` gives a *narrow* pool reference
(`ldrh` against `.word 0`) that encodes the same halfword, and that one token
fixes three residues at once — it dumps the pool inside the function with the
ROM's branch over it, sorts `.word 0` ahead of a symbol whose instruction comes
earlier, and forces the address temporary into `ip`.

Ten spellings around that temporary were measured and **every one tied at
exactly 27 differing** or was worse. That exact tie is itself the tell that the
variable was never the problem.

This also contradicts a note in `src/non_matching/ovl_7d6418/2008dd0.c` —
"byte stores have no QImode analogue of the halfword pooling exception.
MEASURED: they do not." One plainly does. That park should be re-measured.

## SIX MORE LEVERS, each measured

  * **Let gcc CROSS-JUMP a shared tail; do not write the share as a `goto`.**
    On `OvlFunc_911_200a6cc` the explicit-share form never beat 2 differing
    across six configurations of pins and barriers, always leaving the bound's
    pool load in the register the limit vacated. Writing both arms out in full
    is exact. An explicit `goto` share is a constraint on the allocator that
    cross-jumping is not.
  * **A NAMED BYTE OFFSET blocks reassociation.** `off = idx * 4 + 0x14` keeps
    the ROM's separate `add`; inline, gcc folds the constant into the load's
    displacement.
  * **STATEMENT ORDER decides register naming.** On `Func_808bd24`, fetching one
    global before the other rather than after is 12 differing against 24, for a
    reordering that changes nothing semantically.
  * **A main-ROM function CAN reach `divsi3_RAM` legitimately.** `Func_80903bc`
    calls it through a function-pointer local, not the `/` operator — so the
    `__divsi3` alias blocker that parks `809088c.c` does not apply when the ROM
    reaches the helper indirectly. That park is worth re-reading.
  * **The callee's RETURN TYPE decides r0 fill order for a DIRECT call too.**
    `extern int StopTask(void *)` fills r0 last and matches; `extern void` fills
    it first. Two lines, and a named zero for the adjacent store changed nothing.
  * **`do { } while (0)` is a scheduling barrier that emits nothing.** On
    `OvlFunc_880_2008154` the last five lines were the order of two independent
    pool loads; wrapping a pair of statements splits the block into two
    scheduling regions and closes it, where the volatile-asm barrier lands the
    load too early (9 differing) or still needs help (3).

Two negatives from the same function are worth as much: **do not pass a computed
value as an inline's argument** — gcc evaluates arguments *before* the inlined
body, so it hoists above anything the inline does — and **widen a variable so an
explicit cast survives**, since a `u16` has its narrowing folded away at tree
level where the ROM keeps the un-folded `lsl`/`lsr` pair.

## THE PARKS

`Func_80982dc` is the pool-order case above. Three spellings chosen specifically
to change when gcc expands the offending constant all produced **byte-identical**
pool orders, which is the evidence that source position does not drive it.

`Func_80b5a0c` reached 94 of 97. Its useful finding: **when a ROM folds a
constant into the INDEX register rather than the base, the BASE is a real
variable.** Collapsing `s + 2` with a `0x64` index into one pointer is ten
instructions short; writing the base out recovers all ten. The remaining three
lines are induction-variable form — the ROM runs a walking byte offset against a
fixed buffer base, and gcc strength-reduces the subscript into a walking pointer.
Four spellings all did so, and the park names why that is not yet evidence of a
wall: every one varied the SUBSCRIPT, none varied the base, which is `sp` and so
freely rematerialisable.

## ON RUNNING THIS IN PARALLEL

Six independent functions were screened concurrently by subagents while the
build tree and git stayed single-writer: agents drafted and screened only
(`tryc` and `objcmp` compile to temp files and touch neither `asm/` nor the
build), and splitting, writing, building and committing stayed in one place.
Nothing raced, and every agent result was re-verified independently before being
written into the tree — one of them also corrected a wrong file path in its own
brief, and another found the `_AREA_00` result above by measuring rather than
accepting the tell.
