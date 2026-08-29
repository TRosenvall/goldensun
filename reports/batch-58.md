# Batch 58 — six functions, and a number that reframes the parked set

Verified from a clean build: `make clean && make compare` → `goldensun.gba: OK`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_939_200918c` | `0200918c` | ovl_7c460c | [ovl_314_c_a_c_b.c](../src/overlays/rom_7c460c/ovl_314_c_a_c_b.c) |
| `OvlFunc_948_2009bc4` | `02009bc4` | ovl_7d30e0 | [ovl_30_c_c_a_a_c_c_a_c_c_b.c](../src/overlays/rom_7d30e0/ovl_30_c_c_a_a_c_c_a_c_c_b.c) |
| `OvlFunc_959_2008c90` | `02008c90` | ovl_7e7574 | [ovl_9dc_a_c_c_a_a_a_b.c](../src/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_a_b.c) |
| `OvlFunc_959_2008e30` | `02008e30` | ovl_7e7574 | [ovl_9dc_a_c_c_a_a_c_b.c](../src/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_c_b.c) |
| `OvlFunc_959_2008ee0` | `02008ee0` | ovl_7e7574 | [ovl_9dc_a_c_c_a_a_c_c_b.c](../src/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_c_c_b.c) |
| `OvlFunc_968_2008058` | `02008058` | ovl_7f2f14 | [ovl_30_a_a_a_c_a_b.c](../src/overlays/rom_7f2f14/ovl_30_a_a_a_c_a_b.c) |

## Sixty parks are within six instructions of matching

Measured with the new `tools/near_parks.py`, which screens every park and counts
instructions in **disagreeing regions** rather than positions.

That number separates two populations the word "parked" has been hiding:

- a park at **2 of 24** is a compiler difference nobody has cracked
- a park at **30 of 34** is a function whose C is probably wrong

Only the second is worth re-reading from scratch, and until now nothing in the
tree distinguished them.

**Close does not mean reachable.** Many of the sixty sit on named, settled
classes where the residual is a *floor*: the signed lower-bound canonicalisation
(batch 55), the `include/dma.h` register binding (batches 54–55), the pre-header
load merge, multiply operand canonicalisation. Each park note says which.

What the number *does* say is that the remaining difficulty is concentrated in a
handful of compiler behaviours rather than spread across the corpus. Cracking any
one of them at the compiler level would take a large block with it.

### The tool's first advice was wrong, and following it caught that

It initially flagged one park as *"SCREENS CLEAN NOW — re-check those first"*.
That is a trap: **`tryc` normalises literal-pool loads**, so a function whose
only defect is *pool placement* screens OK and still fails `make compare`.
`src/non_matching/ovl_7ec19c/200816c.c` is exactly that — already through two
split-and-revert cycles, and it says so in its own note.

The tool now says a clean screen is not an unpark signal on its own, and points
at that file.

## Enumerating a shape beats reading the ranked list

The two rounds before the last one yielded **one function each**. Both took
candidates off `pick_candidates.py` and kept landing on register-allocation
residuals.

This batch's last round enumerated a *shape* instead — functions of 24–42
instructions whose calls are **all** to simple helpers and which touch no struct
fields — found twelve, and put four in immediately, all first attempt.

That is batch 49's method applied to a filter rather than a body: **rank by
tractability and you get a mixed bag; describe what you can already do and you
get a worklist.** At this size the ranked list has stopped being the better one.

## A table of pairs read with an advancing offset

Three of the four came from one shape:

```c
off = i << 3;
a = *(int *)(t + off);
off += 4;
b = *(int *)(t + off);
```

reproducing `lsl r0,#3 / ldr r6,[r3,r0] / add r0,#4 / ldr r5,[r3,r0]`. **The table
stays the load's base and the offset walks** — a two-int struct array folds the
base in instead.

Their third call derives *both* varying values from the second table entry by
subtraction, and the ROM makes the `[sp, #4]` copy **destructive** on it while the
register argument gets its own. Two different subtractions of one value, so two
expressions rather than one shared local.

`OvlFunc_959_2008ee0`'s third call goes to a **different callee** than its two
twins — read off the `bl`, not assumed from the family.

## Arguments shuffled on the way in

`OvlFunc_968_2008058` saves `r0`–`r2`, then calls `__CreateActor` with
`(r3, r0, r1, r2)` — the fourth parameter becomes the first argument and the rest
slide down. Read off the four `mov`s before the `bl`; nothing else reveals it.

Its `return 0` block sits **after** the main body, so the null check is a `goto`
to a label at the end rather than an early return — the block *order* is what the
branch polarity reports, the same reading as `GetSpriteVoice` in batch 56.

## Parked this batch

- **`OvlFunc_959_2008dcc`**, 6 of 38 — a **new sub-shape of constant-CSE**: the
  same constant as *two arguments of one call*. The documented shape is a
  constant on both sides of a call, which `-fno-rerun-cse-after-loop` fixes; this
  one it makes **worse**, and separate named locals do nothing.
- **`OvlFunc_968_2009644`**, 4 of 39 — the basic-block lever tried *after*
  screening the plain form (per batch 57) and doing nothing. The lever moves a
  constant gcc refuses to rematerialise; here gcc already does, and the ROM
  merely interleaves it with unrelated stores.
- **`Anim_Attack`**, 5 of 39 — pure register exchange on an identical stream.
- **`OvlFunc_941_2008094`**, 9 of 30 and one instruction *short* — gcc chains the
  two field addresses (`0x23 + 0x32 = 0x55`) where the ROM builds both from the
  base. Batch 55's "ours is shorter" signature in its **cross-jumping** form,
  which batch 56 established is the unfixable kind.
- **`OvlFunc_965_200a46c`**, 2 of 30 — purely **prologue order**. Second function
  with this exact residual, so it is named as a shape rather than two unconnected
  one-offs.
