# Batch 207

Five elevated, one parked, two parks re-opened and closed again with a
measurement rather than a guess. The batch's result is a **boundary on last
batch's lever**, found by spending it on two functions where it does not work.

## Function breakdown

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_882_200950c` | `0x0200950c` | [ovl_30_…_c_a_b.c](src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_c_a_b.c) |
| 2 | `OvlFunc_883_200b1b4` | `0x0200b1b4` | [ovl_30_…_c_a_b.c](src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_a_b.c) |
| 3 | `OvlFunc_953_200a4d8` | `0x0200a4d8` | [ovl_30_…_a_c_b.c](src/overlays/rom_7d95dc/ovl_30_c_c_c_c_a_c_a_c_b.c) |
| 4 | `OvlFunc_882_200bfb0` | `0x0200bfb0` | [ovl_30_…_a_c_c_b.c](src/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_c_c_b.c) |
| 5 | `OvlFunc_882_2009348` | `0x02009348` | [ovl_30_…_a_a_b.c](src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_c_a_a_b.c) |

Parked: `OvlFunc_952_2008108` at 36 of 136, length exact.

Gated on a clean `make clean && make compare`, every address verified against
the per-overlay `overlay.elf`.

## THE BARRIER NEEDS A FUNCTION THE ROM DOES NOT SPILL

Batch 206 corrected a rule that had said a crossed mov/shift site is
unreachable, and found that a volatile asm consuming the first mov reaches it.
The obvious next move was to spend that lever on the parks it should now open.
Two of them do not open, and **why they do not is the more useful result than
another elevation would have been.**

Measured on five functions, and the split is total:

    OvlFunc_960_2008838   ROM hi-reg insns 0   barrier reaches the site
    OvlFunc_939_2008ff0   ROM hi-reg insns 0   barrier reaches the site
    OvlFunc_948_2008b68   ROM hi-reg insns 0   barrier reaches the site
    OvlFunc_961_2008120   ROM hi-reg insns 8   barrier REWRITES THE FUNCTION
    OvlFunc_901_2008c1c   ROM hi-reg insns 5   barrier REWRITES THE FUNCTION

`2008120` goes from 2 of 48 to **45 of 46**; `2008c1c` from 2 of 75 to **65 of
74**. In both the function comes out *shorter* than the ROM, which is the tell:
the barrier splits the block into two scheduling regions, every live range
crossing the split gets shorter, and gcc stops needing the high registers it was
spilling. A `mov r5, r8 / push {r5, r6}` prologue is the record of a live range
that has to stay long. Shorten it and the spill — which is **correct**, the ROM
has it — disappears.

**Read the ROM's high-register count first. Zero means the barrier is available;
non-zero means it is not, whatever the residue looks like.**

Note this runs opposite to the usual reading of r8–r11. Elsewhere high-register
traffic is a tell that *gcc* hoisted something, and pinning removes it — that is
exactly what happened in `2008ff0` and `2008b68`, whose parks record an
eight-instruction spill the pins deleted. By the time the barrier went in, those
functions had no high-register traffic left to disturb.

`tools/templated.py` already prints this as its `hi` column, computed for a
different reason — it warns that high-register traffic predicts an intractable
allocation residue. The same number answers this question, so no new screen was
needed, and all five elevations were picked with `hi == 0`.

### The pins are not implicated, and that had to be separated

On `2008120` the two levers were measured apart: an r2 pin alone leaves the
score exactly where the plain literal had it, and a **plain `int` local with the
barrier scores the same 45 as the pinned version**. Pins are cheap in a
high-pressure function; barriers are not.

## THE BARRIER IS PER-MOV, NOT PER-SITE

`200b1b4` carries two crossed sites, one of them literally the
`__Func_8012330(-1, -1, 0xe666)` call the lever was first measured on in another
overlay. Its three-register fill is

    mov r0, #0x80 / mov r1, #0x80 / mov r2, #0x80 / lsl r2, #9 / lsl r0, #10 / lsl r1, #10

One barrier after `q0 = 0x80` fixes r0 and leaves r1 and r2 transposed at 2 of
89 — the remaining pair still orders itself by which shift consumes first. A
second barrier after `q1 = 0x80` is exact. **n movs needing a given order need
n−1 barriers**, since the last has nothing left to be ordered against.

## THE BRACKET IS NOT A RECIPE

Batch 206 found a hoisted pool load that needed the intervening call *bracketed*
with a `do { } while (0)` on each side. `200950c` has the same shape and **one
wall is right; adding the second costs two instructions**, because it
over-constrains the argument fill it now sits inside.

The difference is distance. In the sibling the load had crossed two statements
and needed a wall on each side; here it crossed one, so the wall behind it is
sufficient and the wall in front only removes freedom the scheduler was using
correctly. Add one, measure, add the second only if the load is still moving.

## THE PROLOGUE WIDTH KEEPS PAYING

Three of the five were diagnosed from the push list before a single instruction
was compared:

- `200bfb0` — `0x1016` passed to three calls, gcc caches it in r5, one extra
  pushed register, **77 of 118 differing**. Three r0 pins: 3 of 116, length
  exact.
- `2009348` — three flag ids each used twice, **two** extra pushed registers,
  119 of 119. Six r0 pins: 3 of 117.
- `2008108` (parked) — one extra, same cause, same cure.

And it reads both ways: last batch `200902c` had one push *too few*, which meant
the opposite — a value that needed naming rather than rebuilding.

## ANCHOR ANY CALL THAT HAS A POOL LOAD IN IT

Two functions lost an instruction to the same wrong assumption: that a call
whose ROM order is plain ascending r0/r1/r2 needs no anchoring. It does when an
argument is a **pool load**, because gcc issues those first and the cheap
immediate last.

The controlled case is in `2009348`, which has both in one arm:

    OvlFunc_882_2009a64(0x1bd, 0x494)   both pooled   -> needs nothing
    __Func_80921c4(0, 0x1bf, 0x4cb)     one immediate -> needs anchoring

With no cheap immediate in the list there is nothing to get out of order.

## SMALLER, ALL MEASURED

**Both operands of a store have to be pinned.** `200a4d8`'s ROM interleaves an
address chain in r2 with a value chain in r3; every ordinary spelling produces
that interleave with the registers swapped, and naming the address before the
value, after it, or moving the declaration between them all give exactly 7
differing. Pinning both is exact. Same shape as last batch's accumulate,
extended from a read-modify-write to a plain store.

**A constant can reach the pool even when it fits an 8-bit immediate.**
`*(short *)a = 0x70` pools `0x70`; `v = 0x70;` as its own statement gives
`mov r3, #0x70`. The usual explanation for a pooled constant is that it does not
fit, and here it does — the trigger is the store width.

