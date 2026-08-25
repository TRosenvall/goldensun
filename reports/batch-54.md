# Batch 54 — six main-ROM functions, and a DMA vein opened

Verified from a clean build: `make clean && make compare` → `goldensun.gba: OK`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`.
The two named functions were checked against the addresses in their `.s`
comments as well, since their names do not carry the address.

| Function | Address | File |
|---|---|---|
| `DeleteSpriteLayer` | `0800bc48` | [rom_b798_c_c_b.c](../src/rom_9000/rom_b798_c_c_b.c) |
| `Func_8021360` | `08021360` | [rom_20198_c_c_c_a_a_a_b.c](../src/rom_15000/rom_20198_c_c_c_a_a_a_b.c) |
| `Func_80251d4` | `080251d4` | [rom_23178_a_a_a_a_b.c](../src/rom_15000/rom_23178_a_a_a_a_b.c) |
| `AllocGlobal1F` | `0808fecc` | [rom_8d9a4_c_c_a_c_b.c](../src/rom_8a000/rom_8d9a4_c_c_a_c_b.c) |
| `Func_80958a8` | `080958a8` | [rom_944ec_a_c_a_a_a_b.c](../src/rom_8a000/rom_944ec_a_c_a_a_a_b.c) |
| `Func_809bb34` | `0809bb34` | [rom_9b698_c_c_b.c](../src/rom_8a000/rom_9b698_c_c_b.c) |

**All six are main ROM** — the first batch where none are overlay code.

## A negative answer opened the vein

`Func_809bb34` is the **first use of `DMA3_CLEAR` in this tree**. `include/dma.h`
has carried that helper since the tree was adopted and nothing had ever used it.

What identified it was `tools/find_solved_shape.py` (batch 53) reporting **no**
elevated `.c` producing a `0x85000...` count word. That said: this is the unused
member of a family you already have, not a shape needing a new construct.

Searching the unelevated corpus for `stmia` then found **264 functions**. The
small end of that list is mostly allocate-and-zero or copy-a-tile wrappers that
the `dma.h` helpers already cover — three of this batch's six are exactly that.

**No new inline assembly was added.** Every one of these is another user of what
was already there.

## Two findings worth the notes

### `Func_80251d4`: the ROM's `and` order is the reverse of the source's

It needs the mask as a **named local**, with the two maskings in the order
**a then b**. The ROM does `and r1, r0` — masking the *second* argument — before
`and r0, r3`, so the obvious reading is b-then-a. That reading is **4 of 15**,
with the mask landing in the wrong register.

And without the named mask at all, the function is **one instruction shorter
than the ROM** (14 against 15): gcc keeps the argument in `r0` and loads the mask
into `r3`, where the ROM spends a `mov r3, r0` to free `r0` for the mask. Naming
it is what buys the extra instruction back.

Two lessons layered: a *shorter* result is still a failure, and the order
operations appear in the assembly is not the order they appear in the source.

### `Func_80958a8`: the subtractive declaration lever, on a pool load

`StartTask` is deliberately left **undeclared**. Declared, gcc emits its
`ldr r0, =Func_8095884` between `mov r1, #0xc8` and that register's `lsl r1, #4`;
the ROM emits it *after* the shift. 2 of 21 with the prototype, exact without it.

## Also

- **`Func_8021360`** was one instruction out on branch polarity. The ROM's `bne`
  sends the flag-*set* case to the second table and falls through to the first,
  so the source reads `if (!flag) return first; return second;`.
- A fourteenth `.global` was added to split `DeleteSpriteLayer` out, verified
  byte-neutral before the split.

## Parked: a property of `dma.h`, not of the functions

`OvlFunc_914_2008c0c` and **three instruction-identical twins** (one per overlay)
sit at 21 lines against the ROM's 22. The ROM loads `&REG_DMA3SAD` **twice, once
in each arm**, and performs the `stmia` **once** after the join — a tail merge
that kept the pool load duplicated.

`DMA3_COPY` binds its base register *inside* the inline function, so the number
of `ldr r3, =` equals the number of `DMA3_COPY` calls:

| Form | Result |
|---|---|
| one call after the `if`, selecting only the source | 21 lines, one `ldr r3, =` |
| a call in **each** arm | 24 lines, whole `stmia` block duplicated |

gcc-2.96 does not produce the partial tail merge that would give 22, and nothing
in the C chooses how many times a `register … __asm__("r3")` binding is
materialised.

**The fix belongs in the helper, not the function** — a `DMA3_COPY` variant
taking the base as a parameter, or a formulation where the source selection
happens after the base is bound. Worth doing once for four functions, and worth
checking against the other 264 `stmia` users before committing to a shape.

## The other park from this round

`OvlFunc_961_2008120` and its twin sit at **2 of 48** — a straight-line
`mov`/`neg` interleave, unreachable per batch 42 since the function has no
branches. Its note records **three things that are right** so a future attempt
does not undo them: the table pointer must be a named local (bare, the `ldrsh`
come out with the offset as base); the offset must be a variable that advances
(`off = idx << 2; … off += 2;`, where a two-short struct array is 10 of 48); and
the entries are `short` read into `unsigned short` (the ROM's `ldrsh` plus
`lsl #16 / lsr #16` is a signed load zero-extended, not a `u16` field).

Each of those cost a screen. A park that only said "2 of 48, interleave" would
have thrown all three away.
