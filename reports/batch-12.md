# Batch 12 — 4 functions, both families finished, and a 34-function blocker narrowed

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–11 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean build, unassisted. Every address read back from the linked overlay
ELFs.

## The functions

| Function | Address | New source |
|---|---|---|
| `OvlFunc_924_2008f30` | `0x02008f30` | `src/overlays/rom_7ac2d8/ovl_e20_c_c_c_c_c_c_c_b.c` |
| `OvlFunc_924_200cf90` | `0x0200cf90` | `src/overlays/rom_7ac2d8/ovl_35b8_a_a_c_b.c` |
| `OvlFunc_958_2008d88` | `0x02008d88` | `src/overlays/rom_7e636c/ovl_cc0_c_a_c_a_c_b.c` |
| `OvlFunc_960_2008e5c` | `0x02008e5c` | `src/overlays/rom_7eaf28/ovl_314_c_c_b.c` |

**Both `GetEntrances` families are now complete** — 18 of 18 two-way, 24 of 24
four-way, 42 functions total across batches 08–12.

Two `.s` files gain `.global` lines and one is split by hand; both are called
out below.

## Two functions were parked on estimates I did not check

Worth stating plainly, because in both cases the information needed was already
on screen.

**`OvlFunc_960_2008e5c`** sat parked for three rounds. `split_s.py` refused its
cut on crossing local labels, and the park note reasoned from the file's
totals — nine functions, 54 labels, only 8 exported — that clearing it would
take *"not two exports but dozens, restructuring nine functions' worth of data
to land one 15-instruction stub."*

Computing what crosses **that specific cut** gives **one** label. The splitter
had already printed it by name in its refusal.

The park note also referenced `_ID_a6` for three rounds without that symbol
ever being defined, so the first screen after the split failed on an unresolved
symbol rather than on codegen.

**`OvlFunc_924_2008f30`** was left as assembly because its `.s` holds one
function and fourteen `.incbin` tables. That one was genuine — `split_s.py`
cuts on function boundaries and cannot separate a function from data — but it
is fixable by hand, in two steps each verified before the next:

1. Export the four tables the function selects between. Nine siblings in the
   same `.data` section were already exported this way, so the practice is the
   file's own, and a `.global` emits no bytes.
2. Cut at `.func_end` into `_b` (function) and `_c` (data), both listed in
   `overlay.ld` where the original was. The function comes first, so order and
   layout are preserved.

Keeping the export and the cut separable is what makes it reviewable: if the
ROM had shifted, it would be obvious which step did it.

## The 34-function `narrow-mask` blocker: the operand order was wrong

This is the largest open blocker and it has moved.

The ROM combines with `and r3, r2` — the **mask** is the destination and the
field is the source, i.e. `m & f`. Every previous attempt wrote `(f & m)`,
which in a destructive two-operand `and` puts the wrong register on the left
and diverges from there on.

With `(m & f)` the last five instructions are identical to the ROM. What
remains is eleven against eleven with the field and mask in **swapped
registers**:

    rom    mov r3,#3 / ldrb r2,[r0,#9] / and r1,r3 / mov r3,#0xd / neg r3,r3
    ours   mov r3,#3 / and r1,r3 / mov r2,#0xd / ldrb r3,[r0,#9] / neg r2,r2

Under `REG_ALLOC_ORDER {3,2,1,0}` that means the ROM builds the **mask** pseudo
first and we build the field first. So the residue is **register birth order**,
not scheduling — a class this tree already has a worked fix for
(`Func_808ed4c`, solved by respelling a scaled index).

Eleven formulations are now recorded in
`src/non_matching/overlays/narrow_constant.c`. Eight of them varied statement
*order* while the actual error sat in a single expression.

### A related correction

`OvlFunc_924_200cf90` emits `mov r7, #1 / neg r7, r7` — the same `mov`/`neg`
pair this blocker is named for — **with no help at all**, because its `-1` is
compared twice and gcc keeps it live in a register.

So the pair is not inherently hard. The blocker is about constants used
**once**. Checking whether the value is genuinely reused costs seconds and
should come before any formulation.

## Technique: naming an intermediate stops gcc folding it

Two matches now turn on this, so it is stated once rather than as separate
tricks:

    int off = (slot << 1) + 0xd8;
    *(short *)((char *)unit + off) = value;

Inline, gcc folds the offset into the base pointer and stores `strh r3,[r0]`;
named, it keeps the offset in its own register and produces the ROM's
`strh r2,[r0,r3]`. Parenthesising, spelling `slot * 2`, and indexing a rebased
`short *` all give the folded form.

The earlier instance: a local keeps a shifted constant's `mov`/`lsl` pair
contiguous (`OvlFunc_956_20081b4`, batch 10).

## Assembly edited rather than replaced

The complete list for this batch, since these are the changes a reviewer might
not want:

| File | Change |
|---|---|
| `asm/overlays/rom_7eaf28/ovl_314_c_c.s` | `+.global .L1a00` |
| `asm/overlays/rom_7ac2d8/ovl_e20_c_c_c_c_c_c_c.s` | `+.global` ×4, then split by hand into `_b`/`_c` |

Both revert cleanly.

## Still open, and still only answerable by you

- **Semantic names for the id space.** `unknown_id.sym` now carries ~45
  entries. The values form a dense contiguous space in per-area runs, range
  `0x10`–`0xba`.
- **Five ambiguous offsets in `actor.h`** (batch 03), documented rather than
  guessed.

## Reproducing the verification

    docker build -t goldensun-build -f tools/Dockerfile .
    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        sh -c 'make clean && make compare'
