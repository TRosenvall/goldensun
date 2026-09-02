# Batch 181

Eight functions. **Five were parked**, one of them by me two batches ago. The
batch's method result is that the parked set has been ranked on the wrong number
for most of this project's life, and re-measuring it changes verdicts, not just
numbers.

## Function breakdown

| # | function | address | file | previously | what it took |
|---|---|---|---|---|---|
| 1 | `OvlFunc_939_20092a4` | `0x020092a4` | [ovl_314_c_a_c_c_c_c_b.c](src/overlays/rom_7c460c/ovl_314_c_a_c_c_c_c_b.c) | **parked (mine)** | one expression, not two statements |
| 2 | `Func_8011644` | `0x08011644` | [rom_11568_a_c_c_a.c](src/rom_9000/rom_11568_a_c_c_a.c) | — | register birth order |
| 3 | `Func_80931ec` | `0x080931ec` | [rom_92950_c_c_c_c_a.c](src/rom_8a000/rom_92950_c_c_c_c_a.c) | — | three oracles read off the reference first |
| 4 | `Func_80b8db8` | `0x080b8db8` | [rom_b8228_c_a_c_a_c_c_c_b.c](src/rom_b5000/rom_b8228_c_a_c_a_c_c_c_b.c) | — | `char buf[4]`; assignment order; **a named zero** |
| 5 | `OvlFunc_931_2008d08` | `0x02008d08` | [ovl_30_…_c_c_b.c](src/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_b.c) | **parked** | delete the local |
| 6 | `OvlFunc_960_2008ce4` | `0x02008ce4` | [ovl_314_c_c_a_b.c](src/overlays/rom_7eaf28/ovl_314_c_c_a_b.c) | **parked** | zero-extend on the READ side |
| 7 | `OvlFunc_964_20094ac` | `0x020094ac` | [ovl_30_a_c_c_a_a_b.c](src/overlays/rom_7ed0a0/ovl_30_a_c_c_a_a_b.c) | **parked** | delete the value local, then the `orr` rule on one arm |
| 8 | `OvlFunc_931_200807c` | `0x0200807c` | [ovl_30_c_c_a_a_a.c](src/overlays/rom_7b8cb0/ovl_30_c_c_a_a_a.c) | **parked** | delete two address-only locals |

All eight verified in the linked ELF with a `.gcc2_compiled.` symbol at the
address after a clean `make clean && make -j8 && make compare`.

## THE PARKED SET WAS RANKED ON THE WRONG NUMBER

Batch 180 found one park recorded as "47 of 61" that was six instructions away.
That was not a one-off. A **positional** diff compares instruction *i* to
instruction *i*, so the moment one side has an extra instruction everything
after it reports as differing. `tryc.py --align` reports disagreeing *regions*.

`tools/realign.py` (new) re-measures the parked set on that metric. Re-ranked so
far, the verdicts move by an order of magnitude:

| park | recorded | aligned |
|---|---|---|
| `HeightTile_4` | 22 of 28 | **3** |
| `OvlFunc_939_20092a4` | 38 of 55 | **2** → matched |
| `OvlFunc_952_2008264` | 33 | **6** |
| `OvlFunc_931_2008904` | — | **7 of 222** |

It also **inverted a recorded flag comparison**: `OvlFunc_952_2008264`'s park
measured default at 33 and `-fno-rerun-cse-after-loop` at 57 and concluded the
flag was a loss. Aligned, the flag is *better* — 8 against 9 — and it makes all
three of that function's pool loads exact.

## A PARK OF MINE, AND WHY THE DIAGNOSIS WAS WRONG

I parked `OvlFunc_939_20092a4` in batch 177 reading the ROM's
`lsl r3,#6 / mov r5,r3 / add r5,#0xe6` as a shift-in-place plus a copy, and
filed it under the named-intermediate lever's recorded failure mode — *"the
lever needs the two values simultaneously live, which a shift's input and output
are not."*

**The copy is not about the shift.** Thumb has no three-operand add for an
immediate above 7, so `+ 0xe6` must be destructive, and the shift result and the
biased value therefore have to be two separate pseudos.

```c
ang = X << 6;  ang += 0xe6;      /* ONE pseudo -- += reassigns; gcc folds the copy */
ang = (X << 6) + 0xe6;           /* TWO -- the ROM's three instructions */
```

> **Where the ROM has `<op> rX,#n / mov rY,rX / add rY,#K` with K > 7, write it
> as one expression, not two statements.** This is the mirror of the recorded
> "derivation as its own statement" lever: that one wants the boundary, this one
> must not have it.

The precedent was already in the tree — `src/rom_9000/rom_1219c_b.c` writes
`off = ((layer & 3) << 2) + 0x28;` and emits exactly `lsl / mov / add`.

