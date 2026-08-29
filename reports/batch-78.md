# Batch 78 — three twin pairs, and a split that dropped its data

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, and every
overlay `cmp` clean. Every address below was read back out of the linked ELF.
0 orphaned linker references.

| Function | Address | File |
|---|---|---|
| `OvlFunc_936_20080ac` | `020080ac` | [ovl_30_a_c_a_b.c](../src/overlays/rom_7c097c/ovl_30_a_c_a_b.c) |
| `OvlFunc_969_2008424` | `02008424` | [ovl_314_a_a_c.c](../src/overlays/rom_7f6e64/ovl_314_a_a_c.c) |
| `OvlFunc_922_2009004` | `02009004` | [ovl_30_c_a_c_c_c_c_a_a_a_a.c](../src/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_c_a_a_a_a.c) |
| `OvlFunc_934_2009938` | `02009938` | [ovl_169c_a_c_c_a.c](../src/overlays/rom_7bdeb0/ovl_169c_a_c_c_a.c) |
| `OvlFunc_899_200c698` | `0200c698` | [ovl_30_c_c_c_c_c_c_a_a.c](../src/overlays/rom_794ac0/ovl_30_c_c_c_c_c_c_a_a.c) |
| `OvlFunc_902_2008570` | `02008570` | [ovl_30_c_c_c_b.c](../src/overlays/rom_7987ac/ovl_30_c_c_c_b.c) |

Six functions from **three** solves. Every one of the three had a byte-identical
twin, and every twin matched on the first screen with only the symbol changed.

## The twin scan is now part of the solve, not a separate hunt

Batch 77 found two free twins by hashing function bodies by hand after a solve.
This batch used [tools/find_twins.py](../tools/find_twins.py) properly: it ranks
duplicate groups by payoff and marks which members are already parked, so
picking small unparked pairs is a two-line decision.

A note now sits at the top of that tool saying to run it **after every solve**,
not only when hunting clusters. The evidence for the habit is four of batch 77's
five and all six here.

## One member, read signed and unsigned, keeping both loads

`OvlFunc_936_20080ac` is the interesting solve. It reads the same struct member
two ways and the ROM keeps **both** loads:

```
ldrsh r0, [r4, #0x66]     ... used where the sign matters
ldrh  r1, [r4, #0x66]     ... used where it is truncated anyway
```

Batch 76 recorded that two reads of one member fold into one. That rule needed a
qualifier: it holds when they are the *same* read. `a->f66` and
`(unsigned short)a->f66` produce different values for a negative field, so CSE
cannot unify them, and gcc emits both.

Three levers already on the books then finished it, applied in order:

- **The order of the two reads decides which register holds the zero index.**
  The ROM's is born second, so the unsigned read has to be written first.
- **The countdown is an `int` local.** As `unsigned int`, gcc knows it fits in
  sixteen bits and turns `u - 1` into a masked add.
- **Both exits converge on one `return 1` through a `goto`.** An early return
  emits `mov r0, #1` twice.

## Mask width picked the spelling twice in one function

`OvlFunc_899_200c698` matched on the first screen once both of its masks were
read by width — the batch-71 rule doing its whole job inside one body:

| ROM | Width | Spelling |
|---|---|---|
| `mov r3, #0x21 / neg r3, r3` | 32-bit | bitfield: `s->f5_b5 = 0;` |
| `mov r3, #0xf` | byte | hand-written: `s->f9 &= 0xf;` |

Read the width off the ROM and the spelling follows. No search was needed.

## A function can still just work

`OvlFunc_922_2009004` matched on the first screen with no levers at all — the
three-argument save into r6/r7/r8, the two-call structure and the walked pointer
all fall out of ordinary struct members written the obvious way.

Worth recording, because the reports skew toward the ones that fought back.

## The split mistake: "carries data" is not advice

[tools/split_asm.py](../tools/split_asm.py) printed, for
`asm/overlays/rom_7987ac/ovl_30_c_c_c.s`:

```
carries data      : YES -- keep a .s and its section line
```

I did not act on it, and the build broke in two ways at once:

```
ORPHAN asm/overlays/rom_7987ac/ovl_30_c_c_c.o(.data) -- .s deleted, section still wanted
undefined reference to `gOvl_020086dc'   (and three more)
```

The cause is a bug in the inline cut helper, not in the tool. **It wrote a tail
piece only when another FUNCTION followed the cut.** Here the cut function was
the last one and only a `.data` blob followed, so the tail was silently dropped.

That produced a second failure on top of the first. With no tail piece, the
helper named the elevated C `_b.c` — and gcc writes its assembly to
`asm/.../_b.s`, **overwriting the split piece of that name**. This is the
basename-collision trap from [batch-61](batch-61.md), reached by a new road: the
collision was not with the original `.s` but with a *sibling piece* the helper
had already emitted.

The fix is one line of judgement: **write the tail piece whenever anything at all
follows the cut**, function or not, and point the `.data` line at it. Recovery
was `git restore` on the original, a clean re-cut into `_a` / `_c`, and a third
`.text` line in the overlay script.

Two lessons, both cheap:

- `split_asm.py`'s "carries data" line is a *requirement*, not a remark.
- `asmfacts.py --orphans` named both faults in one run. It has now caught a
  wiring slip in three of the last four batches.

## One park, and what `-fno-strict-aliasing` actually buys

`Func_80b86ec` sits at 47 lines against 44 —
[src/non_matching/rom_b5000/80b86ec.c](../src/non_matching/rom_b5000/80b86ec.c).

The park is worth reading for the half that the flag *solves*. The ROM re-reads
`gKeyHeld` for its second button test:

```
ldr r3, [r0]  ...  strh r3, [r1, #0x36]  ...  ldr r3, [r0]
```

At `-O2` gcc reads it once, because strict aliasing says an `unsigned short`
store cannot touch an `unsigned int` global. With `-fno-strict-aliasing` both
reads come back and the first six instructions match exactly — which is the
evidence that the *reading* is right and this TU would want the flag, alongside
the ones batch 69 gave it to.

What is left is a different class. The constant `0x200` is both the test mask
and the increment, and the ROM keeps it live in r2 across both uses:

```
mov r2, #0x80 / lsl r2, #2  ...  and r3, r2  ...  add r3, r2
```

gcc builds it, uses it, and **builds it again** — the three extra instructions. A
named `int` used in both places does not stop it, and neither does transposing
the AND's operands: rematerialising a two-instruction constant is cheaper than
holding a register live, and gcc prices it that way. That is the same pricing
decision behind the narrow-constant family, seen from a new angle.

One incidental finding recorded in the park: the second global is reached as
`iwram_3001e80[0x20]`, not as its own symbol. The ROM's
`ldr r3, =iwram_3001e80 / add r3, #0x80` is one symbol plus an offset too large
for `ldr`'s immediate field — an array index reproduces it exactly, and inventing
a symbol for it would have been wrong.
