# Batch 31 — 6 functions, a Makefile gap, and narrow_constant read backwards

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–30 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean `make clean && make -j8 && make compare`, unassisted — 96 overlays
compared byte-for-byte and `goldensun.gba: OK`. Every address below was read
back from the linked overlay ELF with `nm`, and every path confirmed to exist.

## Read this first: a `.sym` edit does not rebuild `stage1.o`

**This is a build bug in your tree, not in ours, and it is reachable from a
fresh clone by anyone who edits a symbol file.** It is the same family as the
two clean-build bugs fixed in batch 07.

Overlays are linked against the partially-linked `stage1.o` with `-R stage1.o`,
and `stage1.ld` pulls in `wram.sym`, `message.sym`, `file_table.sym` and
`area.sym`. The dependency generator is:

    define elf_deps
    $(1): $(shell grep -o '[A-Za-z0-9/_-]\+\.o' $(addsuffix .ld,$(basename $(1))))
    endef

It greps a linker script for **`.o` names**. A `.sym` is not an object, so the
symbol files a script `INCLUDE`s are never dependencies of the ELF that needs
them. Adding `_MSG_256c = 0x256c;` to `message.sym` therefore left `stage1.o`
stale, and the overlay that referenced the new symbol died with:

    arm-none-eabi-ld: ... undefined reference to `_MSG_256c'

Which reads exactly like a typo in the C. The fix is a second generator beside
the first, which reads the `INCLUDE` lines the same way `elf_deps` reads the
object names:

    define ld_sym_deps
    $(1): $(shell sed -n 's/^INCLUDE "\(.*\)"/\1/p' $(addsuffix .ld,$(basename $(1))))
    endef
    $(foreach elf,$(ELFS),$(eval $(call ld_sym_deps,$(elf))))

Both generators are in the Makefile in this batch. **The failure is silent until
someone adds a symbol**, which is why it survived thirty batches here: every
earlier `.sym` addition happened to land in a round that also ran `make clean`.

## `narrow_constant` runs in BOTH directions

Every previous member of this class had the same shape: the ROM builds a wide
constant and gcc narrows it. `~0xc` is the canonical one — `mov r3, #0xd /
neg r3, r3` in the ROM against gcc's byte immediate `mov r3, #0xf3`.

`OvlFunc_881_200a8a8` is the mirror image. gcc **pools a constant the ROM builds
with a `mov`**:

    rom    mov r3, #0x1
    ours   ldrh r3, .L1        (a halfword pool entry holding 1)

The store is `*(u16 *)p = 1`. gcc narrows the whole expression to sixteen bits
and materialises the literal as a **halfword pool constant** rather than an
eight-bit immediate. Assigning through an `int` local first keeps the value
word-wide until the store, and the `mov` comes back:

    v = 1;  *(unsigned short *)p = v;

`OvlFunc_913_20089fc` needed the identical lever for a **zero** four rounds
later, which is what promotes this from a one-off to a rule:

| | |
|---|---|
| the mechanism | gcc narrows the OPERATION, not the operand |
| symptom, direction A | ROM `ldr rN, =small` where gcc emits `mov` → the operand was a **symbol** |
| symptom, direction B | ROM `mov rN, #small` where gcc emits a **pool load** → the destination is **narrow**, name an `int` |

Direction B matters because **it looks exactly like the pool tell**, which would
send you hunting for a symbol that does not exist.

## The declaration lever reorders argument CONSTRUCTION, not just `r0`

Every use of that lever in batches 1–30 is about where `mov r0` lands, because
that is the case it was first noticed on. `OvlFunc_898_2008f3c` has `r0` first
on **both** sides and still differs — the difference is the order two *shifts*
are emitted in:

    rom    mov r0,#0xcc / mov r1,#0xa0 / lsl r0,#1 / lsl r1,#1 / mov r2,#5
    ours   mov r0,#0xcc / mov r1,#0xa0 / lsl r1,#1 / mov r2,#5 / lsl r0,#1

gcc interleaves the second shift with the third argument and defers the first to
the end. **Declaring the callee puts the whole block in the ROM's order**, and
the same one line fixed the twin `OvlFunc_898_2008f64`.

So try it on any argument-block ordering difference, not only the ones where
`r0` is misplaced.

Naming the two shifted values as locals in the ROM's order does **not** work,
and the reason is the same one that bounds the stack-arg-pair lever: gcc folds
`0xcc << 1` at compile time, so the locals never become live values and
adjacency has nothing to hold on to.

## `OvlFunc_935_2008410` is the reference for the stack-arg-pair class

All three cases of that lever appear in one nine-line function, which makes it
worth reading before attempting another member:

| call | stack pair | what it needs |
|---|---|---|
| 1 | `0x50`, `9` — two different literals | **both named**, assigned immediately before the call, in the order the ROM stores them |
| 2, 3 | `0x11` in the low slot of both | named **once**, used twice — and here the sharing runs **across a call**, which gcc handles unprompted by parking it in `r5` |

