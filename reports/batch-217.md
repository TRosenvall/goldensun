# Batch 217

Five elevated, one parked, one existing park restored and extended. The batch's
recurring theme is **register live ranges as a source-level decision**: three of
the five needed a value's lifetime changed rather than its spelling, and the two
parks are both cases where no spelling reaches the lifetime the ROM has.

## Function breakdown

| # | function | address | file |
|---|---|---|---|
| 1 | `InitSprites` | `0x0800bb20` | [rom_b798_c_c_a_b.c](src/rom_9000/rom_b798_c_c_a_b.c) |
| 2 | `Func_808d8f0` | `0x0808d8f0` | [rom_8d5dc_c_b.c](src/rom_8a000/rom_8d5dc_c_b.c) |
| 3 | `OvlFunc_960_2008e8c` | `0x02008e8c` | [ovl_314_c_c_c_b.c](src/overlays/rom_7eaf28/ovl_314_c_c_c_b.c) |
| 4 | `OvlFunc_913_200a88c` | `0x0200a88c` | [ovl_30_c_c_c_c_a_b.c](src/overlays/rom_7a04ac/ovl_30_c_c_c_c_a_b.c) |
| 5 | `OvlFunc_913_200a974` | `0x0200a974` | [ovl_30_c_c_c_c_a_c.c](src/overlays/rom_7a04ac/ovl_30_c_c_c_c_a_c.c) |

Parked: `OvlFunc_881_200b95c` (74 lines against 74, six real differing lines).
Restored and extended: `CreateSpriteLayer` — see the housekeeping section, which
is a process failure worth reading before the findings.

Gated on a clean `make clean && make compare`, every address verified against
the linked ELF with `tools/checkaddr.py`.

## HOUSEKEEPING FIRST: I OVERWROTE AN EXISTING PARK

`CreateSpriteLayer` already had a park. I picked the function opportunistically
because it sat beside `InitSprites` in the same file, did not check
`src/non_matching/` by name, and wrote a new park over it.

The prior park was **better than mine** — 66 lines against 66 with 45 differing,
where my body gave 62 of 66 — and it carried the history behind a
`tools/poolblocked.py` correction that reopened a 501-function class. It is
restored verbatim.

My three new attempts were re-run against **its** body so the numbers are
comparable, and all three are worse (62, 63 and 56 differing against its 45), so
its "nothing source-level for the spill" conclusion stands unchanged. One of the
three is worth keeping and is now appended to it:

> **A PIN CANNOT ASK FOR A SPILL.** Pinning the variable to r4 — the very
> register the ROM spills *from* — leaves the output unchanged, because the pin
> is assigned before a call and used after it and is therefore silently dropped.
> That is the batch 210 hazard, and it also blocks the one use that would make a
> pin a spill tool.

The discipline that failed here is already on file from batch 203: **check for
an existing park by name before starting.** It was applied to every other target
this batch and skipped on the one chosen out of convenience.

## `size.sym` GAINS A MEMBER THAT PROVES THE SPACE IS REAL

`InitSprites` copies a 0x7c-byte ARM helper into RAM. 0x7c is the **same value**
as `_SIZE_8015e10` from batch 216 — but a different routine, in a different
file, pooled in the function that copies its own. `Func_800a418` runs from
0x0800a418 to `Func_800a494`, so the size is the gap, exactly as for the
rom_15430.s three.

They are separate symbols for that reason. A single shared `_CONST_7c` would
have matched the bytes and asserted something false. Two symbols with one value
is what an id space looks like when it is real rather than a coincidence, and
0x7c fits an eight-bit `mov`, so this one meets `const.sym`'s criterion 1
literally rather than by extension.

## DO NOT PUT TWO COMPETING PINS ON ONE REGISTER

`InitSprites` keeps a zero in r5 across two DMA writes and then the buffer size
in r5 afterwards. That reads exactly like the one-variable-two-ranges entry —
two `register` declarations naming r5 with different types — and it is **wrong
here**. With both pinned, the allocator puts the DMA control word in r4 and
shifts the size out of place (`lsr r2, r5, #0x2` against the ROM's destructive
`lsr r5, #0x2`), costing four lines. Pinning only the zero and leaving the size
free lets gcc pick r5 by itself and the transfer comes out exact.

**One pin plus a free choice beats two pins**, when the two ranges are not
simultaneously constrained.

