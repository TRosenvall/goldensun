# Batch 116 — what the round measured but did not land, and what it cost to run

Companion to [batch-115](batch-115.md), which lists the 32 functions that
landed. Same clean build, same SHA1. Nothing here changes the ROM; twelve new
park files and three tree changes still owed.

## Twelve parks, and where the mass sits

| Function | Best | Class |
|---|---|---|
| [`OvlFunc_907_2008ed8`](../src/non_matching/ovl_79b154/2008ed8.c) | **1 of 44** | pool-loaded zero in one arm only |
| [`OvlFunc_942_20087dc`](../src/non_matching/ovl_7c6bac/20087dc.c) | **2 of 53** | arg interleave, straight line |
| [`OvlFunc_948_2009df8`](../src/non_matching/rom_7d30e0/2009df8.c) | 18 of 40 | arg interleave, straight line |
| [`OvlFunc_957_2008a54`](../src/non_matching/ovl_7e3e08/2008a54.c) | **3 of 50** | scheduling: store vs sign-extension |
| [`OvlFunc_882_200a09c`](../src/non_matching/ovl_77dd1c/200a09c.c) | **3 of 49** | which register holds an address |
| [`OvlFunc_951_2008880`](../src/non_matching/ovl_7d6418/2008880.c) | 4 of 47 | scheduling, two adjacent swaps |
| [`OvlFunc_950_20080c0`](../src/non_matching/ovl_7d5838/20080c0.c) | 5 of 53 | load order in a table read |
| [`OvlFunc_943_2009920`](../src/non_matching/ovl_7c7b9c/2009920.c) | 5 of 50 | r0 second in a 3-arg call |
| [`OvlFunc_900_20081e4`](../src/non_matching/ovl_797740/20081e4.c) | 6 of 54 | `orr` operand order |
| [`OvlFunc_898_2009754`](../src/non_matching/ovl_793768/2009754.c) | 8 of 44 | scheduling at a join + **2 are a linker alias** |
| [`OvlFunc_943_20088e0`](../src/non_matching/ovl_7c7b9c/20088e0.c) | 11 of 46 | three-way register rotation |
| [`OvlFunc_918_20097ec`](../src/non_matching/ovl_7a5214/20097ec.c) | 18 of 47 | table base kept live across a loop |
| [`OvlFunc_899_200c754`](../src/non_matching/ovl_794ac0/200c754.c) | 37 of 53 | CSE of two differently-signed reads |

Six of these are at **five or fewer** differing lines out of roughly fifty.
That is a different distribution from earlier batches and it is worth naming:
the levers that have accumulated in the doc now get most functions structurally
right, and what is left over is concentrated in **instruction scheduling and
register naming** — two things the C source addresses only indirectly.

## Two structural findings that are worth more than the functions they came from

**A shared tail store defeats gcc's speculation where a shared local does not.**
`OvlFunc_943_20088e0` is `if (c) { small } else { big }` with one `strb` after
the join. Written with a shared result variable and a single store after the
`if` — including the `goto` spelling of the same thing — gcc **speculates the
cheap arm above the compare**, inverts the branch, and the small block stops
being a block at all: 45 of 46. Writing the store **inside each arm** and letting
cross-jumping merge the two identical `strb`s reproduces the ROM exactly: 11 of
46, a 34-position improvement from one restructuring.

> When the ROM's short arm is a real basic block ending in `b`, the source stores
> in the arm. When gcc collapses your short arm, you gave it something
> speculatable.

This is the mirror image of the existing tail-merge notes, which are about arms
that *should* merge.

**Reading a field twice is what produces a redundant-looking `mov`.**
`OvlFunc_882_200a09c` went 23 → 3 differing on this alone. The ROM has
`ldrb r3,[r3] / cmp r3,#0 / … / mov r1, r3`. `n = p->f27; if (n != 0) {…}`
coalesces the copy away; writing the *guard* on the field and the *body* on a
local — two textual reads that gcc then CSEs — reproduces the copy. This
generalises the rebuilt-vs-carried rule to a value the ROM reads once but the
source names twice.

## A correction to a "tell" the doc has been asserting

**`ldr rN, =0` in the ROM is not a symbol tell.** The doc's pooled-small-constant
section says gcc never pools a constant it can `mov`, and sends the reader to
`const.sym`. `OvlFunc_945_2008284` has two `ldr r3, =0` sites and a **plain
literal `0` reproduces both, byte-exact**. gcc-2.96 writes them as
`ldrh r3, .L11` + `.word 0` in its assembly text, which the assembler encodes as
`ldr r3, [pc, #imm]`.