Call 3 also passes `0x11` in `r0`, and that one stays a **literal**: the ROM
materialises it fresh with `mov r0, #0x11` rather than copying `r5`, so naming
it would produce a `mov r0, r5` that is not in the ROM.

## Second use of the symbol-plus-offset tell

`OvlFunc_959_200cd50` parks a message id in a callee-saved register and reaches
the next message with `add r0, r5, #2`:

    ldr r5, =0x256c / mov r0, r5 / bl __MessageID
    ... / add r0, r5, #0x2 / bl __Func_801776c

Written with literals, gcc folds `0x256c + 2` at compile time and emits a
**second pool entry** for `0x256e` — one instruction shorter and one pool word
longer than the ROM. gcc never folds an offset into a symbol address that way,
so the base has to be a symbol. `_MSG_256c` is added to `message.sym` next to
`_MSG_25b8`, where this shape was first recorded and already carries the note
*"the ROM holds it in a register and adds 1 and 2, which only happens when the
operand is a symbol address."* Defining it by value emits no bytes and asserts
nothing beyond the value.

## One field is read signed, and the header is left alone

`OvlFunc_913_20089fc` reaches actor `+0x64` — `goalFacing`, typed `u16` in
`include/actor.h`. Both comparisons in the ROM are **signed sixteen-bit**: the
first shifts the live value and the threshold left by 16 and compares those
(`lsl r3,#16 / mov r2,#0xfa / lsl r2,#18 / cmp r3,r2`, where `0xfa << 18` is
`1000 << 16`), and the second reloads with `ldrsh`. An unsigned field produces
neither form.

The overlay therefore accesses it through a `short *` and **the header's type is
not changed**. One overlay reading a shared field signed is evidence, not a
finding, and retyping `goalFacing` would touch every consumer in the tree. Worth
a decision from someone who knows what that field is.

## Functions

| function | address | overlay | note |
|---|---|---|---|
| `OvlFunc_881_200a8a8` | `0x0200a8a8` | rom_77a7c8 | narrow_constant, inverted |
| `OvlFunc_898_2008f3c` | `0x02008f3c` | rom_793768 | declaration lever, argument order |
| `OvlFunc_898_2008f64` | `0x02008f64` | rom_793768 | twin of the above |
| `OvlFunc_935_2008410` | `0x02008410` | rom_7bf5a8 | stack-arg-pair, all three cases |
| `OvlFunc_959_200cd50` | `0x0200cd50` | rom_7e7574 | symbol-plus-offset |
| `OvlFunc_913_20089fc` | `0x020089fc` | rom_7a04ac | narrow_constant inverted; signed `+0x64` |

One symbol added: `_MSG_256c = 0x256c;` in `message.sym`.

## Parked

**`OvlFunc_950_2008898`** (rom_7d5838) — arg-interleave, 16 against 16 with
fourteen identical. Four lever combinations tried, all byte-identical output.
This is the **fourth** data point on what separates a reachable middle-`r0` from
an unreachable one, and the table is in the park note: the one case the lever
reaches has plain `mov`s on both sides of `r0`; the three it does not each have
a shift or a pool load adjacent.

**`OvlFunc_882_200c5b8`** (rom_77dd1c) — 36 against 30, and a blocker class that
has not appeared before: **gcc holds a constant the ROM rematerialises.**

    rom    live across the calls: r6 = source sprite, r5 = ~0xc.
           0xc is rebuilt with `mov r2,#0xc` / `mov r3,#0xc`, once per block.
    ours   live across the calls: source sprite, 0xc AND ~0xc — three values.

That third value costs six instructions and costs them structurally: the tree
builds with `-fcall-used-r4`, so `r4` is call-clobbered and only `r5`/`r6` are
cheap. The third value lands in **`r8`**, a high register, so the prologue grows
`mov r6,r8 / push {r6}`, the epilogue `pop {r3} / mov r8,r3`, and every use
needs a `mov` down into a low register first.

Six variants were tried, including `--no-rerun-cse` on the theory that this is
the post-loop CSE batch 25 turned off for two TUs. None moves the constant.

**This is the exact inverse of the constant-CSE class.** There, gcc hoists a
value and pays a push/pop the ROM does not. Here, gcc hoists a value the ROM
*rebuilds*, and pays the same price. Both want the same missing lever — a way to
tell gcc-2.96 that a cheap constant should be rematerialised rather than kept
live — and if one is ever found, `OvlFunc_882_200c5b8` is the function to test
it on, because everything else about the translation is settled and matches:
mask operand order, the named negative mask, and the per-block re-read of the
source byte.

Not a bitfield, incidentally: a two-bit field copy emits the shift pair that
turns a bit range into a value and back, and the ROM has no shifts at all.

## Counts

287 functions elevated in total. 3,008 hand-written functions remain in `asm/`
of 5,714. 98 parked functions, plus 5 files that document blocker classes rather
than individual functions.
