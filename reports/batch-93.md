# Batch 93 — the prototype lever runs both ways, and 52 parks are within six instructions

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of the linked overlay ELF with
`arm-none-eabi-nm`.

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_964_200a3a0` | `0200a3a0` | ovl_7ed0a0 | [ovl_30_c_c_c_c_c_a.c](../src/overlays/rom_7ed0a0/ovl_30_c_c_c_c_c_a.c) |
| `OvlFunc_964_200a410` | `0200a410` | ovl_7ed0a0 | same file |
| `OvlFunc_964_200a480` | `0200a480` | ovl_7ed0a0 | same file |
| `OvlFunc_964_200a52c` | `0200a52c` | ovl_7ed0a0 | same file |
| `OvlFunc_944_2008a84` | `02008a84` | ovl_7ca63c | [ovl_30_c_c_a_c_c_b.c](../src/overlays/rom_7ca63c/ovl_30_c_c_a_c_c_b.c) |

One function parked, and two existing parks moved.

## The prototype lever, in both directions

Batch 92 found that deleting a callee's `extern` declaration pushes its r0
argument move to the end of the setup sequence. This round used that
deliberately twice, and established the other direction as well. The direction
is readable straight off the ROM:

| the ROM puts the r0 move | do this |
|---|---|
| **later** than you do | delete the callee's `extern` declaration |
| **earlier** than you do | add one |

`OvlFunc_964_200a52c` was two instructions from matching — `mov r1, r5` and
`mov r0, #0` in the wrong order in the second of two calls. Deleting the
prototype for that callee closed it exactly.

`OvlFunc_966_2008078`, parked since an earlier round, had **no** declaration for
`__Func_8092adc`. Adding one moved `mov r2, #0` into place and took it from three
differing positions to two. What is left there is the genuine interleave the
park was filed under — the ROM slots `mov r0, r6` between `mov r1, #0xc0` and
its `lsl r1, #8`, splitting a shifted constant around another argument.

Two cautions, both measured. It is the **r0 move specifically**; the other
argument registers stay in ascending order either way. And it applies when the
r0 argument is a value in a register — where r0 is a small constant the rotation
has some other cause and the prototype changes nothing, which is why
`OvlFunc_930_2008870` (r0 argument `#0xe`) did not move.

## 52 parks are within six instructions

A sweep screened every parked file that still names a live `.s` — 146 of them —
and counted differing positions. **52 are within six instructions**, and of
those, five are within three:

```
  3 differ  OvlFunc_930_2008870   rom  24   3 differ  SetFlagByte           rom   5
  3 differ  OvlFunc_961_2008120   rom  48   3 differ  OvlFunc_964_2009458   rom  36
  3 differ  OvlFunc_966_2008078   rom  27
```

That changes where the cheap work is. The park corpus is not a graveyard of
hard problems; a third of it is a handful of instructions from done, and the
diffs cluster into a small number of recognisable shapes — argument-move
rotations, `and`/`orr` operand destinations, pool-load ordering, and register
naming. The scan is `scratch/close_parks.py` and takes about ten minutes.

## `SetTextColor`: the `const.sym` route measured, and rejected

That park has said for a long time that its pooled `0xf` must be a symbol,
because gcc never pools what a `mov` can build — and that it was "blocked on
naming". Batch 83's `const.sym` exists for exactly this, so it was worth
actually trying.

A hypothetical `_CONST_F` taken as `(int)&_CONST_F` **does** put the pool load
where the ROM has it, which is the part no literal can do. But it costs a
register: gcc then keeps the mask in r2 and the address in r1 and lands the
`and` in r2, where the ROM reuses r2 for both and lands the `and` in r0.

```
rom        ldr r2, =0xf / and r0, r2 / ldr r2, =0xeae / add r3, r2
_CONST_F   ldr r2, =sym / ldr r1, =0xeae / and r2, r0 / add r3, r1
```

Five spellings of the symbol form all give five differing positions. The plain
literal gives **four**. So the symbol is not simply the missing piece, and
adding `_CONST_F` to `const.sym` would be adding an entry that does not pay for
itself. **No entry was added**, and the park now records the measurement so the
next person does not repeat it.

## A negative that changes how another park should be read

`OvlFunc_901_2008640` is parked at 17 of 47, all of it scheduling inside one
straight-line block. Six statement orders were measured; they decide **which**
high register holds the saved facing and which holds the named zero — that part
is now right — but not where the two moves land relative to the OR block.
Declaration order was permuted three ways with no effect at all.

The useful part is elsewhere. This function is the same cutscene bookend as the
parked `OvlFunc_898_2008acc`, with the same `b .L / <pool> / .L:` shape in its
tail — and **gcc places its pool exactly where the ROM does**. `2008acc` is
parked precisely because its pool lands one instruction early. So that is not a
family-wide placement disagreement to be chased with a compiler flag; it is
specific to `2008acc`, most likely the byte count from function start to the
dump point, since the ROM's point there is 4-aligned and ours is not. Both parks
now cross-reference each other.

## Smaller readings

**`__Random` returns unsigned.** `OvlFunc_944_2008a84` differed in two
instructions of forty-seven, both `lsr` against our `asr`. A right shift is
arithmetic on a signed value and logical on an unsigned one, so the shift names
the return type.

**A `lsl`/`lsr` pair is a 16-bit extract at an offset.** `lsl #2 / lsr #16` is
`(unsigned short)(x >> 14)`; `lsl #13 / lsr #16` is `(unsigned short)(x >> 3)`.
gcc reaches a narrowing cast of a shifted value in two instructions this way
instead of the three a shift-then-mask would take. Read the pair as "sixteen
bits starting at bit `32 - lsl - 16`".

**Fetching an actor twice is the source, not an artifact.** All four `964`
functions repeat `__MapActor_GetActor(n)` once per coordinate. Hoisting it into
a local drops two calls — 46 instructions against 48 — so the duplication is
what the original wrote.

**A value held in a callee-saved register is not evidence it was named.**
`OvlFunc_964_200a52c` keeps 1 and 0xff in r8 and r6 across two calls, which
reads as two function-scope locals. Written that way gcc hoists them above the
first call and the function comes out at 52 against 49. Plain literals at both
sites give 49 and let gcc find the shared registers itself. This is the same
caution batch 92 recorded from the other direction, and it is now the second
time it has cost a spelling.
