# Batch 76 — address versus value, and two ways to stop gcc taking the short form

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, and every
overlay `cmp` clean. Every address below was read back out of the linked ELF.
0 orphaned linker references.

| Function | Address | File |
|---|---|---|
| `Func_801c46c` | `0801c46c` | [rom_1aeec_c_a_c_a_a.c](../src/rom_15000/rom_1aeec_c_a_c_a_a.c) |
| `DecompressStatusIcon` | `0801a4c0` | [rom_19ebc_a_c_c_c_c_b.c](../src/rom_15000/rom_19ebc_a_c_c_c_c_b.c) |
| `Field_Frost_Target` | `08099128` | [rom_97b54_a_c_c_a_c_c_b.c](../src/rom_8a000/rom_97b54_a_c_c_a_c_c_b.c) |
| `OvlFunc_911_20080cc` | `020080cc` | [ovl_30_a_c_a_a_c_b.c](../src/overlays/rom_79e5c0/ovl_30_a_c_a_a_c_b.c) |
| `Func_8097868` | `08097868` | [rom_97384_c_a_c_b.c](../src/rom_8a000/rom_97384_c_a_c_b.c) |

## A bug I shipped, and caught, inside one round

Batch 75 closed a hole in `tryc.py`: its inline-pool warning fired only on a
clean match, so a near-match with one cosmetic difference got no warning — and
that had cost a build.

**The fix was worse than the hole.** It used `elif … continue`, which attached
to the wrong `if` and **suppressed the entire mismatch report** for any
reference with an inline pool. A screen would print the warning and nothing
else: no line counts, no diff.

I found it the same round, by screening a function and getting no verdict back.
Rewritten to print the warning *in addition to* the diff, never instead of it,
with a regression check both ways. The comment in `tryc.py` now records what the
first attempt did, because that shape of mistake is easy to repeat.

## Address versus value — where the struct-member lever stops

Two functions in this batch both want gcc to do something twice that it would
rather do once, and only one of them can be made to.

**`Func_801c46c` matched.** It loads the same offset from the literal pool
twice — `ldr r2, =0x205` for the read and `ldr r0, =0x205` for the write, in a
22-instruction function. gcc CSEs that when the offset is written as
arithmetic; as a struct **member** it materialises the address independently at
each access.

**`Func_80b0694` did not.** It needs gcc to re-read a *value* — the ROM loads
the same count halfword twice, once to test against zero and once as a loop
bound. Two reads of one member with nothing between them are the same
expression wherever they are written, and CSE folds them.

**The line is address versus value.** A member can be re-addressed; it cannot be
re-read without something gcc must treat as a possible write, and `volatile`
would be a fakematch.

## Two ways to stop gcc taking the short form

**A named intermediate forces the three-operand add.** Thumb has both
`add rd, rn, rm` and `add rd, rm`, and gcc picks the short form whenever the
destination is also an operand:

```
DMA0_SET((char *)b + i * 4, ...)     ->  add r0, r4        (ours)

off = i * 4;
src = (char *)b + off;               ->  add r0, r4, r0    (the ROM's)
```

**And it does not always work, which is the instructive part.** The same trick
fails on `Func_80b06c0`'s `lsl r3, r1, #4`, because there the shift's source is
dead immediately after, so gcc allocates the named local to the same register
and the short form stays correct. The lever needs the two values to be
simultaneously live — a pointer base and its offset are, a shift's input and
output are not.

**Arithmetic narrows to the width of its store unless a local says otherwise.**
Batch 71 needed an `int` local to stop a *constant* narrowing;
`OvlFunc_911_20080cc` needs one to stop the *arithmetic* narrowing:

```
if (--a->f64 == 0)   ->  ldr r3, =0xffff / add r2, r3 / and r3, r2

t = a->f64 - 1;      ->  sub r3, #1 / strh / lsl r3, #16 / cmp r3, #0
a->f64 = t;
if ((unsigned short)t == 0)
```

Same cause from the other side: do the arithmetic at int width and put the
truncation only where the value is **tested**.

## Small readings that are worth stating

**`DecompressStatusIcon` gives three offsets three different treatments** —
`0x604` from the pool, `0x600` synthesised as `mov #0xc0 / lsl #3`, and `0x602`
derived from it with `add r0, #2`. All three fall out of struct members; writing
any of them as arithmetic forces one chain and loses the other two.

**`Field_Frost_Target`'s facing byte is `signed char`, and the ROM says so** —
`ldrb` followed by `lsl #24 / asr #24` is how gcc widens a signed char where
there is no `ldrsb` immediate form. Its `|= 2` is a plain read-modify-write and
*not* a bitfield: `mov r3, #2 / orr r3, r2` has no mask, because setting one bit
needs none.

**`Func_801c46c`'s decrement is `+ 0xff`.** gcc emits `sub r3, #1` for `v - 1`.
Same length, so this is not gcc being clever — it is what the source said.

## Parks

| Function | Blocker |
|---|---|
| `OvlFunc_common1_16cc` | register birth order, where the competing pseudo is created by loop-invariant hoisting rather than by any statement |
| `Func_80b0694` | needs a *value* re-read, not an address |
| `Func_80c23e8` | gcc merges two `return 1` blocks the ROM keeps separate — three instructions ahead |
| `Func_80a7440` | gcc keeps the result in r0 and needs neither of the ROM's two moves |
| `Func_8079bf8` | argument setup order plus a two-operand subtract |
| `gfree` / `free` | a prologue gcc emits and the ROM does not |

The `gfree`/`free` park is the one to read. The ROM's `gfree` is a leaf
returning with a bare `bx lr`; gcc emits `push {lr}` and `pop {r0} / bx r0` —
three instructions the ROM does not have — even though nothing is called and
`-fcall-used-r4` means r4 needs no saving. `free`, the same shape **without** the
early return, gets no prologue at all. So the conditional return provokes it,
and rewriting the early return as an `if` around the body is byte-identical.

`Func_80c23e8` records a negative worth keeping: the `goto` lever that fixed
`OvlFunc_937_20080e4` is byte-identical there, because it asks for exactly the
merge gcc is already doing. **Guard inversion moves a block; it cannot un-merge
two that compute the same value.**
