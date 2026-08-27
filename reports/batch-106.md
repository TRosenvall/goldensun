# Batch 106 — the flag and the lever are different tools

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of its overlay's linked ELF with
`arm-none-eabi-nm`.

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_890_2008150` | `02008150` | ovl_78b2ac | [ovl_30_c_c_a_a_c_a.c](../src/overlays/rom_78b2ac/ovl_30_c_c_a_a_c_a.c) |
| `OvlFunc_881_200813c` | `0200813c` | ovl_77a7c8 | [ovl_30_a_a_a_c_b.c](../src/overlays/rom_77a7c8/ovl_30_a_a_a_c_b.c) |
| `OvlFunc_953_200855c` | `0200855c` | ovl_7d95dc | [ovl_30_c_c_c_a_a_a_c_a_c_c.c](../src/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_a_c_a_c_c.c) |
| `OvlFunc_942_20088cc` | `020088cc` | ovl_7c6bac | [ovl_30_c_c_a_c_c_c_c_c_b.c](../src/overlays/rom_7c6bac/ovl_30_c_c_a_c_c_c_c_c_b.c) |
| `OvlFunc_common1_172c` | `0200afc4` / `0200b25c` / `0200bcf4` | shared by ovl_7db0c8, ovl_7ddb88, ovl_7e0928 | [common1_c_a_c_c_a_a.c](../src/overlays/common/common1_c_a_c_c_a_a.c) |

`OvlFunc_common1_172c` is one object linked into three overlays, so it is
verified at three addresses.

**2499 functions remain in assembly** — under 2500 for the first time. 228 are
parked. All five here were fresh from assembly, chosen off
`tools/find_bb_lever.py`.

## The correction: last round over-applied the lever

Batch 105 retired a class with the basic-block lever and that still stands. But
the first function of this round showed the lever being used where it should not
be.

`OvlFunc_890_2008150` written with plain literals is 44 differing of 53, because
gcc CSEs two repeated flag ids into callee-saved registers and the function
grows a `push {r5}` the ROM lacks. The lever fixes it — with **five `int` locals
whose only job is to hold a flag id**. That screens OK and it is not source
anybody wrote.

`-fno-rerun-cse-after-loop` matches the same function **with no change to the C
at all**, and CSE_CFLAGS is an existing per-file group with many members.

So the two mechanisms had to be separated, and they separate cleanly:

* **Constant CSE** — one value used by two calls, held in a register across
  them. The flag fixes it. The lever also does, badly.
* **Argument scheduling** — a two-instruction constant split around another
  argument, or a pool load issued too early. **The flag does not touch it.**

That second claim is measured, not assumed. Re-screening batch 105's three lever
functions with their *literal* spellings under the flag: `OvlFunc_948_2009fd8`
stays at 12 of 97, `OvlFunc_911_2008304` at 2 of 85, `OvlFunc_943_2008a48` at 2
of 57. Batch 105's files are right; this round's first one was not.

**The rule now is: try the flag first and keep the literals; reach for the lever
only for what the flag leaves behind.**

`OvlFunc_942_20088cc` is the clean demonstration that a function can need both,
and that neither reaches the other's instructions:

| | differing of 53 |
|---|---|
| literals, default flags | 46 |
| literals, `-fno-rerun-cse-after-loop` | 3 |
| lever, default flags | 48 |
| lever + `-fno-rerun-cse-after-loop` | **0** |

The flag fixes a flag id read then written inside one block. The lever fixes two
`__MapActor_SetPos` coordinates split around the slot number.

## The complete table of argument orders

`OvlFunc_885_20080dc` sat at 9 of 56 on three calls whose arguments come out in
the wrong order, and nothing moved it. Rather than add a ninth failed spelling to
the park, the compiler was asked directly — four one-line functions compiled
under this tree's exact flags:

| callee | third argument | emitted order |
|---|---|---|
| `void` | cheap constant | r0, r1, r2 |
| `int` | cheap constant | r1, r2, r0 |
| `void` | pool constant, or `mov`+`lsl` | r2, r0, r1 |
| `int` | pool constant, or `mov`+`lsl` | r2, r1, r0 |
| `void` | a global read | `ldr` base, r0, `ldr` r2, r1 |

`precompute_register_parameters` (calls.c:805) copies any argument whose
`rtx_cost` exceeds 2 into a pseudo before any hard register is loaded. That is
the entire difference between the first two rows and the next two.

The ROM wants `mov r2, #0xa / mov r0, #0xc / mov r1, #0` — **row three's order
with row one's cost**. No spelling of a value both clears the threshold at expand
and assembles to a single cheap `mov`, so the function is parked with a reason
rather than a list.