**gcc narrows an AND mask when the result is truncated.** From the parked
`2008108`: `(short)((x + 0x2000) & 0xffffc000)` emits `ldr r2, =0xc000`, because
the `(short)` throws bits 16 and up away and combine propagates that backwards
through the AND. Spelling the same constant `~0x3fff` produces the ROM's word.
This is the same family as the `short`/`unsigned short` store-width lever, from
the other end: **what lands in the pool is decided by the types around the
constant, not by the constant.**

## WHAT THE PARK COULD NOT SPELL

`2008108` is left at 36 of 136 on a single requirement: `e = 0x80 << 7` must be
built *between* the two operands of the following compare and the compare
itself. The value is not used until after the branch, so gcc sinks it past the
`bne`. A barrier hoists it to the top of the block instead — it has only
"before" and "after" to offer, and the ROM wants neither. Pinning it to the
ROM's r6 fails for a specific reason: **r6 already holds the other value**, and
the ROM's trick is that that value's last use is the instruction immediately
before, so the register frees one instruction later. Pinning makes the two
ranges overlap where the ROM has them adjacent.

The open question is how to spell "this value's range begins exactly where that
one's ends" for two separate source variables. Reusing one variable for both is
the obvious try and was not made.

## HOUSEKEEPING

`src/non_matching/ovl_780898/2008e84.c` removed as stale — both functions it
parked have been elevated and its `asm/` path no longer exists. It was the first
candidate picked this batch, on a 3-of-16 residue, and the by-name check caught
it before any work went in. That check is now the first thing done to a park.