## A FIX FOR A CLASS THE NOTEBOOK LISTS AS UNSOLVED

Storing a literal zero to a `short` lvalue is **always** a halfword pool load in
gcc-2.96. Seven spellings measured — `short *`, `unsigned short *`, a struct
field, an `int h:16` bitfield, `&= 0` — and every one gives
`ldrh r3, .Ln / .word 0`. Only a **register-allocated local** escapes:

```c
short zero = 0;  *p = zero;      /* mov r3, #0 / strh */
```

The `movhi` expander `force_const_mem`s a CONST_INT when the destination is
memory. That same root cause produced a second, apparently unrelated
difference: the `ldrh` needed its pool in range, so `arm_reorg` dumped the pool
mid-function and branched over it. **A stray jump-to-next-label in Thumb output
is usually a pool dump, not a jump-optimisation failure.**

## DELETE THE LOCAL — three parks, one lever

Three of the five unparked functions fell to the same thing, and each park had
recorded the opposite conclusion:

- `OvlFunc_931_2008d08` held `int k` because the ROM shared one `mov r2, #0x14`
  across two stores. Six spellings of that local — four types, both declaration
  orders — all measured **exactly 7**, and the notebook's own rule says
  identical counts across unrelated spellings indict the variable itself.
  **A register shared between two stores is not evidence of a source variable.**
- `OvlFunc_931_200807c` carried two locals holding only addresses.
- `OvlFunc_964_20094ac` carried the read-modify-write through a named value.

## Other mechanisms worth keeping

**`PROMOTE_MODE` makes every HImode local sign-extend.** `OvlFunc_960_2008ce4`'s
park noted that declaring the local unsigned "is not enough" and was right:
`arm.h` sets `UNSIGNEDP = TARGET_MMU_TRAPS != 0`, which is 0 here, so no type on
the halfword can produce `lsr`. The fix is on the **read** side — assign into an
`unsigned int` before the store.

**A dead four-byte stack local survives -O2 as a `char` array, not as a struct**
of four bytes, and `mov rX, sp` before a `strb` is the byte-granular
address-taken tell.

**When gcc reuses a compared-against-zero register as a stored constant and the
ROM does not, overwrite that variable before the stores.**

## A MISTAKE, AND WHAT CAUGHT IT

Converting `asm/rom_8a000/rom_92950_c_c_c_c.s` whole dropped a
`.section .rodata` block exporting `.L9ed80`, which `stage1.ld` pulls on its own
line and another translation unit references. The link failed with an undefined
reference — **caught by `make compare`, not by the screen**, which cannot see
data other objects need. The file is now hand-split, because `split_s.py` cuts
at *function* boundaries and would have kept trailing data with the function it
follows.

> Grep a `.s` for `.section`, `.global` and `.incrom` before converting it whole.

## Two tool bugs, both mine, both fixed

`tryc.py` gained a pool-entry guard in batch 180 after an agent found a
candidate that duplicated a pool word and screened **OK** — four bytes larger
than the ROM, and `make compare` would have failed. The existing `.text` size
check should have caught it and silently didn't: `text_size(reference)` returns
`None` whenever the reference will not assemble standalone, which is most
overlay references.

That guard then had a false positive of its own: a reference may spell a pool
word as a bare `.word` under a local label rather than `=value`, and counting
only the `=` form under-counts it. `Func_8011644` is exactly that shape and was
failed wrongly. Both fixed; the OK line now also *says* when the size check did
not run, because silence there reads as verification.

`realign.py` was blind to overlay parks — overlay `.thumb_func_start` lines
carry no address comment, so 33 of the first 60 scanned came back as "no asm",
which reads as *stale park*. Roughly two thousand of the parked set are overlay
functions and none could be re-ranked until it indexed the symbol name too.

## Parks

`OvlFunc_952_20083b0` is parked at **2 of 88** with its tie-break characterised
from sched2's ready lists rather than inferred: the `lsl` always has a lower
LUID because the constant is `force_reg`'d in `precompute_register_parameters`,
and it wins the tie only because an intervening *call* gives it a fourth
dependent. Confirmed by construction — swapping the intervening call for a
two-register-argument one reproduces the ROM exactly. Nineteen flags and around
twenty source spellings measured.

One park was **deleted as genuinely stale**: `OvlFunc_898_2008acc` was elevated
in `54498ab2` and its park was never removed.

## State

1,888 functions remain in `asm/`. Nine subagents contributed screens this batch;
every match was re-verified here by `make compare`, which remains the only
authority — and one of them turned out to depend on a tool bug rather than a
real match, which is exactly why.
