# Batch 94 — a lever that isn't a rule, and a non-signal worth naming

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `Func_808f0d8` | `0808f0d8` | main ROM | [rom_8d9a4_c_a_c_c_c_c_c_a.c](../src/rom_8a000/rom_8d9a4_c_a_c_c_c_c_c_a.c) |
| `Func_808f140` | `0808f140` | main ROM | same file |
| `Func_808f28c` | `0808f28c` | main ROM | [rom_8d9a4_c_a_c_c_c_c_c_c.c](../src/rom_8a000/rom_8d9a4_c_a_c_c_c_c_c_c.c) |
| `OvlFunc_948_20099e8` | `020099e8` | ovl_7d30e0 | [ovl_30_c_c_a_a_c_a.c](../src/overlays/rom_7d30e0/ovl_30_c_c_a_a_c_a.c) |
| `OvlFunc_897_200a8dc` | `0200a8dc` | ovl_791794 | [ovl_30_c_c_a_c_a_c_b.c](../src/overlays/rom_791794/ovl_30_c_c_a_c_a_c_b.c) |

Nothing was parked; three existing parks moved.

## Batch 93's prototype table was too crisp, and this batch broke it

Batch 93 wrote the prototype lever as a rule you could read off the ROM: r0
later than yours means delete the callee's declaration, earlier means add one.
`OvlFunc_961_2008120` has the ROM putting r0 **earlier**, so the table says add
a declaration — and it already had one. **Deleting** it is what improved the
function, three differing positions to two.

So the lever is real and nothing else reaches it, but its direction is not
predictable from where r0 sits. `docs/elevation.md` now says to try both and
measure, which is two compiles, instead of consulting a table that is wrong at
least a quarter of the time. Getting that wrong would have cost the next person
more than the table saved.

Three spellings of the negative argument that remains there — `int n = -0x10`,
`0 - 0x10`, decimal `-16` — are byte-identical at 2 of 48.

## The gState offset must be built, not folded

All three main-ROM functions needed one lever and only that lever:

```
	rom	ldr r3, =gState / mov r2, #0xfa / lsl r2, #1 / add r3, r2 / ldr r0, [r3]
	ours	ldr r3, =gState+500 / ldr r0, [r3]
```

Writing `*(int *)(gState + 0x1f4)` lets gcc fold symbol and offset into one pool
entry. Assigning `gState` to a local `unsigned char *` first blocks the fold —
the local holds the address as a value, so the `+ 0x1f4` has to be real
arithmetic. `Func_808f0d8` went from 39 differing positions of 44 to an exact
match on that alone, and `Func_808f140` and `Func_808f28c` then matched on their
first screen.

The sharpening over the earlier statement of this: it is needed **even when the
base is used once**. The rule is about the fold, not about reuse. And the tell
is in the pool — 0x1f4 is reachable as `mov` + `lsl` (0xfa << 1), 500 is not
reachable by `mov` at all, so the folded form has no choice but to pool it. A
`=symbol+N` pool entry where the ROM has arithmetic means a local base pointer
is missing.

## A value in a callee-saved register is not evidence the source named it

This has cost a spelling in three consecutive batches, so it is now written into
`docs/elevation.md` as a standing non-signal.

`OvlFunc_948_20099e8` sets `mov r5, #0x2a` once before four calls and does
`str r5, [sp, #4]` at each of them, pushing `{r5, lr}`. Everything about that
says a named local survives across the calls. Passing 0x2a as a plain literal at
all four sites compiles to the **same forty-three instructions** — gcc hoists the
repeated constant into r5 by itself. And batch 93's `OvlFunc_964_200a52c` failed
the same way from the other side: written as locals, gcc hoists them *above* the
first call and costs three instructions.

What is genuinely forced, and what this keeps getting confused with, is **two
different constants in the two stack slots**:

```
	rom	mov r3, #9 / mov r2, #0x26 / str r3, [sp] / str r2, [sp, #4]
	ours	mov r3, #9 / str r3, [sp] / mov r3, #0x26 / str r3, [sp, #4]
```

There, naming both is what stops gcc walking one register through both stores.
The lever is about two distinct values needing two distinct registers, not about
a value living a long time. `OvlFunc_897_200a8dc` shows the same distinction
within one function: its shared 2 is assigned inside each arm because hoisting it
above the `if` does diverge (43 against 44, 25 differing) — that one was checked.

## The mask-width rule, and what is not part of it

`Func_808f28c` builds `~0xc` as `mov r3, #0xd / neg r3, r3` and then does
`and r3, r2`, mask in the destination. Two things looked like levers; only one
is.

**Forced:** the mask must be a named `int`. Inline, `(p->f9 & ~0xc) | 4` narrows
to byte width and gives 49 instructions against 50 with 12 differing.

**Not forced:** which side of the `&` the mask is written on. `mask & p->f9` and
`p->f9 & mask` are the same fifty instructions. gcc picks the destination
register itself — the same conclusion `src/non_matching/ovl_7ed0a0/2009458.c`
reached over several batches of trying.

Worth contrasting with `CreateParticleActor` from batch 92, which wants the same
`~0xc` at the same width but reaches it as `sub r3, #0x11` from a 4 already in
the register. Same C, different surroundings.

## Park work

Beyond `OvlFunc_961_2008120` above, `OvlFunc_964_2009458` gained two measured
negatives. Making the mask the accumulator — `v = 0xf7; v &= t;`, which is the
obvious way to tie the `and`'s destination to the mask the way the ROM does —
gives 7 of 36 against the park's existing 3. Declaration order of the two locals
was permuted and both were tried as `unsigned char`; all are byte-identical
except `unsigned char v`, which costs two instructions.

That park's own conclusion — "nothing at the expression level; this is the
allocator choosing r2/r3 the other way round" — survives another round of
attempts, and now says so with the attempts listed.