There are **53 `ldr rN, =0` sites** across `asm/`. The shape appears where gcc
cross-jumps two arms into a shared tail and needs the constant in a register on
the merged path — in the same function the non-merged arm uses a register that
already holds 0. Anyone who reached for a `_CONST_0` on the strength of the old
rule was chasing a symbol that does not exist.

## Method: what running four agents actually cost and returned

Round 2 sent four agents 14 functions each (56 total), worklists filtered by
`tools/blocked_cse.py` to exclude the unreachable constant-CSE shape.

| | screened OK by agent | confirmed on my re-screen | landed |
|---|---|---|---|
| agent 1 | 12 of 14 | 12 | 12 |
| agent 2 | 8 of 14 (+2 behind tree changes) | 7 new (1 was mine) | 7 |
| agent 3 | 3 of 14 (+4 byte-identical) | 3 + 4 | 7 |
| agent 4 | 6 of 14 | 6 | 6 |
| **total** | **33** | **32** | **32** |

**Every single agent-reported OK reproduced on my re-screen.** The one
subtraction is a duplicate, not an error: agent 2 and I independently derived
`OvlFunc_959_2009880` and produced the same C.

That duplicate is worth keeping rather than deduplicating away. Agent 2 reached
the early-return spelling from a reading rule I had not written down —

> **The number of `mov r0, #0` sites tells you how many `return 0` statements
> there are.** One zero site means one combined condition; two means the first
> test is a separate early return.

— and I reached it by staring at the diff. Same answer, and the rule is now in
the doc where the staring is not.

Three operational notes for anyone running this:

* **`tryc.py` can screen against a `.sym` addition without touching the tree.**
  Bind-mount a modified copy over the one path:
  `-v "$PWD/scratch/message_plus.sym:/work/message.sym:ro"`. This turns "1
  differing, and it is the symbol I would have to add" into a real `OK`
  *before* anyone edits a linker fragment.
* **Worklist `ref` paths go stale.** A `.s` that has been split since the
  worklist was generated no longer exists at the recorded path. `showfunc.py
  <name>` resolves it in one call and also catches the already-elevated case,
  since `tryc.py` refuses a generated `.s`. Make it step 0.
* **The bottleneck moved to me.** Screening is now cheap and parallel;
  splitting, wiring, building and committing are serial and mine alone. This
  round that serial tail was most of the wall clock. Batching the integration —
  33 screens integrated in five builds rather than one build each — is what
  made the round worth running, and is the thing to keep doing.

## Tree changes still owed

1. **`overlays/rom_793768/overlay.ld` needs `__divsi3 = _divsi3_RAM;`.** Two of
   `OvlFunc_898_2009754`'s eight differing lines are this and nothing else. The
   overlay's `imports.s` already exports `divsi3_RAM`; the pattern to copy is
   `overlays/rom_7a5214/overlay.ld:79`. Same is owed to
   `overlays/rom_7bc690/overlay.ld` before `OvlFunc_933_2008344` is wired.
2. **`message.sym` needs `_MSG_26e3 = 0x26e3;`** for `OvlFunc_967_20080c8`,
   which is otherwise a verified 58-line match. `message.sym` is **not** a
   tracked dependency of `stage1.o` — delete that object by hand after adding it.
3. **`OvlFunc_common2_380` wants a per-file `-fcall-saved-r4` rule.** With the
   tree's global `-fcall-used-r4` it is 12 of 52 and **every one of the 12 is an
   r4-vs-r5 rename**; with `-fcall-saved-r4` it is exact. The supporting
   measurement is a one-line grep: **0 of 2134 generated `.s` files under
   `asm/overlays/` contain a `push {r4`**, while both functions in
   `common2_c_c_c_c_a.s` do. A hand-written `.s` that pushes r4 cannot have been
   built with the global flag. This is a cheap screen to run across the whole
   parked set — the signature is distinctive and currently reads as
   "register allocation, unreachable from C".
4. **`OvlFunc_common1_e10`** is a verified 47-line match behind a low-numbered
   label collision (`.L2 .L3 .L11 .L12 .L13`), proved with placeholder names.
   The references come from exactly one file — the same one being split — so the
   rename is two files.

Item 3 is the one to do first. If the `push {r4` grep is the discriminator it
appears to be, it reclassifies a slice of the register-allocation parks, which
is the largest blocked class in the corpus.
