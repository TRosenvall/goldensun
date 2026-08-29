# Batch 48 — seven functions, and a candidate list that was lying

Verified from a clean build: `make clean && make compare` → `goldensun.gba: OK`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`
and sits at exactly the address its name claims.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `Func_80798b4` | `080798b4` | main ROM | [rom_79460_c_c_c_a_b.c](../src/rom_77000/rom_79460_c_c_c_a_b.c) |
| `Func_808e118` | `0808e118` | main ROM | [rom_8d9a4_a_c_a_a_a_b.c](../src/rom_8a000/rom_8d9a4_a_c_a_a_a_b.c) |
| `OvlFunc_920_2008148` | `02008148` | ovl_7a6ae4 | [ovl_30_c_a_c_a_c_c_b.c](../src/overlays/rom_7a6ae4/ovl_30_c_a_c_a_c_c_b.c) |
| `OvlFunc_920_2008168` | `02008168` | ovl_7a6ae4 | [ovl_30_c_a_c_a_c_c_c_b.c](../src/overlays/rom_7a6ae4/ovl_30_c_a_c_a_c_c_c_b.c) |
| `OvlFunc_950_200809c` | `0200809c` | ovl_7d5838 | [ovl_30_c_c_a_c_a_a_b.c](../src/overlays/rom_7d5838/ovl_30_c_c_a_c_a_a_b.c) |
| `OvlFunc_965_200a4b0` | `0200a4b0` | ovl_7ef4f4 | [ovl_30_a_c_c_c_c_c_c_b.c](../src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c_c_b.c) |
| `OvlFunc_973_20086f8` | `020086f8` | ovl_7fc720 | [ovl_30_c_a_c_c_c_a_b.c](../src/overlays/rom_7fc720/ovl_30_c_a_c_c_c_a_b.c) |

## The candidate list was offering code that was never C

Eight of the top sixteen candidates at `--min-calls 1 --max-insn 20` were `ply_*`
sound commands from `asm/rom_f9000`. They look ideal — eight instructions, one
call, a tiny shared helper — and **they cannot be elevated at all.**

They are the stock m4a / "Sappy" engine. Their prologue is

```
	mov	r12, lr
	bl	Func_80f9ab4
	...
	bx	r12
```

which gcc-2.96 does not emit for Thumb: it pushes `lr` and returns through a
popped register. That half of the engine is **assembly by origin**.

The Makefile already knew the rest of the story — `src/lib/m4a/` is built with
`old_agbcc` and `-D M4A_SIGNED_CHAR`, **not** Camelot's gcc296. `tryc.py` screens
with gcc296 flags, so a screen there answers the wrong question even for the half
that genuinely was C.

`tools/pick_candidates.py` now excludes `asm/rom_f9000` and any function using
the r12 convention. Fifteen functions tree-wide use it, all fifteen in m4a —
**small in absolute terms, dominant at the top of the list**, which is the only
place that matters.

## An add/sub chain on a constant may be gcc's OWN arithmetic

`OvlFunc_950_200809c` and its instruction-identical twin `OvlFunc_973_20086f8`
walk one register through the whole function:

```
mov r3,#0xe0 / lsl r3,#1 / add r2,r1,r3 / add r3,#0x41 / str r3,[r2]
sub r3,#0x39 / add r2,r1,r3 / mov r3,#0x18 / str r3,[r2]
```

Read as source that says: an offset `0x1c0`, a stored value built from it as
`0x1c0 + 0x41`, then the next offset as `0x201 - 0x39` — the offset-reuse lever
from batch 44. Written that way it is **10 of 15**.

The source is two plain stores:

```c
*(int *)(p + 0x1c0) = 0x201;
*(int *)(p + 0x1c8) = 0x18;
```

gcc had `0x1c0` in a register, needed `0x201`, and chose `add #0x41` over a
second `mov`/`lsl` pair. **The chain is the compiler's arithmetic on a constant
it already held, not the source's.**

### Telling this apart from the real reuse lever

The genuine lever (batch 44, `Func_809b648`) looks almost the same:

```
mov r3,#0x91 / lsl r3,#2 / add r2,r1,r3 / mov r3,#0 / str r3,[r2]
```

The difference is the **final `mov`**. There the register is *overwritten* with an
unrelated value before the store, and gcc has no reason to destroy an offset it
still wants. Where the register is instead *adjusted* into the next value by
`add`/`sub`, that is strength reduction and the source had literals.

**Try the literal form first** — it is shorter, and being wrong costs one screen.
Added to `docs/elevation.md` with both shapes side by side.

## Two namings, neither visible in the ROM

`Func_808e118` needed both, and **each fix alone leaves the other defect** — which
is why a single-lever screen looks like a dead end.

**The stored zero is a named `int`.** Written as the literal `0`, gcc-2.96 puts
the constant in a **literal pool**, loads it with `ldrh`, and plants the pool
mid-function with a `b` jumping over it — 20 instructions against 18, with the
ROM's plain `mov r2, #0` nowhere in sight. This is the *inverted*
narrow_constant tell: where gcc pools what the ROM builds with a `mov`, the
source had a variable.

**The destination is a named pointer.** With the address left inside the store
expression, gcc materialises the zero *before* computing the address; the ROM
does `add r1, r3, r2` and only then `mov r2, #0`.

Named zero alone: 6 of 18. Literal alone: 7 of 18 at the wrong length. Both: match.

## The declaration lever, used in the other direction

The three stack-arg-pair functions here have the callee **declared**, because the
ROM writes `r0` **first**. That is the mirror image of the `888`/`930` family in
batches 44–45, where `r0` comes last and the declaration is deliberately withheld.

Same shape, opposite treatment, and the ROM says which by the position of one
instruction.

## The wildcard-flag check fired before the split

`OvlFunc_965_200a4b0`'s new file would inherit `-O1` from
`ovl_30_a_c_c_c_c_c%`, a rule written for a neighbouring `.s` — the batch-45 trap,
third instance.

This time it was checked **prospectively**, before splitting, rather than after a
failed screen. The function is fourteen instructions of `mov` and `str` and
matches byte-for-byte at **both** `-O1` and `-O2`, so the inherited rule is
harmless and the Makefile was left alone rather than churned. Recorded in the
file so nobody re-opens it.

`Func_80798b4` needed its table declared as an **array of structs** to put it in
the load's base register; as byte-pointer arithmetic the index becomes the base
and the table's pool load lands two instructions late, 7 of 19.

## Parked

`OvlFunc_881_200811c` at **2 of 16**, same length — pure register allocation. The
zero a Thumb `ldrsh` needs for its offset lands in `r1` where the ROM uses `r4`.
gcc picks `r1` because the halfword is loaded into `r1` on the very next
instruction and the zero is dead by then; the ROM kept them apart. There is no
source-level handle — the zero has exactly one use and cannot be made to live
longer without adding an instruction. `-O1`, `-fno-schedule-insns2` and
`-fno-rerun-cse-after-loop` all give the identical two-line diff.

**A false lead is recorded as false.** `r4` is callee-saved and the ROM's prologue
is `push {lr}` alone, so this looked like a function clobbering a saved register
without saving it — which would have said something about the original build. It
says nothing: **826 of 2779** unelevated functions do the same. Checked before
writing it into the park note rather than after.

Two earlier mistakes are also recorded there so they are not re-tried: the
fall-through arm is the **increment**, not the delete; and the increment is on an
`int` — reading the halfword into an `unsigned short` makes gcc re-truncate with
`lsl #16 / lsr #16` before the add.
