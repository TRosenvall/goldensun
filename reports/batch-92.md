# Batch 92 — a missing prototype reorders argument setup, and two claims that did not survive being tested

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `Func_8091660` | `08091660` | main ROM | [rom_91584_a_c_a_c_c_b.c](../src/rom_8a000/rom_91584_a_c_a_c_c_b.c) |
| `CreateParticleActor` | `08096c80` | main ROM | [rom_944ec_c_c_a.c](../src/rom_8a000/rom_944ec_c_c_a.c) |
| `Func_809ade8` | `0809ade8` | main ROM | [rom_9ad70_a_a_a_c_b.c](../src/rom_8a000/rom_9ad70_a_a_a_c_b.c) |
| `Func_80a47b4` | `080a47b4` | main ROM | [rom_a47b4_a_b.c](../src/rom_a1000/rom_a47b4_a_b.c) |
| `Anim_Kite` | `080e6948` | main ROM | [rom_e6638_a_b.c](../src/rom_c9000/rom_e6638_a_b.c) |
| `OvlFunc_957_2008cf8` | `02008cf8` | ovl_7e3e08 | [ovl_30_c_c_a_c_c_c_c_c_c_a_c_b.c](../src/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_a_c_b.c) |

Nothing was parked this round. Three of the six matched on the first screen.

## A rotation in the argument moves means the callee had no prototype

`Func_80a47b4` came down to three instructions, and they were the same three
instructions in a different order:

```
	rom	mov r1, #0 / mov r2, #0 / mov r0, r7
	ours	mov r0, r7 / mov r1, #0 / mov r2, #0
```

With `extern void Func_80a10d0(void *, int, int, int, int, int);` in scope,
gcc-2.96 emits the hard-register argument moves in ascending order and puts r0
first. With **no declaration at all**, the arguments go through the default
promotions and it emits r0 last — which is the ROM.

What makes this worth writing down is everything that did *not* move it: the
declaration-order lever, `void *` parameter types, assigning inside the call
expression, and `-fno-schedule-insns`, `-fno-schedule-insns2`, `-fno-peephole`,
`-fno-defer-pop`, `-fno-caller-saves`. A three-move rotation in front of a call
looks exactly like scheduling noise and is not; it is a statement about the
original translation unit.

Several already-matched files in the tree carry a comment saying one callee is
"intentionally implicit". That was reached by trial in those files. This is the
rule behind it, and it is now in `docs/elevation.md` where it can be reached for
deliberately.

Two other readings were needed to get `Func_80a47b4` that far, both already on
the books and both worth restating because they came up together:

* **The byte offset has to be its own variable.** `ldrh r3, [r6, r5]` is the
  register-offset form, which gcc emits only when the whole offset sits in one
  register. Written inline, `base + 0x178 + idx * 2` associates as
  `(base + idx * 2) + 0x178` and folds the base into the index. Naming
  `off = 0x178 + idx * 2` first keeps them apart.
* **The table entry is read twice, not named.** The ROM has an extra
  `mov r1, r3` that a named local does not produce, because gcc loads a named
  local straight into the argument register. Repeating the subscript in the test
  and in the call, and letting CSE share it, is what puts the value in r3 and
  copies it out.

## The mask width rule, in its cleanest instance yet

`CreateParticleActor` differed from the ROM by **one instruction in forty-two**:

```
	rom	mov r3, #4 ... strb r3 ... strb r3 ... sub r3, #0x11
	ours	mov r3, #4 ... strb r3 ... strb r3 ... mov r3, #0xf3
```

`p->f9 &= ~0xc` and `p->f9 = p->f9 & ~0xc` both build the mask at **byte**
width, because the destination is an `unsigned char` and gcc narrows the whole
expression. Assigning `~0xc` to a named `int` first forces 32 bits — and once
it is 32 bits, `0xfffffff3` is reachable from the `4` already sitting in the
register by a single `sub`, which is what the ROM does.

So the width rule and the derive-from-a-nearby-constant behaviour compose: the
cast is what makes the derivation available. Where the assignment sits is
irrelevant; only the type matters, which was checked both ways.

## Two claims that did not survive being tested

Both of these were written into file comments from reading the assembly, and
both were false. They are recorded as negatives in the files rather than
quietly deleted, because the tempting reading is the wrong one in each case.

**`OvlFunc_957_2008cf8`.** It tests `y == 0x14`, then calls a six-argument
function whose second argument is built with `mov r1, #0x14` while its sixth is
filled with `str r4, [sp, #4]` — r4 still holding `y`. That reads as a clear
statement about which of the three `0x14`s is a live variable. It is not:
passing `y` in the stack slot only, in both, or in neither all compile to the
same thirty-five instructions. Inside the branch gcc knows `y == 0x14` and
decides per operand whether to re-derive or reuse, and the decision is not
reachable from C.

**`Anim_Kite`.** It passes `sp + 0xc` and `sp + 8` as two out-parameters, which
looks like it pins two locals to two frame slots. Swapping their declarations
changes nothing — gcc assigns frame slots by use.

The general form: **a value that is provably constant inside its branch is not
evidence about the source**, however specific the register reuse looks. Contrast
batch 84's stack-arg lever, where the shared value could *not* be
constant-folded and naming it therefore did decide the output. That is the line
between the two.

The working rule that follows, now in `docs/elevation.md`: before writing "the
ROM does X, so the source said Y" into a file, compile the alternative and check
that it differs. A comment claiming a control that was never run is worse than
no comment. When the alternative matches too, say so — the negative is the
useful part.

`Anim_Kite`'s other reading did hold up: the ROM re-reads the slot after
`AnimStart` rather than reusing the pointer it just stored, and holding that
pointer in a local drops the load — 26 instructions against 27, diverging at the
first. gcc has no choice there because the call could have changed it.

## `-fcall-used-r4` ruled out for the message-base class

Batch 91 left `-fcall-used-r4` as the open hypothesis behind the new
message-base-in-a-register park: it is in `GCC296_CFLAGS`, it removes a register
from the callee-saved set, and the park is about gcc declining to spend a
callee-saved register on a constant.

`OvlFunc_962_200806c` was recompiled with the flag removed. It does not change
the decision — gcc uses r4 for the slot instead of r5 and still emits four
independent pool loads. So the difference is not about how many callee-saved
registers are available. Recorded in the park so it is not re-run.