Two things fall out that are worth more than the park:

* **The return-type lever is not broken, it just cannot express this.** The probe
  confirms `int` still moves r0 last. The ROM wants r0 in the **middle**, and
  nothing in the table produces that.
* **A long list of failed spellings is a signal to ask the compiler instead.**
  Batch 105's lesson was "when every failed attempt varies one axis, that is
  evidence about the axis". This is the same lesson one step further on: when
  the axis is exhausted, stop enumerating and go read what the compiler can
  actually emit.

## A low-numbered `.L` symbol cannot use the asm-label extension

`OvlFunc_common1_172c` references a script blob that was called `.L7`.

```c
extern unsigned char L7[] __asm__(".L7");   /* compiles, links, WRONG */
```

gcc numbers its own labels `.L1`, `.L2`, … so a low number collides: gcc emits a
`.L7:` of its own in front of the constant pool, and `.word .L7` points at the
pool instead of the blob.

The tell is nasty. `tryc.py` reports **one** differing line — a stray label at
the end of the stream — which reads exactly like a pool-placement artifact of
comparing generated output against hand-written asm. It was only `make compare`
failing on `overlays/rom_7ddb88` that showed it was real.

The technique's write-up in `docs/elevation.md` recommends the extension over
renaming, and its examples are `.L23f0` and `.L57fc` — high numbers gcc's counter
never reaches. For a low-numbered label the extension is unsafe and the rename is
the only option.

Checking the rename's precondition is not what it looks like:
`grep -rln '^\.L7:' asm/` returns **several hundred files**, because `.L7` is
gcc's per-file local label almost everywhere. What matters is how many reference
it across an object boundary — exactly one, here — so the rename to
`gOvlCommon1_3fe4` was two lines, committed separately with compare green on both
sides.

## `neg` of a constant is the two's complement, so read the bits

`mov r3, #0xd / neg r3, r3` is `~0xc`, **not** `~0xd`. -13 is `0xfffffff3`: bits
2 and 3 cleared, bit 0 left alone.

Read as `~0xd` on `OvlFunc_881_200813c` it looks like three bits being cleared,
which turns one bitfield write into two and costs three instructions. It is a
single two-bit field at offset 2 set to 1, and the `mov r2, #4 / orr` is its
value. Batch 71's rule — a 32-bit `mov`/`neg` pair means a bitfield — is right;
getting the field's width and offset wrong from a misread mask looks exactly like
a codegen difference.

## Two smaller levers, both on `OvlFunc_881_200813c`

**A shared value assigned in two arms goes branchless.** Written as

```c
if (mode & 4) v = 0x14ccc; else v = 0x80 << 9;
a->f18 = v;  a->f1c = v;
```

gcc preloads the pool constant and conditionally overwrites it, and the ROM's
diamond disappears. Duplicating both stores into each arm gives the diamond, and
gcc cross-jumps the tail itself.

**The basic-block lever's symptom can be a whole register.** gcc issued
`ldr r0, =0x11d` before the three `ldr rN, [r0, #k]` filling the other argument
registers, so the incoming parameter had to be copied out of r0 — a `mov r4, r0`
the ROM does not have. `int id = 0x11d;` in a dominating block puts the pool load
last and the parameter stays in r0. The same lever, same shape, closed
`OvlFunc_common1_172c`.
