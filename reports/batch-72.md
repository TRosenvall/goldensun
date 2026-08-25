# Batch 72 — read the state as a struct, and a jump table that was never hand-written

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, and every
overlay `cmp` clean. Every address below was read back out of the linked ELF.
0 orphaned linker references.

| Function | Address | File |
|---|---|---|
| `OvlFunc_922_2009a34` | `02009a34` | [ovl_30_c_a_c_c_c_c_a_c_c_b.c](../src/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_c_a_c_c_b.c) |
| `OvlFunc_886_2008088` | `02008088` | [ovl_30_a_a_c.c](../src/overlays/rom_786f0c/ovl_30_a_a_c.c) |
| `OvlFunc_956_200937c` | `0200937c` | [ovl_30_c_c_c_c_a_b.c](../src/overlays/rom_7e0928/ovl_30_c_c_c_c_a_b.c) |
| `OvlFunc_888_2008070` | `02008070` | [ovl_30_c_c_a_a_a_a_a_a_a.c](../src/overlays/rom_7892c8/ovl_30_c_c_a_a_a_a_a_a_a.c) |
| `Func_8096ab0` | `08096ab0` | [rom_944ec_a_c_c_a_a_a_b.c](../src/rom_8a000/rom_944ec_a_c_c_a_a_a_b.c) |

## Read the state as a struct, not as pointer arithmetic

Thumb `ldrsh` has **no immediate-offset form**, so a signed halfword read always
needs an index register. That gives gcc a choice, and the two spellings pick
differently:

```
gState.area                     ->  add r3, r1, r0 / mov r0, #0 / ldrsh r3, [r3, r0]
*(short *)(base + k + (u32)0)   ->  ldrsh r3, [r6, r2]
```

Written as arithmetic, gcc folds the addition into the load and makes the index
carry the offset — **one instruction shorter than the ROM**. Written as a struct
member, with the offset inside the *type*, gcc has to materialise the address
and supply a zero index, which is what the ROM does.

`OvlFunc_922_2009a34` was parked at 18 of 50 on exactly this, diagnosed as "a
folded address". The diagnosis was right; the fix was in the type, not the
expression.

### What the park got right, and what it overdid

Its note that inlining the zero as `(unsigned int)0` was **byte-identical here**,
while being the exact fix for its sibling `OvlFunc_921_200816c`, was honest and
is now explained: that lever settles *which zero wins* when two compete, and
does nothing about whether an address gets folded. Two notes exist because the
diffs look alike.

It also spent effort forcing gcc to derive `0x1c0` from `0x16c` with an explicit
`k = k + 0x54;`. **Not needed** — gcc emits `add r1, #0x54` by itself from plain
struct members. The same constant-derivation peephole that blocks the bitfield
cases is helping here.

The lever was first-screen for `Func_8096ab0`, `OvlFunc_888_2008070` and
`OvlFunc_937_20080e4`.

## A `switch` reproduces the ROM's jump table

`OvlFunc_888_2008070`'s `.s` carries a 35-entry `.word` table and a
`mov pc, r3`. A plain C `switch` produced it **exactly** — the range check
`sub r3, #1 / cmp r3, #0x22 / bhi`, the `.align 2, 0`, and every entry in order
— with nothing done to provoke it. The case values are dense enough that
gcc-2.96 chooses a tablejump on its own.

Worth recording because a jump table in the `.s` reads like hand-written
assembly and is not.

## A third function on the signed lower-bound floor

`OvlFunc_937_20080e4` is parked at **exactly 2 of 38**:

```
rom    cmp r3, #0x9 / blt
ours   cmp r3, #0x8 / ble
```

`docs/elevation.md` names `OvlFunc_899_2008048` and `Func_80a3ce4` as sitting on
this floor. This is the third and the cleanest, because both of its *other*
differences turned out to be fixable:

- the struct-member read fixed the `ldrsh` shape;
- routing both outcomes through `goto` labels fixed the block placement, taking
  it from 23 of 38 to 2. That is the compound-condition lever applied to the
  **result** rather than to the condition.

So the diagnosis is narrow and confident rather than a shrug: two levers account
for everything except the canonicalisation, which no spelling reaches.

## A union was tried and fails

`OvlFunc_957_2008bc8`'s ROM reads the same halfword twice, once `ldrh` and once
`ldrsh`, and gcc keeps only the signed load. The obvious next idea after "the
cast doesn't separate them" is a union of the two views:

```c
union { unsigned short u; short s; } area;
```

**It does not help.** Two differently-typed members at the same offset are the
same MEM in RTL, so CSE unifies them exactly as it does two casts of one
pointer. Recorded in the park because it is the obvious idea and it is not a
different idea.

Flags probed there too: `-fno-rerun-cse-after-loop` and `-fno-cse-follow-jumps`
are byte-identical; `-fno-gcse` recovers one instruction of the three and still
drops the `ldrh`.

## Two parks that sharpen the argument-precompute rule

**`OvlFunc_922_2009ad0`** — 3 of 30, and **the other six calls in the same
function all match**. The one that fails, `__MapActor_SetSpeed(0, 0xa0 << 10,
0xa0 << 9)`, has two expensive arguments and a cheap one that is not last, which
is exactly what `precompute_register_parameters` predicts. Six matching calls
beside it is better evidence for the rule's scope than the failure alone.

**`OvlFunc_911_200a608`** — 44 of 65 becomes **6 of 65** with
`-fno-rerun-cse-after-loop`, which removes an r7 hoist of a constant used at two
call sites. That is the symptom `CSE_CFLAGS` exists for, and the TU is a
candidate for that group. What is left is the same precompute interleave.

Both parks record the negative probes: `0x620000` written as a literal instead
of `0xc4 << 15` is byte-identical (gcc synthesises it the same way and the
`rtx_cost` is unchanged), and `-fno-gcse` does nothing.

## One more park

**`OvlFunc_893_2008054`** — 28 lines against 30, on three **identical**
arguments. gcc builds the value once and copies it into the other two registers;
the ROM builds each independently. gcc is strictly ahead, which is the signature
of this class, and `-fno-rerun-cse-after-loop` is byte-identical, so the
unification happens earlier than that pass.

What is already right there and should not be re-derived: the ROM's chain of
derived constants for its two stores —

```
mov r3, #0xe0 / lsl r3, #1   (0x1c0, the first offset)
add r3, #0x44                (0x204, the first VALUE)
sub r3, #0x3c                (0x1c8, the second offset)
```

— comes out of plain literals with no help.
