# Batch 89 — two ways a two-way choice goes wrong

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, and every
overlay `cmp` clean. Every address below was read back out of the linked ELF.
0 orphaned linker references.

| Function | Address | File |
|---|---|---|
| `OvlFunc_890_2008238` | `02008238` | [ovl_30_c_c_a_c_a.c](../src/overlays/rom_78b2ac/ovl_30_c_c_a_c_a.c) |
| `OvlFunc_890_20082cc` | `020082cc` | [ovl_30_c_c_a_c_a.c](../src/overlays/rom_78b2ac/ovl_30_c_c_a_c_a.c) |
| `OvlFunc_890_2008360` | `02008360` | [ovl_30_c_c_a_c_a.c](../src/overlays/rom_78b2ac/ovl_30_c_c_a_c_a.c) |
| `OvlFunc_890_20083f4` | `020083f4` | [ovl_30_c_c_a_c_a.c](../src/overlays/rom_78b2ac/ovl_30_c_c_a_c_a.c) |
| `OvlFunc_898_2008314` | `02008314` | [ovl_314_a_a_a_a.c](../src/overlays/rom_793768/ovl_314_a_a_a_a.c) |
| `OvlFunc_901_2008400` | `02008400` | [ovl_314_a_a_c_b.c](../src/overlays/rom_797990/ovl_314_a_a_c_b.c) |
| `OvlFunc_949_2008170` | `02008170` | [ovl_30_a_a_c_c_b.c](../src/overlays/rom_7d4af4/ovl_30_a_a_c_c_b.c) |

Both families came out of `find_shape.py --clusters` with nothing solved in
either. Three of the seven cost a screen each; the other four cost nothing once
the first was done.

## A two-way choice of NEARBY constants goes branchless

    __MapActor_GetActor(c ? 0xf : 0xe)

does not compile to a branch. gcc notices the two values differ by one and emits
the "is non-zero" chain:

    ours   neg r0, r3 / orr r0, r3 / lsr r0, #0x1f / add r0, #0xe
    rom    cmp r3, #0 / beq .L0 / mov r0, #0xf / b .L1 / .L0: mov r0, #0xe / .L1:

An `if`/`else` assigning into a variable does exactly the same thing, and so
does swapping the declaration order — three spellings, one result, three
instructions shorter than the ROM every time.

**Put the call inside each arm.**

```c
if (*p & 1) a = __MapActor_GetActor(0xf);
else        a = __MapActor_GetActor(0xe);
```

gcc cross-jumps the call itself and keeps the branch for the argument, which is
the ROM's shape exactly. That took `OvlFunc_898_2008314` from 58 differing lines
to 4.

This is the **mirror** of the `neg / orr / lsr #31` section already in
`elevation.md`: there the idiom is what the ROM has and a statement-level branch
is what produces it. Here the branch is what the ROM has and the obvious C
produces the idiom. Same two shapes, opposite directions — which is worth
knowing before spending a round pushing the wrong way.

## Two initialisers come out in the opposite order to their assignments

The last four lines of that same function:

    rom    mov r2, #0x12 / mov r10, r2 / mov r3, #0x0  / mov r9, r3
    ours   mov r2, #0x0  / mov r9, r2  / mov r3, #0x12 / mov r10, r3

`kind = 0x12; flag = 0;` emits **flag** first. `flag = 0; kind = 0x12;` emits
**kind** first. Declaration order does not reach it — swapping `int kind;
int flag;` changes nothing. Only the assignment order does, and it inverts.

## The scope of a named stack argument is as load-bearing as the name

The four map repaints each pass `2` and `1` on the stack, four times over.
Batch 83 established that naming shared stack arguments keeps two pseudos alive
so gcc builds both before storing either. Here **where** they are declared
decides between three outcomes:

| spelling | result |
|---|---|
| literals | 12 differing — gcc computes and stores each in turn, reusing r3 |
| two locals declared once at the top | 56 lines against 61, 53 differing — gcc keeps them in callee-saved registers across all four calls |
| declared **inside each `if`** | match |

The ROM rebuilds them per block and pushes only `lr`; a function-scope local
tells gcc they are worth keeping, which is the opposite of what the ROM says.

## One park, and two things solved reaching it

`Func_80bf37c` and its two shape-siblings sit at 32 lines against 32 with seven
differing — five of which are label renumbering behind one decision. Both
`return 0` paths share a tail in the ROM; gcc hoists the `mov r0, #0` that feeds
them above the first test instead, which frees r3 for the loaded byte and leaves
the ROM's `mov r3, r2` copy with nothing to correspond to.

Two findings from the way down are recorded in the park because they are
general:

- **The decrement is `v = v + 0xff`, not `v--`.** The value is a byte stored
  back into a byte, and gcc emits `sub r3, #1` for the decrement but
  `add r3, #0xff` for the addition. The ROM has the add. 12 differing to 11.
- **The last test is written positive.** `if (f(...) != 0) { ...; return 1; }
  return 0;` gives the ROM's `beq` to the shared tail; the negative form inverts
  the branch and costs four lines. 11 to 7.

Six other spellings and six flag settings are tabulated there; none moves it.
