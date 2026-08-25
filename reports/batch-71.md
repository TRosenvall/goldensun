# Batch 71 — the highest-value blocker was a bitfield, and 16 parks were describing solved problems

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, and every
overlay `cmp` clean. Every address below was read back out of the linked ELF.
0 orphaned linker references.

| Function | Address | File |
|---|---|---|
| `Func_800c548` | `0800c548` | [rom_c004_c_a_c_a.c](../src/rom_9000/rom_c004_c_a_c_a.c) |
| `Func_800c570` | `0800c570` | [rom_c004_c_a_c_a.c](../src/rom_9000/rom_c004_c_a_c_a.c) |
| `OvlFunc_957_200b610` | `0200b610` | [ovl_30_c_c_c_c_a.c](../src/overlays/rom_7e3e08/ovl_30_c_c_c_c_a.c) |
| `OvlFunc_927_2008a4c` | `02008a4c` | [ovl_30_a_a_c_c_c_c_c_a.c](../src/overlays/rom_7b4558/ovl_30_a_a_c_c_c_c_c_a.c) |
| `OvlFunc_946_2008a4c` | `02008a4c` | [ovl_30_a_a_c_c_c_c_a.c](../src/overlays/rom_7ced6c/ovl_30_a_a_c_c_c_c_a.c) |
| `OvlFunc_964_2008a4c` | `02008a4c` | [ovl_30_a_a_a_c_c_c_c_c_a.c](../src/overlays/rom_7ed0a0/ovl_30_a_a_a_c_c_c_c_c_a.c) |
| `OvlFunc_965_2008a4c` | `02008a4c` | [ovl_30_a_a_a_c_c_c_c_c_a.c](../src/overlays/rom_7ef4f4/ovl_30_a_a_a_c_c_c_c_c_a.c) |

## The 34-function blocker

`docs/elevation.md` called narrow constant materialisation **"the single
highest-value problem in the project"**:

```
rom    mov r3, #0xd / neg r3, r3      (~0xc built at 32-bit width)
ours   mov r3, #0xf3                  (~0xc narrowed to a byte)
```

**It is a bitfield.** Declared as `unsigned char lo : 2, sel : 2, hi : 4` and
assigned directly, gcc's `store_bit_field` builds the mask, the shift and the
merge itself, at int width. First screen on `Func_800c548`, `Func_800c570` and
`OvlFunc_957_200b610`, on top of batch 70's five copies.

### Why it took so long is the part worth keeping

The old reading was that gcc narrows because it has **proved** the loaded value
is 0..255, so the job was to make the width unknown. Everything tried followed
from that reading and failed: the value in `s32`, `u32` and `u8` locals; the
mask as `0xf3`, `~0xc`, `-13`, `0xfffffff3` and `~(3 << 2)`; a named-constant
mask; an explicit `(s32)` cast; operands both ways round; eleven statement
orders; a shared-constant data dependency; and seven flags.

**A bitfield was among the six width-hiding tricks probed, and was written
off** — because it was tried as a way to hide the *field's* width while the
merge stayed hand-written. That is not what makes it work. The width was never
the thing to fix; writing the merge by hand was. When every spelling of an
expression fails, the question is whether the expression should be there at all.

### And it is not a blanket rule — the mask WIDTH picks the spelling

`OvlFunc_927_2008a4c` writes two masked fields and needs opposite spellings:

```
sprite +9    mov r3, #0xd / neg r3, r3     32-bit mask  -> BITFIELD
actor +0x23  mov r3, #0xfe                 byte mask    -> HAND-WRITTEN
```

`store_bit_field` always works at int width, so a bitfield always produces the
`mov / neg` pair; a bare byte mask is what hand-masking an `unsigned char`
gives. Declaring the second as a bitfield costs one instruction too many.
Read the width off the ROM before choosing.

## An argument permutation is not the argument-precompute blocker

`__CreateActor` is called with this function's own arguments rotated to
`(d, a, b, c)`, and gcc reproduces the ROM's **seven-move shuffle** through
r4/r5/r6 exactly, with no help at all. The argument-precompute class
(`calls.c:805`) is about a call whose arguments mix cheap constants with two or
more expensive values — not about permutation.

Recorded because I nearly skipped the function on the shuffle alone, and it is
four elevations.

## Sixteen parks were describing solved problems

New tool: [tools/stale_parks.py](../tools/stale_parks.py). For every `.c` under
`src/non_matching/`, it pulls the ROM symbol names out of the comment header and
checks each against the `.thumb_func_start` names still present in `asm/`. A
name that is gone has been elevated.

The first run found **sixteen fully-stale files** — every function they describe
already matched. Park count 180 → 164. Among them was
`overlays/narrow_constant.c`, describing the blocker above for functions that
were by then elevated.

**This matters more than tidiness.** The park census in
[batch-68](batch-68.md) counted 177 files by blocker class and its totals were
used to decide what to work on next. Any census taken before this was counting
solved problems.

**27 more are PARTLY stale** — they name a group, some of which is now elevated,
and their notes mix solved and unsolved work. They need editing rather than
deleting, and that has *not* been done; it is recorded in `HANDOFF.md` as a
standing item.

The tool earned itself again in the same session: it caught
`ovl_7ced6c/2008a4c.c` going stale in the round that elevated its four
functions.

## Two flag facts read off the ROM rather than guessed

Both are visible in the ROM's own bytes.

**`pop {r4, r5, r6, pc}` means the TU was built without `-mthumb-interwork`.**
With interwork gcc emits `pop {r4, r5, r6} / pop {r0} / bx r0`. The Makefile's
`COMMON2_CFLAGS` rule already drops interwork for `common2_c%` — but
`common2_a` needs it too, and the wildcard does not reach it.

**A prologue that pushes r4 and keeps a value in it across a call means the TU
was built without `-fcall-used-r4`.** That flag is in `GCC296_CFLAGS`, so gcc
cannot keep anything in r4 across a call; it reaches past to r8 and spends four
instructions saving and restoring it.

Together they took `OvlFunc_common2_28c` from 33 differing lines to **4**.

### Both sweeps, and what they bound

All 164 parks were re-screened, first with `-fcall-saved-r4` alone and then with
both flags together:

| | improves | matches |
|---|---|---|
| `-fcall-saved-r4` | 8 | **0** |
| `+ `-mno-thumb-interwork`` | 6 | **0** |

Improvements are large in places — 36→21, 33→15, 22→16 — so the r4 question is
real and wider than one file. But **neither is a key anywhere else**, and the
tempting move from here is a global change that the measurement says not to
make. Recorded so it is not re-run.

## Two parks

**`OvlFunc_common2_28c`** — 4 of 27 with both flags applied. What is left is
that gcc rewrites the offset-zero store of each pair sp-relative
(`str r0, [sp, #8]`) where the ROM goes through the address register it has
already materialised (`str r0, [r4, #0]`); the *second* store of each pair goes
through the register in both. The stack layout, the three `add rN, sp, #imm`
and the argument order are all settled and kept in the parked source. It also
invalidates that park's earlier note that named pointer locals "made it worse" —
they did, but only because `-fcall-used-r4` was forcing the r8 spill.

**`Field_Move`** — 2 of 25, everything exact except where the prologue's
`sub sp, #0xc` lands. Its twelve bytes of frame are real and **never used**:
nothing is stored, nothing takes the address. An unused `char buf[12]`
reproduces it, gcc-2.96 not removing declared local arrays at `-O2`.
The informative negative: `--no-sched2` does *not* put the `sub sp` back where
the ROM has it, it breaks something else — so the ROM's stream is not gcc's
unscheduled stream either.