## THE NARROW-STORE TABLE, ALL THREE ROWS IN ONE FUNCTION

`Func_808d8f0` stores a literal zero through a 16-bit field, and the three
spellings the table lists give three different results in the same place:

| spelling | result |
|---|---|
| `*(short *)(b + 0x19c) = 0` (cast) | zero POOLED — 21 differing |
| `z = 0; *(short *)(b + 0x19c) = z` (named local) | immediate, but the local takes a register and every scratch register shifts — 17 differing |
| `b->timer = 0` (typed field) | immediate in a SCRATCH register — **exact** |

The table already says "prefer the typed field", and this is the cleanest
demonstration on file of *why*: the named local is not merely equal, it is
measurably worse, and its cost shows up as a whole-function register shift
rather than at the store.

The same function also needs `gKeyHeld` declared `volatile`, because one arm
reads it twice around a branch and the ROM loads it twice from an address kept
in a register; without volatile gcc commons the two reads and the second load
disappears.

## COMPUTE RANGE BOUNDS, DO NOT READ THEM OFF

`OvlFunc_913_200a88c` tests five bounding boxes. Written as ordinary
`x > LO && x < HI`, gcc folds each pair into the unsigned range idiom — one
`add` of a negated constant and one `cmp`/`bhi`. That part was right first time.

**Three of the five upper bounds were wrong**, because the folded form encodes
`lo` as a negated constant and `hi - lo` as a span, and adding the span to the
low bound carries into the next hex digit. `0xC00001 + 0x51FFFE` is `0x111FFFF`,
not `0xC51FFFF`, and reading it off the digits gives the second answer every
time. The tell is immediate and unambiguous: the emitted span constant disagrees
with the ROM's, so `ldr r1, =0xb91fffe` against `=0x51fffe` says the *bound* is
wrong, not the idiom. Compute the bounds arithmetically.

Its layout residue is the single-exit rule seen from a new angle: with a
`return 0` ending each branch, gcc lays the success block after the exit and
every box needs an extra branch to reach it — 91 lines against 86. Routing all
misses to one `out:` label puts the success block where the ROM has it and the
fall-through costs nothing.

## A SWITCH THAT DISPATCHES WITH `bhi` IS SWITCHING ON AN UNSIGNED VALUE

`OvlFunc_913_200a974` compares its state counter with `bhi`, not `bgt`. Declared
`int` the whole function shifts; `unsigned int` fixes it. One instruction's
condition code identifies the type of a global — worth checking on every switch
before treating a dispatch mismatch as a structural problem.

Two more of its residues are lifetime rather than spelling:

  * **The zero stored in case 3 is the spawned actor's own variable.** The ROM
    emits `mov r5, #0` *before* the switch and stores r5 in one arm, and r5
    later holds the actor. A separate zero local does not reproduce it — gcc
    sinks it into the arm — and a barrier on that local puts it in a scratch
    register instead. Initialising the *actor* to 0 up front and storing it in
    the arm gives the callee-saved register a range long enough to reach there.
  * **A cross-jumped tail may share less than it looks.** Both arms load the
    counter themselves and only the compare-and-decrement is shared; writing the
    load once in the shared block leaves the function two instructions short.

It also **brackets the two crossed-site cures inside one function**: one site
falls to the barrier-free reordering (write the load between the constant's
`mov` and its shift), and the other does not — reading the byte into a local
first still leaves `mov r3, #0xd` ahead of the `ldrb`, and only a barrier on the
loaded value holds the order. Same function, same batch, both answers.

## THE PARK: NAMING AN ARGUMENT IS DECISIVELY WRONG

`OvlFunc_881_200b95c` is 74 lines against 74 with seven differing, one of which
is a phantom — `bl __umodsi3` against `bl _umodsi3_RAM`, which this overlay's
linker script already aliases. The six real lines are one thing: in three of
four switch arms the ROM fills the third argument before the second, and we do
the reverse.

Seven spellings were measured. The three inline forms tie at 7; **all four named
forms are three to seven times worse** (24, 26, 47, 49), because the name buys
the value a register of its own and the four arms stop sharing their tail. That
is batch 216's "do not name an intermediate that is consumed immediately"
confirmed from the failure side, on a function already within six lines.

The park names what was *not* varied: which register holds the per-arm constant.
In the ROM, the one arm whose x-offset constant lands in r2 has our order — so
the residue is register assignment, not argument order, and nothing tried
touched it.
