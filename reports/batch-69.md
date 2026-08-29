# Batch 69 — one flag, six copies of one function, and a blocker the screen cannot see

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, and every
overlay `cmp` clean. Every address below was read back out of the linked ELF
(`goldensun.elf` for the ROM, each overlay's `overlay.elf` for the rest).
0 orphaned linker references.

| Function | Address | File |
|---|---|---|
| `Func_80b606c` | `080b606c` | [rom_b5a0c_c_c_a_a_a_a_b.c](../src/rom_b5000/rom_b5a0c_c_c_a_a_a_a_b.c) |
| `Func_809a44c` | `0809a44c` | [rom_9a44c_a_a_a_b.c](../src/rom_8a000/rom_9a44c_a_a_a_b.c) |
| `OvlFunc_927_2008ab0` | `02008ab0` | [ovl_30_a_a_c_c_c_c_c_b.c](../src/overlays/rom_7b4558/ovl_30_a_a_c_c_c_c_c_b.c) |
| `OvlFunc_946_2008ab0` | `02008ab0` | [ovl_30_a_a_c_c_c_c_b.c](../src/overlays/rom_7ced6c/ovl_30_a_a_c_c_c_c_b.c) |
| `OvlFunc_964_2008ab0` | `02008ab0` | [ovl_30_a_a_a_c_c_c_c_c_b.c](../src/overlays/rom_7ed0a0/ovl_30_a_a_a_c_c_c_c_c_b.c) |
| `OvlFunc_965_2008ab0` | `02008ab0` | [ovl_30_a_a_a_c_c_c_c_c_b.c](../src/overlays/rom_7ef4f4/ovl_30_a_a_a_c_c_c_c_c_b.c) |
| `OvlFunc_968_20080e0` | `020080e0` | [ovl_30_a_a_a_c_a_c_b.c](../src/overlays/rom_7f2f14/ovl_30_a_a_a_c_a_c_b.c) |

Six of the seven are the **same 27-instruction function**, duplicated byte for
byte across the main ROM and five per-area overlays. One flag unlocked all six.

## `-fno-strict-aliasing`

The function ends with `p->t->ang += p->spin` — a **pointer** load two words past
an **int** member it has just stored to. At `-O2` gcc-2.96 enables
`-fstrict-aliasing`, so the two are in different alias sets, the post-reload
scheduler proves the store cannot alias the load, and hoists the load two
instructions earlier to fill a load-use stall:

```
ours   ldr r3, [r0, #0x1c] / ldr r1, [r0, #0x50] / add r3, r2 / str r3, [r0, #0x1c]
rom    ldr r3, [r0, #0x1c] / add r3, r2 / str r3, [r0, #0x1c] / ldr r1, [r0, #0x50]
```

Everything else already matched. With `-fno-strict-aliasing` the scheduler has
to assume they may alias, the order stands, and the function is exact.

### `-fno-schedule-insns2` is the wrong knob, and that is the useful part

The obvious reading of "the scheduler moved an instruction" is to turn the
scheduler off. That makes things **worse**: the same pass is also what produces
the ROM's src-before-dst load order inside each `a += b`. With `-fno-sched2` the
five accumulates all break — the emitted order becomes dst-load-first, and
rewriting them as `p->y = p->dy + p->y;` to compensate fixes the order but
transposes the registers, because the ROM's accumulator lives in the
destination's register. Two spellings and two flag settings were tried before
the alias reading fit.

**The pass is wanted. Only its alias information is not.**

### Per-TU, measured rather than assumed

Applying `-fno-strict-aliasing` to `GCC296_CFLAGS` and rebuilding all **5336**
objects generated from `src/` leaves **2631 bytes** differing across the ROM. Six
TUs want it; most do not. That is what a per-file rule means, and it is the same
form of evidence the `O1_CFLAGS` and `CSE_CFLAGS` groups already carry.

### The seventh copy is not elevated

`OvlFunc_common0_d4` in `asm/overlays/common/common0.s` is byte-identical to the
other six. That object is named by many overlay linker scripts, so splitting it
edits all of them — deliberately left for a round that can verify every overlay
it touches.

## `Func_80b606c` — a buffer nothing reads

Narrows four halfword characters into a stack buffer, substituting `'_'` (0x5f)
for any whose low byte is zero, and NUL-terminates at `buf[4]`. **Nothing reads
the buffer.** It does not escape, it is not returned, and the function ends.
That is in the ROM: gcc-2.96 does not eliminate stores to a stack local at
`-O2`, so the whole body survives.

Two levers:

- **Two pointers, not an index.** The ROM walks the buffer with two separate
  induction variables — one advances immediately after the store, the other at
  the bottom of the loop. Written as `buf[3 - i]` twice, gcc produces a single
  pointer and the loop comes out with its registers permuted.
- **The fill character has to be a variable, assigned first.** As a literal in
  the store, gcc materialises `0x5f` last, after the counter; the ROM
  materialises it second, right after the frame copy. Hoisting it into a local
  ahead of the two pointers puts the whole setup block in the ROM's order and
  settles the register assignment with it. That was the last of eight differing
  lines, after the loop body already matched.

Its return type is `int` with no return statement — see below.

## Two parks, both carrying findings

### `Func_80ad5b4`: literal pool ORDERING, and the screen cannot see it

`tools/tryc.py` reports `OK Func_80ad5b4 (29 lines)`. The build then fails
`make compare` on ten bytes.

```
0x0ad5b8  ldr r3, [pc, #N]   imm 12 (ours) vs 13 (rom)
0x0ad5de  ldr r3, [pc, #N]   imm  4 (ours) vs  3 (rom)
0x0ad5ec  two words of pool, transposed
```

The ROM's pool is `[0xffff8000][iwram_3001f2c]`; gcc emits
`[iwram_3001f2c][0xffff8000]` — reference order, which is what
`add_minipool_forward_ref` (`arm.c:4820`) produces: entries are kept sorted by
`max_address`, and the earlier-referenced fix is the more constrained one.

**Why this had never shown up.** A sweep of every gcc-generated `.s` in the tree
finds **zero** literal pools that mix a symbol with an integer constant. Every
matched TU's pool is all-symbols or all-constants, where ordering cannot be
observed. This is the first mixed pool in the corpus, and it disagrees.

**The screen warned and was right.** `tryc.py` normalises pool loads to
`=value`, so a pool at a different distance still compares equal, and it prints
*"VERIFY WITH make compare — this screen cannot see PC-relative offsets"*.
**Treat an `OK` carrying that warning as provisional.** Nothing in the source
controls pool order; it is decided in `machine_dependent_reorg`, after the insn
stream is final.

The parked C is instruction-exact and worth keeping. Three things were needed
for it: `off` as one reused variable rather than two literals; the OR split into
`v = 0xffff8000; v |= b;` so the constant is the destination (and only then does
the preceding `mov r3, r2` survive instead of being coalesced away); and an
`int` return type with no return statement.

### `GetUnit`: a dead `mov r3, r14`

```
push {lr}
mov  r3, r14      <-- copies the return address into r3, overwritten unread
ldr  r2, =gPartyStatus
```

That is a dead read of an **uninitialised** value. gcc-2.96 emits this shape
when a pseudo is live-in with no reaching definition: the pseudo takes a hard
register from `REG_ALLOC_ORDER` (r14 is sixth, right after r12) and reload
materialises the copy. So the original source read a variable before assigning
it, on a path this reconstruction initialises.

**It is unique.** The corpus has 211 `mov rN, r14` instructions and exactly one
immediately after `push {lr}`. The other 210 are r14 used as ordinary scratch
after being saved, which is normal for this compiler — there is no second
example to generalise from.

It costs more than one instruction: the dead move writes r3, and r3 is where the
ROM then keeps the `0x14c` multiplier, leaving the id undisturbed in r0. Without
it gcc puts the multiplier in r0 and copies the id to r2, and everything after
differs by that permutation. Producing it would mean deliberately reading an
uninitialised variable — a guess at which one, not something the bytes identify.

Two things from it are settled and in the parked source: `k = 0x14c; k *= id;`
gives the ROM's `mul r3, r0` where both `id * 0x14c` and `0x14c * id` give
`mul r0, r3`; and the bias is `id * 0x14c - 0xa600`, not `(id - 0x80) * 0x14c` —
the pooled `0xffff5a00` settles which.

## A new lever: `int` return type, no return statement

Two functions this batch needed it. Declared `void`, the epilogue is
`pop {r0} / bx r0`; the ROM uses r1. gcc will not use r0 as the epilogue scratch
when the return type is non-void, because r0 is live at the return **even with
nothing assigned to it**.

For `Func_80ad5b4` that single detail was the difference between 27 of 29 and an
instruction-exact match.

## A correction to batch 68

Batch 68 recorded that this compiler *"writes r4 without saving it… that is this
compiler's behaviour, not a disassembly artifact"*. The observation is right and
the explanation was wrong: it is **`-fcall-used-r4`**, already present in
`GCC296_CFLAGS` at Makefile line 113, where it is credited to Karathan and noted
as required for the byte match. Both the report and the file comment that
repeated it are amended.
