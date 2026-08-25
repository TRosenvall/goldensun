# Batch 41 — the constant-CSE class splits by control flow

*Status: ready to port.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–40 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean `make clean && make -j8 && make compare` — 96 overlays compared
byte-for-byte and `goldensun.gba: OK`. Every address read back from the linked
overlay ELF with `nm`, and the new symbol from `stage1.o`.

## Read this first: which constant-CSE members are worth attempting

Batch 40 showed the basic-block lever defeats constant-CSE, and unparked
`OvlFunc_922_2009750` — a **documented counter-example** to the
`-fno-rerun-cse-after-loop` rule, matched with no flag at all.

`OvlFunc_943_200c218` is a **second counter-example**, and it does **not** yield.
Two shifted constants are passed to one call and again to the next — precisely
the shape the flag is supposed to handle — and the flag is byte-identical, 14 of
25 with and without.

The difference between the two is the whole finding: **`OvlFunc_922_2009750` has
a branch and `OvlFunc_943_200c218` does not.** The lever needs the values
assigned in a block that dominates the uses and is different from them, and a
straight-line function has no such block.

| shape | what to try |
|---|---|
| repetition across a call, **with a branch** | the basic-block lever, separate locals |
| repetition across a call, **straight-line** | `CSE_CFLAGS` — and not always even that |

That is now a mechanical test you can apply before spending a screen, and it
retires the guesswork this class has carried since batch 25.

The straight-line half needs the same missing construct as
`src/non_matching/ovl_77dd1c/200c5b8.c` and the `-1` triple in
`src/non_matching/ovl_787e04/20093e4.c`: **a way to make gcc rematerialise a
value inside one basic block.** Three parked shapes, one construct — the
clearest single open question in the tree.

## Corrected: the sprite-load hoist is scheduling, not aliasing

Batch 40's note on `OvlFunc_927_2008ab0` / `Func_809a44c` — an overlay/main-ROM
twin pair with an identical 2-of-27 diff — said gcc "proved the two cannot
alias" and pointed a future attempt at the alias code. **That was wrong.** The
full listing shows where the load lands, which the region summary did not:

    rom    ldr r3,[r0,#0x1c] / add r3,r2 / str r3,[r0,#0x1c] / ldr r1,[r0,#0x50]
    ours   ldr r3,[r0,#0x1c] / ldr r1,[r0,#0x50] / add r3,r2 / str r3,[r0,#0x1c]

gcc drops the load into the slot between a load and its use — the **post-reload
scheduler filling a load-use stall**. That also explains a flag result that had
looked backwards: `-fno-schedule-insns2` *does* fix this instruction and goes to
8 because turning the scheduler off moves seven others. The function wants the
scheduler on everywhere except one slot, which no flag expresses.

**The tooling lesson is the transferable one.** `--align`'s region summary said
*the load moved*; the full listing said *the load moved into a stall slot*, and
only the second is a diagnosis. Use `--full` on anything where the region
summary does not obviously imply a mechanism.

## The declaration lever, read off the ROM

`OvlFunc_888_20086e8` is thirteen calls and matched on one screen, because the
argument order was read off the ROM before anything was written. Exactly one of
its five multi-argument callees has `r0` filled first, so exactly one is
declared:

    mov r1, #1  / mov r0, #1  / bl __Func_8093500      <- withhold
    mov r2, #0  / mov r1, #0  / mov r0, #8 / bl ...    <- withhold
    mov r1, #4  / mov r0, #8  / bl __MapActor_DoAnim   <- withhold
    mov r1, #0  / mov r0, #8  / bl __ActorMessage      <- withhold

`OvlFunc_896_200a674` and its twin are the other side: `r0` in the **middle** of
an all-`mov` block, which the lever reaches **additively** — declared, they
match; undeclared, two positions out. That confirms batch 26's rule on a fresh
pair, and `pick_candidates.py`'s `r0-mid: all-mov` tag predicted it.

## Functions

| function | address | overlay | note |
|---|---|---|---|
| `OvlFunc_896_200a674` | `0x0200a674` | rom_78ef88 | r0-mid, declare |
| `OvlFunc_896_200a6e0` | `0x0200a6e0` | rom_78ef88 | twin |
| `OvlFunc_931_2008448` | `0x02008448` | rom_7b8cb0 | third of a family |
| `OvlFunc_957_200b4bc` | `0x0200b4bc` | rom_7e3e08 | three-message prompt |
| `OvlFunc_933_2008c6c` | `0x02008c6c` | rom_7bc690 | un-rotated retry loop |
| `OvlFunc_888_20086e8` | `0x020086e8` | rom_7892c8 | four subtractive declarations |

One symbol added: `_MSG_217f`.

`OvlFunc_931_2008448` walks into the trap that parked the first member of its
family for several rounds — a flag guards only the extra `__MessageID`, and the
`__ActorMessage` after it runs either way. **Third time in one overlay.**

`OvlFunc_933_2008c6c` reads `r` after a loop where `r` is only assigned inside
it. That is safe because the loop always runs once, and it is what the ROM does.
It reads like a bug and is not; the header says so.

## Parked

`OvlFunc_943_200c218` (rom_7c7b9c), 14 of 25 — the straight-line constant-CSE
case above, with five things tried and their numbers.

## Counts

355 functions elevated in total, of which 7 are fakematches. 2,940 hand-written
functions remain in `asm/` of 5,714. 90 parked functions and the two
large-function experiments.
