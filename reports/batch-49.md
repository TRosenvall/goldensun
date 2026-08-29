# Batch 49 — nine functions from one enumerated shape

Verified from a clean build: `make clean && make compare` → `goldensun.gba: OK`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`
and sits at exactly the address its name claims.

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_901_2008e30` | `02008e30` | ovl_797990 | [ovl_314_c_c_a_c_a_a_b.c](../src/overlays/rom_797990/ovl_314_c_c_a_c_a_a_b.c) |
| `OvlFunc_901_2008e60` | `02008e60` | ovl_797990 | [ovl_314_c_c_a_c_a_a_c.c](../src/overlays/rom_797990/ovl_314_c_c_a_c_a_a_c.c) |
| `OvlFunc_920_2008188` | `02008188` | ovl_7a6ae4 | [ovl_30_c_a_c_a_c_c_c_c_b.c](../src/overlays/rom_7a6ae4/ovl_30_c_a_c_a_c_c_c_c_b.c) |
| `OvlFunc_920_20081bc` | `020081bc` | ovl_7a6ae4 | [ovl_30_c_a_c_a_c_c_c_c_c.c](../src/overlays/rom_7a6ae4/ovl_30_c_a_c_a_c_c_c_c_c.c) |
| `OvlFunc_943_200b9b8` | `0200b9b8` | ovl_7c7b9c | [ovl_30_c_a_a_c_c_b.c](../src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_c_b.c) |
| `OvlFunc_948_2009c6c` | `02009c6c` | ovl_7d30e0 | [ovl_30_c_c_a_c_a_a.c](../src/overlays/rom_7d30e0/ovl_30_c_c_a_c_a_a.c) |
| `OvlFunc_959_200a26c` | `0200a26c` | ovl_7e7574 | [ovl_9dc_…_c_b.c](../src/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_c_c_c_c_c_b.c) |
| `OvlFunc_959_200a2a0` | `0200a2a0` | ovl_7e7574 | [ovl_9dc_…_c_c_b.c](../src/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_c_c_c_c_c_c_b.c) |
| `OvlFunc_959_200a2d4` | `0200a2d4` | ovl_7e7574 | [ovl_9dc_…_c_c_c_b.c](../src/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_c_c_c_c_c_c_c_b.c) |

**Nine for nine, every one on the first screened attempt.**

## Found by enumerating the shape, not by picking candidates

Rather than running `pick_candidates.py` and reading down the list, this batch
came from asking the corpus a single question: *which functions consist only of
immediate `mov`s, stack stores, and calls?*

```
sub sp, #N present; no ldr; no .L labels;
opcodes ⊆ {push, pop, sub, add, mov, str, bl, bx, lsl, neg}
```

**Fifteen functions tree-wide.** Two were already parked; nine of the remaining
thirteen are elevated here across two rounds.

That is a different way to work than the ranked candidate list, and it was
markedly better for this shape: the list ranks by tractability and offers a
mixed bag, while enumerating a shape you have *already solved* hands you a
worklist where the answer is known before you start.

## A value in a callee-saved register is shared across both calls

The batch's main finding. `OvlFunc_959_200a26c` and six others push `r5` in the
prologue and store it into the stack-argument slot before **each** of two calls:

```
push {r5, lr}
mov  r3, #0x51 / str r3, [sp, #4]
mov  r5, #0x15                       <- callee-saved
...  str r5, [sp] / bl __Func_80105d4
mov  r3, #0x22 / str r3, [sp, #4]
...  str r5, [sp] / bl __Func_8010704
```

**gcc does not spend a push unless the value has to survive the call.** So `r5`
is one local passed as the fifth argument of both calls, and the per-call value
is a second local reassigned between them:

```c
m = 0x15;
n = 0x51;  __Func_80105d4(2, 0x52, 1, 2, m, n);
n = 0x22;  __Func_8010704(0x15, 0x20, 1, 1, m, n);
```

That settles the **store order** for free. Where the shared value goes to `[sp]`
last, immediately before the `bl`, and the per-call value to `[sp, #4]` early, no
reordering of the C is needed — it falls out of which local is shared.

### Which slot is shared is read off the `str r5` offset

Three of these put `r5` in `[sp]` and the per-call value in `[sp, #4]`.
`OvlFunc_948_2009c6c` is **the other way round**. The two orders look identical in
the argument block, and the only thing that distinguishes them is the offset on
that one store.

## Do NOT reuse an earlier pair's local for a later shared value

`OvlFunc_901_2008e30`'s second call stores one register into *both* slots
(`mov r3,#3 / str r3,[sp] / str r3,[sp,#4]`). Three spellings were screened:

| Spelling | Result |
|---|---|
| `n = 3;` … `(…, n, n)` — recycling the first pair's local | **3 of 22** |
| a fresh local `k = 3;` … `(…, k, k)` | match |
| bare literals `(…, 3, 3)` | match |

The failure is worth recording because **the diff lands three instructions
before the statement that caused it** — in the *first* pair's register
assignment, which the second call has no business touching. Nobody would look
there.

Literals went in, being the shorter of the two that match, and consistent with
batch 48's "try the literal form first".

## Skipped without screening

`OvlFunc_948_20091d8` is the same family but puts `r0` inside a `mov`/`lsl`
construction:

```
mov r1,#0x80 / mov r2,#0x80 / mov r0,#0xc / lsl r1,#12 / lsl r2,#12
```

in straight-line code, with no branch for the basic-block lever to use. That is
the unreachable side of the `local-alloc.c` mechanism established in batch 42 —
**the fifth time the catalogue has stopped work before it started** rather than
explaining a failure afterwards.

## Process

The prospective wildcard-flag check now runs on every destination path *before*
splitting, not after a puzzling screen. Nine paths this batch, all clean, no
Makefile interaction. Batch 48 was the round where that check first paid for
itself; it is routine now.

`tools/asmfacts.py --asm-pairs` confirms all 2,895 tracked sources still have
their sibling `.s` tracked.
